// Choose the appropriate EC probe type by uncommenting the desired line
//#define EC_PROBE_01L    // EC Probe K 0.1L (Low conductivity range)
#define EC_PROBE_01  // EC Probe K 0.1 (Medium conductivity range)
// #define EC_PROBE_10    // EC Probe K 1.0 (High conductivity range)

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

#define OXYGEN_SENS_RESISTOR_VALUE 240.0
#define OXYGEN_SENS_LOWER_VALUE 0.0
#define OXYGEN_SENS_HIGHER_VALUE 32.0

// Variables globales para valores crudos
float rawValuePh, rawValuePh2, voltagePh, voltagePh2, currentPh, currentPh2;
float rawValueEc, rawValueEc2, voltageEc, voltageEc2, currentEc, currentEc2;
float rawValueOxygen, rawValueOxygen2, voltageOxygen, voltageOxygen2, currentOxygen, currentOxygen2;

// Do not change the following parameters, as they depend on the selected probe type
// Settings for EC probe 0.1L (Low range)
#ifdef EC_PROBE_01L
#define EC_TDS_VALUE_RANGE 1080   // TDS range for probe K 0.1L
#define EC_MAX_SENSOR_VALUE 2000  // Maximum sensor value for probe K 0.1L
#endif

// Settings for EC probe 0.1 (Medium range)
#ifdef EC_PROBE_01
#define EC_TDS_VALUE_RANGE 13500   // TDS range for probe K 0.1
#define EC_MAX_SENSOR_VALUE 25000  // Maximum sensor value for probe K 0.1
#endif

// Settings for EC probe 1.0 (High range)
#ifdef EC_PROBE_10
#define EC_TDS_VALUE_RANGE 54000    // TDS range for probe K 1.0
#define EC_MAX_SENSOR_VALUE 100000  // Maximum sensor value for probe K 1.0
#endif

// Display diagnostic information for all sensors
void printInfo() {
  Serial.println("-------- SENSOR DIAGNOSTICS --------");

  // Print Oxygen Sensor Readings
  Serial.print("[Oxygen] Raw ADC: ");
  Serial.print(rawValueOxygen);
  Serial.print(" | Voltage: ");
  Serial.print(voltageOxygen);
  Serial.print(" | Current: ");
  Serial.println(currentOxygen);

  // Print EC Sensor Readings
  Serial.print("[EC] Raw ADC: ");
  Serial.print(rawValueEc);
  Serial.print(" | Voltage: ");
  Serial.print(voltageEc);
  Serial.print(" | Current: ");
  Serial.println(currentEc);

  // Print pH Sensor Readings
  Serial.print("[pH] Raw ADC: ");
  Serial.print(rawValuePh);
  Serial.print(" | Voltage: ");
  Serial.print(voltagePh);
  Serial.print(" | Current: ");
  Serial.println(currentPh);

  Serial.println("------------------------------------");
}

// Custom mapping function for floating-point numbers
float mapFloat(float x, float in_min, float in_max, float out_min, float out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

// Generate random decimal numbers for testing purposes
float getRandomDecimal(float minVal, float maxVal) {
  return minVal + (maxVal - minVal) * ((float)random(0, 1000) / 1000.0);
}

// Read Dissolved Oxygen Sensor
float readOxygenSensor() {
  float voltage = 0.0, current = 0.0, oxygen_sensor_value = 0.0;

  for (int i = 0; i < 10; i++) {  // Average 10 readings for accuracy
    float rawValue = analogRead(SENSOR_DISSOLVE_OXYGEN_PIN);
    rawValueOxygen = rawValue;
    voltage += (0.002710 * rawValue - 0.0084);
  }
  voltage /= 10.0;  // Calculate the average voltage

  if (voltage < 0) voltage = 0;  // Prevent negative voltage readings

  current = (voltage / OXYGEN_SENS_RESISTOR_VALUE) * 1000.0;
  oxygen_sensor_value = mapFloat(current, 4.00, 20.00, OXYGEN_SENS_LOWER_VALUE, OXYGEN_SENS_HIGHER_VALUE);
  oxygen_sensor_value = constrain(oxygen_sensor_value, OXYGEN_SENS_LOWER_VALUE, OXYGEN_SENS_HIGHER_VALUE);

#ifdef testing
  oxygen_sensor_value = getRandomDecimal(0.1, 2.0);
#endif

  return oxygen_sensor_value;
}
float readOxygenSensor2() {
  float voltage = 0.0, current = 0.0, oxygen_sensor_value = 0.0;

  for (int i = 0; i < 10; i++) {
    float rawValue = analogRead(SENSOR_DISSOLVE_OXYGEN2_PIN);
    rawValueOxygen2 = rawValue;
    voltage += (0.002710 * rawValue - 0.0084);
  }
  voltage /= 10.0;
  if (voltage < 0) voltage = 0;

  current = (voltage / OXYGEN_SENS_RESISTOR_VALUE) * 1000.0;
  oxygen_sensor_value = mapFloat(current, 4.00, 20.00, OXYGEN_SENS_LOWER_VALUE, OXYGEN_SENS_HIGHER_VALUE);
  oxygen_sensor_value = constrain(oxygen_sensor_value, OXYGEN_SENS_LOWER_VALUE, OXYGEN_SENS_HIGHER_VALUE);

#ifdef testing
  oxygen_sensor_value = getRandomDecimal(0.1, 2.0);
#endif

  return oxygen_sensor_value;
}

// Read EC Sensor (Electrical Conductivity)
float readConductivitySensor() {
  float voltage = 0.0, current = 0.0, ec_sensor_value = 0.0;

  for (int i = 0; i < 10; i++) {  // Average 10 readings for accuracy
    float rawValue = analogRead(SENSOR_CONDUCTIVITY_PIN);
    rawValueEc = rawValue;
    voltage += (0.002700 * rawValue - 0.0084);
  }
  voltage /= 10.0;

  if (voltage < 0) voltage = 0;  // Prevent negative voltage readings

  current = (voltage / EC_SENS_RESISTOR_VALUE) * 1000.0;
  ec_sensor_value = mapFloat(current, 4.00, 20.00, EC_SENS_LOWER_VALUE, EC_MAX_SENSOR_VALUE);
  ec_sensor_value = constrain(ec_sensor_value, 1.0, EC_MAX_SENSOR_VALUE);

#ifdef testing
  ec_sensor_value = random(120000, 140000) / 100;
#endif

  if (ec_sensor_value == 0.0) {
    Serial.println("EC Warning: Sensor disconnected or incorrect values.");
  }
  return ec_sensor_value;
}
float readConductivitySensor2() {
  float voltage = 0.0, current = 0.0, ec_sensor_value = 0.0;

  for (int i = 0; i < 10; i++) {
    float rawValue = analogRead(SENSOR_CONDUCTIVITY2_PIN);
    rawValueEc2 = rawValue;
    voltage += (0.002700 * rawValue - 0.0084);
  }
  voltage /= 10.0;
  if (voltage < 0) voltage = 0;

  current = (voltage / EC_SENS_RESISTOR_VALUE) * 1000.0;
  ec_sensor_value = mapFloat(current, 4.00, 20.00, EC_SENS_LOWER_VALUE, EC_MAX_SENSOR_VALUE);
  ec_sensor_value = constrain(ec_sensor_value, 1.0, EC_MAX_SENSOR_VALUE);

#ifdef testing
  ec_sensor_value = random(120000, 140000) / 100;
#endif

  if (ec_sensor_value == 0.0) {
    Serial.println("EC2 Warning: Sensor disconnected or incorrect values.");
  }

  return ec_sensor_value;
}

// Read pH Sensor
float readpHSensor() {
  float voltage = 0.0, current = 0.0, ph_sensor_value = 0.0;

  for (int i = 0; i < 10; i++) {  // Average 10 readings for accuracy
    float rawValue = analogRead(SENSOR_PH_PIN);
    rawValuePh = rawValue;
    voltage += (0.0027 * rawValue - 0.0083);
  }
  voltage /= 10.0;

  if (voltage < 0) voltage = 0;  // Prevent negative voltage readings

  current = (voltage / PH_SENS_RESISTOR_VALUE) * 1000.0;
  ph_sensor_value = mapFloat(current, 4.00, 20.00, PH_SENS_LOWER_VALUE, PH_SENS_HIGHER_VALUE);
  ph_sensor_value = constrain(ph_sensor_value, PH_SENS_LOWER_VALUE, PH_SENS_HIGHER_VALUE);

#ifdef testing
  ph_sensor_value = getRandomDecimal(7.0, 7.9);
#endif

  if (ph_sensor_value == 0.0) {
    Serial.println("pH Warning: Sensor disconnected or incorrect values.");
  }
  return ph_sensor_value;
}
float readpHSensor2() {
  float voltage = 0.0, current = 0.0, ph_sensor_value = 0.0;

  for (int i = 0; i < 10; i++) {
    float rawValue = analogRead(SENSOR_PH2_PIN);
    rawValuePh2 = rawValue;
    voltage += (0.0027 * rawValue - 0.0083);
  }
  voltage /= 10.0;
  if (voltage < 0) voltage = 0;

  current = (voltage / PH_SENS_RESISTOR_VALUE) * 1000.0;
  ph_sensor_value = mapFloat(current, 4.00, 20.00, PH_SENS_LOWER_VALUE, PH_SENS_HIGHER_VALUE);
  ph_sensor_value = constrain(ph_sensor_value, PH_SENS_LOWER_VALUE, PH_SENS_HIGHER_VALUE);

#ifdef testing
  ph_sensor_value = getRandomDecimal(7.0, 7.9);
#endif

  if (ph_sensor_value == 0.0) {
    Serial.println("pH2 Warning: Sensor disconnected or incorrect values.");
  }

  return ph_sensor_value;
}
