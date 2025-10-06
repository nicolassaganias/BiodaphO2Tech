#include <ArduinoJson.h>

#define BBT "mqtt.beebotte.com"
#define TOKEN "token:token_5iHgg9PJs6OoYQM6"  // Dejamos SOLO el token más reciente
#define Channel "OptaAntisa"

// Recursos usados - canales (pueden renombrarse si querés que no se mezclen con los de la otra caja)

// Generador de IDs aleatorios
const char chars[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890";
char id[17];

int cnt = 0;            // Contador para hacer una escritura por cada x publicaciones
int pub = 3;            // Dejamos la última versión (3 en lugar de 5)
bool PubWrite = false;  // True = persistente, false = publicado sin persistencia

#define PERIODIC_UPDATE_TIME 500

// ---------------------------
// Función de lectura analógica
// ---------------------------
float lecturaAnalog(uint8_t pin) {
  float a = analogRead(pin) * 10.0 / 65535.0;
  float b = analogRead(pin);

  // Dejamos los prints comentados para debug opcional
  // Serial.print("analog:");
  // Serial.println(a);
  // Serial.print("analog raw:");
  // Serial.println(b);

  // Lectura directa de entradas analógicas del Opta, mapeadas de 0..65535 a corriente 4..20mA
  // return analogRead(pin) * (20.0 - 4.0) / 65535.0 + 4.0;  // Para señales 4-20mA

  return a; // Devuelve el valor calculado
}

// ---------------------------
// Setup
// ---------------------------
void setup() {
  Serial.begin(115200);
  pinMode(LED_USER, OUTPUT);
  digitalWrite(LED_USER, LOW);

  // Inicializamos la conexión MQTT
  client.setServer(BBT, 1883);
  lastReconnectAttempt = 0;
}

// ---------------------------
// Loop principal
// ---------------------------
void loop() {
  if (!client.connected()) {
    Serial.println("DESCONEXION");
    long now = millis();
    if (now - lastReconnectAttempt > 10000) {  // Dejamos el delay más grande (10s)
      lastReconnectAttempt = now;
      digitalWrite(LED_USER, LOW);
      if (reconnect()) {
        lastReconnectAttempt = 0;
      }
    }
  } else {
    client.loop();
  }
}

// ---------------------------
// Reconexión MQTT
// ---------------------------
bool reconnect() {
  if (client.connect(generateID(), TOKEN, "")) {
    Serial.println("Conectado a Beebotte MQTT");
    digitalWrite(LED_USER, HIGH);
    delay(100);
    return true;
  }
  return false;
}

// ---------------------------
// Función de escalado
// ---------------------------
float escalarValores(float valor, float min_ing, float max_ing, float min, float max) {
  return ((valor - min_ing) * (max - min) / (max_ing - min_ing)) + min;
}

// ---------------------------
// Publicación de datos en MQTT
// ---------------------------
void publish(const char* resource, float data, bool persist) {
  StaticJsonDocument<128> root;
  root["channel"] = Channel;
  root["resource"] = resource;
  root["write"] = persist;
  root["data"] = data;

  char buffer[128];
  serializeJson(root, buffer);

  char topic[64];
  sprintf(topic, "%s/%s", Channel, resource);  // El topic tiene que ser así para Beebotte

  Serial.print("Publicados en ");
  Serial.print(resource);
  Serial.print(" los datos ");
  Serial.print(data, 1);
  Serial.print(" con persistencia ");
  Serial.println(persist ? "True" : "False");
  Serial.println("-------");

  client.publish(topic, buffer);  // Envío el JSON por MQTT
}
