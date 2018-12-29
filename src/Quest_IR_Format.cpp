#include "Quest_IR_Format.h"

uint8_t calculateCRC(uint8_t *bitBuffers, uint16_t bits)
{
    // calculate the num of bytes that have values in the bit buffers
    uint8_t bytes = (bits >> 3) + 1;

    // seed the CRC value
    uint8_t crc = 0b01010010;

    // XOR each byte onto the CRC
    for (uint16_t i = 0; i < bytes; i++)
    {
        crc ^= bitBuffers[i];
    }

    // only use first 4 bits
    uint8_t highBits = crc >> 4;
    return (crc ^ highBits) & 0b00001111;
}
