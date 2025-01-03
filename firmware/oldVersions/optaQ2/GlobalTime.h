#include <NTPClient.h>
#include <WiFiUdp.h>
#include <time.h> 

//NTP server credentials
const char* ntpServer = "pool.ntp.org";         // server address
const int ntpPort = 123;                        // server port
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, ntpServer, 0);

// config ntp time
void configNTPServer() {
  // Set the time zone offset for GMT+6 (6 hours ahead of UTC)
  timeClient.setTimeOffset(0);  // 6 hours * 3600 seconds per hour
}

// get global time
String getGlobalTime() {
  timeClient.update();
  time_t rawTime = timeClient.getEpochTime();       // Get the raw epoch time
  struct tm *timeInfo;
  timeInfo = gmtime(&rawTime);                      // Convert the epoch time to a tm struct in UTC
  
  char iso8601Time[30];                             // Create a buffer for the ISO 8601 formatted time
  snprintf(iso8601Time, sizeof(iso8601Time), "%04d-%02d-%02dT%02d:%02d:%02dZ",
           timeInfo->tm_year + 1900, timeInfo->tm_mon + 1, timeInfo->tm_mday,
           timeInfo->tm_hour, timeInfo->tm_min, timeInfo->tm_sec);
  return iso8601Time;
}
