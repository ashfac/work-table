#include <Arduino.h>
#include <HardwareSerial.h>

#include "Control.h"

const unsigned long BAUD_RATE = 57600; // baud rate for serial port
const unsigned long COMMS_TIMEOUT = 1000; // timeout in milli-seconds for reading from serial port
const size_t BUFFER_SIZE = 64; // buffer size for message

Control control(BAUD_RATE, COMMS_TIMEOUT, BUFFER_SIZE);

void setup() {
    control.init();
}

void loop() {
    control.step();
}
