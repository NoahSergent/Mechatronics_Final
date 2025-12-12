// /*
//  * scoreboard.h
//  *
//  * Main file for Mechatronics Final
//  */

// #ifndef SCOREBOARD_H
// #define SCOREBOARD_H

// #include "masks.h"
// #include <avr/interrupt.h>
// #include <avr/io.h>

// class Scoreboard {
// private:
//     // Constants for Scoreboard
//     static uint8_t START_BYTE;
//     static uint8_t UPDATE_SCORE_CMD;
//     static uint8_t END_BYTE;

// public:
//     static void configure() {

//         DDRB |= ( 1 << PORTB0 ) | ( 1 << PORTB1 ) | ( 1 << PORTB3 );

//         // PORTB |= ( 1 << PORTB3 ); // DE pin, sets RS485 transceiver to transmit mode.
//         PORTB &= ~( 1 << PORTB3 );

//         UCSR0B = ( ( 1 << TXCIE0 ) | ( 0 << UDRIE0 ) | ( 1 << TXEN0 ) | ( 0 << UCSZ02 ) );
//         UCSR0C = ( 1 << UCSZ01 ) | ( 1 << UCSZ00 );

//         // UBRR0 = 3; //corresponds to 250k Baud
//         UBRR0 = 103; //corresponds to 9600 Baud

//         // Read UCSR0A&(1<<TXC0) is true when transmission is complete

//         send_buffer[0] = &START_BYTE;
//         // send_buffer[1] = &UPDATE_SCORE_CMD;
//         send_buffer[1] = &upper_score_byte;
//         send_buffer[2] = &middle_score_byte;
//         send_buffer[3] = &lower_score_byte;
//         send_buffer[4] = &END_BYTE;

//         send_index = 0;
//         // need_to_send = false;
//         overflow = false;
//     }

//     static void setScore( uint32_t value ) {
//         score = value;
//         if ( score > 99999 )
//             overflow = true;
//         // sendScoreInterrupt();
//     }

//     static void setScore( uint16_t value ) {
//         setScore( static_cast<uint32_t>( value ) );
//     }

//     static void setScore( uint8_t value ) {
//         setScore( static_cast<uint32_t>( value ) );
//     }

//     static void addToScore( uint32_t value, bool send_update ) {
//         score = score + value;
//         if ( send_update )
//             sendScoreInterrupt();
//     }
//     static void addToScore( uint32_t value ) {
//         score = score + value;
//     }

//     static void addToScore( uint16_t value ) {
//         addToScore( static_cast<uint32_t>( value ) );
//     }

//     static void addToScore( uint8_t value ) {
//         addToScore( static_cast<uint32_t>( value ) );
//     }

//     static const void sendScorePolling( const uint8_t upperbyte, const uint8_t midbyte, const uint8_t lowerbyte ) {
//         sendBytePolling( START_BYTE );
//         // sendBytePolling( UPDATE_SCORE_CMD );
//         sendBytePolling( upperbyte );
//         sendBytePolling( midbyte );
//         sendBytePolling( lowerbyte );
//         sendBytePolling( END_BYTE );
//     }

//     static const void sendScorePolling( uint32_t other_score ) {
//         uint8_t lowerbyte = static_cast<uint8_t>( other_score & 0x00FF );
//         uint8_t midbyte = static_cast<uint8_t>( ( other_score >> 8 ) & 0x00FF );
//         uint8_t upperbyte = static_cast<uint8_t>( ( other_score >> 16 ) & 0x00FF );
//         sendScorePolling( upperbyte, midbyte, lowerbyte );
//     }

//     static const void sendScorePolling( void ) {
//         lower_score_byte = static_cast<uint8_t>( score & 0x00FF );
//         upper_score_byte = static_cast<uint8_t>( ( score >> 16 ) & 0x00FF );
//         middle_score_byte = static_cast<uint8_t>( ( score >> 8 ) & 0x00FF );
//         sendScorePolling( upper_score_byte, middle_score_byte, lower_score_byte );
//     }

//     static const void sendScoreInterrupt() {
//         send_index = 0;
//         lower_score_byte = static_cast<uint8_t>( score & 0x00FF );
//         upper_score_byte = static_cast<uint8_t>( ( score >> 16 ) & 0x00FF );
//         middle_score_byte = static_cast<uint8_t>( ( score >> 8 ) & 0x00FF );
//         // need_to_send = true;
//         sendByteFromBuffer();
//     }

//     static void unitTest() {
//         configure();
//         setScore( ( uint8_t )10 );
//         for ( uint8_t i = 0; i < 255; i++ );
//         sendScorePolling();
//         while ( 1 ) {
//             for ( uint16_t i = 0; i < 65500; i++ )
//                 ;
//             addToScore( ( uint8_t )1 );
//             sendScorePolling();
//         }
//     }

//     static void sendByteFromBuffer() {
//         UDR0 = *send_buffer[send_index++];
//         if ( send_index >= 6 ) {
//             send_index = 0;
//             // need_to_send = false;
//         }
//     }
// private:
//     static volatile uint32_t score;
//     static volatile uint8_t lower_score_byte;
//     static volatile uint8_t middle_score_byte;
//     static volatile uint8_t upper_score_byte;

//     static volatile uint8_t *send_buffer[5];
//     static volatile uint8_t send_index;
//     // static volatile bool need_to_send;
//     static volatile bool overflow;

//     static void sendBytePolling( const uint8_t to_send ) {
//         while ( !( UCSR0A & ( 1 << UDRE0 ) ) )
//             ;
//         UDR0 = to_send;
//     }

// };

// #endif