/* Quest_IR_Receiver.h Quest IR Receiver Library
 * Receives encoded data in the Quest format, but can also print raw marks
 * and spaces for debugging. See Quest_IR_Format for format notes.
 */
#ifndef quest_ir_receiver_h
#define quest_ir_receiver_h

#include "./irlib2/IRLibRecvPCI.h"
#include "Quest_IR_Format.h"

// #define DEBUG_IR_RECEIVER

class Quest_IR_Receiver
{
public:
  uint16_t bitsReceived;

  void begin(uint8_t irPin);
  void enableBlink(bool enabled);
  bool hasData();
  uint32_t readBits(uint8_t bitsToRead);
  void reset();
  void printRawSignal();

private:
  IRrecvPCI receiver;
  uint16_t nextBitPosition;
  uint8_t bitBuffers[QIR_BIT_BUFFERS];

  void clearBuffer();
  bool checkBuffer(uint16_t expected, uint16_t actual);
  void attemptDecode();
};

#endif