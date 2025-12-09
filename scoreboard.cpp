/*
 * scoreboard.cpp
 *
 * Main file for Mechatronics Final
 */


#include "scoreboard.h"
#include "masks.h"



volatile uint32_t Scoreboard::score = 0;
volatile uint8_t Scoreboard::lower_score_byte = 0;
volatile uint8_t Scoreboard::middle_score_byte = 0;
volatile uint8_t Scoreboard::upper_score_byte = 0;
volatile uint8_t* Scoreboard::send_buffer[MESSAGE_LENGTH] = {nullptr};
volatile uint8_t Scoreboard::send_index = 0;
volatile bool Scoreboard::need_to_send = false;
volatile bool Scoreboard::overflow = false;