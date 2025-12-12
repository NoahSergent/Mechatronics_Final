/*
 * masks.cpp
 *
 * Contains masks for the gpio inputs
 */


// #include <avr/io.h>
#include "masks.h"

// Top Lanes Constants
const uint8_t TOP_LANES_JACKPOT_BONUS = 50;

// Atmega Pins
    // Breadcrumb I/O
    extern GPIOPin breadcrumb_pin[2] = { &PORTC, PORTC5 };

    // SPI Latch I/O
    extern GPIOPin switch_latch_pin[2] = { &PORTB, PORTB2 };
    extern GPIOPin LED_latch_pin[2] = { &PORTB, PORTB1 };

    // Newtons Pendulum I/O
    extern GPIOPin opticalEncoder_A_pin[2] = { &PORTC, PORTC3 };
    extern GPIOPin opticalEncoder_B_pin[2] = { &PORTC, PORTC4 };


// SPI Expansion Input Masks TODO redefine after checking switch location on pinball machine
    extern const uint8_t TOP_LANE0_SWITCH[2] = {0, 0};
    extern const uint8_t TOP_LANE1_SWITCH[2] = {0, 0};

    extern const uint8_t spinner_sm[2] = { 0, 0x02 };

    extern const uint8_t hurry_up[2] = { 0, 0x80 };

// SPI Expansion Output Masks TODO redefine after checking LED locations on pinball machine (include current limiting resistors)

    extern const uint8_t TOP_LANE0_LED[2] = {0, 0};
    extern const uint8_t TOP_LANE1_LED[2] = {0, 0};
    extern const uint8_t ramp_lights[2] = { 0, 0x01 };

    extern const uint8_t drop_bank_lights[2] = { 0, 0x01 };

    extern const uint8_t left_lane_upper_lights[2] = { 0, 0x01 };
    extern const uint8_t right_lane_upper_lights[2] = { 0, 0x01 };
    extern const uint8_t left_lane_lower_lights[2] = { 0, 0x01 };
    extern const uint8_t right_lane_lower_lights[2] = { 0, 0x01 };

    extern const uint8_t orbit_lights[2] = { 0, 0x01 };

    extern const uint8_t spinner_lights[2] = { 0, 0x01 };

    extern const uint8_t newton_lights[2] = { 2, 0xFF }; // This requires 8 LEDs, so a full port, assuming port 3
    
    extern const uint8_t standup_lights[2] = { 0, 0x01 };





