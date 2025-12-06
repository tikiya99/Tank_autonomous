"""
Comprehensive SLAM and Navigation launch file.
Launches camera, ORB-SLAM3, Nav2, serial bridge, and TF transformations.
"""

from launch import LaunchDescription
from launch_ros.actions import Node
from launch.actions import IncludeLaunchDescription, DeclareLaunchArgument, ExecuteProcess
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch.substitutions import LaunchConfiguration, PathJoinSubstitution
from launch_ros.substitutions import FindPackageShare
import os

def generate_launch_description():
    """Generate comprehensive launch description"""
    
    # Path configurations
    orb_slam3_ws = '/home/thasinduwickrama/Documents/PlatformIO/Projects/RoverMini/ros2_ws/src/ORB_SLAM3_ROS2'
    vocabulary_path = os.path.join(orb_slam3_ws, 'vocabulary', 'ORBvoc.txt')
    config_path = os.path.join(orb_slam3_ws, 'config', 'monocular', 'usb_camera.yaml')
    
    # Nav2 params path
    nav2_params_path = PathJoinSubstitution([
        FindPackageShare('rover_navigation'),
        'config',
        'nav2_params.yaml'
    ])
    
    return LaunchDescription([
        # ============ CAMERA ============
        Node(
            package='usb_cam',
            executable='usb_cam_node_exe',
            name='usb_cam',
            output='screen',
            parameters=[{
                'video_device': '/dev/video0',
                'image_width': 640,
                'image_height': 480,
                'pixel_format': 'yuyv',
                'camera_frame_id': 'camera_link',
                'framerate': 30.0,
                'io_method': 'mmap',
                'camera_name': 'rover_camera',
            }],
            remappings=[
                ('image_raw', '/camera/image_raw'),
                ('camera_info', '/camera/camera_info'),
            ]
        ),
        
        # ============ ORB-SLAM3 ============
        ExecuteProcess(
            cmd=[
                'ros2', 'run', 'orbslam3', 'mono',
                vocabulary_path,
                config_path
            ],
            output='screen',
            shell=False
        ),
        
        # ============ TRANSFORMS ============
        # Static transform: base_link -> camera_link
        Node(
            package='tf2_ros',
            executable='static_transform_publisher',
            name='base_to_camera_tf',
            arguments=['0.1', '0', '0.15', '0', '0', '0', 'base_link', 'camera_link']
        ),
        
        # ============ SERIAL BRIDGE (ESP32) ============
        Node(
            package='rover_control',
            executable='serial_bridge.py',
            name='serial_bridge',
            output='screen'
        ),
        
        # ============ NAV2 STACK ============
        # Note: Uncomment when Nav2 is fully configured and tested
        # IncludeLaunchDescription(
        #     PythonLaunchDescriptionSource([
        #         PathJoinSubstitution([
        #             FindPackageShare('nav2_bringup'),
        #             'launch',
        #             'navigation_launch.py'
        #         ])
        #     ]),
        #     launch_arguments={
        #         'params_file': nav2_params_path,
        #         'use_sim_time': 'false'
        #     }.items()
        # ),
        
        # ============ RVIZ ============
        # Node(
        #     package='rviz2',
        #     executable='rviz2',
        #     name='rviz2',
        #     output='screen',
        #     arguments=['-d', PathJoinSubstitution([
        #         FindPackageShare('rover_navigation'),
        #         'config',
        #         'slam_nav.rviz'
        #     ])]
        # ),
    ])
