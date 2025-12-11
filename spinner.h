/*
 * spinner.h
 *
 * 
 */

#ifndef SPINNER_H
#define SPINNER_H

#include <avr/io.h>

volatile uint16_t score = 0;

void spinner_init() {
	// PD3 as input (spinner switch)
	DDRC &= ~(1 << PINC5);          // input
	PORTC |= (1 << PINC5);          // enable pull-up
	

	// PD7 as output (spinner LED)
	DDRD |= (1 << PIND7);
}

void spinner_update() {
	static uint8_t prev_state = 1;   // switch is normally HIGH (pull-up)
	uint8_t curr_state;

	// Read current switch state
	curr_state = (PIND & (1 << PINC3)) ;

	// Edge detected (rising or falling)
	if (curr_state != prev_state) {
		// Blink LED (toggel)
		PORTD ^= (1 << PIND7);

		// Add points
		score += 5;

		// Update for next cycle
		prev_state = curr_state;
	}
}



#endif