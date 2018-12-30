/* Quest_IR_Transmitter.h Quest IR Transmitter Library
 * Sends encoded data in the Quest format, but can also send raw marks
 * and spaces for custom signals. See Quest_IR_Format for format notes.
 */
#ifndef quest_ir_transmitter_h
#define quest_ir_transmitter_h

#include "irlib2/IRLibSendBase.h"
#include "Quest_IR_Format.h"
#include "Quest_IR_BitReader.h"

// #define DEBUG_IR_TRANSMITTER

class Quest_IR_Transmitter : public virtual IRsendBase
{
public:
  uint8_t encodedBits[QIR_BUFFER_SIZE];

  void begin();
  void sendRawSignal(uint16_t *buffer, uint8_t length);

  bool sendBits(uint16_t encodedBitsToSend);

private:
  Quest_IR_BitReader encodeReader = Quest_IR_BitReader(encodedBits, QIR_BUFFER_SIZE);
  void sendSpace(bool one);
  void sendMark(bool one);
};

#endif
