/* Quest_IR_Receiver.h Quest IR Receiver Library
 * Receives encoded data in the Quest format, but can also print raw marks
 * and spaces for debugging. See Quest_IR_Format for format notes.
 */
#ifndef quest_ir_receiver_h
#define quest_ir_receiver_h

#include "./irlib2/IRLibRecvPCI.h"
#include "Quest_IR_Format.h"
#include "Quest_IR_BitWriter.h"

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
  uint8_t decodedBits[QIR_BUFFER_SIZE];
  uint16_t decodedBitCount;

  void begin(uint8_t irPin);
  void enable();
  void disable();
  void enableBlink(bool enabled);
  bool hasSignal();
  void reset();
  void printRawSignal();

private:
  IRrecvPCI receiver;
  Quest_IR_BitWriter decodeWriter = Quest_IR_BitWriter(decodedBits, QIR_BUFFER_SIZE);

  void clearBuffer();
  void attemptDecode();
  bool verifyCRC();
  uint8_t decodeCRCSignals();
  bool checkSignal(uint16_t expected, uint16_t actual);
  bool signalToBit(uint16_t signal);
  void setInvalidPacketState(DecodeState state);
};

#endif
