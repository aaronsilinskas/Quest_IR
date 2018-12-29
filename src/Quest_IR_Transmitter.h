/* Quest_IR_Transmitter.h Quest IR Transmitter Library
 * Sends encoded data in the Quest format, but can also send raw marks
 * and spaces for custom signals. See Quest_IR_Format for format notes.
 */
#ifndef quest_ir_transmitter_h
#define quest_ir_transmitter_h

#include "irlib2/IRLibSendBase.h"
#include "Quest_IR_Format.h"

// #define DEBUG_IR_TRANSMITTER

class Quest_IR_Transmitter : public virtual IRsendBase
{
public:
  void begin();
  void sendRawSignal(uint16_t *buffer, uint8_t length);

  void clearBits();
  bool addBits(uint32_t data, uint8_t bitsToSend);
  bool sendBits();

private:
  uint16_t nextBitPosition;
  uint8_t bitBuffers[QIR_BIT_BUFFERS];

  void sendSpace(bool one);
  void sendMark(bool one);
};

#endif
