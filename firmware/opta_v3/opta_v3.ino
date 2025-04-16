#include "Settings.h"
#include "Sensor.h"
#include "OptaClient.h"

void setup() {
  digitalWrite(LED4, HIGH);
  WiFi.disconnect();
  delay(3000);

  Serial.begin(115200);
  Serial.println(STATION_NAME);
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  pinMode(LED4, OUTPUT);
  analogReadResolution(12);

  connectToWiFi();

  // Wait until WiFi is connected
  while (WiFi.status() != WL_CONNECTED) {
    Serial.println("Retrying WiFi...");
    delay(5000);
    connectToWiFi();
  }
  ntpUDP.begin(ntpPort);  // NTP setup, only done once
}

void loop() {
  digitalWrite(LED4, LOW);
  checkWiFiConnection();

  // LED indicator for NTP sync status
  if (ntpSynced == true) {
    digitalWrite(LED3, HIGH);
  } else {
    blinkLED(LED3, 1000);
  }

  // NTP Synchronization (only once at the beginning)
  if (!ntpSynced && WiFi.status() == WL_CONNECTED) {
    syncNTP_UDP();
    if (currentEpoch > 100000) {  // Check if NTP synced successfully
      ntpSynced = true;
      lastNTPUpdate = millis();
      Serial.println("✅ NTP synced on first loop.");
    } else {
      Serial.println("❌ NTP not synced yet. Waiting...");
      delay(3000);
      return;
    }
  }

  // Send first email (only once)
if (!initialEmailSent) {
  SensorData data;
  data.do1 = readOxygenSensor();
  data.do2 = readOxygenSensor2();
  data.ec1 = readConductivitySensor();
  data.ec2 = readConductivitySensor2();
  data.ph1 = readpHSensor();
  data.ph2 = readpHSensor2();

  sendFirstSensorDataEmail(data);
  initialEmailSent = true;
}

  // Visual indicator of operation
  blinkLED(LED1, 1000);

  // Re-sync NTP at regular intervals
  if ((millis() - lastNTPUpdate) > ntpUpdateInterval && ntpSynced) {
    Serial.println("⌛ Attempting NTP re-sync...");
    syncNTP_UDP();

    if (currentEpoch > 100000) {  // If the synchronization was successful
      lastNTPUpdate = millis();   // Update the last update time
      Serial.println("✅ NTP re-synced successfully.");
    } else {
      Serial.println("❌ NTP re-sync failed.");
      ntpSynced = false;  // Mark as unsynced
    }
  }

  // Periodic print to serial
if ((millis() - last_print) > PRINT_DELAY) {
  last_print = millis();
  Serial.println(
    "DO1: " + String(readOxygenSensor()) + " mg/L\t" +
    "DO2: " + String(readOxygenSensor2()) + " mg/L\t" +
    "EC1: " + String(readConductivitySensor()) + " uS/cm\t" +
    "EC2: " + String(readConductivitySensor2()) + " uS/cm\t" +
    "pH1: " + String(readpHSensor()) + "\t" +
    "pH2: " + String(readpHSensor2())
  );
}


  // Periodic email sending with sensor data
  if ((millis() - lastEmailSent) > EMAIL_INTERVAL) {
    sendSensorDataEmail();
    lastEmailSent = millis();
    Serial.println("Sensor data email sent.");
  }

  // Periodic data collection
  if ((millis() - lastDataUpdate) > DATA_INTERVAL) {
    lastDataUpdate = millis();

    SensorData data;
    data.do1 = readOxygenSensor();
    data.do2 = readOxygenSensor2();
    data.ec1 = readConductivitySensor();
    data.ec2 = readConductivitySensor2();
    data.ph1 = readpHSensor();
    data.ph2 = readpHSensor2();

    collectNewData(data);
  }

  // Check for system failures
  checkFailure();
}
