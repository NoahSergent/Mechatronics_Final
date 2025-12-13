/*
 * spi.h
 *
 * 
 */

#ifndef SPI_H
#define SPI_H

#include "masks.h"
#include <avr/io.h>

// How to pull data from debounced switch inputs	(place in better location for easy access for everyone)
inline uint8_t CheckSwitchState( uint8_t const switch_mask[2] ) {
    return switch_states[switch_mask[0]] & switch_mask[1];
}
inline uint8_t CheckFallingEdges( uint8_t const switch_mask[2] ) {
    return falling_edges[switch_mask[0]] & switch_mask[1];
}
inline uint8_t CheckRisingEdges( uint8_t const switch_mask[2] ) {
    return rising_edges[switch_mask[0]] & switch_mask[1]; //may need to be reversed
}

// How to toggle LED *** Check that this will behave as expected
inline void LED_toggle( const uint8_t LED_mask[2] ) {
    SPIoutput[LED_mask[0]] ^= ( LED_mask[1] ); // Toggles LED
}
inline void LED_on( const uint8_t LED_mask[2] ) {
    SPIoutput[LED_mask[0]] |= ( LED_mask[1] ); // Turns on LED
}
inline void LED_off( const uint8_t LED_mask[2] ) {
    SPIoutput[LED_mask[0]] &= ~( LED_mask[1] ); // Turns off LED
}
inline void LED_set( const uint8_t LED_mask[2], uint8_t bit_pattern ) { //for newton's pendulum
    SPIoutput[LED_mask[0]] = bit_pattern & ( LED_mask[1] );             // Sets LEDs in groups
}

#endif