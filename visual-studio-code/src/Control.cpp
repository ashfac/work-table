
#include "Control.h"

Control::Control(unsigned long baudrate, unsigned long timeout, size_t buffer_size)
    : m_comms(baudrate, timeout, buffer_size)
{ }

void Control::init() {
    m_peripherals.init();
    m_comms.init();
}

void Control::step() {
    if(m_comms.message_received()) {
        process_message(m_comms.get_tag(), m_comms.get_length(), m_comms.get_value());
        m_comms.message_processed();
    } else {
        process_inputs();
    }
}

void Control::process_message(Protocol::tag_t tag, Protocol::length_t length, uint8_t* value) {
    switch (tag) {
        case Protocol::TAG_GET_RELAY_STATUS: on_get_relay_status(); break;
        case Protocol::TAG_ACTIVATE_RELAY: on_activate_relay(*value); break;
        case Protocol::TAG_DEACTIVATE_RELAY: on_deactivate_relay(*value); break;
        case Protocol::TAG_ACTIVATE_ALL_RELAYS: on_activate_all_relays(); break;
        case Protocol::TAG_DEACTIVATE_ALL_RELAYS: on_deactivate_all_relays(); break;
        default: on_unrecognized_tag(); break;
    }
}

void Control::process_inputs() {
    m_peripherals.process_inputs();
}

void Control::on_get_relay_status() {
    m_comms.send_acknowledgement(Protocol::RES_ACK_OK);
    send_relay_status();
}

void Control::on_activate_relay(uint8_t relay) {
    set_relay_state(relay, true);
    m_comms.send_acknowledgement(Protocol::RES_ACK_OK);
}

void Control::on_deactivate_relay(uint8_t relay) {
    set_relay_state(relay, false);
    m_comms.send_acknowledgement(Protocol::RES_ACK_OK);
}

void Control::on_activate_all_relays() {
    for(uint8_t relay = Protocol::RELAY_BEGIN; relay <= Protocol::RELAY_END; relay++) {
        set_relay_state(relay, true);
    }
    m_comms.send_acknowledgement(Protocol::RES_ACK_OK);
}

void Control::on_deactivate_all_relays() {
    for(uint8_t relay = Protocol::RELAY_BEGIN; relay <= Protocol::RELAY_END; relay++) {
        set_relay_state(relay, false);
    }
    m_comms.send_acknowledgement(Protocol::RES_ACK_OK);
}

void Control::on_unrecognized_tag() {
    m_comms.send_acknowledgement(Protocol::RES_ACK_MSG_PARSING_ERROR);
}

void Control::set_relay_state(uint8_t relay, bool state) {
    switch (relay) {
        case Protocol::RELAY_SPOT_LIGHTS: m_peripherals.set_relay_spot_lights(state); break;
        case Protocol::RELAY_BAR_LIGHT: m_peripherals.set_relay_bar_lights(state); break;
        case Protocol::RELAY_UNDER_TABLE_SOCKETS: m_peripherals.set_relay_under_table_sockets(state); break;
        case Protocol::RELAY_TABLE_TOP_SOCKETS: m_peripherals.set_relay_table_top_sockets(state); break;
        case Protocol::RELAY_12V: m_peripherals.set_relay_12V(state); break;
        case Protocol::RELAY_18V: m_peripherals.set_relay_18V(state); break;
        case Protocol::RELAY_24V: m_peripherals.set_relay_24V(state); break;
        case Protocol::RELAY_USB: m_peripherals.set_relay_USB(state); break;
        default : break;
    }
}

void Control::send_relay_status() {
    uint8_t relay_status =  m_peripherals.get_relay_status();
    m_comms.send_message(Protocol::TAG_RES_RELAY_STATUS, 1, &relay_status);

}
