// generated from rosidl_generator_cpp/resource/idl__traits.hpp.em
// with input from next_motor_interface:msg/MotorFeedback.idl
// generated code does not contain a copyright notice

#ifndef NEXT_MOTOR_INTERFACE__MSG__DETAIL__MOTOR_FEEDBACK__TRAITS_HPP_
#define NEXT_MOTOR_INTERFACE__MSG__DETAIL__MOTOR_FEEDBACK__TRAITS_HPP_

#include <stdint.h>

#include <sstream>
#include <string>
#include <type_traits>

#include "next_motor_interface/msg/detail/motor_feedback__struct.hpp"
#include "rosidl_runtime_cpp/traits.hpp"

// Include directives for member types
// Member 'header'
#include "std_msgs/msg/detail/header__traits.hpp"

namespace next_motor_interface
{

namespace msg
{

inline void to_flow_style_yaml(
  const MotorFeedback & msg,
  std::ostream & out)
{
  out << "{";
  // member: header
  {
    out << "header: ";
    to_flow_style_yaml(msg.header, out);
    out << ", ";
  }

  // member: right_wheel_position_rad
  {
    out << "right_wheel_position_rad: ";
    rosidl_generator_traits::value_to_yaml(msg.right_wheel_position_rad, out);
    out << ", ";
  }

  // member: right_wheel_velocity_rad_s
  {
    out << "right_wheel_velocity_rad_s: ";
    rosidl_generator_traits::value_to_yaml(msg.right_wheel_velocity_rad_s, out);
    out << ", ";
  }

  // member: right_wheel_command_rad_s
  {
    out << "right_wheel_command_rad_s: ";
    rosidl_generator_traits::value_to_yaml(msg.right_wheel_command_rad_s, out);
    out << ", ";
  }

  // member: right_wheel_statusword
  {
    out << "right_wheel_statusword: ";
    rosidl_generator_traits::value_to_yaml(msg.right_wheel_statusword, out);
    out << ", ";
  }

  // member: left_wheel_position_rad
  {
    out << "left_wheel_position_rad: ";
    rosidl_generator_traits::value_to_yaml(msg.left_wheel_position_rad, out);
    out << ", ";
  }

  // member: left_wheel_velocity_rad_s
  {
    out << "left_wheel_velocity_rad_s: ";
    rosidl_generator_traits::value_to_yaml(msg.left_wheel_velocity_rad_s, out);
    out << ", ";
  }

  // member: left_wheel_command_rad_s
  {
    out << "left_wheel_command_rad_s: ";
    rosidl_generator_traits::value_to_yaml(msg.left_wheel_command_rad_s, out);
    out << ", ";
  }

  // member: left_wheel_statusword
  {
    out << "left_wheel_statusword: ";
    rosidl_generator_traits::value_to_yaml(msg.left_wheel_statusword, out);
  }
  out << "}";
}  // NOLINT(readability/fn_size)

inline void to_block_style_yaml(
  const MotorFeedback & msg,
  std::ostream & out, size_t indentation = 0)
{
  // member: header
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "header:\n";
    to_block_style_yaml(msg.header, out, indentation + 2);
  }

  // member: right_wheel_position_rad
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "right_wheel_position_rad: ";
    rosidl_generator_traits::value_to_yaml(msg.right_wheel_position_rad, out);
    out << "\n";
  }

  // member: right_wheel_velocity_rad_s
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "right_wheel_velocity_rad_s: ";
    rosidl_generator_traits::value_to_yaml(msg.right_wheel_velocity_rad_s, out);
    out << "\n";
  }

  // member: right_wheel_command_rad_s
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "right_wheel_command_rad_s: ";
    rosidl_generator_traits::value_to_yaml(msg.right_wheel_command_rad_s, out);
    out << "\n";
  }

  // member: right_wheel_statusword
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "right_wheel_statusword: ";
    rosidl_generator_traits::value_to_yaml(msg.right_wheel_statusword, out);
    out << "\n";
  }

  // member: left_wheel_position_rad
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "left_wheel_position_rad: ";
    rosidl_generator_traits::value_to_yaml(msg.left_wheel_position_rad, out);
    out << "\n";
  }

  // member: left_wheel_velocity_rad_s
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "left_wheel_velocity_rad_s: ";
    rosidl_generator_traits::value_to_yaml(msg.left_wheel_velocity_rad_s, out);
    out << "\n";
  }

  // member: left_wheel_command_rad_s
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "left_wheel_command_rad_s: ";
    rosidl_generator_traits::value_to_yaml(msg.left_wheel_command_rad_s, out);
    out << "\n";
  }

  // member: left_wheel_statusword
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "left_wheel_statusword: ";
    rosidl_generator_traits::value_to_yaml(msg.left_wheel_statusword, out);
    out << "\n";
  }
}  // NOLINT(readability/fn_size)

inline std::string to_yaml(const MotorFeedback & msg, bool use_flow_style = false)
{
  std::ostringstream out;
  if (use_flow_style) {
    to_flow_style_yaml(msg, out);
  } else {
    to_block_style_yaml(msg, out);
  }
  return out.str();
}

}  // namespace msg

}  // namespace next_motor_interface

namespace rosidl_generator_traits
{

[[deprecated("use next_motor_interface::msg::to_block_style_yaml() instead")]]
inline void to_yaml(
  const next_motor_interface::msg::MotorFeedback & msg,
  std::ostream & out, size_t indentation = 0)
{
  next_motor_interface::msg::to_block_style_yaml(msg, out, indentation);
}

[[deprecated("use next_motor_interface::msg::to_yaml() instead")]]
inline std::string to_yaml(const next_motor_interface::msg::MotorFeedback & msg)
{
  return next_motor_interface::msg::to_yaml(msg);
}

template<>
inline const char * data_type<next_motor_interface::msg::MotorFeedback>()
{
  return "next_motor_interface::msg::MotorFeedback";
}

template<>
inline const char * name<next_motor_interface::msg::MotorFeedback>()
{
  return "next_motor_interface/msg/MotorFeedback";
}

template<>
struct has_fixed_size<next_motor_interface::msg::MotorFeedback>
  : std::integral_constant<bool, has_fixed_size<std_msgs::msg::Header>::value> {};

template<>
struct has_bounded_size<next_motor_interface::msg::MotorFeedback>
  : std::integral_constant<bool, has_bounded_size<std_msgs::msg::Header>::value> {};

template<>
struct is_message<next_motor_interface::msg::MotorFeedback>
  : std::true_type {};

}  // namespace rosidl_generator_traits

#endif  // NEXT_MOTOR_INTERFACE__MSG__DETAIL__MOTOR_FEEDBACK__TRAITS_HPP_
