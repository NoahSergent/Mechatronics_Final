#include "drop_target.h"

void DropTarget::init() {
    DDRD |= (1 << PORTD7);
    clearGPIOPin(drop_target_pin);

    solenoid_state = Idle;
    solenoid_count = 0;
    score_to_add_ = 0;

    for (uint8_t i = 0; i < 3; i++) {
        uint8_t stable = CheckSwitchState(target_mask_arr[i]);
        target_states_[i] = (stable == 0) ? Down : Up;
        previous_states[i] = target_states_[i];
    }
}

void DropTarget::incrementCount() {
    solenoid_count++;
}

void DropTarget::fireSolenoid() {
    if (solenoid_count < DROP_TARGET_MAX_COUNT) {
        setGPIOPin(drop_target_pin);
    } else {
        clearGPIOPin(drop_target_pin);
        solenoid_state = Idle;
        solenoid_count = 0;

        for (uint8_t i = 0; i < 3; i++) {
            uint8_t stable = CheckSwitchState(target_mask_arr[i]);
            target_states_[i] = (stable != 0) ? Down : Up;
            previous_states[i] = target_states_[i];
        }
    }
}

void DropTarget::checkTargets() {
    if (solenoid_state != Fire) {
        clearGPIOPin(drop_target_pin);
    }

    switch (solenoid_state) {
    case Idle: {
        bool all_down = true;

        for (uint8_t i = 0; i < 3; i++) {
            uint8_t stable = CheckSwitchState(target_mask_arr[i]);
            target_states_[i] = (stable == 0) ? Down : Up;

            if (previous_states[i] == Up && target_states_[i] == Down) {
                Scoreboard::addToScore((uint16_t)10);
            }

            previous_states[i] = target_states_[i];

            if (target_states_[i] != Down) {
                all_down = false;
            }
        }

        if (all_down) {
            Scoreboard::addToScore(score_to_add_);
            score_to_add_ = 50;
            solenoid_count = 0;
            solenoid_state = Wait;
        }
    } break;

    case Wait:
        if (solenoid_count >= (uint16_t)(DROP_TARGET_MAX_COUNT * 3U)) {
            solenoid_count = 0;
            solenoid_state = Fire;
        }
        break;

    case Fire:
        fireSolenoid();
        break;

    default:
        clearGPIOPin(drop_target_pin);
        solenoid_state = Idle;
        solenoid_count = 0;
        break;
    }
}

TargetState DropTarget::target_states_[3] = { Up, Up, Up };
TargetState DropTarget::previous_states[3] = { Up, Up, Up };
const uint8_t* DropTarget::target_mask_arr[3] = { drop_bank_mask0, drop_bank_mask1, drop_bank_mask2 };

volatile uint16_t DropTarget::solenoid_count = 0;
SolenoidState DropTarget::solenoid_state = Idle;
uint16_t DropTarget::score_to_add_ = 0;

