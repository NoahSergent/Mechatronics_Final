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

#include "masks.h"
#include "scoreboard.h"
#include "top_lanes.h"
#include <avr/interrupt.h>
#include <avr/io.h>

void Setup( void ) {
	// flipper setup
	DDRD |= (1<<PORTD6)| (1<<PORTD5);
    TCCR0A = (1<<COM0A1)|(0<<COM0A0)|(1<<COM0B1)|(0<<COM0B0)|(1<<WGM01)|(1<<WGM00); // Setup Fast PWM Mode for Channel A and Channel B
	TCCR0B = (0<<WGM02)|(0<<CS02)|(1<<CS01)|(0<<CS00); // Define Clock Source Prescaler
	OCR0A = 0; //Initialized PWM Duty Cycle for flipper 0
	OCR0B = 0; //Initialized PWM Duty Cycle for flipper 1

    // SPI
    DDRB = ( 1 << PORTB1 ) | ( 1 << PORTB2 ) | ( 1 << PORTB3 ) | ( 0 << PORTB4 ) | ( 1 << PORTB5 );
    DDRC = ( 1 << PORTC5 );
    //DDRD = (1 << PORTD0);

    PORTB |= ( 1 << PORTB2 );
    PORTB &= ~( 1 << PORTB1 );

    SPCR = ( 1 << SPIE ) | ( 1 << SPE ) | ( 0 << DORD ) | ( 1 << MSTR ) | ( 0 << CPOL ) | ( 0 << CPHA ) | ( 0 << SPR1 ) | ( 0 << SPR0 ); //clock speed f_osc/128 if double is high
    SPSR = ( 1 << SPI2X );                                                                                                               // can read SPIF from bit 7, and WCOL from bit 6
    //SPDR is data register

    // Timer1
    TCCR1A = ( 0 << COM1A1 ) | ( 0 << COM1A0 ) | ( 0 << COM1B1 ) | ( 0 << COM1B0 ) | ( 0 << WGM11 ) | ( 0 << WGM10 );           // CTC mode
    TCCR1B = ( 0 << ICNC1 ) | ( 0 << ICES1 ) | ( 0 << WGM13 ) | ( 1 << WGM12 ) | ( 1 << CS12 ) | ( 0 << CS11 ) | ( 0 << CS10 ); // Set timer 1 to CTC mode and prescaler to 256.
    //TCCR1C = (0<<FOC1A)|(0<<FOC1B);
    // update OSR1A based on longest SPI duration to guarantee no collisions
    OCR1A = 3124; // 3124 -> 0.05 sec //624 -> 0.01 sec // total_CTC_time = (OCR1A+1)*del_t = (OCR1A+1)*n/16MHz		0.1 sec in this case
    TIMSK1 = ( 0 << ICIE1 ) | ( 0 << OCIE1B ) | ( 1 << OCIE1A ) | ( 0 << TOIE1 );
    //TIFR1 = (0<<ICF1)|(0<<OCF1B)|(0<<OCF1A)|(0<<TOV1);

    uint16_t score = 0;
    uint8_t score_update = 0;

    // Newton Pendulum variables
    // CCW = +1, CW = -1, index select is (2-bit old values),(2-bit new values)
    const int16_t encoder_table[16] = {
        0, 1, -1, 255, -1, 0, 255, 1, 1, 255, 0, -1, 255, -1, 1, 0
    };
    volatile int16_t position = 0;
    volatile uint8_t old_channels = 0;
    volatile uint8_t new_channels = 0;
    uint8_t pendulumSwitch = 0;
    uint16_t max_position = 76; // Assumes that 256 options is sufficient, update value after testing
                                //DDRC &= ~(1<<PORTC4) | ~(1<<PORTC3); // Encoder channel pins as input
}

uint8_t input_data;

void debounce( volatile uint8_t noisyData[Bank_Size] );

void pendulum( void );



/* Example use case
					if(CheckFallingEdges(spinner_sm)){
						LED_toggle(spinner_lights);
						score+=10;
					}
				*/


uint8_t previous_falling_edges = 0;
const uint8_t flipper_mask[2] = {0,1};
void UpdateFlipper0();
void UpdateFlipper1();
uint8_t flipper_state0 = 0; //Track the state of the flipper
uint16_t high_count0 = 0; // Track Duration of high power pulse
uint8_t flipper_state1 = 0; //Track the state of the flipper
uint16_t high_count1 = 0; // Track Duration of high power pulse
uint16_t high_count_max = 1;

int main( void ) {
    Setup();
    Scoreboard::configure();
	TopLanes::init();
    sei();
    //uint16_t LEDproportion = 0;
    //const uint16_t totalPulses = 9000; // Measure total pulses on full range of Newton's Pendulum travel  [ensure (num-1) is evenly divisible by 9 to create 8 bins]

    Scoreboard::setScore( 111 );

    while ( 1 ) {
        //PORTC ^= (1<<PORTC5);
        //PORTC ^= (1<<breadcrumb_pin); // Toggle pin C5 for breadcrumb
        // Add score updates
        if ( updateFlag == 1 ) {
            debounce( readSwitches );
            //std::copy(std::begin(SPIoutput[0]),std::end(SPIoutput[Bank_Size]),std::begin(switch_states[0]));

			// Flipper
			Scoreboard::setScore(0);
			// uint16_t temp_score = Scoreboard::getScore();
			// flipper_button0 = CheckSwitchState(flipper_mask0);
			// if (!(flipper_button0 != 0)) {
			// 	temp_score += 1;
			// }
			// flipper_button1 = CheckSwitchState(flipper_mask1);
			// if (!(flipper_button1 != 0)) temp_score +=10000;
			// EOS_switch0 = CheckSwitchState(eos_mask0);
			// if (EOS_switch0 != 0) temp_score += 100;
			// EOS_switch1 = CheckSwitchState(eos_mask1);
			// if (EOS_switch1 != 0) temp_score += 1000;

			// Scoreboard::setScore(temp_score);

			UpdateFlipper0();
			UpdateFlipper1();

            // This for loop sets the outputs equal to the inputs
            for ( int z = 0; z < Bank_Size; z++ ) {
                SPIoutput[z] = ~switch_states[z]; // works with ~readSwitches[z];
                if ( z == 3 ) {
                    *breadcrumb_pin->port ^= ( 1 << breadcrumb_pin->bit ); // breadcrumb_pin // Toggle breadcrumb
                }
            }
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
			// if (previous_falling_edges != falling_edges[1]) Scoreboard::setScore(uint16_t(falling_edges[1]));
            Scoreboard::sendScoreInterrupt();
			// previous_falling_edges = falling_edges[1];
			

            updateFlag = 0;
        }

        pendulum();
    }
}

void debounce( volatile uint8_t noisyData[Bank_Size] ) {
    static uint8_t switchIndex = 0;
    //static uint8_t bankIndex = 0;

    uint8_t stable_high[Bank_Size];
    for ( uint8_t i = 0; i < Bank_Size; ++i ) {
        stable_high[i] = 0xFF;                       //Initialize temporary stable_high all high
        circularBuff[i][switchIndex] = noisyData[i]; // i was bankIndex
    }
    uint8_t stable_low[Bank_Size] = { 0 }; //Initialize temporary stable_low all low

    //Loop through all historical switch samples to check for stable highs and lows
    for ( uint8_t j = 0; j < Bank_Size; ++j ) {
        for ( uint8_t k = 0; k < Buffer_Length; ++k ) {
            //"And" for stable high (all 1's will produce "1" for stable high)
            stable_high[j] &= circularBuff[j][k];
            //"Or" for stable low (all 0's will produce "0" for stable low)
            stable_low[j] |= circularBuff[j][k];
        }

        rising_edges[j] = ( ~switch_states[j] ) & stable_high[j];                 //Detect Rising Edges
        falling_edges[j] = switch_states[j] & ( ~stable_low[j] );                 //Detect Falling Edges
        switch_states[j] = stable_high[j] | ( switch_states[j] & stable_low[j] ); //Update switch states
        //Update sample index and wrap if necessary
        if ( ++switchIndex >= Buffer_Length ) {
            switchIndex = 0; //wrap
        }
    }
}

void pendulum( void ) {
}

void UpdateFlipper0() {

	flipper_button0 = CheckSwitchState(flipper_mask0);
	EOS_switch0 = CheckSwitchState(eos_mask0);
	Scoreboard::setScore(flipper_state0);
	if((flipper_button0 != 0)){//Button Not Pressed
		flipper_state0 = 0; //Flipper at rest
		OCR0A = 0; //Set at 0 power
		//*************************************
	}
	else{//Button Pressed
		switch(flipper_state0) {
			case 0: //New Flip
				flipper_state0 = 1;//set state to high power
				OCR0A = kHit_Power; // Set to flipping power
				high_count0 = 0; // Reset 40ms pulse counter
				break;
			case 1: //High Power Flip
				if(high_count0<=high_count_max){ // Still flipping
					high_count0++;  //increment to record another 0.1ms
				} else { //Flip over, switch to low power holding
					flipper_state0=2; //Update state to low power hold
				}
				break;
			case 2: //Low Power Hold
				if(!(EOS_switch0 != 0)) {  //EOS Switch is closed, flipper is falling
					flipper_state0=1; //Restart another high powered pulse
					high_count0=0;  
					OCR0A = kHit_Power; // Hit Power
				}
				else {
					OCR0A = kHold_Power; // Hold Power
				}
				break;
			default: // state that should never be reached
				flipper_state0 = 0; // Flipper at rest
				OCR0A = 0; // Set power to 0

				}
		}
}

void UpdateFlipper1() {

	flipper_button1 = CheckSwitchState(flipper_mask1);
	EOS_switch1 = CheckSwitchState(eos_mask1);
	Scoreboard::setScore(flipper_state1);
	if((flipper_button1 != 0)){//Button Not Pressed
		flipper_state1 = 0; //Flipper at rest
		OCR0B = 0; //Set at 0 power
		//*************************************
	}
	else{//Button Pressed
		switch(flipper_state1) {
			case 0: //New Flip
				flipper_state1 = 1;//set state to high power
				OCR0B = kHit_Power; // Set to flipping power
				high_count1 = 0; // Reset 40ms pulse counter
				break;
			case 1: //High Power Flip
				if(high_count1<=high_count_max){ // Still flipping
					high_count1++;  //increment to record another 0.1ms
				} else { //Flip over, switch to low power holding
					flipper_state1=2; //Update state to low power hold
				}
				break;
			case 2: //Low Power Hold
				if(!(EOS_switch1 != 0)) {  //EOS Switch is closed, flipper is falling
					flipper_state1=1; //Restart another high powered pulse
					high_count1=0;  
					OCR0B = kHit_Power; // Hit Power
				}
				else {
					OCR0B = kHold_Power; // Hold Power
				}
				break;
			default: // state that should never be reached
				flipper_state1 = 0; // Flipper at rest
				OCR0B = 0; // Set power to 0

				}
		}
}

ISR( TIMER1_COMPA_vect ) {
    //PORTC ^= (1<<breadcrumb_pin); // Toggle breadcrumb pin

    *switch_latch_pin->port &= ~( 1 << switch_latch_pin->bit ); // Falling edge
    *switch_latch_pin->port |= ( 1 << switch_latch_pin->bit );  // Rising edge
    SPDR = SPIoutput[LEDcount];                                 // SPIoutput[LEDcount];//input_data;//Start Serial Transfer
}

ISR( SPI_STC_vect ) { // SPI Serial Transfer Complete
    //PORTC ^= (1<<breadcrumb_pin); // Toggle breadcrumb pin when entering SPI ISR

    readSwitches[LEDcount] = SPDR;  //Store received SPI data.
    if ( ++LEDcount < Bank_Size ) { // update LEDcount
        SPDR = SPIoutput[LEDcount];
    } else { // LEDcount >= Bank_Size	Keep in mind uint8_t limit on LEDcount
        //Strobe RCK to SPI transferred data into output register
        *LED_latch_pin->port |= ( 1 << LED_latch_pin->bit );  //Rising edge of low pulse
        *LED_latch_pin->port &= ~( 1 << LED_latch_pin->bit ); //Latch Serial Output Data
        // add dummy loop delay here if needed
        PORTB &= ~( 1 << PORTB1 ); //Latch Serial Output Data
        LEDcount = 0;
        updateFlag = 1;
    }
}

ISR( USART_TX_vect ) {
    Scoreboard::interruptStateMachine();
}