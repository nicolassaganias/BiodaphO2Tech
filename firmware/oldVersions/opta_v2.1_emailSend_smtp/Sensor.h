// choose the appropriate EC probe type
//#define EC_PROBE_01L                           // EC Probe K 0.1L
#define EC_PROBE_01                         // EC Probe K 0.1
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

// map function to map the sensors minimum and maximum values
float mapFloat(float x, float in_min, float in_max, float out_min, float out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

// read the dissolved oxygen sensor adc value
float readOxygenADC() {
  // Serial.println("-------- OXYGEN SENSOR --------");
  uint32_t rawValue = 0;
  for (int i = 0; i < 10; i++) {
    rawValue += analogRead(SENSOR_DISSOLVE_OXYGEN_PIN);  // read the adc value
    delay(10);
  }
  float avgValue = rawValue / 10.0;
  float voltage = 0.002710 * avgValue - 0.0084;  // calculate the voltage
  // Serial.print(" ADC: ");
  // Serial.print(avgValue);
  // Serial.print(" | Voltage: ");
  // Serial.print(voltage);  // print the voltage
  return voltage;  // return the voltage
}

// read the dissolved oxygen sensor value
float readOxygenSensor() {
  float voltage = readOxygenADC();                                   // read the sensor voltage
  float current = (voltage / OXYGEN_SENS_RESISTOR_VALUE) * 1000.00;  // convert voltage to current
  // Serial.print("| current:  ");
  // Serial.print(current);

  // calculate the oxygen sensor value
  float oxygen_sensor_value = min(max(0, mapFloat(current, 4.00, 20.00, OXYGEN_SENS_LOWER_VALUE, OXYGEN_SENS_HIGHER_VALUE)), OXYGEN_SENS_HIGHER_VALUE);
  if (oxygen_sensor_value >= OXYGEN_SENS_LOWER_VALUE && oxygen_sensor_value <= OXYGEN_SENS_HIGHER_VALUE) {  // the check sensor value validity
    // Serial.print("| Oxygen: ");
    // Serial.print(oxygen_sensor_value);  // print the sensor value
    // Serial.println();
    return oxygen_sensor_value;
  } else {
    Serial.println(" Oxyegn Warning: The sensor might be connected incorrectly or receiving inaccurate values.");  // print an error message
    return 0;
  }
}

// read the conductivity (ec) sensor adc value
float readECADC() {
  // Serial.println("-------- EC SENSOR --------");
  uint32_t rawValue = 0;
  for (int i = 0; i < 10; i++) {
    rawValue += analogRead(SENSOR_CONDUCTIVITY_PIN);  // read the adc value
    delay(10);
  }
  float avgValue = rawValue / 10.0;
  float voltage = 0.002700 * avgValue - 0.0084;  // calculate the voltage
  // Serial.print(" ADC: ");
  // Serial.print(avgValue);
  // Serial.print(" | Voltage: ");
  // Serial.print(voltage);
  return voltage;  // return the voltage value
}

// read the conductivity (ec) sensor value
float readConductivitySensor() {
  float voltage = readECADC();                                   // read the sensor voltage
  float current = (voltage / EC_SENS_RESISTOR_VALUE) * 1000.00;  // convert voltage to current
  // Serial.print("| current:  ");
  // Serial.print(current);  // print measured constant current

  // calculate the sensor value
  float ec_sensor_value = (min(max(1, mapFloat(current, 4.00, 20.00, EC_SENS_LOWER_VALUE, EC_MAX_SENSOR_VALUE)), EC_MAX_SENSOR_VALUE));
  if (ec_sensor_value <= EC_TDS_VALUE_RANGE) {  // check the sensor value validity
    // Serial.print("| EC: ");
    // Serial.print(ec_sensor_value);  // print the sensor value
    // Serial.println();
    return ec_sensor_value;
  } else {
    Serial.println(" EC Warning: The sensor might be connected incorrectly or the wrong sensor probe may have been selected.");  // print a error message
    return 0;
  }
}


// read the pH sensor adc value
float readpHADC() {
  //Serial.println("-------- PH SENSOR --------");
  uint32_t rawValue = 0;
  for (int i = 0; i < 10; i++) {
    rawValue += analogRead(SENSOR_PH_PIN);  // read adc value
    delay(10);
  }
  float avgValue = rawValue / 10.0;
  float voltage = 0.0027 * avgValue - 0.0083;  // calculate the voltage
  // Serial.print(" ADC: ");
  // Serial.print(avgValue);
  // Serial.print(" | Voltage: ");
  // Serial.print(voltage);  // print the voltage
  return voltage;  // return the voltage value
}

// read the pH sensor value
float readpHSensor() {
  float voltage = readpHADC();                                   // read the sensor voltage
  float current = (voltage / PH_SENS_RESISTOR_VALUE) * 1000.00;  // convert voltage to current
  // Serial.print("| current:  ");
  // Serial.print(current);  // print the receive constant current

  // calculate the ec sensor value
  float ph_sensor_value = min(max(0.0, mapFloat(current, 4.00, 20.00, PH_SENS_LOWER_VALUE, PH_SENS_HIGHER_VALUE)), PH_SENS_HIGHER_VALUE);
  if (ph_sensor_value >= PH_SENS_LOWER_VALUE && ph_sensor_value <= PH_SENS_HIGHER_VALUE) {  // check the pH sensor validity
    // Serial.print(" | pH: ");
    // Serial.print(ph_sensor_value);  // print the sensor the value
    // Serial.println();
    return ph_sensor_value;
  } else {
    Serial.println(" pH Warning: The sensor might be connected incorrectly or receiving inaccurate values.");  // print error message
    return 0;
  }
}