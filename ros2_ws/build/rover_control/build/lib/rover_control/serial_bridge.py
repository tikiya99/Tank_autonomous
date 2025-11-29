import rclpy
from rclpy.node import Node
from geometry_msgs.msg import Twist
import serial
import time

class SerialBridge(Node):
    def __init__(self):
        super().__init__('serial_bridge')
        
        # Parameters
        self.declare_parameter('serial_port', '/dev/ttyUSB0')
        self.declare_parameter('baud_rate', 115200)
        
        self.port = self.get_parameter('serial_port').get_parameter_value().string_value
        self.baud = self.get_parameter('baud_rate').get_parameter_value().integer_value
        
        self.get_logger().info(f'Connecting to {self.port} at {self.baud}...')
        
        try:
            self.ser = serial.Serial(self.port, self.baud, timeout=1)
            self.get_logger().info('Connected to Serial Port')
        except serial.SerialException as e:
            self.get_logger().error(f'Failed to connect to serial port: {e}')
            self.ser = None

        self.subscription = self.create_subscription(
            Twist,
            'cmd_vel',
            self.cmd_vel_callback,
            10)
        self.subscription  # prevent unused variable warning

    def cmd_vel_callback(self, msg):
        if self.ser and self.ser.is_open:
            # Format: <linear_x,angular_z>
            command = f"{msg.linear.x:.2f},{msg.angular.z:.2f}\n"
            self.ser.write(command.encode('utf-8'))
            # Enable debug to see what's being sent
            self.get_logger().info(f'Sent to ESP32: {command.strip()}')

    def destroy_node(self):
        if self.ser and self.ser.is_open:
            self.ser.close()
        super().destroy_node()

def main(args=None):
    rclpy.init(args=args)
    serial_bridge = SerialBridge()
    rclpy.spin(serial_bridge)
    serial_bridge.destroy_node()
    rclpy.shutdown()

if __name__ == '__main__':
    main()
