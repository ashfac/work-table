#ifndef Control_h
#define Control_h

#include <Arduino.h>

#include "Protocol.h"
#include "Peripherals.h"
#include "Comms.h"

class Control {
    public:
        Control(unsigned long baudrate, unsigned long timeout, size_t buffer_size);

        void init();
        void step();

    protected:
        void process_message(Protocol::tag_t tag, Protocol::length_t length, uint8_t* value);
        void process_inputs();

        void on_get_relay_status();

        void on_activate_relay(uint8_t relay);
        void on_deactivate_relay(uint8_t relay);

        void on_activate_all_relays();
        void on_deactivate_all_relays();

        void on_unrecognized_tag();

        void set_relay_state(uint8_t relay, bool state);

        void send_relay_status();

        Peripherals m_peripherals;
        Comms m_comms;
};

#endif
