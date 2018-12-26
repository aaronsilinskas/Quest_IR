#include "Quest_IR_Transmitter.h"

void Quest_IR_Transmitter::begin()
{
}

void Quest_IR_Transmitter::sendRawSignal(uint16_t *buffer, uint8_t length)
{
    sender.send(buffer, length, 38);
}