/* Server credentials */
//#define CLIENT_ID "OPTA TEST"

#define WIFI_SSID "Outdoor-WiFi-255BC6"
#define WIFI_PASSWORD "oT0,2LiM-WlZ"  // oT0,2LiM-WlZ OutdoorRouter Admin Pass. la "l"es una L
#define EMAIL_ADDRESS "nicosaga@gmail.com"
#define EMAIL_PASSWORD "asqa ylrd ksro myyb"  // otra app pass: fdyj irkk avxn pkoe
#define RECEIVER_EMAIL "anakaren.agustin@udg.edu"
#define ADMIN_EMAIL "nicolassaganias@protonmail.com"
#define ST2  // OR ST2 for STATION 1 & STATION 2
/* Sensor pins */
#define SENSOR_DISSOLVE_OXYGEN_PIN A0  // Dissolve oxygen sensor kit
#define SENSOR_CONDUCTIVITY_PIN A1     // Conductivity transmitter sensor kit
#define SENSOR_PH_PIN A2               // pH sensor kit

const unsigned long PRINT_DELAY = 5000;                   //5 seconds
const unsigned long EMAIL_INTERVAL = 3600000;           // 1 hour in milliseconds
const unsigned long SYSTEM_STATUS_INTERVAL = 86400000;  // 1 day in milliseconds

unsigned long last_print, lastStatusEmailSent, lastEmailSent = 0;
