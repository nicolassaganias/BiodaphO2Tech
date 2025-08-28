#include "OptaBlue.h"
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

#define BBT "mqtt.beebotte.com"
#define TOKEN "token:token_5iHgg9PJs6OoYQM6"
#define Channel "OptaAntisa"

// Recursos usados - canales
#define RES_AS_DO1 "AS_DO1_GR"
#define RES_AS_PH1 "AS_PH1_GR"
#define RES_AS_EC1 "AS_EC1_GR"
#define RES_ALL_READINGS "All_readings"

#define Write true
#define Publ false

// === Intercambio de pines: ahora pH en A1 y EC en A0 ===
#define PH_PIN A1
#define EC_PIN A0

int status = WL_IDLE_STATUS;
float lec_AS_DO1_GR, lec_AS_PH1_GR, lec_AS_EC1_GR;

// const char* ssid = "Outdoor-WiFi-255BC6";
// const char* password = "oT0,2LiM-WlZ";
const char* ssid = "Can_Saguer_Wifi";
const char* password = "cansaguer2";

WiFiClient wifiClient;
PubSubClient client(wifiClient);

// Tiempos
long lastReconnectAttempt = 0;
const long interval = 60000;        // cada cuanto guarda/publica datos (1 min)
const long intervalPrint = 2000;    // cada cuanto hace prints (5 s) -> cambiar a 1000 si querés 1 s

unsigned long lastReadingMillis = 0;
unsigned long lastPrintMillis = 0;

// Para ID MQTT
const char chars[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890";
char id[17];

// Control de persistencia
int cnt = 0;            // Contador para hacer una escritura por cada x publicaciones
int pub = 10;           // Cada cuantas publicaciones se hace una escritura persistente
bool PubWrite = false;  // True = persistente, false = sin persistencia

// RAWs (ADC 16 bits)
uint16_t raw_A0 = 0;
uint16_t raw_A1 = 0;

#define PERIODIC_UPDATE_TIME 500
#define DELAY_AFTER_SETUP 5000

/* -------------------------------------------------------------------------- */
/*                                 SETUP                                      */
/* -------------------------------------------------------------------------- */
void setup() {
  Serial.begin(115200);
  delay(2000);
  pinMode(LED_D0, OUTPUT);
  pinMode(LED_USER, OUTPUT);
  digitalWrite(LED_D0, LOW);
  digitalWrite(LED_USER, LOW);

  // CANALES ANALOGICOS
  analogReadResolution(16);  // 0..65535

  // WiFi
  Serial.println("Conectando a WiFi");
  while (status != WL_CONNECTED) {
    Serial.println("Intentando conectar al SSID existente: ");
    Serial.println(ssid);
    status = WiFi.begin(ssid, password);
    delay(10000);
  }
  Serial.println("\nWiFi conectado, IP: ");
  Serial.println(WiFi.localIP());

  client.setServer(BBT, 1883);  // MQTT sin TLS
  lastReconnectAttempt = 0;
}

/* -------------------------------------------------------------------------- */
// Lee 1 vez, guarda RAW según pin y devuelve mapeo 0..10V.
// Mantengo los prints de debug "analog map" y "analog raw" como tenías.
float lecturaAnalog(uint8_t pin) {
  uint16_t raw = analogRead(pin);              // valor crudo 0..65535
  float mapped = raw * 10.0 / 65535.0;         // 0..10V con módulo 0-10V

  if (pin == A0) raw_A0 = raw;
  if (pin == A1) raw_A1 = raw;

  Serial.print("analog map:");
  Serial.print(mapped);
  Serial.print("   analog raw:");
  Serial.println(raw);

  return mapped;
}

/* -------------------------------------------------------------------------- */
/*                                  LOOP                                      */
/* -------------------------------------------------------------------------- */
void loop() {
  if (!client.connected()) {
    Serial.println("DESCONEXION");
    long now = millis();
    if (now - lastReconnectAttempt > 10000) {
      lastReconnectAttempt = now;
      digitalWrite(LED_USER, LOW);
      if (reconnect()) {
        lastReconnectAttempt = 0;
      }
    }
  } else {
    unsigned long currentMillis = millis();

    // ---- PUBLICACIÓN A BEEBOTTE CADA 1 MINUTO ----
    if (currentMillis - lastReadingMillis >= interval) {
      lastReadingMillis = currentMillis;
      if (cnt == pub) {
        PubWrite = true;
      }

      // Iniciamos lectura de sensores y publicamos
      digitalWrite(LED_D0, HIGH);

      // PH ahora en PH_PIN (A1)
      lec_AS_PH1_GR = escalarValores(lecturaAnalog(PH_PIN), 0.0, 10.0, 0.0, 14.0);
      if (!isnan(lec_AS_PH1_GR)) {
        publish(RES_AS_PH1, lec_AS_PH1_GR, PubWrite);
        Serial.print("PH Atlas Scientific 1: ");
        Serial.print(lec_AS_PH1_GR, 2);
        Serial.println("");
      }
      delay(30);

      // EC ahora en EC_PIN (A0)
      lec_AS_EC1_GR = escalarValores(lecturaAnalog(EC_PIN), 0.0, 9.3, 0.0, 25000.0);
      if (!isnan(lec_AS_EC1_GR)) {
        publish(RES_AS_EC1, lec_AS_EC1_GR, PubWrite);
        Serial.print("EC Atlas Scientific 1: ");
        Serial.print(lec_AS_EC1_GR, 2);
        Serial.println(" mS/cm");
        Serial.println("");
      }
      delay(30);

      // Armar string separado por comas
      String all_readings = String(lec_AS_PH1_GR, 1) + "," + String(lec_AS_EC1_GR, 1);
      publish(RES_ALL_READINGS, all_readings, false);  // Publicar string completo
      Serial.print("All:");
      Serial.println(all_readings);
      cnt++;

      if (cnt > pub) {
        PubWrite = false;
        cnt = 0;
      }
      digitalWrite(LED_D0, LOW);
    }

    // ---- PRINT INDEPENDIENTE CADA 5 SEGUNDOS (LECTURA FRESCA + RAW) ----
    if (currentMillis - lastPrintMillis >= intervalPrint) {
      lastPrintMillis = currentMillis;

      // Re-lectura rápida solo para mostrar por Serial (no publica)
      float phTemp = escalarValores(lecturaAnalog(PH_PIN), 0.0, 9.75, 0.0, 14.0);
      float ecTemp = escalarValores(lecturaAnalog(EC_PIN), 0.0, 9.3, 0.0, 25000.0);

      Serial.println("----- Print periódico -----");
      Serial.print("PH Atlas Scientific 1: ");
      Serial.print(phTemp, 2);
      Serial.print("   (raw ");
      Serial.print((PH_PIN == A1) ? raw_A1 : raw_A0);
      Serial.println(")");

      Serial.print("EC Atlas Scientific 1: ");
      Serial.print(ecTemp, 2);
      Serial.print(" mS/cm   (raw ");
      Serial.print((EC_PIN == A0) ? raw_A0 : raw_A1);
      Serial.println(")");
      Serial.println("---------------------------");
    }

    client.loop();  // Mantiene el ping MQTT activo
  }
}

/* -------------------------------------------------------------------------- */
bool reconnect() {
  // Intento de conexión con ID dinámico y usando solo el token como auth
  if (client.connect(generateID(), TOKEN, "")) {
    Serial.println("Conectado a Beebotte MQTT");
    digitalWrite(LED_USER, HIGH);
    delay(100);
    return client.connected();
  }
  return false;
}

float escalarValores(float valor, float min_ing, float max_ing, float min, float max) {
  // map para float
  return (valor - min_ing) * (max - min) / (max_ing - min_ing) + min;
}

void publish(const char* resource, float data, bool persist) {
  // JSON para Beebotte (ArduinoJson versión vieja)
  StaticJsonBuffer<128> jsonOutBuffer;
  JsonObject& root = jsonOutBuffer.createObject();
  root["channel"] = Channel;
  root["resource"] = resource;
  if (persist) root["write"] = true;
  root["data"] = data;

  char buffer[128];
  root.printTo(buffer, sizeof(buffer));  // Serializar a texto

  char topic[64];
  sprintf(topic, "%s/%s", Channel, resource);  // Topic para Beebotte

  Serial.println("-------");
  Serial.print("Publicados en ");
  Serial.print(String(resource));
  Serial.print(" los datos ");
  Serial.print(String(data, 1));
  Serial.print(" con persistencia ");
  if (persist) {
    Serial.println(" True");
  } else {
    Serial.println(" False");
  }
  Serial.println("-------");

  client.publish(topic, buffer);  // Publicar por MQTT
}

// Sobrecarga para enviar String (All_readings)
void publish(const char* resource, String data, bool persist) {
  StaticJsonBuffer<128> jsonOutBuffer;
  JsonObject& root = jsonOutBuffer.createObject();
  root["channel"] = Channel;
  root["resource"] = resource;
  if (persist) root["write"] = true;
  root["data"] = data;

  char buffer[128];
  root.printTo(buffer, sizeof(buffer));

  char topic[64];
  sprintf(topic, "%s/%s", Channel, resource);

  client.publish(topic, buffer);
}

const char* generateID() {
  randomSeed(micros());  // mejor que analogRead(0)
  for (int i = 0; i < sizeof(id) - 1; i++) {
    id[i] = chars[random(sizeof(chars) - 1)];
  }
  id[sizeof(id) - 1] = '\0';
  return id;
}
