#include <WiFi.h>
#include <EMailSender.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

// NTP Configuration
const char* ntpServer = "pool.ntp.org";  //const char* ntpServer = "pool.ntp.org";
const int ntpPort = 123;
WiFiUDP ntpUDP;
unsigned long currentEpoch = 0;  // Store last synced time
#define TIMEZONE_OFFSET 3600     // Adjust for 1-hour difference (in seconds)

// Wi-Fi Configuration
const int WIFI_TIMEOUT = 600000;
const int WIFI_RECONNECT_INTERVAL = 5000;
bool wifiConnected = false;
unsigned long last_wifi_check = 0;
bool ntpConnected = false;
bool systemRunning = true;  // Estado del sistema

// Email Configuration
EMailSender emailSend(EMAIL_ADDRESS, EMAIL_PASSWORD);
unsigned long lastErrorEmailDay = 99;

// Función para hacer parpadear un LED rápidamente
void blinkLED(int pin, int interval) {
  static unsigned long previousMillis = 0;
  static bool ledState = LOW;
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    ledState = !ledState;  // Toggle LED state
    digitalWrite(pin, ledState);
  }
}

void syncNTP_UDP() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("❌ WiFi is not connected. Cannot sync NTP.");
    return;
  }

  Serial.println("🔄 Sending raw NTP request...");

  ntpUDP.begin(ntpPort);
  IPAddress ntpServerIP;

  if (!WiFi.hostByName(ntpServer, ntpServerIP)) {
    Serial.println("❌ Failed to resolve NTP server.");
    blinkLED(LED3, 1000);
    return;
  }

  Serial.print("🌎 Using NTP Server: ");
  Serial.println(ntpServerIP);

  byte packetBuffer[48] = { 0 };
  packetBuffer[0] = 0b11100011;  // LI, Version, Mode
  packetBuffer[1] = 0;           // Stratum
  packetBuffer[2] = 6;           // Polling Interval
  packetBuffer[3] = 0xEC;        // Peer Clock Precision

  ntpUDP.beginPacket(ntpServerIP, 123);  // Send request to port 123
  ntpUDP.write(packetBuffer, 48);
  ntpUDP.endPacket();

  delay(1000);  // Wait for response

  int packetSize = ntpUDP.parsePacket();
  if (packetSize) {
    Serial.println("✅ NTP response received!");
    digitalWrite(LED3, HIGH);
    ntpUDP.read(packetBuffer, 48);
    unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
    unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
    unsigned long epoch = (highWord << 16 | lowWord) - 2208988800UL;  // Convert to Unix time

    epoch += TIMEZONE_OFFSET;  // Apply timezone correction

    currentEpoch = epoch;  // Store corrected time for getTimestamp()

    Serial.print("🕒 Unix Time Synced (with offset): ");
    Serial.println(epoch);

    // Mostrar fecha y hora legible
    time_t rawtime = epoch;
    struct tm * timeinfo = localtime(&rawtime);
    char buffer[30];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeinfo);
    Serial.print("📅 Fecha y hora local: ");
    Serial.println(buffer);
  } else {
    Serial.println("❌ No response from NTP server. UDP may be blocked.");
    digitalWrite(LED3, LOW);
  }
}

// Connect to Wi-Fi
void connectToWiFi() {
  Serial.println("Attempting to connect to WiFi...");
  Serial.print("Connecting to Wi-Fi: ");
  Serial.println(WIFI_SSID);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  unsigned long startAttemptTime = millis();
  const unsigned long timeout = 15000;  // Esperar hasta 15 segundos

  while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < timeout) {
    delay(500);
    Serial.print(".");
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\n✅ Wi-Fi Connected!");
    Serial.print("📡 IP Address: ");
    Serial.println(WiFi.localIP());
    digitalWrite(LED2, HIGH);
    wifiConnected = true;
  } else {
    Serial.println("\n❌ Error: Unable to connect to Wi-Fi.");
    digitalWrite(LED2, LOW);
    wifiConnected = false;
  }
}


// Check Wi-Fi Connection and Reconnect if Needed
void checkWiFiConnection() {
  if (millis() - last_wifi_check >= WIFI_RECONNECT_INTERVAL) {
    last_wifi_check = millis();
    if (WiFi.status() != WL_CONNECTED) {
      Serial.println("Wi-Fi disconnected. Attempting to reconnect...");
      wifiConnected = false;
      blinkLED(LED2, 1000);
      WiFi.disconnect();
      delay(1000);
      WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
      unsigned long startAttemptTime = millis();
      int attempts = 0;
      while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < WIFI_TIMEOUT && attempts < 10) {
        delay(30000);
        Serial.print(".");
        attempts++;
      }
      if (WiFi.status() == WL_CONNECTED) {
        Serial.println("\nWi-Fi reconnected successfully.");
        digitalWrite(LED2, HIGH);
        syncNTP_UDP();  // Intentar actualizar NTP hasta que tenga éxito
      } else {
        Serial.println("\nFailed to reconnect to Wi-Fi.");
        blinkLED(LED2, 1000);
      }
    }
  }
}

String getTimestamp() {
  if (currentEpoch == 0) {  // If time hasn't been set yet, return placeholder
    return "00:00:00";
  }

  unsigned long epoch = currentEpoch + (millis() / 1000);  // Adjust for uptime
  unsigned long hours = (epoch % 86400L) / 3600;           // Get hours
  unsigned long minutes = (epoch % 3600) / 60;             // Get minutes
  unsigned long seconds = epoch % 60;                      // Get seconds

  char timestamp[10];
  sprintf(timestamp, "%02lu:%02lu:%02lu", hours, minutes, seconds);
  return String(timestamp);
}

void checkFailure() {
  const unsigned long EMAIL_TIMEOUT = EMAIL_INTERVAL * 3;
  static int wifiFailCount = 0;

  // Verificar si el email no se envió en mucho tiempo
  if (millis() - lastEmailSent > EMAIL_TIMEOUT) {
    Serial.println("\nEmail not sent for too long. Restarting...");
    digitalWrite(LED1, LOW);
    delay(2000);
    NVIC_SystemReset();
  }

  // Verificar si el WiFi ha fallado demasiadas veces
  if (WiFi.status() != WL_CONNECTED) {
    wifiFailCount++;
    if (wifiFailCount >= 10) {
      Serial.println("\nWiFi failed to reconnect after 10 attempts. Restarting...");
      digitalWrite(LED1, LOW);
      delay(30000);
      NVIC_SystemReset();
    }
  } else {
    wifiFailCount = 0;
  }
}

void collectNewData(float oxygen, float ec, float ph){
  allData[nextDataIndex] = getTimestamp()+ " &emsp; Oxygen: " + String(oxygen) + " mg/L" + " &emsp; EC: " + String(ec) + " mS/cm" + " &emsp; pH: " + String(ph) + "<br>";
  lastDataIndex = nextDataIndex;
  nextDataIndex++;
  if(nextDataIndex >= numberOfData){
     nextDataIndex = 0;
  }
}

// Send Sensor Data Email
void sendSensorDataEmail() {
  EMailSender::EMailMessage message;
#ifdef ST1
  message.subject = "Sensor Data Report - ST1 - " + getTimestamp();
#elif defined(ST2)
  message.subject = "Sensor Data Report - ST2 - " + getTimestamp();
#elif defined(GR)
  message.subject = "Sensor Data Report - GREECE - " + getTimestamp();
#else
  message.subject = "Sensor Data Report - Unknown Station - " + getTimestamp();
#endif
  message.message = "Sensor Readings:<br>";
  for(int i = nextDataIndex ; i < numberOfData; i++){
    message.message += allData[i];
  }
  for(int i = 0 ; i < nextDataIndex; i++){
    message.message += allData[i];
  }
  message.message += "<br>Regards";
  EMailSender::Response resp = emailSend.send(RECEIVER_EMAIL, message);
  //Serial.println(resp.status);
}

void sendFirstSensorDataEmail(float oxygen, float ec, float ph) {
  EMailSender::EMailMessage message;
#ifdef ST1
  message.subject = "Sensor Data Report - ST1 - " + getTimestamp();
#elif defined(ST2)
  message.subject = "Sensor Data Report - ST2 - " + getTimestamp();
#elif defined(GR)
  message.subject = "Sensor Data Report - GREECE - " + getTimestamp();
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
