#ifndef Comms_h
#define Comms_h

#include <Arduino.h>

#include "Protocol.h"

class Comms {
    enum State { Waiting, Receiving, Destuffing, Done };

    public:
        Comms(unsigned long baudrate, unsigned long timeout, size_t buffer_size);
        ~Comms();

        void init();

        bool message_received();
        void message_processed();

        void send_acknowledgement(const uint8_t ack);
        void send_message(const Protocol::tag_t tag, const Protocol::length_t length, const uint8_t* value);

        Protocol::tag_t get_tag();
        Protocol::length_t get_length();
        uint8_t* get_value();

    protected:
        Protocol::checksum_t calculate_checksum(uint8_t* buffer, size_t length);

        void write_bytes(uint8_t *buffer, size_t length);
        bool read_bytes(uint8_t *buffer, size_t length);

        State m_state;

        size_t m_buffer_size;
        size_t m_recv_bytes;

        uint8_t* m_recv_buffer;
        uint8_t* m_send_buffer;

        unsigned long m_baudrate;
        unsigned long m_timeout;
};

#endif
