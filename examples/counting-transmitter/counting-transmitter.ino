// TODO: DO NOT COMMIT! FOR PIO

/* counting-transmitter.cpp Example sketch for Quest IR Library
 * This example will send an incrementing payload to an IR LED
 * using the Quest IR encoding format.
 * 
 * Note: Update PIN_IR_TRANSMITTER to the pin connected to
 * an IR LED.
 */

#include <Quest_IR_Transmitter.h>

// update this pin
#define IR_SEND_PWM_PIN 11
Quest_IR_Transmitter irTransmitter;

//uint16_t message[] = {500, 1000, 1500, 500, 1000, 1500, 500, 1000, 1500};
uint16_t message[] = {500 * 3, 1000 * 3, 1500 * 3, 500 * 3, 1000 * 3, 1500 * 3, 500 * 3, 1000 * 3, 1500 * 3};

void setup()
{
    Serial.begin(9600);

    // start to listen for IR signals on the given pin
    irTransmitter.begin();
}

void loop()
{
    Serial.println("Sending");
    irTransmitter.send(message, sizeof(message));
    Serial.println("Sent");

    delay(2000);    
}