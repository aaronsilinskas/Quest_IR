#include "Quest_IR_Receiver.h"

void Quest_IR_Receiver::begin(uint8_t irPin)
{
    receiver = IRrecvPCI(irPin);
    receiver.enableIRIn();
}

void Quest_IR_Receiver::enable()
{
    receiver.enableIRIn();
}

void Quest_IR_Receiver::disable()
{
    receiver.disableIRIn();
}

void Quest_IR_Receiver::enableBlink(bool enabled)
{
    receiver.blink13(enabled);
}

bool Quest_IR_Receiver::hasSignal()
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
    decodedBitCount = 0;
    decodeWriter.reset();
    receiver.enableIRIn();
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
    decodedBitCount = 0;
    decodeWriter.reset();
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

#ifdef DEBUG_IR_RECEIVER
    Serial.print(F("Bits received: "));
    Serial.println(lastBit - 3); // don't count the 2 header pulses
#endif

    for (bufIndex_t i = 3; i < lastBit; i++)
    {
        decodeWriter.writeBit(signalToBit(recvGlobal.recvBuffer[i]));

        // if signalToBit flagged an invalid pulse, stop decoding
        if (decodeState == InvalidPulse)
        {
            return;
        }
    }

    decodedBitCount = decodeWriter.bitsWritten();

#ifdef DEBUG_IR_RECEIVER
    Serial.print(F("Decoded bits: "));
    printBinaryArray(decodedBits, QIR_BUFFER_SIZE);
#endif

    // make sure the packet bits have not been corrupted
    if (!verifyCRC())
    {
        return;
    }

    // valid packet received!
    decodeState = Decoded;

#ifdef DEBUG_IR_RECEIVER
    Serial.println(F("Decode successful"));
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

    uint8_t calculatedCRC = calculateCRC(decodedBits, decodedBitCount);
    if (expectedCRC != calculatedCRC)
    {
#ifdef DEBUG_IR_RECEIVER
        Serial.print(F("CRC check failed. Expected: "));
        Serial.print(expectedCRC, BIN);
        Serial.print(F("  Calculated: "));
        Serial.println(calculatedCRC, BIN);
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
