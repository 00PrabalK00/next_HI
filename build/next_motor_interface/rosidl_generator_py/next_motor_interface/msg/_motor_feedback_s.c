// generated from rosidl_generator_py/resource/_idl_support.c.em
// with input from next_motor_interface:msg/MotorFeedback.idl
// generated code does not contain a copyright notice
#define NPY_NO_DEPRECATED_API NPY_1_7_API_VERSION
#include <Python.h>
#include <stdbool.h>
#ifndef _WIN32
# pragma GCC diagnostic push
# pragma GCC diagnostic ignored "-Wunused-function"
#endif
#include "numpy/ndarrayobject.h"
#ifndef _WIN32
# pragma GCC diagnostic pop
#endif
#include "rosidl_runtime_c/visibility_control.h"
#include "next_motor_interface/msg/detail/motor_feedback__struct.h"
#include "next_motor_interface/msg/detail/motor_feedback__functions.h"

ROSIDL_GENERATOR_C_IMPORT
bool std_msgs__msg__header__convert_from_py(PyObject * _pymsg, void * _ros_message);
ROSIDL_GENERATOR_C_IMPORT
PyObject * std_msgs__msg__header__convert_to_py(void * raw_ros_message);

ROSIDL_GENERATOR_C_EXPORT
bool next_motor_interface__msg__motor_feedback__convert_from_py(PyObject * _pymsg, void * _ros_message)
{
  // check that the passed message is of the expected Python class
  {
    char full_classname_dest[55];
    {
      char * class_name = NULL;
      char * module_name = NULL;
      {
        PyObject * class_attr = PyObject_GetAttrString(_pymsg, "__class__");
        if (class_attr) {
          PyObject * name_attr = PyObject_GetAttrString(class_attr, "__name__");
          if (name_attr) {
            class_name = (char *)PyUnicode_1BYTE_DATA(name_attr);
            Py_DECREF(name_attr);
          }
          PyObject * module_attr = PyObject_GetAttrString(class_attr, "__module__");
          if (module_attr) {
            module_name = (char *)PyUnicode_1BYTE_DATA(module_attr);
            Py_DECREF(module_attr);
          }
          Py_DECREF(class_attr);
        }
      }
      if (!class_name || !module_name) {
        return false;
      }
      snprintf(full_classname_dest, sizeof(full_classname_dest), "%s.%s", module_name, class_name);
    }
    assert(strncmp("next_motor_interface.msg._motor_feedback.MotorFeedback", full_classname_dest, 54) == 0);
  }
  next_motor_interface__msg__MotorFeedback * ros_message = _ros_message;
  {  // header
    PyObject * field = PyObject_GetAttrString(_pymsg, "header");
    if (!field) {
      return false;
    }
    if (!std_msgs__msg__header__convert_from_py(field, &ros_message->header)) {
      Py_DECREF(field);
      return false;
    }
    Py_DECREF(field);
  }
  {  // right_wheel_position_rad
    PyObject * field = PyObject_GetAttrString(_pymsg, "right_wheel_position_rad");
    if (!field) {
      return false;
    }
    assert(PyFloat_Check(field));
    ros_message->right_wheel_position_rad = PyFloat_AS_DOUBLE(field);
    Py_DECREF(field);
  }
  {  // right_wheel_velocity_rad_s
    PyObject * field = PyObject_GetAttrString(_pymsg, "right_wheel_velocity_rad_s");
    if (!field) {
      return false;
    }
    assert(PyFloat_Check(field));
    ros_message->right_wheel_velocity_rad_s = PyFloat_AS_DOUBLE(field);
    Py_DECREF(field);
  }
  {  // right_wheel_command_rad_s
    PyObject * field = PyObject_GetAttrString(_pymsg, "right_wheel_command_rad_s");
    if (!field) {
      return false;
    }
    assert(PyFloat_Check(field));
    ros_message->right_wheel_command_rad_s = PyFloat_AS_DOUBLE(field);
    Py_DECREF(field);
  }
  {  // right_wheel_statusword
    PyObject * field = PyObject_GetAttrString(_pymsg, "right_wheel_statusword");
    if (!field) {
      return false;
    }
    assert(PyLong_Check(field));
    ros_message->right_wheel_statusword = (uint16_t)PyLong_AsUnsignedLong(field);
    Py_DECREF(field);
  }
  {  // left_wheel_position_rad
    PyObject * field = PyObject_GetAttrString(_pymsg, "left_wheel_position_rad");
    if (!field) {
      return false;
    }
    assert(PyFloat_Check(field));
    ros_message->left_wheel_position_rad = PyFloat_AS_DOUBLE(field);
    Py_DECREF(field);
  }
  {  // left_wheel_velocity_rad_s
    PyObject * field = PyObject_GetAttrString(_pymsg, "left_wheel_velocity_rad_s");
    if (!field) {
      return false;
    }
    assert(PyFloat_Check(field));
    ros_message->left_wheel_velocity_rad_s = PyFloat_AS_DOUBLE(field);
    Py_DECREF(field);
  }
  {  // left_wheel_command_rad_s
    PyObject * field = PyObject_GetAttrString(_pymsg, "left_wheel_command_rad_s");
    if (!field) {
      return false;
    }
    assert(PyFloat_Check(field));
    ros_message->left_wheel_command_rad_s = PyFloat_AS_DOUBLE(field);
    Py_DECREF(field);
  }
  {  // left_wheel_statusword
    PyObject * field = PyObject_GetAttrString(_pymsg, "left_wheel_statusword");
    if (!field) {
      return false;
    }
    assert(PyLong_Check(field));
    ros_message->left_wheel_statusword = (uint16_t)PyLong_AsUnsignedLong(field);
    Py_DECREF(field);
  }

  return true;
}

ROSIDL_GENERATOR_C_EXPORT
PyObject * next_motor_interface__msg__motor_feedback__convert_to_py(void * raw_ros_message)
{
  /* NOTE(esteve): Call constructor of MotorFeedback */
  PyObject * _pymessage = NULL;
  {
    PyObject * pymessage_module = PyImport_ImportModule("next_motor_interface.msg._motor_feedback");
    assert(pymessage_module);
    PyObject * pymessage_class = PyObject_GetAttrString(pymessage_module, "MotorFeedback");
    assert(pymessage_class);
    Py_DECREF(pymessage_module);
    _pymessage = PyObject_CallObject(pymessage_class, NULL);
    Py_DECREF(pymessage_class);
    if (!_pymessage) {
      return NULL;
    }
  }
  next_motor_interface__msg__MotorFeedback * ros_message = (next_motor_interface__msg__MotorFeedback *)raw_ros_message;
  {  // header
    PyObject * field = NULL;
    field = std_msgs__msg__header__convert_to_py(&ros_message->header);
    if (!field) {
      return NULL;
    }
    {
      int rc = PyObject_SetAttrString(_pymessage, "header", field);
      Py_DECREF(field);
      if (rc) {
        return NULL;
      }
    }
  }
  {  // right_wheel_position_rad
    PyObject * field = NULL;
    field = PyFloat_FromDouble(ros_message->right_wheel_position_rad);
    {
      int rc = PyObject_SetAttrString(_pymessage, "right_wheel_position_rad", field);
      Py_DECREF(field);
      if (rc) {
        return NULL;
      }
    }
  }
  {  // right_wheel_velocity_rad_s
    PyObject * field = NULL;
    field = PyFloat_FromDouble(ros_message->right_wheel_velocity_rad_s);
    {
      int rc = PyObject_SetAttrString(_pymessage, "right_wheel_velocity_rad_s", field);
      Py_DECREF(field);
      if (rc) {
        return NULL;
      }
    }
  }
  {  // right_wheel_command_rad_s
    PyObject * field = NULL;
    field = PyFloat_FromDouble(ros_message->right_wheel_command_rad_s);
    {
      int rc = PyObject_SetAttrString(_pymessage, "right_wheel_command_rad_s", field);
      Py_DECREF(field);
      if (rc) {
        return NULL;
      }
    }
  }
  {  // right_wheel_statusword
    PyObject * field = NULL;
    field = PyLong_FromUnsignedLong(ros_message->right_wheel_statusword);
    {
      int rc = PyObject_SetAttrString(_pymessage, "right_wheel_statusword", field);
      Py_DECREF(field);
      if (rc) {
        return NULL;
      }
    }
  }
  {  // left_wheel_position_rad
    PyObject * field = NULL;
    field = PyFloat_FromDouble(ros_message->left_wheel_position_rad);
    {
      int rc = PyObject_SetAttrString(_pymessage, "left_wheel_position_rad", field);
      Py_DECREF(field);
      if (rc) {
        return NULL;
      }
    }
  }
  {  // left_wheel_velocity_rad_s
    PyObject * field = NULL;
    field = PyFloat_FromDouble(ros_message->left_wheel_velocity_rad_s);
    {
      int rc = PyObject_SetAttrString(_pymessage, "left_wheel_velocity_rad_s", field);
      Py_DECREF(field);
      if (rc) {
        return NULL;
      }
    }
  }
  {  // left_wheel_command_rad_s
    PyObject * field = NULL;
    field = PyFloat_FromDouble(ros_message->left_wheel_command_rad_s);
    {
      int rc = PyObject_SetAttrString(_pymessage, "left_wheel_command_rad_s", field);
      Py_DECREF(field);
      if (rc) {
        return NULL;
      }
    }
  }
  {  // left_wheel_statusword
    PyObject * field = NULL;
    field = PyLong_FromUnsignedLong(ros_message->left_wheel_statusword);
    {
      int rc = PyObject_SetAttrString(_pymessage, "left_wheel_statusword", field);
      Py_DECREF(field);
      if (rc) {
        return NULL;
      }
    }
  }

  // ownership of _pymessage is transferred to the caller
  return _pymessage;
}
