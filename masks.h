/*
 * masks.h
 *
 * Contains masks for the gpio inputs
 */

#ifndef MASKS_H
#define MASKS_H

#include <avr/io.h>




// SPI Expansion Outputs
extern const uint8_t TOP_LANE0_LED[] = { 0, 0 };
extern const uint8_t TOP_LANE1_LED[] = { 0, 0 };


// SPI Expansion Inputs
extern const uint8_t TOP_LANE0_SWITCH[] = { 0, 0 };
extern const uint8_t TOP_LANE1_SWITCH[] = { 0, 0 };


#endif