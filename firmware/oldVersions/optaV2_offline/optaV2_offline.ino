/*
@project          : Life V2.0
@firmware version : v1.0.0 
*/

#include "Settings.h"
#include "Sensor.h"
#include "GlobalTime.h"
#include <FirebaseClient.h>
#include <WiFi.h> // Ensure you include the appropriate WiFi library for Opta WiFi

unsigned long last_print = 0;

void setup() {
  Serial.begin(115200);
  analogReadResolution(12);  // set resolution 12-bit (don't change it)
  connectToWiFi();
  configNTPServer();  // configure ntp server to fetch real time and date
}

void loop() {

  if (millis() - last_print > 3000) {
    // Read sensor values
    float oxygen = readOxygenSensor();
    float ec = readConductivitySensor();
    float pH = readpHSensor();

    // Fetch current time in ISO 8601 format
    String currentTime = getGlobalTime();

    // Split currentTime into date and time
    String date = currentTime.substring(8, 10) + "/" + currentTime.substring(5, 7) + "/" + currentTime.substring(0, 4);
    String time = currentTime.substring(11, 19);

    // Print sensor values with labels and formatted time
    Serial.println("Oxygen: " + String(oxygen) + "\t\tEC: " + String(ec) + "\t\tpH: " + String(pH) + "\t\tDate: " + date + "\t\tTime: " + time);

    last_print = millis();
  }
}