from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument
from launch.substitutions import LaunchConfiguration
from launch_ros.actions import Node

def generate_launch_description():
    return LaunchDescription([
        # RTAB-Map Odometry (Visual Odometry)
        Node(
            package='rtabmap_odom',
            executable='rgbd_odometry',
            output='screen',
            parameters=[{
                'frame_id': 'base_link',
                'subscribe_depth': False,
                'subscribe_rgb': True,
                'approx_sync': True,
                'Reg/Strategy': '0', # 0=Vis, 1=Icp, 2=VisIcp
            }],
            remappings=[
                ('rgb/image', '/image_raw'),
                ('rgb/camera_info', '/camera_info'),
                ('odom', '/odom')
            ]
        ),

        # RTAB-Map SLAM
        Node(
            package='rtabmap_slam',
            executable='rtabmap',
            output='screen',
            parameters=[{
                'frame_id': 'base_link',
                'subscribe_depth': False,
                'subscribe_rgb': True,
                'approx_sync': True,
                'Rtabmap/DetectionRate': '3.5',
            }],
            remappings=[
                ('rgb/image', '/image_raw'),
                ('rgb/camera_info', '/camera_info'),
                ('odom', '/odom')
            ],
            arguments=['-d'] # Delete database on start
        ),
    ])
