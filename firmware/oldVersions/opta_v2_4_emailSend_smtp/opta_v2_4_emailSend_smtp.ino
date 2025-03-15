/*
@project          : Life V2.0
@firmware version : v3.2.3 
*/

#include "Settings.h"
#include "Sensor.h"
#include "OptaClient.h"

void setup() {
  Serial.begin(115200);
  analogReadResolution(12);  // set resolution 12-bit (don't change it)
  connectToWiFi();
  delay(100);

  // Initialize NTP
  if (initializeNTP()) {
    Serial.println("Time synchronization complete.");
  } else {
    Serial.println("Warning: Proceeding without NTP time sync.");
  }
  delay(100);
  // Send initial email after connection
  float oxygen = readOxygenSensor();
  float ec = readConductivitySensor();
  float ph = readpHSensor();
  sendSensorDataEmail(oxygen, ec, ph);
  Serial.println("Initial sensor data email sent.");
}

void loop() {
  checkWiFiConnection();  // Revisa y reconecta sin delay()

  if ((long)(millis() - last_print) > PRINT_DELAY) {  // Prints Sensor Values in Serial Monitor
    Serial.println("Oxygen: " + String(readOxygenSensor()) + "\t\tEC: " + String(readConductivitySensor()) + "\t\tpH: " + String(readpHSensor()));
    last_print = millis();
  }

  if ((long)(millis() - lastEmailSent) > EMAIL_INTERVAL) {  // Sends Sensor Values by e-mail once every EMAIL_INTERVAL
    float oxygen = readOxygenSensor();
    float ec = readConductivitySensor();
    float ph = readpHSensor();

    sendSensorDataEmail(oxygen, ec, ph);
    lastEmailSent = millis();
    Serial.println("Sensor data email sent.");
  }

  if ((long)(millis() - lastStatusEmailSent) > SYSTEM_STATUS_INTERVAL) {  // Check system health once a day
    //healthcheck();
  }
}