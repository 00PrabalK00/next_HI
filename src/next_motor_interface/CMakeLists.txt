cmake_minimum_required(VERSION 3.8)
project(next_motor_interface)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(ament_cmake REQUIRED)
find_package(rosidl_default_generators REQUIRED)
find_package(std_msgs REQUIRED)
find_package(lely_core_libraries REQUIRED)
find_package(hardware_interface REQUIRED)
find_package(pluginlib REQUIRED)
find_package(rclcpp REQUIRED)
find_package(rclcpp_lifecycle REQUIRED)

# Messages for your motor interface package (keep as-is)
rosidl_generate_interfaces(${PROJECT_NAME}
  "msg/MotorFeedback.msg"
  DEPENDENCIES std_msgs
)

# Hardware interface plugin (keep as-is)
add_library(kinco_hardware_interface SHARED
  src/kinco_hardware_interface.cpp
)

target_include_directories(kinco_hardware_interface PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

ament_target_dependencies(kinco_hardware_interface
  hardware_interface
  pluginlib
  rclcpp
  rclcpp_lifecycle
  std_msgs
)

rosidl_target_interfaces(kinco_hardware_interface
  ${PROJECT_NAME} "rosidl_typesupport_cpp"
)

pluginlib_export_plugin_description_file(hardware_interface kinco_hardware_interface.xml)

install(TARGETS kinco_hardware_interface
  DESTINATION lib
)

# Install package resources (keep as-is)
install(
  DIRECTORY launch config urdf maps rviz scripts
  DESTINATION share/${PROJECT_NAME}
)

# ------------------------------------------------------------
# Lift Node 3 Driver (std_msgs only)
# ------------------------------------------------------------
add_executable(kinco_lift_node3_std src/kinco_lift_node3_std.cpp)
ament_target_dependencies(kinco_lift_node3_std rclcpp std_msgs)
install(TARGETS kinco_lift_node3_std
  DESTINATION lib/${PROJECT_NAME}
)

ament_package()
