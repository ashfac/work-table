#ifndef Protocol_h
#define Protocol_h

#include <Arduino.h>

class Protocol {
    public:
        typedef uint8_t som_eom_t;
        typedef uint8_t tag_t;
        typedef uint8_t length_t;
        typedef uint8_t checksum_t;

        static const size_t MIN_PACKET_SIZE = 5U;

        //-- SOF TAG  LENGTH  VALUE CHECKSUM EOF --//
        //--  1   1      1     LEN    1       1  --//
        //--  (above numbers are sizes in bytes) --//

        // SOF & EOF
        static const som_eom_t START_OF_MESSAGE = 0x55U;
        static const som_eom_t END_OF_MESSAGE = 0x99U;

        static const uint8_t STUFFED_BYTE = 0x80U;

        // request messages
        static const tag_t TAG_GET_RELAY_STATUS = 0x00U;

        static const tag_t TAG_ACTIVATE_RELAY = 0x01U;
        static const tag_t TAG_DEACTIVATE_RELAY = 0x02U;

        static const tag_t TAG_ACTIVATE_ALL_RELAYS = 0x11U;
        static const tag_t TAG_DEACTIVATE_ALL_RELAYS = 0x12U;

        // response messages
        static const tag_t TAG_RES_RELAY_STATUS = 0xF0U;
        static const tag_t TAG_RES_ACKNOWLEDGEMENT = 0xF1U;

        // TAG_ACTIVATE_RELAY + TAG_DEACTIVATE_RELAY SUB-TAGS

        static const uint8_t RELAY_SPOT_LIGHTS = 0x01U;
        static const uint8_t RELAY_BAR_LIGHT = 0x02U;
        static const uint8_t RELAY_UNDER_TABLE_SOCKETS = 0x03U;
        static const uint8_t RELAY_TABLE_TOP_SOCKETS = 0x04U;
        static const uint8_t RELAY_12V = 0x05U;
        static const uint8_t RELAY_18V = 0x06U;
        static const uint8_t RELAY_24V = 0x07U;
        static const uint8_t RELAY_USB = 0x08U;

        static const uint8_t RELAY_BEGIN = RELAY_SPOT_LIGHTS;
        static const uint8_t RELAY_END = RELAY_USB;

        static const uint8_t RES_ACK_OK = 0x00U;
        static const uint8_t RES_ACK_CHECKSUM_FAILED = 0x01U;
        static const uint8_t RES_ACK_MSG_TOO_LONG = 0x02U;
        static const uint8_t RES_ACK_MSG_PARSING_ERROR = 0x03U;

        static const size_t TAG_OFFSET = sizeof(som_eom_t);
        static const size_t LENGTH_OFFSET = TAG_OFFSET + sizeof(tag_t);
        static const size_t VALUE_OFFSET = LENGTH_OFFSET + sizeof(length_t);

        static const size_t PROTOCOL_OVERHEAD = 
            sizeof(som_eom_t) + sizeof(tag_t) + sizeof(length_t) + sizeof(checksum_t) + sizeof(som_eom_t);

        static bool is_flag_byte(uint8_t dt) {
            return (dt == START_OF_MESSAGE || dt == END_OF_MESSAGE || dt == STUFFED_BYTE);
        }

};

#endif
