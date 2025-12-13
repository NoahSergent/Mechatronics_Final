/*
 * Lab 5 - SPI Bus Port Expansion.cpp
 *
 * Created: 10/28/2025 9:01:51 PM
 * Author : Griffin Ramsey & Noah Sergent
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>

#define Buffer_Length 2 // Modify by checking with scope for debounce time in switches - adjust with Timer1 clock speed
#define Bank_Size 3 // Modify if we ever use more or less than three banks of switches

void Setup(void){
	
	
	// Switch Masks
	//global switch input masks - redefine after checking switch location on pinball machine
	uint8_t spinner_sm[2] = {0,0x02};
	uint8_t hurry_up[2] = {0,0x80};
	uint8_t left_lane_sm[2] = {1,0x01};
	uint8_t right_lane_sm[2] = {1,0x02};

	// global LED output mask - single LED each - redefine after checking LED locations on pinball machine (include current limiting resistors)
	uint8_t ramp_lights[2] = {0,0x01};
	uint8_t drop_bank_lights[2] = {0,0x01};
	uint8_t left_lane_upper_lights[2] = {0,0x01};
	uint8_t right_lane_upper_lights[2] = {0,0x01};
	uint8_t left_lane_lower_lights[2] = {0,0x01};
	uint8_t right_lane_lower_lights[2] = {0,0x01};
	uint8_t orbit_lights[2] = {0,0x01};
	uint8_t spinner_lights[2] = {0,0x01};
	uint8_t newton_lights[2] = {2,0xFF}; // This requires 8 LEDs, so a full port, assuming port 3
	uint8_t standup_lights[2] = {0,0x01};
	
	
	
	
	
	
	
	// SPI
	DDRB = (1<<PORTB1)|(1<<PORTB2)|(1<<PORTB3)|(0<<PORTB4)|(1<<PORTB5);
	DDRC = (1<<PORTC5);
	//DDRD = (1 << PORTD0);
	
	PORTB |= (1 << PORTB2);
	PORTB &= ~(1 << PORTB1);
	
	SPCR = (1<<SPIE)|(1<<SPE)|(0<<DORD)|(1<<MSTR)|(0<<CPOL)|(0<<CPHA)|(0<<SPR1)|(0<<SPR0); //clock speed f_osc/128 if double is high
	SPSR = (1 << SPI2X); // can read SPIF from bit 7, and WCOL from bit 6
	//SPDR is data register
	
	
	uint8_t switch_latch_pin = PORTB2; // PORTB
	uint8_t LED_latch_pin = PORTB1; // PORTB
	
	
	// Breadcrumb
	uint8_t breadcrumb_pin = PORTC5;
	
	
	// Timer1
	TCCR1A = (0<<COM1A1)|(0<<COM1A0)|(0<<COM1B1)|(0<<COM1B0)|(0<<WGM11)|(0<<WGM10); // CTC mode
	TCCR1B = (0<<ICNC1)|(0<<ICES1)|(0<<WGM13)|(1<<WGM12)|(1<<CS12)|(0<<CS11)|(0<<CS10); // Set timer 1 to CTC mode and prescaler to 256.
	//TCCR1C = (0<<FOC1A)|(0<<FOC1B);
	// update OSR1A based on longest SPI duration to guarantee no collisions
	OCR1A = 3124; // 3124 -> 0.05 sec //624 -> 0.01 sec // total_CTC_time = (OCR1A+1)*del_t = (OCR1A+1)*n/16MHz		0.1 sec in this case
	TIMSK1 = (0<<ICIE1)|(0<<OCIE1B)|(1<<OCIE1A)|(0<<TOIE1);
	//TIFR1 = (0<<ICF1)|(0<<OCF1B)|(0<<OCF1A)|(0<<TOV1);
	
	
	
	
	
	
	
	// Pendulum
	uint8_t opticalEncoder_A_pin = PORTC3;
	uint8_t opticalEncoder_B_pin = PORTC4;

	uint16_t score = 0;
	uint8_t score_update = 0;
	
	// Newton Pendulum variables
	// CCW = +1, CW = -1, index select is (2-bit old values),(2-bit new values)
	const int16_t encoder_table[16] = {
		0,1,-1,255,
		-1,0,255,1,
		1,255,0,-1,
	255,-1,1,0};
	volatile int16_t position = 0;
	volatile uint8_t old_channels = 0;
	volatile uint8_t new_channels = 0;
	uint8_t pendulumSwitch = 0;
	uint16_t max_position = 76;	// Assumes that 256 options is sufficient, update value after testing
	//DDRC &= ~(1<<PORTC4) | ~(1<<PORTC3); // Encoder channel pins as input
	
	
}



uint8_t input_data;


// SPI Global
volatile uint8_t SPIoutput[Bank_Size] = {0}; // LED data
volatile uint8_t LEDcount = 0;
volatile uint8_t readSwitches[Bank_Size] = {0}; // Bank_Size is 3 by default
volatile uint8_t updateFlag = 0; // SPI full Bank_Index data received

// Debounce Global
volatile uint8_t noisySwitchSet[Bank_Size][Buffer_Length] = {0}; // sets all inputs to 0 initially
volatile uint8_t circularBuff[Bank_Size][Buffer_Length] = {0};
volatile uint8_t rising_edges[Bank_Size] = {0};
volatile uint8_t falling_edges[Bank_Size] = {0};
volatile uint8_t switch_states[Bank_Size] = {0};
void debounce(volatile uint8_t noisyData[Bank_Size]);


void pendulum(void);



/*
			// How to pull data from debounced switch inputs	(place in better location for easy access for everyone)
				uint8_t CheckSwitchState(uint8_t switch_mask[2]) {
					return switch_states[switch_mask[0]] & switch_mask[1];
				}
				uint8_t CheckFallingEdges(uint8_t switch_mask[2]) {
					return falling_edges[switch_mask[0]] & switch_mask[1];
				}
				uint8_t CheckRisingEdges(uint8_t switch_mask[2]){
					return rising_edges[switch_mask[0]] & switch_mask[1]; //may need to be reversed
				}
	

				// How to toggle LED *** Check that this will behave as expected
				uint8_t LED_toggle (uint8_t LED_mask[2]) {
					return SPIoutput[LED_mask[0]] ^= (SPIoutput[LED_mask[0]] & LED_mask[1]); // Toggles LED
				}
				uint8_t LED_on (uint8_t LED_mask[2]) {
					return SPIoutput[LED_mask[0]] |= (SPIoutput[LED_mask[0]] & LED_mask[1]); // Turns on LED
				}
				uint8_t LED_off (uint8_t LED_mask[2]) {
					return SPIoutput[LED_mask[0]] &= ~(SPIoutput[LED_mask[0]] & LED_mask[1]); // Turns off LED
				}
				uint8_t LED_set (uint8_t LED_mask[2], uint8_t bit_pattern) { //for newton's pendulum
					return SPIoutput[LED_mask[0]] = bit_pattern & (SPIoutput[LED_mask[0]] & LED_mask[1]); // Sets LEDs in groups
				}


				/* Example use case
					if(CheckFallingEdges(spinner_sm)){
						LED_toggle(spinner_lights);
						score+=10;
					}
				*/


int main(void)
{
	Setup();
	sei();
		
	while(1){	
		if (updateFlag==1){
			debounce(readSwitches);
			//std::copy(std::begin(SPIoutput[0]),std::end(SPIoutput[Bank_Size]),std::begin(switch_states[0]));
			for (int z=0; z<Bank_Size; z++){
				SPIoutput[z] = ~switch_states[z]; // works with ~readSwitches[z];
				if (z==3){
					PORTC ^= (1<<PORTC5); // breadcrumb_pin // Toggle breadcrumb
				}
			}
			
			// Update score here
			
			updateFlag=0;
		}
		
		pendulum();
		
		
		
	}
}


void debounce(volatile uint8_t noisyData[Bank_Size]) {
	static uint8_t switchIndex = 0;
	//static uint8_t bankIndex = 0;
	
	uint8_t stable_high[Bank_Size];
	for (uint8_t i=0; i<Bank_Size; ++i){
		stable_high[i] = 0xFF; //Initialize temporary stable_high all high
		circularBuff[i][switchIndex] = noisyData[i]; // i was bankIndex
	}
	uint8_t stable_low[Bank_Size] = {0}; //Initialize temporary stable_low all low
	
	
	
	//Loop through all historical switch samples to check for stable highs and lows
	for (uint8_t j=0; j<Bank_Size; ++j){
		for (uint8_t k=0; k<Buffer_Length; ++k){
			//"And" for stable high (all 1's will produce "1" for stable high)
			stable_high[j] &= circularBuff[j][k];
			//"Or" for stable low (all 0's will produce "0" for stable low)
			stable_low[j] |= circularBuff[j][k];
		}
		
		rising_edges[j] = (~switch_states[j]) & stable_high[j];//Detect Rising Edges
		falling_edges[j] = switch_states[j] & (~stable_low[j]);//Detect Falling Edges
		switch_states[j] = stable_high[j] | (switch_states[j] & stable_low[j]);//Update switch states
		//Update sample index and wrap if necessary
		if(++switchIndex>=Buffer_Length){
			switchIndex = 0;//wrap
		}
	}
}



void pendulum(void) {
	
}

ISR(TIMER1_COMPA_vect){
	//PORTC ^= (1<<breadcrumb_pin); // Toggle breadcrumb pin
	
	PORTB &= ~(1<<PORTB2); // Falling edge
	PORTB |= (1<<PORTB2); //Latch Parallel Data IN
	SPDR = SPIoutput[LEDcount];// SPIoutput[LEDcount];//input_data;//Start Serial Transfer
}

ISR(SPI_STC_vect) {	// SPI Serial Transfer Complete
	//PORTC ^= (1<<breadcrumb_pin); // Toggle breadcrumb pin when entering SPI ISR

	readSwitches[LEDcount] = SPDR; //Store received SPI data.
	if (++LEDcount<Bank_Size) { // update LEDcount
		SPDR = SPIoutput[LEDcount];
	} else { // LEDcount >= Bank_Size	Keep in mind uint8_t limit on LEDcount
		//Strobe RCK to SPI transferred data into output register
		PORTB |= (1<<PORTB1); //Rising edge of low pulse
		// add dummy loop delay here if needed
		PORTB &= ~(1<<PORTB1);//Latch Serial Output Data
		LEDcount=0;
		updateFlag = 1;
	}
}