# next_motor_interface/launch/robot_control.launch.py

from launch import LaunchDescription
from launch.actions import TimerAction
from launch.substitutions import Command, PathJoinSubstitution
from launch_ros.actions import Node
from launch_ros.substitutions import FindPackageShare
from launch.substitutions import FindExecutable
from launch_ros.parameter_descriptions import ParameterValue


def generate_launch_description():
    pkg = FindPackageShare("next_motor_interface")

    # Absolute paths inside install/ (NOT package://) so yaml-cpp can open them
    bus_yml = PathJoinSubstitution([pkg, "config", "bus_can0", "bus.yml"])
    master_dcf = PathJoinSubstitution([pkg, "config", "bus_can0", "master.dcf"])

    controllers_yaml = PathJoinSubstitution([pkg, "config", "bus_can0", "ros2_controllers.yaml"])

    # Build robot_description and pass xacro args used by ros2_control.xacro:
    # <param name="bus_config">${bus_config}</param>
    # <param name="master_config">${master_config}</param>
    robot_description_content = Command([
        PathJoinSubstitution([FindExecutable(name="xacro")]),
        " ",
        PathJoinSubstitution([pkg, "urdf", "robot.urdf.xacro"]),
        " ",
        "bus_config:=", bus_yml,
        " ",
        "master_config:=", master_dcf,
    ])

    robot_description = {
        "robot_description": ParameterValue(robot_description_content, value_type=str)
    }

    robot_state_publisher_node = Node(
        package="robot_state_publisher",
        executable="robot_state_publisher",
        output="screen",
        parameters=[robot_description],
    )

    ros2_control_node = Node(
        package="controller_manager",
        executable="ros2_control_node",
        output="screen",
        parameters=[robot_description, controllers_yaml],
    )

    joint_state_broadcaster_spawner = Node(
        package="controller_manager",
        executable="spawner",
        arguments=["joint_state_broadcaster", "--controller-manager", "/controller_manager"],
        output="screen",
    )

    wheel_controller_spawner = Node(
        package="controller_manager",
        executable="spawner",
        arguments=["wheel_controller", "--controller-manager", "/controller_manager"],
        output="screen",
    )

    return LaunchDescription([
        robot_state_publisher_node,
        ros2_control_node,
        TimerAction(period=2.0, actions=[
            joint_state_broadcaster_spawner,
            wheel_controller_spawner
        ]),
    ])

