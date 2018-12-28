#ifndef quest_ir_receiver_h
#define quest_ir_receiver_h

#include "./irlib2/IRLibAll.h"

#define MAX_IR_DATA_LENGTH 48

class Quest_IR_Receiver
{
public:
  uint8_t data[MAX_IR_DATA_LENGTH];
  uint8_t dataLength;

  void begin(uint8_t irPin);
  void enableBlink(bool enabled);
  bool hasData();
  bool decodeData();
  void reset();
  void printRawSignal();

private:
  IRrecvPCI receiver;
  bool checkBuffer(bufIndex_t index, uint16_t expected);
};

#endif