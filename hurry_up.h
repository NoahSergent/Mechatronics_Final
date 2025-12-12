/*
 * hurry_up.h
 *
 * 
 */

#ifndef HURRY_UP_H
#define HURRY_UP_H

/*
 * HurryUpTarget.c
 * Hurry-Up Target Feature for Pinball

 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>

//*****************************************************************************
// Pin Definitions (ATmega328P)
//*****************************************************************************

// Inputs – Right Inlane (PC0), Hurry-Up Target (PC1)
#define RIGHT_INLANE_BIT   PC0
#define HURRY_TARGET_BIT   PC1

// Output – Hurry-Up LED (PC2)
#define HURRY_LED_BIT      PORTC2   // used with PORTC register

//*****************************************************************************
// Timing Constants
//

#define TICK_MS                  10

#define HURRY_DURATION_MS        5000
#define HURRY_DURATION_TICKS     (HURRY_DURATION_MS / TICK_MS)   // 500 ticks

// 2 Hz blink: period = 500 ms → toggle every 250 ms
#define HURRY_LED_TOGGLE_MS      250
#define HURRY_LED_TOGGLE_TICKS   (HURRY_LED_TOGGLE_MS / TICK_MS) // 25 ticks

//*****************************************************************************
// Hurry-Up State and Variables
//*****************************************************************************

typedef enum {
    HURRY_STATE_IDLE = 0,
    HURRY_STATE_ACTIVE
} HurryState_t;

volatile HurryState_t hurry_state = HURRY_STATE_IDLE;

// Counts down from HURRY_DURATION_TICKS (e.g., 500 ticks = 5 seconds)
volatile uint16_t hurry_timer_ticks = 0;

// Counts up to HURRY_LED_TOGGLE_TICKS (e.g., 25 ticks = 250 ms)
volatile uint16_t hurry_led_ticks = 0;

// Tracks whether LED is currently ON or OFF
volatile uint8_t hurry_led_on = 0;

// Simple score variable for jackpot
volatile uint16_t score = 0;

// Previous switch states for rising-edge detection (active-low inputs)
static uint8_t prev_inlane_pressed = 0;
static uint8_t prev_target_pressed = 0;

//*****************************************************************************
// Local Helper Functions
//*****************************************************************************

// Start Hurry-Up mode
static void Hurry_Start(void)
{
    hurry_state = HURRY_STATE_ACTIVE;

    hurry_timer_ticks = HURRY_DURATION_TICKS;  // 5 seconds total
    hurry_led_ticks = 0;

    // Turn LED on initially
    hurry_led_on = 1;
    PORTC |= (1 << HURRY_LED_BIT);
}

// Stop Hurry-Up mode
static void Hurry_Stop(void)
{
    hurry_state = HURRY_STATE_IDLE;
    hurry_timer_ticks = 0;
    hurry_led_ticks = 0;

    // Turn LED off
    hurry_led_on = 0;
    PORTC &= ~(1 << HURRY_LED_BIT);
}

//*****************************************************************************
// Initialization
//*****************************************************************************

void Hurry_Init(void)
{
    //-------------------------------------------------------------------------
    // Configure Port C Pins
    //-------------------------------------------------------------------------
    // PC0: Right Inlane Switch (input with pull-up)
    // PC1: Hurry-Up Target Switch (input with pull-up)
    // PC2: Hurry-Up LED (output)
    //-------------------------------------------------------------------------

    // PC0, PC1 as inputs
    DDRC &= ~((1 << RIGHT_INLANE_BIT) | (1 << HURRY_TARGET_BIT));
    // Enable internal pull-ups on PC0, PC1
    PORTC |= (1 << RIGHT_INLANE_BIT) | (1 << HURRY_TARGET_BIT);

    // PC2 as output
    DDRC |= (1 << DDC2);
    PORTC &= ~(1 << HURRY_LED_BIT);    // LED initially OFF

    //-------------------------------------------------------------------------
    // Configure Timer1 for 10 ms periodic interrupt
    //-------------------------------------------------------------------------
    // Mode: CTC (Clear Timer on Compare Match)
    // Prescaler: clk/64
    // OCR1A = 2499 → interrupt every 10 ms
    //-------------------------------------------------------------------------

    // COM1A1:0 = 0 (no pin toggle), WGM11:0 = 0 (CTC mode via WGM12)
    TCCR1A = (0 << COM1A1) | (0 << COM1A0) |
             (0 << WGM11)  | (0 << WGM10);

    // WGM13:0 = 0100 (CTC mode using OCR1A as TOP), CS12:0 = 011 (clk/64)
    TCCR1B = (0 << WGM13) | (1 << WGM12) |
             (0 << CS12)  | (1 << CS11) | (1 << CS10);

    // Enable Timer1 Compare Match A interrupt
    TIMSK1 = (1 << OCIE1A);

    // Compare value for 10 ms period
    OCR1A = 2499;

    // Initialize state
    Hurry_Stop();

    // Enable global interrupts
    sei();
}

//*****************************************************************************
// Timer1 Compare Match A ISR – runs every 10 ms
//*****************************************************************************

ISR(TIMER1_COMPA_vect)
{
    // Read switches (active-low because of pull-ups)
    uint8_t inlane_pressed = ((PINC & (1 << RIGHT_INLANE_BIT)) == 0);
    uint8_t target_pressed = ((PINC & (1 << HURRY_TARGET_BIT)) == 0);

    //-------------------------------------------------------------------------
    // Detect rising edge on Right Inlane → start Hurry-Up
    //-------------------------------------------------------------------------
    if (inlane_pressed && !prev_inlane_pressed) {
        // Ball just rolled through Right Inlane
        Hurry_Start();
    }

    //-------------------------------------------------------------------------
    // Hurry-Up active: handle timer, blinking, and target hit
    //-------------------------------------------------------------------------
    if (hurry_state == HURRY_STATE_ACTIVE) {
        // Countdown total Hurry-Up time
        if (hurry_timer_ticks > 0) {
            hurry_timer_ticks--;
        } else {
            // Time expired with no hit → stop Hurry-Up
            Hurry_Stop();
        }

        // LED blinking: toggle every HURRY_LED_TOGGLE_TICKS (e.g., 25 ticks = 250 ms)
        hurry_led_ticks++;
        if (hurry_led_ticks >= HURRY_LED_TOGGLE_TICKS) {
            hurry_led_ticks = 0;

            if (hurry_led_on) {
                PORTC &= ~(1 << HURRY_LED_BIT);
                hurry_led_on = 0;
            } else {
                PORTC |= (1 << HURRY_LED_BIT);
                hurry_led_on = 1;
            }
        }

        // Detect rising edge on Hurry-Up Target while active → jackpot
        if (target_pressed && !prev_target_pressed) {
            score += 100;     // Award 100 point jackpot
            Hurry_Stop();     // Turn off LED and deactivate Hurry-Up
        }
    }

    // Save current switch states for next edge-detection
    prev_inlane_pressed = inlane_pressed;
    prev_target_pressed = target_pressed;
}

//*****************************************************************************
// Main
//*****************************************************************************

int main(void)
{
    // Initialize Hurry-Up system (I/O + Timer1)
    Hurry_Init();

    // Main loop – can be expanded later for more game logic
    while (1) {
        
    }
}


#endif