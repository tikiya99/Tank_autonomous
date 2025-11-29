# Teleop Keyboard Fix

## The Problem

The error you're seeing:
```
termios.error: (25, 'Inappropriate ioctl for device')
```

This happens because **keyboard input doesn't work when nodes are launched via launch files**. The `stdin` is not connected to a real terminal, so the keyboard can't be read.

## The Solution

Run the **serial bridge** and **teleop keyboard** in **separate terminals**.

---

## How to Use Teleop (CORRECT WAY)

### Terminal 1: Launch Serial Bridge

```bash
cd /home/thasinduwickrama/Documents/PlatformIO/Projects/RoverMini/ros2_ws
source install/setup.bash

# Just launches the serial bridge
ros2 launch rover_navigation teleop.launch.py
```

You should see:
```
[serial_bridge-1] [INFO] ... Connected to Serial Port
```

**Leave this terminal running.**

---

### Terminal 2: Run Teleop Keyboard

Open a **NEW terminal**, then:

```bash
cd /home/thasinduwickrama/Documents/PlatformIO/Projects/RoverMini/ros2_ws
source install/setup.bash

# Run teleop keyboard directly (NOT via launch file)
ros2 run rover_control teleop_keyboard
```

You should see:
```
Control Your Rover:
   W: Forward
   S: Backward
   A: Turn Left
   ...
```

**Now you can type W, A, S, D, etc. in this terminal!**

---

## Summary

| Terminal | Command | Purpose |
|----------|---------|---------|
| Terminal 1 | `ros2 launch rover_navigation teleop.launch.py` | Serial bridge (ESP32 communication) |
| Terminal 2 | `ros2 run rover_control teleop_keyboard` | Keyboard control (type here) |

**Both terminals must be running at the same time.**

---

## Quick Start Commands

### First Time Setup

```bash
# Upload firmware to ESP32 (only once)
cd /home/thasinduwickrama/Documents/PlatformIO/Projects/RoverMini/esp_firmware
pio run -t upload
```

### Every Time You Want to Control the Rover

**Terminal 1:**
```bash
cd /home/thasinduwickrama/Documents/PlatformIO/Projects/RoverMini/ros2_ws
source install/setup.bash
ros2 launch rover_navigation teleop.launch.py
```

**Terminal 2 (in new terminal):**
```bash
cd /home/thasinduwickrama/Documents/PlatformIO/Projects/RoverMini/ros2_ws
source install/setup.bash
ros2 run rover_control teleop_keyboard
```

Then press **W** to move forward!

---

## Troubleshooting

### "No such file or directory: '/dev/ttyUSB0'"

Find your ESP32 port:
```bash
ls /dev/tty* | grep USB
# or
ls /dev/tty* | grep ACM
```

Then specify the correct port:
```bash
ros2 launch rover_navigation teleop.launch.py serial_port:=/dev/ttyACM0
```

### "Permission denied"

```bash
sudo chmod 666 /dev/ttyUSB0  # Replace with your port
```

### Keyboard not responding

Make sure:
1. Terminal 2 is in **focus** (click on it)
2. You're typing in **Terminal 2** (where you ran `ros2 run rover_control teleop_keyboard`)
3. Both terminals are running simultaneously
