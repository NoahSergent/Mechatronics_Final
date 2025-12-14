// /*
//  * masks.h
//  *
//  * Contains masks for the gpio inputs
//  */

#ifndef MASKS_H
#define MASKS_H

#include <avr/io.h>

#define Buffer_Length 6 // Modify by checking with scope for debounce time in switches - adjust with Timer1 clock speed
#define Bank_Size 3 // Modify if we ever use more or less than three banks of switches

// // Top Lanes Constants
extern const uint8_t TOP_LANES_JACKPOT_BONUS;

//Drop target constants
extern const uint16_t DROP_TARGET_MAX_COUNT;

// 
// // Atmega Pins
    // This struct will help with pointing to the GPIO pins
    struct GPIOPin {
        volatile uint8_t *port; // Pointer to port
        uint8_t bit;            // bit index
    };

    // Breadcrumb I/O
    extern GPIOPin breadcrumb_pin;

    // SPI Latch I/O
    extern GPIOPin switch_latch_pin;
    extern GPIOPin LED_latch_pin;

    // Newtons Pendulum I/O
    extern GPIOPin opticalEncoder_A_pin;
    extern GPIOPin opticalEncoder_B_pin;

    // Scoreboard I/O
    extern GPIOPin rs_485_de;

    // Ball launch on
    extern GPIOPin launch_pin;

    // Drop target pin
    extern GPIOPin drop_target_pin;

// SPI Globals
extern volatile uint8_t SPIoutput[Bank_Size]; // LED data
extern volatile uint8_t LEDcount;
extern volatile uint8_t readSwitches[Bank_Size]; // Bank_Size is 3 by default
extern volatile uint8_t updateFlag; // SPI full Bank_Index data received

// Debounce Globals
extern volatile uint8_t noisySwitchSet[Bank_Size][Buffer_Length]; // sets all inputs to 0 initially
extern volatile uint8_t circularBuff[Bank_Size][Buffer_Length];
extern volatile uint8_t rising_edges[Bank_Size];
extern volatile uint8_t falling_edges[Bank_Size];
extern volatile uint8_t switch_states[Bank_Size];
extern volatile uint8_t switchIndex;


// // SPI Expansion Input Masks
    extern const uint8_t TOP_LANE0_SWITCH[2];
    extern const uint8_t TOP_LANE1_SWITCH[2];

    extern const uint8_t spinner_sm[2];
    extern const uint8_t drop_bank_mask0[2];
    extern const uint8_t drop_bank_mask1[2];
    extern const uint8_t drop_bank_mask2[2];

    extern const uint8_t hurry_up[2];

// SPI Expansion Output Masks
    extern const uint8_t TOP_LANE0_LED[2];
    extern const uint8_t TOP_LANE1_LED[2];
    extern const uint8_t ramp_lights[2];


    extern const uint8_t left_lane_upper_lights[2];
    extern const uint8_t right_lane_upper_lights[2];
    extern const uint8_t left_lane_lower_lights[2];
    extern const uint8_t right_lane_lower_lights[2];

    extern const uint8_t orbit_lights[2];

    extern const uint8_t spinner_lights[2];

    extern const uint8_t newton_lights[2];
    
    extern const uint8_t standup_lights[2];

//Flipper   
// PWM Variables
    extern const uint8_t flipper_mask0[2];
    extern const uint8_t flipper_mask1[2];
    extern const uint8_t eos_mask0[2];
    extern const uint8_t eos_mask1[2];

    extern const uint8_t kHold_Power; // Define Holding OCR0A/B Value
    extern const uint8_t kHit_Power; // Define Flipping 0CR0A/B Value. %DC = 0CR0A / 255
    extern uint8_t flipper_button0; // Global Flipper state variable
    extern uint8_t EOS_switch0;//Global EOS State variable
    extern uint8_t flipper_button1; // Global Flipper state variable
    extern uint8_t EOS_switch1; //Global EOS State variable

// Ball launch
    extern const uint8_t launch_button_mask[2];
    extern const uint8_t launch_LED[2];
    extern const uint8_t launch_max_count;

	// Pendulum
	extern const int16_t encoder_table[16];

	volatile extern int16_t position;
	volatile extern uint8_t old_channels;
	volatile extern uint8_t new_channels;
	volatile extern uint16_t pendulumSwitch;
	volatile extern uint16_t max_position; // Assumes that 256 options is sufficient, update value after testing
	
	volatile extern uint16_t score;
	

#endif