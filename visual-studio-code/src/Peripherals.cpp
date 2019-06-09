#include <Arduino.h>

#include "Peripherals.h"

Peripherals::Peripherals()
    : m_input_status(0)
{ }

void Peripherals::init() {

// all button inputs have external 10k-ohm pulldown resistors
// pulling these inputs high will trigger the relays

    pinMode(m_button_1, INPUT);
    pinMode(m_button_2, INPUT);
    pinMode(m_button_3, INPUT);
    pinMode(m_button_4, INPUT);
    pinMode(m_button_5, INPUT);
    pinMode(m_button_6, INPUT);
    pinMode(m_button_7, INPUT);
    pinMode(m_button_8, INPUT);


    pinMode(m_relay_spot_lights, OUTPUT);
    pinMode(m_relay_bar_light, OUTPUT);
    pinMode(m_relay_under_table_sockets, OUTPUT);
    pinMode(m_relay_table_top_sockets, OUTPUT);
    pinMode(m_relay_12V, OUTPUT);
    pinMode(m_relay_18V, OUTPUT);
    pinMode(m_relay_24V, OUTPUT);
    pinMode(m_relay_USB, OUTPUT);

    set_relay_spot_lights(false);
    set_relay_bar_lights(false);
    set_relay_under_table_sockets(false);
    set_relay_table_top_sockets(false);
    set_relay_12V(false);
    set_relay_18V(false);
    set_relay_24V(false);
    set_relay_USB(false);
}

void Peripherals::process_inputs() {
    Peripherals::input_status_t input_status = get_input_status();

    if(input_status != m_input_status) {
        Peripherals::relay_status_t relay_status = get_relay_status();

        if(input_status & INPUT_MASK_1) {
            set_relay_spot_lights(!(relay_status & RELAY_MASK_1));
        }

        if(input_status & INPUT_MASK_2) {
            set_relay_bar_lights(!(relay_status & RELAY_MASK_2));
        }

        if(input_status & INPUT_MASK_3) {
            set_relay_under_table_sockets(!(relay_status & RELAY_MASK_3));
        }

        if(input_status & INPUT_MASK_4) {
            set_relay_table_top_sockets(!(relay_status & RELAY_MASK_4));
        }

        if(input_status & INPUT_MASK_5) {
            set_relay_12V(!(relay_status & RELAY_MASK_5));
        }

        if(input_status & INPUT_MASK_6) {
            set_relay_18V(!(relay_status & RELAY_MASK_6));
        }

        if(input_status & INPUT_MASK_7) {
            set_relay_24V(!(relay_status & RELAY_MASK_7));
        }

        if(input_status & INPUT_MASK_8) {
            set_relay_USB(!(relay_status & RELAY_MASK_8));
        }

        m_input_status = input_status;

        // debounce input
        if(input_status) {
            delay(200);
        }
    }
}

Peripherals::relay_status_t Peripherals::get_relay_status() {
    Peripherals::relay_status_t relay_status = 
          is_relay_activated(m_relay_spot_lights) << 0
        | is_relay_activated(m_relay_bar_light) << 1
        | is_relay_activated(m_relay_under_table_sockets) << 2
        | is_relay_activated(m_relay_table_top_sockets) << 3
        | is_relay_activated(m_relay_12V) << 4
        | is_relay_activated(m_relay_18V) << 5
        | is_relay_activated(m_relay_24V) << 6
        | is_relay_activated(m_relay_USB) << 7;

    return relay_status;
}

Peripherals::input_status_t Peripherals::get_input_status() {
    Peripherals::input_status_t input_status = 
          is_button_pressed(m_button_1) << 0
        | is_button_pressed(m_button_2) << 1
        | is_button_pressed(m_button_3) << 2
        | is_button_pressed(m_button_4) << 3
        | is_button_pressed(m_button_5) << 4
        | is_button_pressed(m_button_6) << 5
        | is_button_pressed(m_button_7) << 6
        | is_button_pressed(m_button_8) << 7;

    return input_status;
}

void Peripherals::set_relay_spot_lights(bool state) {
    set_relay(m_relay_spot_lights, state);
}

void Peripherals::set_relay_bar_lights(bool state) {
    set_relay(m_relay_bar_light, state);
}

void Peripherals::set_relay_under_table_sockets(bool state) {
    set_relay(m_relay_under_table_sockets, state);
}

void Peripherals::set_relay_table_top_sockets(bool state) {
    set_relay(m_relay_table_top_sockets, state);
}

void Peripherals::set_relay_12V(bool state) {
    set_relay(m_relay_12V, state);
}

void Peripherals::set_relay_18V(bool state) {
    set_relay(m_relay_18V, state);
}

void Peripherals::set_relay_24V(bool state) {
    set_relay(m_relay_24V, state);
}

void Peripherals::set_relay_USB(bool state) {
    set_relay(m_relay_USB, state);
}

bool Peripherals::is_button_pressed(uint8_t button) {
    return (digitalRead(button) == BUTTON_PRESSED);
}

bool Peripherals::is_relay_activated(uint8_t relay) {
    return (digitalRead(relay) == RELAY_STATE_ACTIVATED);
}

void Peripherals::set_relay(uint8_t relay, bool state) {
    digitalWrite(relay, state ? RELAY_STATE_ACTIVATED : RELAY_STATE_DEACTIVATED);
}
