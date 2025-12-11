/*
 * masks.h
 *
 * Contains masks for the gpio inputs
 */

#ifndef MASKS_H
#define MASKS_H

#include <avr/io.h>

// Top Lanes Constants
extern const uint8_t TOP_LANES_JACKPOT_BONUS;

// SPI Expansion Outputs
extern const uint8_t TOP_LANE0_LED[2];
extern const uint8_t TOP_LANE1_LED[2];

// SPI Expansion Inputs
extern const uint8_t TOP_LANE0_SWITCH[2];
extern const uint8_t TOP_LANE1_SWITCH[2];


#endif