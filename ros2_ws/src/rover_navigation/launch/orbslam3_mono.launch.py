"""
Launch file for monocular SLAM with ORB-SLAM3 and USB camera.
This file starts the camera node and ORB-SLAM3 node.
"""

from launch import LaunchDescription
from launch_ros.actions import Node
from launch.actions import DeclareLaunchArgument, ExecuteProcess
from launch.substitutions import LaunchConfiguration
import os

def generate_launch_description():
    """Generate launch description for monocular SLAM"""
    
    # Path configurations
    orb_slam3_ws = '/home/thasinduwickrama/Documents/PlatformIO/Projects/RoverMini/ros2_ws/src/ORB_SLAM3_ROS2'
    vocabulary_path = os.path.join(orb_slam3_ws, 'vocabulary', 'ORBvoc.txt')
    config_path = os.path.join(orb_slam3_ws, 'config', 'monocular', 'usb_camera.yaml')
    
    # Declare launch arguments
    use_sim_time = LaunchConfiguration('use_sim_time', default='false')
    
    return LaunchDescription([
        # USB Camera Node
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
                'camera_info_url': '',
            }],
            remappings=[
                ('image_raw', '/camera/image_raw'),
                ('camera_info', '/camera/camera_info'),
            ]
        ),
        
        # ORB-SLAM3 Monocular Node
        Node(
            package='orbslam3',
            executable='mono',
            name='orbslam3_mono',
            output='screen',
            arguments=[vocabulary_path, config_path],
            remappings=[
                ('camera', '/camera/image_raw'),  # Map ORB-SLAM3's 'camera' topic to usb_cam's output
            ]
        ),
        
        # Static transform from base_link to camera frame
        # Using 'default_cam' because that's what usb_cam is actually publishing
        Node(
            package='tf2_ros',
            executable='static_transform_publisher',
            name='base_to_camera_tf',
            arguments=['0.1', '0', '0.15', '0', '0', '0', 'base_link', 'default_cam']
            # Format: x y z yaw pitch roll parent_frame child_frame
            # x=0.1m forward, z=0.15m up from base_link
        ),
    ])
