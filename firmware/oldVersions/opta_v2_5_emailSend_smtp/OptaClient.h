#include <WiFi.h>
#include <EMailSender.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

// NTP Configuration
const char* ntpServer = "pool.ntp.org";
const int ntpPort = 123;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, ntpServer, 3600);  // 3600s = 1 hour timezone offset

// Wi-Fi Configuration
const int WIFI_TIMEOUT = 10000;
const int WIFI_RECONNECT_INTERVAL = 5000;
bool wifiConnected = false;
unsigned long last_wifi_check = 0;

// Email Configuration
EMailSender emailSend(EMAIL_ADDRESS, EMAIL_PASSWORD);
unsigned long lastErrorEmailDay = 99;

// Initialize and Sync NTP
bool initializeNTP() {
  Serial.println("Initializing NTP...");
  timeClient.begin();
  int retries = 0;
  while (!timeClient.update() && retries < 10) {
    timeClient.forceUpdate();
    delay(1000);
    retries++;
  }
  if (timeClient.update()) {
    Serial.println("NTP synchronization successful.");
    Serial.print("Current Time: ");
    Serial.println(timeClient.getFormattedTime());
    return true;
  }
  Serial.println("NTP synchronization failed.");
  return false;
}

void syncNTP() {
    Serial.println("Actualizando NTP...");
    if (timeClient.update()) {
        Serial.print("Hora actualizada: ");
        Serial.println(timeClient.getFormattedTime());
    } else {
        Serial.println("Error: No se pudo actualizar NTP.");
    }
}

// Connect to Wi-Fi
void connectToWiFi() {
  Serial.print("Connecting to Wi-Fi: ");
  Serial.println(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  unsigned long startAttemptTime = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < WIFI_TIMEOUT) {}
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWi-Fi Connected!");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
    wifiConnected = true;
  } else {
    Serial.println("\nError: Unable to connect to Wi-Fi.");
    wifiConnected = false;
  }
}

// Check Wi-Fi Connection and Reconnect if Needed
void checkWiFiConnection() {
  if (millis() - last_wifi_check >= WIFI_RECONNECT_INTERVAL) {
    last_wifi_check = millis();
    if (WiFi.status() != WL_CONNECTED) {
      Serial.println("Wi-Fi disconnected. Attempting to reconnect...");
      WiFi.disconnect();
      WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
      unsigned long startAttemptTime = millis();
      while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < WIFI_TIMEOUT) {
        delay(500);
        Serial.print(".");
      }
      Serial.println(WiFi.status() == WL_CONNECTED ? "\nWi-Fi reconnected successfully." : "\nFailed to reconnect to Wi-Fi.");
    }
  }
}

// Get Timestamp from NTP 
String getTimestamp() {
  return timeClient.getFormattedTime();
}

// Send Sensor Data Email
void sendSensorDataEmail(float oxygen, float ec, float ph) {
  EMailSender::EMailMessage message;
#ifdef ST1
  message.subject = "Sensor Data Report - ST1 - " + getTimestamp();
#elif defined(ST2)
  message.subject = "Sensor Data Report - ST2 - " + getTimestamp();
#else
  message.subject = "Sensor Data Report - Unknown Station - " + getTimestamp();
#endif
  message.message = "Sensor Readings:\n";
  message.message += "Oxygen: " + String(oxygen) + " mg/L\n";
  message.message += "EC: " + String(ec) + " mS/cm\n";
  message.message += "pH: " + String(ph) + "\n";
  message.message += "\nRegards";
  EMailSender::Response resp = emailSend.send(RECEIVER_EMAIL, message);
  //Serial.println(resp.status);
}
