#include <Arduino.h>

#define hall_pin 15


volatile int revolutions;
float rpmilli;
float speed;
unsigned long timeold = 0;
unsigned long timeold2 = 0;

void increase_revolutions(){
    revolutions++;
}

void setup(){
    Serial.begin(115200);
    pinMode(hall_pin, INPUT);
    attachInterrupt(digitalPinToInterrupt(hall_pin), increase_revolutions, RISING);
    revolutions = 0;
    rpmilli = 0;
    timeold = millis();
    timeold2 = millis();

}

void read_wind(){
    if(millis() -timeold2 >= 1000){
        float rps = (1000.0 *revolutions) / (millis() -timeold);
        Serial.print("RPS: ");
        Serial.println(rps);
        timeold2 = millis();
        Serial.println(revolutions);
    }
    if(millis() - timeold >= 10000){
        timeold = millis();
        revolutions = 0;
    }

}

void loop(){
    read_wind();
}