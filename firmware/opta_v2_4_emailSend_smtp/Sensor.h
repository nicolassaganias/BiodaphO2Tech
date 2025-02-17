// choose the appropriate EC probe type
//#define EC_PROBE_01L                           // EC Probe K 0.1L
#define EC_PROBE_01  // EC Probe K 0.1
// #define EC_PROBE_10                        // EC Probe K 1.0

// dissolved oxygen sensor parameters
#define OXYGEN_SENS_RESISTOR_VALUE 240.0  // change the resistor value
#define OXYGEN_SENS_LOWER_VALUE 0.0       // oxygen sensor lower value (don't change)
#define OXYGEN_SENS_HIGHER_VALUE 32.0     // oxygen sensor higher value (don't change)

// pH sensor parameters
#define PH_SENS_RESISTOR_VALUE 240.0  // change the resistor value
#define PH_SENS_LOWER_VALUE 0.0       // ph sensor lower value (don't change)
#define PH_SENS_HIGHER_VALUE 14.0     //ph sensor higher value (don't change)

// ec sensor parameters
#define EC_SENS_RESISTOR_VALUE 240.0  // change the resistor value
#define EC_SENS_LOWER_VALUE 0.0       // ec sensor lower value (don't change)

// don't change the following parameters
// settings for ec probe 0.1L
#ifdef EC_PROBE_01L
#define EC_TDS_VALUE_RANGE 1080
#define EC_MAX_SENSOR_VALUE 2000
#endif

// settings for ec probe 0.1
#ifdef EC_PROBE_01
#define EC_TDS_VALUE_RANGE 13500
#define EC_MAX_SENSOR_VALUE 25000
#endif

// settings for ec probe 1.0
#ifdef EC_PROBE_10
#define EC_TDS_VALUE_RANGE 54000
#define EC_MAX_SENSOR_VALUE 100000
#endif

// Map function for floating-point values
float mapFloat(float x, float in_min, float in_max, float out_min, float out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

// Read dissolved oxygen sensor
float readOxygenSensor() {
  float voltage = 0.0, current = 0.0, oxygen_sensor_value = 0.0;
  for (int i = 0; i < 10; i++) {
    float rawValue = analogRead(SENSOR_DISSOLVE_OXYGEN_PIN);  // Read ADC
    voltage += (0.002710 * rawValue - 0.0084);                 // Convert to voltage
  }
  voltage /= 10.0;  // Average readings

  current = (voltage / OXYGEN_SENS_RESISTOR_VALUE) * 1000.0;
  oxygen_sensor_value = mapFloat(current, 4.00, 20.00, OXYGEN_SENS_LOWER_VALUE, OXYGEN_SENS_HIGHER_VALUE);
  oxygen_sensor_value = constrain(oxygen_sensor_value, OXYGEN_SENS_LOWER_VALUE, OXYGEN_SENS_HIGHER_VALUE);

  if (oxygen_sensor_value == 0.0) {
    Serial.println("Oxygen Warning: Sensor disconnected or incorrect values.");
  }
  return oxygen_sensor_value;
}

// Read conductivity (EC) sensor
float readConductivitySensor() {
  float voltage = 0.0, current = 0.0, ec_sensor_value = 0.0;
  for (int i = 0; i < 10; i++) {
    float rawValue = analogRead(SENSOR_CONDUCTIVITY_PIN);  // Read ADC
    voltage += (0.002700 * rawValue - 0.0084);              // Convert to voltage
  }
  voltage /= 10.0;  // Average readings

  current = (voltage / EC_SENS_RESISTOR_VALUE) * 1000.0;
  ec_sensor_value = mapFloat(current, 4.00, 20.00, EC_SENS_LOWER_VALUE, EC_MAX_SENSOR_VALUE);
  ec_sensor_value = constrain(ec_sensor_value, 1.0, EC_MAX_SENSOR_VALUE);

  if (ec_sensor_value > EC_TDS_VALUE_RANGE) {
    Serial.println("EC Warning: Incorrect probe selection or sensor error.");
    return 0;
  }
  return ec_sensor_value;
}

// Read pH sensor
float readpHSensor() {
  float voltage = 0.0, current = 0.0, ph_sensor_value = 0.0;
  for (int i = 0; i < 10; i++) {
    float rawValue = analogRead(SENSOR_PH_PIN);  // Read ADC
    voltage += (0.0027 * rawValue - 0.0083);      // Convert to voltage
  }
  voltage /= 10.0;  // Average readings

  current = (voltage / PH_SENS_RESISTOR_VALUE) * 1000.0;
  ph_sensor_value = mapFloat(current, 4.00, 20.00, PH_SENS_LOWER_VALUE, PH_SENS_HIGHER_VALUE);
  ph_sensor_value = constrain(ph_sensor_value, PH_SENS_LOWER_VALUE, PH_SENS_HIGHER_VALUE);

  if (ph_sensor_value == 0.0) {
    Serial.println("pH Warning: Sensor disconnected or incorrect values.");
  }
  return ph_sensor_value;
}

// Print sensor diagnostics
void printInfo() {
  Serial.println("-------- SENSOR DIAGNOSTICS --------");
  Serial.print("Oxygen: ");
  Serial.println(readOxygenSensor());
  Serial.print("EC: ");
  Serial.println(readConductivitySensor());
  Serial.print("pH: ");
  Serial.println(readpHSensor());
  Serial.println("------------------------------------");
}