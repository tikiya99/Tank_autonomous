import os
from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch.actions import IncludeLaunchDescription
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch.substitutions import LaunchConfiguration

def generate_launch_description():
    rover_navigation_dir = get_package_share_directory('rover_navigation')
    nav2_params_file = os.path.join(rover_navigation_dir, 'config', 'nav2_params.yaml')
    nav2_launch_dir = os.path.join(get_package_share_directory('nav2_bringup'), 'launch')

    return LaunchDescription([
        IncludeLaunchDescription(
            PythonLaunchDescriptionSource(os.path.join(nav2_launch_dir, 'navigation_launch.py')),
            launch_arguments={
                'params_file': nav2_params_file,
                'use_sim_time': 'False',
            }.items(),
        ),
    ])
