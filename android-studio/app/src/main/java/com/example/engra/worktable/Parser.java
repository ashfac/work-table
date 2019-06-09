package com.example.engra.worktable;

import android.support.annotation.Nullable;

import java.util.ArrayList;

public class Parser {
    private enum State {Waiting, SomReceived, TagReceived, Receiving, Destuffing};

    byte tag;
    byte length;
    ArrayList<Byte> value;

    State state = State.Waiting;

    @Nullable
    private SendAcknowledgementListener sendAcknowledgementListener;

    @Nullable
    private MessageReceivedListener messageReceivedListener;

    public Parser() {
        tag = Protocol.TAG_UNKNOWN;
        length = 0;
        value = new ArrayList<Byte>();
    }

    public void setListeners(@Nullable MessageReceivedListener messageReceivedListener,
                             @Nullable SendAcknowledgementListener sendAcknowledgementListener) {
        this.messageReceivedListener = messageReceivedListener;
        this.sendAcknowledgementListener = sendAcknowledgementListener;
    }

    public void parse(byte[] bytes) {
        bytes = removeStuffedBytes(bytes);

        for (int i=0; i<bytes.length; i++) {
            byte b = bytes[i];
            switch (state) {
                case Waiting: {
                    if(b == Protocol.START_OF_MESSAGE) {
                        state = State.SomReceived;
                    }
                    break;
                }

                case SomReceived: {
                    tag = b;
                    state = State.TagReceived;
                    break;
                }
                
                case TagReceived: {
                    length = b;
                    value.clear();
                    state = State.Receiving;
                    break;
                }

                case Receiving: {
                    if(b == Protocol.START_OF_MESSAGE) {
                        sendAcknowledgement(Protocol.RES_ACK_MSG_PARSING_ERROR);
                    } else if(b == Protocol.END_OF_MESSAGE) {
                        if (value.size() != (length + 1) ) {
                            sendAcknowledgement(Protocol.RES_ACK_MSG_PARSING_ERROR);
                            state = State.Waiting;

                        } else if(!verifyChecksum(value)) {
                            sendAcknowledgement(Protocol.RES_ACK_CHECKSUM_FAILED);
                            state = State.Waiting;
                        } else {
                            value.remove(value.size()-1);
                            onMessageReceived(tag, value);
                            state = State.Waiting;
                        }
                    } else if(b == Protocol.STUFFED_BYTE) {
                        state = State.Destuffing;
                    } else {
                        value.add(b);
                    }
                    break;
                }

                case Destuffing: {
                    value.add((byte)(Protocol.STUFFED_BYTE ^ b));
                    state = State.Receiving;
                    break;
                }
            }
        }
    }

    private boolean verifyChecksum(ArrayList<Byte> bytes) {
        int checksum = Protocol.START_OF_MESSAGE + tag + length;

        for (int i=0; i < value.size(); i++) {
            checksum += value.get(i);
        }

        return (checksum == 0);
    }

    private void onMessageReceived(byte tag, ArrayList<Byte> value) {
        messageReceivedListener.onMessageReceived(tag, value);
    }

    private void sendAcknowledgement(byte b) {
        sendAcknowledgementListener.onSendAcknowledgement(b);
    }

    private byte[] removeStuffedBytes(byte[] bytes) {
        return bytes;
    }

    public interface MessageReceivedListener {
        void onMessageReceived(byte tag, ArrayList<Byte> value);
    }

    public interface SendAcknowledgementListener {
        void onSendAcknowledgement(byte ack);
    }
}

