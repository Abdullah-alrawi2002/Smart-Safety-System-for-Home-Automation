#include "Arduino.h"
#include "DCMotor.h"
#include "NewPing.h"
#include "LED.h"
#include "PIR.h"
#include "PiezoSpeaker.h"
#include "Servo.h"

// Pin Definitions
#define HCSR04_PIN_TRIG  9
#define HCSR04_PIN_ECHO  8
#define LED_PIN_VIN      4
#define MQ4_PIN_AOUT     A10
#define PIR_PIN_SIG      10
#define SERVO_PIN_SIG    11

// Global variables and defines
unsigned int piezoSpeakerNoteDurations[] = {8, 8, 8, 4, 8, 4};
unsigned int piezoSpeakerMelody[] = {NOTE_C4, NOTE_E4, NOTE_G4, NOTE_C5, NOTE_G4, NOTE_C5};
const int timeout = 10000; //define timeout of 10 sec
char menuOption = 0;
long time0;

// Objects initialization
NewPing sonar(HCSR04_PIN_TRIG, HCSR04_PIN_ECHO);
LED led(LED_PIN_VIN);
PIR pir(PIR_PIN_SIG);
PiezoSpeaker piezoSpeaker(PIR_PIN_SIG);
Servo servo;

void setup() {
    Serial.begin(9600);
    while (!Serial);
    Serial.println("Start");
    servo.attach(SERVO_PIN_SIG, 500, 2500); // Set bounds for servo
    menuOption = menu();
}

void loop() {
    if(menuOption == '1') {
        int distance = sonar.ping_cm();
        Serial.print("Distance: "); Serial.print(distance); Serial.println(" cm");
        if (distance < 15) {
            servo.write(90);
            delay(5000); // Wait for 5 seconds
        } else {
            servo.write(0);
        }
        int gasLevel = analogRead(MQ4_PIN_AOUT);
        Serial.print("Gas level: "); Serial.println(gasLevel);
        if (gasLevel >= 220) {
            led.on();
        } else {
            led.off();
        }
        bool pirValue = pir.read();
        Serial.print("PIR Value: "); Serial.println(pirValue);
        if (pirValue) {
            piezoSpeaker.playMelody(sizeof(piezoSpeakerMelody) / sizeof(piezoSpeakerMelody[0]), piezoSpeakerMelody, piezoSpeakerNoteDurations);
        }
        delay(1000); // Wait for 1 second between reads
    }
    if (millis() - time0 > timeout) {
        menuOption = menu();
    }
}

char menu() {
    Serial.println(F("\nWhich component would you like to test?"));
    Serial.println(F("(1) Test Suite: Distance, Gas, PIR, and Servo"));
    while (!Serial.available());
    while (Serial.available()) {
        char c = Serial.read();
        if (isAlphaNumeric(c)) {
            if (c == '1') {
                Serial.println(F("Testing Suite Activated"));
                time0 = millis();
                return c;
            }
            else {
                Serial.println(F("Invalid input!"));
                return 0;
            }
        }
    }
}
