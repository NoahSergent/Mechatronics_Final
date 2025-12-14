// /*
//  * masks.cpp
//  *
//  * Contains masks for the gpio inputs
//  */

#include "masks.h"
#include <avr/io.h>

// // Top Lanes Constants
const uint8_t TOP_LANES_JACKPOT_BONUS = 50;

//Drop target constants
extern const uint16_t DROP_TARGET_MAX_COUNT= 1000;

// // Atmega Pins
//     // Breadcrumb I/O
extern GPIOPin breadcrumb_pin = { &PORTC, PORTC5 };

//     // SPI Latch I/O
extern GPIOPin switch_latch_pin = { &PORTB, PORTB2 };
extern GPIOPin LED_latch_pin = { &PORTB, PORTB1 };

// Newtons Pendulum I/O
extern GPIOPin opticalEncoder_A_pin = { &PORTC, PORTC3 };
extern GPIOPin opticalEncoder_B_pin = { &PORTC, PORTC4 };

// Ball Launch
extern GPIOPin launch_pin = { &PORTD, PORTD4 };

// Drop target pin
extern GPIOPin drop_target_pin = { &PORTD, PORTD7 };

// SPI Globals
extern volatile uint8_t SPIoutput[Bank_Size] = { 0 }; // LED data
extern volatile uint8_t LEDcount = 0;
extern volatile uint8_t readSwitches[Bank_Size] = { 0 }; // Bank_Size is 3 by default
extern volatile uint8_t updateFlag = 0;                  // SPI full Bank_Index data received

// Debounce Globals
extern volatile uint8_t noisySwitchSet[Bank_Size][Buffer_Length] = { 0 }; // sets all inputs to 0 initially
extern volatile uint8_t circularBuff[Bank_Size][Buffer_Length] = { 0 };
extern volatile uint8_t rising_edges[Bank_Size] = { 0 };
extern volatile uint8_t falling_edges[Bank_Size] = { 0 };
extern volatile uint8_t switch_states[Bank_Size] = { 0 };
extern volatile uint8_t switchIndex = 0;

// SPI Expansion Input Masks TODO redefine after checking switch location on pinball machine
extern const uint8_t TOP_LANE0_SWITCH[2] = { 1, 0b00000010 }; // Bank B pin 1
extern const uint8_t TOP_LANE1_SWITCH[2] = { 1, 0b00000100 }; // Bank B pin 2

    extern const uint8_t spinner_sm[2] = { 1, 0x20 }; // finished

extern const uint8_t right_inlane_switch[2] = { 0, 0b01000000 }; //new
extern const uint8_t hurry_up_lights[2] = { 1, 0b00010000 }; //new
extern volatile uint16_t hurryCount1 = 0; //new
extern volatile uint16_t hurryCount2 = 0; //new
extern const uint8_t hurry_up[2] = { 1, 0b00010000 }; //updated

// SPI Expansion Output Masks TODO redefine after checking LED locations on pinball machine (include current limiting resistors)

extern const uint8_t TOP_LANE0_LED[2] = { 1, 0b00000010 };
extern const uint8_t TOP_LANE1_LED[2] = { 1, 0b00000100 };
extern const uint8_t ramp_lights[2] = { 0, 0x01 };

extern const uint8_t drop_bank_mask0[2] = { 2, 0b00000010 };
extern const uint8_t drop_bank_mask1[2] = { 2, 0b00000100 };
extern const uint8_t drop_bank_mask2[2] = { 2, 0b00001000 };

extern const uint8_t left_lane_upper_lights[2] = { 0, 0x01 };
extern const uint8_t right_lane_upper_lights[2] = { 0, 0x01 };
extern const uint8_t left_lane_lower_lights[2] = { 0, 0x01 };
extern const uint8_t right_lane_lower_lights[2] = { 0, 0x01 };

extern const uint8_t orbit_lights[2] = { 0, 0x01 };

    extern const uint8_t spinner_lights[2] = { 1, 0x01 };

extern const uint8_t newton_lights[2] = { 2, 0xFF }; // This requires 8 LEDs, so a full port, assuming port 3

extern const uint8_t standup_lights[2] = { 0, 0x01 };

// PWM Variables
    extern const uint8_t flipper_mask0[2] = {0, 1};
    extern const uint8_t flipper_mask1[2] = {0, 0b00000100};
    extern const uint8_t eos_mask0[2] = {0, 0b00000010};
    extern const uint8_t eos_mask1[2] = {0, 0b00001000};

    extern const uint8_t kHold_Power = 25; // Define Holding OCR0A/B Value
    extern const uint8_t kHit_Power = 255; // Define Flipping 0CR0A/B Value. %DC = 0CR0A / 255
    extern uint8_t flipper_button0 = 1; // Global Flipper state variable
    extern uint8_t EOS_switch0 = 0; //Global EOS State variable
    extern uint8_t flipper_button1 = 1; // Global Flipper state variable
    extern uint8_t EOS_switch1 = 0; //Global EOS State variable
 
 // Pendulum
	extern const int16_t encoder_table[16] = {
		0, 1, -1, 255,
		-1, 0, 255, 1,
		1, 255, 0, -1,
		255, -1, 1, 0
	};

	volatile extern int16_t position = 0;
	volatile extern uint8_t old_channels = 0;
	volatile extern uint8_t new_channels = 0;
	volatile extern uint16_t pendulumSwitch = 0;
	volatile extern uint16_t max_position = 76; // Assumes that 256 options is sufficient, update value after testing

	volatile extern uint16_t score = 0;

// Ball launch
extern const uint8_t launch_button_mask[2] = { 0, 0b00010000 };
extern const uint8_t launch_LED[2] = { 2, 0b10000000 };
extern const uint8_t launch_max_count = 1000;

