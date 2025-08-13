#include <ArduinoJson.h>

#define BBT "mqtt.beebotte.com"
#define TOKEN "token:token_PQWMUB6fP5fBX0H0"
#define TOKEN "token:token_5iHgg9PJs6OoYQM6"
#define Channel "OptaAntisa"

// Recursos usados - canales (pueden renombrarse si querés que no se mezclen con los de la otra caja)
@@ -36,7 +36,7 @@ const char chars[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ123456
char id[17];

int cnt = 0;            // Contador para hacer una escritura por cada x publicaciones
int pub = 5;            //Cada cuantas publicaciones se hace una escritura presistente para el dashboard
int pub = 3;            //Cada cuantas publicaciones se hace una escritura presistente para el dashboard
bool PubWrite = false;  // True = persistente, false = publicado sin persistencia

#define PERIODIC_UPDATE_TIME 500
@@ -76,10 +76,10 @@ void setup() {
float lecturaAnalog(uint8_t pin) {
  float a = analogRead(pin) * 10.0 / 65535.0;
  float b = analogRead(pin);
  Serial.print("analog:");
  Serial.println(a);
  Serial.print("analog raw:");
  Serial.println(b);
  //Serial.print("analog:");
  //Serial.println(a);
  //Serial.print("analog raw:");
  //Serial.println(b);

  // Lectura directa de entradas analógicas del Opta, mapeadas de 0..65535 a corriente 4..20mA
  //return analogRead(pin) * (20.0 - 4.0) / 65535.0 + 4.0;  // Para señales 4-20ma
@@ -93,8 +93,9 @@ float lecturaAnalog(uint8_t pin) {
void loop() {

  if (!client.connected()) {
    Serial.println("DESCONEXION");
    long now = millis();
    if (now - lastReconnectAttempt > 5000) {
    if (now - lastReconnectAttempt > 10000) {
      lastReconnectAttempt = now;
      digitalWrite(LED_USER, LOW);
      if (reconnect()) {
@@ -155,8 +156,10 @@ bool reconnect() {
  if (client.connect(generateID(), TOKEN, "")) {
    Serial.println("Conectado a Beebotte MQTT");
    digitalWrite(LED_USER, HIGH);
    delay(100);
    return client.connected();
  }
  return client.connected();
  return false;
}

float escalarValores(float valor, float min_ing, float max_ing, float min, float max) {
@@ -178,7 +181,18 @@ void publish(const char* resource, float data, bool persist) {

  char topic[64];
  sprintf(topic, "%s/%s", Channel, resource);  // El topic tiene que ser así para Beebotte
  Serial.println("Publicado en",resource,"los datos",data);
  Serial.println("-------");
  Serial.print("Publicados en ");
  Serial.print(String(resource));
  Serial.print(" los datos ");
  Serial.print(String(data,1));
  Serial.print(" con persistencia ");
  if (persist){
    Serial.println(" True");
  } else{
    Serial.println(" False");
  }
  Serial.println("-------");
  client.publish(topic, buffer);  // Envío el JSON por MQTT
}