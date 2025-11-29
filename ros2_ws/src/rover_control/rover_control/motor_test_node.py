#!/usr/bin/env python3
import rclpy
from rclpy.node import Node
from geometry_msgs.msg import Twist
import time

class MotorTestNode(Node):
    def __init__(self):
        super().__init__('motor_test_node')
        
        self.publisher_ = self.create_publisher(Twist, 'cmd_vel', 10)
        
        self.get_logger().info('Motor Test Node Started')
        self.get_logger().info('This will send test commands to the rover motors via serial bridge')
        self.get_logger().info('Make sure the serial_bridge node is running!')
        
    def publish_cmd(self, linear, angular, duration=2.0):
        """Publish velocity command for a duration"""
        msg = Twist()
        msg.linear.x = linear
        msg.angular.z = angular
        
        start_time = time.time()
        rate = self.create_rate(10)  # 10 Hz
        
        while (time.time() - start_time) < duration:
            self.publisher_.publish(msg)
            rate.sleep()
    
    def stop(self, duration=1.0):
        """Send stop command"""
        self.get_logger().info('  Stopping...')
        self.publish_cmd(0.0, 0.0, duration)
    
    def run_tests(self):
        """Run comprehensive motor tests"""
        self.get_logger().info('\n=================================')
        self.get_logger().info('  STARTING MOTOR TESTS')
        self.get_logger().info('=================================\n')
        
        time.sleep(2)
        
        # Test 1: Forward motion
        self.get_logger().info('Test 1: Moving Forward (50% speed)')
        self.publish_cmd(0.5, 0.0, 3.0)
        self.stop()
        
        # Test 2: Forward motion (full speed)
        self.get_logger().info('Test 2: Moving Forward (100% speed)')
        self.publish_cmd(1.0, 0.0, 3.0)
        self.stop()
        
        # Test 3: Backward motion
        self.get_logger().info('Test 3: Moving Backward (50% speed)')
        self.publish_cmd(-0.5, 0.0, 3.0)
        self.stop()
        
        # Test 4: Backward motion (full speed)
        self.get_logger().info('Test 4: Moving Backward (100% speed)')
        self.publish_cmd(-1.0, 0.0, 3.0)
        self.stop()
        
        # Test 5: Rotate left in place
        self.get_logger().info('Test 5: Rotating Left in Place')
        self.publish_cmd(0.0, 1.0, 3.0)
        self.stop()
        
        # Test 6: Rotate right in place
        self.get_logger().info('Test 6: Rotating Right in Place')
        self.publish_cmd(0.0, -1.0, 3.0)
        self.stop()
        
        # Test 7: Turn left while moving forward
        self.get_logger().info('Test 7: Turning Left While Moving Forward')
        self.publish_cmd(0.5, 0.5, 3.0)
        self.stop()
        
        # Test 8: Turn right while moving forward
        self.get_logger().info('Test 8: Turning Right While Moving Forward')
        self.publish_cmd(0.5, -0.5, 3.0)
        self.stop()
        
        self.get_logger().info('\n=================================')
        self.get_logger().info('  ALL MOTOR TESTS COMPLETE!')
        self.get_logger().info('=================================\n')

def main(args=None):
    rclpy.init(args=args)
    motor_test = MotorTestNode()
    
    try:
        motor_test.run_tests()
    except KeyboardInterrupt:
        motor_test.get_logger().info('Test interrupted by user')
    finally:
        # Ensure motors are stopped
        msg = Twist()
        motor_test.publisher_.publish(msg)
        motor_test.destroy_node()
        rclpy.shutdown()

if __name__ == '__main__':
    main()
