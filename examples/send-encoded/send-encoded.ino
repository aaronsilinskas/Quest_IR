/* send-encoded.cpp Example sketch for Quest IR Library
 * This example will send encoded data over IR.
 * 
 * Note: For ItsyBitsy M0, the IR LED is set to pin 4 for 
 * compatibility with the Quest stack running on a single MCU.
 */
#include "Quest_IR_Transmitter.h"

Quest_IR_Transmitter irTransmitter;

void setup()
{
    Serial.begin(9600);

    irTransmitter.begin();
}

void loop()
{
    irTransmitter.clearBits();
    irTransmitter.addBits('H', 8);
    irTransmitter.addBits('I', 8);
    irTransmitter.addBits(7, 3);
    irTransmitter.addBits(1, 3);
    irTransmitter.addBits(1, 2);

    unsigned long startTimeMs = millis();
    irTransmitter.sendBits();
    unsigned long transmitTime = millis() - startTimeMs;

    Serial.print("Bits sent in ");
    Serial.print(transmitTime);
    Serial.println("ms");

    digitalWrite(LED_BUILTIN, HIGH);
    delay(500);
    digitalWrite(LED_BUILTIN, LOW);
    delay(1500);
}
