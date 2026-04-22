#include <rclcpp/rclcpp.hpp>
#include <std_msgs/msg/bool.hpp>
#include <std_msgs/msg/int32.hpp>
#include <std_msgs/msg/u_int16.hpp>

#include <linux/can.h>
#include <linux/can/raw.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>

#include <chrono>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <string>
#include <vector>
#include <algorithm>
#include <cerrno>

using namespace std::chrono_literals;

class KincoLiftNodeStd : public rclcpp::Node {
public:
  KincoLiftNodeStd() : Node("kinco_lift_node3_std") {
    can_interface_ = declare_parameter<std::string>("can_interface", "can0");
    node_id_ = declare_parameter<int>("node_id", 3);

    allowed_topic_ = declare_parameter<std::string>("allowed_master_topic", "/allowed_master_on");
    cmd_rpm_topic_ = declare_parameter<std::string>("cmd_rpm_topic", "/lift/cmd_rpm");

    state_rpm_topic_ = declare_parameter<std::string>("state_rpm_topic", "/lift/state_rpm");
    statusword_topic_ = declare_parameter<std::string>("statusword_topic", "/lift/statusword");
    digital_inputs_topic_ = declare_parameter<std::string>("digital_inputs_topic", "/lift/digital_inputs");
    enabled_topic_ = declare_parameter<std::string>("enabled_topic", "/lift/enabled");
    lift_status_topic_ = declare_parameter<std::string>("lift_status_topic", "/lift/status");

    max_rpm_ = declare_parameter<int>("max_rpm", 2500);
    send_period_ms_ = declare_parameter<int>("send_period_ms", 20);

    rpdo_cobid_ = declare_parameter<int>("rpdo_cobid", 0x170 + node_id_);
    tpdo_cobid_ = declare_parameter<int>("tpdo_cobid", 0x180 + node_id_);

    rpm_to_obj_k_ = declare_parameter<double>("rpm_to_obj_k", (512.0 * 2500.0 * 4.0) / 1875.0);
    cmd_sign_ = declare_parameter<int>("cmd_sign", 1);

    send_nmt_reset_all_ = declare_parameter<bool>("send_nmt_reset_all", false);
    send_nmt_start_all_ = declare_parameter<bool>("send_nmt_start_all", true);

    enable_di_poll_ = declare_parameter<bool>("enable_di_poll", true);
    di_poll_hz_ = declare_parameter<double>("di_poll_hz", 10.0);

    enable_limit_guard_ = declare_parameter<bool>("enable_limit_guard", false);
    limit_low_bit_ = declare_parameter<int>("limit_low_bit", 0);
    limit_high_bit_ = declare_parameter<int>("limit_high_bit", 1);
    limit_low_active_high_ = declare_parameter<bool>("limit_low_active_high", true);
    limit_high_active_high_ = declare_parameter<bool>("limit_high_active_high", true);
    motion_start_rpm_ = declare_parameter<double>("motion_start_rpm", 30.0);
    motion_stop_rpm_ = declare_parameter<double>("motion_stop_rpm", 10.0);
    stale_tpdo_stop_s_ = declare_parameter<double>("stale_tpdo_stop_s", 0.3);
    if (motion_stop_rpm_ > motion_start_rpm_) {
      RCLCPP_WARN(get_logger(),
        "motion_stop_rpm (%.2f) > motion_start_rpm (%.2f), clamping stop=start",
        motion_stop_rpm_, motion_start_rpm_);
      motion_stop_rpm_ = motion_start_rpm_;
    }

    if (!open_can()) {
      throw std::runtime_error("Failed to open CAN interface");
    }

    allowed_sub_ = create_subscription<std_msgs::msg::Bool>(
      allowed_topic_, 10,
      [&](const std_msgs::msg::Bool & msg) {
        allowed_master_on_ = msg.data;
        if (!allowed_master_on_) requested_rpm_ = 0;
      });

    cmd_sub_ = create_subscription<std_msgs::msg::Int32>(
      cmd_rpm_topic_, 10,
      [&](const std_msgs::msg::Int32 & msg) {
        if (!allowed_master_on_) {
          requested_rpm_ = 0;
          return;
        }
        int rpm = msg.data;
        rpm = std::clamp(rpm, -max_rpm_, max_rpm_);
        requested_rpm_ = rpm;
      });

    state_rpm_pub_ = create_publisher<std_msgs::msg::Int32>(state_rpm_topic_, 10);
    statusword_pub_ = create_publisher<std_msgs::msg::UInt16>(statusword_topic_, 10);
    di_pub_ = create_publisher<std_msgs::msg::UInt16>(digital_inputs_topic_, 10);
    enabled_pub_ = create_publisher<std_msgs::msg::Bool>(enabled_topic_, 10);
    lift_status_pub_ = create_publisher<std_msgs::msg::Int32>(lift_status_topic_, 10);

    const auto period = std::chrono::milliseconds(send_period_ms_);
    timer_ = create_wall_timer(period, [&]() { loop(); });

    RCLCPP_INFO(get_logger(),
      "Lift driver (std_msgs) up: can=%s node_id=%d rpdo=0x%03X tpdo=0x%03X cmd=%s state=%s",
      can_interface_.c_str(), node_id_, rpdo_cobid_, tpdo_cobid_,
      cmd_rpm_topic_.c_str(), state_rpm_topic_.c_str());
  }

  ~KincoLiftNodeStd() override {
    if (can_socket_ >= 0) {
      ::close(can_socket_);
      can_socket_ = -1;
    }
  }

private:
  bool open_can() {
    can_socket_ = ::socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if (can_socket_ < 0) {
      RCLCPP_ERROR(get_logger(), "socket(PF_CAN) failed errno=%d", errno);
      return false;
    }

    struct ifreq ifr;
    std::memset(&ifr, 0, sizeof(ifr));
    std::strncpy(ifr.ifr_name, can_interface_.c_str(), IFNAMSIZ - 1);
    ifr.ifr_name[IFNAMSIZ - 1] = '\0';

    if (::ioctl(can_socket_, SIOCGIFINDEX, &ifr) < 0) {
      RCLCPP_ERROR(get_logger(), "ioctl(SIOCGIFINDEX) failed for %s errno=%d", can_interface_.c_str(), errno);
      ::close(can_socket_);
      can_socket_ = -1;
      return false;
    }

    struct sockaddr_can addr{};
    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;

    if (::bind(can_socket_, reinterpret_cast<const struct sockaddr*>(&addr), sizeof(addr)) < 0) {
      RCLCPP_ERROR(get_logger(), "bind() failed for %s errno=%d", can_interface_.c_str(), errno);
      ::close(can_socket_);
      can_socket_ = -1;
      return false;
    }

    std::vector<struct can_filter> filters;

    // TPDO
    {
      struct can_filter f{};
      f.can_id = static_cast<canid_t>(tpdo_cobid_);
      f.can_mask = CAN_SFF_MASK;
      filters.push_back(f);
    }

    // SDO response
    {
      struct can_filter f{};
      f.can_id = static_cast<canid_t>(0x580 + node_id_);
      f.can_mask = CAN_SFF_MASK;
      filters.push_back(f);
    }

    // Heartbeat/bootup
    {
      struct can_filter f{};
      f.can_id = static_cast<canid_t>(0x700 + node_id_);
      f.can_mask = CAN_SFF_MASK;
      filters.push_back(f);
    }

    if (!filters.empty()) {
      if (::setsockopt(can_socket_, SOL_CAN_RAW, CAN_RAW_FILTER,
                       filters.data(), filters.size() * sizeof(struct can_filter)) < 0) {
        RCLCPP_WARN(get_logger(), "setsockopt(CAN_RAW_FILTER) failed errno=%d", errno);
      }
    }

    return true;
  }

  void send_frame(uint32_t can_id, const uint8_t *data, uint8_t dlc) {
    struct can_frame frame{};
    frame.can_id = can_id;
    frame.can_dlc = dlc;
    std::memcpy(frame.data, data, dlc);

    ssize_t n = ::write(can_socket_, &frame, sizeof(frame));
    if (n < 0) {
      RCLCPP_WARN_THROTTLE(get_logger(), *get_clock(), 1000,
        "CAN write failed id=0x%03X errno=%d", can_id, errno);
    }
  }

  void send_nmt(uint8_t cmd, uint8_t node) {
    uint8_t d[2] = {cmd, node};
    send_frame(0x000, d, 2);
  }

  void sdo_download_u8(uint16_t idx, uint8_t sub, uint8_t val) {
    uint8_t d[8] = {
      0x2F,
      static_cast<uint8_t>(idx & 0xFF),
      static_cast<uint8_t>((idx >> 8) & 0xFF),
      sub,
      val, 0, 0, 0
    };
    send_frame(0x600 + node_id_, d, 8);
  }

  void sdo_download_u16(uint16_t idx, uint8_t sub, uint16_t val) {
    uint8_t d[8] = {
      0x2B,
      static_cast<uint8_t>(idx & 0xFF),
      static_cast<uint8_t>((idx >> 8) & 0xFF),
      sub,
      static_cast<uint8_t>(val & 0xFF),
      static_cast<uint8_t>((val >> 8) & 0xFF),
      0, 0
    };
    send_frame(0x600 + node_id_, d, 8);
  }

  void sdo_upload_u16(uint16_t idx, uint8_t sub) {
    uint8_t d[8] = {
      0x40,
      static_cast<uint8_t>(idx & 0xFF),
      static_cast<uint8_t>((idx >> 8) & 0xFF),
      sub,
      0, 0, 0, 0
    };
    send_frame(0x600 + node_id_, d, 8);
  }

  // Write a 32-bit SDO (available if you need it later)
  void sdo_download_u32(uint16_t idx, uint8_t sub, uint32_t val) {
    uint8_t d[8] = {
      0x23,
      static_cast<uint8_t>(idx & 0xFF),
      static_cast<uint8_t>((idx >> 8) & 0xFF),
      sub,
      static_cast<uint8_t>(val & 0xFF),
      static_cast<uint8_t>((val >> 8) & 0xFF),
      static_cast<uint8_t>((val >> 16) & 0xFF),
      static_cast<uint8_t>((val >> 24) & 0xFF)
    };
    send_frame(0x600 + node_id_, d, 8);
  }

  // Blocking read helper for one u16 SDO response (used only in fault reset)
  bool sdo_upload_u16_wait(uint16_t idx, uint8_t sub, uint16_t &out, double timeout_s = 0.2) {
    uint8_t req[8] = {
      0x40,
      static_cast<uint8_t>(idx & 0xFF),
      static_cast<uint8_t>((idx >> 8) & 0xFF),
      sub,
      0,0,0,0
    };
    send_frame(0x600 + node_id_, req, 8);

    struct can_frame frame{};
    const double start = now_s();
    while ((now_s() - start) < timeout_s) {
      int n = ::recv(can_socket_, &frame, sizeof(frame), MSG_DONTWAIT);
      if (n <= 0) {
        usleep(2000);
        continue;
      }
      uint32_t cob = frame.can_id & 0x7FF;
      if (cob == static_cast<uint32_t>(0x580 + node_id_) && frame.can_dlc == 8) {
        uint16_t ridx = static_cast<uint16_t>(frame.data[1]) | (static_cast<uint16_t>(frame.data[2]) << 8);
        uint8_t rsub = frame.data[3];
        if (ridx == idx && rsub == sub) {
          out = static_cast<uint16_t>(frame.data[4]) | (static_cast<uint16_t>(frame.data[5]) << 8);
          return true;
        }
      }
    }
    return false;
  }

  // Kinco-style fault reset sequence
  void reset_fault_seq() {
    RCLCPP_WARN(get_logger(), "FAULT RESET sequence start (node=%d)", node_id_);

    // 1) Standard CiA-402 fault reset
    sdo_download_u16(0x6040, 0x00, 0x0080);
    usleep(100000);

    // 2) Shutdown
    sdo_download_u16(0x6040, 0x00, 0x0006);
    usleep(100000);

    // 3) Kinco-specific: 0x2010:01 bit3 toggle (if exists)
    uint16_t cur = 0;
    if (sdo_upload_u16_wait(0x2010, 0x01, cur, 0.2)) {
      uint16_t set = static_cast<uint16_t>(cur | (1 << 3));
      sdo_download_u16(0x2010, 0x01, set);
      usleep(50000);
      sdo_download_u16(0x2010, 0x01, cur);
      usleep(50000);
      RCLCPP_INFO(get_logger(), "0x2010:01 toggled (cur=0x%04X)", cur);
    } else {
      RCLCPP_WARN(get_logger(), "0x2010:01 read timeout, continuing");
    }

    // 4) Re-enable will be handled by ensure_enabled()
    did_set_mode_ = false;
    enable_step_ = 0;

    RCLCPP_WARN(get_logger(), "FAULT RESET sequence done, will re-enable");
  }

  void send_rpdo_target_rpm(int rpm_cmd) {
    int rpm = std::clamp(rpm_cmd, -max_rpm_, max_rpm_);
    rpm *= cmd_sign_;

    int32_t obj = static_cast<int32_t>(std::llround(static_cast<double>(rpm) * rpm_to_obj_k_));
    uint8_t d[4] = {
      static_cast<uint8_t>(obj & 0xFF),
      static_cast<uint8_t>((obj >> 8) & 0xFF),
      static_cast<uint8_t>((obj >> 16) & 0xFF),
      static_cast<uint8_t>((obj >> 24) & 0xFF)
    };
    send_frame(static_cast<uint32_t>(rpdo_cobid_), d, 4);
  }

  static bool has_fault(uint16_t sw) {
    return (sw & 0x0008) != 0;
  }

  static bool is_operation_enabled(uint16_t sw) {
    return (sw & 0x006F) == 0x0027;
  }

  bool bit_active(uint16_t v, int bit, bool active_high) const {
    bool raw = ((v >> bit) & 0x1) != 0;
    return active_high ? raw : !raw;
  }

  int8_t motion_dir() const {
    return motion_dir_latched_;
  }

  bool cmd_moves_up(int cmd) const {
    // cmd_sign flips commanded motor direction before RPDO write.
    // Use it here so limit guard stays correct when cmd_sign is -1.
    return (cmd_sign_ >= 0) ? (cmd < 0) : (cmd > 0);
  }

  bool cmd_moves_down(int cmd) const {
    return (cmd_sign_ >= 0) ? (cmd > 0) : (cmd < 0);
  }

  void update_motion_dir() {
    const double t = now_s();
    if (!tpdo_seen_ || ((t - last_tpdo_time_) > stale_tpdo_stop_s_)) {
      motion_dir_latched_ = 0;
      return;
    }

    const double rpm_in_cmd_convention = motor_rpm_meas_ * static_cast<double>(cmd_sign_);

    if (motion_dir_latched_ == 0) {
      if (rpm_in_cmd_convention > motion_start_rpm_) {
        motion_dir_latched_ = 1;
      } else if (rpm_in_cmd_convention < -motion_start_rpm_) {
        motion_dir_latched_ = -1;
      }
      return;
    }

    if (motion_dir_latched_ > 0) {
      if (rpm_in_cmd_convention < -motion_start_rpm_) {
        motion_dir_latched_ = -1;
      } else if (rpm_in_cmd_convention < motion_stop_rpm_) {
        motion_dir_latched_ = 0;
      }
      return;
    }

    if (rpm_in_cmd_convention > motion_start_rpm_) {
      motion_dir_latched_ = 1;
    } else if (rpm_in_cmd_convention > -motion_stop_rpm_) {
      motion_dir_latched_ = 0;
    }
  }

  void poll_can() {
    struct can_frame frame{};
    for (int i = 0; i < 40; i++) {
      int n = ::recv(can_socket_, &frame, sizeof(frame), MSG_DONTWAIT);
      if (n <= 0) break;

      uint32_t cob = frame.can_id & 0x7FF;

      if (cob == static_cast<uint32_t>(tpdo_cobid_) && frame.can_dlc >= 6) {
        int32_t velocity_raw =
          (static_cast<int32_t>(frame.data[0])      ) |
          (static_cast<int32_t>(frame.data[1]) <<  8) |
          (static_cast<int32_t>(frame.data[2]) << 16) |
          (static_cast<int32_t>(frame.data[3]) << 24);

        statusword_ = static_cast<uint16_t>(frame.data[4]) |
                      (static_cast<uint16_t>(frame.data[5]) << 8);

        motor_rpm_meas_ = (velocity_raw * 1875.0) / (512.0 * 2500.0 * 4.0);
        tpdo_seen_ = true;
        last_tpdo_time_ = now_s();
      }

      if (cob == static_cast<uint32_t>(0x580 + node_id_) && frame.can_dlc == 8) {
        uint16_t idx = static_cast<uint16_t>(frame.data[1]) | (static_cast<uint16_t>(frame.data[2]) << 8);
        uint8_t sub = frame.data[3];

        if (idx == 0x60FD && sub == 0x00) {
          digital_inputs_ = static_cast<uint16_t>(frame.data[4]) |
                            (static_cast<uint16_t>(frame.data[5]) << 8);
          last_di_time_ = now_s();
        }
      }
    }
  }

  void ensure_enabled() {
    const double t = now_s();

    if (!did_nmt_) {
      if (send_nmt_reset_all_) {
        send_nmt(0x81, 0x00);
        usleep(150000);
      }
      if (send_nmt_start_all_) {
        send_nmt(0x01, 0x00);
        usleep(80000);
      }
      did_nmt_ = true;
      last_enable_step_time_ = t;
      return;
    }

    if (tpdo_seen_ && has_fault(statusword_)) {
      enabled_ = false;
      if ((t - last_fault_reset_time_) > 1.0) {
        last_fault_reset_time_ = t;
        reset_fault_seq();
      }
      return;
    }

    if (tpdo_seen_ && is_operation_enabled(statusword_)) {
      enabled_ = true;
      return;
    }

    enabled_ = false;

    if ((t - last_enable_step_time_) < 0.12) return;
    last_enable_step_time_ = t;

    if (!did_set_mode_) {
      sdo_download_u8(0x6060, 0x00, 0x03); // Profile Velocity
      did_set_mode_ = true;
      enable_step_ = 0;
      RCLCPP_INFO(get_logger(), "Sent 6060=3");
      return;
    }

    if (enable_step_ == 0) {
      sdo_download_u16(0x6040, 0x00, 0x0080);
      enable_step_ = 1;
      return;
    }
    if (enable_step_ == 1) {
      sdo_download_u16(0x6040, 0x00, 0x0006);
      enable_step_ = 2;
      return;
    }
    if (enable_step_ == 2) {
      sdo_download_u16(0x6040, 0x00, 0x0007);
      enable_step_ = 3;
      return;
    }
    sdo_download_u16(0x6040, 0x00, 0x000F);
    enable_step_ = 1;
  }

  void publish_topics() {
    std_msgs::msg::Int32 rpm_msg;
    rpm_msg.data = static_cast<int32_t>(std::llround(motor_rpm_meas_));
    state_rpm_pub_->publish(rpm_msg);

    std_msgs::msg::UInt16 sw_msg;
    sw_msg.data = statusword_;
    statusword_pub_->publish(sw_msg);

    std_msgs::msg::UInt16 di_msg;
    di_msg.data = digital_inputs_;
    di_pub_->publish(di_msg);

    std_msgs::msg::Bool en_msg;
    en_msg.data = enabled_;
    enabled_pub_->publish(en_msg);

    const bool at_bottom = bit_active(digital_inputs_, limit_low_bit_,  limit_low_active_high_);
    const bool at_top = bit_active(digital_inputs_, limit_high_bit_, limit_high_active_high_);
    const int8_t dir = motion_dir(); // -1 up, 0 stopped, 1 down

    // Packed /lift/status Int32:
    // bit0 = at_top (user bit1), bit1 = at_bottom (user bit2), bits[15:8] = int8 direction.
    int32_t packed = 0;
    if (at_top) packed |= (1 << 0);
    if (at_bottom) packed |= (1 << 1);
    packed |= (static_cast<int32_t>(static_cast<uint8_t>(dir)) << 8);

    std_msgs::msg::Int32 status_msg;
    status_msg.data = packed;
    lift_status_pub_->publish(status_msg);
  }

  void loop() {
    poll_can();
    update_motion_dir();

    if (enable_di_poll_) {
      const double t = now_s();
      const double period = (di_poll_hz_ > 0.0) ? (1.0 / di_poll_hz_) : 0.1;
      if ((t - last_di_poll_time_) > period) {
        sdo_upload_u16(0x60FD, 0x00);
        last_di_poll_time_ = t;
      }
    }

    ensure_enabled();

    int cmd = requested_rpm_;
    if (!allowed_master_on_) cmd = 0;
    if (!enabled_) cmd = 0;

    if (enable_limit_guard_) {
      bool low  = bit_active(digital_inputs_, limit_low_bit_,  limit_low_active_high_);
      bool high = bit_active(digital_inputs_, limit_high_bit_, limit_high_active_high_);

      if (cmd_moves_down(cmd) && low) cmd = 0;
      if (cmd_moves_up(cmd) && high)  cmd = 0;

      // Secondary safety fence: if measured motion is toward an active limit,
      // force zero command regardless of caller command convention/sign.
      const int8_t dir = motion_dir(); // -1 up, +1 down
      if (high && dir < 0) cmd = 0;
      if (low && dir > 0)  cmd = 0;

      if (low && high) {
        RCLCPP_WARN_THROTTLE(get_logger(), *get_clock(), 1000,
          "Both limits read active DI=0x%04X check polarity or wiring", digital_inputs_);
      }
    }

    const double t = now_s();
    if ((t - last_send_time_) > 0.02 || cmd != last_sent_cmd_) {
      send_rpdo_target_rpm(cmd);
      last_sent_cmd_ = cmd;
      last_send_time_ = t;
    }

    publish_topics();
  }

  double now_s() const {
    using clock = std::chrono::steady_clock;
    return std::chrono::duration_cast<std::chrono::duration<double>>(clock::now().time_since_epoch()).count();
  }

private:
  std::string can_interface_;
  int node_id_{3};
  int can_socket_{-1};

  std::string allowed_topic_;
  std::string cmd_rpm_topic_;
  std::string state_rpm_topic_;
  std::string statusword_topic_;
  std::string digital_inputs_topic_;
  std::string enabled_topic_;
  std::string lift_status_topic_;

  int max_rpm_{2500};
  int send_period_ms_{20};

  int rpdo_cobid_{0x173};
  int tpdo_cobid_{0x183};
  double rpm_to_obj_k_{(512.0 * 2500.0 * 4.0) / 1875.0};
  int cmd_sign_{1};

  bool send_nmt_reset_all_{false};
  bool send_nmt_start_all_{true};

  bool enable_di_poll_{true};
  double di_poll_hz_{10.0};

  bool enable_limit_guard_{false};
  int limit_low_bit_{0};
  int limit_high_bit_{1};
  bool limit_low_active_high_{true};
  bool limit_high_active_high_{true};
  double motion_start_rpm_{30.0};
  double motion_stop_rpm_{10.0};
  double stale_tpdo_stop_s_{0.3};

  bool allowed_master_on_{false};
  int requested_rpm_{0};

  bool did_nmt_{false};
  bool did_set_mode_{false};
  int enable_step_{0};
  bool enabled_{false};

  bool tpdo_seen_{false};
  uint16_t statusword_{0};
  double motor_rpm_meas_{0.0};
  int8_t motion_dir_latched_{0};

  uint16_t digital_inputs_{0};

  double last_enable_step_time_{0.0};
  double last_fault_reset_time_{0.0};
  double last_send_time_{0.0};
  int last_sent_cmd_{0};

  double last_di_poll_time_{0.0};
  double last_di_time_{0.0};
  double last_tpdo_time_{0.0};

  rclcpp::TimerBase::SharedPtr timer_;

  rclcpp::Subscription<std_msgs::msg::Bool>::SharedPtr allowed_sub_;
  rclcpp::Subscription<std_msgs::msg::Int32>::SharedPtr cmd_sub_;

  rclcpp::Publisher<std_msgs::msg::Int32>::SharedPtr state_rpm_pub_;
  rclcpp::Publisher<std_msgs::msg::UInt16>::SharedPtr statusword_pub_;
  rclcpp::Publisher<std_msgs::msg::UInt16>::SharedPtr di_pub_;
  rclcpp::Publisher<std_msgs::msg::Bool>::SharedPtr enabled_pub_;
  rclcpp::Publisher<std_msgs::msg::Int32>::SharedPtr lift_status_pub_;
};

int main(int argc, char **argv) {
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<KincoLiftNodeStd>());
  rclcpp::shutdown();
  return 0;
}
