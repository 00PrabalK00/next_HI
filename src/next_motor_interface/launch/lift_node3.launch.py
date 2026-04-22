from launch import LaunchDescription
from launch_ros.actions import Node
from ament_index_python.packages import get_package_share_directory
import os

def generate_launch_description():
    pkg = get_package_share_directory("next_motor_interface")
    params = os.path.join(pkg, "config", "lift_node3.yaml")

    return LaunchDescription([
        Node(
            package="next_motor_interface",
            executable="kinco_lift_node3_std",
            name="kinco_lift_node3_std",
            output="screen",
            parameters=[params],
        )
    ])
