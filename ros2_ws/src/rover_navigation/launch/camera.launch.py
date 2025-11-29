from launch import LaunchDescription
from launch_ros.actions import Node

def generate_launch_description():
    """Launch USB camera node for rover"""
    
    return LaunchDescription([
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
                ('image_raw', '/image_raw'),
                ('camera_info', '/camera_info'),
            ]
        ),
    ])
