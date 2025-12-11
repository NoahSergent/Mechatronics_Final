/*
* main.cpp
*
* Main file for Mechatronics Final
*/

#include <avr/interrupt.h>
#include <avr/io.h>

// NOTE: We should probably keep the ISRs in the main file
#include "debounce.h"
#include "drop_target.h"
#include "flipper.h"
#include "hurry_up.h"
#include "launch.h"
#include "masks.h"
#include "pendulum.h"
#include "scoreboard.h"
/*
Provides functions
- Scoreboard::SetScore(int)
- Scoreboard::addToScore(int)
*/
#include "spi.h"
#include "spinner.h"
#include "top_lanes.h"

int main( void ) {
    // Scoreboard::unitTest();
    spinner_init();
    while ( 1 )
        spinner_update();
        
}

ISR( USART_TX_vect ) {
}