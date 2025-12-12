// /*
//  * masks.h
//  *
//  * Contains masks for the gpio inputs
//  */

#ifndef MASKS_H
#define MASKS_H

#include <avr/io.h>

// // Top Lanes Constants
// extern const uint8_t TOP_LANES_JACKPOT_BONUS;

// // Atmega Pins
//     // This struct will help with pointing to the GPIO pins
    struct GPIOPin {
        volatile uint8_t *port; // Pointer to port
        uint8_t bit;            // bit index
    };

    // Breadcrumb I/O
    extern GPIOPin breadcrumb_pin[2];

//     // SPI Latch I/O
//     extern GPIOPin switch_latch_pin[2];
//     extern GPIOPin LED_latch_pin[2];

//     // Newtons Pendulum I/O
//     extern GPIOPin opticalEncoder_A_pin[2];
//     extern GPIOPin opticalEncoder_B_pin[2];

//     // Scoreboard I/O
//     extern GPIOPin rs_485_de[2];



// // SPI Expansion Input Masks
//     extern const uint8_t TOP_LANE0_SWITCH[2];
//     extern const uint8_t TOP_LANE1_SWITCH[2];

//     extern const uint8_t spinner_sm[2];

//     extern const uint8_t hurry_up[2];

// // SPI Expansion Output Masks
//     extern const uint8_t TOP_LANE0_LED[2];
//     extern const uint8_t TOP_LANE1_LED[2];
//     extern const uint8_t ramp_lights[2];

//     extern const uint8_t drop_bank_lights[2];

//     extern const uint8_t left_lane_upper_lights[2];
//     extern const uint8_t right_lane_upper_lights[2];
//     extern const uint8_t left_lane_lower_lights[2];
//     extern const uint8_t right_lane_lower_lights[2];

//     extern const uint8_t orbit_lights[2];

//     extern const uint8_t spinner_lights[2];

//     extern const uint8_t newton_lights[2];
    
//     extern const uint8_t standup_lights[2];


#endif