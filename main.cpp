
#include <avr/io.h>
#include <avr/interrupt.h>
#include "scoreboard.h"


void setupTimer1( void ) {
    // Set Timer 1 speed fast enough to capture switch events (including debounce time), and fast enough that the LEDs do not appear to flicker
    // Set Timer 1 speed slow enough to not cause data collisions with SPI state machine (we can increase SPI clock speed if needed)
    // Current SPI speed is 125 kHz, need to measure total duration of SPI transfer

    TCCR1A = ( 0 << COM1A1 ) | ( 0 << COM1A0 ) | ( 0 << COM1B1 ) | ( 0 << COM1B0 ) | ( 0 << WGM11 ) | ( 0 << WGM10 );           // CTC mode
    TCCR1B = ( 0 << ICNC1 ) | ( 0 << ICES1 ) | ( 0 << WGM13 ) | ( 1 << WGM12 ) | ( 1 << CS12 ) | ( 0 << CS11 ) | ( 0 << CS10 ); // Set timer 1 to CTC mode and prescaler to 256.
    //TCCR1C = (0<<FOC1A)|(0<<FOC1B);
    // update OSR1A based on longest SPI duration to guarantee no collisions
    OCR1A = 62499; //6249 // total_CTC_time = (OCR1A+1)*del_t = (OCR1A+1)*n/16MHz		0.1 sec in this case
    TIMSK1 = ( 0 << ICIE1 ) | ( 0 << OCIE1B ) | ( 1 << OCIE1A ) | ( 0 << TOIE1 );
    //TIFR1 = (0<<ICF1)|(0<<OCF1B)|(0<<OCF1A)|(0<<TOV1);
}

void setupSPI( void ) // Setup SPI Pins
{
    DDRB = ( 1 << PORTB1 ) | ( 1 << PORTB2 ) | ( 1 << PORTB3 ) | ( 0 << PORTB4 ) | ( 1 << PORTB5 ); //sets up SPI pins
    //DDRB |= (1<<PORTB2); // Switch load latch
    //DDRB |= (1<<PORTB1); // LED latch

    //PORTB |= (1<<PORTB2);
    PORTB |= ( 1 << PORTB2 );  // Sets switch latch pin high, requires output to avoid SS conflict
    PORTB &= ~( 1 << PORTB1 ); // Sets LED latch pin high

    SPCR = ( 1 << SPIE ) | ( 1 << SPE ) | ( 0 << DORD ) | ( 1 << MSTR ) | ( 0 << CPOL ) | ( 0 << CPHA ) | ( 1 << SPR1 ) | ( 1 << SPR0 );
    //clock speed (f_osc/128) if double is low, slowest possible speed for chip (125kHz update)
    SPSR = ( 0 << SPI2X ); // can read SPIF from bit 7, and WCOL from bit 6
                           //SPDR is data register
                           //PORTC ^= (1<<breadcrumb_pin); // Toggle breadcrumb pin
}

volatile uint8_t temp_spdr = 0;

int main() {
    setupTimer1();
    setupSPI();
    Scoreboard::configure();
    Scoreboard::setScore(0);
    while(1);

}

ISR(TIMER1_COMPA_vect) {
    SPDR = temp_spdr;
    temp_spdr = SPDR;
    Scoreboard::setScore(temp_spdr);
    Scoreboard::sendScoreInterrupt();

}

ISR(USART_TX_vect) {
    Scoreboard::interruptStateMachine();
}