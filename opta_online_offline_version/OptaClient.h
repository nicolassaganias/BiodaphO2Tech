#include <WiFi.h>
#include <PubSubClient.h>

WiFiClient optaClient;            // create a Wi-Fi client object
PubSubClient client(optaClient);  // create a mqtt client object

unsigned long last_mqtt_time = 0;
unsigned long last_wifi_time = 0;
bool wifiConnected = false;
bool serverConnected = false;

// wifi connection handler function
void connectToWiFi() {
  Serial.println("Wifi try to connect");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);  // connect to the wifi network
  while (millis() - last_wifi_time < 10000) {
    if (WiFi.status() == WL_CONNECTED) {
      delay(5000);
      Serial.println("Wi-Fi Connected!");
      wifiConnected = true;
      break;
    }
  }
  last_wifi_time = millis();
}

// handle client communication
void reconnectClient() {
  if (millis() - last_mqtt_time > 1000) {
    if (!client.connected()) {
      // Serial.println("Connecting to the server...");
      if (client.connect(CLIENT_ID, SERVER_USER_NAME, SERVER_USER_PASSWORD)) {
        Serial.println("Server Connected!");
        serverConnected = true;
      }
    }
    last_mqtt_time = millis();
  }
}

// initialize Opta client communication
void optaClientInit() {
  //Serial.println("Connecting to Wi-Fi...");
  connectToWiFi();
  // if (WiFi.status() != WL_CONNECTED) {
  //   Serial.println("Failed.");
  // }
  //Serial.println("Connecting to the server...");
  client.setServer(SERVER_ADDRESS, SERVER_PORT);
  reconnectClient();
  // if (!client.connected()) {
  //   Serial.print("Failed! Status: ");
  //   Serial.print(client.state());
  //   Serial.println();
  // }
}

void runOptaClient() {
  client.loop();
  if (WiFi.status() != WL_CONNECTED) {
    // Serial.println("Wi-Fi connection failed. ");
    WiFi.disconnect();
    connectToWiFi();
  }

  if (!serverConnected && wifiConnected) {
    reconnectClient();
  }
}

// prepare JSON data
String sendData(String dataType, String sensorData, String sensor_ID) {
  String timeStamp = getGlobalTime();
  String sData = makeJson(dataType, sensorData, timeStamp, sensor_ID);
  // Serial.println(sData);
  return sData;
}

// Send data to the firebase server
void sendDataToServer() {
  if (!client.connected()) {
    if (WiFi.status() != WL_CONNECTED) {
      Serial.println("Wi-Fi disconnected.");
      WiFi.disconnect();
      connectToWiFi();
      reconnectClient();
      wifiConnected = false;
    }
    // else {
    //   Serial.println("Server disconnected.");
    //   reconnectClient();
    // }
    Serial.println("Server disconnected.");
    serverConnected = false;
  } else {
    if (millis() - last_update > DELAY_BETWEEN_SENDING) {
      Serial.println("Starting to send data to the server...");
      if (client.publish("oxygen", String(sendData(DATA_TYPE_OF_SENSOR_VALUE, String(readOxygenSensor()), DISSOLVE_OXYGEN_SENSOR)).c_str())) {
        Serial.println("Oxygen data sent successfully!");
      } else {
        Serial.println("Failed to send oxygen data.");
      }
      delay(3000);
      if (client.publish("ec", String(sendData(DATA_TYPE_OF_SENSOR_VALUE, String(readConductivitySensor()), CONDUCTIVITY_SENSOR)).c_str())) {
        Serial.println("EC data sent successfully!");
      } else {
        Serial.println("Failed to send EC data.");
      }
      delay(3000);
      if (client.publish("ph", String(sendData(DATA_TYPE_OF_SENSOR_VALUE, String(readpHSensor()), PH_SENSOR)).c_str())) {
        Serial.println("pH data sent successfully!");
      } else {
        Serial.println("Failed to send pH data.");
      }
      last_update = millis();
    }
  }
}
