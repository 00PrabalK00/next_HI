#!/usr/bin/env python3
# Copyright 2024 Next Robotics
# SLAM Toolbox localization launch file (for use with existing map)

import os
from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument
from launch.substitutions import LaunchConfiguration
from launch_ros.actions import Node
from ament_index_python.packages import get_package_share_directory


def generate_launch_description():
    # Launch arguments
    use_sim_time = LaunchConfiguration('use_sim_time')
    map_file = LaunchConfiguration('map_file')
    
    # Paths
    pkg_share = get_package_share_directory('next_motor_interface')
    default_map_file = os.path.join(pkg_share, 'maps', 'my_map.yaml')
    
    declare_use_sim_time = DeclareLaunchArgument(
        'use_sim_time',
        default_value='false',
        description='Use simulation clock if true'
    )
    
    declare_map_file = DeclareLaunchArgument(
        'map_file',
        default_value=default_map_file,
        description='Full path to map yaml file'
    )
    
    # SLAM Toolbox Node (localization mode)
    localization_node = Node(
        package='slam_toolbox',
        executable='localization_slam_toolbox_node',
        name='slam_toolbox',
        output='screen',
        parameters=[
            {
                'use_sim_time': use_sim_time,
                'odom_frame': 'odom',
                'map_frame': 'map',
                'base_frame': 'base_footprint',
                'scan_topic': '/scan',
                'mode': 'localization',
                'map_file_name': map_file,
                'map_start_pose': [0.0, 0.0, 0.0],
            }
        ],
    )
    
    return LaunchDescription([
        declare_use_sim_time,
        declare_map_file,
        localization_node,
    ])
