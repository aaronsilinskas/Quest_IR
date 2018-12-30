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

bool Quest_IR_Transmitter::sendBits(uint16_t encodedBitsToSend)
{
    // no bits to send?
    if (encodedBitsToSend == 0)
    {
#ifdef DEBUG_IR_TRANSMITTER
        Serial.println("Trying to send with no encoded bits: ");
#endif
        return false;
    }

    // too many bits to send?
    if (encodedBitsToSend > (QIR_BUFFER_SIZE << 3))
    {
#ifdef DEBUG_IR_TRANSMITTER
        Serial.print("Trying to send too many bits: ");
        Serial.println(encodedBitsToSend);
#endif
        return false;
    }

    encodeReader.reset(encodedBitsToSend);

    uint8_t crc = calculateCRC(encodedBits, encodedBitsToSend);

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
    for (uint16_t bitPos = 0; bitPos < encodedBitsToSend; bitPos++)
    {
        if (bitPos & 1)
        {
            sendSpace(encodeReader.readBit());
        }
        else
        {
            sendMark(encodeReader.readBit());
        }
    }

    // if sending an odd number of bits, add an extra mark to ensure all bits are sent
    bool sendPadding = (encodedBitsToSend & 1) == 0;
    if (sendPadding)
    {
        mark(QIR_PULSE_PADDING);
    }

    // send the CRC
    sendSpace(crc & 0b1000);
    sendMark(crc & 0b0100);
    sendSpace(crc & 0b0010);
    sendMark(crc & 0b0001);

    space(QIR_LEAD_OUT);

#ifdef DEBUG_IR_TRANSMITTER
    Serial.print("Bits sent: ");
    Serial.println(encodedBitsToSend);
    printBinaryArray(encodedBits, QIR_BUFFER_SIZE);
    if (sendPadding)
    {
        Serial.println("Padding signal sent");
    }
#endif

    return true;
}

inline void Quest_IR_Transmitter::sendSpace(bool one)
{
    if (one)
    {
        space(QIR_PULSE_ONE);
    }
    else
    {
        space(QIR_PULSE_ZERO);
    }
}

inline void Quest_IR_Transmitter::sendMark(bool one)
{
    if (one)
    {
        mark(QIR_PULSE_ONE);
    }
    else
    {
        mark(QIR_PULSE_ZERO);
    }
}
