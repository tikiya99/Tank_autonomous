#!/usr/bin/env python3
import rclpy
from rclpy.node import Node
from geometry_msgs.msg import Twist
import sys
import tty
import termios
import select

class TeleopKeyboard(Node):
    def __init__(self):
        super().__init__('teleop_keyboard')
        
        self.publisher_ = self.create_publisher(Twist, 'cmd_vel', 10)
        
        # Movement parameters
        self.linear_speed = 0.5  # m/s
        self.angular_speed = 1.0  # rad/s
        self.speed_step = 0.1
        
        self.get_logger().info('Teleop Keyboard Node Started')
        self.get_logger().info('-------------------------------')
        self.get_logger().info('Control Your Rover:')
        self.get_logger().info('   W: Forward')
        self.get_logger().info('   S: Backward')
        self.get_logger().info('   A: Turn Left')
        self.get_logger().info('   D: Turn Right')
        self.get_logger().info('   Q: Rotate Left in Place')
        self.get_logger().info('   E: Rotate Right in Place')
        self.get_logger().info('   X: Stop')
        self.get_logger().info('   +: Increase Speed')
        self.get_logger().info('   -: Decrease Speed')
        self.get_logger().info('   ESC: Quit')
        self.get_logger().info('-------------------------------')
        
        self.print_speed()

    def print_speed(self):
        self.get_logger().info(f'Current speeds - Linear: {self.linear_speed:.2f} m/s, Angular: {self.angular_speed:.2f} rad/s')

    def get_key(self):
        """Get a single keypress from the terminal"""
        tty.setraw(sys.stdin.fileno())
        select.select([sys.stdin], [], [], 0)
        key = sys.stdin.read(1)
        termios.tcsetattr(sys.stdin, termios.TCSADRAIN, self.settings)
        return key

    def publish_velocity(self, linear, angular):
        """Publish velocity command"""
        msg = Twist()
        msg.linear.x = linear
        msg.angular.z = angular
        self.publisher_.publish(msg)

    def run(self):
        """Main control loop"""
        self.settings = termios.tcgetattr(sys.stdin)
        
        try:
            while True:
                key = self.get_key()
                
                linear = 0.0
                angular = 0.0
                
                if key == 'w' or key == 'W':
                    linear = self.linear_speed
                    self.get_logger().info('Moving Forward')
                elif key == 's' or key == 'S':
                    linear = -self.linear_speed
                    self.get_logger().info('Moving Backward')
                elif key == 'a' or key == 'A':
                    linear = self.linear_speed
                    angular = self.angular_speed
                    self.get_logger().info('Turning Left')
                elif key == 'd' or key == 'D':
                    linear = self.linear_speed
                    angular = -self.angular_speed
                    self.get_logger().info('Turning Right')
                elif key == 'q' or key == 'Q':
                    angular = self.angular_speed
                    self.get_logger().info('Rotating Left')
                elif key == 'e' or key == 'E':
                    angular = -self.angular_speed
                    self.get_logger().info('Rotating Right')
                elif key == 'x' or key == 'X':
                    linear = 0.0
                    angular = 0.0
                    self.get_logger().info('STOP')
                elif key == '+' or key == '=':
                    self.linear_speed += self.speed_step
                    self.angular_speed += self.speed_step * 2
                    self.print_speed()
                elif key == '-' or key == '_':
                    self.linear_speed = max(0.0, self.linear_speed - self.speed_step)
                    self.angular_speed = max(0.0, self.angular_speed - self.speed_step * 2)
                    self.print_speed()
                elif key == '\x1b':  # ESC
                    self.get_logger().info('Exiting...')
                    break
                elif key == '\x03':  # Ctrl+C
                    break
                
                self.publish_velocity(linear, angular)
                
        except Exception as e:
            self.get_logger().error(f'Error: {e}')
        finally:
            # Send stop command before exiting
            self.publish_velocity(0.0, 0.0)
            termios.tcsetattr(sys.stdin, termios.TCSADRAIN, self.settings)

def main(args=None):
    rclpy.init(args=args)
    teleop = TeleopKeyboard()
    
    try:
        teleop.run()
    except KeyboardInterrupt:
        pass
    finally:
        teleop.destroy_node()
        rclpy.shutdown()

if __name__ == '__main__':
    main()
