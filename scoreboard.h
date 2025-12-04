/*
 * scoreboard.h
 *
 * Main file for Mechatronics Final
 */

#ifndef SCOREBOARD_H
#define SCOREBOARD_H

#include <avr/interrupt.h>
#include <avr/io.h>

uint8_t START_BYTE = 0xFF;
uint8_t UPDATE_SCORE_CMD = 0x00;
uint8_t END_BYTE = 0x00;
const uint8_t MESSAGE_LENGTH = 6;


class Scoreboard {
public:
    static void configure() {

        UCSR0B = ( ( 1 << TXCIE0 ) | ( 0 << UDRIE0 ) | ( 1 << TXEN0 ) | ( 0 << UCSZ02 ) );
        UCSR0C = ( 1 << UCSZ01 ) | ( 1 << UCSZ00 );

        UBRR0 = 4; //corresponds to 250k Baud

        // Read UCSR0A&(1<<TXC0) is true when transmission is complete

        send_buffer[0] = &START_BYTE;
        send_buffer[1] = &UPDATE_SCORE_CMD;
        send_buffer[2] = &upper_score_byte;
        send_buffer[3] = &lower_score_byte;
        send_buffer[4] = &END_BYTE;

        send_index = 0;
    }

    static const void sendScorePolling(const uint8_t upperbyte, const uint8_t midbyte, const uint8_t lowerbyte) {
        sendBytePolling(START_BYTE);
        sendBytePolling(UPDATE_SCORE_CMD);
        sendBytePolling(upperbyte);
        sendBytePolling(midbyte);
        sendBytePolling(lowerbyte);
        sendBytePolling(END_BYTE);
        
    }
    
    static const void sendScorePolling(uint32_t other_score) {
        uint8_t lowerbyte = static_cast<uint8_t>(other_score & 0x00FF);
        uint8_t midbyte = static_cast<uint8_t>((other_score >> 8) & 0x00FF);
        uint8_t upperbyte = static_cast<uint8_t>((other_score >> 16) & 0x00FF);
        sendScorePolling(upperbyte, midbyte, lowerbyte);
    }

    static const void sendScorePolling(void) {
        lower_score_byte = static_cast<uint8_t>(score & 0x00FF);
        upper_score_byte = static_cast<uint8_t>((score >> 8) & 0x00FF);
        middle_score_byte = static_cast<uint8_t>((score >> 16) & 0x00FF);
        sendScorePolling(upper_score_byte, middle_score_byte, lower_score_byte);
    }

    static const void sendScoreInterrupt() {
        send_index = 0;
        lower_score_byte = static_cast<uint8_t>(score & 0x00FF);
        upper_score_byte = static_cast<uint8_t>((score >> 8) & 0x00FF);
        middle_score_byte = static_cast<uint8_t>((score >> 16) & 0x00FF);
        sendByteFromBuffer();
    }

    static volatile uint32_t score;
    static volatile uint8_t lower_score_byte;
    static volatile uint8_t middle_score_byte;
    static volatile uint8_t upper_score_byte;

    static volatile uint8_t *send_buffer[MESSAGE_LENGTH];
    static volatile uint8_t send_index;

private:
    static void sendBytePolling( const uint8_t to_send ) {
        while ( !( UCSR0A & ( 1 << UDRE0 ) ) );
        UDR0 = to_send;
    }

    static void sendByteFromBuffer() { 
        UDR0 = *send_buffer[send_index++];
        if(send_index >= MESSAGE_LENGTH) {
            send_index = 0;
        }
    }

};

#endif