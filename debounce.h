// /*
//  * debounce.h
//  *
//  * 
//  */

// #ifndef DEBOUNCE_H
// #define DEBOUNCE_H

// #include <avr/io.h>
// #include 

// void debounce(volatile uint8_t noisyData[Bank_Size]) {
// 	static uint8_t switchIndex = 0;
// 	//static uint8_t bankIndex = 0;
	
// 	uint8_t stable_high[Bank_Size];
// 	for (uint8_t i=0; i<Bank_Size; i++){
// 		stable_high[i] = 0xFF; //Initialize temporary stable_high all high
// 		circularBuff[i][switchIndex] = noisyData[i]; // i was bankIndex
// 	}
// 	uint8_t stable_low[Bank_Size] = {0}; //Initialize temporary stable_low all low
	
	
	
// 	//Loop through all historical switch samples to check for stable highs and lows
// 	for (uint8_t j=0; j<Bank_Size; j++){
// 		for (uint8_t k=0; k<Buffer_Length; k++){
// 			//"And" for stable high (all 1's will produce "1" for stable high)
// 			stable_high[j] &= circularBuff[j][k];
// 			//"Or" for stable low (all 0's will produce "0" for stable low)
// 			stable_low[j] |= circularBuff[j][k];
// 		}
	
// 		rising_edges[j] = (~switch_states[j]) & stable_high[j];//Detect Rising Edges
// 		falling_edges[j] = switch_states[j] & (~stable_low[j]);//Detect Falling Edges
// 		switch_states[j] = stable_high[j] | (switch_states[j] & stable_low[j]);//Update switch states
// 		//Update sample index and wrap if necessary
// 		if(++switchIndex>=Buffer_Length){
// 			switchIndex = 0;//wrap
// 		}
// 	}
// }


// // How to pull data from debounced switch inputs	(place in better location for easy access for everyone)
// uint8_t CheckSwitchState(uint8_t switch_mask[2]) {
//     return switch_states[switch_mask[0]] & switch_mask[1];
// }
// uint8_t CheckFallingEdges(uint8_t switch_mask[2]) {
//     return falling_edges[switch_mask[0]] & switch_mask[1];
// }
// uint8_t CheckRisingEdges(uint8_t switch_mask[2]){
//     return rising_edges[switch_mask[0]] & switch_mask[1]; //may need to be reversed
// }


// #endif