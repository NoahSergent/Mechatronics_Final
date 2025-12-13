/*
 * scoreboard.cpp
 *
 * Main file for Mechatronics Final
 */


#include "scoreboard.h"
#include "masks.h"
#include "top_lanes.h"

TopLanes::LEDStatus TopLanes::LED_0_status = TopLanes::Off;
TopLanes::LEDStatus TopLanes::LED_1_status = TopLanes::Off;
TopLanes::LEDStatus* TopLanes::LED_status_arr[2] = { nullptr, nullptr };
const uint8_t* TopLanes::LED_mask_arr[2] = {TOP_LANE0_LED, TOP_LANE1_LED};
const uint8_t* TopLanes::switch_arr[2] = {TOP_LANE0_SWITCH, TOP_LANE1_SWITCH};
bool TopLanes::activated = false;