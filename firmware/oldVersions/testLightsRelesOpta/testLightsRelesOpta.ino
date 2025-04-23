// Programa para testear luces y relés en Arduino Opta

#define LED1 LED_D0  // STATUS LED 1
#define LED2 LED_D1  // STATUS LED 2
#define LED3 LED_D2  // STATUS LED 3
#define LED4 LED_D3  // STATUS LED 4

#define RELAY1 D0  // OUTPUT 1
#define RELAY2 D1  // OUTPUT 2
#define RELAY3 D2  // OUTPUT 3
#define RELAY4 D3  // OUTPUT 4

int intervalo = 500; // Intervalo de parpadeo en milisegundos

void setup() {
    digitalWrite(LED1, HIGH);
    blinkLED(2, 1000);
    blinkLED(3, 3000);
    blinkLED(4, 5000);
    Serial.begin(115200);
    pinMode(LED1, OUTPUT);
    pinMode(LED2, OUTPUT);
    pinMode(LED3, OUTPUT);
    pinMode(LED4, OUTPUT);
    pinMode(RELAY1, OUTPUT);
    pinMode(RELAY2, OUTPUT);
    pinMode(RELAY3, OUTPUT);
    pinMode(RELAY4, OUTPUT);
    Serial.println("Test de luces y relés en Arduino Opta");
    Serial.println("Comandos disponibles:");
    Serial.println(" ONx / OFFx - Enciende/Apaga LEDx");
    Serial.println(" RELONx / RELOFFx - Activa/Desactiva RELAYx");
    Serial.println(" BLINK x t - Parpadeo de LEDx cada t ms");
}

void loop() {
    if (Serial.available()) {
        String comando = Serial.readStringUntil('\n');
        comando.trim();
        
        if (comando.startsWith("ON")) {
            int led = comando.charAt(2) - '0';
            controlarLED(led, HIGH);
        } else if (comando.startsWith("OFF")) {
            int led = comando.charAt(3) - '0';
            controlarLED(led, LOW);
        } else if (comando.startsWith("RELON")) {
            int relay = comando.charAt(5) - '0';
            controlarRelay(relay, HIGH);
        } else if (comando.startsWith("RELOFF")) {
            int relay = comando.charAt(6) - '0';
            controlarRelay(relay, LOW);
        } else if (comando.startsWith("BLINK")) {
            int led, time;
            sscanf(comando.c_str(), "BLINK %d %d", &led, &time);
            blinkLED(led, time);
        } else {
            Serial.println("Comando no reconocido");
        }
    }
}

void controlarLED(int led, int estado) {
    int pin;
    switch (led) {
        case 1: pin = LED1; break;
        case 2: pin = LED2; break;
        case 3: pin = LED3; break;
        case 4: pin = LED4; break;
        default: Serial.println("LED no válido"); return;
    }
    digitalWrite(pin, estado);
    Serial.print("LED"); Serial.print(led);
    Serial.println(estado == HIGH ? " encendido" : " apagado");
}

void controlarRelay(int relay, int estado) {
    int pin;
    switch (relay) {
        case 1: pin = RELAY1; break;
        case 2: pin = RELAY2; break;
        case 3: pin = RELAY3; break;
        case 4: pin = RELAY4; break;
        default: Serial.println("Relé no válido"); return;
    }
    digitalWrite(pin, estado);
    Serial.print("Relé"); Serial.print(relay);
    Serial.println(estado == HIGH ? " activado" : " desactivado");
}

void blinkLED(int led, int time) {
    int pin;
    switch (led) {
        case 1: pin = LED1; break;
        case 2: pin = LED2; break;
        case 3: pin = LED3; break;
        case 4: pin = LED4; break;
        default: Serial.println("LED no válido"); return;
    }
    
    Serial.print("Parpadeo LED"); Serial.print(led); Serial.print(" cada "); Serial.print(time); Serial.println(" ms");
    for (int i = 0; i < 10; i++) { // Parpadeo 10 veces
        digitalWrite(pin, HIGH);
        delay(time);
        digitalWrite(pin, LOW);
        delay(time);
    }
}
