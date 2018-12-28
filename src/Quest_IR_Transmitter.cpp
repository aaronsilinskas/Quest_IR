#include "Quest_IR_Transmitter.h"

void Quest_IR_Transmitter::begin()
{
}

void Quest_IR_Transmitter::sendRawSignal(uint16_t *buffer, uint8_t length)
{
    enableIROut(38);

    for (int i = 0; i < length; i++) {
        if (i & 1) {
            space(buffer[i]);
        } else {
            mark(buffer[i]);
        }
    }
}