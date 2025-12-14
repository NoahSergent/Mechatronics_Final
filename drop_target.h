/*
 * drop_target.h
 *
 * 
 */

#ifndef DROP_TARGET_H
#define DROP_TARGET_H

#include <avr/io.h>
#include "masks.h"
#include "scoreboard.h"
#include "spi.h"

enum TargetState : uint8_t { Up = 1,
                             Down = 0 };

enum Targets : uint8_t { Target0 = 0,
                         Target1 = 1,
                         Target2 = 2 };

enum SolenoidState {Idle, Wait, Fire};

class DropTarget {
public:
    static void init();
    static void checkTargets();
    static void incrementCount();

private:
    static TargetState target_states_[3];
    static SolenoidState solenoid_state;
    static volatile uint16_t solenoid_count;

    static void fireSolenoid();
    static const uint8_t *target_mask_arr[3];
    static TargetState previous_states[3];
};

#endif