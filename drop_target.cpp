/*
 * drop_target.h
 *
 * 
 */

#include "drop_target.h"

void DropTarget::init() {
    DDRD |= ( 1 << PORTD7 );
    solenoid_count = 0;
    // fireSolenoid();
}

void DropTarget::fireSolenoid() {
    if ( solenoid_count < DROP_TARGET_MAX_COUNT ) {
        setGPIOPin( drop_target_pin );
    } else {
        clearGPIOPin( drop_target_pin );
        solenoid_state = Idle;
    }
}

void DropTarget::checkTargets() {
    bool ready_to_fire = true;
    uint8_t current_state;
    switch ( solenoid_state ) {
    case ( Idle ):
    clearGPIOPin(drop_target_pin);

        for ( uint8_t i = 0; i < 3; i++ ) {
            current_state = CheckFallingEdges( target_mask_arr[i] );
            if ( current_state == 1 ) {
                target_states_[i] = Up;
                ready_to_fire = false;
            } else
                target_states_[i] = Down;

            if ( target_states_[i] != previous_states[i] )
                Scoreboard::addToScore( ( uint16_t )5 );

            previous_states[i] = target_states_[i];
        }
        if ( ready_to_fire ) {
            Scoreboard::addToScore( ( uint16_t )50 );
            solenoid_count = 0;
            target_states_[0] = Up;
            target_states_[1] = Up;
            target_states_[2] = Up;
            solenoid_state = Wait;
        }
        break;
    case ( Wait ):
    clearGPIOPin(drop_target_pin);
        if ( solenoid_count >= DROP_TARGET_MAX_COUNT * 3 ) {
            solenoid_state = Fire;
        }
        break;
    case ( Fire ):
        fireSolenoid();

        break;
    }
}

void DropTarget::incrementCount() {
    solenoid_count++;
}

TargetState DropTarget::target_states_[3] = { Up, Up, Up };
TargetState DropTarget::previous_states[3] = { Up, Up, Up };
const uint8_t *DropTarget::target_mask_arr[3] = { drop_bank_mask0, drop_bank_mask1, drop_bank_mask2 };
uint16_t volatile DropTarget::solenoid_count = 0;
SolenoidState DropTarget::solenoid_state = Idle;