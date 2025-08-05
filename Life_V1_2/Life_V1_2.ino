#include "OptaBlue.h"

#include <WiFi.h>
#include <PubSubClient.h>  // Cliente MQTT, básico pero confiable - PubSubClient@2.8.0
#include <ArduinoJson.h>   // Versión vieja, pero suficiente para este laburo - ArduinoJson@5.13.5

#define BBT "mqtt.beebotte.com"
#define TOKEN "token:token_iYji5r6mC7OuBNgY"  // Ojo que esto lo ideal sería esconderlo en producción
#define Channel "Opta"

// Recursos usados - canales
#define RES_AS_DO1 "AS_DO1"
#define RES_AS_PH1 "AS_PH1"
#define RES_AS_EC1 "AS_EC1"
#define RES_AS_DO2 "AS_DO2"
#define RES_AS_PH2 "AS_PH2"
#define RES_AS_EC2 "AS_EC2"
#define RES_SM7020_1 "SM7020_1"
#define RES_SM7020_2 "SM7020_2"
#define RES_SM_9000 "SM_9000"
#define RES_TSS_MLSS "TSS_MLSS"
#define RES_ALL_READINGS "All_readings"

#define Write true  // esto indica si se guarda el dato en el canal (persistencia)
#define Publ false

int status = WL_IDLE_STATUS;
float lec_AS_DO1, lec_AS_PH1, lec_AS_EC1, lec_AS_DO2, lec_AS_PH2, lec_AS_EC2, lec_SM7020_1, lec_SM7020_2, lec_TSS_MLSS, lec_SM_9000;

const char* ssid = "Outdoor-WiFi-255BC6";  // Cambiar por el nombre de tu red
const char* password = "oT0,2LiM-WlZ";     // Y obviamente la clave del WiFi

// mail a mandar: "anakaren.agustin@udg.edu" y nicosaga@gmail.com y biodapho2@gmail.com

WiFiClient wifiClient;
PubSubClient client(wifiClient);  // Se usa este cliente como transporte para MQTT

long lastReconnectAttempt = 0;
const long interval = 60000;  // cada cuanto guarda datos en el .csv
const long intervalPrint = 5000;

unsigned long lastReadingMillis, lastPrintMillis = 0;

const char chars[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890";  // Para generar ID random medio casero
char id[17];

int cnt = 0;            // Contador para hacer una escritura por cada x publicaciones
int pub = 10;           //Cada cuantas publicaciones se hace una escritura presistente para el dashboard
bool PubWrite = false;  // True = persistente, false = publicado sin persistencia

#define PERIODIC_UPDATE_TIME 500
#define DELAY_AFTER_SETUP 5000

using namespace Opta;


/* -------------------------------------------------------------------------- */
/*                                 SETUP                                      */
/* -------------------------------------------------------------------------- */
void setup() {
  /* -------------------------------------------------------------------------- */
  Serial.begin(115200);
  delay(2000);
  pinMode(LED_D0, OUTPUT);
  pinMode(LED_USER, OUTPUT);
  digitalWrite(LED_D0, LOW);
  digitalWrite(LED_USER, LOW);
  // CANALES ANALOGICOS
  // Configuramos ésto para el canal analógico del Opta principal
  analogReadResolution(16);

  // Esto configura el canal analógico de la expansión
  OptaController.begin();

  for (int i = 0; i < OptaController.getExpansionNum(); i++) {

    for (int k = 0; k < OA_AN_CHANNELS_NUM; k++) {
      /* Todos los canales de la expansión inicializados como ADC 4..20 mA */
      AnalogExpansion::beginChannelAsAdc(OptaController, i,  // Equipo
                                         k,                  // Canal
                                         OA_CURRENT_ADC,     // Tipo de ADC
                                         false,              // pull down (desactivado)
                                         false,              // rejection (desactivado)
                                         false,              // diagnostic (desactivado)
                                         0);                 // averaging (descativado)
    }
  }

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
float lecturaAnalog(uint8_t sensor) {
  /* -------------------------------------------------------------------------- */
  AnalogExpansion exp = OptaController.getExpansion(0);
  if (exp) {
    //Serial.print(" - ch " + String(sensor));
    float value = exp.pinCurrent((uint8_t)sensor);
    //Serial.println(" -> Corriente " + String(value) + " mA");
    //Serial.println();
    return value;
  }
  return 0.0;
}

/* -------------------------------------------------------------------------- */
/*                                  LOOP                                      */
/* -------------------------------------------------------------------------- */
void loop() {
  /* -------------------------------------------------------------------------- */
  OptaController.update();


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

    if (currentMillisPrint - lastPrintMillis >= intervalPrint) {
      lastPrintMillis = currentMillisPrint;
      Serial.println("===== Lecturas de Sensores =====");
      Serial.print("DO Atlas Scientific 1: ");
      Serial.print(lec_AS_DO1, 2);
      Serial.println(" mg/L");
      Serial.print("PH Atlas Scientific 1: ");
      Serial.print(lec_AS_PH1, 2);
      Serial.println("");
      Serial.print("EC Atlas Scientific 1: ");
      Serial.print(lec_AS_EC1, 2);
      Serial.println(" mS/cm");
      Serial.print("DO Atlas Scientific 2: ");
      Serial.print(lec_AS_DO2, 2);
      Serial.println(" mg/L");
      Serial.print("PH Atlas Scientific 2: ");
      Serial.print(lec_AS_PH2, 2);
      Serial.println("");
      Serial.print("EC Atlas Scientific 2: ");
      Serial.print(lec_AS_EC2, 2);
      Serial.println(" mS/cm");
      Serial.print("Caudalimetro SM7020 1: ");
      Serial.print(lec_SM7020_1, 2);
      Serial.println(" L/min");
      Serial.print("Caudalimetro SM7020 2: ");
      Serial.print(lec_SM7020_2, 2);
      Serial.println(" L/min");
      Serial.print("Turbidímetro: ");
      Serial.print(lec_TSS_MLSS, 2);
      Serial.println(" NTU");
      Serial.print("Caudalimetro SM9000: ");
      Serial.print(lec_SM_9000, 2);
      Serial.println(" L/min");
      Serial.println("================================");
    }

    if (currentMillis - lastReadingMillis >= interval) {
      lastReadingMillis = currentMillis;
      if (cnt == pub) {
        PubWrite = true;
      }
      // Iniciamos lectura de sensores
      digitalWrite(LED_D0, HIGH);
      // Señales de corriente (4-20 mA), escaladas con escalarValores()
      lec_AS_DO1 = escalarValores(lecturaAnalog(0), 4.0, 20.0, 0.0, 32.0);
      publish(RES_AS_DO1, lec_AS_DO1, PubWrite);
      delay(30);

      lec_AS_PH1 = escalarValores(lecturaAnalog(1), 4.0, 20.0, 0.0, 14.0);
      publish(RES_AS_PH1, lec_AS_PH1, PubWrite);
      delay(30);

      lec_AS_EC1 = escalarValores(lecturaAnalog(2), 4.0, 20.0, 0.0, 25000.0);
      publish(RES_AS_EC1, lec_AS_EC1, PubWrite);
      delay(30);

      lec_AS_DO2 = escalarValores(lecturaAnalog(3), 4.0, 20.0, 0.0, 32.0);
      publish(RES_AS_DO2, lec_AS_DO2, PubWrite);
      delay(30);

      lec_AS_PH2 = escalarValores(lecturaAnalog(4), 4.0, 20.0, 0.0, 14.0);
      publish(RES_AS_PH2, lec_AS_PH2, PubWrite);
      delay(30);

      lec_AS_EC2 = escalarValores(lecturaAnalog(5), 4.0, 20.0, 0.0, 25000.0);
      publish(RES_AS_EC2, lec_AS_EC2, PubWrite);
      delay(30);

      lec_SM7020_1 = escalarValores(lecturaAnalog(6), 4.0, 20.0, 0.1, 75.0);  // Caudalímetro IFM SM7020 1
      publish(RES_SM7020_1, lec_SM7020_1, PubWrite);
      delay(30);

      float lec_SM7020_2 = escalarValores(lecturaAnalog(7), 4.0, 20.0, 0.1, 75.0);  // Caudalímetro IFM SM7020 2
      publish(RES_SM7020_2, lec_SM7020_2, PubWrite);
      delay(30);

      // Señales de tensión, escaladas a mano con rangos específicos
      lec_TSS_MLSS = analogRead(A5) * ((4000.0 - 0.0) / 65535.0) + 0.0;  // Turbidímetro con módulo adaptador 0..10v
      publish(RES_TSS_MLSS, lec_TSS_MLSS, PubWrite);
      delay(30);

      // El A6 queda desconectado

      lec_SM_9000 = analogRead(A7) * ((300.0 - 5.0) / 65535.0) + 5.0;  // Caudalímetro IFM SM9000
      publish(RES_SM_9000, lec_SM_9000, PubWrite);
      delay(30);


      // Armar string separado por comas
      String all_readings = String(lec_AS_DO1, 1) + "," + String(lec_AS_PH1, 1) + "," + String(lec_AS_EC1, 1) + "," + String(lec_AS_DO2, 1) + "," + String(lec_AS_PH2, 1) + "," + String(lec_AS_EC2, 1) + "," + String(lec_SM7020_1, 1) + "," + String(lec_SM7020_2, 1) + "," + String(lec_SM_9000, 1) + "," + String(lec_TSS_MLSS, 1);

      // Publicar string completo
      publish(RES_ALL_READINGS, all_readings, false);
      //Serial.println(all_readings);

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