const unsigned long EMAIL_INTERVAL = 12 /*hours*/ * 60 /*minutes*/ * 60 /*seconds*/ * 1000 /*milliseconds*/;// 12 hour email sending interval in milliseconds
const unsigned long DATA_INTERVAL = 60 /*minutes*/ * 60 /*seconds*/ * 1000 /*milliseconds*/;                // 30 minutes data tracking interval in milliseconds

const unsigned long numberOfData = (EMAIL_INTERVAL / DATA_INTERVAL) + 1;
String allData[numberOfData];
int lastDataIndex = 0;
int nextDataIndex = lastDataIndex + 1;
long lastDataUpdate = 0;

//Wifi Config
// #define WIFI_SSID "Outdoor-WiFi-255BC6"
// #define WIFI_PASSWORD "oT0,2LiM-WlZ"  // oT0,2LiM-WlZ OutdoorRouter Admin Pass. la "l"es una L
#define WIFI_SSID "Can_Saguer_Wifi"
#define WIFI_PASSWORD "cansaguer2"  // oT0,2LiM-WlZ OutdoorRouter Admin Pass. la "l"es una L

//e-mails Config
#define EMAIL_ADDRESS "nicosaga@gmail.com"
#define EMAIL_PASSWORD "asqa ylrd ksro myyb"  // otra app pass: fdyj irkk avxn pko
//#define RECEIVER_EMAIL "anakaren.agustin@udg.edu"
#define RECEIVER_EMAIL "nicolas.saganias@udg.edu"
#define ADMIN_EMAIL "nicolassaganias@protonmail.com"

// definitions
#define GR  // OR ST2 for STATION 1 & STATION 2 OR GR for GREECE
#define testing

/* Sensor pins */
#define SENSOR_DISSOLVE_OXYGEN_PIN A0  // Dissolve oxygen sensor kit
#define SENSOR_CONDUCTIVITY_PIN A2     // Conductivity transmitter sensor kit
#define SENSOR_PH_PIN A3               // pH sensor kit

// Definición de LEDs
#define LED1 LED_D0  // Indica que el sistema está funcionando
#define LED2 LED_D1  // Indica conexión WiFi
#define LED3 LED_D2  // Indica sincronización NTP
#define LED4 LED_D3  // Vacío

const unsigned long ntpUpdateInterval = 6 * 60 * 60 * 1000;  // 6hrs en milisegundos
const unsigned long PRINT_DELAY = 10000;                      //5 seconds
unsigned long last_print, lastStatusEmailSent, lastEmailSent = 0;
unsigned long lastNTPUpdate = 0;
