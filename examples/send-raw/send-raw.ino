/* send-raw.cpp Example sketch for Quest IR Library
 * This example will send marks and delays to test basic signal
 * reception.
 * 
 * Note: For ItsyBitsy M0, the IR LED is set to pin 3 for 
 * compatibility with the Quest stack running on a single MCU.
 */
#include "Quest_IR_Transmitter.h"

Quest_IR_Transmitter irTransmitter;

// Header: 2000, 500
// Mark 1: 1000, 500
// Mark 2: 750, 750
// Mark 3: 500, 1000
uint16_t messageBuffer[] = {2000, 500, 1000, 500, 750, 750, 500, 1000};

void setup()
{
    Serial.begin(9600);

    // start to listen for IR signals on the given pin
    irTransmitter.begin();
}

void loop()
{
    irTransmitter.sendRawSignal(messageBuffer, sizeof(messageBuffer));
    Serial.println("Signal Sent");

    digitalWrite(LED_BUILTIN, HIGH);
    delay(500);
    digitalWrite(LED_BUILTIN, LOW);
    delay(500);
}