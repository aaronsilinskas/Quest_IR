#include "Quest_IR_Receiver.h"

void Quest_IR_Receiver::begin(uint8_t irPin)
{
    receiver = IRrecvPCI(irPin);
    receiver.enableIRIn();
}

void Quest_IR_Receiver::enableBlink(bool enabled)
{
    receiver.blink13(enabled);
}

bool Quest_IR_Receiver::hasData()
{
    if (receiver.getResults())
    {
        attemptDecode();
        return true;
    }
    return false;
}

void Quest_IR_Receiver::reset()
{
    clearBuffer();
    receiver.enableIRIn();
}

uint32_t Quest_IR_Receiver::readBits(uint8_t bitsToRead)
{
    // already read all of the bits
    if (nextBitPosition >= bitsReceived)
    {
#ifdef DEBUG_IR_RECEIVER
        Serial.println("All bits read, returning 0");
#endif
        return 0;
    }

    // do not read more bits than received
    if (nextBitPosition + bitsToRead > bitsReceived)
    {
        bitsToRead = bitsReceived - nextBitPosition;

#ifdef DEBUG_IR_RECEIVER
        Serial.print("Reading more bits than received, only returning ");
        Serial.println(bitsToRead);
#endif
    }

    uint32_t readBits = 0;

    uint8_t bufferPos = nextBitPosition >> 3;
    uint8_t buffer = bitBuffers[bufferPos];
    uint8_t bufferBitMask = 0b10000000 >> (nextBitPosition & 0b111);

    for (uint16_t i = 0; i < bitsToRead; i++)
    {
        // shift the read bits, and set the next bit from the buffer
        readBits <<= 1;
        if (buffer & bufferBitMask)
        {
            readBits |= 1;
        }

        // move to the next bit in the buffer
        bufferBitMask >>= 1;
        if (bufferBitMask == 0)
        {
            // the current buffer has been read, move to the next
            bufferPos++;
            bufferBitMask = 0b10000000;
        }
    }

    nextBitPosition += bitsToRead;

    return readBits;
}

void Quest_IR_Receiver::printRawSignal()
{
    Serial.print(F("Header: "));
    Serial.print(recvGlobal.recvBuffer[1], DEC);
    Serial.print(F(", "));
    Serial.println(recvGlobal.recvBuffer[2], DEC);

    for (bufIndex_t i = 3; i < recvGlobal.recvLength; i += 2)
    {
        int mark = recvGlobal.recvBuffer[i];
        int bitDelay = recvGlobal.recvBuffer[i + 1];

        Serial.print(F(" Mark "));
        Serial.print(mark, DEC);
        Serial.print(F(" Delay "));
        Serial.println(bitDelay, DEC);
    }
    Serial.println();
}

void Quest_IR_Receiver::clearBuffer()
{
    nextBitPosition = 0;
    bitsReceived = 0;
    memset(bitBuffers, 0, sizeof(bitBuffers));
}

void Quest_IR_Receiver::attemptDecode()
{
    if (recvGlobal.recvLength < 4)
    {
#ifdef DEBUG_IR_RECEIVER
        Serial.println("Not enough signals received");
#endif

        return;
    }

    bool headerMatch = checkBuffer(QIR_HEADER_MARK, recvGlobal.recvBuffer[1]) && checkBuffer(QIR_HEADER_SPACE, recvGlobal.recvBuffer[2]);
    if (!headerMatch)
    {
#ifdef DEBUG_IR_RECEIVER
        Serial.println("Header mismatch");
#endif
        return;
    }

    // skip the last delay
    bufIndex_t lastBit = recvGlobal.recvLength - 1;

    // skip the last mark if it is just padding
    if (checkBuffer(QIR_PULSE_PADDING, recvGlobal.recvBuffer[lastBit - 1]))
    {
        lastBit--;

#ifdef DEBUG_IR_RECEIVER
        Serial.println("Padding on last bit, ignoring it");
#endif
    }

    // calculate the number of expected bits, don't bother counting as we go
    bitsReceived = lastBit - 3;

#ifdef DEBUG_IR_RECEIVER
    Serial.print("Bits received: ");
    Serial.println(bitsReceived);
#endif

    uint8_t bufferPos = 0;
    uint8_t buffer = 0;
    uint16_t bitMask = 0b10000000;
    for (bufIndex_t i = 3; i < lastBit; i++)
    {
        uint16_t signal = recvGlobal.recvBuffer[i];
        if (checkBuffer(QIR_PULSE_ZERO, signal))
        {
            // it's a 0, skip
        }
        else if (checkBuffer(QIR_PULSE_ONE, signal))
        {
            // it's a 1, update the buffer
            buffer |= bitMask;
        }
        else
        {
#ifdef DEBUG_IR_RECEIVER
            Serial.print("Bad signal, stopping decode: ");
            Serial.println(signal);
#endif
            // bad signal, stop decoding
            clearBuffer();
            return;
        }

        // move to the next bit in the buffer
        bitMask >>= 1;
        if (bitMask == 0)
        {
            // the current buffer is full, store it and move to the next
            bitBuffers[bufferPos] = buffer;
            buffer = 0;

            bufferPos++;
            bitMask = 0b10000000;
        }
    }

    // make sure the last buffer is stored in cases where bits received is not byte-aligned
    if (bufferPos < QIR_BIT_BUFFERS)
    {
        bitBuffers[bufferPos] = buffer;
    }

#ifdef DEBUG_IR_RECEIVER
    Serial.print("Decode successful: ");
    for (uint8_t i = 0; i < QIR_BIT_BUFFERS; i++)
    {
        Serial.print(bitBuffers[i], BIN);
        Serial.print(" ");
    }
    Serial.println();
#endif
}

bool Quest_IR_Receiver::checkBuffer(uint16_t expected, uint16_t actual)
{
    int32_t diff = abs((int32_t)actual - expected);
    return diff < QIR_ERROR_MARGIN;
}
