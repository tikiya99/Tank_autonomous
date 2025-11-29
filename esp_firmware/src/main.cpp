#include <Arduino.h>

// Motor A (Left)
const int ENA = 14;
const int IN1 = 32;
const int IN2 = 33;

// Motor B (Right)
const int ENB = 27;
const int IN3 = 22;
const int IN4 = 23;

// LiDAR Settings
const float EMERGENCY_STOP = 0.12; // 12cm - STOP ALL MOVEMENT
const float SAFE_DISTANCE = 0.30;  // 30cm - block forward only

// LiDAR data (360° detection)
bool obstacleDetected = false;
bool emergencyStop = false;
float minDistance = 12.0;

// Store last command for continuous execution
float lastLinear = 0.0;
float lastAngular = 0.0;

// LD-19 packet parsing
const uint8_t PACKET_HEADER = 0x54;
const uint8_t PACKET_SIZE = 47;
uint8_t packetBuffer[PACKET_SIZE];
uint8_t bufferIndex = 0;

void setup() {
  // Serial for ROS2 (USB)
  Serial.begin(115200);

  // Serial2 for LiDAR (RX2=16, TX2=17)
  Serial2.begin(230400, SERIAL_8N1, 16, 17);

  // Motor pins
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  digitalWrite(ENA, HIGH);
  digitalWrite(ENB, HIGH);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);

  Serial.println("\n=== Rover + LiDAR ===");
  Serial.println("Serial: ROS2 commands");
  Serial.println("Serial2: LiDAR data");
  Serial.println("360° obstacle detection");
  Serial.println("=====================\n");
}

void setMotorDirection(int in1, int in2, float speed) {
  if (speed > 0.05) {
    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);
  } else if (speed < -0.05) {
    digitalWrite(in1, LOW);
    digitalWrite(in2, HIGH);
  } else {
    digitalWrite(in1, LOW);
    digitalWrite(in2, LOW);
  }
}

void parseLidarPacket() {
  // Check all 12 distances (360°)
  minDistance = 12.0;

  for (int i = 0; i < 12; i++) {
    uint8_t offset = 6 + (i * 3);
    uint16_t distance = (packetBuffer[offset + 1] << 8) | packetBuffer[offset];
    float dist_m = distance / 1000.0;

    if (dist_m > 0.01 && dist_m < 12.0 && dist_m < minDistance) {
      minDistance = dist_m;
    }
  }

  // Set flags based on distance
  emergencyStop = (minDistance < EMERGENCY_STOP);   // <12cm: STOP ALL
  obstacleDetected = (minDistance < SAFE_DISTANCE); // <30cm: block forward
}

void updateLidar() {
  int count = 0;
  while (Serial2.available() && count < 100) {
    uint8_t byte = Serial2.read();
    count++;

    if (bufferIndex == 0) {
      if (byte == PACKET_HEADER) {
        packetBuffer[0] = byte;
        bufferIndex = 1;
      }
    } else {
      packetBuffer[bufferIndex++] = byte;

      if (bufferIndex >= PACKET_SIZE) {
        parseLidarPacket();
        bufferIndex = 0;
      }
    }
  }
}

void loop() {
  // CRITICAL: Update LiDAR FIRST
  updateLidar();

  // Check for new commands
  if (Serial.available() > 0) {
    String input = Serial.readStringUntil('\n');
    input.trim();

    if (input.length() > 0) {
      int commaIndex = input.indexOf(',');
      if (commaIndex != -1) {
        lastLinear = input.substring(0, commaIndex).toFloat();
        lastAngular = input.substring(commaIndex + 1).toFloat();
      }
    }
  }

  // Calculate motor speeds
  float left_speed, right_speed;

  if (abs(lastAngular) > 0.3) {
    if (lastAngular > 0) {
      left_speed = -0.5;
      right_speed = 0.5;
    } else {
      left_speed = 0.5;
      right_speed = -0.5;
    }
  } else if (abs(lastLinear) > 0.1) {
    if (abs(lastAngular) > 0.1) {
      if (lastAngular > 0) {
        left_speed = 0.0;
        right_speed = lastLinear;
      } else {
        left_speed = lastLinear;
        right_speed = 0.0;
      }
    } else {
      left_speed = lastLinear;
      right_speed = lastLinear;
    }
  } else {
    left_speed = 0.0;
    right_speed = 0.0;
  }

  // OBSTACLE SAFETY
  if (emergencyStop) {
    // <12cm: STOP ALL
    left_speed = 0.0;
    right_speed = 0.0;

    static unsigned long lastEmergencyWarn = 0;
    if (millis() - lastEmergencyWarn > 500) {
      Serial.print("EMERGENCY ");
      Serial.print(minDistance * 100, 0);
      Serial.println("cm");
      lastEmergencyWarn = millis();
    }
  } else if (obstacleDetected && (left_speed > 0 || right_speed > 0)) {
    // 12-30cm: block forward
    left_speed = 0.0;
    right_speed = 0.0;

    static unsigned long lastWarn = 0;
    if (millis() - lastWarn > 1000) {
      Serial.print("BLOCKED ");
      Serial.print(minDistance * 100, 0);
      Serial.println("cm");
      lastWarn = millis();
    }
  }

  // Set motors
  setMotorDirection(IN1, IN2, left_speed);
  setMotorDirection(IN3, IN4, right_speed);

  // Run as fast as possible - NO DELAY
}
