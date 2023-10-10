#include <Arduino.h>

#define hall_pin 15


int hall_count = 0;

void setup(){
    Serial.begin(115200);
    pinMode(hall_pin, INPUT);

}

void loop(){
    if(digitalRead(hall_pin) == 0){
        hall_count++;

    }

    Serial.println(hall_count);
}