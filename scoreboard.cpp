// /*
//  * scoreboard.cpp
//  *
//  * Main file for Mechatronics Final
//  */

#include "scoreboard.h"
#include "masks.h"
#include <avr/io.h>

// uint8_t Scoreboard::START_BYTE = 0xFF;
// uint8_t Scoreboard::UPDATE_SCORE_CMD = 0x00;
// uint8_t Scoreboard::END_BYTE = 0x00;

// volatile uint32_t Scoreboard::score = 0;
// volatile uint8_t Scoreboard::lower_score_byte = 0;
// volatile uint8_t Scoreboard::middle_score_byte = 0;
// volatile uint8_t Scoreboard::upper_score_byte = 0;
// volatile uint8_t* Scoreboard::send_buffer[5] = {nullptr};
// volatile uint8_t Scoreboard::send_index = 0;
// // volatile bool Scoreboard::need_to_send = false;
// volatile bool Scoreboard::overflow = false;

uint8_t const Scoreboard::SCOREBOARD_ADDR = 0x10;
uint8_t const Scoreboard::SCORE_LOWERBYTE_ADDR = 0x01;
uint8_t const Scoreboard::SCORE_UPPERBYTE_ADDR = 0x02;
uint8_t const Scoreboard::STOP_BYTE = 0xFF;

uint16_t volatile Scoreboard::score_ = 0;
uint8_t volatile Scoreboard::transmit_state_ = 0;

void Scoreboard::configure() {
    DDRD |= ( 1 << PORTD2 );
    UCSR0A = ( 0 << U2X0 ) | ( 1 << MPCM0 ); //revisit if TXC0 should be omitted
    //UCSR0B = (0<<RXCIE0)|(1<<TXCIE0)|(0<<UDRIE0)|(0<<RXEN0)|(1<<TXEN0)|(1<<UCSZ02)|(0<<TXB80);
    UCSR0B = ( ( 1 << TXCIE0 ) | ( 0 << UDRIE0 ) | ( 1 << TXEN0 ) | ( 1 << UCSZ02 ) );
    //UCSR0C = (1<<UMSEL01)|(1<<UMSEL00)|(0<<UPM01)|(0<<UPM00)|(1<<USBS0)|(1<<UCSZ01)|(1<<UCSZ00)|(0<<UCPOL0);
    UCSR0C = ( 1 << UCSZ01 ) | ( 1 << UCSZ00 );

    UBRR0 = 4; //corresponds to 250k Baud, was 3
}

void Scoreboard::sendScoreInterrupt() {
    if ( UCSR0A & ( 1 << UDRE0 ) ) {
        if ( transmit_state_ == 0 ) {
            PORTD |= ( 1 << PORTD2 ); //Enable Transmitter (RS-485 Transceiver IC)
            UCSR0B |= ( 1 << TXB80 ); //Set Address Bit
            UDR0 = SCOREBOARD_ADDR;   //Send node address 0x10
            transmit_state_++;        //Update transmit state
        }
    }
}

void Scoreboard::interruptStateMachine() {
    //Parse packet and send next chunk
    switch ( transmit_state_ ) {
    case 0:                        //Terminate packet
        PORTD &= ~( 1 << PORTD6 ); //Enable Receiver
        break;
    case 1:                          //Completed node address transmission, transmit lower score address
        UCSR0B &= ~( 1 << TXB80 );   //Clear Address Bit
        UDR0 = SCORE_LOWERBYTE_ADDR; //Send lower byte internal address
        transmit_state_++;           //Update transmit_state
        break;
    case 2:                                    //Completed lower byte address, transmit lower byte transmit
        UDR0 = ( uint8_t )( score_ & 0x00FF ); //Send lower
        transmit_state_++;
        break;
    case 3:                          //Completed lower byte, transmit upper byte address
        UDR0 = SCORE_UPPERBYTE_ADDR; //Send upper byte internal address
        transmit_state_++;
        break;
    case 4:                                //Completed upper byte address, transmit upper byte
        UDR0 = ( uint8_t )( score_ >> 8 ); //Send upper byte
        transmit_state_++;                 //Update transmit_state
        break;
    case 5:                  //Completed upper byte, transmit stop byte
        UDR0 = STOP_BYTE;    //Send stop byte
        transmit_state_ = 0; //Update transmit_state
        break;
    default:
        //PORTD &= ~(1<<PORTD6);//Enable Receiver
        transmit_state_ = 0;
        break;
    }
}