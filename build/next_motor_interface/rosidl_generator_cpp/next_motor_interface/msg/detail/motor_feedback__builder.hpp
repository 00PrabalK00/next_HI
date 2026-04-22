// generated from rosidl_generator_cpp/resource/idl__builder.hpp.em
// with input from next_motor_interface:msg/MotorFeedback.idl
// generated code does not contain a copyright notice

#ifndef NEXT_MOTOR_INTERFACE__MSG__DETAIL__MOTOR_FEEDBACK__BUILDER_HPP_
#define NEXT_MOTOR_INTERFACE__MSG__DETAIL__MOTOR_FEEDBACK__BUILDER_HPP_

#include <algorithm>
#include <utility>

#include "next_motor_interface/msg/detail/motor_feedback__struct.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


namespace next_motor_interface
{

namespace msg
{

namespace builder
{

class Init_MotorFeedback_left_wheel_statusword
{
public:
  explicit Init_MotorFeedback_left_wheel_statusword(::next_motor_interface::msg::MotorFeedback & msg)
  : msg_(msg)
  {}
  ::next_motor_interface::msg::MotorFeedback left_wheel_statusword(::next_motor_interface::msg::MotorFeedback::_left_wheel_statusword_type arg)
  {
    msg_.left_wheel_statusword = std::move(arg);
    return std::move(msg_);
  }

private:
  ::next_motor_interface::msg::MotorFeedback msg_;
};

class Init_MotorFeedback_left_wheel_command_rad_s
{
public:
  explicit Init_MotorFeedback_left_wheel_command_rad_s(::next_motor_interface::msg::MotorFeedback & msg)
  : msg_(msg)
  {}
  Init_MotorFeedback_left_wheel_statusword left_wheel_command_rad_s(::next_motor_interface::msg::MotorFeedback::_left_wheel_command_rad_s_type arg)
  {
    msg_.left_wheel_command_rad_s = std::move(arg);
    return Init_MotorFeedback_left_wheel_statusword(msg_);
  }

private:
  ::next_motor_interface::msg::MotorFeedback msg_;
};

class Init_MotorFeedback_left_wheel_velocity_rad_s
{
public:
  explicit Init_MotorFeedback_left_wheel_velocity_rad_s(::next_motor_interface::msg::MotorFeedback & msg)
  : msg_(msg)
  {}
  Init_MotorFeedback_left_wheel_command_rad_s left_wheel_velocity_rad_s(::next_motor_interface::msg::MotorFeedback::_left_wheel_velocity_rad_s_type arg)
  {
    msg_.left_wheel_velocity_rad_s = std::move(arg);
    return Init_MotorFeedback_left_wheel_command_rad_s(msg_);
  }

private:
  ::next_motor_interface::msg::MotorFeedback msg_;
};

class Init_MotorFeedback_left_wheel_position_rad
{
public:
  explicit Init_MotorFeedback_left_wheel_position_rad(::next_motor_interface::msg::MotorFeedback & msg)
  : msg_(msg)
  {}
  Init_MotorFeedback_left_wheel_velocity_rad_s left_wheel_position_rad(::next_motor_interface::msg::MotorFeedback::_left_wheel_position_rad_type arg)
  {
    msg_.left_wheel_position_rad = std::move(arg);
    return Init_MotorFeedback_left_wheel_velocity_rad_s(msg_);
  }

private:
  ::next_motor_interface::msg::MotorFeedback msg_;
};

class Init_MotorFeedback_right_wheel_statusword
{
public:
  explicit Init_MotorFeedback_right_wheel_statusword(::next_motor_interface::msg::MotorFeedback & msg)
  : msg_(msg)
  {}
  Init_MotorFeedback_left_wheel_position_rad right_wheel_statusword(::next_motor_interface::msg::MotorFeedback::_right_wheel_statusword_type arg)
  {
    msg_.right_wheel_statusword = std::move(arg);
    return Init_MotorFeedback_left_wheel_position_rad(msg_);
  }

private:
  ::next_motor_interface::msg::MotorFeedback msg_;
};

class Init_MotorFeedback_right_wheel_command_rad_s
{
public:
  explicit Init_MotorFeedback_right_wheel_command_rad_s(::next_motor_interface::msg::MotorFeedback & msg)
  : msg_(msg)
  {}
  Init_MotorFeedback_right_wheel_statusword right_wheel_command_rad_s(::next_motor_interface::msg::MotorFeedback::_right_wheel_command_rad_s_type arg)
  {
    msg_.right_wheel_command_rad_s = std::move(arg);
    return Init_MotorFeedback_right_wheel_statusword(msg_);
  }

private:
  ::next_motor_interface::msg::MotorFeedback msg_;
};

class Init_MotorFeedback_right_wheel_velocity_rad_s
{
public:
  explicit Init_MotorFeedback_right_wheel_velocity_rad_s(::next_motor_interface::msg::MotorFeedback & msg)
  : msg_(msg)
  {}
  Init_MotorFeedback_right_wheel_command_rad_s right_wheel_velocity_rad_s(::next_motor_interface::msg::MotorFeedback::_right_wheel_velocity_rad_s_type arg)
  {
    msg_.right_wheel_velocity_rad_s = std::move(arg);
    return Init_MotorFeedback_right_wheel_command_rad_s(msg_);
  }

private:
  ::next_motor_interface::msg::MotorFeedback msg_;
};

class Init_MotorFeedback_right_wheel_position_rad
{
public:
  explicit Init_MotorFeedback_right_wheel_position_rad(::next_motor_interface::msg::MotorFeedback & msg)
  : msg_(msg)
  {}
  Init_MotorFeedback_right_wheel_velocity_rad_s right_wheel_position_rad(::next_motor_interface::msg::MotorFeedback::_right_wheel_position_rad_type arg)
  {
    msg_.right_wheel_position_rad = std::move(arg);
    return Init_MotorFeedback_right_wheel_velocity_rad_s(msg_);
  }

private:
  ::next_motor_interface::msg::MotorFeedback msg_;
};

class Init_MotorFeedback_header
{
public:
  Init_MotorFeedback_header()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_MotorFeedback_right_wheel_position_rad header(::next_motor_interface::msg::MotorFeedback::_header_type arg)
  {
    msg_.header = std::move(arg);
    return Init_MotorFeedback_right_wheel_position_rad(msg_);
  }

private:
  ::next_motor_interface::msg::MotorFeedback msg_;
};

}  // namespace builder

}  // namespace msg

template<typename MessageType>
auto build();

template<>
inline
auto build<::next_motor_interface::msg::MotorFeedback>()
{
  return next_motor_interface::msg::builder::Init_MotorFeedback_header();
}

}  // namespace next_motor_interface

#endif  // NEXT_MOTOR_INTERFACE__MSG__DETAIL__MOTOR_FEEDBACK__BUILDER_HPP_
