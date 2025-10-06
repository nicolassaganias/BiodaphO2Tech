/*
@project          : Life V2.0
@firmware version : v1.0.0 
*/

#include "Settings.h"
#include "GlobalTime.h"
#include "JsonMaker.h"
#include "Sensor.h"
#include "OptaClient.h"

unsigned long last_print = 0;

void setup() {
  Serial.begin(115200);
  analogReadResolution(12);  // set resolution 12-bit (don't change it)
  optaClientInit();          // initialize Opta client communication
  configNTPServer();  // configure ntp server to fetch real time and date
  timeClient.begin();       // start NTP client
}

void loop() {
  
  runOptaClient();
  if (wifiConnected && serverConnected) {
    sendDataToServer();
  }
  if (millis() - last_print > 3000) {
timeClient.update();
    time_t rawTime = timeClient.getEpochTime();
    struct tm *timeInfo = gmtime(&rawTime + 3600);

    char timeBuffer[30];
    snprintf(timeBuffer, sizeof(timeBuffer), "%02d-%02d-%04d %02d:%02d:%02d", 
             timeInfo->tm_mday, timeInfo->tm_mon + 1, timeInfo->tm_year + 1900, 
             timeInfo->tm_hour, timeInfo->tm_min, timeInfo->tm_sec);

    Serial.print("Time: ");
    Serial.print(timeBuffer);
    Serial.print("  Oxygen: ");
    Serial.print(readOxygenSensor());
    Serial.print("  EC: ");
    Serial.print(readConductivitySensor());
    Serial.print("  pH: ");
    Serial.println(readpHSensor());

    last_print = millis();
  }
}