/* dump-receiver.cpp Example sketch for Quest IR Library
 * This example will receive IR signals from a pin and
 * log the mark and delay times to Serial.
 * 
 * Note: Update PIN_IR_RECEIVER to the pin connected to
 * an IR receiver such as the Vishay TSOP38238.
 */

#include <Quest_IR_Receiver.h>

// update this pin
const int PIN_IR_RECEIVER = 7;
Quest_IR_Receiver irReceiver;

void setup()
{
    Serial.begin(9600);

    // start to listen for IR signals on the given pin
    irReceiver.begin(PIN_IR_RECEIVER);
    irReceiver.enableBlink(true);
}

void loop()
{
    // check to see if the receiver decoded a signal
    if (irReceiver.hasData())
    {
        // data was received, dump it to Serial
        irReceiver.dumpData();

        // make sure the receiver is reset to decode the next signal
        irReceiver.reset();
    }
}