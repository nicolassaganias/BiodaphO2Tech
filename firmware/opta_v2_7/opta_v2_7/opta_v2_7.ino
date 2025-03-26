#include "Settings.h"
#include "Sensor.h"
#include "OptaClient.h"

void setup() {
  digitalWrite(LED4, HIGH);  // Encender LED al inicio

  WiFi.disconnect();  // Reset WiFi connection
  delay(3000);

  Serial.begin(115200);
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  pinMode(LED4, OUTPUT);
  analogReadResolution(12);  // set resolution 12-bit (no cambiar)

  connectToWiFi();  // Primer intento

  // Reintentar si falla
  while (WiFi.status() != WL_CONNECTED) {
    Serial.println("Retrying WiFi...");
    delay(5000);
    connectToWiFi();
  }

  Serial.println("Attempting to sync NTP...");
  syncNTP_UDP();
  delay(1000);

  float oxygen = readOxygenSensor();
  float ec = readConductivitySensor();
  float ph = readpHSensor();
  sendFirstSensorDataEmail(oxygen, ec, ph);
  Serial.println("\nInitial sensor data email sent.\n");
}

void loop() {
  blinkLED(LED1, 5000);
  digitalWrite(LED4, LOW);
  checkWiFiConnection();  // Revisa y reconecta sin delay()
  //checkFailure();

  if ((long)(millis() - last_print) > PRINT_DELAY) {  // Prints Sensor Values in Serial Monitor
    Serial.println("Oxygen: " + String(readOxygenSensor()) + "\t\tEC: " + String(readConductivitySensor()) + "\t\tpH: " + String(readpHSensor()));
    last_print = millis();
  }

  if ((long)(millis() - lastEmailSent) > EMAIL_INTERVAL) {  // Sends Sensor Values by e-mail once every EMAIL_INTERVAL
    sendSensorDataEmail();
    lastEmailSent = millis();
    Serial.print("\nSensor data email sent.\n");
  }

  if (millis() - lastNTPUpdate >= ntpUpdateInterval) {
    syncNTP_UDP();
    lastNTPUpdate = millis();
  }

  if (millis() - lastDataUpdate >= DATA_INTERVAL) {
    lastDataUpdate = millis();
    float oxygen = readOxygenSensor();
    float ec = readConductivitySensor();
    float ph = readpHSensor();
    collectNewData(oxygen, ec, ph);
  }
}
