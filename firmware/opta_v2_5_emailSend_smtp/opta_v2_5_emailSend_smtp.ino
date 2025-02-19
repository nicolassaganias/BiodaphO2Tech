/*
@project          : Life V2.0
@firmware version : v3.2.3 
*/

#include "Settings.h"
#include "Sensor.h"
#include "OptaClient.h"

void setup() {
  delay(3000);
  Serial.begin(115200);
  analogReadResolution(12);  // set resolution 12-bit (don't change it)
  connectToWiFi();
  delay(100);

  timeClient.begin();
  syncNTP();
  delay(1000);
  // Send initial email after connection
  float oxygen = readOxygenSensor();
  float ec = readConductivitySensor();
  float ph = readpHSensor();
  sendSensorDataEmail(oxygen, ec, ph);
  Serial.println("\nInitial sensor data email sent.\n");
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
    Serial.print("\nSensor data email sent.\n");
  }

  if (millis() - lastNTPUpdate >= ntpUpdateInterval) {
    syncNTP();
    lastNTPUpdate = millis();
  }
}
