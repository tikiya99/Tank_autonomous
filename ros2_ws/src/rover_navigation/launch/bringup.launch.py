import os
from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument
from launch.substitutions import LaunchConfiguration
from launch_ros.actions import Node

def generate_launch_description():
    rover_description_dir = get_package_share_directory('rover_description')
    urdf_file = os.path.join(rover_description_dir, 'urdf', 'rover.urdf')

    return LaunchDescription([
        # Robot State Publisher
        Node(
            package='robot_state_publisher',
            executable='robot_state_publisher',
            name='robot_state_publisher',
            output='screen',
            arguments=[urdf_file]
        ),
        
        # Serial Bridge
        Node(
            package='rover_control',
            executable='serial_bridge',
            name='serial_bridge',
            output='screen',
            parameters=[{'serial_port': '/dev/ttyUSB0'}] # Adjust port if needed
        ),

        # USB Camera Driver
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
            }],
            remappings=[
                ('image_raw', '/image_raw'),
                ('camera_info', '/camera_info'),
            ]
        ),
        
        # Static TF for camera (if not in URDF or need adjustment)
        # Node(
        #     package='tf2_ros',
        #     executable='static_transform_publisher',
        #     arguments=['0.1', '0', '0.1', '0', '0', '0', 'base_link', 'camera_link']
        # )
    ])
