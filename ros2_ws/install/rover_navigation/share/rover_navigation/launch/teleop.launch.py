from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument
from launch.substitutions import LaunchConfiguration
from launch_ros.actions import Node

def generate_launch_description():
    """
    Launch ONLY the serial bridge for ESP32 communication.
    
    NOTE: Run teleop_keyboard separately in another terminal:
        ros2 run rover_control teleop_keyboard
    
    This is because keyboard input doesn't work properly when launched via launch files.
    """
    
    # Declare arguments
    serial_port_arg = DeclareLaunchArgument(
        'serial_port',
        default_value='/dev/ttyUSB0',
        description='Serial port for ESP32 connection'
    )
    
    baud_rate_arg = DeclareLaunchArgument(
        'baud_rate',
        default_value='115200',
        description='Baud rate for serial communication'
    )
    
    # Serial bridge node
    serial_bridge_node = Node(
        package='rover_control',
        executable='serial_bridge',
        name='serial_bridge',
        output='screen',
        parameters=[{
            'serial_port': LaunchConfiguration('serial_port'),
            'baud_rate': LaunchConfiguration('baud_rate'),
        }]
    )
    
    return LaunchDescription([
        serial_port_arg,
        baud_rate_arg,
        serial_bridge_node,
    ])
