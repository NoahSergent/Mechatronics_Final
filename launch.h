// /*
//  * launch.h
//  *
//  * 
//  */

// //Ball Launch***********************************************************************
// #ifndef LAUNCH_SOLENOID_H
// #define LAUNCH_SOLENOID_H

// #include "masks.h"
// #include "spi.h"



// void Ball_launch(uint8_t launch_ball, uint16_t ball_launch_count) {
//     if ( launch_ball == 0 ) {
//         LED_off( launch_LED );
//         setGPIOPin(launch_pin);

//         if ( CheckFallingEdges( launch_button_mask ) ) {
//             launch_ball = 1;
//             ball_launch_count = 0;
//         }
//     } else { //launch ball is 1
//         LED_on( launch_LED );
//         PORTC |= ( 1 << PORTC4 );
//         ball_launch_count++;
//         if ( ball_launch_count >= 500 ) {
//             launch_ball = 0;
//         }
//     }
// }

// #endif
