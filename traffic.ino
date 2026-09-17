#include <WiFi.h>
#include <WebServer.h>

// Replace with your network credentials
const char* ssid = "Ganapathi";
const char* password = "GANAPATHI";

// Water level sensor analog pin
const int sensorPin = 34;  // ESP32 ADC pin
const int outputPin = 4;   // Pin that goes HIGH when flooded

WebServer server(80);

int sensorValue = 0;
int sensorPercent = 0;

void readSensor() {
  int total = 0;
  for (int i = 0; i < 20; i++) {
    total += analogRead(sensorPin);
    delay(10);
  }
  sensorValue = total / 20;
  sensorPercent = map(sensorValue, 0, 1800, 0, 100);
  sensorPercent = constrain(sensorPercent, 0, 100);

  // Pin 4 logic: HIGH if water level > 30%
  if (sensorPercent > 30) {
    digitalWrite(outputPin, HIGH);
  } else {
    digitalWrite(outputPin, LOW);
  }
}

void handleRoot() {
  readSensor();
  String html = "<html><body>";
  html += "<h1>Water Level Sensor</h1>";
  html += "<p>Raw Value: " + String(sensorValue) + "</p>";
  html += "<p>Percentage: " + String(sensorPercent) + "%</p>";
  html += "<p>Pin 4 Status: " + String(sensorPercent > 30 ? "HIGH (Flood)" : "LOW (Safe)") + "</p>";
  html += "<meta http-equiv='refresh' content='1'>";
  html += "</body></html>";
  server.send(200, "text/html", html);
}

void setup() {
  Serial.begin(9600);
  pinMode(sensorPin, INPUT);
  pinMode(outputPin, OUTPUT);
  digitalWrite(outputPin, LOW);

  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.begin();
  Serial.println("Web server started");
}

void loop() {
  server.handleClient();
  readSensor();  // keeps pin 4 updated even with no one viewing the page
  delay(200);
}