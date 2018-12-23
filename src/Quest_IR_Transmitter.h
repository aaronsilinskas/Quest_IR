#ifndef quest_ir_transmitter_h
#define quest_ir_transmitter_h

#define IR_SEND_PWM_PIN 11

#include "irlib2/IRLibSendBase.h"
#include "irlib2/IRLib_HashRaw.h"

class Quest_IR_Transmitter
{
  public:
    // For now, IR send pin must be set via: #define IR_SEND_PWM_PIN 4
    void begin();
    void send(uint16_t *buffer, uint8_t length);

  private:
    IRsendRaw sender;
};

#endif