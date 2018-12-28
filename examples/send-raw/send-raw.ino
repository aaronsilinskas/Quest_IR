/* send-raw.cpp Example sketch for Quest IR Library
 * This example will send marks and delays to test basic signal
 * reception.
 * 
 * Note: For ItsyBitsy M0, the IR LED is set to pin 4 for 
 * compatibility with the Quest stack running on a single MCU.
 */
#include "Quest_IR_Transmitter.h"

Quest_IR_Transmitter irTransmitter;

// Header: 8000, 4000
// Mark 1: 1500, 500
// Mark 2: 500, 500
// Mark 3: 500, 1500
// Mark 4: 500, 500
const uint16_t PULSE = 500;
uint16_t messageBuffer[] = {PULSE * 16, PULSE * 8, PULSE * 3, PULSE, PULSE, PULSE, PULSE, PULSE * 3, PULSE, PULSE};

void setup()
{
    Serial.begin(9600);

    irTransmitter.begin();
}

void loop()
{
    irTransmitter.sendRawSignal(messageBuffer, sizeof(messageBuffer) / 2);
    Serial.println("Signal Sent");

    digitalWrite(LED_BUILTIN, HIGH);
    delay(500);
    digitalWrite(LED_BUILTIN, LOW);
    delay(1500);
}