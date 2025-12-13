/*
 * hurry_up.h
 *
 * 
 */

 //Hurry up target***************************************************************/
 if( right_lane_sm && !hurry_up){
	 hurry_up_enable= 1;//hurry up target active
	 hurry_up_hit = 0;
	 led_control |=(1<<3); //3rd bit of shift register
 }
 if((falling_edges[1]&(1<<4))&&(hurry_up_prev )&&(!hurry_up)){
	 if(hurry_up_enable){
		 score_ +=100;
		 updateFlag =1;
		 hurry_up_hit= 1;
		 hurry_up_active= 0;
		 hurry_timer =0;
		 led_control &= ~(1<<3);// clear 3rd bit of led register
	 }
 }