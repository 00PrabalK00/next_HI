// generated from rosidl_generator_c/resource/idl__functions.c.em
// with input from next_motor_interface:msg/MotorFeedback.idl
// generated code does not contain a copyright notice
#include "next_motor_interface/msg/detail/motor_feedback__functions.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "rcutils/allocator.h"


// Include directives for member types
// Member `header`
#include "std_msgs/msg/detail/header__functions.h"

bool
next_motor_interface__msg__MotorFeedback__init(next_motor_interface__msg__MotorFeedback * msg)
{
  if (!msg) {
    return false;
  }
  // header
  if (!std_msgs__msg__Header__init(&msg->header)) {
    next_motor_interface__msg__MotorFeedback__fini(msg);
    return false;
  }
  // right_wheel_position_rad
  // right_wheel_velocity_rad_s
  // right_wheel_command_rad_s
  // right_wheel_statusword
  // left_wheel_position_rad
  // left_wheel_velocity_rad_s
  // left_wheel_command_rad_s
  // left_wheel_statusword
  return true;
}

void
next_motor_interface__msg__MotorFeedback__fini(next_motor_interface__msg__MotorFeedback * msg)
{
  if (!msg) {
    return;
  }
  // header
  std_msgs__msg__Header__fini(&msg->header);
  // right_wheel_position_rad
  // right_wheel_velocity_rad_s
  // right_wheel_command_rad_s
  // right_wheel_statusword
  // left_wheel_position_rad
  // left_wheel_velocity_rad_s
  // left_wheel_command_rad_s
  // left_wheel_statusword
}

bool
next_motor_interface__msg__MotorFeedback__are_equal(const next_motor_interface__msg__MotorFeedback * lhs, const next_motor_interface__msg__MotorFeedback * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  // header
  if (!std_msgs__msg__Header__are_equal(
      &(lhs->header), &(rhs->header)))
  {
    return false;
  }
  // right_wheel_position_rad
  if (lhs->right_wheel_position_rad != rhs->right_wheel_position_rad) {
    return false;
  }
  // right_wheel_velocity_rad_s
  if (lhs->right_wheel_velocity_rad_s != rhs->right_wheel_velocity_rad_s) {
    return false;
  }
  // right_wheel_command_rad_s
  if (lhs->right_wheel_command_rad_s != rhs->right_wheel_command_rad_s) {
    return false;
  }
  // right_wheel_statusword
  if (lhs->right_wheel_statusword != rhs->right_wheel_statusword) {
    return false;
  }
  // left_wheel_position_rad
  if (lhs->left_wheel_position_rad != rhs->left_wheel_position_rad) {
    return false;
  }
  // left_wheel_velocity_rad_s
  if (lhs->left_wheel_velocity_rad_s != rhs->left_wheel_velocity_rad_s) {
    return false;
  }
  // left_wheel_command_rad_s
  if (lhs->left_wheel_command_rad_s != rhs->left_wheel_command_rad_s) {
    return false;
  }
  // left_wheel_statusword
  if (lhs->left_wheel_statusword != rhs->left_wheel_statusword) {
    return false;
  }
  return true;
}

bool
next_motor_interface__msg__MotorFeedback__copy(
  const next_motor_interface__msg__MotorFeedback * input,
  next_motor_interface__msg__MotorFeedback * output)
{
  if (!input || !output) {
    return false;
  }
  // header
  if (!std_msgs__msg__Header__copy(
      &(input->header), &(output->header)))
  {
    return false;
  }
  // right_wheel_position_rad
  output->right_wheel_position_rad = input->right_wheel_position_rad;
  // right_wheel_velocity_rad_s
  output->right_wheel_velocity_rad_s = input->right_wheel_velocity_rad_s;
  // right_wheel_command_rad_s
  output->right_wheel_command_rad_s = input->right_wheel_command_rad_s;
  // right_wheel_statusword
  output->right_wheel_statusword = input->right_wheel_statusword;
  // left_wheel_position_rad
  output->left_wheel_position_rad = input->left_wheel_position_rad;
  // left_wheel_velocity_rad_s
  output->left_wheel_velocity_rad_s = input->left_wheel_velocity_rad_s;
  // left_wheel_command_rad_s
  output->left_wheel_command_rad_s = input->left_wheel_command_rad_s;
  // left_wheel_statusword
  output->left_wheel_statusword = input->left_wheel_statusword;
  return true;
}

next_motor_interface__msg__MotorFeedback *
next_motor_interface__msg__MotorFeedback__create()
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  next_motor_interface__msg__MotorFeedback * msg = (next_motor_interface__msg__MotorFeedback *)allocator.allocate(sizeof(next_motor_interface__msg__MotorFeedback), allocator.state);
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(next_motor_interface__msg__MotorFeedback));
  bool success = next_motor_interface__msg__MotorFeedback__init(msg);
  if (!success) {
    allocator.deallocate(msg, allocator.state);
    return NULL;
  }
  return msg;
}

void
next_motor_interface__msg__MotorFeedback__destroy(next_motor_interface__msg__MotorFeedback * msg)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (msg) {
    next_motor_interface__msg__MotorFeedback__fini(msg);
  }
  allocator.deallocate(msg, allocator.state);
}


bool
next_motor_interface__msg__MotorFeedback__Sequence__init(next_motor_interface__msg__MotorFeedback__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  next_motor_interface__msg__MotorFeedback * data = NULL;

  if (size) {
    data = (next_motor_interface__msg__MotorFeedback *)allocator.zero_allocate(size, sizeof(next_motor_interface__msg__MotorFeedback), allocator.state);
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = next_motor_interface__msg__MotorFeedback__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        next_motor_interface__msg__MotorFeedback__fini(&data[i - 1]);
      }
      allocator.deallocate(data, allocator.state);
      return false;
    }
  }
  array->data = data;
  array->size = size;
  array->capacity = size;
  return true;
}

void
next_motor_interface__msg__MotorFeedback__Sequence__fini(next_motor_interface__msg__MotorFeedback__Sequence * array)
{
  if (!array) {
    return;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();

  if (array->data) {
    // ensure that data and capacity values are consistent
    assert(array->capacity > 0);
    // finalize all array elements
    for (size_t i = 0; i < array->capacity; ++i) {
      next_motor_interface__msg__MotorFeedback__fini(&array->data[i]);
    }
    allocator.deallocate(array->data, allocator.state);
    array->data = NULL;
    array->size = 0;
    array->capacity = 0;
  } else {
    // ensure that data, size, and capacity values are consistent
    assert(0 == array->size);
    assert(0 == array->capacity);
  }
}

next_motor_interface__msg__MotorFeedback__Sequence *
next_motor_interface__msg__MotorFeedback__Sequence__create(size_t size)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  next_motor_interface__msg__MotorFeedback__Sequence * array = (next_motor_interface__msg__MotorFeedback__Sequence *)allocator.allocate(sizeof(next_motor_interface__msg__MotorFeedback__Sequence), allocator.state);
  if (!array) {
    return NULL;
  }
  bool success = next_motor_interface__msg__MotorFeedback__Sequence__init(array, size);
  if (!success) {
    allocator.deallocate(array, allocator.state);
    return NULL;
  }
  return array;
}

void
next_motor_interface__msg__MotorFeedback__Sequence__destroy(next_motor_interface__msg__MotorFeedback__Sequence * array)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (array) {
    next_motor_interface__msg__MotorFeedback__Sequence__fini(array);
  }
  allocator.deallocate(array, allocator.state);
}

bool
next_motor_interface__msg__MotorFeedback__Sequence__are_equal(const next_motor_interface__msg__MotorFeedback__Sequence * lhs, const next_motor_interface__msg__MotorFeedback__Sequence * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  if (lhs->size != rhs->size) {
    return false;
  }
  for (size_t i = 0; i < lhs->size; ++i) {
    if (!next_motor_interface__msg__MotorFeedback__are_equal(&(lhs->data[i]), &(rhs->data[i]))) {
      return false;
    }
  }
  return true;
}

bool
next_motor_interface__msg__MotorFeedback__Sequence__copy(
  const next_motor_interface__msg__MotorFeedback__Sequence * input,
  next_motor_interface__msg__MotorFeedback__Sequence * output)
{
  if (!input || !output) {
    return false;
  }
  if (output->capacity < input->size) {
    const size_t allocation_size =
      input->size * sizeof(next_motor_interface__msg__MotorFeedback);
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    next_motor_interface__msg__MotorFeedback * data =
      (next_motor_interface__msg__MotorFeedback *)allocator.reallocate(
      output->data, allocation_size, allocator.state);
    if (!data) {
      return false;
    }
    // If reallocation succeeded, memory may or may not have been moved
    // to fulfill the allocation request, invalidating output->data.
    output->data = data;
    for (size_t i = output->capacity; i < input->size; ++i) {
      if (!next_motor_interface__msg__MotorFeedback__init(&output->data[i])) {
        // If initialization of any new item fails, roll back
        // all previously initialized items. Existing items
        // in output are to be left unmodified.
        for (; i-- > output->capacity; ) {
          next_motor_interface__msg__MotorFeedback__fini(&output->data[i]);
        }
        return false;
      }
    }
    output->capacity = input->size;
  }
  output->size = input->size;
  for (size_t i = 0; i < input->size; ++i) {
    if (!next_motor_interface__msg__MotorFeedback__copy(
        &(input->data[i]), &(output->data[i])))
    {
      return false;
    }
  }
  return true;
}
