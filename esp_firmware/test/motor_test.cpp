#include <Arduino.h>

// Motor A (Left)
const int ENA = 13;
const int IN1 = 12;
const int IN2 = 14;

// Motor B (Right)
const int ENB = 25;
const int IN3 = 26;
const int IN4 = 27;

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("\n\n=================================");
  Serial.println("    ROVER MOTOR TEST PROGRAM");
  Serial.println("    (Digital ON/OFF Control)");
  Serial.println("=================================\n");

  // Configure Motor Pins (including enable pins)
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  // Enable motors (always ON for digital control)
  digitalWrite(ENA, HIGH);
  digitalWrite(ENB, HIGH);

  // Stop motors initially
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);

  Serial.println("Motor pins configured.");
  Serial.println("Motors will run at FULL SPEED (digital ON/OFF)");
  Serial.println("Starting motor tests in 2 seconds...\n");
  delay(2000);
}

void testMotor(String motorName, int in1, int in2) {
  Serial.println("-------------------------------");
  Serial.println("Testing " + motorName);
  Serial.println("-------------------------------");

  // Test Forward
  Serial.println("  Forward (FULL SPEED)...");
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  delay(2000);

  // Stop
  Serial.println("  Stopping...");
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  delay(1000);

  // Test Backward
  Serial.println("  Backward (FULL SPEED)...");
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  delay(2000);

  // Stop
  Serial.println("  Stopping...");
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  delay(1000);

  Serial.println("  " + motorName + " test complete!\n");
}

void loop() {
  Serial.println("\n\n=================================");
  Serial.println("  STARTING COMPREHENSIVE TEST");
  Serial.println("=================================\n");

  // Test Motor A (Left)
  testMotor("Motor A (Left)", IN1, IN2);

  delay(2000);

  // Test Motor B (Right)
  testMotor("Motor B (Right)", IN3, IN4);

  delay(2000);

  // Test Both Motors Together
  Serial.println("-------------------------------");
  Serial.println("Testing Both Motors Together");
  Serial.println("-------------------------------");

  Serial.println("  Both motors forward (FULL SPEED)...");
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  delay(3000);

  Serial.println("  Stopping...");
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  delay(1000);

  Serial.println("  Both motors backward (FULL SPEED)...");
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  delay(3000);

  Serial.println("  Stopping...");
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);

  Serial.println("\n=================================");
  Serial.println("    ALL TESTS COMPLETE!");
  Serial.println("=================================");
  Serial.println("\nWaiting 5 seconds before repeating...\n");

  delay(5000);
}
