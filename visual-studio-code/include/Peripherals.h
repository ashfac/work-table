#ifndef Io_h
#define Io_h

#include <Arduino.h>

class Peripherals {
    public:
        typedef uint8_t input_status_t;
        typedef uint8_t relay_status_t;

        Peripherals();
        
        void init();
        void process_inputs();

        relay_status_t get_relay_status();
        input_status_t get_input_status();

        void set_relay_spot_lights(bool state);
        void set_relay_bar_lights(bool state);
        void set_relay_under_table_sockets(bool state);
        void set_relay_table_top_sockets(bool state);
        void set_relay_12V(bool state);
        void set_relay_18V(bool state);
        void set_relay_24V(bool state);
        void set_relay_USB(bool state);

    protected:
        static const uint8_t m_button_1 = 10;
        static const uint8_t m_button_2 = 11;
        static const uint8_t m_button_3 = 12;
        static const uint8_t m_button_4 = 13;
        static const uint8_t m_button_5 = A0;
        static const uint8_t m_button_6 = A1;
        static const uint8_t m_button_7 = A2;
        static const uint8_t m_button_8 = A3;

        static const uint8_t m_relay_spot_lights = 2;
        static const uint8_t m_relay_bar_light = 3;
        static const uint8_t m_relay_under_table_sockets = 4;
        static const uint8_t m_relay_table_top_sockets = 5;
        static const uint8_t m_relay_12V = 6;
        static const uint8_t m_relay_18V = 7;
        static const uint8_t m_relay_24V = 8;
        static const uint8_t m_relay_USB = 9;

        static const uint8_t RELAY_STATE_ACTIVATED = LOW;
        static const uint8_t RELAY_STATE_DEACTIVATED = HIGH;

        static const uint8_t BUTTON_PRESSED = HIGH;
        static const uint8_t BUTTON_RELEASED = LOW;

        static const uint8_t INPUT_MASK_1 = 1 << 0;
        static const uint8_t INPUT_MASK_2 = 1 << 1;
        static const uint8_t INPUT_MASK_3 = 1 << 2;
        static const uint8_t INPUT_MASK_4 = 1 << 3;
        static const uint8_t INPUT_MASK_5 = 1 << 4;
        static const uint8_t INPUT_MASK_6 = 1 << 5;
        static const uint8_t INPUT_MASK_7 = 1 << 6;
        static const uint8_t INPUT_MASK_8 = 1 << 7;

        static const uint8_t RELAY_MASK_1 = 1 << 0;
        static const uint8_t RELAY_MASK_2 = 1 << 1;
        static const uint8_t RELAY_MASK_3 = 1 << 2;
        static const uint8_t RELAY_MASK_4 = 1 << 3;
        static const uint8_t RELAY_MASK_5 = 1 << 4;
        static const uint8_t RELAY_MASK_6 = 1 << 5;
        static const uint8_t RELAY_MASK_7 = 1 << 6;
        static const uint8_t RELAY_MASK_8 = 1 << 7;

        bool is_button_pressed(uint8_t button);
        bool is_relay_activated(uint8_t relay);

        void set_relay(uint8_t relay, bool state);

        Peripherals::input_status_t m_input_status;
};

#endif
