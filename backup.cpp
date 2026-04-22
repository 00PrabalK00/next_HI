// Copyright 2024 Next Robotics
// Custom Hardware Interface for Kinco Motor Drives via CANopen
// Based on working Python motor_controller code

#include <hardware_interface/system_interface.hpp>
#include <hardware_interface/types/hardware_interface_type_values.hpp>
#include <rclcpp/rclcpp.hpp>
#include <rclcpp_lifecycle/state.hpp>
#include <std_msgs/msg/bool.hpp>
#include <rclcpp/executors.hpp>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cstring>
#include <vector>
#include <string>
#include <cmath>
#include <chrono>
#include <cstdlib>
#include <cerrno>
#include <limits>

namespace next_motor_interface {

class KincoHardwareInterface : public hardware_interface::SystemInterface {
public:
  enum class ControlState {
    INIT,
    ROBOT_SYSTEM_CHECK,
    INIT_MOTOR,
    MOTOR_STATE_CHECK,
    WAIT_STATUSWORD_ENABLED,
    BASE_CONTROL,
    FAULT_DETECTED,
    CLEAR_FAULT,
    ERROR,
    PAUSE
  };

  KincoHardwareInterface() {}

  hardware_interface::CallbackReturn on_init(const hardware_interface::HardwareInfo & info) override {
    if (hardware_interface::SystemInterface::on_init(info) != hardware_interface::CallbackReturn::SUCCESS) {
      return hardware_interface::CallbackReturn::ERROR;
    }
    can_interface_ = info_.hardware_parameters["can_interface_name"];
    RCLCPP_INFO(rclcpp::get_logger("KincoHardwareInterface"), "CAN Interface: %s", can_interface_.c_str());
  hw_positions_.resize(info_.joints.size(), 0.0);
  hw_velocities_.resize(info_.joints.size(), 0.0);
  hw_commands_.resize(info_.joints.size(), 0.0);
    motor_ids_.resize(info_.joints.size());
    statuswords_.resize(info_.joints.size(), 0);
  // Track last sent rpm and times to avoid flooding the CAN bus
  last_sent_rpm_.resize(info_.joints.size(), kNoRpm);
  last_sent_time_.resize(info_.joints.size(), std::chrono::steady_clock::time_point::min());
    for (size_t i = 0; i < info_.joints.size(); i++) {
      motor_ids_[i] = std::stoi(info_.joints[i].parameters.at("motor_id"));
      RCLCPP_INFO(rclcpp::get_logger("KincoHardwareInterface"), "Joint '%s' -> Motor ID: %d", info_.joints[i].name.c_str(), motor_ids_[i]);
    }
    return hardware_interface::CallbackReturn::SUCCESS;
  }

  std::vector<hardware_interface::StateInterface> export_state_interfaces() override {
    std::vector<hardware_interface::StateInterface> state_interfaces;
    for (size_t i = 0; i < info_.joints.size(); i++) {
      state_interfaces.emplace_back(hardware_interface::StateInterface(
        info_.joints[i].name, hardware_interface::HW_IF_POSITION, &hw_positions_[i]));
      state_interfaces.emplace_back(hardware_interface::StateInterface(
        info_.joints[i].name, hardware_interface::HW_IF_VELOCITY, &hw_velocities_[i]));
    }
    return state_interfaces;
  }

  std::vector<hardware_interface::CommandInterface> export_command_interfaces() override {
    std::vector<hardware_interface::CommandInterface> command_interfaces;
    for (size_t i = 0; i < info_.joints.size(); i++) {
      command_interfaces.emplace_back(hardware_interface::CommandInterface(
        info_.joints[i].name, hardware_interface::HW_IF_VELOCITY, &hw_commands_[i]));
    }
    return command_interfaces;
  }

  hardware_interface::CallbackReturn on_configure(const rclcpp_lifecycle::State & /*previous_state*/) override {
    RCLCPP_INFO(rclcpp::get_logger("KincoHardwareInterface"), "Configuring CAN interface...");
    can_socket_ = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if (can_socket_ < 0) {
      RCLCPP_ERROR(rclcpp::get_logger("KincoHardwareInterface"), "Failed to create CAN socket");
      return hardware_interface::CallbackReturn::ERROR;
    }
    // Bind the socket to the CAN interface
    struct ifreq ifr;
    strncpy(ifr.ifr_name, can_interface_.c_str(), IFNAMSIZ - 1);
    ifr.ifr_name[IFNAMSIZ - 1] = '\0';
    if (ioctl(can_socket_, SIOCGIFINDEX, &ifr) < 0) {
      RCLCPP_ERROR(rclcpp::get_logger("KincoHardwareInterface"), "CAN ioctl failed");
      return hardware_interface::CallbackReturn::ERROR;
    }
    struct sockaddr_can addr = {};
    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;
    if (bind(can_socket_, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
      RCLCPP_ERROR(rclcpp::get_logger("KincoHardwareInterface"), "CAN bind failed");
      return hardware_interface::CallbackReturn::ERROR;
    }
    // Increase receive buffer to reduce kernel controller overrun
    int rcvbuf = 4 * 1024 * 1024; // 4MB
    if (setsockopt(can_socket_, SOL_SOCKET, SO_RCVBUF, &rcvbuf, sizeof(rcvbuf)) < 0) {
      RCLCPP_WARN(rclcpp::get_logger("KincoHardwareInterface"), "Failed to increase CAN SO_RCVBUF");
    }
    // Install CAN filters to accept only relevant TPDOs and SDO responses for our motors
    std::vector<struct can_filter> filters;
    for (auto id : motor_ids_) {
      struct can_filter f1; f1.can_id = 0x180 + id; f1.can_mask = CAN_SFF_MASK; filters.push_back(f1);
      struct can_filter f2; f2.can_id = 0x580 + id; f2.can_mask = CAN_SFF_MASK; filters.push_back(f2);
      // optionally accept bootup/heartbeat (0x700 + id)
      struct can_filter f3; f3.can_id = 0x700 + id; f3.can_mask = CAN_SFF_MASK; filters.push_back(f3);
    }
    if (!filters.empty()) {
      if (setsockopt(can_socket_, SOL_CAN_RAW, CAN_RAW_FILTER, filters.data(), filters.size() * sizeof(struct can_filter)) < 0) {
        RCLCPP_WARN(rclcpp::get_logger("KincoHardwareInterface"), "Failed to set CAN filters");
      }
    }
    // Only NMT reset/start here, all other init in state machine
    nmt_service(0x00, 0x81); // Reset all nodes
    usleep(200000);
    nmt_service(0x00, 0x01); // Start all nodes
    usleep(100000);
    return hardware_interface::CallbackReturn::SUCCESS;
  }

  hardware_interface::CallbackReturn on_activate(const rclcpp_lifecycle::State & /*previous_state*/) override {
    RCLCPP_INFO(rclcpp::get_logger("KincoHardwareInterface"), "Activating Kinco motors...");
    // All drive enabling logic is now in the state machine
    return hardware_interface::CallbackReturn::SUCCESS;
  }

  hardware_interface::CallbackReturn on_deactivate(const rclcpp_lifecycle::State & /*previous_state*/) override {
    RCLCPP_INFO(rclcpp::get_logger("KincoHardwareInterface"), "Deactivating motors...");
    for (size_t i = 0; i < motor_ids_.size(); i++) {
      sdo_motor_spin(motor_ids_[i], 0.0);
    }
    return hardware_interface::CallbackReturn::SUCCESS;
  }

  hardware_interface::CallbackReturn on_cleanup(const rclcpp_lifecycle::State & /*previous_state*/) override {
    if (can_socket_ >= 0) {
      close(can_socket_);
      can_socket_ = -1;
    }
    return hardware_interface::CallbackReturn::SUCCESS;
  }

  hardware_interface::return_type write(const rclcpp::Time & /*time*/, const rclcpp::Duration & /*period*/) override {
    // Gate commands: only send velocity when in BASE_CONTROL (fully enabled)
    if (current_state_ != ControlState::BASE_CONTROL) {
      // Optionally send 0 rpm during init, or just skip
      return hardware_interface::return_type::OK;
    }
    
    for (size_t i = 0; i < motor_ids_.size(); i++) {
      double rpm = -(hw_commands_[i] * 60.0) / (2.0 * M_PI);
      rpm *= 20.0;
      if (motor_ids_[i] == 1) {
        rpm = -rpm;
      }
      sdo_motor_spin(motor_ids_[i], rpm);
    }
    return hardware_interface::return_type::OK;
  }

  hardware_interface::return_type read(const rclcpp::Time & /*time*/, const rclcpp::Duration & period) override {
    struct can_frame frame;
    int max_reads = 20;
    int count = 0;
    while (count < max_reads) {
      int nbytes = recv(can_socket_, &frame, sizeof(struct can_frame), MSG_DONTWAIT);
      if (nbytes <= 0) break;
      
      // Process TPDO messages (0x180 + motor_id)
      uint32_t cob_id = frame.can_id & 0x7FF;
      if (cob_id >= 0x181 && cob_id <= 0x1FF) {
        uint8_t tpdo_motor_id = cob_id - 0x180;
        
        // Find which array index corresponds to this motor_id
        for (size_t i = 0; i < motor_ids_.size(); i++) {
          if (motor_ids_[i] == static_cast<int>(tpdo_motor_id)) {
            // Extract velocity from bytes 0-3 as 32-bit signed integer (little-endian)
            int32_t velocity_raw = static_cast<int32_t>(
              static_cast<uint32_t>(frame.data[0]) |
              (static_cast<uint32_t>(frame.data[1]) << 8) |
              (static_cast<uint32_t>(frame.data[2]) << 16) |
              (static_cast<uint32_t>(frame.data[3]) << 24)
            );
            // Extract statusword from bytes 4-5
            uint16_t sw = static_cast<uint16_t>(frame.data[4]) | (static_cast<uint16_t>(frame.data[5]) << 8);
            statuswords_[i] = sw;
            
            // Convert velocity to rad/s (matching Python logic)
            // rpm = (velocity_raw * 1875) / (512 * 2500 * 4) / gear_ratio
            double rpm = (velocity_raw * 1875.0) / (512.0 * 2500.0 * 4.0) / 20.0;
            // Convert RPM to rad/s
            hw_velocities_[i] = -(rpm * 2.0 * M_PI) / 60.0;
            if (motor_ids_[i] == 1) {
              hw_velocities_[i] = -hw_velocities_[i];
            }
            break;
          }
        }
      }
      count++;
    }
    control_state_machine();
    
    // Update positions using actual measured velocities and the actual period
    double dt = period.seconds();
    if (dt <= 0.0 || dt > 1.0) {
      dt = 0.02; // Fallback to 50Hz if period is invalid
    }
    
    for (size_t i = 0; i < hw_velocities_.size(); i++) {
      hw_positions_[i] += hw_velocities_[i] * dt;
    }
    return hardware_interface::return_type::OK;
  }

  // --- State Machine ---
  ControlState current_state_ = ControlState::INIT;
  size_t state_step_timer_ = 0;
  std::chrono::steady_clock::time_point next_state_action_time_;
  int enable_sub_step_ = 0;

  // CiA-402 statusword checks (proper bit masking)
  static bool is_operation_enabled(uint16_t sw) {
    return (sw & 0x006F) == 0x0027;
  }
  static bool is_switched_on(uint16_t sw) {
    return (sw & 0x006F) == 0x0023;
  }
  static bool is_ready_to_switch_on(uint16_t sw) {
    return (sw & 0x004F) == 0x0021;
  }
  static bool has_fault(uint16_t sw) {
    return (sw & 0x0008) != 0;
  }

  void control_state_machine() {
    switch (current_state_) {
      case ControlState::INIT:
        RCLCPP_INFO(rclcpp::get_logger("KincoHardwareInterface"), "STATE: INIT - System Starting...");
        current_state_ = ControlState::ROBOT_SYSTEM_CHECK;
        break;
      case ControlState::ROBOT_SYSTEM_CHECK:
        current_state_ = ControlState::INIT_MOTOR;
        break;
      case ControlState::INIT_MOTOR:
        if (enable_sub_step_ == 0) {
          RCLCPP_INFO(rclcpp::get_logger("KincoHardwareInterface"), "STATE: INIT_MOTOR - Setting drive mode and enabling");
          enable_sub_step_++;
          next_state_action_time_ = std::chrono::steady_clock::now();
        }
        // Non-blocking time-based progression
        if (std::chrono::steady_clock::now() < next_state_action_time_) {
          return; // Wait
        }
        
        if (enable_sub_step_ == 1) {
          // Set mode of operation to Profile Velocity (0x03) for all motors
          for (auto motor_id : this->motor_ids_) {
            RCLCPP_INFO(rclcpp::get_logger("KincoHardwareInterface"), "Setting Profile Velocity mode for Motor %d", motor_id);
            uint8_t cmd[] = {0x2F, 0x60, 0x60, 0x00, 0x03, 0x00, 0x00, 0x00};
            this->send_sdo(motor_id, cmd);
          }
          next_state_action_time_ = std::chrono::steady_clock::now() + std::chrono::milliseconds(100);
          enable_sub_step_++;
        } else if (enable_sub_step_ >= 2 && enable_sub_step_ < static_cast<int>(2 + motor_ids_.size() * 3)) {
          // Enable motors one at a time: shutdown (0x06), switch on (0x07), enable op (0x0F)
          int motor_index = (enable_sub_step_ - 2) / 3;
          int cmd_step = (enable_sub_step_ - 2) % 3;
          
          if (motor_index < static_cast<int>(motor_ids_.size())) {
            auto motor_id = motor_ids_[motor_index];
            if (cmd_step == 0) {
              RCLCPP_INFO(rclcpp::get_logger("KincoHardwareInterface"), "Motor %d: Shutdown (0x06)", motor_id);
              this->motor_switched(motor_id, 0x06);
              next_state_action_time_ = std::chrono::steady_clock::now() + std::chrono::milliseconds(100);
            } else if (cmd_step == 1) {
              RCLCPP_INFO(rclcpp::get_logger("KincoHardwareInterface"), "Motor %d: Switch On (0x07)", motor_id);
              this->motor_switched(motor_id, 0x07);
              next_state_action_time_ = std::chrono::steady_clock::now() + std::chrono::milliseconds(100);
            } else if (cmd_step == 2) {
              RCLCPP_INFO(rclcpp::get_logger("KincoHardwareInterface"), "Motor %d: Enable Operation (0x0F)", motor_id);
              this->motor_switched(motor_id, 0x0F);
              next_state_action_time_ = std::chrono::steady_clock::now() + std::chrono::milliseconds(200);
            }
            enable_sub_step_++;
          }
        } else {
          // All motors commanded, move to state check
          enable_sub_step_ = 0;
          current_state_ = ControlState::MOTOR_STATE_CHECK;
        }
        break;
      case ControlState::MOTOR_STATE_CHECK:
        RCLCPP_INFO(rclcpp::get_logger("KincoHardwareInterface"), "STATE: MOTOR_STATE_CHECK - Verifying motor status");
        for (size_t i = 0; i < this->statuswords_.size(); ++i) {
          RCLCPP_INFO(rclcpp::get_logger("KincoHardwareInterface"), "Motor %d statusword=0x%04x (fault=%s, enabled=%s)", 
                      motor_ids_[i], this->statuswords_[i],
                      (this->statuswords_[i] & (1<<3)) ? "YES" : "NO",
                      (this->statuswords_[i] & (1<<2)) ? "YES" : "NO");
          
          // Read diagnostic SDOs to understand drive state
          uint16_t controlword = 0, error_code = 0, error_reg = 0, digital_inputs = 0;
          if (sdo_read_u16(motor_ids_[i], 0x6040, 0x00, controlword)) {
            RCLCPP_INFO(rclcpp::get_logger("KincoHardwareInterface"), "  Motor %d controlword=0x%04x", motor_ids_[i], controlword);
          }
          if (sdo_read_u16(motor_ids_[i], 0x603F, 0x00, error_code)) {
            RCLCPP_INFO(rclcpp::get_logger("KincoHardwareInterface"), "  Motor %d error_code=0x%04x", motor_ids_[i], error_code);
          }
          if (sdo_read_u16(motor_ids_[i], 0x1001, 0x00, error_reg)) {
            RCLCPP_INFO(rclcpp::get_logger("KincoHardwareInterface"), "  Motor %d error_register=0x%04x", motor_ids_[i], error_reg);
          }
          if (sdo_read_u16(motor_ids_[i], 0x60FD, 0x00, digital_inputs)) {
            RCLCPP_INFO(rclcpp::get_logger("KincoHardwareInterface"), "  Motor %d digital_inputs=0x%04x (STO/Enable/QuickStop)", motor_ids_[i], digital_inputs);
          }
          
          if (this->statuswords_[i] & (1<<3)) {
            RCLCPP_ERROR(rclcpp::get_logger("KincoHardwareInterface"), "Motor %d has FAULT bit set!", motor_ids_[i]);
            current_state_ = ControlState::FAULT_DETECTED;
            return;
          }
        }
        current_state_ = ControlState::WAIT_STATUSWORD_ENABLED;
        state_step_timer_ = 0;
        break;
      case ControlState::WAIT_STATUSWORD_ENABLED:
        state_step_timer_++;
        for (size_t i = 0; i < this->statuswords_.size(); ++i) {
          if (has_fault(this->statuswords_[i])) {
            RCLCPP_ERROR(rclcpp::get_logger("KincoHardwareInterface"), "Motor %d FAULT detected (statusword=0x%04x)", motor_ids_[i], this->statuswords_[i]);
            current_state_ = ControlState::FAULT_DETECTED;
            return;
          }
        }
        {
          bool all_enabled = true;
          for (size_t i = 0; i < this->statuswords_.size(); ++i) {
            if (!is_operation_enabled(this->statuswords_[i])) {
              all_enabled = false;
              if (state_step_timer_ % 50 == 0) {
                // Decode statusword using proper CiA-402 states
                uint16_t sw = this->statuswords_[i];
                RCLCPP_WARN(rclcpp::get_logger("KincoHardwareInterface"), 
                  "Motor %d NOT operation_enabled (sw=0x%04x): ready_to_switch=%d, switched_on=%d, op_enabled=%d",
                  motor_ids_[i], sw, 
                  is_ready_to_switch_on(sw), is_switched_on(sw), is_operation_enabled(sw));
              }
            }
          }
          if (all_enabled) {
            RCLCPP_INFO(rclcpp::get_logger("KincoHardwareInterface"), "All motors ENABLED! Entering BASE_CONTROL");
            current_state_ = ControlState::BASE_CONTROL;
            return;
          }
        }
        // Periodically re-send enable sequence if not yet enabled
        if (state_step_timer_ % 50 == 0) {  // Every ~1 second
          RCLCPP_INFO(rclcpp::get_logger("KincoHardwareInterface"), "Re-attempting enable sequence...");
          for (size_t i = 0; i < this->motor_ids_.size(); ++i) {
            auto motor_id = this->motor_ids_[i];
            // Check if this specific motor needs enabling (using proper CiA-402 check)
            if (!is_operation_enabled(this->statuswords_[i])) {
              RCLCPP_WARN(rclcpp::get_logger("KincoHardwareInterface"), "Re-enabling Motor %d (statusword=0x%04x)", motor_id, this->statuswords_[i]);
              // Send sequence without blocking
              this->motor_switched(motor_id, 0x06);
              this->motor_switched(motor_id, 0x07);
              this->motor_switched(motor_id, 0x0F);
            }
          }
        }
        // Timeout and give up if motors don't enable after reasonable time
        if (state_step_timer_ > 250) {  // ~5 seconds at 50Hz
          RCLCPP_ERROR(rclcpp::get_logger("KincoHardwareInterface"), "TIMEOUT waiting for motors to enable. Check motor connections and statuswords.");
          for (size_t i = 0; i < this->motor_ids_.size(); ++i) {
            RCLCPP_ERROR(rclcpp::get_logger("KincoHardwareInterface"), "  Motor %d: statusword=0x%04x (enabled=%s)", 
                        motor_ids_[i], this->statuswords_[i], 
                        is_operation_enabled(this->statuswords_[i]) ? "YES" : "NO");
            
            // Full diagnostic dump for failed motors
            uint16_t controlword = 0, error_code = 0, error_reg = 0, digital_inputs = 0;
            if (sdo_read_u16(motor_ids_[i], 0x6040, 0x00, controlword)) {
              RCLCPP_ERROR(rclcpp::get_logger("KincoHardwareInterface"), "    controlword=0x%04x (should be 0x0F for enable)", controlword);
            }
            if (sdo_read_u16(motor_ids_[i], 0x603F, 0x00, error_code)) {
              RCLCPP_ERROR(rclcpp::get_logger("KincoHardwareInterface"), "    error_code=0x%04x", error_code);
            }
            if (sdo_read_u16(motor_ids_[i], 0x1001, 0x00, error_reg)) {
              RCLCPP_ERROR(rclcpp::get_logger("KincoHardwareInterface"), "    error_register=0x%04x", error_reg);
            }
            if (sdo_read_u16(motor_ids_[i], 0x60FD, 0x00, digital_inputs)) {
              RCLCPP_ERROR(rclcpp::get_logger("KincoHardwareInterface"), "    digital_inputs=0x%04x (check STO/Enable DI)", digital_inputs);
            }
          }
          current_state_ = ControlState::ERROR;
        }
        break;
      case ControlState::BASE_CONTROL:
        // Ensure drive stays enabled
        for (size_t i = 0; i < this->statuswords_.size(); ++i) {
          if (!(this->statuswords_[i] & (1<<2))) {
            // Not enabled, try to re-enable
            for (auto motor_id : this->motor_ids_) {
              this->motor_switched(motor_id, 0x06);
              usleep(20000);
              this->motor_switched(motor_id, 0x07);
              usleep(20000);
              this->motor_switched(motor_id, 0x0F);
              usleep(20000);
            }
            current_state_ = ControlState::WAIT_STATUSWORD_ENABLED;
            state_step_timer_ = 0;
            return;
          }
        }
        break;
      case ControlState::FAULT_DETECTED:
        RCLCPP_ERROR(rclcpp::get_logger("KincoHardwareInterface"), "FAULT DETECTED! Clearing fault.");
        current_state_ = ControlState::CLEAR_FAULT;
        state_step_timer_ = 0;
        break;
      case ControlState::CLEAR_FAULT:
        state_step_timer_++;
        if (state_step_timer_ == 1) {
          for (auto motor_id : this->motor_ids_) {
            this->reset_fault_seq(motor_id);
          }
        }
        if (state_step_timer_ > 100) {
          // After fault reset, re-init drive
          current_state_ = ControlState::INIT_MOTOR;
        }
        break;
      case ControlState::ERROR:
      case ControlState::PAUSE:
        break;
    }
  }

  // --- CAN/SDO/Utility Methods ---
  void send_can(uint32_t arbitration_id, const uint8_t* data, size_t len) {
    struct can_frame frame;
    frame.can_id = arbitration_id;
    frame.can_dlc = len;
    std::memcpy(frame.data, data, len);
    ssize_t nbytes = ::write(can_socket_, &frame, sizeof(struct can_frame));
    if (nbytes < 0) {
      int err = errno;
      double now = static_cast<double>(std::chrono::duration_cast<std::chrono::microseconds>(
                    std::chrono::high_resolution_clock::now().time_since_epoch()).count()) / 1e6;
      // Throttle frequent ENOBUFS (no buffer space) messages to once per second
      if (err == ENOBUFS) {
        if ((now - last_send_error_log_time_) > 1.0) {
          RCLCPP_WARN(rclcpp::get_logger("KincoHardwareInterface"), "CAN send ENOBUFS: id=0x%03x", arbitration_id);
          last_send_error_log_time_ = now;
        }
      } else {
        RCLCPP_ERROR(rclcpp::get_logger("KincoHardwareInterface"), "CAN send failed: id=0x%03x, errno=%d", arbitration_id, err);
      }
    }
  }

  void nmt_service(uint8_t motor_id, uint8_t cmd) {
    uint8_t data[] = {cmd, motor_id};
    send_can(0x000, data, 2);
  }

  void motor_switched(uint8_t motor_id, uint8_t cmd) {
    uint8_t data[] = {0x2B, 0x40, 0x60, 0x00, cmd, 0x00, 0x00, 0x00};
    send_can(0x600 + motor_id, data, 8);
  }

  void send_sdo(uint8_t motor_id, const uint8_t* data) {
    send_can(0x600 + motor_id, data, 8);
  }

  bool sdo_read_u16(uint8_t motor_id, uint16_t index, uint8_t subindex, uint16_t & out_val, double timeout_s = 0.2) {
    uint8_t req[8] = {0x40, static_cast<uint8_t>(index & 0xFF), static_cast<uint8_t>((index >> 8) & 0xFF), subindex, 0, 0, 0, 0};
    send_sdo(motor_id, req);
    struct can_frame frame;
    double start = static_cast<double>(std::chrono::duration_cast<std::chrono::microseconds>(
                  std::chrono::high_resolution_clock::now().time_since_epoch()).count()) / 1e6;
    while (true) {
      ssize_t n = ::read(can_socket_, &frame, sizeof(frame));
      if (n > 0) {
        if ((frame.can_id & 0x7FF) == static_cast<uint32_t>(0x580 + motor_id)) {
          out_val = static_cast<uint16_t>(frame.data[4]) | (static_cast<uint16_t>(frame.data[5]) << 8);
          return true;
        }
      }
      double now = static_cast<double>(std::chrono::duration_cast<std::chrono::microseconds>(
                   std::chrono::high_resolution_clock::now().time_since_epoch()).count()) / 1e6;
      if ((now - start) > timeout_s) break;
    }
    RCLCPP_WARN(rclcpp::get_logger("KincoHardwareInterface"), "SDO read timeout for id=%u index=0x%04x", motor_id, index);
    return false;
  }

  void sdo_write_u16(uint8_t motor_id, uint16_t index, uint8_t subindex, uint16_t value) {
    uint8_t data[8] = {0x2B, static_cast<uint8_t>(index & 0xFF), static_cast<uint8_t>((index >> 8) & 0xFF), subindex,
                      static_cast<uint8_t>(value & 0xFF), static_cast<uint8_t>((value >> 8) & 0xFF), 0x00, 0x00};
    send_sdo(motor_id, data);
  }

  void reset_fault_seq(uint8_t motor_id) {
    RCLCPP_WARN(rclcpp::get_logger("KincoHardwareInterface"), "Attempting RESET FAULT for motor %u", motor_id);
    uint8_t msg1[8] = {0x2B, 0x40, 0x60, 0x00, 0x80, 0x00, 0x00, 0x00};
    send_sdo(motor_id, msg1);
    usleep(100000);
    uint8_t msg2[8] = {0x2B, 0x40, 0x60, 0x00, 0x06, 0x00, 0x00, 0x00};
    send_sdo(motor_id, msg2);
    usleep(100000);
    uint8_t msg3[8] = {0x2B, 0x10, 0x20, 0x06, 0x02, 0x00, 0x00, 0x00};
    send_sdo(motor_id, msg3);
    usleep(100000);
    uint16_t cur = 0;
    if (sdo_read_u16(motor_id, 0x2010, 0x01, cur)) {
      sdo_write_u16(motor_id, 0x2010, 0x01, static_cast<uint16_t>(cur | (1<<3)));
      usleep(50000);
      sdo_write_u16(motor_id, 0x2010, 0x01, cur);
    }
    RCLCPP_INFO(rclcpp::get_logger("KincoHardwareInterface"), "Reset sequence completed for %u", motor_id);
  }

  void sdo_motor_spin(uint8_t motor_id, double target_rpm) {
    // Throttle identical sends to reduce CAN bus load
    // Find the index for this motor_id
    int idx = -1;
    for (size_t i = 0; i < motor_ids_.size(); ++i) {
      if (motor_ids_[i] == static_cast<int>(motor_id)) { 
        idx = static_cast<int>(i); 
        break; 
      }
    }
    if (idx < 0) {
      RCLCPP_ERROR(rclcpp::get_logger("KincoHardwareInterface"), "sdo_motor_spin: motor_id %d not found", motor_id);
      return;
    }
    
    int32_t objects_dec = static_cast<int32_t>((target_rpm * 512.0 * 2500.0 * 4.0) / 1875.0);
    // If rpm unchanged and last send was recent, skip
    auto now = std::chrono::steady_clock::now();
    if (last_sent_rpm_[idx] == objects_dec) {
      auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - last_sent_time_[idx]).count();
      if (elapsed < 100) return; // send at most 10Hz per motor for identical values
    }
    last_sent_rpm_[idx] = objects_dec;
    last_sent_time_[idx] = now;

    uint8_t data[4];
    data[0] = static_cast<uint8_t>(objects_dec & 0xFF);
    data[1] = static_cast<uint8_t>((objects_dec >> 8) & 0xFF);
    data[2] = static_cast<uint8_t>((objects_dec >> 16) & 0xFF);
    data[3] = static_cast<uint8_t>((objects_dec >> 24) & 0xFF);
    send_can(0x170 + motor_id, data, 4);
  }

  // --- Member Variables ---
  std::string can_interface_;
  int can_socket_ = -1;
  std::vector<int> motor_ids_;
  std::vector<double> hw_positions_;
  std::vector<double> hw_velocities_;
  std::vector<double> hw_commands_;
  std::vector<uint16_t> statuswords_;
  // Throttling helpers to avoid flooding the CAN bus
  static constexpr int32_t kNoRpm = std::numeric_limits<int32_t>::min();
  std::vector<int32_t> last_sent_rpm_;
  std::vector<std::chrono::steady_clock::time_point> last_sent_time_;
  double last_send_error_log_time_ = 0.0;
};

} // namespace next_motor_interface

#include "pluginlib/class_list_macros.hpp"
PLUGINLIB_EXPORT_CLASS(next_motor_interface::KincoHardwareInterface, hardware_interface::SystemInterface)
