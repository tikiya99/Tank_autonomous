#!/usr/bin/env python3
"""
Direct serial test - bypasses ROS2 to test ESP32 communication
Usage: python3 test_serial.py
"""
import serial
import time

# Configure your serial port
PORT = '/dev/ttyUSB0'  # Change if needed
BAUD = 115200

print("=" * 50)
print("ESP32 Direct Serial Test")
print("=" * 50)
print(f"Connecting to {PORT} at {BAUD}...")

try:
    ser = serial.Serial(PORT, BAUD, timeout=1)
    time.sleep(2)  # Wait for ESP32 to reset
    print("✓ Connected!")
    print("\nSending test commands...\n")
    
    # Test 1: Forward
    print("Test 1: Forward (0.5, 0.0)")
    ser.write(b"0.50,0.00\n")
    time.sleep(0.5)
    
    # Read any response
    if ser.in_waiting:
        response = ser.readline().decode('utf-8').strip()
        print(f"  ESP32 Response: {response}")
    else:
        print("  No response from ESP32")
    
    time.sleep(3)
    
    # Test 2: Stop
    print("\nTest 2: Stop (0.0, 0.0)")
    ser.write(b"0.00,0.00\n")
    time.sleep(0.5)
    
    if ser.in_waiting:
        response = ser.readline().decode('utf-8').strip()
        print(f"  ESP32 Response: {response}")
    else:
        print("  No response from ESP32")
    
    time.sleep(2)
    
    # Test 3: Rotate
    print("\nTest 3: Rotate (0.0, 1.0)")
    ser.write(b"0.00,1.00\n")
    time.sleep(0.5)
    
    if ser.in_waiting:
        response = ser.readline().decode('utf-8').strip()
        print(f"  ESP32 Response: {response}")
    else:
        print("  No response from ESP32")
    
    time.sleep(3)
    
    # Final stop
    print("\nFinal: Stop")
    ser.write(b"0.00,0.00\n")
    
    print("\n" + "=" * 50)
    print("Test complete!")
    print("=" * 50)
    print("\nWhat to check:")
    print("1. Did motors move during tests?")
    print("2. Did you see ESP32 responses?")
    print("3. Check motor power supply is ON")
    print("4. Check all wire connections")
    
    ser.close()
    
except serial.SerialException as e:
    print(f"✗ Error: {e}")
    print("\nTroubleshooting:")
    print("1. Is ESP32 plugged in?")
    print("2. Is the port correct? Try: ls /dev/tty*")
    print("3. Close ROS2 serial_bridge if it's running")
    print("4. Try: sudo chmod 666 /dev/ttyUSB0")
except KeyboardInterrupt:
    print("\n\nTest interrupted by user")
    ser.write(b"0.00,0.00\n")  # Stop motors
    ser.close()
