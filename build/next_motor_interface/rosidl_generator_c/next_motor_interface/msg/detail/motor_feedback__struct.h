// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from next_motor_interface:msg/MotorFeedback.idl
// generated code does not contain a copyright notice

#ifndef NEXT_MOTOR_INTERFACE__MSG__DETAIL__MOTOR_FEEDBACK__STRUCT_H_
#define NEXT_MOTOR_INTERFACE__MSG__DETAIL__MOTOR_FEEDBACK__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


// Constants defined in the message

// Include directives for member types
// Member 'header'
#include "std_msgs/msg/detail/header__struct.h"

/// Struct defined in msg/MotorFeedback in the package next_motor_interface.
/**
  * Motor feedback message for Kinco drives
  * Published at controller update rate (typically 100 Hz)
 */
typedef struct next_motor_interface__msg__MotorFeedback
{
  std_msgs__msg__Header header;
  /// Right wheel (Motor ID 1)
  double right_wheel_position_rad;
  double right_wheel_velocity_rad_s;
  double right_wheel_command_rad_s;
  uint16_t right_wheel_statusword;
  /// Left wheel (Motor ID 2)
  double left_wheel_position_rad;
  double left_wheel_velocity_rad_s;
  double left_wheel_command_rad_s;
  uint16_t left_wheel_statusword;
} next_motor_interface__msg__MotorFeedback;

// Struct for a sequence of next_motor_interface__msg__MotorFeedback.
typedef struct next_motor_interface__msg__MotorFeedback__Sequence
{
  next_motor_interface__msg__MotorFeedback * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} next_motor_interface__msg__MotorFeedback__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // NEXT_MOTOR_INTERFACE__MSG__DETAIL__MOTOR_FEEDBACK__STRUCT_H_
