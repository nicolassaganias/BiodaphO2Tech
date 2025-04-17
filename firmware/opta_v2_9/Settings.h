//Variables. Here change parameters.
// Configuration for time intervals (in milliseconds)
const unsigned long EMAIL_INTERVAL = 4 /*hours*/ * 60 /*minutes*/ * 60 /*seconds*/ * 1000 /*milliseconds*/;  // Interval for sending emails - Set to 12 hours
const unsigned long DATA_INTERVAL = 60 /*minutes*/ * 60 /*seconds*/ * 1000 /*milliseconds*/;                  // Interval for collecting sensor data - Set to 1 hour
const unsigned long ntpUpdateInterval = 6 * 60 * 60 * 1000;                                                   // NTP time synchronization interval - Set to 6 hours
const unsigned long PRINT_DELAY = 10000;                                                                      // Delay for printing data to the serial monitor - Set to 10 seconds

// Station Definition - Uncomment one station only
// #define ST1
// #define ST2
// #define GR
#define TEST
const char* VERSION = "Ver. 2.9";
// Fixed code. Don't touch
#define numberOfData ((EMAIL_INTERVAL / DATA_INTERVAL) + 1)  // Define the total number of data entries based on the ratio of EMAIL_INTERVAL to DATA_INTERVAL. This array will hold all collected data until an email is sent

char allData[numberOfData][128];  // Array to store all collected data as strings (max 128 characters per entry)
int nextDataIndex = 0;            // Index for storing the next data entry
int lastDataIndex = 0;            // Index for tracking the last stored data entry
long lastDataUpdate = 0;          // Tracks the last time data was updated

// Email Configuration - Replace with your email credentials and receiver details
#define EMAIL_ADDRESS "nicosaga@gmail.com"            // Sender email address
#define ADMIN_EMAIL "nicolassaganias@protonmail.com"  // Administrator email address for notifications

// Station-specific configuration
#ifdef ST1

#define WIFI_SSID "Outdoor-WiFi-255BC6"
#define WIFI_PASSWORD "oT0,2LiM-WlZ"
#define RECEIVER_EMAIL "anakaren.agustin@udg.edu"
#define EMAIL_PASSWORD "azaw mwmp nmxb cwpc"

#define SENSOR_DISSOLVE_OXYGEN_PIN A0
#define SENSOR_CONDUCTIVITY_PIN A2
#define SENSOR_PH_PIN A3

const char* STATION_NAME = "ST1";

#elif defined(ST2)

#define WIFI_SSID "Outdoor-WiFi-255BC6"
#define WIFI_PASSWORD "oT0,2LiM-WlZ"
#define RECEIVER_EMAIL "anakaren.agustin@udg.edu"
#define EMAIL_PASSWORD "wdgc ejbd eqqj kaqp"

#define SENSOR_DISSOLVE_OXYGEN_PIN A0
#define SENSOR_CONDUCTIVITY_PIN A2
#define SENSOR_PH_PIN A3

const char* STATION_NAME = "ST2";

#elif defined(GR)

#define WIFI_SSID "Post 2.4"
#define WIFI_PASSWORD "28742241"
#define RECEIVER_EMAIL "biodaph2o.antissa.sensors@gmail.com"
#define EMAIL_PASSWORD "lzct rxap ncra bwto"

#define SENSOR_DISSOLVE_OXYGEN_PIN A3
#define SENSOR_CONDUCTIVITY_PIN A1
#define SENSOR_PH_PIN A2

const char* STATION_NAME = "GR";

#elif defined(TEST)

#define testing
#define WIFI_SSID "Can_Saguer_Wifi-2G-EXT"
#define WIFI_PASSWORD "cansaguer2"
#define RECEIVER_EMAIL "nicolassaganias@protonmail.com"
#define EMAIL_PASSWORD "jvjz aezk ccad cfut" 

#define SENSOR_DISSOLVE_OXYGEN_PIN A0
#define SENSOR_CONDUCTIVITY_PIN A2
#define SENSOR_PH_PIN A3

const char* STATION_NAME = "TEST";

#else
#error "❌ No valid station defined (GR, ST1, ST2, TEST). Please uncomment the correct one."
#endif

// LED Configuration for system status indication
#define LED1 LED_D0  // Indicates system operation (blinks when running)
#define LED2 LED_D1  // Indicates WiFi connection status (ON when connected)
#define LED3 LED_D2  // Indicates NTP synchronization status (ON when synced)
#define LED4 LED_D3  // Unused LED (available for custom functionality)

// Variables to track time of last events
unsigned long last_print = 0;     // Last time data was printed to the serial monitor
unsigned long lastEmailSent = 0;  // Last time an email was successfully sent
unsigned long lastNTPUpdate = 0;  // Last time NTP synchronization was performed
