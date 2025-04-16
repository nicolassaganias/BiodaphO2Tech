const unsigned long EMAIL_INTERVAL = 12 /*hours*/ * 60 /*minutes*/ * 60 /*seconds*/ * 1000 /*milliseconds*/;  // x hour email sending interval in milliseconds
const unsigned long DATA_INTERVAL = 60 /*minutes*/ * 60 /*seconds*/ * 1000 /*milliseconds*/;                  // x minutes data tracking interval in milliseconds
const unsigned long ntpUpdateInterval = 6 * 60 * 60 * 1000;                                                   // 6hrs en milisegundos
const unsigned long PRINT_DELAY = 10000;                                                                      //in miliseconds

#define numberOfData ((EMAIL_INTERVAL / DATA_INTERVAL) + 1)

char allData[numberOfData][128];
int nextDataIndex = 0;
int lastDataIndex = 0;
long lastDataUpdate = 0;

//Wifi Config
//#define WIFI_SSID "Outdoor-WiFi-255BC6"
//#define WIFI_PASSWORD "oT0,2LiM-WlZ"  // oT0,2LiM-WlZ OutdoorRouter Admin Pass. la "l"es una L
//#define WIFI_SSID "Can_Saguer_Wifi"
//#define WIFI_PASSWORD "cansaguer2"  // oT0,2LiM-WlZ OutdoorRouter Admin Pass. la "l"es una L
#define WIFI_SSID "Post 2.4"
#define WIFI_PASSWORD "28742241"  // oT0,2LiM-WlZ OutdoorRouter Admin Pass. la "l"es una L

//e-mails Config
#define EMAIL_ADDRESS "nicosaga@gmail.com"
#define EMAIL_PASSWORD "asqa ylrd ksro myyb"  // otra app pass: fdyj irkk avxn pko
#define RECEIVER_EMAIL "anakaren.agustin@udg.edu"
//#define RECEIVER_EMAIL "nicolas.saganias@udg.edu"
#define ADMIN_EMAIL "nicolassaganias@protonmail.com"

// definitions
#define ST1  // OR ST2 for STATION 1 & STATION 2 OR GR for GREECE & TEST for TESTING
//#define testing

/* Sensor pins QUART */
//#define SENSOR_DISSOLVE_OXYGEN_PIN A0  // Dissolve oxygen sensor kit
//#define SENSOR_CONDUCTIVITY_PIN A2     // Conductivity transmitter sensor kit
//#define SENSOR_PH_PIN A3               // pH sensor kit
/* Sensor pins GREECE */
#define SENSOR_DISSOLVE_OXYGEN_PIN A3  // Dissolve oxygen sensor kit
#define SENSOR_CONDUCTIVITY_PIN A1     // Conductivity transmitter sensor kit
#define SENSOR_PH_PIN A2               // pH sensor kit

// Definición de LEDs
#define LED1 LED_D0  // Indica que el sistema está funcionando
#define LED2 LED_D1  // Indica conexión WiFi
#define LED3 LED_D2  // Indica sincronización NTP
#define LED4 LED_D3  // Vacío

unsigned long last_print, lastEmailSent = 0;
unsigned long lastNTPUpdate = 0;
