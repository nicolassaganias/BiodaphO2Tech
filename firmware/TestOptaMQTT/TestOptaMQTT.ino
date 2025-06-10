#include <WiFi.h>
#include <PubSubClient.h> // Cliente MQTT, básico pero confiable - PubSubClient@2.8.0
#include <ArduinoJson.h>  // Versión vieja, pero suficiente para este laburo - ArduinoJson@5.13.5

#define BBT "mqtt.beebotte.com"
#define TOKEN "token:token_rRdhbr5pcKLzhFV7" // Ojo que esto lo ideal sería esconderlo en producción
#define Channel "Opta"
#define LSRecurso "Lectura_Sensor1"
#define Write true // esto indica si se guarda el dato en el canal (persistencia)
int status = WL_IDLE_STATUS;  

const char* ssid = "Can_Saguer_Wifi"; // Cambiar por el nombre de tu red
const char* password = "cansaguer2"; // Y obviamente la clave del WiFi

WiFiClient wifiClient;
PubSubClient client(wifiClient); // Se usa este cliente como transporte para MQTT

const int aOutPin = 0;  // Entrada analógica del sensor (CO, MQ-7, etc.)
const int dOutPin = 8;  // Salida digital del sensor, aunque en este sketch no la usamos mucho
const int ledPin = 13;  // LED para debug visual, por si pinta

long lastReconnectAttempt = 0;
const long interval = 10000; // Intervalo entre lecturas (10s)
unsigned long lastReadingMillis = 0;

const char chars[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890"; // Para generar ID random medio casero
char id[17];

void setup() {
  Serial.begin(9600);
  pinMode(dOutPin, INPUT);
  pinMode(ledPin, OUTPUT);

  Serial.print("Conectando a WiFi");
  while (status != WL_CONNECTED) {
    Serial.print("Intentando conectar al SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network:
    status = WiFi.begin(ssid, password);
    // wait 10 seconds for connection:
    delay(10000);
  }
  Serial.println("\nWiFi conectado, IP: ");
  Serial.println(WiFi.localIP());

  client.setServer(BBT, 1883); // Usamos MQTT sin TLS (puerto 1883), Beebotte lo permite
  lastReconnectAttempt = 0;
}

void loop() {
  if (!client.connected()) {
    long now = millis();
    if (now - lastReconnectAttempt > 5000) {
      lastReconnectAttempt = now;
      if (reconnect()) {
        lastReconnectAttempt = 0;
      }
    }
  } else {
    // Si estamos conectados, controlamos si ya pasaron los 10 segundos para leer de nuevo
    unsigned long currentMillis = millis();
    if (currentMillis - lastReadingMillis >= interval) {
      lastReadingMillis = currentMillis;
      readSensorData();
      Serial.println("Dato enviado");
      }
    client.loop(); // Esto es re importante para mantener el ping MQTT activo
  }
}

bool reconnect() {
  // Intento de conexión con ID dinámico y usando solo el token como auth
  if (client.connect(generateID(), TOKEN, "")) {
    Serial.println("Conectado a Beebotte MQTT");
  }
  return client.connected();
}

void readSensorData() {
  int rawValue = analogRead(aOutPin); // Lectura analógica cruda del sensor
  if (!isnan(rawValue)) {
    publish(LSRecurso, rawValue, Write); // Si tiene sentido, lo mando
  }
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
  root.printTo(buffer, sizeof(buffer)); // Lo serializo a un buffer de texto plano

  char topic[64];
  sprintf(topic, "%s/%s", Channel, resource); // El topic tiene que ser así para Beebotte

  client.publish(topic, buffer); // Envío el JSON por MQTT
}

const char* generateID() {
  // Genero un ID al azar cada vez que arranca. No muy seguro, pero funcional para identificar al dispositivo.
  randomSeed(analogRead(0)); // Medio trucho pero sirve para inicializar
  for (int i = 0; i < sizeof(id) - 1; i++) {
    id[i] = chars[random(sizeof(chars))];
  }
  id[sizeof(id) - 1] = '\0';
  return id;
}
