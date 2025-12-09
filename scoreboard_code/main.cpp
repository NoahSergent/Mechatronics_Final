/*
 * UARTLabTemplate.cpp
 *
 * Created: 10/20/2021 9:38:41 AM
 * Author : Fuchs
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>

const uint16_t SCOREBOARD_ADDRESS =  0x10;

volatile uint16_t score=0;
volatile uint16_t display_LED=0;
volatile uint8_t USART_state=0;


//This is generic displayboard code

int main(void)
{
 //Configure Pins
 //DDRB = (1<<PORTB0)|(1<<PORTB1)|(1<<PORTB2);
 DDRC = (1<<PORTC5)|(1<<PORTC4)|(1<<PORTC3)|(1<<PORTC2)|(1<<PORTC1)|(1<<PORTC0);
 DDRD = (1<<PORTD2)|(1<<PORTD3)|(1<<PORTD4)|(1<<PORTD6)|(1<<PORTD7);
 //*****************************************************************************
 //Configure Timer 1
 //*****************************************************************************
 //Run in CTC (Clear Timer on Compare)
 TCCR1A = (0<<COM1A1)|(0<<COM1A0)|(0<<WGM11)|(0<<WGM10);
 //CTC Mode, CLK/256  
 TCCR1B = (0<<WGM13)|(1<<WGM12)|(1<<CS12)|(0<<CS11)|(0<<CS10);
 TIMSK1 = (1<<OCIE1A);
 OCR1A = 62;//  (1s)*(16 Mhz/256)

 //**************************************************************************************
 //Configure USART
 //**************************************************************************************
 //Write your configuration code here

 //Enable all interrupts
 sei();
    while(1)
    {
        //Do Nothing
    }
}



ISR(TIMER1_COMPA_vect)
{
 static uint8_t digit_index = 0; //Index for tracking digit activation
 static uint8_t display_value = 0;
 static uint16_t count_index = 0;
 static uint8_t debounced_state = 0;
 static uint16_t accumulate = 0;


 //Turn on digit
 PORTD = (PORTD & 0b11100011)|(digit_index<<2);
 //PORTD = 0b11100011|(digit_index<<2);

 //Calculate the digit value
 switch (digit_index) {
  case 4: display_value = score % 10;break;//Ones
  case 3: display_value = (score/10) % 10;break;//Tens
  case 2: display_value = (score/100) % 10;break;//Hundreds
  case 1: display_value = (score/1000) % 10;break;//Thousands
  case 0: display_value = (score/10000) % 10;break;//Ten Thousands
 }
 
 //Decode segment values
 switch (display_value) {
  case 0:
    PORTD = (PORTD&0x7F)|0x00;
    PORTC = (PORTC&0xC0)|0x3F; break;
  case 1:
    PORTD = (PORTD&0x7F)|0x00;
    PORTC = (PORTC&0xC0)|0x06; break;
  case 2:
    PORTD = (PORTD&0x7F)|0x80;
    PORTC = (PORTC&0xC0)|0x1B; break;
  case 3:
    PORTD = (PORTD&0x7F)|0x80;
    PORTC = (PORTC&0xC0)|0x0F; break;
  case 4:
    PORTD = (PORTD&0x7F)|0x80;
    PORTC = (PORTC&0xC0)|0x26; break;
  case 5:
    PORTD = (PORTD&0x7F)|0x80;
    PORTC = (PORTC&0xC0)|0x2D; break;
  case 6:
    PORTD = (PORTD&0x7F)|0x80;
    PORTC = (PORTC&0xC0)|0x3D; break;
  case 7:
    PORTD = (PORTD&0x7F)|0x00;
    PORTC = (PORTC&0xC0)|0x07; break;
  case 8:
    PORTD = (PORTD&0x7F)|0x80;;
    PORTC = (PORTC&0xC0)|0x3F; break;
  case 9:
    PORTD = (PORTD&0x7F)|0x80;
    PORTC = (PORTC&0xC0)|0x27; break;
  default:
    PORTD = (PORTD&0x7F);
    PORTC = (PORTC&0xC0)|0x3F; break;
 }
 

 if(digit_index++>3){
  digit_index=0;}

 score++;
}


 