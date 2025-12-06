#!/usr/bin/env python3
"""
Serial Bridge Node - Converts ROS2 cmd_vel to ESP32 serial format
Subscribes to: /cmd_vel (geometry_msgs/Twist)
Sends to ESP32: "linear,angular\n" format via serial
"""

import rclpy
from rclpy.node import Node
from geometry_msgs.msg import Twist
import serial
import time

class SerialBridge(Node):
    def __init__(self):
        super().__init__('serial_bridge')
        
        # Declare parameters
        self.declare_parameter('serial_port', '/dev/ttyACM0')
        self.declare_parameter('baud_rate', 115200)
        
        # Get parameters
        serial_port = self.get_parameter('serial_port').value
        baud_rate = self.get_parameter('baud_rate').value
        
        # Initialize serial connection
        try:
            self.serial = serial.Serial(serial_port, baud_rate, timeout=0.1)
            time.sleep(2)  # Wait for Arduino to reset
            self.get_logger().info(f'Connected to {serial_port} at {baud_rate} baud')
        except Exception as e:
            self.get_logger().error(f'Failed to connect to serial: {e}')
            self.serial = None
        
        # Subscribe to cmd_vel
        self.subscription = self.create_subscription(
            Twist,
            '/cmd_vel',
            self.cmd_vel_callback,
            10
        )
        
        self.get_logger().info('Serial bridge node started')
    
    def cmd_vel_callback(self, msg):
        """Convert Twist to serial format and send"""
        if self.serial is None:
            return
        
        linear = msg.linear.x
        angular = msg.angular.z
        
        try:
            # Format: "linear,angular\n"
            command = f"{linear:.2f},{angular:.2f}\n"
            self.serial.write(command.encode())
            self.get_logger().debug(f'Sent: {command.strip()}')
        except Exception as e:
            self.get_logger().error(f'Serial write error: {e}')
    
    def destroy_node(self):
        """Clean up serial connection"""
        if self.serial is not None:
            self.serial.close()
        super().destroy_node()

def main(args=None):
    rclpy.init(args=args)
    node = SerialBridge()
    
    try:
        rclpy.spin(node)
    except KeyboardInterrupt:
        pass
    finally:
        node.destroy_node()
        rclpy.shutdown()

if __name__ == '__main__':
    main()
