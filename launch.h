/*
 * launch.h
 *
 * 
 */

//Ball Launch***********************************************************************
#ifndef LAUNCH_SOLENOID_H
#define LAUNCH_SOLENOID_H


volatile uint8_t launch_ball = 0;
volatile uint16_t ball_launch_count = 0;
uint8_t ball_launch_sol[2] = {0, 0x08};
uint8_t ball_launch_button[2] = {1, 0x10};

VOID Ball_launch(
    if(launch_ball == 0){
	LED_off(ball_launch_sol);
	PORTC &=~(1<<PORTC4);
 
	if(switch_is_closed(ball_launch_button)){
		launch_ball =1;
		ball_launch_count = 0;
}
}
    else {//launch ball is 1
	LED_on (ball_launch_sol);
	PORTC |=(1<<PORTC4);
       ball_launch_count++;
if (ball_launch_count >=500){
       launch_ball=0;
}
}
)
