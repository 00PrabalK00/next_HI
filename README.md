# Next HI Hardware Interface Wiki

## 1. Purpose

`next_HI` is a ROS 2 hardware integration package for a differential drive mobile robot using Kinco servo drives over CANopen. The repository provides the low level bridge between ROS 2 motion commands and the physical motor drives. It also includes robot description files, controller configuration, a lift actuator driver, SLAM Toolbox configuration and Nav2 bringup support.

The central package is `next_motor_interface`. Its main responsibility is to expose Kinco wheel motors as a `ros2_control` hardware system so higher level ROS 2 controllers can command wheel velocity without directly handling CAN frames, CiA 402 state transitions or Kinco specific fault recovery.

The system is designed around these major goals:

1. Convert ROS 2 velocity commands into Kinco drive RPDO target velocity frames.
2. Read TPDO feedback from the drives and expose wheel position plus velocity to `ros2_control`.
3. Handle CiA 402 enable, operation enabled detection, fault detection and fault recovery.
4. Provide a separate lift motor node for Node 3 with command gating and limit switch protection.
5. Support mapping, localization and autonomous navigation using SLAM Toolbox and Nav2.
6. Provide a robot model through URDF and xacro for visualization, transforms and controller setup.

## 2. Repository Structure

```text
next_HI
  src
    next_motor_interface
      CMakeLists.txt
      package.xml
      kinco_hardware_interface.xml
      src
        kinco_hardware_interface.cpp
        kinco_lift_node3_std.cpp
      msg
        MotorFeedback.msg
      launch
        robot_control.launch.py
        lift_node3.launch.py
        slam.launch.py
        localization.launch.py
        nav2.launch.py
      config
        bus_can0
          bus.yml
          ros2_controllers.yaml
          left_can0.eds
          right_can0.eds
        lift_node3.yaml
        mapper_params_online_async.yaml
        nav2_params.yaml
        slam.rviz
      urdf
        robot.urdf.xacro
        robot_core.xacro
        ros2_control.xacro
        ros2_control_kinco.xacro
        lidar.xacro
        imu.xacro
        face.xacro
      maps
        README.md
      rviz
        nav2.rviz
```

### 2.1 Important source files

| File | Role |
| --- | --- |
| `kinco_hardware_interface.cpp` | Custom `hardware_interface::SystemInterface` plugin for the two base wheel Kinco drives. |
| `kinco_lift_node3_std.cpp` | Standalone ROS 2 node for the lift actuator on CANopen Node 3. |
| `ros2_control.xacro` | Declares the custom hardware plugin and maps wheel joints to Kinco motor IDs. |
| `ros2_controllers.yaml` | Configures `joint_state_broadcaster` and `diff_drive_controller`. |
| `robot_control.launch.py` | Starts robot state publisher, controller manager and controller spawners. |
| `lift_node3.launch.py` | Starts the lift actuator driver with parameters from `lift_node3.yaml`. |
| `nav2.launch.py` | Starts Nav2 bringup and bridges `/cmd_vel` into the wheel controller command topic. |
| `slam.launch.py` | Starts SLAM Toolbox in async mapping mode. |
| `localization.launch.py` | Starts SLAM Toolbox in localization mode using an existing map. |
| `MotorFeedback.msg` | Defines a structured motor feedback message. The current base driver does not publish it yet. |

## 3. High Level Architecture

```text
Operator or Nav2
  ↓
/cmd_vel or teleop command
  ↓
Diff drive controller
  ↓
ros2_control controller manager
  ↓
KincoHardwareInterface plugin
  ↓
SocketCAN raw CAN socket
  ↓
CANopen RPDO, TPDO, SDO and NMT traffic
  ↓
Kinco wheel drives on Node 1 and Node 2
```

For the lift actuator, the flow is separate:

```text
Lift command source
  ↓
/lift/cmd_rpm plus /allowed_master_on
  ↓
kinco_lift_node3_std
  ↓
CANopen RPDO, TPDO, SDO and NMT traffic
  ↓
Kinco lift drive on Node 3
  ↓
/lift/state_rpm, /lift/statusword, /lift/digital_inputs, /lift/enabled and /lift/status
```

The base wheel system is integrated into ROS 2 through `ros2_control`. The lift system is implemented as a standalone node because it is not modeled as a wheel joint. This separation keeps navigation control and actuator control independent while still sharing the same CAN bus.

## 4. Core Concepts

### 4.1 ROS 2 control

`ros2_control` is the ROS 2 framework that separates high level controllers from hardware specific drivers. A controller, such as `diff_drive_controller`, only sees generic joint interfaces. The custom hardware plugin translates those generic joint commands into CANopen messages understood by the Kinco drives.

In this package, the hardware plugin exports:

| Joint | Command interface | State interfaces | Motor ID |
| --- | --- | --- | --- |
| `left_wheel_joint` | velocity | position and velocity | 2 |
| `right_wheel_joint` | velocity | position and velocity | 1 |

The motor ID mapping is declared in `ros2_control.xacro`.

### 4.2 CANopen

CANopen is the communication layer used by the Kinco drives. This project uses raw SocketCAN frames directly inside the C++ drivers.

Key CANopen message types used here:

| Type | Meaning | Used for |
| --- | --- | --- |
| NMT | Network management | Resetting and starting CANopen nodes. |
| SDO | Service data object | Reading or writing object dictionary entries such as mode of operation, controlword, statusword and digital inputs. |
| RPDO | Receive process data object | Sending fast runtime commands from ROS 2 to the drive. |
| TPDO | Transmit process data object | Receiving fast runtime feedback from the drive. |
| Heartbeat or bootup | Node state indication | Accepted by CAN filters for diagnostics. |

### 4.3 CiA 402

CiA 402 is the CANopen drive state machine used by servo drives. It defines states such as ready to switch on, switched on, operation enabled and fault. The code uses statusword bit masks to decide whether the drive is safe to command.

Important checks implemented in the base driver:

```cpp
operation_enabled = (statusword & 0x006F) == 0x0027
switched_on       = (statusword & 0x006F) == 0x0023
ready_to_switch   = (statusword & 0x004F) == 0x0021
fault             = (statusword & 0x0008) != 0
```

The main command gate is simple and strict: wheel velocity commands are only sent when the hardware state machine reaches `BASE_CONTROL` and every motor reports operation enabled.

## 5. Build Requirements

The package is designed for ROS 2 Humble style workspaces and depends on the following ROS 2 components:

| Dependency | Purpose |
| --- | --- |
| `ament_cmake` | ROS 2 CMake build system. |
| `rosidl_default_generators` | Generates the custom `MotorFeedback` message. |
| `std_msgs` | Standard message types for heartbeat and lift topics. |
| `hardware_interface` | Base API for the custom `ros2_control` hardware plugin. |
| `pluginlib` | Runtime loading of the hardware interface plugin. |
| `rclcpp` | ROS 2 C++ node API. |
| `rclcpp_lifecycle` | Lifecycle integration for the hardware interface. |
| `lely_core_libraries` | CANopen related dependency. |
| `canopen` | CANopen package dependency declared in `package.xml`. |

The robot side must also have a working CAN interface such as `can0` configured before launching the drivers.

## 6. Build Instructions

From the workspace root:

```bash
source /opt/ros/humble/setup.bash
colcon build --packages-select next_motor_interface
source install/setup.bash
```

If the CAN interface is not already active, bring it up before starting the robot:

```bash
sudo ip link set can0 down
sudo ip link set can0 type can bitrate 1000000
sudo ip link set can0 up
```

Use the bitrate that matches the physical Kinco drive configuration. The repository configs assume the logical interface name is `can0`.

## 7. Runtime Launch Flows

### 7.1 Base robot control

Launch the robot description, controller manager and diff drive controller:

```bash
ros2 launch next_motor_interface robot_control.launch.py
```

This launch file performs the following sequence:

1. Builds the robot description from `robot.urdf.xacro`.
2. Starts `robot_state_publisher`.
3. Starts `ros2_control_node` with the robot description and `ros2_controllers.yaml`.
4. Waits two seconds.
5. Spawns `joint_state_broadcaster`.
6. Spawns `wheel_controller`.

### 7.2 Teleoperation command path

The included command file remaps teleop output directly into the diff drive controller input:

```bash
ros2 run teleop_twist_keyboard teleop_twist_keyboard --ros-args -r cmd_vel:=/wheel_controller/cmd_vel_unstamped
```

The wheel controller is configured with `use_stamped_vel: false`, so it expects an unstamped `geometry_msgs/Twist` command.

### 7.3 Nav2 command path

Launch Nav2:

```bash
ros2 launch next_motor_interface nav2.launch.py
```

The Nav2 launch flow does this:

1. Loads the Nav2 parameter file from `config/nav2_params.yaml`.
2. Loads the default map from `~/maps/my_map.yaml` unless another map is passed.
3. Starts `nav2_bringup`.
4. Starts an optional RViz instance with `rviz/nav2.rviz`.
5. Starts a `topic_tools` relay from `/cmd_vel` to `/wheel_controller/cmd_vel_unstamped`.

The resulting command path is:

```text
Nav2 controller server
  ↓
/cmd_vel
  ↓
topic_tools relay
  ↓
/wheel_controller/cmd_vel_unstamped
  ↓
diff_drive_controller
  ↓
KincoHardwareInterface
```

### 7.4 Lift driver

Launch the lift node:

```bash
ros2 launch next_motor_interface lift_node3.launch.py
```

The lift node reads parameters from `config/lift_node3.yaml`. By default it uses CANopen Node 3, RPDO COB ID `0x203`, TPDO COB ID `0x183` and command topic `/lift/cmd_rpm`.

### 7.5 Mapping

Launch SLAM Toolbox in async mapping mode:

```bash
ros2 launch next_motor_interface slam.launch.py
```

The mapping node uses `mapper_params_online_async.yaml`. It subscribes to `/scan`, uses `odom` as odometry frame, publishes `map` and uses `base_footprint` as the robot base frame.

### 7.6 Localization

Launch SLAM Toolbox localization mode with an existing map:

```bash
ros2 launch next_motor_interface localization.launch.py map_file:=/path/to/my_map.yaml
```

This mode uses an existing map and starts the robot at `[0.0, 0.0, 0.0]` unless changed in the launch file.

## 8. Base Wheel Hardware Interface

The base wheel interface is implemented in `kinco_hardware_interface.cpp`. It inherits from `hardware_interface::SystemInterface` and is exported through `pluginlib` as:

```text
next_motor_interface/KincoHardwareInterface
```

### 8.1 Lifecycle methods

| Method | Responsibility |
| --- | --- |
| `on_init` | Reads `can_interface_name`, creates joint state arrays, command arrays, statusword arrays, motor ID mappings and a heartbeat publisher. |
| `export_state_interfaces` | Exposes wheel position and wheel velocity to ROS 2 controllers. |
| `export_command_interfaces` | Exposes wheel velocity command interfaces to ROS 2 controllers. |
| `on_configure` | Opens the CAN socket, binds to `can0`, increases the receive buffer, installs CAN filters, resets and starts all nodes then runs the Node 2 init sequence. |
| `on_activate` | Marks the hardware interface ready for runtime state machine operation. |
| `on_deactivate` | Sends zero RPM to all wheel motors. |
| `on_cleanup` | Closes the CAN socket and resets heartbeat resources. |
| `read` | Processes incoming TPDO frames, updates wheel velocity, integrates position, runs the state machine and publishes heartbeat. |
| `write` | Sends wheel velocity commands only after the state machine reaches `BASE_CONTROL`. |

### 8.2 CAN socket setup

The driver opens a raw CAN socket using:

```cpp
socket(PF_CAN, SOCK_RAW, CAN_RAW)
```

It then binds the socket to the configured CAN interface name. The receive buffer is increased to 4 MB to reduce dropped frames under high traffic.

The driver installs CAN filters for each wheel motor:

| Accepted COB ID | Purpose |
| --- | --- |
| `0x180 + motor_id` | TPDO feedback from drive. |
| `0x580 + motor_id` | SDO response from drive. |
| `0x700 + motor_id` | Heartbeat or bootup frame. |

This reduces unnecessary userspace processing and keeps the read loop focused on relevant drive messages.

### 8.3 Motor mapping

The active URDF maps motors like this:

| Joint | Motor ID | Physical meaning |
| --- | --- | --- |
| `right_wheel_joint` | 1 | Right wheel drive. |
| `left_wheel_joint` | 2 | Left wheel drive. |

The code applies sign handling so both wheels follow the same ROS motion convention even though the physical motors are mounted opposite each other.

## 9. Base Motor State Machine

The base driver contains a non blocking drive state machine. This is the most important safety and reliability mechanism in the package.

### 9.1 State list

| State | Meaning |
| --- | --- |
| `INIT` | Initial entry state after construction. |
| `ROBOT_SYSTEM_CHECK` | Placeholder system check stage. |
| `INIT_MOTOR` | Sends mode of operation and CiA 402 enable sequence. |
| `MOTOR_STATE_CHECK` | Reads statusword and diagnostic SDOs. |
| `WAIT_STATUSWORD_ENABLED` | Waits until all motors report operation enabled. |
| `BASE_CONTROL` | Normal runtime state where wheel commands are allowed. |
| `FAULT_DETECTED` | Fault bit detected in at least one motor statusword. |
| `CLEAR_FAULT` | Attempts drive fault reset and reinitialization. |
| `ERROR` | Terminal error state after timeout or unrecoverable enable failure. |
| `PAUSE` | Reserved pause state. |

### 9.2 State transition flow

```text
INIT
  ↓
ROBOT_SYSTEM_CHECK
  ↓
INIT_MOTOR
  ↓
MOTOR_STATE_CHECK
  ↓
WAIT_STATUSWORD_ENABLED
  ↓
BASE_CONTROL
```

Fault path:

```text
Any monitored state
  ↓
FAULT_DETECTED
  ↓
CLEAR_FAULT
  ↓
INIT_MOTOR
```

Timeout path:

```text
WAIT_STATUSWORD_ENABLED
  ↓
ERROR
```

### 9.3 Enable algorithm

The enable sequence follows the CiA 402 drive startup pattern:

1. Set mode of operation to Profile Velocity using object `0x6060:00 = 0x03`.
2. Send shutdown controlword `0x6040:00 = 0x0006`.
3. Send switch on controlword `0x6040:00 = 0x0007`.
4. Send enable operation controlword `0x6040:00 = 0x000F`.
5. Read TPDO statusword updates.
6. Confirm operation enabled using `(statusword & 0x006F) == 0x0027`.
7. Enter `BASE_CONTROL` only when every configured motor passes the operation enabled check.

The implementation is non blocking. It uses time based delays between sub steps so the hardware loop can continue processing feedback.

### 9.4 Re enable algorithm

If a motor is not operation enabled while waiting, the driver periodically resends the enable sequence. Every approximate one second window, it checks each statusword and resends shutdown, switch on and enable operation for motors that are not ready.

If the motors still do not enable after the timeout window, the driver prints a diagnostic dump and transitions to `ERROR`.

Diagnostics include:

| Object | Meaning |
| --- | --- |
| `0x6040:00` | Controlword. |
| `0x603F:00` | Error code. |
| `0x1001:00` | Error register. |
| `0x60FD:00` | Digital inputs. |
| `0x2010:03` | DIN1 function configuration. |
| `0x2010:0A` | Input status. |
| `0x2010:01` | Input polarity. |

### 9.5 Fault recovery algorithm

When a fault bit is detected, the driver enters `CLEAR_FAULT`. For each motor it runs a Kinco oriented reset sequence:

1. Send standard CiA 402 fault reset with `0x6040:00 = 0x0080`.
2. Send shutdown with `0x6040:00 = 0x0006`.
3. Read `0x2010:01`.
4. Temporarily set bit 3 of `0x2010:01` if the read succeeds.
5. Restore the original `0x2010:01` value.
6. Re enter `INIT_MOTOR` and attempt normal enable again.

This combines the standard CiA 402 reset path with a Kinco specific digital input reset toggle.

## 10. Wheel Command Algorithm

The `write` method converts ROS wheel velocity commands into drive target RPM and then into Kinco velocity object units.

### 10.1 ROS command to motor RPM

For each wheel command in radians per second:

```text
rpm = negative(command_rad_per_second × 60) / (2 × pi)
rpm = rpm × 20
```

The factor `20` is the gear ratio used by the driver. Motor ID 1 receives an additional sign inversion to account for physical mounting direction.

### 10.2 RPM to Kinco velocity object value

The RPDO payload is a signed 32 bit target velocity value. The conversion used is:

```text
object_value = rpm × 512 × 2500 × 4 / 1875
```

The object value is serialized little endian into four bytes and sent through an RPDO frame:

```text
CAN ID = 0x170 + motor_id
DLC = 4
Payload = signed 32 bit object_value in little endian order
```

### 10.3 Command throttling

To avoid flooding the CAN bus, the driver stores the last sent object value per motor. If the new command is identical and the last send was less than 100 ms ago, the frame is skipped. This still refreshes unchanged commands at up to 10 Hz while allowing changed commands to pass immediately.

### 10.4 Command safety gate

The driver does not send motor commands unless:

```text
current_state == BASE_CONTROL
```

This prevents motion during initialization, fault clearing, diagnostics, cleanup and error states.

## 11. Wheel Feedback Algorithm

The `read` method processes up to 20 CAN frames per cycle. It looks for TPDO frames with IDs from `0x181` to `0x1FF`.

### 11.1 TPDO decoding

For each TPDO:

| Payload bytes | Meaning |
| --- | --- |
| `0..3` | Signed 32 bit raw velocity, little endian. |
| `4..5` | Unsigned 16 bit statusword, little endian. |

The motor ID is derived from:

```text
motor_id = COB_ID minus 0x180
```

### 11.2 Raw velocity to ROS velocity

The base wheel measured velocity is converted using:

```text
rpm = raw_velocity × 1875 / (512 × 2500 × 4) / 20
rad_per_second = negative(rpm × 2 × pi) / 60
```

Motor ID 1 receives a sign inversion so right wheel feedback matches the ROS joint convention.

### 11.3 Position integration

The hardware interface does not read absolute wheel position from the drive. It integrates wheel position from measured velocity:

```text
position = previous_position + measured_velocity × dt
```

`dt` comes from the `ros2_control` period. If the period is invalid or too large, the driver falls back to `0.02` seconds.

This gives the diff drive controller continuous joint position feedback for odometry generation.

## 12. Motor Heartbeat

The base hardware plugin publishes a Boolean heartbeat on:

```text
motor_heartbeat
```

Heartbeat behavior:

1. It is only considered valid if the state is `BASE_CONTROL`.
2. Every motor must report operation enabled.
3. The enabled condition must remain stable for 500 ms.
4. After stability is confirmed, the driver publishes `true` at most once per second.

This topic is useful for higher level supervisors that need to know when the wheel system is truly ready for motion.

## 13. Node 2 Special Initialization

The base driver includes a special initialization sequence for CANopen Node 2.

Purpose:

1. Read DIN1 function from `0x2010:03`.
2. If DIN1 is configured as an enable function, put Node 2 into Pre Operational mode.
3. Clear `0x2010:03` by writing `0x0000`.
4. Read the value back to confirm the change.
5. Return Node 2 to Operational mode.
6. Run fault reset.
7. Set mode of operation to `0xFD`.
8. Send shutdown, switch on and enable operation controlwords.
9. Read statusword and controlword for confirmation.

This sequence appears to address a drive condition where DIN1 could block software enabling if it is configured as a physical enable input and is inactive.

## 14. Lift Actuator Driver

The lift actuator is implemented in `kinco_lift_node3_std.cpp`. It is a standalone ROS 2 node named:

```text
kinco_lift_node3_std
```

The default CANopen node ID is `3`.

### 14.1 Lift topics

| Topic | Type | Direction | Meaning |
| --- | --- | --- | --- |
| `/allowed_master_on` | `std_msgs/Bool` | Input | Master safety permission. If false, requested RPM is forced to zero. |
| `/lift/cmd_rpm` | `std_msgs/Int32` | Input | Requested lift motor RPM. |
| `/lift/state_rpm` | `std_msgs/Int32` | Output | Measured lift motor RPM. |
| `/lift/statusword` | `std_msgs/UInt16` | Output | Latest CiA 402 statusword. |
| `/lift/digital_inputs` | `std_msgs/UInt16` | Output | Latest digital input bitfield. |
| `/lift/enabled` | `std_msgs/Bool` | Output | True when operation enabled is detected. |
| `/lift/status` | `std_msgs/Int32` | Output | Packed lift status used by higher level logic. |

### 14.2 Lift parameters

Important parameters from `lift_node3.yaml`:

| Parameter | Current value | Meaning |
| --- | --- | --- |
| `can_interface` | `can0` | SocketCAN interface. |
| `node_id` | `3` | CANopen node ID of lift drive. |
| `rpdo_cobid` | `0x203` | Command RPDO COB ID. |
| `tpdo_cobid` | `0x183` | Feedback TPDO COB ID. |
| `enable_limit_guard` | `true` | Enables limit switch based command blocking. |
| `di_poll_hz` | `100.0` | Digital input polling rate. |
| `limit_low_bit` | `1` | Digital input bit for lower limit. |
| `limit_high_bit` | `0` | Digital input bit for upper limit. |
| `motion_start_rpm` | `50.0` | RPM threshold to latch motion direction. |
| `motion_stop_rpm` | `20.0` | RPM threshold to release motion direction. |
| `stale_tpdo_stop_s` | `0.3` | Feedback timeout before motion direction is treated as stopped. |

### 14.3 Lift enable algorithm

The lift node continuously calls `ensure_enabled()` from the timer loop.

The sequence is:

1. Optionally send NMT reset all.
2. Send NMT start all if enabled by parameter.
3. If TPDO feedback reports a fault, run the fault reset sequence.
4. If statusword reports operation enabled, set `/lift/enabled` true.
5. If not enabled, send mode of operation `0x6060:00 = 0x03`.
6. Send fault reset controlword `0x0080`.
7. Send shutdown controlword `0x0006`.
8. Send switch on controlword `0x0007`.
9. Send enable operation controlword `0x000F`.
10. Repeat until operation enabled is detected.

The lift enable loop is timed so it does not block the ROS node.

### 14.4 Lift command algorithm

The lift command topic accepts integer RPM. The node clamps the command to `max_rpm`. It then applies `cmd_sign` and converts RPM into Kinco object units:

```text
object_value = rpm × rpm_to_obj_k
rpm_to_obj_k = 512 × 2500 × 4 / 1875
```

The output is serialized as a signed 32 bit little endian value and sent to the configured RPDO COB ID.

### 14.5 Lift command gating

Before sending a command, the lift node applies these gates:

1. If `/allowed_master_on` is false, command becomes zero.
2. If the drive is not operation enabled, command becomes zero.
3. If lower limit is active and the command moves down, command becomes zero.
4. If upper limit is active and the command moves up, command becomes zero.
5. If measured motion direction is toward an active limit, command becomes zero regardless of the requested command.

This gives the lift two layers of protection: command intent protection and measured motion protection.

### 14.6 Lift motion direction latch

The node estimates lift motion direction from measured RPM. It uses hysteresis to prevent direction flicker:

1. If no TPDO was received recently, direction is zero.
2. If current direction is stopped and RPM exceeds `motion_start_rpm`, direction is latched.
3. If current direction is moving and RPM drops below `motion_stop_rpm`, direction returns to stopped.
4. If RPM reverses strongly, the direction latch flips.

The published status uses this direction estimate plus limit switch states.

### 14.7 Packed lift status

`/lift/status` is a packed `Int32` value:

| Bits | Meaning |
| --- | --- |
| bit 0 | Upper limit active. |
| bit 1 | Lower limit active. |
| bits 15 to 8 | Signed 8 bit direction value. |

Direction convention in the code:

| Direction value | Meaning |
| --- | --- |
| `-1` | Moving up. |
| `0` | Stopped or unknown. |
| `1` | Moving down. |

## 15. Controller Configuration

The controller configuration is in `config/bus_can0/ros2_controllers.yaml`.

### 15.1 Controller manager

```yaml
controller_manager:
  ros__parameters:
    update_rate: 100
```

The controller manager runs at 100 Hz.

### 15.2 Joint state broadcaster

`joint_state_broadcaster` publishes joint states from the hardware interface. These states come from the measured wheel velocity and integrated position.

### 15.3 Diff drive controller

`wheel_controller` is configured as `diff_drive_controller/DiffDriveController`.

Important values:

| Parameter | Value | Meaning |
| --- | --- | --- |
| `left_wheel_names` | `left_wheel_joint` | Left wheel joint source. |
| `right_wheel_names` | `right_wheel_joint` | Right wheel joint source. |
| `wheel_separation` | `0.525` m | Distance between wheels used by odometry and angular velocity calculation. |
| `wheel_radius` | `0.10` m | Wheel radius used by odometry and velocity conversion. |
| `open_loop` | `false` | Odometry uses feedback from wheel state instead of assuming commanded velocity. |
| `enable_odom_tf` | `false` | Controller does not publish odom TF. |
| `publish_odom` | `true` | Controller publishes odometry message for downstream localization. |
| `cmd_vel_timeout` | `0.5` s | Stops if command topic becomes stale. |
| `publish_rate` | `50.0` Hz | Odometry and command publish rate. |
| `linear.x.max_velocity` | `1.0` m per second | Linear velocity limit. |
| `angular.z.max_velocity` | `1.5` rad per second | Angular velocity limit. |

The controller publishes odometry but leaves TF ownership to a higher level localization source.

## 16. Navigation Stack

The repository includes a Nav2 parameter file and launch wrapper. Nav2 is not implemented inside this package, but the package configures and connects it to the wheel controller.

### 16.1 Global planner

The global planner is configured as:

```yaml
plugin: nav2_navfn_planner/NavfnPlanner
use_astar: false
allow_unknown: true
```

This means the configured planner is Navfn with Dijkstra style planning rather than A star. It computes a path through the global costmap from the robot pose to the goal pose.

### 16.2 Local planner

The local controller plugin is:

```yaml
plugin: dwb_core::DWBLocalPlanner
```

DWB means Dynamic Window Based local planning. It samples candidate linear and angular velocities, simulates short trajectories and scores them using critics.

Configured critics:

| Critic | Purpose |
| --- | --- |
| `RotateToGoal` | Encourages final heading alignment near the goal. |
| `Oscillation` | Penalizes oscillatory motion. |
| `BaseObstacle` | Penalizes trajectories near obstacles. |
| `GoalAlign` | Encourages heading toward the goal. |
| `PathAlign` | Encourages alignment with the global path. |
| `PathDist` | Penalizes distance away from the global path. |
| `GoalDist` | Penalizes remaining distance to the goal. |

The controller samples `20` linear x velocities and `20` angular velocities over a `1.7` second simulation window.

### 16.3 Local costmap

The local costmap uses a rolling window in the `odom` frame with a 3 m by 3 m area and 0.05 m resolution. It uses:

1. Voxel layer for scan based obstacle marking and clearing.
2. Inflation layer for obstacle safety margin.

### 16.4 Global costmap

The global costmap uses the `map` frame and includes:

1. Static layer for the loaded map.
2. Obstacle layer for runtime scan obstacles.
3. Inflation layer for safety margin around obstacles.

### 16.5 Behavior tree navigator

The behavior tree navigator includes standard Nav2 actions and recovery behaviors such as path planning, path following, spin, wait, backup, clear costmap and recovery nodes.

## 17. SLAM and Localization

### 17.1 Mapping mode

`slam.launch.py` starts:

```text
slam_toolbox async_slam_toolbox_node
```

The configured SLAM mode is mapping. It uses Ceres based scan matching:

| Parameter | Value |
| --- | --- |
| `solver_plugin` | `solver_plugins::CeresSolver` |
| `ceres_linear_solver` | `SPARSE_NORMAL_CHOLESKY` |
| `ceres_preconditioner` | `SCHUR_JACOBI` |
| `ceres_trust_strategy` | `LEVENBERG_MARQUARDT` |
| `do_loop_closing` | `true` |
| `resolution` | `0.05` m |
| `minimum_travel_distance` | `0.3` m |
| `minimum_travel_heading` | `0.3` rad |

The mapping configuration is tuned to reduce excessive scan updates while keeping loop closure enabled for drift correction.

### 17.2 Localization mode

`localization.launch.py` starts:

```text
slam_toolbox localization_slam_toolbox_node
```

It uses an existing map file and provides localization against that map using `/scan`, `odom`, `map` and `base_footprint`.

## 18. Robot Description

The robot model is built from xacro files.

### 18.1 Main xacro

`robot.urdf.xacro` includes:

1. `robot_core.xacro`
2. `ros2_control.xacro`
3. `lidar.xacro`
4. `face.xacro`

### 18.2 Base geometry

`robot_core.xacro` defines:

| Component | Value |
| --- | --- |
| Chassis length | `0.80` m |
| Chassis width | `0.58` m |
| Chassis height | `0.09` m |
| Wheel radius | `0.10` m |
| Wheel thickness | `0.04` m |
| Wheel y offset | `0.2625` m |
| Front and rear casters | Fixed caster links. |
| IMU link | Fixed to `base_link`. |

The model uses `base_footprint` as the ground aligned base frame and `base_link` as the physical body frame.

### 18.3 LiDAR frames

`lidar.xacro` defines front and rear LiDAR frames:

| Frame | Position | Direction |
| --- | --- | --- |
| `laser_front_frame` | `x = 0.35`, `z = 0.10` | Rotated by pi around yaw. |
| `laser_rear_frame` | `x = -0.35`, `z = 0.10` | Forward yaw. |

The Gazebo simulation blocks publish `scan` and `scan2` when simulation is used.

## 19. CANopen Object and Frame Reference

### 19.1 Base wheel frame map

| Frame or object | Direction | Meaning |
| --- | --- | --- |
| `0x000` | TX | NMT command. |
| `0x600 + motor_id` | TX | SDO request to motor. |
| `0x580 + motor_id` | RX | SDO response from motor. |
| `0x170 + motor_id` | TX | Base wheel RPDO target velocity. |
| `0x180 + motor_id` | RX | Base wheel TPDO velocity and statusword. |
| `0x700 + motor_id` | RX | Heartbeat or bootup. |

### 19.2 Lift frame map

| Frame or object | Direction | Meaning |
| --- | --- | --- |
| `0x203` | TX | Lift RPDO target velocity from config. |
| `0x183` | RX | Lift TPDO measured velocity and statusword from config. |
| `0x603` | TX | SDO request to lift Node 3. |
| `0x583` | RX | SDO response from lift Node 3. |
| `0x703` | RX | Lift heartbeat or bootup. |

### 19.3 Frequently used object dictionary entries

| Object | Meaning |
| --- | --- |
| `0x6040:00` | Controlword. Used for fault reset, shutdown, switch on and enable operation. |
| `0x6041:00` | Statusword. Used to detect operation enabled and fault state. |
| `0x6060:00` | Mode of operation. Set to Profile Velocity mode. |
| `0x603F:00` | Error code. Used for diagnostics. |
| `0x1001:00` | Error register. Used for diagnostics. |
| `0x60FD:00` | Digital inputs. Used by diagnostics and lift limit guard. |
| `0x2010:01` | Kinco input polarity or function related field used in reset logic. |
| `0x2010:03` | Kinco DIN1 function. Used to diagnose or clear physical enable dependency. |
| `0x2010:0A` | Kinco input status. Used for DIN1 diagnostics. |

## 20. Custom Message

`MotorFeedback.msg` defines a feedback message containing:

1. Header.
2. Right wheel position, velocity, command and statusword.
3. Left wheel position, velocity, command and statusword.

This message is generated by the build system. In the current source code, the base hardware interface does not publish this message. The closest active base status output is `motor_heartbeat`. If detailed base motor telemetry is required in the future, this message should be connected to a publisher inside `KincoHardwareInterface::read()` after TPDO decoding.

## 21. Diagnostics and Debugging

### 21.1 Check CAN interface

```bash
ip link show can0
```

Expected result: `can0` exists and is `UP`.

### 21.2 Watch CAN traffic

```bash
candump can0
```

Useful filtered views:

```bash
candump can0 | grep 181
candump can0 | grep 182
candump can0 | grep 183
candump can0 | grep 581
candump can0 | grep 582
candump can0 | grep 583
```

### 21.3 Check controller state

```bash
ros2 control list_controllers
ros2 control list_hardware_interfaces
ros2 topic echo /wheel_controller/odom
ros2 topic echo /joint_states
```

### 21.4 Check base readiness

```bash
ros2 topic echo /motor_heartbeat
```

The heartbeat should publish `true` only when the base state machine reaches `BASE_CONTROL` and both wheel drives are operation enabled.

### 21.5 Check lift state

```bash
ros2 topic echo /lift/enabled
ros2 topic echo /lift/statusword
ros2 topic echo /lift/digital_inputs
ros2 topic echo /lift/status
ros2 topic echo /lift/state_rpm
```

### 21.6 Send lift command

First allow the master gate:

```bash
ros2 topic pub /allowed_master_on std_msgs/msg/Bool "{data: true}" --once
```

Then send a lift command:

```bash
ros2 topic pub /lift/cmd_rpm std_msgs/msg/Int32 "{data: 200}" --rate 10
```

Stop the lift:

```bash
ros2 topic pub /lift/cmd_rpm std_msgs/msg/Int32 "{data: 0}" --once
```

### 21.7 Common failure cases

| Symptom | Likely cause | What to check |
| --- | --- | --- |
| Hardware interface cannot configure | CAN socket failed or `can0` missing | `ip link show can0` and bitrate setup. |
| Motors never reach operation enabled | Drive fault, wrong mode, inactive DIN enable or missing TPDO feedback | Statusword, error code, digital inputs and DIN1 diagnostics. |
| CAN send `ENOBUFS` warning | CAN bus or kernel TX queue overloaded | Reduce command traffic, check bitrate and physical bus health. |
| Robot does not move but controller is active | Hardware state is not `BASE_CONTROL` | Check logs and `motor_heartbeat`. |
| Odometry moves incorrectly | Wheel signs, radius or separation mismatch | Verify wheel direction, `wheel_radius`, `wheel_separation` and motor ID mapping. |
| Lift ignores command | `/allowed_master_on` false, drive not enabled or active limit switch | Check `/lift/enabled`, `/allowed_master_on` and `/lift/digital_inputs`. |
| Lift stops near a limit | Limit guard is active | Confirm limit bit mapping and polarity in `lift_node3.yaml`. |

## 22. Validation Checklist

Use this checklist after flashing, wiring or changing configuration.

### 22.1 CAN layer

1. `can0` is up.
2. `candump can0` shows frames from Node 1, Node 2 and Node 3 when powered.
3. SDO responses appear at `0x581`, `0x582` and `0x583` when requests are sent.
4. TPDO feedback appears at `0x181`, `0x182` and `0x183`.

### 22.2 Base control layer

1. `ros2 launch next_motor_interface robot_control.launch.py` starts without plugin load errors.
2. `ros2 control list_controllers` shows `joint_state_broadcaster` and `wheel_controller` active.
3. `/joint_states` publishes wheel state.
4. `/wheel_controller/odom` publishes odometry.
5. `/motor_heartbeat` becomes true after the drives are stable.
6. A small teleop command moves the robot in the correct direction.
7. A zero command stops both wheels.

### 22.3 Lift layer

1. `ros2 launch next_motor_interface lift_node3.launch.py` starts successfully.
2. `/lift/statusword` updates.
3. `/lift/enabled` becomes true.
4. `/lift/digital_inputs` changes when limit switches are activated.
5. `/allowed_master_on` false forces output command to zero.
6. The lift stops when the active limit is reached.
7. Measured direction in `/lift/status` matches actual motion.

### 22.4 Navigation layer

1. `/scan` exists.
2. `/tf` contains `map`, `odom`, `base_footprint`, `base_link` and sensor frames.
3. SLAM Toolbox can build or localize against a map.
4. Nav2 lifecycle nodes become active.
5. `/cmd_vel` from Nav2 is relayed to `/wheel_controller/cmd_vel_unstamped`.
6. The robot follows a short goal in RViz without controller timeout.

## 23. Known Design Notes

### 23.1 Active custom driver versus ros2_canopen config

The repository includes `bus_can0/bus.yml` for `ros2_canopen` style configuration, but the active robot model uses the custom plugin `next_motor_interface/KincoHardwareInterface`. This means the wheel drive path currently relies on the custom raw SocketCAN driver rather than using the `ros2_canopen` Cia402 driver at runtime.

### 23.2 `MotorFeedback.msg` is generated but not published

The package defines a useful detailed motor feedback message. The current implementation does not publish it. Adding this publisher would make diagnostics cleaner and reduce the need to parse logs or raw CAN traffic.

### 23.3 Base wheel position is integrated

The hardware interface integrates position from measured velocity rather than reading absolute position from the drive. This is acceptable for wheel odometry when velocity feedback is stable, but it can accumulate error over time.

### 23.4 Lift is not a `ros2_control` joint

The lift is controlled by a standalone node. This is simple and practical, but it means lift state is not visible through standard `ros2_control` joint interfaces.

### 23.5 Launch argument mismatch to watch

`robot_control.launch.py` passes `bus_config` and `master_config` xacro arguments, but the active `ros2_control.xacro` shown in this repository uses only the custom hardware plugin and `can_interface_name`. If those arguments are not used elsewhere, they are harmless but should be cleaned or wired intentionally.

## 24. Suggested Future Improvements

1. Publish `MotorFeedback.msg` from the base hardware interface.
2. Add a service to reset base motor faults manually.
3. Add a service to query current base state machine state.
4. Expose detailed statusword decoding as ROS diagnostics.
5. Add launch level parameter for CAN interface name instead of hardcoding `can0` in xacro.
6. Add integration tests with virtual CAN using `vcan0`.
7. Add a safety watchdog that commands zero if no valid controller update arrives.
8. Move Kinco conversion constants into parameters so gear ratio and encoder scaling can be tuned without recompilation.
9. Convert the lift actuator into a `ros2_control` hardware interface if future applications need unified actuator control.
10. Add a small CLI diagnostic tool for SDO reads, statusword decoding and digital input inspection.

## 25. Glossary

| Term | Meaning |
| --- | --- |
| CAN | Controller Area Network, the physical and link layer used by the robot drives. |
| SocketCAN | Linux CAN interface used to read and write CAN frames from userspace. |
| CANopen | Higher level protocol on top of CAN used by the Kinco drives. |
| NMT | Network Management, used to reset or start CANopen nodes. |
| SDO | Service Data Object, used for object dictionary reads and writes. |
| PDO | Process Data Object, used for fast cyclic command or feedback. |
| RPDO | Receive PDO from the drive perspective, used here for target velocity commands. |
| TPDO | Transmit PDO from the drive perspective, used here for measured velocity and statusword. |
| CiA 402 | CANopen device profile for drives and motion control. |
| Controlword | CiA 402 command word at object `0x6040`. |
| Statusword | CiA 402 state feedback word at object `0x6041`. |
| Operation enabled | Drive state where runtime motion commands are accepted. |
| `ros2_control` | ROS 2 framework that connects controllers to hardware interfaces. |
| Diff drive controller | ROS 2 controller that converts linear and angular velocity into left and right wheel commands. |
| DWB | Dynamic Window Based local planner used by Nav2. |
| Navfn | Grid based global planner used by Nav2. |
| SLAM Toolbox | ROS 2 package used here for mapping and localization. |
| URDF | Robot description format used for geometry, joints, links and transforms. |
| xacro | Macro language used to generate URDF files. |

## 26. One Page Flow Summary

```text
Base motion

Nav2 or teleop command
  ↓
/wheel_controller/cmd_vel_unstamped
  ↓
diff_drive_controller computes left and right wheel joint velocity
  ↓
ros2_control calls KincoHardwareInterface::write()
  ↓
write() checks BASE_CONTROL
  ↓
rad per second command becomes motor RPM
  ↓
motor RPM becomes Kinco object units
  ↓
RPDO frame is sent to CAN ID 0x171 or 0x172
  ↓
Kinco drive moves wheel
  ↓
TPDO frame returns velocity and statusword on 0x181 or 0x182
  ↓
read() decodes feedback, updates velocity and integrates position
  ↓
diff_drive_controller publishes odometry
```

```text
Lift motion

/lift/cmd_rpm plus /allowed_master_on
  ↓
kinco_lift_node3_std checks master permission
  ↓
node checks drive enabled state
  ↓
node checks limit switch guard
  ↓
RPM command becomes Kinco object units
  ↓
RPDO frame is sent to 0x203
  ↓
TPDO feedback returns on 0x183
  ↓
node publishes RPM, statusword, digital inputs, enabled flag and packed status
```

## 27. Final Notes

This repository is best understood as a hardware bringup and control bridge. The most important custom logic is not the Nav2 configuration or URDF model. The most important custom logic is the Kinco CANopen integration layer that turns ROS 2 controller calls into safe drive commands while managing CiA 402 state, CAN filtering, velocity scaling, fault recovery, heartbeat readiness and lift limit protection.

For a GitHub Wiki, this page can serve as the main technical reference. A good follow up structure would be separate pages for base motor bringup, lift actuator bringup, CAN diagnostics, Nav2 configuration and troubleshooting.
