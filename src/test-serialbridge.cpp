#include <Arduino.h>

#define COMPUTER_BAUDRATE 9600
#define GPS_BAUDRATE 115200//9600

void setup()
{
    Serial.begin(COMPUTER_BAUDRATE);
    Serial2.begin(GPS_BAUDRATE);
}

void loop()
{
    // If there is data from the GPS receiver, read it and send it to the computer or vice versa.
    if (Serial2.available())
    {
        Serial.write(Serial2.read());
    }

    if (Serial.available())
    {
        Serial2.write(Serial.read());
    }
}