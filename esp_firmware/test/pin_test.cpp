#include <Arduino.h>

// Test the exact pins we're using for motors
const int IN1 = 12;
const int IN2 = 14;
const int IN3 = 26;
const int IN4 = 27;

void setup() {
  Serial.begin(115200);

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  Serial.println("GPIO Pin Test Starting...");
  Serial.println("Pins will blink in sequence:");
  Serial.println("IN1 (Pin 12), IN2 (Pin 14), IN3 (Pin 26), IN4 (Pin 27)");
  Serial.println("");
  Serial.println("If you have an LED or multimeter, check each pin!");
}

void loop() {
  Serial.println("IN1 (Pin 12) - HIGH");
  digitalWrite(IN1, HIGH);
  delay(1000);
  digitalWrite(IN1, LOW);
  delay(500);

  Serial.println("IN2 (Pin 14) - HIGH");
  digitalWrite(IN2, HIGH);
  delay(1000);
  digitalWrite(IN2, LOW);
  delay(500);

  Serial.println("IN3 (Pin 26) - HIGH");
  digitalWrite(IN3, HIGH);
  delay(1000);
  digitalWrite(IN3, LOW);
  delay(500);

  Serial.println("IN4 (Pin 27) - HIGH");
  digitalWrite(IN4, HIGH);
  delay(1000);
  digitalWrite(IN4, LOW);
  delay(500);

  Serial.println("");
  Serial.println("=== Sequence Complete ===");
  Serial.println("");
  delay(1000);
}
