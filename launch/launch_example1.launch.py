from launch import LaunchDescription
from launch_ros.actions import Node


def generate_launch_description():
    return LaunchDescription([
        Node(
            package='ben_d91_autonom',
            executable='distance_sim_node',
            name='distance_sim_node',
            output='screen',
        ),
        Node(
            package='ben_d91_autonom',
            executable='emergency_brake_node',
            name='emergency_brake_node',
            output='screen',
        ),
    ])
