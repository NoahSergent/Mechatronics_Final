/*
 * Final_Project.cpp
 *
 * Created: 12/2/2025 1:54:58 PM
 * Author : Griffin
 */ 

/*
Sections to complete
	I/O SPI Port Expansion
		You will utilize six shift registers to expand the I/O capabilities of your microcontroller. You will
		use three parallel-input/serial-output shift registers to provide 16 digital inputs to your Arduino
		board. These shift registers are configured in series so that data can be serially shifted through both
		registers with three consecutive SPI transfers. Similarly, you will utilize three serial- input/parallel-
		output shift registers to provide 16 digital outputs. You will use these outputs to control lights on
		the pinball playfield. This will be nearly identical to the SPI lab, except that you will use additional
		shift registers. The datasheets for the shift registers are uploaded to canvas. You will need to
		identify which pins on the microcontroller to use for control signals, and you will need to ensure
		that you provide the correct rising/falling edges for the latch signals. Additionally, you will now be
		interfacing additional shift registers that are daisy chained together, which will require additional
		SPI transfers to shift all of the data through the system. Lastly, you will need to debounce the
		sampled switch values. This can be achieved by utilizing the debounce routine you previously
		developed in Lab 3. Note however, the current function can debounce eight (8) switch inputs at a
		time. You will need to modify your code to debounce twenty four (24) switch inputs.
		
		Switch IC inputs
			SH/LD -		
			CLK INH -	
			CLK -		SCK
			SER -		DAISY CHAINED (Q_h OUTPUT)
		Above is already completed in the daughterboard	
		Needed Arduino connections
			- GND (next to pin 13)
			- SCK (PB5, pin 13)
			- MISO (PB4, pin 12)
			- MOSI (PB3, pin 11)
			- SS (output, no required connection - used for other outputs)
			- L_I (input latch, up to determination)
			- L_O (output latch, up to determination)
		
	I/O Port Debouncing
		The two parallel-input/serial-output shift registers will be connected to switches on the pinball
		playfield. Of these twenty four inputs (16) must be debounced. You should use the state machine
		method developed in Lab 3. Note however, the current function can debounce eight (8) switch
		inputs at a time. You will need to modify your code to debounce twenty four (24) switch inputs.
		
	Newton Pendulum Target
		The pendulum target located in the center of the playfield utilizes a quadrature encoder to measure
		the rotation of the pendulum. There is also a light bar consisting of eight LEDs mounted on the target
		which are controlled via the output ports of the shift registers. The ball in the front of the target is
		bolted through the playfield. Whenever the free rolling ball strikes the static ball, momentum is
		transferred through the static ball into the pendulum causing it to rotate. The quadrature encoder
		can then be used to measure that rotation. In order to receive the full 20 points available, you must
		interface with the quadrature encoder to measure the motion of the pendulum. In particular, you
		must be able to detect the direction of rotation. To demonstrate the ability to detect both the motion
		and direction, you must develop code that lights up the LED bar as the pendulum rotates. For
		example, when the pendulum is at rest, no LEDs are lit. As the pendulum rotates up, the LEDs
		begin to light up from the bottom. When the pendulum is halfway up, half of the LEDs should be
		lit. When the pendulum is fully rotated, all of the LEDs should be lit. As the pendulum rotates back
		down, the LEDs should reverse the behavior. The encoder interface code should be placed in the
		the main while(1) loop. You should measure and update the encoder state in the main loop instead
		of the Timer 1 interrupt because it requires very fast sampling in order to ensure no missed edges.
		
		
		Additional tips
		Code flow - use timer 1 as heartbeat of code, it should start the SPI transfer. (Timer 0 for flipper control)
		Timer 1 - starts SPI transfer
		SPI Interrupt sends remaining two packets to daisy chained components (state machine)
		After all three transfers occur, debounce the switch inputs (should not be in the ISR state machine)
		After debounce is complete, update rules (if that optional objective is selected) then send output signals to mechanisms
		Finally, start USART to update scoreboard (perhaps trigger USART flag to start that ISR? No need, just list that directly.)
			Ignore the following - after working through it the code flow makes sense
			Come back to this later, it seems to differ from the recommendations - but it should be faster
				Not really, we can be working with each batch of switch data at a time, this way 
				Still, there is a possible inefficiency with looping through all debounce sets before we get all sets of SPI data
		
		Only exception is the optical encoder, which is in the main while(1) loop for speed
		
		I/O port expansion
		Switch values can be saved in array form (array of arrays), or as individual variables - former recommended
			array form, three sets of 'switch_states', three sets of 'falling_edges', three sets of 'rising_edges'
				set up a global list of which switch and LED value corresponds to each I/O (matrix value) - useful for team
				This is a defined switch mask (matrix elements are 'bank index' and 'pin number')
		Setup similar array form, port bank, and pin numbers for the LEDs - to be defined as we identify the LED mapping from the pinball machine
		
		Plan
		setup timer 1
			determine appropriate clock speed (relatively fast, but should not be faster than SPI transfers - could use scope to find timing, then decrease time to match maximum)
			(slow baud rate for better communication) - but increase if needed to improve timer 1 frequency
		For SPI to send LED and switch details, the debounce should be setup in 8-bit multiples
		Implement SPI protocol
			send LED transfer (part of timer 1 ISR)
			then receive first switches, and send second LED
			then receive second switches, and send third LED
			then receive third switches, and move to debounce routine/back to main while(1) loop
			
		Check the appropriate rising/falling latch signals for inputs and output chips - same input chip, different LED chip
			switch chip wants a falling edge to load data
			LED chip wants a rising edge to latch data
		phase and polarity need to be setup properly
			data is clocked on the rising edge of CLK signal
			data is setup on the falling edge of CLK signal
			Mode 0	CPOL=0, CPHA=0
		Transmission speed (unless we need it to be faster) f_osc/128	125000 Hz
			SPI2X=0, SPR1=1, SPR0=1
		Program should wait until SPI transfer is complete, one method to poll SPIF flag in SPSR register - not ideal for our use case due to speed
		
		******* Set current limiting resistors for LEDs, 10mA per pin target (include calculation) ********
		
		******* Need to make a full system diagram - see what online tools the team wants to use for this *******
*/

#include <avr/io.h>
#include <avr/interrupt.h>

//global switch input masks - redefine after checking switch location on pinball machine
extern uint8_t spinner_sm[2] = {0,0x02};
extern uint8_t hurry_up[2] = {0,0x80};
extern uint8_t left_lane_sm[2] = {1,0x01};
extern uint8_t right_lane_sm[2] = {1,0x02};

// global LED output mask - single LED each - redefine after checking LED locations on pinball machine (include current limiting resistors)
extern uint8_t ramp_lights[2] = {0,0x01};
extern uint8_t drop_bank_lights[2] = {0,0x01};
extern uint8_t left_lane_upper_lights[2] = {0,0x01};
extern uint8_t right_lane_upper_lights[2] = {0,0x01};
extern uint8_t left_lane_lower_lights[2] = {0,0x01};
extern uint8_t right_lane_lower_lights[2] = {0,0x01};
extern uint8_t orbit_lights[2] = {0,0x01};
extern uint8_t spinner_lights[2] = {0,0x01};
extern uint8_t newton_lights[2] = {2,0xFF}; // This requires 8 LEDs, so a full port, assuming port 3
extern uint8_t standup_lights[2] = {0,0x01};

// Breadcrumb I/O
extern uint8_t breadcrumb_pin[2] = {PORTC, PORTC5};

// SPI Latch I/O
extern uint8_t switch_latch_pin[2] = {PORTB, PORTB2};
extern uint8_t LED_latch_pin[2] = {PORTB, PORTB1};

// Newtons Pendulum I/O
extern uint8_t opticalEncoder_A_pin[2] = {PORTC, PORTC3};
extern uint8_t opticalEncoder_B_pin[2] = {PORTC, PORTC4};

extern uint16_t score = 0;
extern uint8_t score_update = 0;





//void setupTimer1(void);
void setupTimer1(void){
	// Set Timer 1 speed fast enough to capture switch events (including debounce time), and fast enough that the LEDs do not appear to flicker
	// Set Timer 1 speed slow enough to not cause data collisions with SPI state machine (we can increase SPI clock speed if needed)
	// Current SPI speed is 125 kHz, need to measure total duration of SPI transfer
	
	TCCR1A = (0<<COM1A1)|(0<<COM1A0)|(0<<COM1B1)|(0<<COM1B0)|(0<<WGM11)|(0<<WGM10); // CTC mode
	TCCR1B = (0<<ICNC1)|(0<<ICES1)|(0<<WGM13)|(1<<WGM12)|(1<<CS12)|(0<<CS11)|(0<<CS10); // Set timer 1 to CTC mode and prescaler to 256.
	//TCCR1C = (0<<FOC1A)|(0<<FOC1B);
	// update OSR1A based on longest SPI duration to guarantee no collisions
	OCR1A = 6249;  // total_CTC_time = (OCR1A+1)*del_t = (OCR1A+1)*n/16MHz		0.1 sec in this case
	TIMSK1 = (0<<ICIE1)|(0<<OCIE1B)|(1<<OCIE1A)|(0<<TOIE1);
	//TIFR1 = (0<<ICF1)|(0<<OCF1B)|(0<<OCF1A)|(0<<TOV1);
}


//void setupSPI(void);
void setupSPI(void) // Setup SPI Pins
{
	DDRB = (1<<PORTB2)|(1<<PORTB3)|(0<<PORTB4)|(1<<PORTB5); //sets up SPI pins
	DDRB |= (1<<PORTB2); // Switch load latch
	DDRB |= (1<<PORTB1); // LED latch
	
	//PORTB |= (1<<PORTB2);
	PORTB &= ~(1<<PORTB2); // Sets switch latch pin low, requires output to avoid SS conflict
	PORTB &= ~(1<<PORTB1); // Sets LED latch pin low

	SPCR = (1<<SPIE)|(1<<SPE)|(0<<DORD)|(1<<MSTR)|(0<<CPOL)|(0<<CPHA)|(1<<SPR1)|(1<<SPR0);
	//clock speed (f_osc/128) if double is low, slowest possible speed for chip (125kHz update)
	SPSR = (0 << SPI2X); // can read SPIF from bit 7, and WCOL from bit 6
	//SPDR is data register
}

// Debounce Setup for 24 pins total, three sets of 8-bit
extern const uint8_t Buffer_Length = 6; // Modify by checking with scope for debounce time in switches - adjust with Timer1 clock speed
extern const uint8_t Bank_Size = 3; // Modify if we ever use more or less than three banks of switches

volatile extern uint8_t noisySwitchSet[Bank_Size][Buffer_Length] = {0}; // sets all inputs to 0 initially
	
//set the following as volatile if they do not populate correctly, use breadcrumbs as needed
extern uint8_t circularBuff[Bank_Size][Buffer_Length] = {0};
extern uint8_t rising_edges[Bank_Size] = {0};
extern uint8_t falling_edges[Bank_Size] = {0};
extern uint8_t switch_states[Bank_Size] = {0};


// SPI data variable
extern volatile uint8_t SPIoutput[3] = {0}; // LED data
extern volatile uint8_t LEDcount = 0;
extern volatile uint8_t readSwitches[Bank_Size] = {0}; // Bank_Size is 3 by default
extern volatile uint8_t updateFlag = 0; // SPI full Bank_Index data received


// Newton Pendulum variables
// CCW = +1, CW = -1, index select is (2-bit old values),(2-bit new values)
extern const int16_t encoder_table[16] = {
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

// How to pull data from debounced switch inputs	(place in better location for easy access for everyone)
extern uint8_t CheckSwitchState(uint8_t switch_mask[2]) {
	return switch_states[switch_mask[0]] & switch_mask[1];
}
extern uint8_t CheckFallingEdges(uint8_t switch_mask[2]) {
	return falling_edges[switch_mask[0]] & switch_mask[1];
}
extern uint8_t CheckRisingEdges(uint8_t switch_mask[2]){
	return rising_edges[switch_mask[0]] & switch_mask[1]; //may need to be reversed
}
	

// How to toggle LED *** Check that this will behave as expected
extern uint8_t LED_toggle (uint8_t LED_mask[2]) {
	SPIoutput[LED_mask[0]] ^= (SPIoutput[LED_mask[0]] & LED_mask[1]); // Toggles LED
}
extern uint8_t LED_on (uint8_t LED_mask[2]) {
	SPIoutput[LED_mask[0]] |= (SPIoutput[LED_mask[0]] & LED_mask[1]); // Turns on LED
}
extern uint8_t LED_off (uint8_t LED_mask[2]) {
	SPIoutput[LED_mask[0]] &= ~(SPIoutput[LED_mask[0]] & LED_mask[1]); // Turns off LED
}
extern uint8_t LED_set (uint8_t LED_mask[2], uint8_t bit_pattern) { //for newton's pendulum
	SPIoutput[LED_mask[0]] = bit_pattern & (SPIoutput[LED_mask[0]] & LED_mask[1]); // Sets LEDs in groups
}


/* Example use case
	if(CheckFallingEdges(spinner_sm)){
		LED_toggle(spinner_lights);
		score+=10;
	}
*/


//void debounce(volatile uint8_t noisyData[Bank_Size]);
void debounce(volatile uint8_t noisyData[Bank_Size]) {
	static uint8_t switchIndex = 0;
	//static uint8_t bankIndex = 0;
	
	uint8_t stable_high[Bank_Size];
	for (uint8_t i=0; i<Bank_Size; i++){
		stable_high[i] = 0xFF; //Initialize temporary stable_high all high
		circularBuff[i][switchIndex] = noisyData[i]; // i was bankIndex
	}
	uint8_t stable_low[Bank_Size] = {0}; //Initialize temporary stable_low all low
	
	
	
	//Loop through all historical switch samples to check for stable highs and lows
	for (uint8_t j=0; j<Bank_Size; j++){
		for (uint8_t k=0; k<Buffer_Length; k++){
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




int main(void)
{
	
	setupTimer1();
	setupSPI();
	sei();
	//uint16_t LEDproportion = 0;
	//const uint16_t totalPulses = 9000; // Measure total pulses on full range of Newton's Pendulum travel  [ensure (num-1) is evenly divisible by 9 to create 8 bins]
	
    while (1) {
		// Add score updates
		if(updateFlag>=1){
			debounce(readSwitches);
			// Add mechanism calls here (include pinball board LED changes)
			// Flipper_Control();
			// Ball_Launch();
			// Spinner();
			// Top_Lanes();
			// Hurry-up_Targets();
			// Drop_Bank_Targets();
			// Ramp(); // Optional, if we need the points
			
			/* Example use case
				if(CheckFallingEdges(spinner_sm)){
					LED_toggle(spinner_lights);
					score+=10;
					
				}
			*/	
					// SPI for loop, delete if not used
					//for (uint8_t n=0, n<Bank_Size, n++){
					//}
			
			// Update rules if additional points are needed
			// when something happens, begin counting down score, if it hits zero (underflow triggers ISR) drive both flipper solenoids high regardless of user input - or deactivate flippers entirely / turn flipper high PWM to 10%
				// To increase the pressure, change Flipper hold value PWM to 100% to burn out solenoids! mhuaa hahaha... just kidding
			
			// Add scoreboard update here
			
			updateFlag = 0; // Resets debounce checker for improved speed
		}
		
		// Add optical encoder code here
		// Not debouncing due to optical encoder's lack of jitter & use of Grey codes
		new_channels = (opticalEncoder_A_pin[0] & (opticalEncoder_A_pin[1]|opticalEncoder_B_pin[1])) >> 3; // assumes channel A and channel B are next to one another on a port. Also the pin order affects ccw/cw direction.
		
		uint8_t encoderLUTindex = old_channels | new_channels;
		int16_t direction = encoder_table[encoderLUTindex];
		
		if(direction==255) { //Check for error
			//breadcrumb_pin[1] ^= breadcrumb_pin[2]; // Error is occurring
			position +=0;
		}else if (direction>0){
			// CCW direction - LEDs should be increasing
			position += direction; //Update position value
			// divide range of encoder to calculate number of LEDs active (assume 90 degree travel range)
			//LEDproportion = (position * 8) / totalPulses; // multiply by 8 LEDs, then divide
			//LED_on(newton_lights)
		}else if(direction>0){
			// CW direction - LEDs should be decreasing
			position += direction; //Update position value
			
		}else{
			// No change (error if direction is not 0
			position += direction;
		}
				
		//Shift and save current channels as old for next time
		old_channels = new_channels<<2;
		
		pendulumSwitch = max_position / 9;	// 8-bit integer division from 16-bit number (overflow potential)
		
		switch(pendulumSwitch) {
			case 0: // LEDs off
			LED_set(newton_lights,pendulumSwitch);
			score_update = 0;
			break;
			case 1:
			//LED_set(newton_lights,pendulumSwitch);
			SPIoutput[newton_lights[0]] = pendulumSwitch & (SPIoutput[newton_lights[0]] & newton_lights[1]);
			if (score_update==pendulumSwitch){
				score += pendulumSwitch;
			} else {
				score_update = pendulumSwitch;
			}
			break;
			case 2:
			//LED_set(newton_lights,pendulumSwitch);
			SPIoutput[newton_lights[0]] = pendulumSwitch & (SPIoutput[newton_lights[0]] & newton_lights[1]);
			if (score_update==pendulumSwitch){
				score += pendulumSwitch;
				} else {
				score_update = pendulumSwitch;
			}
			break;
			case 3:
			//LED_set(newton_lights,pendulumSwitch);
			SPIoutput[newton_lights[0]] = pendulumSwitch & (SPIoutput[newton_lights[0]] & newton_lights[1]);
			if (score_update==pendulumSwitch){
				score += pendulumSwitch;
				} else {
				score_update = pendulumSwitch;
			}
			break;
			case 4:
			//LED_set(newton_lights,pendulumSwitch);
			SPIoutput[newton_lights[0]] = pendulumSwitch & (SPIoutput[newton_lights[0]] & newton_lights[1]);
			if (score_update==pendulumSwitch){
				score += pendulumSwitch;
				} else {
				score_update = pendulumSwitch;
			}
			break;
			case 5:
			//LED_set(newton_lights,pendulumSwitch);
			SPIoutput[newton_lights[0]] = pendulumSwitch & (SPIoutput[newton_lights[0]] & newton_lights[1]);
			if (score_update==pendulumSwitch){
				score += pendulumSwitch;
				} else {
				score_update = pendulumSwitch;
			}
			break;
			case 6:
			//LED_set(newton_lights,pendulumSwitch);
			SPIoutput[newton_lights[0]] = pendulumSwitch & (SPIoutput[newton_lights[0]] & newton_lights[1]);
			if (score_update==pendulumSwitch){
				score += pendulumSwitch;
				} else {
				score_update = pendulumSwitch;
			}
			break;
			case 7:
			//LED_set(newton_lights,pendulumSwitch);
			SPIoutput[newton_lights[0]] = pendulumSwitch & (SPIoutput[newton_lights[0]] & newton_lights[1]);
			if (score_update==pendulumSwitch){
				score += pendulumSwitch;
				} else {
				score_update = pendulumSwitch;
			}
			break;
		}
		
		
// 		if(CheckFallingEdges(spinner_sm)){
// 			LED_on(newton_lights);
// 			score+=10;
// 		}
		
		// Add any & only speed-critical code here
    }
}




ISR(TIMER1_COMPA_vect){
	// Alternative option to CTC mode, is to trigger an internal interrupt when the scoreboard is updated - needs to be started somehow
	
	// Modified Lab 3 code
	//Latch parallel switch data into shift register
	switch_latch_pin[1] &= ~(1<<switch_latch_pin[2]); // Falling edge
	// *** add dummy loop delay here if needed, only after reducing SPI clock frequency
	switch_latch_pin[1] |= (1<<switch_latch_pin[2]); //Latch Parallel Data IN
	
	
	SPDR = SPIoutput[LEDcount]; //Start SPI Serial Transfer, farthest LED shift register data being sent
	
}


ISR(SPI_STC_vect){	// SPI Serial Transfer Complete
	breadcrumb_pin[1] ^= breadcrumb_pin[2]; // Toggle breadcrumb pin when entering SPI ISR
	
	readSwitches[LEDcount] = SPDR; // Store the received SPI data
	if(++LEDcount>=Bank_Size){ // increment LEDcoutn
		//Strobe RCK to SPI transferred data into output shift register
		LED_latch_pin[1] |= (1<<LED_latch_pin[2]); //Rising edge of low pulse
		
		LEDcount = 0;
		updateFlag = 1; // trigger debounce in main loop
		
		// *** add dummy loop delay here if needed, only after reducing SPI clock frequency
		LED_latch_pin[1] &= ~(1<<LED_latch_pin[2]); //Latch Serial Output Data
	}
	SPDR = SPIoutput[LEDcount]; // Starts next SPI transfer
}