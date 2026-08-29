#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

// WiFi Configuration
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

WebServer server(80);

// Pin Definitions
#define ENA 13
#define IN1 12
#define IN2 14
#define IN3 27
#define IN4 26
#define ENB 25

#define MQ_PIN 34
#define US_TRIG 5
#define US_ECHO 18

Adafruit_MPU6050 mpu;

// Navigation & Telemetry State
float posX = 0.0, posY = 0.0;
float depthMeters = 0.0;
float totalDistance = 0.0;
float currentHeading = 0.0;
unsigned long lastTelemetryUpdate = 0;

void setup() {
  Serial.begin(115200);

  pinMode(ENA, OUTPUT); pinMode(IN1, OUTPUT); pinMode(IN2, OUTPUT);
  pinMode(ENB, OUTPUT); pinMode(IN3, OUTPUT); pinMode(IN4, OUTPUT);
  stopMotors();

  pinMode(US_TRIG, OUTPUT);
  pinMode(US_ECHO, INPUT);

  Wire.begin();
  if (!mpu.begin()) {
    Serial.println("Warning: MPU6050 IMU initialization failed!");
  }

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi Connected. IP: " + WiFi.localIP().toString());

  server.on("/cmd", HTTP_GET, handleCommand);
  server.on("/telemetry", HTTP_GET, handleTelemetry);
  
  server.enableCORS(true);
  server.begin();
}

void loop() {
  server.handleClient();
  
  if (millis() - lastTelemetryUpdate > 100) {
    updateNavigationData();
    lastTelemetryUpdate = millis();
  }
}

void setMotorSpeeds(int speedA, int speedB, bool dirA1, bool dirA2, bool dirB1, bool dirB2) {
  analogWrite(ENA, speedA);
  analogWrite(ENB, speedB);
  digitalWrite(IN1, dirA1);
  digitalWrite(IN2, dirA2);
  digitalWrite(IN3, dirB1);
  digitalWrite(IN4, dirB2);
}

void stopMotors() {
  setMotorSpeeds(0, 0, LOW, LOW, LOW, LOW);
}

void handleCommand() {
  String dir = server.arg("dir");
  int speed = server.arg("speed").toInt();
  if (speed <= 0) speed = 200;

  if (dir == "forward") setMotorSpeeds(speed, speed, HIGH, LOW, HIGH, LOW);
  else if (dir == "backward") setMotorSpeeds(speed, speed, LOW, HIGH, LOW, HIGH);
  else if (dir == "left") setMotorSpeeds(speed, speed, LOW, HIGH, HIGH, LOW);
  else if (dir == "right") setMotorSpeeds(speed, speed, HIGH, LOW, LOW, HIGH);
  else stopMotors();

  server.send(200, "text/plain", "OK");
}

float readUltrasonicDistance() {
  digitalWrite(US_TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(US_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(US_TRIG, LOW);
  long duration = pulseIn(US_ECHO, HIGH, 25000);
  if (duration == 0) return 0.0;
  return (duration * 0.0343) / 2.0 / 100.0;
}

void updateNavigationData() {
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  float gz = g.gyro.z; 
  if (abs(gz) > 0.05) {
    currentHeading += gz * 0.1; 
  }

  float stepDist = 0.02;
  
  if (digitalRead(IN1) == HIGH && digitalRead(IN2) == LOW) {
    posX += stepDist * cos(currentHeading);
    posY += stepDist * sin(currentHeading);
    totalDistance += stepDist;
  }
  
  depthMeters = readUltrasonicDistance();
}

void handleTelemetry() {
  int rawGas = analogRead(MQ_PIN);
  float gasPPM = map(rawGas, 0, 4095, 0, 1000);

  StaticJsonDocument<256> doc;
  doc["x"] = posX;
  doc["y"] = posY;
  doc["depth"] = depthMeters;
  doc["distance"] = totalDistance;
  doc["gasPPM"] = gasPPM;
  doc["battery"] = 12.2;

  String jsonStr;
  serializeJson(doc, jsonStr);

  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(200, "application/json", jsonStr);
}
