/*
 * spi.h
 *
 * 
 */

#ifndef SPI_H
#define SPI_H

#include <avr/io.h>
#include "masks.h"

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
inline uint8_t LED_toggle( uint8_t LED_mask[2] ) {
    return SPIoutput[LED_mask[0]] ^= ( SPIoutput[LED_mask[0]] & LED_mask[1] ); // Toggles LED
}
inline uint8_t LED_on( uint8_t LED_mask[2] ) {
    return SPIoutput[LED_mask[0]] |= ( SPIoutput[LED_mask[0]] & LED_mask[1] ); // Turns on LED
}
inline uint8_t LED_off( uint8_t LED_mask[2] ) {
    return SPIoutput[LED_mask[0]] &= ~( SPIoutput[LED_mask[0]] & LED_mask[1] ); // Turns off LED
}
inline uint8_t LED_set( uint8_t LED_mask[2], uint8_t bit_pattern ) {                               //for newton's pendulum
    return SPIoutput[LED_mask[0]] = bit_pattern & ( SPIoutput[LED_mask[0]] & LED_mask[1] ); // Sets LEDs in groups
}


#endif