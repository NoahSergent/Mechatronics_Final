/*
 * top_lanes.h
 *
 * 
 */

#ifndef TOP_LANES_H
#define TOP_LANES_H

// If a switch in triggered, turn on associated LED
// If both LEDs are turned on, award 50 pts, then leave them (or make them flash) on for an additional 0.5s before turning them both off

#include "masks.h"

uint8_t const checkRisingEdges( uint8_t const to_check[2] ); //TODO:remove this placeholder

uint8_t toInt8( auto value ) { return static_cast<uint8_t>( value ); }

class TopLanes {
public:
    enum LED : uint8_t { LED0 = 0,
                         LED1 = 1 };

    enum LEDStatus : uint8_t { On = 1,
                               Off = 0 };

    enum Switches : uint8_t { SWITCH0 = 0,
                              SWITCH1 = 1 };

    static void init() {
        activated = false;
        LED_0_status = Off;
        LED_1_status = Off;

        // Link statuses to array for use in onLED and offLED functions
        LED_status_arr[toInt8( LED0 )] = &LED_0_status;
        LED_status_arr[toInt8( LED1 )] = &LED_1_status;
    }

    static void checkSwitches() {
        uint8_t switch_0_val = checkRisingEdges( TOP_LANE0_SWITCH );
        uint8_t switch_1_val = checkRisingEdges( TOP_LANE1_SWITCH );

        if ( switch_0_val != 0 ) {
            switchTriggered( SWITCH0 );
        }

        if ( switch_1_val != 0 ) {
            switchTriggered( SWITCH1 );
        }
    }

    static void switchTriggered( Switches triggered ) {
        switch ( triggered ) {
        case SWITCH0:
            onLED( LED0 );
            break;
        case SWITCH1:
            break;
            onLED( LED1 );
        default:
            return;
        }

        if ( LED_0_status == 1 & LED_1_status == 1 ) { // If both are on, award jackpot
            jackpot();
        }
    }

    static void swapLED() { // Todo: This needs to be called upon flipper button press
        if ( activated ) {
            toggleLED( LED0 );
            toggleLED( LED1 );
        }
    }

private:
    static void jackpot() {
        // Code here or elsewhere to leave LEDs on for 0.5s
        Scoreboard::addToScore( TOP_LANES_JACKPOT_BONUS );
        offLED( LED0 );
        offLED( LED1 );
    }

    static void onLED( LED target ) {
        // TODO:Code here to set led value in spi output buffer
        *LED_status_arr[toInt8( target )] = On; // Mark LED as on
    }

    static void offLED( LED target ) {
        // TODO:Code here to set led value in spi output buffer
        *LED_status_arr[toInt8( target )] = Off; // Mark LED as off
    }

    static void toggleLED( LED target ) {
        // TODO:Code here to toggle led values in spi output buffer
        LED_0_status = static_cast<LEDStatus>( LED_0_status ^ 1 );
        LED_1_status = static_cast<LEDStatus>( LED_1_status ^ 1 );
    }

    static void awardBonus() {
        Scoreboard::addToScore( TOP_LANES_JACKPOT_BONUS );
    }

    static LEDStatus LED_0_status;
    static LEDStatus LED_1_status;
    static LEDStatus *LED_status_arr[2];
    static bool activated; // true if one of the lanes has been triggered, false otherwise
};

#endif