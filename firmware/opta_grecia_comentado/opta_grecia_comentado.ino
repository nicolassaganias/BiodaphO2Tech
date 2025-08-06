#include "OptaBlue.h"
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

#define BBT "mqtt.beebotte.com"
#define TOKEN "token:token_PQWMUB6fP5fBX0H0"
#define Channel "OptaAntisa"

// Recursos usados - canales (pueden renombrarse si querés que no se mezclen con los de la otra caja)
#define RES_AS_DO1 "AS_DO1_GR"
#define RES_AS_PH1 "AS_PH1_GR"
#define RES_AS_EC1 "AS_EC1_GR"

#define Write true  // esto indica si se guarda el dato en el canal (persistencia)
#define Publ false

int status = WL_IDLE_STATUS;
float lec_AS_DO1, lec_AS_PH1, lec_AS_EC1;

// const char* ssid = "Outdoor-WiFi-255BC6";  // Cambiar por el nombre de tu red
// const char* password = "oT0,2LiM-WlZ";     // Y obviamente la clave del WiFi
const char* ssid = "Can_Saguer_Wifi";  // Cambiar por el nombre de tu red
const char* password = "cansaguer2";   // Y obviamente la clave del WiFi

WiFiClient wifiClient;
PubSubClient client(wifiClient);  // Se usa este cliente como transporte para MQTT

long lastReconnectAttempt = 0;
const long interval = 10000;  // cada cuanto guarda datos en el .csv
const long intervalPrint = 5000;

unsigned long lastReadingMillis, lastPrintMillis = 0;

const char chars[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890";  // Para generar ID random medio casero
char id[17];

int cnt = 0;            // Contador para hacer una escritura por cada x publicaciones
int pub = 5;            //Cada cuantas publicaciones se hace una escritura presistente para el dashboard
bool PubWrite = false;  // True = persistente, false = publicado sin persistencia

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
  analogReadResolution(16);  // Configuración de resolución de lectura analógica 16 bits (0..65535)

  // WiFi
  Serial.println("Conectando a WiFi");
  while (status != WL_CONNECTED) {
    Serial.println("Intentando conectar al SSID existente: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network:
    status = WiFi.begin(ssid, password);
    // wait 10 seconds for connection:
    delay(10000);
  }
  Serial.println("\nWiFi conectado, IP: ");
  Serial.println(WiFi.localIP());

  client.setServer(BBT, 1883);  // Usamos MQTT sin TLS (puerto 1883), Beebotte lo permite
  lastReconnectAttempt = 0;
}

/* -------------------------------------------------------------------------- */
float lecturaAnalog(uint8_t pin) {
  // Lectura directa de entradas analógicas del Opta, mapeadas de 0..65535 a corriente 4..20mA
  return analogRead(pin) * ((4000.0 - 0.0) / 65535.0) + 0.0;  // Con módulo adaptador 0..10v OJO QUIZAS HAY QUE HACER / 10
  //return analogRead(pin) * (20.0 - 4.0) / 65535.0 + 4.0; // Para señales 4-20ma
}

/* -------------------------------------------------------------------------- */
/*                                  LOOP                                      */
/* -------------------------------------------------------------------------- */
void loop() {

  if (!client.connected()) {
    long now = millis();
    if (now - lastReconnectAttempt > 5000) {
      lastReconnectAttempt = now;
      digitalWrite(LED_USER, LOW);
      if (reconnect()) {
        lastReconnectAttempt = 0;
      }
    }
  } else {
    // Si estamos conectados, controlamos si ya pasaron los 60 segundos para leer de nuevo
    unsigned long currentMillis = millis();
    unsigned long currentMillisPrint = millis();

    if (currentMillis - lastReadingMillis >= interval) {
      lastReadingMillis = currentMillis;
      if (cnt == pub) {
        PubWrite = true;
      }

      // Iniciamos lectura de sensores
      digitalWrite(LED_D0, HIGH);

      // Señales de corriente (4-20 mA), escaladas con escalarValores()
      // lec_AS_DO1 = escalarValores(lecturaAnalog(A0), 4.0, 20.0, 0.0, 32.0);
      // publish(RES_AS_DO1, lec_AS_DO1, PubWrite);
      // delay(30);

      lec_AS_PH1 = escalarValores(lecturaAnalog(A0),0.0, 10.0, 0.0, 14.0);
      if (!isnan(lec_AS_PH1)) {
        publish(RES_AS_PH1, lec_AS_PH1, PubWrite);
        Serial.print("PH Atlas Scientific 1: ");
        Serial.print(lec_AS_PH1, 2);
        Serial.println("");
      }
      delay(30);

      lec_AS_EC1 = escalarValores(lecturaAnalog(A1), 0.0, 10.0, 0.0, 25000.0);
      if (!isnan(lec_AS_EC1)) {
        publish(RES_AS_EC1, lec_AS_EC1, PubWrite);
        Serial.print("EC Atlas Scientific 1: ");
        Serial.print(lec_AS_EC1, 2);
        Serial.println(" mS/cm");
      }
      delay(30);

      cnt++;
      if (cnt > pub) {
        PubWrite = false;
        cnt = 0;
      }

      digitalWrite(LED_D0, LOW);
    }
    client.loop();  // Esto es re importante para mantener el ping MQTT activo
  }
}

bool reconnect() {
  // Intento de conexión con ID dinámico y usando solo el token como auth
  if (client.connect(generateID(), TOKEN, "")) {
    Serial.println("Conectado a Beebotte MQTT");
    digitalWrite(LED_USER, HIGH);
  }
  return client.connected();
}

float escalarValores(float valor, float min_ing, float max_ing, float min, float max) {
  // Lo hacemos así por que map solo funciona con long
  return (valor - min_ing) * (max - min) / (max_ing - min_ing) + min;
}

void publish(const char* resource, float data, bool persist) {
  // Acá armamos el JSON que Beebotte espera. Versión vieja de ArduinoJson = sintaxis rara.
  StaticJsonBuffer<128> jsonOutBuffer;
  JsonObject& root = jsonOutBuffer.createObject();
  root["channel"] = Channel;
  root["resource"] = resource;
  if (persist) root["write"] = true;
  root["data"] = data;

  char buffer[128];
  root.printTo(buffer, sizeof(buffer));  // Lo serializo a un buffer de texto plano

  char topic[64];
  sprintf(topic, "%s/%s", Channel, resource);  // El topic tiene que ser así para Beebotte

  client.publish(topic, buffer);  // Envío el JSON por MQTT
}

// Sobrecarga de función, por que la porquería no toma el String para la combinación de todos los canales
// El compilador toma la que corresponda, con data float o data string
void publish(const char* resource, String data, bool persist) {
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

  client.publish(topic, buffer);  // Publicar por MQTT
}

const char* generateID() {
  randomSeed(micros());  // Mejor que analogRead(0), pero sigue siendo pseudoaleatorio

  for (int i = 0; i < sizeof(id) - 1; i++) {
    id[i] = chars[random(sizeof(chars) - 1)];
  }
  id[sizeof(id) - 1] = '\0';
  return id;
}