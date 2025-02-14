#include <WiFi.h>
#include <EMailSender.h>

// Wi-Fi configuration
WiFiClient optaClient;  // create a Wi-Fi client object
unsigned long last_wifi_check = 0;
const int WIFI_TIMEOUT = 10000;            // Maximum connection attempt time (10s)
const int WIFI_RECONNECT_INTERVAL = 5000;  // Check connection every 5s
EMailSender emailSend(EMAIL_ADDRESS, EMAIL_PASSWORD);
bool wifiConnected = false;
unsigned long lastEmailHour = -1;      // Stores the last hour an email was sent
unsigned long lastErrorEmailDay = -1;  // Track last error email day

// Function to connect to Wi-Fi (non-blocking)
void connectToWiFi() {
  Serial.print("Connecting to Wi-Fi: ");
  Serial.println(WIFI_SSID);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  unsigned long startAttemptTime = millis();

  while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < WIFI_TIMEOUT) {
    // No delay(), let the loop continue running
    if (millis() - startAttemptTime > WIFI_TIMEOUT) {
      Serial.println("\n Error: Unable to connect to Wi-Fi.");
      wifiConnected = false;
      return;
    }
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\n Wi-Fi Connected!");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
    wifiConnected = true;
  }
}

// Function to check Wi-Fi status and reconnect if needed (non-blocking)
void checkWiFiConnection() {
  if (millis() - last_wifi_check >= WIFI_RECONNECT_INTERVAL) {
    last_wifi_check = millis();

    if (WiFi.status() != WL_CONNECTED) {
      Serial.println("Wi-Fi disconnected. Attempting to reconnect...");
      WiFi.disconnect();                     // Ensure a clean reconnection
      WiFi.begin(WIFI_SSID, WIFI_PASSWORD);  // Restart Wi-Fi connection
      unsigned long startAttemptTime = millis();
      // Attempt to reconnect for 10 seconds
      while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 10000) {
        delay(500);  // Wait for connection attempt
        Serial.print(".");
      }
      if (WiFi.status() == WL_CONNECTED) {
        Serial.println("\nWi-Fi reconnected successfully.");
      } else {
        Serial.println("\nFailed to reconnect to Wi-Fi.");
      }
    }
  }
}

// Function to send sensor data email
void sendSensorDataEmail(float oxygen, float ec, float ph) {
  EMailSender::EMailMessage message;

#ifdef ST1
  message.subject = "Sensor Data Report - ST1 - " + getTimestamp();
#elif defined(ST2)
  message.subject = "Sensor Data Report - ST2 - " + getTimestamp();
#else
  message.subject = "Sensor Data Report - Unknown Station - " + getTimestamp();
#endif

  message.message = "Sensor Readings: \n";
  message.message += "Oxygen: " + String(oxygen) + " mg/L \n";
  message.message += "EC: " + String(ec) + " mS/cm \n";
  message.message += "pH: " + String(ph) + "\n";
  message.message += "\nRegards";

  EMailSender::Response resp = emailSend.send(EMAIL_ADDRESS, message);
  Serial.println(resp.status);
}

// Function to send error notification email
void sendErrorEmail(String errorDetails) {
  EMailSender::EMailMessage message;
  message.subject = "Sensor Error Detected";
  message.message = "An issue was detected in the sensor system:\n\n";
  message.message += errorDetails;
  message.message += "\n\nPlease check the system.";

  EMailSender::Response resp = emailSend.send(ADMIN_EMAIL, message);
  Serial.println(resp.status);
}

// Function to check time and send emails at HH:15
void checkAndSendEmail() {
  timeClient.update();
  int currentHour = timeClient.getHours();
  int currentMinute = timeClient.getMinutes();
  int currentSecond = timeClient.getSeconds();
  int currentDay = timeClient.getDay();

#ifdef ST1
  int targetMinute = 45;
  int targetSecond = 0;
#elif defined(ST2)
  int targetMinute = 45;
  int targetSecond = 2;
#else
  int targetMinute = 45;
  int targetSecond = 0;
#endif

  if (currentMinute == targetMinute && currentSecond == targetSecond && lastEmailHour != currentHour) {
    float oxygen = readOxygenSensor();
    float ec = readConductivitySensor();
    float ph = readpHSensor();

    sendSensorDataEmail(oxygen, ec, ph);
    lastEmailHour = currentHour;
    Serial.println("Sensor data email sent.");

    // Check for errors and send an alert if needed
    String errorReport = "";
    if (ec == 0.0) errorReport += "- EC sensor reading is 0. Possible probe issue.\n";
    if (ph == 0.0) errorReport += "- pH sensor reading is 0. Sensor might be disconnected or faulty.\n";

    if (errorReport.length() > 0 && lastErrorEmailDay != currentDay) {
      sendErrorEmail(errorReport);
      lastErrorEmailDay = currentDay;
      Serial.println("Error notification email sent.");
    }
  }
}