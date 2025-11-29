#include <Arduino.h>

// Motor A (Left)
const int ENA = 14;
const int IN1 = 32;
const int IN2 = 33;

// Motor B (Right)
const int ENB = 27;
const int IN3 = 22;
const int IN4 = 23;

// PWM Properties
const int freq = 30000;
const int pwmChannelA = 0;
const int pwmChannelB = 1;
const int resolution = 8;

// Robot Parameters
// Adjust these based on your specific robot chassis
const float MAX_PWM = 255.0;
const float WHEEL_SEPARATION = 0.20; // Meters
const float WHEEL_RADIUS = 0.03;     // Meters

void setup() {
  Serial.begin(115200);

  // Configure Motor Pins
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  // Configure PWM
  ledcAttachChannel(ENA, freq, resolution, pwmChannelA);
  ledcAttachChannel(ENB, freq, resolution, pwmChannelB);

  // Stop motors initially
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  ledcWrite(ENA, 0);
  ledcWrite(ENB, 0);
}

void setMotorSpeed(int pin, int in1, int in2, int speed) {
  if (speed > 0) {
    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);
  } else if (speed < 0) {
    digitalWrite(in1, LOW);
    digitalWrite(in2, HIGH);
    speed = -speed;
  } else {
    digitalWrite(in1, LOW);
    digitalWrite(in2, LOW);
    speed = 0;
  }

  // Constrain speed to 0-255
  speed = constrain(speed, 0, 255);
  ledcWrite(pin, speed);
}

void loop() {
  if (Serial.available() > 0) {
    String input = Serial.readStringUntil('\n');
    input.trim();

    // Expected format: <linear_x,angular_z>
    // Example: 0.5,0.1
    if (input.length() > 0) {
      int commaIndex = input.indexOf(',');
      if (commaIndex != -1) {
        String linearStr = input.substring(0, commaIndex);
        String angularStr = input.substring(commaIndex + 1);

        float linear_x = linearStr.toFloat();
        float angular_z = angularStr.toFloat();

        // Differential Drive Kinematics
        // V_left = V - (omega * L / 2)
        // V_right = V + (omega * L / 2)
        // Note: This is a simplified model mapping directly to PWM for open
        // loop control. For better control, PID with encoders is recommended.

        // Simple mapping for open loop:
        // We map -1.0 to 1.0 range (approx) to PWM -255 to 255
        // This is a basic driver.

        float left_speed_cmd =
            linear_x - (angular_z * 0.5); // Simplified mixing
        float right_speed_cmd = linear_x + (angular_z * 0.5);

        // Map arbitrary units to PWM (assuming input is roughly -1.0 to 1.0)
        // You might need to tune this scaling factor
        int left_pwm = (int)(left_speed_cmd * 255.0);
        int right_pwm = (int)(right_speed_cmd * 255.0);

        setMotorSpeed(ENA, IN1, IN2, left_pwm);
        setMotorSpeed(ENB, IN3, IN4, right_pwm);

        // Debug output (optional, might interfere with high freq control)
        // Serial.print("L:"); Serial.print(left_pwm);
        // Serial.print(" R:"); Serial.println(right_pwm);
      }
    }
  }
}
