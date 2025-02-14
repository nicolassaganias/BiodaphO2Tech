/*
@project          : Life V2.0
@firmware version : v3.2.3 
*/

#include "Settings.h"
#include "GlobalTime.h"
#include "Sensor.h"
#include "OptaClient.h"

unsigned long last_print = 0;
unsigned long prevEmailTime = 0;

void setup() {
  Serial.begin(115200);
  analogReadResolution(12);  // set resolution 12-bit (don't change it)
  connectToWiFi();
  configNTPServer();  // configure ntp server to fetch real time and date


  // Send initial email after connection
  float oxygen = readOxygenSensor();
  float ec = readConductivitySensor();
  float ph = readpHSensor();
  sendSensorDataEmail(oxygen, ec, ph);
  Serial.println("Initial sensor data email sent.");
}

void loop() {
  checkWiFiConnection();  // Revisa y reconecta sin delay()
  checkAndSendEmail();

  if ((long)(millis() - last_print) > PRINT_DELAY) {
    Serial.println("Oxygen: " + String(readOxygenSensor()) + "\t\tEC: " + String(readConductivitySensor()) + "\t\tpH: " + String(readpHSensor()));
    last_print = millis();
  }
}
