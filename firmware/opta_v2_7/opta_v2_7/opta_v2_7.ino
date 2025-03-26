#include "Settings.h"
#include "Sensor.h"
#include "OptaClient.h"

void setup() {
  delay(3000);
  digitalWrite(LED1, LOW);
  Serial.begin(115200);
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  pinMode(LED4, OUTPUT);
  analogReadResolution(12);  // set resolution 12-bit (don't change it)

  // Reset WiFi before attempting to connect
  WiFi.disconnect();  // Reset WiFi connection
  delay(1000);        // Short delay to ensure it fully resets
  connectToWiFi();    // Try connecting

  // Intentar conectar a WiFi hasta lograrlo
  while (WiFi.status() != WL_CONNECTED) {
    Serial.println("Attempting to connect to WiFi...");
    connectToWiFi();
    delay(5000);  // Esperar antes de reintentar
  }

  // Ensure NTP sync before proceeding
  Serial.println("Attempting to sync NTP...");
  syncNTP_UDP();
  delay(1000);

  // Send initial email after connection
  float oxygen = readOxygenSensor();
  float ec = readConductivitySensor();
  float ph = readpHSensor();
  sendFirstSensorDataEmail(oxygen,ec,ph);
  Serial.println("\nInitial sensor data email sent.\n");
}

void loop() {
  blinkLED(LED1, 5000);
  checkWiFiConnection();  // Revisa y reconecta sin delay()
  checkFailure();

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
  
  if(millis() - lastDataUpdate >= DATA_INTERVAL){
    lastDataUpdate = millis();
    float oxygen = readOxygenSensor();
    float ec = readConductivitySensor();
    float ph = readpHSensor();
    collectNewData(oxygen, ec, ph);
  }
}
