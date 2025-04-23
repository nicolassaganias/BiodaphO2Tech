#include "Settings.h"
#include "Sensor.h"
#include "OptaClient.h"

void setup() {
  digitalWrite(LED4, HIGH);

  WiFi.disconnect();
  delay(3000);

  Serial.begin(115200);
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  pinMode(LED4, OUTPUT);
  analogReadResolution(12);

  connectToWiFi();

  while (WiFi.status() != WL_CONNECTED) {
    Serial.println("Retrying WiFi...");
    delay(5000);
    connectToWiFi();
  }
  ntpUDP.begin(ntpPort);  // Solo se hace una vez
}
void loop() {
  digitalWrite(LED4, LOW);
  checkWiFiConnection();
  if (ntpSynced == true) {
    digitalWrite(LED3, HIGH);
  } else {
    blinkLED(LED3, 1000);
  }
  // Sincronización NTP (una sola vez al inicio)
  if (!ntpSynced && WiFi.status() == WL_CONNECTED) {
    syncNTP_UDP();
    if (currentEpoch > 100000) {  // Verificamos si el NTP se sincronizó correctamente
      ntpSynced = true;
      lastNTPUpdate = millis();
      Serial.println("✅ NTP synced on first loop.");
    } else {
      Serial.println("❌ NTP not synced yet. Waiting...");
      delay(3000);
      return;
    }
  }

  // Enviar primer email (una sola vez)
  if (!initialEmailSent) {
    float oxygen = readOxygenSensor();
    float ec = readConductivitySensor();
    float ph = readpHSensor();
    sendFirstSensorDataEmail(oxygen, ec, ph);
    Serial.println("✅ First sensor email sent.");
    initialEmailSent = true;
    lastEmailSent = millis();  // Evita que se dispare enseguida el siguiente
  }

  // Indicador visual de funcionamiento
  blinkLED(LED1, 5000);

  // Re-sincronizar NTP cada cierto intervalo
  if ((millis() - lastNTPUpdate) > ntpUpdateInterval) {
    syncNTP_UDP();
    lastNTPUpdate = millis();
  }

  // Print periódico por serial
  if ((millis() - last_print) > PRINT_DELAY) {
    Serial.println("Oxygen: " + String(readOxygenSensor()) + "\t\tEC: " + String(readConductivitySensor()) + "\t\tpH: " + String(readpHSensor()));
    last_print = millis();
  }

  // Envío periódico de email con datos
  if ((millis() - lastEmailSent) > EMAIL_INTERVAL) {
    sendSensorDataEmail();
    lastEmailSent = millis();
    Serial.println("Sensor data email sent.");
  }

  // Recolección periódica de datos
  if ((millis() - lastDataUpdate) > DATA_INTERVAL) {
    lastDataUpdate = millis();
    float oxygen = readOxygenSensor();
    float ec = readConductivitySensor();
    float ph = readpHSensor();
    collectNewData(oxygen, ec, ph);
  }
  checkFailure();
}
