/* Quest_IR_Receiver.h Quest IR Receiver Library
 * Receives encoded data in the Quest format, but can also print raw marks
 * and spaces for debugging. See Quest_IR_Format for format notes.
 */
#ifndef quest_ir_receiver_h
#define quest_ir_receiver_h

#include "./irlib2/IRLibRecvPCI.h"
#include "Quest_IR_Format.h"

// #define DEBUG_IR_RECEIVER

enum DecodeState
{
  NoData,         // Ready for data
  SignalTooShort, // Not enough data received to be a valid QIR packet
  HeaderMismatch, // Header mark and space did not match QIR headers, packet ignored
  InvalidPulse,   // Invalid pulse length in signal, data likely corrupt and packet ignored
  InvalidCRC,     // The CRC calculated from decoded bits and the packet's CRC do not match, data corrupted and packet ignored
  Decoded         // The packet was decoded successfully
};

class Quest_IR_Receiver
{
public:
  DecodeState decodeState;
  uint16_t bitsReceived;

  void begin(uint8_t irPin);
  void enable();
  void disable();
  void enableBlink(bool enabled);
  bool hasData();
  uint16_t unreadBits();
  uint32_t readBits(uint8_t bitsToRead);
  void reset();
  void printRawSignal();

private:
  IRrecvPCI receiver;
  uint16_t nextBitPosition;
  uint8_t bitBuffers[QIR_BIT_BUFFERS];

  void clearBuffer();
  void attemptDecode();
  bool verifyCRC();
  uint8_t decodeCRCSignals();
  bool checkSignal(uint16_t expected, uint16_t actual);
  bool signalToBit(uint16_t signal);
  void setInvalidPacketState(DecodeState state);
};

#endif
