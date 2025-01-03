/* Server credentials */
//#define CLIENT_ID "OPTA TEST"

#define WIFI_SSID "Can_Saguer_Wifi"
#define WIFI_PASSWORD "cansaguer2"
#define EMAIL_ADDRESS "nicosaga@gmail.com"
#define EMAIL_PASSWORD "asqa ylrd ksro myyb"  // otra app pass: fdyj irkk avxn pkoe
#define RECEIVER_EMAIL "nicolas.saganias@udg.edu"
#define EMAIL_DELAY 5 * 60 * 1000 //5 minutes
#define PRINT_DELAY 5 * 1000 //5 seconds
/* Sensor pins */
#define SENSOR_DISSOLVE_OXYGEN_PIN A0  // Dissolve oxygen sensor kit
#define SENSOR_CONDUCTIVITY_PIN A1     // Conductivity transmitter sensor kit
#define SENSOR_PH_PIN A2               // pH sensor kit

// data format
#define DATA_TYPE_OF_SENSOR_VALUE "doubleValue"  // It is the data type of sensor's value
#define DELAY_BETWEEN_DATA_SENDING 5000          // Delay between two consecutive data transmissions.

// device id
#define DISSOLVE_OXYGEN_SENSOR "OXT01"  // must need an unique id for oxygen sensor
#define CONDUCTIVITY_SENSOR "ECT01"     // must need an unique id for conductivity sensor
#define PH_SENSOR "PHT01"               // must need an unique id for pH sensor

unsigned long last_update = 0;