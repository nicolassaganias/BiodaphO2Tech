/*
@project          : Life V2.0
@firmware version : v1.0.0 
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
  optaClientInit();          // initialize Opta client communication
  configNTPServer();  // configure ntp server to fetch real time and date
}

void loop() {
  runOptaClient();
  //if (wifiConnected && serverConnected) {
    //sendDataToServer();
  // }
  if (millis() - last_print > 3000) {
    Serial.println("Oxygen: " + String(readOxygenSensor()) + "\t\tEC: " + String(readConductivitySensor()) + "\t\tpH: " + String(readpHSensor()));
    last_print = millis();
  }

  if (millis() - prevEmailTime > EMAIL_DELAY) {
    sendSensorEmail(readOxygenSensor(), readConductivitySensor(), readpHSensor());
    prevEmailTime = millis();
  }
}

