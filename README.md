# RoverMini - Autonomous Rover Control System

A ROS2-based autonomous rover with ESP32 motor control, USB camera integration, and keyboard teleoperation for navigation and SLAM.

## Hardware Requirements

- **ESP32 Development Board**
- **2x DC Motors** with H-bridge driver (L298N or similar)
- **USB Camera** (any generic webcam)
- **Power Supply** for motors
- **Raspberry Pi or Ubuntu laptop** for running ROS2

## Wiring Configuration

### ESP32 Pin Connections:
- **Motor A (Left):**
  - ENA (PWM) → GPIO 13
  - IN1 → GPIO 12
  - IN2 → GPIO 14

- **Motor B (Right):**
  - ENB (PWM) → GPIO 25
  - IN3 → GPIO 26
  - IN4 → GPIO 27

## Software Setup

### Prerequisites

1. **Install ROS2 Humble** (Ubuntu 22.04):
```bash
# Follow official ROS2 Humble installation guide
sudo apt update
sudo apt install ros-humble-desktop
```

2. **Install PlatformIO** (for ESP32):
```bash
# Using pip
pip install platformio

# Or using VS Code extension
# Install "PlatformIO IDE" from VS Code marketplace
```

3. **Install Required ROS2 Packages**:
```bash
sudo apt install ros-humble-usb-cam
sudo apt install ros-humble-rtabmap-slam
sudo apt install python3-serial
```

## Quick Start Guide

### 1. Upload ESP32 Firmware

#### Option A: Motor Test (Recommended First)
Test your motors before full deployment:

```bash
cd esp_firmware
# Temporarily use the motor test firmware
cp test/motor_test.cpp src/main.cpp.backup
cp src/main.cpp src/main.cpp.original
cp test/motor_test.cpp src/main.cpp

# Upload to ESP32
pio run -t upload

# Monitor serial output to verify motors
pio device monitor
```

Expected behavior:
- Each motor will test forward/backward at 50% and 100% speed
- Both motors will test together
- Serial output shows which test is running

**Note:** If a motor spins in the wrong direction, swap its IN1/IN2 (or IN3/IN4) wires.

#### Option B: Main Control Firmware
After motors are verified:

```bash
cd esp_firmware
# Restore original firmware (if you ran motor test)
cp src/main.cpp.original src/main.cpp

# Upload to ESP32
pio run -t upload
```

### 2. Build ROS2 Workspace

```bash
cd ros2_ws
colcon build
source install/setup.bash
```

### 3. Test Teleop Control

**Terminal 1 - Serial Bridge:**
```bash
cd ros2_ws
source install/setup.bash

# Find your ESP32 port (usually /dev/ttyUSB0 or /dev/ttyACM0)
ls /dev/tty*  # Check before and after plugging in ESP32

# Launch teleop with serial bridge
ros2 launch rover_navigation teleop.launch.py serial_port:=/dev/ttyUSB0
```

**Controls:**
- **W**: Forward
- **S**: Backward  
- **A**: Turn Left
- **D**: Turn Right
- **Q**: Rotate Left in Place
- **E**: Rotate Right in Place
- **X**: Stop
- **+/-**: Increase/Decrease Speed
- **ESC**: Quit

### 4. Test Camera

```bash
cd ros2_ws
source install/setup.bash

# Launch camera
ros2 launch rover_navigation camera.launch.py

# In another terminal, view camera feed
ros2 run rqt_image_view rqt_image_view
# Select /image_raw topic
```

### 5. Run Full System (Bringup)

```bash
cd ros2_ws
source install/setup.bash
ros2 launch rover_navigation bringup.launch.py
```

This launches:
- Robot state publisher
- Serial bridge
- USB camera

### 6. SLAM (Mapping)

```bash
cd ros2_ws
source install/setup.bash

# Launch SLAM
ros2 launch rover_navigation slam.launch.py

# In another terminal, launch teleop to drive around
ros2 launch rover_navigation teleop.launch.py
```

## Troubleshooting

### ESP32 Not Found
```bash
# Check serial port
ls /dev/tty*

# Add user to dialout group (logout/login required)
sudo usermod -a -G dialout $USER

# Check permissions
sudo chmod 666 /dev/ttyUSB0  # Or your port
```

### Motors Not Responding
1. Run motor test firmware first
2. Check serial output - are commands being received?
3. Verify motor driver connections
4. Check power supply to motors
5. Ensure ESP32 and motor driver share common ground

### Camera Not Working
```bash
# List video devices
v4l2-ctl --list-devices

# If camera is /dev/video1 or other:
ros2 launch rover_navigation camera.launch.py video_device:=/dev/video1

# Test with ffplay
ffplay /dev/video0
```

### Wrong Direction
If motors spin opposite to expected:
- Swap IN1 and IN2 for Motor A
- Swap IN3 and IN4 for Motor B

### ROS2 Package Not Found
```bash
# Make sure workspace is built and sourced
cd ros2_ws
colcon build
source install/setup.bash

# Check if packages are visible
ros2 pkg list | grep rover
```

## ROS2 Topics

- `/cmd_vel` - Velocity commands (geometry_msgs/Twist)
- `/image_raw` - Camera image stream (sensor_msgs/Image)
- `/camera_info` - Camera calibration info
- `/odom` - Odometry (from visual odometry in SLAM mode)

## ROS2 Nodes

### rover_control Package
- `serial_bridge` - Forwards cmd_vel to ESP32 via serial
- `teleop_keyboard` - Keyboard teleoperation
- `motor_test_node` - Automated motor testing via ROS2

### rover_navigation Package
- Launch files for camera, teleop, SLAM, and bringup

## Testing Commands

### Check Topics
```bash
ros2 topic list
ros2 topic echo /cmd_vel
ros2 topic hz /image_raw
```

### Manual Velocity Commands
```bash
# Move forward
ros2 topic pub /cmd_vel geometry_msgs/Twist "{linear: {x: 0.5}, angular: {z: 0.0}}"

# Rotate
ros2 topic pub /cmd_vel geometry_msgs/Twist "{linear: {x: 0.0}, angular: {z: 1.0}}"

# Stop
ros2 topic pub /cmd_vel geometry_msgs/Twist "{linear: {x: 0.0}, angular: {z: 0.0}}"
```

### ROS2 Motor Test
```bash
cd ros2_ws
source install/setup.bash

# Terminal 1: Start serial bridge
ros2 run rover_control serial_bridge

# Terminal 2: Run motor test
ros2 run rover_control motor_test_node
```

## Configuration

### Serial Port
Default: `/dev/ttyUSB0`

Change via launch argument:
```bash
ros2 launch rover_navigation teleop.launch.py serial_port:=/dev/ttyACM0
```

### Camera Device  
Default: `/dev/video0`

Change via launch argument:
```bash
ros2 launch rover_navigation camera.launch.py video_device:=/dev/video1
```

### Camera Resolution
Edit `ros2_ws/src/rover_navigation/launch/camera.launch.py`:
```python
'image_width': 1280,  # Change from 640
'image_height': 720,   # Change from 480
'framerate': 60.0,     # Change from 30.0
```

## Development

### Project Structure
```
RoverMini/
├── esp_firmware/           # ESP32 firmware (PlatformIO)
│   ├── src/
│   │   └── main.cpp       # Main control firmware
│   └── test/
│       └── motor_test.cpp # Motor testing firmware
└── ros2_ws/               # ROS2 workspace
    └── src/
        ├── rover_control/     # Serial bridge & teleop
        ├── rover_description/ # URDF robot model
        └── rover_navigation/  # Launch files & SLAM
```

### Adding New Features

To modify velocity mapping on ESP32:
- Edit `esp_firmware/src/main.cpp`
- Adjust scaling in `setMotorSpeed()` function
- Rebuild with `pio run -t upload`

To add new ROS2 nodes:
- Create Python file in `rover_control/rover_control/`
- Add entry point in `setup.py`
- Rebuild workspace with `colcon build`

## License

MIT License (or your preferred license)

## Contributors

- Thasindu Wickrama (thasinduwickrama12@gmail.com)

## Acknowledgments

- ROS2 Humble
- RTAB-Map for SLAM
- Arduino ESP32 framework
