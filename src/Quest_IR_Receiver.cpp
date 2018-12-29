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
    decodeState = NoData;
    clearBuffer();
    receiver.enableIRIn();
}

uint16_t Quest_IR_Receiver::unreadBits()
{
    // bit position can be greater than received when more bits are read than available
    if (nextBitPosition > bitsReceived)
    {
        return 0;
    }
    return bitsReceived - nextBitPosition;
}

uint32_t Quest_IR_Receiver::readBits(uint8_t bitsToRead)
{
    // already read all of the bits
    if (nextBitPosition >= bitsReceived)
    {
        return 0;
    }

    // do not read more bits than received
    if (nextBitPosition + bitsToRead > bitsReceived)
    {
        bitsToRead = bitsReceived - nextBitPosition;
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
    // minimum bits needed: header (2) + at least one data bit (1) + CRC (4) + leadout (1)
    if (recvGlobal.recvLength < 8)
    {
        setInvalidPacketState(SignalTooShort);
        return;
    }

    // verify the packet's header is QIR
    bool headerMatch = checkSignal(QIR_HEADER_MARK, recvGlobal.recvBuffer[1]) && checkSignal(QIR_HEADER_SPACE, recvGlobal.recvBuffer[2]);
    if (!headerMatch)
    {
        setInvalidPacketState(HeaderMismatch);
        return;
    }

    // skip the leadout (1 bit) and CRC (4 bits)
    bufIndex_t lastBit = recvGlobal.recvLength - 4;

    // skip the last mark if it is just padding
    if (checkSignal(QIR_PULSE_PADDING, recvGlobal.recvBuffer[lastBit - 1]))
    {
        lastBit--;
    }

    // calculate the number of expected bits, don't bother counting as we go
    bitsReceived = lastBit - 3;

#ifdef DEBUG_IR_RECEIVER
    Serial.print(F("Bits received: "));
    Serial.println(bitsReceived);
#endif

    uint8_t bufferPos = 0;
    uint8_t buffer = 0;
    uint16_t bitMask = 0b10000000;
    for (bufIndex_t i = 3; i < lastBit; i++)
    {
        // buffer is all zero's, only update when a 1 is received
        if (signalToBit(recvGlobal.recvBuffer[i]))
        {
            // it's a 1, update the buffer
            buffer |= bitMask;
        }

        // if signalToBit flagged an invalid pulse, stop decoding
        if (decodeState == InvalidPulse)
        {
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

    // make sure the packet bits have not been corrupted
    if (!verifyCRC())
    {
        return;
    }

    // valid packet received!
    decodeState = Decoded;

#ifdef DEBUG_IR_RECEIVER
    Serial.print(F("Decode successful: "));
    for (uint8_t i = 0; i < QIR_BIT_BUFFERS; i++)
    {
        Serial.print(bitBuffers[i], BIN);
        Serial.print(F(" "));
    }
    Serial.println();
#endif
}

bool Quest_IR_Receiver::verifyCRC()
{
    // decode the received CRC
    uint8_t expectedCRC = decodeCRCSignals();

    // if the decoded CRC contained an invalid pulse, stop decoding
    if (decodeState == InvalidPulse)
    {
        return false;
    }

    uint8_t actualCRC = calculateCRC(bitBuffers, bitsReceived);
    if (expectedCRC != actualCRC)
    {
#ifdef DEBUG_IR_RECEIVER
        Serial.print(F("CRC check failed. Expected: "));
        Serial.print(expectedCRC, BIN);
        Serial.print(F("  Calculated: "));
        Serial.println(actualCRC, BIN);
        ;
#endif

        setInvalidPacketState(InvalidCRC);
        return false;
    }

    return true;
}

uint8_t Quest_IR_Receiver::decodeCRCSignals()
{
    uint8_t crc = 0;

    // the CRC is in the last 4 bits before the final space
    bufIndex_t crcFirstBit = recvGlobal.decodeLength - 4;
    if (signalToBit(recvGlobal.recvBuffer[crcFirstBit]))
    {
        crc |= 0b1000;
    }
    if (signalToBit(recvGlobal.recvBuffer[crcFirstBit + 1]))
    {
        crc |= 0b0100;
    }
    if (signalToBit(recvGlobal.recvBuffer[crcFirstBit + 2]))
    {
        crc |= 0b0010;
    }
    if (signalToBit(recvGlobal.recvBuffer[crcFirstBit + 3]))
    {
        crc |= 0b0001;
    }
    return crc;
}

inline bool Quest_IR_Receiver::checkSignal(uint16_t expected, uint16_t actual)
{
    int32_t diff = abs((int32_t)actual - expected);
    return diff < QIR_ERROR_MARGIN;
}

inline bool Quest_IR_Receiver::signalToBit(uint16_t signal)
{
    if (checkSignal(QIR_PULSE_ZERO, signal))
    {
        return false;
    }
    else if (checkSignal(QIR_PULSE_ONE, signal))
    {
        return true;
    }
    else
    {
        // pulse width is unknown, likely a corrupted signal
        setInvalidPacketState(InvalidPulse);

#ifdef DEBUG_IR_RECEIVER
        Serial.print(F("Invalid signal: "));
        Serial.println(signal);
#endif
        return false;
    }
}

void Quest_IR_Receiver::setInvalidPacketState(DecodeState state)
{
    decodeState = state;

#ifdef DEBUG_IR_RECEIVER
    Serial.print(F("Invalid packate state: "));
    Serial.println(state);
#endif

    clearBuffer();
}
