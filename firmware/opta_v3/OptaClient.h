#include <WiFi.h>
#include <EMailSender.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

// NTP Configuration
const char* ntpServer = "pool.ntp.org";  // NTP server address used to synchronize time
const int ntpPort = 123;                 // Standard NTP port
WiFiUDP ntpUDP;                          // UDP instance for NTP communication
bool ntpSynced = false;                  // Flag to check if NTP has been synchronized
bool initialEmailSent = false;           // Flag to indicate if the first email has been sent

unsigned long currentEpoch = 0;  // Stores the last synchronized epoch time
#define TIMEZONE_OFFSET 7200     // Timezone offset in seconds (e.g., 7200 for UTC+2)

// Wi-Fi Configuration
const int WIFI_TIMEOUT = 600000;           // Maximum time to attempt Wi-Fi connection (in milliseconds)
const int WIFI_RECONNECT_INTERVAL = 5000;  // Interval to check Wi-Fi connection status (in milliseconds)
bool wifiConnected = false;                // Flag to indicate if Wi-Fi is connected
unsigned long last_wifi_check = 0;         // Stores the last time Wi-Fi was checked

// Email Configuration
EMailSender emailSend(EMAIL_ADDRESS, EMAIL_PASSWORD);  // Email sender instance

String getTimestamp() {
  if (currentEpoch == 0) return "00:00:00";

  // Calcular epoch actual estimado
  unsigned long epoch = currentEpoch + (millis() - lastNTPUpdate) / 1000;

  unsigned long hours = (epoch % 86400L) / 3600;
  unsigned long minutes = (epoch % 3600) / 60;
  unsigned long seconds = epoch % 60;

  char timestamp[10];
  sprintf(timestamp, "%02lu:%02lu:%02lu", hours, minutes, seconds);
  return String(timestamp);
}

// Function to make an LED blink quickly
// This provides visual feedback about the system status
void blinkLED(int pin, int interval) {
  static unsigned long previousMillis = 0;  // Store the last time the LED was toggled
  static bool ledState = LOW;               // Current state of the LED (ON/OFF)
  unsigned long currentMillis = millis();   // Current time

  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;  // Update the last toggled time
    ledState = !ledState;            // Toggle the LED state
    digitalWrite(pin, ledState);     // Apply the new state to the LED
  }
}

// Synchronize time using NTP over UDP
// Returns true if synchronization was successful
bool syncNTP_UDP() {
  if (WiFi.status() != WL_CONNECTED) return false;  // Ensure Wi-Fi is connected

  IPAddress ntpServerIP;
  if (!WiFi.hostByName(ntpServer, ntpServerIP)) return false;  // Resolve NTP server address

  byte packetBuffer[48] = { 0 };  // Buffer to hold incoming and outgoing packets
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode - this is a standard NTP request header

  ntpUDP.beginPacket(ntpServerIP, 123);  // Send NTP request to server
  ntpUDP.write(packetBuffer, 48);        // Send packet to server
  ntpUDP.endPacket();

  unsigned long startTime = millis();  // Record the time of request
  int packetSize = 0;

  // Wait for a response from the NTP server (Timeout: 1500 ms)
  while ((packetSize = ntpUDP.parsePacket()) == 0 && millis() - startTime < 1500) {
    delay(10);
  }

  if (!packetSize) {  // If no response was received within the timeout
    ntpUDP.stop();
    ntpSynced = false;
    return false;
  }

  ntpUDP.read(packetBuffer, 48);  // Read the packet into the buffer
  ntpUDP.stop();

  // Extract the epoch timestamp from the packet buffer
  unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
  unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
  unsigned long epoch = (highWord << 16 | lowWord) - 2208988800UL;  // Convert to Unix time
  epoch += TIMEZONE_OFFSET;                                         // Adjust for the desired timezone

  if (epoch > 100000) {  // Validate the epoch timestamp
    currentEpoch = epoch;
    ntpSynced = true;
    lastNTPUpdate = millis();
    Serial.println("✅ NTP successfully synchronized.");
    return true;
  } else {  // If the timestamp is invalid, mark synchronization as failed
    ntpSynced = false;
    Serial.println("❌ NTP synchronization failed.");
    return false;
  }
}

// Connect to Wi-Fi network using credentials
void connectToWiFi() {
  Serial.println("Attempting to connect to WiFi...");
  Serial.print("Connecting to Wi-Fi: ");
  Serial.println(WIFI_SSID);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);  // Start Wi-Fi connection

  unsigned long startAttemptTime = millis();
  const unsigned long timeout = 15000;  // Timeout for connection attempt (15 seconds)

  // Wait for the connection to be established or until timeout
  while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < timeout) {
    delay(500);
    Serial.print(".");
  }

  if (WiFi.status() == WL_CONNECTED) {  // Check if Wi-Fi connection was successful
    Serial.println("\n✅ Wi-Fi Connected!");
    Serial.print("📡 IP Address: ");
    Serial.println(WiFi.localIP());
    digitalWrite(LED2, HIGH);  // Turn on LED to indicate connection success
    wifiConnected = true;      // Set Wi-Fi connection flag to true
  } else {
    Serial.println("\n❌ Error: Unable to connect to Wi-Fi.");
    digitalWrite(LED2, LOW);  // Turn off LED to indicate connection failure
    wifiConnected = false;    // Set Wi-Fi connection flag to false
  }
}

// Check Wi-Fi Connection and Reconnect if Needed
void checkWiFiConnection() {
  if (millis() - last_wifi_check >= WIFI_RECONNECT_INTERVAL) {  // Check connection status at intervals
    last_wifi_check = millis();
    if (WiFi.status() != WL_CONNECTED) {
      Serial.println("Wi-Fi disconnected. Attempting to reconnect...");
      wifiConnected = false;
      blinkLED(LED2, 1000);  // Blink LED to indicate disconnection status
      WiFi.disconnect();
      delay(1000);
      WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

      unsigned long startAttemptTime = millis();
      int attempts = 0;

      // Try reconnecting for a limited number of attempts
      while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < WIFI_TIMEOUT && attempts < 10) {
        delay(30000);
        Serial.print(".");
        attempts++;
      }
      if (WiFi.status() == WL_CONNECTED) {  // Reconnection successful
        Serial.println("\nWi-Fi reconnected successfully.");
        digitalWrite(LED2, HIGH);
        syncNTP_UDP();  // Attempt to resynchronize NTP after reconnecting
      } else {          // Reconnection failed
        Serial.println("\nFailed to reconnect to Wi-Fi.");
        blinkLED(LED2, 1000);
      }
    }
  }
}

struct SensorData {
  float do1;
  float do2;
  float ec1;
  float ec2;
  float ph1;
  float ph2;
};

void collectNewData(const SensorData& data) {
  snprintf(allData[nextDataIndex], 256,
           "%s &emsp; DO1: %.2f mg/L &emsp; DO2: %.2f mg/L &emsp; EC1: %.2f uS/cm &emsp; EC2: %.2f uS/cm &emsp; pH1: %.2f &emsp; pH2: %.2f<br>",
           getTimestamp().c_str(), data.do1, data.do2, data.ec1, data.ec2, data.ph1, data.ph2);

  lastDataIndex = nextDataIndex;
  nextDataIndex++;

  if (nextDataIndex >= numberOfData) {
    nextDataIndex = 0;
  }
}
void sendFirstSensorDataEmail(const SensorData& data) {
  EMailSender::EMailMessage message;

#ifdef ST1
  message.subject = "Sensor Data Report - ST1 - " + getTimestamp();
#elif defined(ST2)
  message.subject = "Sensor Data Report - ST2 - " + getTimestamp();
#elif defined(TEST)
  message.subject = "Sensor Data Report - TEST - " + getTimestamp();
#elif defined(GR)
  message.subject = "Sensor Data Report - GREECE - " + getTimestamp();
#else
  message.subject = "Sensor Data Report - Unknown Station - " + getTimestamp();
#endif

  message.message = "Sensor Readings:<br>";
  message.message += "DO 1: " + String(data.do1, 2) + " mg/L<br>";
  message.message += "DO 2: " + String(data.do2, 2) + " mg/L<br>";
  message.message += "EC 1: " + String(data.ec1, 2) + " uS/cm<br>";
  message.message += "EC 2: " + String(data.ec2, 2) + " uS/cm<br>";
  message.message += "pH 1: " + String(data.ph1, 2) + "<br>";
  message.message += "pH 2: " + String(data.ph2, 2) + "<br>";
  message.message += "<br>Regards";

  EMailSender::Response resp = emailSend.send(RECEIVER_EMAIL, message);
}

// Send Sensor Data Email
void sendSensorDataEmail() {
  EMailSender::EMailMessage message;

#ifdef ST1
  message.subject = "Sensor Data Report - ST1 - " + getTimestamp();
#elif defined(ST2)
  message.subject = "Sensor Data Report - ST2 - " + getTimestamp();
#elif defined(TEST)
  message.subject = "Sensor Data Report - TEST - " + getTimestamp();
#elif defined(GR)
  message.subject = "Sensor Data Report - GREECE - " + getTimestamp();
#else
  message.subject = "Sensor Data Report - Unknown Station - " + getTimestamp();
#endif

  message.message = "Sensor Readings:<br>";

  for (int i = nextDataIndex; i < numberOfData; i++) {
    message.message += String(allData[i]) + "<br>";
  }

  message.message += "<br>Regards";

  EMailSender::Response resp = emailSend.send(RECEIVER_EMAIL, message);
}


void checkFailure() {
  static unsigned long lastCheck = 0;
  static int wifiFailCount = 0;
  const unsigned long CHECK_INTERVAL = 10000;  // Cada 10 segundos
  const unsigned long EMAIL_TIMEOUT = EMAIL_INTERVAL * 3;

  if (millis() - lastCheck < CHECK_INTERVAL) return;
  lastCheck = millis();

  // Si no se ha enviado mail en mucho tiempo, algo puede andar mal
  if (millis() - lastEmailSent > EMAIL_TIMEOUT) {
    Serial.println("❌ Email not sent for too long. Restarting...");
    delay(2000);
    NVIC_SystemReset();
  }

  // Si el WiFi sigue caído, contamos intentos fallidos
  if (WiFi.status() != WL_CONNECTED) {
    wifiFailCount++;
    Serial.print("⚠️ WiFi fail count: ");
    Serial.println(wifiFailCount);
    if (wifiFailCount >= 10) {
      Serial.println("❌ WiFi failed to reconnect after 10 attempts. Restarting...");
      delay(5000);
      NVIC_SystemReset();
    }
  } else {
    wifiFailCount = 0;
  }
}
