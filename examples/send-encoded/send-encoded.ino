/* send-encoded.cpp Example sketch for Quest IR Library
 * This example will send encoded data over IR.
 *
 * Note: For ItsyBitsy M0, the IR LED is set to pin 4 for
 * compatibility with the Quest stack running on a single MCU.
 */
#include <Quest_IR_Transmitter.h>
#include <Quest_IR_BitWriter.h>

Quest_IR_Transmitter irTransmitter;
Quest_IR_BitWriter irBitWriter = Quest_IR_BitWriter(irTransmitter.encodedBits, QIR_BUFFER_SIZE);

int8_t patternToSend = 0;

void setup()
{
    Serial.begin(9600);

    irTransmitter.begin();
}

void loop()
{
    irBitWriter.reset();

    switch (patternToSend)
    {
    case 0:
        Serial.println("---- Sending byte aligned packet (will also send padding) ----");
        irBitWriter.writeBits('H', 8);
        irBitWriter.writeBits('I', 8);
        break;
    case 1:
        Serial.println("---- Sending non-byte aligned packet ----");
        irBitWriter.writeBits(0xABC, 12);
        break;
    default:
        patternToSend = -1; // it'll be incremented to 0 right after this switch
        Serial.println("---- Sending less than one byte packet (odd bits for no padding) ----");
        irBitWriter.writeBits(0b00010101, 5);
        break;
    }

    patternToSend++;

    unsigned long startTimeMs = millis();
    irTransmitter.sendBits(irBitWriter.bitsWritten());
    unsigned long transmitTime = millis() - startTimeMs;

    Serial.print("Bits sent in ");
    Serial.print(transmitTime);
    Serial.println("ms");

    digitalWrite(LED_BUILTIN, HIGH);
    delay(500);
    digitalWrite(LED_BUILTIN, LOW);
    delay(1500);
}
