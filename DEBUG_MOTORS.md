# Motor Not Running - Debug Guide

## Current Status

✅ Serial bridge connected to ESP32  
✅ Teleop keyboard accepting commands  
❌ Motors not responding  

---

## Step-by-Step Debugging

### Step 1: Verify ROS2 Messages Are Being Published

In a **3rd terminal**, check if teleop is publishing messages:

```bash
cd /home/thasinduwickrama/Documents/PlatformIO/Projects/RoverMini/ros2_ws
source install/setup.bash
ros2 topic echo /cmd_vel
```

**Then in Terminal 2 (teleop), press W**

You should see output like:
```
linear:
  x: 0.5
  y: 0.0
  z: 0.0
angular:
  x: 0.0
  y: 0.0
  z: 0.0
```

**If you DON'T see this**, the teleop keyboard isn't working. Make sure you're typing in the correct terminal.

---

### Step 2: Check Serial Bridge Debug Output

**After rebuilding** (I just enabled debug logging), restart your system:

**Terminal 1:**
```bash
cd /home/thasinduwickrama/Documents/PlatformIO/Projects/RoverMini/ros2_ws
source install/setup.bash
ros2 launch rover_navigation teleop.launch.py
```

**Terminal 2:**
```bash
cd /home/thasinduwickrama/Documents/PlatformIO/Projects/RoverMini/ros2_ws
source install/setup.bash
ros2 run rover_control teleop_keyboard
```

**Now when you press W in Terminal 2, you should see in Terminal 1:**
```
[serial_bridge]: Sent to ESP32: 0.50,0.00
```

**If you see this**, the serial bridge IS sending commands to the ESP32!

**If you DON'T see this**, there's a problem with the ROS2 setup.

---

### Step 3: Re-upload ESP32 Firmware with Debug

I've enabled debug output in the ESP32 code. Upload it:

```bash
cd /home/thasinduwickrama/Documents/PlatformIO/Projects/RoverMini/esp_firmware
pio run -t upload
```

---

### Step 4: Check Hardware Connections

If messages are being sent but motors still don't move, check:

#### Power Supply
- [ ] **Motor driver has separate power supply** (ESP32 can't power motors)
- [ ] **Common ground** between ESP32 and motor driver
- [ ] **Power supply is ON** and sufficient voltage (typically 6-12V for DC motors)

#### Wiring Verification

**ESP32 → Motor Driver:**
```
ESP32 Pin 13 (ENA) → Motor Driver ENA (Enable A)
ESP32 Pin 12 (IN1) → Motor Driver IN1
ESP32 Pin 14 (IN2) → Motor Driver IN2
ESP32 Pin 25 (ENB) → Motor Driver ENB (Enable B)
ESP32 Pin 26 (IN3) → Motor Driver IN3
ESP32 Pin 27 (IN4) → Motor Driver IN4
ESP32 GND → Motor Driver GND (CRITICAL!)
```

**Motor Driver → Motors:**
```
OUT1 & OUT2 → Motor A (Left)
OUT3 & OUT4 → Motor B (Right)
```

**Motor Driver Power:**
```
+12V (or appropriate voltage) → Motor Driver VCC/VM
GND → Motor Driver GND
```

---

### Step 5: Test Motors Manually

To verify the motor hardware works, test with the motor test firmware:

```bash
cd /home/thasinduwickrama/Documents/PlatformIO/Projects/RoverMini/esp_firmware

# Backup current firmware
cp src/main.cpp src/main.cpp.teleop_backup

# Use motor test
cp test/motor_test.cpp src/main.cpp

# Upload
pio run -t upload

# Monitor (disconnect ROS2 first!)
# Stop Terminal 1 (Ctrl+C)
pio device monitor
```

You should see motors running and test output. **If motors don't run here**, it's a hardware issue!

After testing, restore teleop firmware:
```bash
cp src/main.cpp.teleop_backup src/main.cpp
pio run -t upload
```

---

### Step 6: Common Issues

#### Issue: Enable Pins Not Working

Some motor drivers need **5V on enable pins**, not 3.3V from ESP32.

**Quick Fix:** Jumper the enable pins
- Remove wires from ENA and ENB
- Use a jumper to connect ENA and ENB directly to **5V** or **VCC** on the motor driver
- Comment out enable pin code in ESP32:

```cpp
// In setup(), comment out:
// digitalWrite(ENA, HIGH);
// digitalWrite(ENB, HIGH);
```

#### Issue: Wrong Pin Numbers

Double-check your ESP32 pinout. Some boards label pins differently!

#### Issue: Motor Direction Only (No Enable)

If your motor driver doesn't use enable pins (some use PWM directly):
- Connect motors' enable pins to high voltage permanently
- Only use IN1, IN2, IN3, IN4 for direction

---

## Quick Checklist

- [ ] ROS2 messages publishing (`ros2 topic echo /cmd_vel` shows data)
- [ ] Serial bridge shows "Sent to ESP32" when pressing keys
- [ ] ESP32 firmware uploaded with latest code
- [ ] Motor power supply connected and ON
- [ ] Common ground between ESP32 and motor driver
- [ ] All 7 wires connected (ENA, IN1, IN2, ENB, IN3, IN4, GND)
- [ ] Motor test firmware can run motors
- [ ] Enable pins at correct voltage (3.3V or 5V depending on driver)

---

## Expected Behavior When Everything Works

1. Press **W** in Terminal 2 (teleop)
2. Terminal 1 shows: `[serial_bridge]: Sent to ESP32: 0.50,0.00`
3. **Motors start spinning forward**
4. Press **X** to stop
5. Terminal 1 shows: `[serial_bridge]: Sent to ESP32: 0.00,0.00`
6. **Motors stop**

---

## Need More Help?

Run all these commands and share the output:

```bash
# Check ROS2 topics
ros2 topic list

# Check cmd_vel messages (while pressing W)
ros2 topic echo /cmd_vel --once

# Check serial bridge node
ros2 node list

# Check if ESP32 is connected
ls -la /dev/ttyUSB* /dev/ttyACM*
```
