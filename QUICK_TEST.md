# Quick Motor Test Guide

## The Problem
- ✅ ROS2 is connected
- ✅ Teleop keyboard accepting input
- ✅ Serial bridge connected to ESP32
- ❌ Motors NOT moving

## Quick Test (Bypass ROS2)

### 1. **Stop ROS2** (if running)
Press **Ctrl+C** in both Terminal 1 and Terminal 2

### 2. **Re-upload Firmware** (with debug enabled)
```bash
cd /home/thasinduwickrama/Documents/PlatformIO/Projects/RoverMini/esp_firmware
pio run -t upload
```

### 3. **Run Direct Serial Test**
```bash
cd /home/thasinduwickrama/Documents/PlatformIO/Projects/RoverMini/ros2_ws/src/rover_control/rover_control
python3 test_serial.py
```

**Watch for:**
- Do motors move?
- Do you see "ESP32 Response: L:... R:..."?

If **NO motors moving** → Hardware issue (see below)  
If **motors move** → ROS2 configuration issue

---

## Hardware Checklist (If motors don't move)

### Critical: Power Supply
```
[ ] Motor power supply is connected
[ ] Motor power supply is TURNED ON
[ ] Power supply voltage is correct (6-12V typically)
[ ] Power supply has enough current (2A+ recommended)
```

### Critical: Common Ground
```
[ ] ESP32 GND connected to Motor Driver GND
    (Without this, NOTHING will work!)
```

### Wiring Check
```
ESP32 Pin 13 → Motor Driver ENA
ESP32 Pin 12 → Motor Driver IN1
ESP32 Pin 14 → Motor Driver IN2
ESP32 Pin 25 → Motor Driver ENB
ESP32 Pin 26 → Motor Driver IN3
ESP32 Pin 27 → Motor Driver IN4
ESP32 GND   → Motor Driver GND ⚠️ CRITICAL!
```

### Enable Pin Issue (Common!)
Some motor drivers **need 5V on enable pins**, ESP32 gives only 3.3V.

**Quick Fix:**
1. Disconnect ENA wire from ESP32 pin 13
2. Disconnect ENB wire from ESP32 pin 25  
3. Connect both ENA and ENB **directly to 5V** on motor driver
4. Test again

---

## Alternative: Test with Motor Test Firmware

```bash
cd /home/thasinduwickrama/Documents/PlatformIO/Projects/RoverMini/esp_firmware

# Use motor test
cp src/main.cpp src/main.cpp.backup
cp test/motor_test.cpp src/main.cpp

# Upload
pio run -t upload

# Monitor (watch for motor movement)
pio device monitor
```

**If motors run now** → Wiring/power is OK, issue is in main firmware  
**If motors still don't run** → Hardware problem (power/wiring)

Restore main firmware:
```bash
cp src/main.cpp.backup src/main.cpp
pio run -t upload
```

---

## With ROS2 (After hardware verified)

**Terminal 1:**
```bash
cd ~/Documents/PlatformIO/Projects/RoverMini/ros2_ws
source install/setup.bash
ros2 launch rover_navigation teleop.launch.py
```

**When you press W in Terminal 2, Terminal 1 should show:**
```
[serial_bridge]: Sent to ESP32: 0.50,0.00
```

**Terminal 2:**
```bash
cd ~/Documents/PlatformIO/Projects/RoverMini/ros2_ws
source install/setup.bash
ros2 run rover_control teleop_keyboard
```

---

## Most Common Solutions

### 1. **No Power to Motors**
→ Check power supply is ON and connected

### 2. **Missing Ground Connection**
→ ESP32 GND MUST connect to Motor Driver GND

### 3. **Enable Pins Need 5V**
→ Connect ENA/ENB directly to 5V instead of ESP32

### 4. **Wrong Serial Port**
→ Check with: `ls /dev/tty*` (look for ttyUSB0 or ttyACM0)

---

## What Should Happen

✅ **With test_serial.py:**
- See "ESP32 Response: L:0.50 R:0.50" 
- Motors spin forward 3 seconds
- Motors stop
- Motors rotate 3 seconds
- Motors stop

✅ **With ROS2:**
- Terminal 1 shows "Sent to ESP32: ..." when you press keys
- Motors respond immediately
- Press X to stop
