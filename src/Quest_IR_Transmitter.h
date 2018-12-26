#ifndef quest_ir_transmitter_h
#define quest_ir_transmitter_h

#define IR_SEND_PWM_PIN 11

#include "irlib2/IRLibSendBase.h"
#include "irlib2/IRLib_HashRaw.h"

class Quest_IR_Transmitter
{
  public:
    void begin();    
    void sendRawSignal(uint16_t *buffer, uint8_t length);

  private:
    IRsendRaw sender;
};

#endif