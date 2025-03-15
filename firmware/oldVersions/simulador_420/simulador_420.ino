const int pin_oxygen = 9;
const int pin_ec = 6;
const int pin_ph = 5;

// Resistencia utilizada para conversión de voltaje a corriente
const float RESISTOR_VALUE = 240.0;

// Función para interpolar valores flotantes (similar a map() de Arduino)
float mapFloat(float x, float in_min, float in_max, float out_min, float out_max) {
  return out_min + ((x - in_min) * (out_max - out_min) / (in_max - in_min));
}

// Generador de números aleatorios dentro del rango esperado en el Opta
float getRandomDecimal(float minVal, float maxVal) {
  return minVal + (maxVal - minVal) * ((float)random(0, 1000) / 1000.0);
}

void setup() {
  pinMode(pin_oxygen, OUTPUT);
  pinMode(pin_ec, OUTPUT);
  pinMode(pin_ph, OUTPUT);
  Serial.begin(115200);
  randomSeed(analogRead(A0));  // Inicializar semilla aleatoria
}

void loop() {
  // **Generamos los valores que queremos ver reflejados en el Opta**
  float oxygen_sensor_value = getRandomDecimal(0.1, 2.0);  // mg/L (valor real esperado)
  float ec_sensor_value = getRandomDecimal(1200, 1700);    // μS/cm (valor real esperado)
  float ph_sensor_value = getRandomDecimal(7.2, 7.7);      // pH (valor real esperado)

  // **Convertimos estos valores a miliamperios (4-20mA) según las tablas**
  float oxygen_mA = (oxygen_sensor_value * 0.5) + 4;                 // Según la tabla de O2
  float ec_mA = mapFloat(ec_sensor_value, 0, 25000, 4.00064, 20.0);  // Según la tabla de EC
  float ph_mA = ((ph_sensor_value / 14.0) * 16.0) + 4.0 - 2.0;       // Según la tabla de pH

  // **Convertimos a PWM**
  int pwm_oxygen = oxygen_mA;
  int pwm_ec = ec_mA;
  int pwm_ph = ph_mA;

  // **Enviar las señales PWM simuladas**
  analogWrite(pin_oxygen, pwm_oxygen);
  analogWrite(pin_ec, pwm_ec);
  analogWrite(pin_ph, pwm_ph);

  // **Imprimir valores en el serial para verificar**
  Serial.print("Oxygen Sensor: ");
  Serial.print(oxygen_sensor_value);
  Serial.print(" mg/L -> ");
  Serial.print(oxygen_mA);
  Serial.print(" mA");
  Serial.print(" pwm -> ");
  Serial.println(pwm_oxygen);
  Serial.print("EC Sensor: ");
  Serial.print(ec_sensor_value);
  Serial.print(" μS/cm -> ");
  Serial.print(ec_mA);
  Serial.print(" mA");
  Serial.print(" pwm -> ");
  Serial.println(pwm_ec);
  Serial.print("pH Sensor: ");
  Serial.print(ph_sensor_value);
  Serial.print(" pH -> ");
  Serial.print(ph_mA);
  Serial.print(" mA");
  Serial.print(" pwm -> ");
  Serial.println(pwm_ph);

  Serial.println("--------------------------------------");

  delay(2000);
}
