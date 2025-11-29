# Manual LiDAR Parsing (No Library)

## Changes Made

Switched from library-based to **manual packet parsing**:

✅ **No external dependencies**  
✅ **Full control over data processing**  
✅ **Smaller code size**  
✅ **Uses Serial2 (RX2=GPIO16, TX2=GPIO17) instead of RX0**

---

## New Wiring

| LD-19 Pin | ESP32 Pin | Notes |
|-----------|-----------|-------|
| GND | GND | Ground |
| 5V | 5V/VIN | Power |
| TX | **GPIO 16 (RX2)** | Changed from RX0! |
| RX | Not connected | - |

> [!IMPORTANT]
> **Now using Serial2 (GPIO 16) instead of RX0 (GPIO 3)**
> 
> **Benefits:**
> - ✅ Serial Monitor works normally (no conflict)
> - ✅ Can debug while LiDAR is connected
> - ✅ Serial for ROS2, Serial2 for LiDAR

---

## How It Works

### LD-19 Packet Format (47 bytes)

```
Byte 0:    0x54 (Header)
Byte 1:    0x2C (Version/Length)
Byte 2-3:  Speed (rotation speed)
Byte 4-5:  Start Angle
Byte 6-41: 12 measurement points (3 bytes each)
           [Distance_Low, Distance_High, Intensity]
Byte 42-43: End Angle
Byte 44-45: Timestamp
Byte 46:   CRC checksum
```

### Manual Parsing Steps

1. **Read from Serial2** (230400 baud)
2. **Find packet header** (0x54)
3. **Collect 47 bytes**
4. **Extract 12 distance points**
5. **Calculate angles** for each point
6. **Check front sector** (345° to 15°)
7. **Find minimum distance**
8. **Set obstacle flag**

---

## Code Structure

### Main Components

```cpp
// Serial for ROS2 commands
Serial.begin(115200);

// Serial2 for LiDAR data
Serial2.begin(230400, SERIAL_8N1, 16, 17);
```

### Packet Parsing

```cpp
void updateLidar() {
    // Read bytes from Serial2
    // Find header 0x54
    // Collect 47 bytes
    // Call parseLidarPacket()
}

void parseLidarPacket() {
    // Extract start/end angles
    // Process 12 measurement points
    // Check front sector
    // Update minFrontDistance
}
```

---

## Testing

### Upload New Firmware

```bash
cd /home/thasinduwickrama/Documents/PlatformIO/Projects/RoverMini/esp_firmware
pio run -t upload
```

### Wire LiDAR to GPIO 16 (Serial2)

**Move the TX wire:**
- **OLD:** LiDAR TX → ESP32 RX0 (GPIO 3)
- **NEW:** LiDAR TX → ESP32 GPIO 16 (RX2)

### Test with Serial Monitor

```bash
pio device monitor
```

**Now you can:**
- See startup messages
- See obstacle warnings
- Debug while LiDAR connected
- No garbage data!

### Test Obstacle Detection

Run ROS2 teleop:
```bash
# Terminal 1
ros2 launch rover_navigation teleop.launch.py

# Terminal 2  
ros2 run rover_control teleop_keyboard
```

Place object 20cm in front → Rover should stop

---

## Advantages

| Manual Parsing | Library |
|----------------|---------|
| ✅ No dependencies | ❌ Needs library |
| ✅ Smaller code | ❌ Larger |
| ✅ Full control | ❌ Black box |
| ✅ Serial2 available | ❌ Must use RX0 |
| ✅ Easy debugging | ❌ Serial conflict |

---

## Configuration

### Change Safe Distance

```cpp
const float SAFE_DISTANCE = 0.30;  // 30cm (change as needed)
```

### Change Detection Sector

```cpp
// In parseLidarPacket():
if ((angle > 345.0 || angle < 15.0) && ...) {
    // Front ±15°
}

// For wider sector (±30°):
if ((angle > 330.0 || angle < 30.0) && ...) {
```

---

## Summary

✅ **Switched to manual parsing**  
✅ **No library needed**  
✅ **Serial2 (GPIO 16) - no Serial Monitor conflict**  
✅ **Full debugging capability**  
✅ **Same obstacle detection functionality**

**Just rewire LiDAR TX to GPIO 16 and upload!**
