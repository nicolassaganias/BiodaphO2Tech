#include <WiFi.h>

#define CLIENT_ID "OPTA T001"
#define WIFI_SSID "Can_Saguer_Wifi"
#define WIFI_PASSWORD "cansaguer2"

/* Sensor pins */
#define SENSOR_DISSOLVE_OXYGEN_PIN            A0        // Dissolve oxygen sensor kit
#define SENSOR_CONDUCTIVITY_PIN               A1        // Conductivity transmitter sensor kit
#define SENSOR_PH_PIN                         A2        // pH sensor kit

// data format
#define DATA_TYPE_OF_SENSOR_VALUE             "doubleValue"                // It is the data type of sensor's value
#define DELAY_BETWEEN_DATA_SENDING            5000                         // Delay between two consecutive data transmissions.

// device id
#define DISSOLVE_OXYGEN_SENSOR "OXT001" // must need an unique id for oxygen sensor
#define CONDUCTIVITY_SENSOR "ECT001"    // must need an unique id for conductivity sensor
#define PH_SENSOR "PHT001"              // must need an unique id for pH sensor

unsigned long last_update = 0;
unsigned long last_wifi_time = 0;
bool wifiConnected = false;

const unsigned long DELAY_BETWEEN_SENDING = 30000;  // 30 seconds
// const unsigned long DELAY_BETWEEN_EC_SENDING = 60000;      // 60 seconds
// const unsigned long DELAY_BETWEEN_PH_SENDING = 90000;      // 90 seconds

// wifi connection handler function
void connectToWiFi() {
  Serial.println("Intentando conectar a WiFi...");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);  // Iniciar conexión
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {  // Máximo 20 intentos
    delay(1000);
    Serial.print(".");
    attempts++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nConexión WiFi exitosa!");
    Serial.print("Dirección IP: ");
    Serial.println(WiFi.localIP());
    wifiConnected = true;
  } else {
    Serial.println("\nError: No se pudo conectar a WiFi. Verifique SSID/Contraseña.");
    wifiConnected = false;
  }
}
