# RoverMini - Digital Motor Control Update

## Changes Made

### 1. ESP32 Firmware - Removed PWM Control

#### Main Control Firmware ([main.cpp](file:///home/thasinduwickrama/Documents/PlatformIO/Projects/RoverMini/esp_firmware/src/main.cpp))

**Changed from PWM to Digital ON/OFF:**
- ✅ Removed LEDC PWM configuration
- ✅ Motors now use simple `digitalWrite()` for direction control
- ✅ Enable pins (ENA, ENB) set to HIGH permanently
- ✅ Motors run at **full speed** when commanded (ON/OFF only)

**Updated Parameters:**
```cpp
const float WHEEL_RADIUS = 0.05;      // 5cm
const float WHEEL_SEPARATION = 0.20;  // 20cm
```

**Simplified Control Logic:**
- `speed > 0.1`: Motor forward (full speed)
- `speed < -0.1`: Motor backward (full speed)
- `-0.1 ≤ speed ≤ 0.1`: Motor stop

#### Motor Test Firmware ([motor_test.cpp](file:///home/thasinduwickrama/Documents/PlatformIO/Projects/RoverMini/esp_firmware/test/motor_test.cpp))

- ✅ Updated to match digital control scheme
- ✅ Tests motors at FULL SPEED only (no variable speeds)
- ✅ Simplified test sequences

### 2. Teleop Keyboard Fix

#### Issue
When launching `teleop.launch.py`, the keyboard interface didn't show up because it tried to open xterm (which may not be installed).

#### Solution ([teleop.launch.py](file:///home/thasinduwickrama/Documents/PlatformIO/Projects/RoverMini/ros2_ws/src/rover_navigation/launch/teleop.launch.py))

**Removed:**
```python
prefix='xterm -e',  # This was trying to open a new terminal
```

**Result:**
- Teleop keyboard now runs in the **same terminal** where you launch it
- No external dependencies needed
- You can directly type W/A/S/D commands

### 3. Build Status

ROS2 workspace rebuilt successfully:
```
Summary: 3 packages finished [3.38s]
```

---

## How to Use

### Upload Firmware

```bash
cd /home/thasinduwickrama/Documents/PlatformIO/Projects/RoverMini/esp_firmware
pio run -t upload
```

### Run Teleop Control

> [!IMPORTANT]
> The teleop keyboard now runs in the **same terminal** where you launch it. Make sure you can type in that terminal!

```bash
cd /home/thasinduwickrama/Documents/PlatformIO/Projects/RoverMini/ros2_ws
source install/setup.bash

# Launch (keyboard controls will appear in this terminal)
ros2 launch rover_navigation teleop.launch.py serial_port:=/dev/ttyUSB0
```

**You should see:**
```
Control Your Rover:
   W: Forward
   S: Backward
   A: Turn Left
   D: Turn Right
   Q: Rotate Left in Place
   E: Rotate Right in Place
   X: Stop
   +: Increase Speed
   -: Decrease Speed
   ESC: Quit
```

**Then you can type commands directly in that terminal.**

### Test Motors First

If this is your first time, test motors with the motor test script:

```bash
cd /home/thasinduwickrama/Documents/PlatformIO/Projects/RoverMini/esp_firmware

# Use motor test firmware
cp src/main.cpp src/main.cpp.backup
cp test/motor_test.cpp src/main.cpp

# Upload and monitor
pio run -t upload
pio device monitor
```

Motors will run at FULL SPEED during tests. You should see:
- Each motor testing forward and backward
- Both motors together
- Serial output showing test progress

After testing, restore main firmware:
```bash
cp src/main.cpp.backup src/main.cpp
pio run -t upload
```

---

## Important Notes

### Motor Speed Behavior

> [!WARNING]
> **Motors now run at FULL SPEED (100%) when any command is given.**
> 
> This means:
> - Pressing W → Motors go FORWARD at full speed
> - Pressing S → Motors go BACKWARD at full speed  
> - No variable speed control
> 
> If you need speed control later, you'll need to add PWM back or use hardware speed controllers.

### Differential Drive Kinematics

The firmware still uses proper differential drive mathematics:
```cpp
float left_speed = linear_x - (angular_z * WHEEL_SEPARATION / 2.0);
float right_speed = linear_x + (angular_z * WHEEL_SEPARATION / 2.0);
```

But instead of mapping these to PWM values, it only checks if they're positive, negative, or zero.

### Keyboard Teleop

> [!NOTE]
> **The teleop keyboard must run in an interactive terminal.**
> 
> - It reads raw keyboard input
> - It won't work if launched as a background service
> - You must be able to type in the terminal where it's running

---

## Troubleshooting

### Keyboard Controls Not Working

1. **Make sure teleop is running in the foreground:**
   ```bash
   # You should see the control menu
   Control Your Rover:
   W: Forward
   ...
   ```

2. **Make sure your terminal has focus** - click on it before typing

3. **Check if serial bridge is connected:**
   ```bash
   # In another terminal
   ros2 topic echo /cmd_vel
   # Press W in teleop terminal - you should see messages here
   ```

### Motors Running Wrong Direction

Simply swap the direction wires:
- Motor A: Swap IN1 (pin 12) and IN2 (pin 14) connections
- Motor B: Swap IN3 (pin 26) and IN4 (pin 27) connections

### Serial Port Issues

Find your ESP32 port:
```bash
# Before plugging in
ls /dev/tty* > before.txt

# After plugging in
ls /dev/tty* > after.txt

# See what's new
diff before.txt after.txt
```

Common ports: `/dev/ttyUSB0`, `/dev/ttyACM0`, `/dev/ttyUSB1`

### Permission Denied

```bash
sudo chmod 666 /dev/ttyUSB0  # Replace with your port
```

---

## Summary

✅ **Digital motor control** - Simple ON/OFF, no PWM  
✅ **Updated wheel parameters** - 5cm radius, 20cm separation  
✅ **Fixed teleop keyboard** - Runs in launching terminal  
✅ **Workspace rebuilt** - All packages ready  

**Ready to test!** Upload the firmware and launch teleop to control your rover.
