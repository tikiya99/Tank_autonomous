# Quick Start Guide - ORB-SLAM3 System

## Important: Source the Workspace First!

Every time you open a new terminal, run these commands **separately**:

```bash
cd ~/Documents/PlatformIO/Projects/RoverMini/ros2_ws
source install/setup.bash
```

**Note**: These are **two separate commands**, not one!

**Important**: If you rebuild packages, you must source the workspace again in the same terminal to pick up the changes:
```bash
# After running colcon build...
source install/setup.bash  # Re-source to update!
```

## Installation Steps

### 1. Install Camera Calibration Package
```bash
sudo apt install -y ros-humble-camera-calibration
```

### 2. USB Camera Package
✅ **Already installed!** The `usb_cam` package has been built from source and is ready to use.

## Usage Commands

### Test 1: Launch Camera Only
```bash
# Terminal 1: Source and launch camera
cd ~/Documents/PlatformIO/Projects/RoverMini/ros2_ws
source install/setup.bash
ros2 launch rover_navigation camera.launch.py
```

### Test 2: Launch Camera + ORB-SLAM3
```bash
# Terminal 1: Source and launch
cd ~/Documents/PlatformIO/Projects/RoverMini/ros2_ws
source install/setup.bash
ros2 launch rover_navigation orbslam3_mono.launch.py
```

### Test 3: Full System (Camera + SLAM + Serial Bridge)
```bash
# Terminal 1: Source and launch
cd ~/Documents/PlatformIO/Projects/RoverMini/ros2_ws
source install/setup.bash
ros2 launch rover_navigation slam_navigation.launch.py
```

### Camera Calibration (After installing camera_calibration package)
```bash
# Terminal 1: Start camera
cd ~/Documents/PlatformIO/Projects/RoverMini/ros2_ws
source install/setup.bash
ros2 launch rover_navigation camera.launch.py

# Terminal 2: Run calibration (in a NEW terminal)
cd ~/Documents/PlatformIO/Projects/RoverMini/ros2_ws
source install/setup.bash
ros2 run camera_calibration cameracalibrator --size 8x6 --square 0.025 --ros-args --remap image:=/camera/image_raw
```

**Note**: Print an 8x6 checkerboard pattern with 25mm squares. Move it slowly in front of the camera until the calibration bars turn green, then click "Calibrate".

## Common Errors and Fixes

### Error: "Package 'rover_navigation' not found"
**Fix**: You forgot to source the workspace!
```bash
cd ~/Documents/PlatformIO/Projects/RoverMini/ros2_ws
source install/setup.bash
```

### Error: "Package 'camera_calibration' not found"
**Fix**: Install the package:
```bash
sudo apt install -y ros-humble-camera-calibration
```

### Error: "bash: cd: too many arguments"
**Fix**: Run commands on **separate lines**, not all together!

### ORB-SLAM3 Doesn't Initialize
**Solution**: Monocular SLAM needs motion to initialize. Move the camera side-to-side slowly while pointing at a textured scene (not a blank wall).

## File Locations Reference

- **Camera config**: `ros2_ws/src/ORB_SLAM3_ROS2/config/monocular/usb_camera.yaml`
- **Launch files**: `ros2_ws/src/rover_navigation/launch/`
- **ORB vocabulary**: `ros2_ws/src/ORB_SLAM3_ROS2/vocabulary/ORBvoc.txt`
- **Serial bridge**: `ros2_ws/src/rover_control/rover_control/serial_bridge.py`

## Permanent Setup (Optional)

To avoid sourcing every time, add this to your `~/.bashrc`:

```bash
echo "source ~/Documents/PlatformIO/Projects/RoverMini/ros2_ws/install/setup.bash" >> ~/.bashrc
```

Then restart your terminal or run:
```bash
source ~/.bashrc
```
