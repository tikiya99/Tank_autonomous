# LD-19 LiDAR Setup Guide

## Wiring

### Connections

| LD-19 Pin | ESP32 Pin | Wire Color (typical) |
|-----------|-----------|---------------------|
| **GND**   | GND      | Black |
| **5V**    | 5V/VIN   | Red |
| **TX**    | **RX0 (GPIO 3)** | White/Green |
| **RX**    | Not connected | - |

> [!IMPORTANT]
> - LiDAR TX connects to ESP32 **RX0** (GPIO 3)
> - LiDAR needs **5V power** (NOT 3.3V)
> - **Do NOT connect RX pin** (not needed)
> - ESP32's RX0 is also used for USB serial, so you **cannot use Serial Monitor while LiDAR is connected**

---

## Step 1: Test LiDAR Hardware

### Upload Test Firmware

```bash
cd /home/thasinduwickrama/Documents/PlatformIO/Projects/RoverMini/esp_firmware

# Backup current firmware
cp src/main.cpp src/main.cpp.motors_working

# Use LiDAR test
cp test/lidar_test.cpp src/main.cpp

# Upload
pio run -t upload
```

### Disconnect LiDAR TX Temporarily

> [!WARNING]
> **Before monitoring:** Disconnect the LiDAR TX wire from ESP32 RX0, otherwise you won't see serial output!

```bash
# Monitor serial output (after disconnecting LiDAR TX)
pio device monitor
```

### Reconnect and Observe

1. **Disconnect USB** from ESP32
2. **Reconnect LiDAR TX** to RX0
3. **Reconnect USB** (or power ESP32)
4. **Listen for LiDAR motor** - you should hear it spinning

### Without Serial Monitor (Quick Test)

Alternative test without serial monitor:
1. Upload firmware
2. Keep LiDAR connected
3. **Listen** - LiDAR motor should spin constantly
4. **Wave hand** in front of LiDAR
5. If integrated later: rover should stop when hand is close

---

## Step 2: Integrate with Motor Control

Once LiDAR test works:

```bash
cd /home/thasinduwickrama/Documents/PlatformIO/Projects/RoverMini/esp_firmware

# Use integrated firmware (will be created)
cp src/main.cpp.motors_working src/main.cpp
# (After integration code is ready)
pio run -t upload
```

---

## Expected Behavior with Obstacle Avoidance

| Scenario | Rover Behavior |
|----------|---------------|
| **Path clear** | Moves normally with teleop |
| **Object 20cm ahead** | **Stops** when trying to move forward |
| **Object on side** | Moves forward normally |
| **Object removed** | **Resumes** normal movement |
| **Backing up** | Works even with  obstacle ahead |

---

## Safety Zones

- **< 15cm**: Emergency stop
- **15-30cm**: Stop forward movement
- **> 30cm**: No restrictions

Only monitors **front sector** (±15° from front).

---

## Troubleshooting

### LiDAR Not Spinning

- Check 5V power connection
- Verify GND connection
- Measure voltage at LiDAR pins (should be 5V)

### No Distance Readings

- Check TX wire connection to RX0 (GPIO 3)
- Verify baud rate is 230400
- Make sure LiDAR is spinning

### Rover Doesn't Stop

- Check that LiDAR data is being received
- Verify obstacle is in front sector (not side/back)
- Check safe distance threshold (default 30cm)

### Serial Monitor Shows Garbage

- This is normal if LiDAR TX is connected
- LiDAR sends binary data at 230400 baud
- Disconnect LiDAR TX to see normal serial output

---

## Quick Reference

**LiDAR Specs:**
- Model: LD-19 DTOF
- Range: 0.15m to 12m  
- Baud: 230400
- Power: 5V, ~140mA
- Scan rate: 10 Hz

**ESP32 Pins:**
- RX0 = GPIO 3
- 5V = VIN pin
- GND = any GND pin
