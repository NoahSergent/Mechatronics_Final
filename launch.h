/*
 * launch.h
 *
 * 
 */

//Ball Launch***********************************************************************
#ifndef LAUNCH_SOLENOID_H
#define LAUNCH_SOLENOID_H

#include "masks.h"
#include "spi.h"
#include "scoreboard.h"



void Ball_launch(volatile uint8_t &launch_ball, volatile uint16_t &ball_launch_count) {
    if ( launch_ball == 0 ) {
        if ( CheckFallingEdges( launch_button_mask ) ) {
            launch_ball = 1;
            ball_launch_count = 0;
            // Scoreboard::setScore(ball_launch_count); //DEBUG
        }
        LED_off( launch_LED );
        clearGPIOPin(launch_pin);
        // Scoreboard::setScore(ball_launch_count); //DEBUG
        
    } else { //launch ball is 1
        LED_on( launch_LED );
        setGPIOPin(launch_pin);
        ball_launch_count++;
        // Scoreboard::setScore(ball_launch_count); // DBEUG
        if ( ball_launch_count >= launch_max_count ) {
            launch_ball = 0;
        }
    }
}

#endif
