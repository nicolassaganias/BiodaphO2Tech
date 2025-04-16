#include <Arduino.h>

// Pines de sensores (ajustá según conexiones reales)
#define SENSOR_PH_PIN A0
#define SENSOR_PH2_PIN A1
#define SENSOR_CONDUCTIVITY_PIN A2
#define SENSOR_CONDUCTIVITY2_PIN A3
#define SENSOR_DISSOLVE_OXYGEN_PIN A4
#define SENSOR_DISSOLVE_OXYGEN2_PIN A5

// Constantes compartidas
#define PH_SENS_RESISTOR_VALUE 240.0
#define PH_SENS_LOWER_VALUE 0.0
#define PH_SENS_HIGHER_VALUE 14.0

#define EC_SENS_RESISTOR_VALUE 240.0
#define EC_SENS_LOWER_VALUE 0.0
#define EC_MAX_SENSOR_VALUE 25000  // K=0.1

#define OXYGEN_SENS_RESISTOR_VALUE 240.0
#define OXYGEN_SENS_LOWER_VALUE 0.0
#define OXYGEN_SENS_HIGHER_VALUE 32.0

// Variables globales para valores crudos
float rawValuePh, rawValuePh2, voltagePh, voltagePh2, currentPh, currentPh2;
float rawValueEc, rawValueEc2, voltageEc, voltageEc2, currentEc, currentEc2;
float rawValueOxygen, rawValueOxygen2, voltageOxygen, voltageOxygen2, currentOxygen, currentOxygen2;

float mapFloat(float x, float in_min, float in_max, float out_min, float out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

// Funciones de lectura duplicadas para cada sensor

float readpHSensor(uint8_t pin, float &raw, float &voltage, float &current) {
  voltage = 0.0;
  for (int i = 0; i < 10; i++) {
    raw = analogRead(pin);
    voltage += (0.0027 * raw - 0.0083);
  }
  voltage /= 10.0;
  if (voltage < 0) voltage = 0;
  current = (voltage / PH_SENS_RESISTOR_VALUE) * 1000.0;
  float val = mapFloat(current, 4.00, 20.00, PH_SENS_LOWER_VALUE, PH_SENS_HIGHER_VALUE);
  return constrain(val, PH_SENS_LOWER_VALUE, PH_SENS_HIGHER_VALUE);
}

float readConductivitySensor(uint8_t pin, float &raw, float &voltage, float &current) {
  voltage = 0.0;
  for (int i = 0; i < 10; i++) {
    raw = analogRead(pin);
    voltage += (0.002700 * raw - 0.0084);
  }
  voltage /= 10.0;
  if (voltage < 0) voltage = 0;
  current = (voltage / EC_SENS_RESISTOR_VALUE) * 1000.0;
  float val = mapFloat(current, 4.00, 20.00, EC_SENS_LOWER_VALUE, EC_MAX_SENSOR_VALUE);
  return constrain(val, 1.0, EC_MAX_SENSOR_VALUE);
}

float readOxygenSensor(uint8_t pin, float &raw, float &voltage, float &current) {
  voltage = 0.0;
  for (int i = 0; i < 10; i++) {
    raw = analogRead(pin);
    voltage += (0.002710 * raw - 0.0084);
  }
  voltage /= 10.0;
  if (voltage < 0) voltage = 0;
  current = (voltage / OXYGEN_SENS_RESISTOR_VALUE) * 1000.0;
  float val = mapFloat(current, 4.00, 20.00, OXYGEN_SENS_LOWER_VALUE, OXYGEN_SENS_HIGHER_VALUE);
  return constrain(val, OXYGEN_SENS_LOWER_VALUE, OXYGEN_SENS_HIGHER_VALUE);
}

void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println("Sistema de sensores iniciado.");
}

 void loop() {
  float ph1 = readpHSensor(SENSOR_PH_PIN, rawValuePh, voltagePh, currentPh);
  float ph2 = readpHSensor(SENSOR_PH2_PIN, rawValuePh2, voltagePh2, currentPh2);

  float ec1 = readConductivitySensor(SENSOR_CONDUCTIVITY_PIN, rawValueEc, voltageEc, currentEc);
  float ec2 = readConductivitySensor(SENSOR_CONDUCTIVITY2_PIN, rawValueEc2, voltageEc2, currentEc2);

  float do1 = readOxygenSensor(SENSOR_DISSOLVE_OXYGEN_PIN, rawValueOxygen, voltageOxygen, currentOxygen);
  float do2 = readOxygenSensor(SENSOR_DISSOLVE_OXYGEN2_PIN, rawValueOxygen2, voltageOxygen2, currentOxygen2);

  Serial.println("---------- MEDICIONES ----------");

  Serial.print("[pH1] ");
  Serial.print(ph1, 2);
  Serial.print(" | Raw: ");
  Serial.print(rawValuePh, 0);
  Serial.print(" | V: ");
  Serial.print(voltagePh, 3);
  Serial.print(" | I: ");
  Serial.print(currentPh, 2);
  Serial.println(" mA");

  Serial.print("[pH2] ");
  Serial.print(ph2, 2);
  Serial.print(" | Raw: ");
  Serial.print(rawValuePh2, 0);
  Serial.print(" | V: ");
  Serial.print(voltagePh2, 3);
  Serial.print(" | I: ");
  Serial.print(currentPh2, 2);
  Serial.println(" mA");

  Serial.print("[EC1] ");
  Serial.print(ec1, 2);
  Serial.print(" µS/cm | Raw: ");
  Serial.print(rawValueEc, 0);
  Serial.print(" | V: ");
  Serial.print(voltageEc, 3);
  Serial.print(" | I: ");
  Serial.print(currentEc, 2);
  Serial.println(" mA");

  Serial.print("[EC2] ");
  Serial.print(ec2, 2);
  Serial.print(" µS/cm | Raw: ");
  Serial.print(rawValueEc2, 0);
  Serial.print(" | V: ");
  Serial.print(voltageEc2, 3);
  Serial.print(" | I: ");
  Serial.print(currentEc2, 2);
  Serial.println(" mA");

  Serial.print("[DO1] ");
  Serial.print(do1, 2);
  Serial.print(" mg/L | Raw: ");
  Serial.print(rawValueOxygen, 0);
  Serial.print(" | V: ");
  Serial.print(voltageOxygen, 3);
  Serial.print(" | I: ");
  Serial.print(currentOxygen, 2);
  Serial.println(" mA");

  Serial.print("[DO2] ");
  Serial.print(do2, 2);
  Serial.print(" mg/L | Raw: ");
  Serial.print(rawValueOxygen2, 0);
  Serial.print(" | V: ");
  Serial.print(voltageOxygen2, 3);
  Serial.print(" | I: ");
  Serial.print(currentOxygen2, 2);
  Serial.println(" mA");

  Serial.println("--------------------------------");

  delay(2000);
}
