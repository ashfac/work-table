package com.example.engra.worktable;

import java.io.ByteArrayOutputStream;

public class Protocol {
    //-- SOF TAG  LENGTH  VALUE CHECKSUM EOF --//
    //--  1   1      1     LEN    1       1  --//
    //--  (above numbers are sizes in bytes) --//

    // SOF & EOF
    public static final byte START_OF_MESSAGE = BYTE(0x055);
    public static final byte END_OF_MESSAGE = BYTE(0x99);

    public static final byte STUFFED_BYTE = BYTE(0x80);

    // request messages
    public static final byte TAG_GET_RELAY_STATUS = BYTE(0x00);

    public static final byte TAG_ACTIVATE_RELAY = BYTE(0x01);
    public static final byte TAG_DEACTIVATE_RELAY = BYTE(0x02);

    public static final byte TAG_ACTIVATE_ALL_RELAYS = BYTE(0x11);
    public static final byte TAG_DEACTIVATE_ALL_RELAYS = BYTE(0x12);

    public static final byte TAG_UNKNOWN = BYTE(0xFF);

    // response messages
    public static final byte TAG_RES_RELAY_STATUS = BYTE(0xF0);
    public static final byte TAG_RES_ACKNOWLEDGEMENT = BYTE(0xF1);

    // TAG_ACTIVATE_RELAY + TAG_DEACTIVATE_RELAY SUB-TAGS

    public static final byte RELAY_SPOT_LIGHTS = BYTE(0x01);
    public static final byte RELAY_BAR_LIGHT = BYTE(0x02);
    public static final byte RELAY_UNDER_TABLE_SOCKETS = BYTE(0x03);
    public static final byte RELAY_TABLE_TOP_SOCKETS = BYTE(0x04);
    public static final byte RELAY_12V = BYTE(0x05);
    public static final byte RELAY_18V = BYTE(0x06);
    public static final byte RELAY_24V = BYTE(0x07);
    public static final byte RELAY_USB = BYTE(0x08);

    public static final byte RELAY_BEGIN = RELAY_SPOT_LIGHTS;
    public static final byte RELAY_END = RELAY_USB;

    public static final byte RES_ACK_OK = BYTE(0x00);
    public static final byte RES_ACK_CHECKSUM_FAILED = BYTE(0x01);
    public static final byte RES_ACK_MSG_TOO_LONG = BYTE(0x02);
    public static final byte RES_ACK_MSG_PARSING_ERROR = BYTE(0x03);

    public static final int TAG_OFFSET = 1;
    public static final int LENGTH_OFFSET = TAG_OFFSET + 1;
    public static final int VALUE_OFFSET = LENGTH_OFFSET + 1;

    public static final int PROTOCOL_OVERHEAD = 5;

    public static final int MASK_RELAY_SPOT_LIGHTS = (1 << 0);
    public static final int MASK_RELAY_BAR_LIGHT = (1 << 1);
    public static final int MASK_RELAY_UNDER_TABLE_SOCKETS = (1 << 2);
    public static final int MASK_RELAY_TABLE_TOP_SOCKETS = (1 << 3);
    public static final int MASK_RELAY_12V = (1 << 4);
    public static final int MASK_RELAY_18V = (1 << 5);
    public static final int MASK_RELAY_24V = (1 << 6);
    public static final int MASK_RELAY_USB = (1 << 7);

    public static byte[] composeMessage(byte tag) {
        byte[] value = {};
        return composeMessage(tag, value);
    }

    public static byte[] composeMessage(byte tag, byte[] value) {
        ByteArrayOutputStream bMessage = new ByteArrayOutputStream(value.length + PROTOCOL_OVERHEAD);

        bMessage.write(START_OF_MESSAGE);
        bMessage.write(tag);
        bMessage.write(value.length);

        bMessage.write(value, 0, value.length);

        bMessage.write(calculateChecksum(bMessage.toByteArray()));

        bMessage.write(END_OF_MESSAGE);

        byte[] message = bMessage.toByteArray();

        return message;
    }

    private static byte calculateChecksum(byte[] message) {
        int checksum = 0;

        for(int i = 0 ; i < message.length; i++) {
            checksum += message[i];
        }

        return (byte)(((~checksum) + 1) & 0xFF);
    }

    private static byte BYTE(int in) {
        return ((byte)in);
    }

    private static boolean isFlagByte(byte dt) {
        return (dt == START_OF_MESSAGE || dt == END_OF_MESSAGE || dt == STUFFED_BYTE);
    }
}
