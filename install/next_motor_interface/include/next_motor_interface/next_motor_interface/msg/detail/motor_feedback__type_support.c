// generated from rosidl_typesupport_introspection_c/resource/idl__type_support.c.em
// with input from next_motor_interface:msg/MotorFeedback.idl
// generated code does not contain a copyright notice

#include <stddef.h>
#include "next_motor_interface/msg/detail/motor_feedback__rosidl_typesupport_introspection_c.h"
#include "next_motor_interface/msg/rosidl_typesupport_introspection_c__visibility_control.h"
#include "rosidl_typesupport_introspection_c/field_types.h"
#include "rosidl_typesupport_introspection_c/identifier.h"
#include "rosidl_typesupport_introspection_c/message_introspection.h"
#include "next_motor_interface/msg/detail/motor_feedback__functions.h"
#include "next_motor_interface/msg/detail/motor_feedback__struct.h"


// Include directives for member types
// Member `header`
#include "std_msgs/msg/header.h"
// Member `header`
#include "std_msgs/msg/detail/header__rosidl_typesupport_introspection_c.h"

#ifdef __cplusplus
extern "C"
{
#endif

void next_motor_interface__msg__MotorFeedback__rosidl_typesupport_introspection_c__MotorFeedback_init_function(
  void * message_memory, enum rosidl_runtime_c__message_initialization _init)
{
  // TODO(karsten1987): initializers are not yet implemented for typesupport c
  // see https://github.com/ros2/ros2/issues/397
  (void) _init;
  next_motor_interface__msg__MotorFeedback__init(message_memory);
}

void next_motor_interface__msg__MotorFeedback__rosidl_typesupport_introspection_c__MotorFeedback_fini_function(void * message_memory)
{
  next_motor_interface__msg__MotorFeedback__fini(message_memory);
}

static rosidl_typesupport_introspection_c__MessageMember next_motor_interface__msg__MotorFeedback__rosidl_typesupport_introspection_c__MotorFeedback_message_member_array[9] = {
  {
    "header",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message (initialized later)
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(next_motor_interface__msg__MotorFeedback, header),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "right_wheel_position_rad",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_DOUBLE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(next_motor_interface__msg__MotorFeedback, right_wheel_position_rad),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "right_wheel_velocity_rad_s",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_DOUBLE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(next_motor_interface__msg__MotorFeedback, right_wheel_velocity_rad_s),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "right_wheel_command_rad_s",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_DOUBLE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(next_motor_interface__msg__MotorFeedback, right_wheel_command_rad_s),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "right_wheel_statusword",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_UINT16,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(next_motor_interface__msg__MotorFeedback, right_wheel_statusword),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "left_wheel_position_rad",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_DOUBLE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(next_motor_interface__msg__MotorFeedback, left_wheel_position_rad),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "left_wheel_velocity_rad_s",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_DOUBLE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(next_motor_interface__msg__MotorFeedback, left_wheel_velocity_rad_s),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "left_wheel_command_rad_s",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_DOUBLE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(next_motor_interface__msg__MotorFeedback, left_wheel_command_rad_s),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "left_wheel_statusword",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_UINT16,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(next_motor_interface__msg__MotorFeedback, left_wheel_statusword),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  }
};

static const rosidl_typesupport_introspection_c__MessageMembers next_motor_interface__msg__MotorFeedback__rosidl_typesupport_introspection_c__MotorFeedback_message_members = {
  "next_motor_interface__msg",  // message namespace
  "MotorFeedback",  // message name
  9,  // number of fields
  sizeof(next_motor_interface__msg__MotorFeedback),
  next_motor_interface__msg__MotorFeedback__rosidl_typesupport_introspection_c__MotorFeedback_message_member_array,  // message members
  next_motor_interface__msg__MotorFeedback__rosidl_typesupport_introspection_c__MotorFeedback_init_function,  // function to initialize message memory (memory has to be allocated)
  next_motor_interface__msg__MotorFeedback__rosidl_typesupport_introspection_c__MotorFeedback_fini_function  // function to terminate message instance (will not free memory)
};

// this is not const since it must be initialized on first access
// since C does not allow non-integral compile-time constants
static rosidl_message_type_support_t next_motor_interface__msg__MotorFeedback__rosidl_typesupport_introspection_c__MotorFeedback_message_type_support_handle = {
  0,
  &next_motor_interface__msg__MotorFeedback__rosidl_typesupport_introspection_c__MotorFeedback_message_members,
  get_message_typesupport_handle_function,
};

ROSIDL_TYPESUPPORT_INTROSPECTION_C_EXPORT_next_motor_interface
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, next_motor_interface, msg, MotorFeedback)() {
  next_motor_interface__msg__MotorFeedback__rosidl_typesupport_introspection_c__MotorFeedback_message_member_array[0].members_ =
    ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, std_msgs, msg, Header)();
  if (!next_motor_interface__msg__MotorFeedback__rosidl_typesupport_introspection_c__MotorFeedback_message_type_support_handle.typesupport_identifier) {
    next_motor_interface__msg__MotorFeedback__rosidl_typesupport_introspection_c__MotorFeedback_message_type_support_handle.typesupport_identifier =
      rosidl_typesupport_introspection_c__identifier;
  }
  return &next_motor_interface__msg__MotorFeedback__rosidl_typesupport_introspection_c__MotorFeedback_message_type_support_handle;
}
#ifdef __cplusplus
}
#endif
