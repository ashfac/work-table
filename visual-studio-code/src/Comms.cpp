
#include "Comms.h"

Comms::Comms(unsigned long baudrate, unsigned long timeout, size_t buffer_size)
    : m_state(State::Waiting)
    , m_buffer_size(buffer_size)
    , m_recv_bytes(0)
    , m_recv_buffer(new uint8_t[buffer_size])
    , m_send_buffer(new uint8_t[buffer_size])
    , m_baudrate(baudrate)
    , m_timeout(timeout)
{ }

Comms::~Comms() {
    delete[] m_recv_buffer;
    delete[] m_send_buffer;
}

void Comms::init() {
    Serial.begin(m_baudrate);
    Serial.setTimeout(m_timeout);
}

bool Comms::message_received() {
    while(Serial.available()) {
        switch(m_state) {
            case State::Waiting: {
                Serial.readBytes(m_recv_buffer, 1);
                if(*m_recv_buffer == Protocol::START_OF_MESSAGE) {
                    m_recv_bytes = 1;
                    m_state = State::Receiving;
                }
                break;
            }
            
            case State::Receiving: {
                uint8_t recvd_byte;
                Serial.readBytes(&recvd_byte, 1);

                switch(recvd_byte) {
                    case Protocol::START_OF_MESSAGE: {
                        send_acknowledgement(Protocol::RES_ACK_MSG_PARSING_ERROR);
                        m_recv_bytes = 1;
                        break;
                    }

                    case Protocol::END_OF_MESSAGE: {
                        if (m_recv_bytes != (get_length() + Protocol::PROTOCOL_OVERHEAD - 1) ) {
                            send_acknowledgement(Protocol::RES_ACK_MSG_PARSING_ERROR);
                            m_recv_bytes = 0;
                            m_state = State::Waiting;
                        } else if(calculate_checksum(m_recv_buffer, m_recv_bytes)) {
                            send_acknowledgement(Protocol::RES_ACK_CHECKSUM_FAILED);
                            m_recv_bytes = 0;
                            m_state = State::Waiting;
                        } else {
                            *(m_recv_buffer + m_recv_bytes++) = recvd_byte;
                            m_state = State::Done;
                        }
                        break;
                    }

                    case Protocol::STUFFED_BYTE: {
                        m_state = State::Destuffing;
                        break;
                    }

                    default: {
                        if((m_recv_bytes+1) < m_buffer_size) {
                            *(m_recv_buffer + m_recv_bytes) = recvd_byte;
                            m_recv_bytes++;
                        } else {
                            // buffer overflow
                            send_acknowledgement(Protocol::RES_ACK_MSG_TOO_LONG);
                            m_recv_bytes = 0;
                            m_state = State::Waiting;
                        }
                    }
                }
                break;
            }

            case State::Destuffing : {
                if((m_recv_bytes+1) < m_buffer_size) {
                    uint8_t recvd_byte;
                    Serial.readBytes(&recvd_byte, 1);
                    *(m_recv_buffer + m_recv_bytes) = Protocol::STUFFED_BYTE ^ recvd_byte;
                    m_recv_bytes++;
                } else {
                    // buffer overflow
                    send_acknowledgement(Protocol::RES_ACK_MSG_TOO_LONG);
                    m_recv_bytes = 0;
                    m_state = State::Waiting;
                }
                m_state = State::Receiving;
                break;
            }

            case State::Done: {
                break;
            }

            default: {
                break;
            }
        }
    }

    return (m_state == State::Done);
}

void Comms::send_message(const Protocol::tag_t tag, const Protocol::length_t length, const uint8_t* value) {
    if((length + Protocol::PROTOCOL_OVERHEAD) <= m_buffer_size) {
        uint8_t send_bytes = 0;
        memset(m_send_buffer, 0, m_buffer_size);
        
        memcpy(m_send_buffer, &Protocol::START_OF_MESSAGE, sizeof(Protocol::som_eom_t));
        send_bytes += sizeof(Protocol::som_eom_t);
        
        memcpy(m_send_buffer + send_bytes, &tag, sizeof(Protocol::tag_t));
        send_bytes += sizeof(Protocol::tag_t);

        memcpy(m_send_buffer + send_bytes, &length, sizeof(Protocol::length_t));
        send_bytes += sizeof(Protocol::length_t);

        memcpy(m_send_buffer + send_bytes, value, length);
        send_bytes += length;

        const Protocol::checksum_t checksum = calculate_checksum(m_send_buffer, send_bytes);
        memcpy(m_send_buffer + send_bytes, &checksum, sizeof(Protocol::checksum_t));
        send_bytes += sizeof(Protocol::checksum_t);

        memcpy(m_send_buffer + send_bytes, &Protocol::END_OF_MESSAGE, sizeof(Protocol::som_eom_t));
        send_bytes += sizeof(Protocol::som_eom_t);

        write_bytes(m_send_buffer, send_bytes);
    }
}

void Comms::message_processed() {
    m_recv_bytes = 0;
    m_state = State::Waiting;
}

Protocol::tag_t Comms::get_tag() {
    Protocol::tag_t tag;
    memset(&tag, 0, sizeof(Protocol::tag_t));
    memcpy(&tag, m_recv_buffer + Protocol::TAG_OFFSET, sizeof(Protocol::tag_t));
    return tag;
}

Protocol::length_t Comms::get_length() {
    Protocol::length_t length;
    memset(&length, 0, sizeof(Protocol::length_t));
    memcpy(&length, m_recv_buffer + Protocol::LENGTH_OFFSET, sizeof(Protocol::length_t));
    return length;
}

uint8_t* Comms::get_value() {
    return (m_recv_buffer + Protocol::VALUE_OFFSET);
}

void Comms::send_acknowledgement(const uint8_t ack) {
    send_message(Protocol::TAG_RES_ACKNOWLEDGEMENT, 1, &ack);
}

Protocol::checksum_t Comms::calculate_checksum(uint8_t* buffer, size_t length) {
    // START_OF_MESSAGE is included in checksum while
    // END_OF_MESSAGE is excluded
    unsigned int checksum;

    for(checksum = 0 ; length != 0; length--) {
        checksum += *(buffer++);
    }

    return static_cast<Protocol::checksum_t>((~checksum) + 1);
}

void Comms::write_bytes(uint8_t *buffer, size_t length) {
    
    // write start of message
    Serial.write(buffer++, 1);

    while((--length) != 1) {
        if(Protocol::is_flag_byte(*buffer)) {
            uint8_t stuffed_byte = Protocol::STUFFED_BYTE;
            Serial.write(&stuffed_byte, 1);
            *buffer = stuffed_byte ^ *buffer; 
        }

        Serial.write(buffer++, 1);
    }

    // write end of message
    Serial.write(buffer, 1);

    Serial.flush();
}

bool Comms::read_bytes(uint8_t *buffer, size_t length) {
    if(Serial.available()) {
        if(Serial.readBytes(buffer, length) == length) {
            return true;
        }
    }

    return false;
}
