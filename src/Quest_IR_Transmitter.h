#ifndef quest_ir_transmitter_h
#define quest_ir_transmitter_h

#include "irlib2/IRLibSendBase.h"

class Quest_IR_Transmitter: public virtual IRsendBase
{
  public:
    void begin();    
    void sendRawSignal(uint16_t *buffer, uint8_t length);

  private:
    
};

#endif
