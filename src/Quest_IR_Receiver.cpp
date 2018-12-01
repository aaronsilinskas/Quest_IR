#include "Quest_IR_Receiver.h"

void Quest_IR_Receiver::begin(uint8_t irPin)
{
    receiver = IRrecvPCI(irPin);
    receiver.enableIRIn();    
}

void Quest_IR_Receiver::enableBlink(bool enabled) {
    receiver.blink13(enabled);
}

bool Quest_IR_Receiver::hasData()
{
    return receiver.getResults();
}

void Quest_IR_Receiver::reset()
{
    receiver.enableIRIn();
}

bool Quest_IR_Receiver::decodeData()
{
    if (!checkBuffer(1, 2600) || !checkBuffer(2, 500))
    {
        return false;
    }

    //   uint8_t data[MAX_IR_DATA_LENGTH];
    //   uint8_t dataLength;

    // int bitCount = 0;
    // int currentBits = 0;
    // for (bufIndex_t i = 3; i < recvGlobal.recvLength; i += 2)
    // {
    //     int mark = recvGlobal.recvBuffer[i];
    //     int bitDelay = recvGlobal.recvBuffer[i + 1];

    //     Serial.print(F(" Mark "));
    //     Serial.print(mark, DEC);
    //     Serial.print(F(" Delay "));
    //     Serial.println(bitDelay, DEC);

    //     currentBits = currentBits << 1;
    //     if (bitDelay > 900)
    //     {
    //         currentBits |= 1;
    //     }
    //     bitCount++;
    //     if (bitCount == 8)
    //     {
    //         Serial.print(F("HEX CHAR = "));
    //         Serial.println(currentBits, HEX);
    //         bitCount = 0;
    //         currentBits = 0;
    //     }
    // }
    // Serial.println();

    return false;
}

void Quest_IR_Receiver::dumpData()
{
    Serial.print(F("Header: "));
    Serial.print(recvGlobal.recvBuffer[1], DEC);
    Serial.print(F(", "));
    Serial.println(recvGlobal.recvBuffer[2], DEC);

    for (bufIndex_t i = 3; i < recvGlobal.recvLength; i += 2)
    {
        int mark = recvGlobal.recvBuffer[i];
        int bitDelay = recvGlobal.recvBuffer[i + 1];

        Serial.print(F(" Mark "));
        Serial.print(mark, DEC);
        Serial.print(F(" Delay "));
        Serial.println(bitDelay, DEC);
    }
    Serial.println();
}

bool Quest_IR_Receiver::checkBuffer(bufIndex_t index, uint16_t expected)
{
    if (index >= recvGlobal.recvLength)
    {
        return false;
    }
    uint16_t actual = recvGlobal.recvBuffer[index];
    return abs(actual - expected) < 100;
}