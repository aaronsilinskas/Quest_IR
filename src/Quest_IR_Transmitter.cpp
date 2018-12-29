#include "Quest_IR_Transmitter.h"

void Quest_IR_Transmitter::begin()
{
}

void Quest_IR_Transmitter::sendRawSignal(uint16_t *buffer, uint8_t length)
{
    enableIROut(38);

    for (uint8_t i = 0; i < length; i++)
    {
        if (i & 1)
        {
            space(buffer[i]);
        }
        else
        {
            mark(buffer[i]);
        }
    }
}

void Quest_IR_Transmitter::clearBits()
{
    nextBitPosition = 0;
    memset(bitBuffers, 0, sizeof(bitBuffers));
}

bool Quest_IR_Transmitter::addBits(uint32_t data, uint8_t bitsToSend)
{
#ifdef DEBUG_IR_TRANSMITTER
    Serial.print("Adding: ");
    Serial.print(data, BIN);
    Serial.print(" (");
    Serial.print(bitsToSend);
    Serial.print(" bits)");
    Serial.print(" to Position: ");
    Serial.println(nextBitPosition);
#endif

    // make sure there is enough room in the buffer
    uint16_t bitsAfterAdd = nextBitPosition + bitsToSend;
    if (bitsAfterAdd >= QIR_MAX_BITS)
    {
        return false;
    }

    // shift the data so the first bit as left-most
    data <<= (32 - bitsToSend);

    // add each bit from the data to the correct bit buffer
    for (uint16_t bitPos = nextBitPosition; bitPos < bitsAfterAdd; bitPos++)
    {
        if (data & QIR_TOP_BIT)
        {
            uint8_t bufferPos = bitPos >> 3;
            uint8_t bitMask = 0b10000000 >> (bitPos & 0b111);

#ifdef DEBUG_IR_TRANSMITTER
            Serial.print("Bit: ");
            Serial.print(bitPos);
            Serial.print(" Buffer: ");
            Serial.print(bufferPos);
            Serial.print(" Data: ");
            Serial.print(bitBuffers[bufferPos], BIN);
            Serial.print(" | ");
            Serial.print(bitMask, BIN);
            Serial.print(" = ");
#endif

            bitBuffers[bufferPos] = bitBuffers[bufferPos] | bitMask;

#ifdef DEBUG_IR_TRANSMITTER
            Serial.println(bitBuffers[bufferPos], BIN);
#endif
        }
        data <<= 1;
    }

    // update the bit position after adding the data
    nextBitPosition = bitsAfterAdd;

    return true;
}

bool Quest_IR_Transmitter::sendBits()
{
    // no bits to send?
    if (nextBitPosition == 0)
    {
        return false;
    }

    uint8_t crc = calculateCRC(bitBuffers, nextBitPosition);

#ifdef DEBUG_IR_TRANSMITTER
    Serial.print("Sending Quest Message with CRC: ");
    Serial.println(crc, BIN);
#endif

    // start IR transmit
    enableIROut(38);

    // send the header
    mark(QIR_HEADER_MARK);
    space(QIR_HEADER_SPACE);

    // encode each bit in the buffer, one bit encoded in each mark and each space
    uint8_t bufferPos = 0;
    uint8_t bitMask = 0b10000000;
    for (uint16_t bitPos = 0; bitPos < nextBitPosition; bitPos++)
    {
#ifdef DEBUG_IR_TRANSMITTER
        Serial.print("Bit: ");
        Serial.print(bitPos);
        Serial.print(" Buffer: ");
        Serial.print(bufferPos);
        Serial.print(" Data: ");
        Serial.print(bitBuffers[bufferPos], BIN);
        Serial.print(" & ");
        Serial.print(bitMask, BIN);
        Serial.print(" = ");
#endif

        if (bitPos & 1)
        {
            sendSpace(bitBuffers[bufferPos] & bitMask);
        }
        else
        {
            sendMark(bitBuffers[bufferPos] & bitMask);
        }

        // move to the next bit in the buffer
        bitMask >>= 1;
        if (bitMask == 0)
        {
            // finished the current buffer, move to the next one
            bufferPos++;
            bitMask = 0b10000000;
        }
    }

    // if sending an odd number of bits, add an extra mark to ensure all bits are sent
    if ((nextBitPosition & 1) == 0)
    {
#ifdef DEBUG_IR_TRANSMITTER
        Serial.println("M_PADDING");
#endif
        mark(QIR_PULSE_PADDING);
    }

    // send the CRC
    sendSpace(crc & 0b1000);
    sendMark(crc & 0b0100);
    sendSpace(crc & 0b0010);
    sendMark(crc & 0b0001);

    space(QIR_LEAD_OUT);

    return true;
}

inline void Quest_IR_Transmitter::sendSpace(bool one)
{
    if (one)
    {
#ifdef DEBUG_IR_TRANSMITTER
        Serial.println("S_1");
#endif
        space(QIR_PULSE_ONE);
    }
    else
    {
#ifdef DEBUG_IR_TRANSMITTER
        Serial.println("S_0");
#endif
        space(QIR_PULSE_ZERO);
    }
}

inline void Quest_IR_Transmitter::sendMark(bool one)
{
    if (one)
    {
#ifdef DEBUG_IR_TRANSMITTER
        Serial.println("M_1");
#endif
        mark(QIR_PULSE_ONE);
    }
    else
    {
#ifdef DEBUG_IR_TRANSMITTER
        Serial.println("M_0");
#endif
        mark(QIR_PULSE_ZERO);
    }
}