# generated from rosidl_generator_py/resource/_idl.py.em
# with input from next_motor_interface:msg/MotorFeedback.idl
# generated code does not contain a copyright notice


# Import statements for member types

import builtins  # noqa: E402, I100

import math  # noqa: E402, I100

import rosidl_parser.definition  # noqa: E402, I100


class Metaclass_MotorFeedback(type):
    """Metaclass of message 'MotorFeedback'."""

    _CREATE_ROS_MESSAGE = None
    _CONVERT_FROM_PY = None
    _CONVERT_TO_PY = None
    _DESTROY_ROS_MESSAGE = None
    _TYPE_SUPPORT = None

    __constants = {
    }

    @classmethod
    def __import_type_support__(cls):
        try:
            from rosidl_generator_py import import_type_support
            module = import_type_support('next_motor_interface')
        except ImportError:
            import logging
            import traceback
            logger = logging.getLogger(
                'next_motor_interface.msg.MotorFeedback')
            logger.debug(
                'Failed to import needed modules for type support:\n' +
                traceback.format_exc())
        else:
            cls._CREATE_ROS_MESSAGE = module.create_ros_message_msg__msg__motor_feedback
            cls._CONVERT_FROM_PY = module.convert_from_py_msg__msg__motor_feedback
            cls._CONVERT_TO_PY = module.convert_to_py_msg__msg__motor_feedback
            cls._TYPE_SUPPORT = module.type_support_msg__msg__motor_feedback
            cls._DESTROY_ROS_MESSAGE = module.destroy_ros_message_msg__msg__motor_feedback

            from std_msgs.msg import Header
            if Header.__class__._TYPE_SUPPORT is None:
                Header.__class__.__import_type_support__()

    @classmethod
    def __prepare__(cls, name, bases, **kwargs):
        # list constant names here so that they appear in the help text of
        # the message class under "Data and other attributes defined here:"
        # as well as populate each message instance
        return {
        }


class MotorFeedback(metaclass=Metaclass_MotorFeedback):
    """Message class 'MotorFeedback'."""

    __slots__ = [
        '_header',
        '_right_wheel_position_rad',
        '_right_wheel_velocity_rad_s',
        '_right_wheel_command_rad_s',
        '_right_wheel_statusword',
        '_left_wheel_position_rad',
        '_left_wheel_velocity_rad_s',
        '_left_wheel_command_rad_s',
        '_left_wheel_statusword',
    ]

    _fields_and_field_types = {
        'header': 'std_msgs/Header',
        'right_wheel_position_rad': 'double',
        'right_wheel_velocity_rad_s': 'double',
        'right_wheel_command_rad_s': 'double',
        'right_wheel_statusword': 'uint16',
        'left_wheel_position_rad': 'double',
        'left_wheel_velocity_rad_s': 'double',
        'left_wheel_command_rad_s': 'double',
        'left_wheel_statusword': 'uint16',
    }

    SLOT_TYPES = (
        rosidl_parser.definition.NamespacedType(['std_msgs', 'msg'], 'Header'),  # noqa: E501
        rosidl_parser.definition.BasicType('double'),  # noqa: E501
        rosidl_parser.definition.BasicType('double'),  # noqa: E501
        rosidl_parser.definition.BasicType('double'),  # noqa: E501
        rosidl_parser.definition.BasicType('uint16'),  # noqa: E501
        rosidl_parser.definition.BasicType('double'),  # noqa: E501
        rosidl_parser.definition.BasicType('double'),  # noqa: E501
        rosidl_parser.definition.BasicType('double'),  # noqa: E501
        rosidl_parser.definition.BasicType('uint16'),  # noqa: E501
    )

    def __init__(self, **kwargs):
        assert all('_' + key in self.__slots__ for key in kwargs.keys()), \
            'Invalid arguments passed to constructor: %s' % \
            ', '.join(sorted(k for k in kwargs.keys() if '_' + k not in self.__slots__))
        from std_msgs.msg import Header
        self.header = kwargs.get('header', Header())
        self.right_wheel_position_rad = kwargs.get('right_wheel_position_rad', float())
        self.right_wheel_velocity_rad_s = kwargs.get('right_wheel_velocity_rad_s', float())
        self.right_wheel_command_rad_s = kwargs.get('right_wheel_command_rad_s', float())
        self.right_wheel_statusword = kwargs.get('right_wheel_statusword', int())
        self.left_wheel_position_rad = kwargs.get('left_wheel_position_rad', float())
        self.left_wheel_velocity_rad_s = kwargs.get('left_wheel_velocity_rad_s', float())
        self.left_wheel_command_rad_s = kwargs.get('left_wheel_command_rad_s', float())
        self.left_wheel_statusword = kwargs.get('left_wheel_statusword', int())

    def __repr__(self):
        typename = self.__class__.__module__.split('.')
        typename.pop()
        typename.append(self.__class__.__name__)
        args = []
        for s, t in zip(self.__slots__, self.SLOT_TYPES):
            field = getattr(self, s)
            fieldstr = repr(field)
            # We use Python array type for fields that can be directly stored
            # in them, and "normal" sequences for everything else.  If it is
            # a type that we store in an array, strip off the 'array' portion.
            if (
                isinstance(t, rosidl_parser.definition.AbstractSequence) and
                isinstance(t.value_type, rosidl_parser.definition.BasicType) and
                t.value_type.typename in ['float', 'double', 'int8', 'uint8', 'int16', 'uint16', 'int32', 'uint32', 'int64', 'uint64']
            ):
                if len(field) == 0:
                    fieldstr = '[]'
                else:
                    assert fieldstr.startswith('array(')
                    prefix = "array('X', "
                    suffix = ')'
                    fieldstr = fieldstr[len(prefix):-len(suffix)]
            args.append(s[1:] + '=' + fieldstr)
        return '%s(%s)' % ('.'.join(typename), ', '.join(args))

    def __eq__(self, other):
        if not isinstance(other, self.__class__):
            return False
        if self.header != other.header:
            return False
        if self.right_wheel_position_rad != other.right_wheel_position_rad:
            return False
        if self.right_wheel_velocity_rad_s != other.right_wheel_velocity_rad_s:
            return False
        if self.right_wheel_command_rad_s != other.right_wheel_command_rad_s:
            return False
        if self.right_wheel_statusword != other.right_wheel_statusword:
            return False
        if self.left_wheel_position_rad != other.left_wheel_position_rad:
            return False
        if self.left_wheel_velocity_rad_s != other.left_wheel_velocity_rad_s:
            return False
        if self.left_wheel_command_rad_s != other.left_wheel_command_rad_s:
            return False
        if self.left_wheel_statusword != other.left_wheel_statusword:
            return False
        return True

    @classmethod
    def get_fields_and_field_types(cls):
        from copy import copy
        return copy(cls._fields_and_field_types)

    @builtins.property
    def header(self):
        """Message field 'header'."""
        return self._header

    @header.setter
    def header(self, value):
        if __debug__:
            from std_msgs.msg import Header
            assert \
                isinstance(value, Header), \
                "The 'header' field must be a sub message of type 'Header'"
        self._header = value

    @builtins.property
    def right_wheel_position_rad(self):
        """Message field 'right_wheel_position_rad'."""
        return self._right_wheel_position_rad

    @right_wheel_position_rad.setter
    def right_wheel_position_rad(self, value):
        if __debug__:
            assert \
                isinstance(value, float), \
                "The 'right_wheel_position_rad' field must be of type 'float'"
            assert not (value < -1.7976931348623157e+308 or value > 1.7976931348623157e+308) or math.isinf(value), \
                "The 'right_wheel_position_rad' field must be a double in [-1.7976931348623157e+308, 1.7976931348623157e+308]"
        self._right_wheel_position_rad = value

    @builtins.property
    def right_wheel_velocity_rad_s(self):
        """Message field 'right_wheel_velocity_rad_s'."""
        return self._right_wheel_velocity_rad_s

    @right_wheel_velocity_rad_s.setter
    def right_wheel_velocity_rad_s(self, value):
        if __debug__:
            assert \
                isinstance(value, float), \
                "The 'right_wheel_velocity_rad_s' field must be of type 'float'"
            assert not (value < -1.7976931348623157e+308 or value > 1.7976931348623157e+308) or math.isinf(value), \
                "The 'right_wheel_velocity_rad_s' field must be a double in [-1.7976931348623157e+308, 1.7976931348623157e+308]"
        self._right_wheel_velocity_rad_s = value

    @builtins.property
    def right_wheel_command_rad_s(self):
        """Message field 'right_wheel_command_rad_s'."""
        return self._right_wheel_command_rad_s

    @right_wheel_command_rad_s.setter
    def right_wheel_command_rad_s(self, value):
        if __debug__:
            assert \
                isinstance(value, float), \
                "The 'right_wheel_command_rad_s' field must be of type 'float'"
            assert not (value < -1.7976931348623157e+308 or value > 1.7976931348623157e+308) or math.isinf(value), \
                "The 'right_wheel_command_rad_s' field must be a double in [-1.7976931348623157e+308, 1.7976931348623157e+308]"
        self._right_wheel_command_rad_s = value

    @builtins.property
    def right_wheel_statusword(self):
        """Message field 'right_wheel_statusword'."""
        return self._right_wheel_statusword

    @right_wheel_statusword.setter
    def right_wheel_statusword(self, value):
        if __debug__:
            assert \
                isinstance(value, int), \
                "The 'right_wheel_statusword' field must be of type 'int'"
            assert value >= 0 and value < 65536, \
                "The 'right_wheel_statusword' field must be an unsigned integer in [0, 65535]"
        self._right_wheel_statusword = value

    @builtins.property
    def left_wheel_position_rad(self):
        """Message field 'left_wheel_position_rad'."""
        return self._left_wheel_position_rad

    @left_wheel_position_rad.setter
    def left_wheel_position_rad(self, value):
        if __debug__:
            assert \
                isinstance(value, float), \
                "The 'left_wheel_position_rad' field must be of type 'float'"
            assert not (value < -1.7976931348623157e+308 or value > 1.7976931348623157e+308) or math.isinf(value), \
                "The 'left_wheel_position_rad' field must be a double in [-1.7976931348623157e+308, 1.7976931348623157e+308]"
        self._left_wheel_position_rad = value

    @builtins.property
    def left_wheel_velocity_rad_s(self):
        """Message field 'left_wheel_velocity_rad_s'."""
        return self._left_wheel_velocity_rad_s

    @left_wheel_velocity_rad_s.setter
    def left_wheel_velocity_rad_s(self, value):
        if __debug__:
            assert \
                isinstance(value, float), \
                "The 'left_wheel_velocity_rad_s' field must be of type 'float'"
            assert not (value < -1.7976931348623157e+308 or value > 1.7976931348623157e+308) or math.isinf(value), \
                "The 'left_wheel_velocity_rad_s' field must be a double in [-1.7976931348623157e+308, 1.7976931348623157e+308]"
        self._left_wheel_velocity_rad_s = value

    @builtins.property
    def left_wheel_command_rad_s(self):
        """Message field 'left_wheel_command_rad_s'."""
        return self._left_wheel_command_rad_s

    @left_wheel_command_rad_s.setter
    def left_wheel_command_rad_s(self, value):
        if __debug__:
            assert \
                isinstance(value, float), \
                "The 'left_wheel_command_rad_s' field must be of type 'float'"
            assert not (value < -1.7976931348623157e+308 or value > 1.7976931348623157e+308) or math.isinf(value), \
                "The 'left_wheel_command_rad_s' field must be a double in [-1.7976931348623157e+308, 1.7976931348623157e+308]"
        self._left_wheel_command_rad_s = value

    @builtins.property
    def left_wheel_statusword(self):
        """Message field 'left_wheel_statusword'."""
        return self._left_wheel_statusword

    @left_wheel_statusword.setter
    def left_wheel_statusword(self, value):
        if __debug__:
            assert \
                isinstance(value, int), \
                "The 'left_wheel_statusword' field must be of type 'int'"
            assert value >= 0 and value < 65536, \
                "The 'left_wheel_statusword' field must be an unsigned integer in [0, 65535]"
        self._left_wheel_statusword = value
