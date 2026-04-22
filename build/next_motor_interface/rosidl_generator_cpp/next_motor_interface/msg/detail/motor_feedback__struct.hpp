// generated from rosidl_generator_cpp/resource/idl__struct.hpp.em
// with input from next_motor_interface:msg/MotorFeedback.idl
// generated code does not contain a copyright notice

#ifndef NEXT_MOTOR_INTERFACE__MSG__DETAIL__MOTOR_FEEDBACK__STRUCT_HPP_
#define NEXT_MOTOR_INTERFACE__MSG__DETAIL__MOTOR_FEEDBACK__STRUCT_HPP_

#include <algorithm>
#include <array>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "rosidl_runtime_cpp/bounded_vector.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


// Include directives for member types
// Member 'header'
#include "std_msgs/msg/detail/header__struct.hpp"

#ifndef _WIN32
# define DEPRECATED__next_motor_interface__msg__MotorFeedback __attribute__((deprecated))
#else
# define DEPRECATED__next_motor_interface__msg__MotorFeedback __declspec(deprecated)
#endif

namespace next_motor_interface
{

namespace msg
{

// message struct
template<class ContainerAllocator>
struct MotorFeedback_
{
  using Type = MotorFeedback_<ContainerAllocator>;

  explicit MotorFeedback_(rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  : header(_init)
  {
    if (rosidl_runtime_cpp::MessageInitialization::ALL == _init ||
      rosidl_runtime_cpp::MessageInitialization::ZERO == _init)
    {
      this->right_wheel_position_rad = 0.0;
      this->right_wheel_velocity_rad_s = 0.0;
      this->right_wheel_command_rad_s = 0.0;
      this->right_wheel_statusword = 0;
      this->left_wheel_position_rad = 0.0;
      this->left_wheel_velocity_rad_s = 0.0;
      this->left_wheel_command_rad_s = 0.0;
      this->left_wheel_statusword = 0;
    }
  }

  explicit MotorFeedback_(const ContainerAllocator & _alloc, rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  : header(_alloc, _init)
  {
    if (rosidl_runtime_cpp::MessageInitialization::ALL == _init ||
      rosidl_runtime_cpp::MessageInitialization::ZERO == _init)
    {
      this->right_wheel_position_rad = 0.0;
      this->right_wheel_velocity_rad_s = 0.0;
      this->right_wheel_command_rad_s = 0.0;
      this->right_wheel_statusword = 0;
      this->left_wheel_position_rad = 0.0;
      this->left_wheel_velocity_rad_s = 0.0;
      this->left_wheel_command_rad_s = 0.0;
      this->left_wheel_statusword = 0;
    }
  }

  // field types and members
  using _header_type =
    std_msgs::msg::Header_<ContainerAllocator>;
  _header_type header;
  using _right_wheel_position_rad_type =
    double;
  _right_wheel_position_rad_type right_wheel_position_rad;
  using _right_wheel_velocity_rad_s_type =
    double;
  _right_wheel_velocity_rad_s_type right_wheel_velocity_rad_s;
  using _right_wheel_command_rad_s_type =
    double;
  _right_wheel_command_rad_s_type right_wheel_command_rad_s;
  using _right_wheel_statusword_type =
    uint16_t;
  _right_wheel_statusword_type right_wheel_statusword;
  using _left_wheel_position_rad_type =
    double;
  _left_wheel_position_rad_type left_wheel_position_rad;
  using _left_wheel_velocity_rad_s_type =
    double;
  _left_wheel_velocity_rad_s_type left_wheel_velocity_rad_s;
  using _left_wheel_command_rad_s_type =
    double;
  _left_wheel_command_rad_s_type left_wheel_command_rad_s;
  using _left_wheel_statusword_type =
    uint16_t;
  _left_wheel_statusword_type left_wheel_statusword;

  // setters for named parameter idiom
  Type & set__header(
    const std_msgs::msg::Header_<ContainerAllocator> & _arg)
  {
    this->header = _arg;
    return *this;
  }
  Type & set__right_wheel_position_rad(
    const double & _arg)
  {
    this->right_wheel_position_rad = _arg;
    return *this;
  }
  Type & set__right_wheel_velocity_rad_s(
    const double & _arg)
  {
    this->right_wheel_velocity_rad_s = _arg;
    return *this;
  }
  Type & set__right_wheel_command_rad_s(
    const double & _arg)
  {
    this->right_wheel_command_rad_s = _arg;
    return *this;
  }
  Type & set__right_wheel_statusword(
    const uint16_t & _arg)
  {
    this->right_wheel_statusword = _arg;
    return *this;
  }
  Type & set__left_wheel_position_rad(
    const double & _arg)
  {
    this->left_wheel_position_rad = _arg;
    return *this;
  }
  Type & set__left_wheel_velocity_rad_s(
    const double & _arg)
  {
    this->left_wheel_velocity_rad_s = _arg;
    return *this;
  }
  Type & set__left_wheel_command_rad_s(
    const double & _arg)
  {
    this->left_wheel_command_rad_s = _arg;
    return *this;
  }
  Type & set__left_wheel_statusword(
    const uint16_t & _arg)
  {
    this->left_wheel_statusword = _arg;
    return *this;
  }

  // constant declarations

  // pointer types
  using RawPtr =
    next_motor_interface::msg::MotorFeedback_<ContainerAllocator> *;
  using ConstRawPtr =
    const next_motor_interface::msg::MotorFeedback_<ContainerAllocator> *;
  using SharedPtr =
    std::shared_ptr<next_motor_interface::msg::MotorFeedback_<ContainerAllocator>>;
  using ConstSharedPtr =
    std::shared_ptr<next_motor_interface::msg::MotorFeedback_<ContainerAllocator> const>;

  template<typename Deleter = std::default_delete<
      next_motor_interface::msg::MotorFeedback_<ContainerAllocator>>>
  using UniquePtrWithDeleter =
    std::unique_ptr<next_motor_interface::msg::MotorFeedback_<ContainerAllocator>, Deleter>;

  using UniquePtr = UniquePtrWithDeleter<>;

  template<typename Deleter = std::default_delete<
      next_motor_interface::msg::MotorFeedback_<ContainerAllocator>>>
  using ConstUniquePtrWithDeleter =
    std::unique_ptr<next_motor_interface::msg::MotorFeedback_<ContainerAllocator> const, Deleter>;
  using ConstUniquePtr = ConstUniquePtrWithDeleter<>;

  using WeakPtr =
    std::weak_ptr<next_motor_interface::msg::MotorFeedback_<ContainerAllocator>>;
  using ConstWeakPtr =
    std::weak_ptr<next_motor_interface::msg::MotorFeedback_<ContainerAllocator> const>;

  // pointer types similar to ROS 1, use SharedPtr / ConstSharedPtr instead
  // NOTE: Can't use 'using' here because GNU C++ can't parse attributes properly
  typedef DEPRECATED__next_motor_interface__msg__MotorFeedback
    std::shared_ptr<next_motor_interface::msg::MotorFeedback_<ContainerAllocator>>
    Ptr;
  typedef DEPRECATED__next_motor_interface__msg__MotorFeedback
    std::shared_ptr<next_motor_interface::msg::MotorFeedback_<ContainerAllocator> const>
    ConstPtr;

  // comparison operators
  bool operator==(const MotorFeedback_ & other) const
  {
    if (this->header != other.header) {
      return false;
    }
    if (this->right_wheel_position_rad != other.right_wheel_position_rad) {
      return false;
    }
    if (this->right_wheel_velocity_rad_s != other.right_wheel_velocity_rad_s) {
      return false;
    }
    if (this->right_wheel_command_rad_s != other.right_wheel_command_rad_s) {
      return false;
    }
    if (this->right_wheel_statusword != other.right_wheel_statusword) {
      return false;
    }
    if (this->left_wheel_position_rad != other.left_wheel_position_rad) {
      return false;
    }
    if (this->left_wheel_velocity_rad_s != other.left_wheel_velocity_rad_s) {
      return false;
    }
    if (this->left_wheel_command_rad_s != other.left_wheel_command_rad_s) {
      return false;
    }
    if (this->left_wheel_statusword != other.left_wheel_statusword) {
      return false;
    }
    return true;
  }
  bool operator!=(const MotorFeedback_ & other) const
  {
    return !this->operator==(other);
  }
};  // struct MotorFeedback_

// alias to use template instance with default allocator
using MotorFeedback =
  next_motor_interface::msg::MotorFeedback_<std::allocator<void>>;

// constant definitions

}  // namespace msg

}  // namespace next_motor_interface

#endif  // NEXT_MOTOR_INTERFACE__MSG__DETAIL__MOTOR_FEEDBACK__STRUCT_HPP_
