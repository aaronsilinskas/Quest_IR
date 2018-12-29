/* Quest_IR_Format.h Quest IR Format definitions
 * These values are used by the receiver and transmitter Quest IR libraries.
 * 
 * Encoding Notes:
 * Each IR signal contains a header mark and space, followed by the encoded
 * data bits. Many IR protocols only encode bits in either the mark or the 
 * space, but the Quest IR format encodes bits in pulse width of both the
 * mark and space, halving the transmission size.
 * If the number of bits sent is even, an addition padding mark will be sent
 * for IR signal compatibility that will be ignored by the receiver.
 * Pulse timing is not 100% accurate, so if a pulse is less than the expected
 * value + QIR_ERROR_MARGIN, then it will still count as that value. For 
 * instance, a pulse of 680 is less than QIR_PULSE_ZERO + QIR_PULSE_MARGIN so
 * will count as a zero bit.
 */
#ifndef quest_ir_format_h
#define quest_ir_format_h

#include <Arduino.h>

#define QIR_PULSE 500
#define QIR_ERROR_MARGIN 250

#define QIR_HEADER_MARK QIR_PULSE * 8
#define QIR_HEADER_SPACE QIR_PULSE * 6

#define QIR_PULSE_ZERO QIR_PULSE
#define QIR_PULSE_ONE QIR_PULSE * 3
#define QIR_PULSE_PADDING QIR_PULSE * 5
#define QIR_LEAD_OUT QIR_PULSE * 10

#define QIR_MAX_BITS 256
#define QIR_BIT_BUFFERS QIR_MAX_BITS / 8
#define QIR_TOP_BIT 0x80000000

uint8_t calculateCRC(uint8_t *bitBuffers, uint16_t bits);

#endif
