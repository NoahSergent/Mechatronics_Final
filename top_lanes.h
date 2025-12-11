/*
 * top_lanes.h
 *
 * 
 */

#ifndef TOP_LANES_H
#define TOP_LANES_H

#include "masks.h"

class TopLanes {
public:
    enum LED : uint8_t { LED0 = 0,
                         LED1 = 1 };

    enum LEDStatus { On,
                     Off };

    enum Switches { SWITCH0,
                    SWITCH1 };

    static void init() {
        activated = false;
        LED_0_status = Off;
        LED_1_status = Off;
    }

    static void switchTriggered( Switches triggered ) {
        switch ( triggered ) {
        case SWITCH0:

            break;
        case SWITCH1:
            break;
        }
    }

    static void swapLED() {
        if (activated) {
            toggleLED(LED0);
            toggleLED(LED1);
        }
    }

private:
    static void onLED( LED target ) {
    }

    static void offLED( LED target ) {
    }

    static void toggleLED(LED target) {

    }

    static void awardBonus() {
        Scoreboard::addToScore( TOP_LANES_BONUS );
    }

    static LEDStatus LED_0_status;
    static LEDStatus LED_1_status;
    static bool activated; // true if one of the lanes has been triggered, false otherwise
};

#endif