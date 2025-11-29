#include <Arduino.h>
#include <LDRobotLidarDriver.h>

// Using Serial (RX0/TX0) for LiDAR
// RX0 = GPIO 3 (receives LiDAR TX data)
// TX0 = GPIO 1 (not used, but part of Serial)
LDRobotLidarDriver lidar;

void setup() {
  // Serial for debugging (USB)
  Serial.begin(115200);
  delay(2000);

  Serial.println("\n=================================");
  Serial.println("   LD-19 LiDAR Test");
  Serial.println("=================================\n");

  // Note: Serial for LiDAR is already initialized by LDRobotLidarDriver
  // at 230400 baud on RX0/TX0

  Serial.println("Initializing LD-19 LiDAR...");

  // Initialize LiDAR on Serial (RX0=GPIO3, TX0=GPIO1)
  if (lidar.init()) {
    Serial.println("✓ LD-19 initialized successfully!");
  } else {
    Serial.println("✗ Failed to initialize LD-19");
    Serial.println("Check connections:");
    Serial.println("  - LiDAR TX → ESP32 RX0 (GPIO 3)");
    Serial.println("  - LiDAR GND → ESP32 GND");
    Serial.println("  - LiDAR 5V → ESP32 5V");
  }

  Serial.println("\nStarting scan...");
  Serial.println("You should hear the LiDAR motor spinning.");
  Serial.println("");
}

void loop() {
  if (lidar.dataReceiveOk()) {
    // Get scan data
    LDRobotLidarDriver::PointData data[lidar.POINTS_PER_PACK];
    lidar.getPointData(data);

    // Find minimum distance in each sector
    float minFrontDist = 12.0;
    float minLeftDist = 12.0;
    float minRightDist = 12.0;
    float minBackDist = 12.0;

    for (int i = 0; i < lidar.POINTS_PER_PACK; i++) {
      float angle = data[i].angle;
      float dist = data[i].distance / 1000.0; // mm to meters

      if (dist > 0.01 && dist < 12.0) { // Valid reading
        // Front: 345° to 15°
        if (angle > 345 || angle < 15) {
          if (dist < minFrontDist)
            minFrontDist = dist;
        }
        // Right: 75° to 105°
        else if (angle > 75 && angle < 105) {
          if (dist < minRightDist)
            minRightDist = dist;
        }
        // Back: 165° to 195°
        else if (angle > 165 && angle < 195) {
          if (dist < minBackDist)
            minBackDist = dist;
        }
        // Left: 255° to 285°
        else if (angle > 255 && angle < 285) {
          if (dist < minLeftDist)
            minLeftDist = dist;
        }
      }
    }

    // Print sector distances every 500ms
    static unsigned long lastPrint = 0;
    if (millis() - lastPrint > 500) {
      Serial.print("Front: ");
      Serial.print(minFrontDist, 2);
      Serial.print("m | Right: ");
      Serial.print(minRightDist, 2);
      Serial.print("m | Back: ");
      Serial.print(minBackDist, 2);
      Serial.print("m | Left: ");
      Serial.print(minLeftDist, 2);
      Serial.println("m");

      // Obstacle warning
      if (minFrontDist < 0.30) {
        Serial.println("  ⚠️  OBSTACLE AHEAD!");
      }

      lastPrint = millis();
    }
  }

  // Small delay to prevent overwhelming
  delay(10);
}
