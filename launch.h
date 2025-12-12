/*
 * launch.h
 *
 * 
 */

#ifndef LAUNCH_H
#define LAUNCH_H

#include <avr/io.h>
#include <stdint.h>

#define LAUNCH_BTN_BIT   PD4     // button on PD4
#define SOLENOID_BIT     PD0     // solenoid on PD0

int main(void)
{
    // IO Setup
    // Button: input with pull-up
    DDRD  &= ~(1 << LAUNCH_BTN_BIT);   // PD4 input
    PORTD |=  (1 << LAUNCH_BTN_BIT);   // pull-up on

    // Solenoid: output, start OFF
    DDRD  |=  (1 << SOLENOID_BIT);     // PD0 output
    PORTD &= ~(1 << SOLENOID_BIT);     // low = off

    uint8_t  firing = 0;      // 0 = off, 1 = currently firing
    uint16_t count  = 0;      // software “timer”

    while (1)
    {
        uint8_t button_now = PIND & (1 << LAUNCH_BTN_BIT);  // read PD4

        if (!firing) {
            // button is active-low: pressed when bit == 0
            if (button_now == 0) {
                // start launch pulse
                firing = 1;
                count  = 0;
                PORTD |= (1 << SOLENOID_BIT);   // solenoid ON
            }
        } else {
            // already firing: count loop cycles
            count++;

            // tune this number on the real machine so it’s ≤ 1 s
            if (count >= 60000) {               // software limit
                PORTD &= ~(1 << SOLENOID_BIT);  // solenoid OFF
                firing = 0;
            }
        }
    }
}

#endif