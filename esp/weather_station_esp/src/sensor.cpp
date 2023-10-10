#include <Arduino.h>

#define hall_pin 15


volatile byte revolutions;
float rpmilli;
float speed;
unsigned long timeold = 0;

void increase_revolutions(){
    revolutions++;
}

void setup(){
    Serial.begin(115200);
    pinMode(hall_pin, INPUT);
    attachInterrupt(digitalPinToInterrupt(hall_pin), increase_revolutions, RISING);
    revolutions = 0;
    rpmilli = 0;
    timeold = 0;

}

void loop(){
    if (revolutions >= 1) {
         //Update RPM every 20 counts, increase this for better RPM resolution,
         //decrease for faster update

         // calculate the revolutions per milli(second)
         rpmilli = ((float)revolutions)/(millis()-timeold);

         timeold = millis();
         revolutions = 0;
         Serial.println(revolutions);
         Serial.println(rpmilli);
         Serial.println(timeold);
         // WHEELCIRC = 2 * PI * radius (in meters)
         // WHEELCIRC = 2 * PI * 0.095 = 0,59690260418206071530790224282311
         // speed = rpmilli * WHEELCIRC * “milliseconds per hour” / “meters per kilometer”

         // simplify the equation to reduce the number of floating point operations
         // speed = rpmilli * WHEELCIRC * 3600000 / 1000
         // speed = rpmilli * WHEELCIRC * 3600

        //  speed = rpmilli 1.288053600;
        speed = rpmilli * 2148.849375055418575;

         Serial.print("RPM:");
         Serial.println(rpmilli * 60000 ,DEC);
         Serial.print(" Speed:");
         Serial.print(speed,DEC);
         Serial.println(" kph");
    }
}