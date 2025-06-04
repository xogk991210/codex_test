from launch import LaunchDescription
from launch_ros.actions import Node

def generate_launch_description():
    return LaunchDescription([
        Node(
            package='ground_removal_costmap',
            executable='ground_removal_node',
            name='ground_removal_node',
            output='screen'
        )
    ])
