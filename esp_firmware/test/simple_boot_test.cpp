#include <Arduino.h>

// Motor A (Left)
const int IN1 = 12;
const int IN2 = 14;

// Motor B (Right)
const int IN3 = 26;
const int IN4 = 27;

void setup() {
  Serial.begin(115200);

  // Configure Motor Pins
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  Serial.println("Simple Motor Test - Motors will run on boot!");
  delay(1000);

  // Turn both motors FORWARD immediately
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);

  Serial.println("Motors should be running FORWARD now!");
}

void loop() {
  // Motors stay on
  delay(1000);
}
