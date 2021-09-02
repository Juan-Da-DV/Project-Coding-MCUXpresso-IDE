/*
 	 Title: MCO455 Final Project
 	 Authors: Juan David Daza V	(108722182) MCO455NEEL
 	 		  Kenneth Garcia 	(159384189)	MCO455NEEL
 	 Date:  November 22, 2019
 	 Description: A program that increases or decreases the set point temperature. The temperature will increase
 	 	 	 	  or decrease until it is equal with the set point. The temperature reading is sent to the sensor
 	 	 	 	  on TP10 and it will be read on TP8. The set point and temperature reading will be displayed on
 	 	 	 	  the 7 segment displays, LCD, and PC screen.
*/


// Includes Section
#include <hidef.h> 														// for EnableInterrupts macro
#include "derivative.h" 												// include peripheral declarations 
#include "E:\library_de1.h" 											// Arduino-like library 


// Global Variables 
volatile unsigned char setTemp;											// Set point temperature 
volatile unsigned char tempReading;										// Temperature change...
volatile unsigned char bob;												// Temperature from TP8
volatile unsigned char keyTemp;											// variable obtains values from Key Pad
volatile unsigned char friendSetTemp;									// Set point temp transmitted to friend terminal 
volatile unsigned char friendTemp;										// value from partner terminal
volatile unsigned char recent;											//updated temp
volatile unsigned char rob;												// receives temp values from LCD
unsigned static const segs[]=
	{
		0x02,0x9f,0x24,0x0c,0x98,0x48,0x40,0x1e,
		0x00,0x18
	};																	// assign values for 7 segment display

// Function Prototypes Section
void set_increase (void);												// increase setTemp
void set_decrease (void);												// decrease setTemp
void temp_increase (void);												// increase tempReading
void temp_decrease (void);												// decrease tempReading
void display(void);														// displays menu on LCD
void terminal(void); 													// displays menu on HyperTerminal
void sendTemp(void);													// remote system 
void clean(void); 														// clear Green and Red LEDS
void selectTrans_Receiver(void);										// determine if Transmitter or Receiver 
void tempTransmitted (void);											// Obtain set point from LCD and Transmitter
void recent_temp (void);												// send recent set temperature to partner terminal
void temp_LCD (void);													// 

// Main Function
void main(void)
{
	int i;																// for counting
	unsigned int var; 
	volatile unsigned char recentTemp; 									// obtain temperatures from SCI1D
	setTemp = 0x14;														// put 0x14 Hexadecimal value
	tempReading = 0x14;													// put 0x14 Hexadecimal value
	
	SOPT_COPE = 0;  													// disable COP
	SCI2BDL = 0x82;														// set bound to 9600
	SCI2C2 = 0x0c; 														// enable Tx and Rx 
	LCD_ROWS=4;															// 4 line LCD
	LCD_COLS=16;														// 16 character LCD
	clean();															// clear LEDS 
		
	devices_init();														// initialize all devices 
	
	for(;;)																// loop forever
	{			
		display();														// displays menu on LCD
		terminal();														// displays menu on HyperTerminal	
			
		analog_write_int(1, hex2dec(tempReading) << 4);					// send to TP10 
		bob = analog_read_int(1) >> 4;									// read on TP8					
				
		HEX3 = segs[hex2dec(setTemp) >> 4];								// set value on 7 signal display
		HEX2 = segs[hex2dec(setTemp) & 0x0f];							// set value on 7 signal display
		HEX1 = segs[bob >> 4];											// set value on 7 signal display
		HEX0 = segs[bob & 0x0f];										// set value on 7 signal display
			
		if (SW_KEY3 == 0)												// if SW_KEY3 is pressed
		{
			while (setTemp > 10 & SW_KEY3 == 0)							// stay in loop if SW_KEY3 is pressed
			{
				set_decrease();											// decrease setTemp
				for (i = 0; i < 3; i++)
				{
					delay_milli(100);									// let go of button
				}
			}
			
			while (setTemp < tempReading)								// as long as setTemp < tempReading
			{ 
				if (SW_KEY3 != 0)										// if SW_KEY3 is not pressed
				{	
					temp_decrease();									// decrease tempReading
					for (i = 0; i < 10; i++)
					{	
						delay_milli(100);								// decrease tempReading per 1 second
					}
				}
			}
				LEDG = LEDG & 0b00000000;									// turn off green LEDs
		}
						
		if(SW_KEY2 == 0)												// if SW_KEY2 is pressed
		{				
			while(setTemp < 35 & SW_KEY2 == 0)							// stay in loop if SW_KEY2 is pressed
			{
				set_increase();											// increase setTemp
				for(i = 0; i < 3; i++)
				{
					delay_milli(100);									// let go of button
				}
			}
					
			while(setTemp > tempReading)								// as long as setTemp > tempReading
			{
				if (SW_KEY2 != 0)										// if SW_KEY2 is not pressed
				{
					temp_increase();									// increase tempReading
					for (i = 0; i < 10; i++)
					{	
						delay_milli(100);								// increase tempReading per 1 second
					}
				}
			}
				LEDRL = LEDRL & 0b00000000; 								// turn off red LEDs
				LEDG = LEDG & 0b00000000; 									// turn off green LEDs
		}
			
		if (SW_KEY1 == 0)												// if SW_KEY1 is pressed 
		{
			rob = get_dec(1,4,1);									    // position on LCD and wait for keypad input
				
			if (rob >= 10 && rob <= 35)	
			{
				scr_setcursor(18,31);									//set cursor to display value 
				scr_write(hex2dec(rob));
				scr_setcursor(18,45);
				scr_write(tempReading);
				
				while(!(SCI1S1 & 0x80));								// Wait until TDRE = 1
				SCI1D = friendTemp;
				
				while((SCI2S1 & 0x80)!=0x80);				
				SCI2D = var;
				
				selectTrans_Receiver ();
			}
		}
			
		if (SCI1D & 0x20)
		{
			selectTrans_Receiver();			
		}
	}
}
		
	


// Function Definitions
void set_increase(void)
{
	setTemp++;															// increase setTemp
								
	HEX3 = segs[hex2dec(setTemp) >> 4];									// set values on 7 signal display 
	HEX2 = segs[hex2dec(setTemp) & 0x0f];								// set values on 7 signal display
	
	lcd_setcursor(1, 4);												// set cursor position on LCD
	lcd_write(hex2dec(setTemp));										// display setTemp on LCD
	
	scr_setcursor(6, 25);												// set cursor position on PC screen
	scr_write(hex2dec(setTemp));										// display setTemp on PC screen
}

void temp_increase(void)
{
	tempReading++;														// increase tempReading
									
	analog_write_int(1, hex2dec(tempReading) << 4);						// send to TP10
	bob = analog_read_int(1) >> 4;										// read to TP8
			
	HEX1 = segs[bob >> 4];												// set value on 7 segment display 
	HEX0 = segs[bob & 0x0f];											// set value on 7 segment display 
		
	lcd_setcursor(1, 12);												// set cursor position on LCD
	lcd_write(bob);														// display temperature on LCD
	lcd_setcursor(3,6);													// set cursor position
	lcd_print("on ");													// turn fan on
	lcd_setcursor(3, 0);												// set cursor position
	lcd_print("on ");													// turn heat on
	
	scr_setcursor(6, 49);												// set cursor position on PC screen
	scr_write(bob);														// display temperature on PC screen
	scr_setcursor(10, 39);												// set cursor position
	scr_print("on ");													// turn fan on
	scr_setcursor(10, 25);												// set cursor position
	scr_print("on ");													// turn heat on

	LEDRL = LEDRL | 0b00000001;											// turn on red LED for heating
	LEDG = LEDG | 0b10000000;											// turn on green LED for fan 
}

void set_decrease(void)
{
	setTemp--;															// decrease setTemp
							
	HEX3 = segs[hex2dec(setTemp) >> 4];									// set value on 7 signal display 
	HEX2 = segs[hex2dec(setTemp) &0x0f];								// set value on 7 signal display
	
	lcd_setcursor(1, 4);												// set cursor position on LCD
	lcd_write(hex2dec(setTemp));										// display setTemp on LCD
	lcd_setcursor(3,6);													// set cursor position
	lcd_print("on ");													// turn fan on
	lcd_setcursor(3,11);												// set cursor position
	lcd_print("on");													// turn cool on
	
	
	scr_setcursor(6, 25);												// set cursor position on PC screen
	scr_write(hex2dec(setTemp));										// display setTemp on PC screen
}

void temp_decrease(void)
{
	tempReading--;														// decrease tempReading
								
	analog_write_int(1, hex2dec(tempReading) << 4);						// send to TP10
	bob = analog_read_int(1) >> 4;										// read to TP8
	
	HEX1 = segs[bob >> 4];												// set value on 7 signal display
	HEX0 = segs[bob & 0x0f];											// set value on 7 signal display
	
	lcd_setcursor(1, 12);												// set cursor position on LCD
	lcd_write(bob);														// display temperature on LCD
	lcd_setcursor(3,6);													// set cursor position
	lcd_print("on ");													// turn fan on
	lcd_setcursor(3, 11);												// set cursor position
	lcd_print("on ");													// turn cool on
	
	scr_setcursor(6 , 49);												// set cursor position on PC screen
	scr_write(bob);														// display temperature on PC screen
	scr_setcursor(10, 39);												// set cursor position
	scr_print("on ");													// turn fan on
	scr_setcursor(10, 52);												// set cursor position
	scr_print("on ");													// turn cool on
	
	LEDG = LEDG | 0b10100000;											// turn on green LEDs for cool
}

void selectTrans_Receiver (void)
{
	if((SCI1D & 0xC0)==0x00)
	{
		recent = SCI1D;
		if (recent & 0xC0)
		{
			friendSetTemp = recent & 0x3F;
		}
	}
	else
	{
		scr_setcursor(18, 31);
		scr_write(hex2dec(recent));
	}
	
	display();															//will update new "recent" temperature in LCD
	terminal();															//will update new "recent" temperature in Terminal
																		//will update new
	
	if (friendSetTemp > setTemp)
	{
		setTemp = setTemp + 0x01;
		if (setTemp >= 0x23)
		{
			setTemp = 0x23;
		}
	}
}

void display(void)
{	
	lcd_setcursor(0,0);													// set cursor position
	lcd_print("TEMP Ctrl System ");										// display heading
	lcd_setcursor(1,0);													// set cursor position
	lcd_print("StPT");													// display heading
	lcd_write(hex2dec(setTemp));										// display set point
	lcd_writech(0xdf);													// degree symbol
	lcd_print("C");														// for Celsius
	lcd_print(" Tmp");													// display heading
	lcd_write(bob);														// display temperature
	lcd_writech(0xdf);													// degree symbol
	lcd_print("C");														// for Celsius
	lcd_setcursor(2,0);													// set cursor position
	lcd_print("HTR   FAN  Cool");										// display heading
	lcd_setcursor(3,6);													// set cursor position
	if (hex2dec(setTemp) != hex2dec(tempReading))						// if temperatures are not equal
		lcd_print("on");												// turn fan on
	else
		lcd_print("off");												// otherwise turn fan off
	lcd_setcursor(3,0);													// set cursor position
	if (hex2dec(setTemp) > hex2dec(tempReading))						// if set point is higher than reading
	{
		lcd_print("on");												// turn heat on
		lcd_setcursor(3,6);												// set cursor position
		lcd_print("on");												// turn fan on
	}
	else
		lcd_print("off");												// otherwise turn heat off
	lcd_setcursor(3,11);												// set cursor position
	if (hex2dec(setTemp) < hex2dec(tempReading))						// if set point is lower than reading
	{
		lcd_print("on");												// turn cool on
		lcd_setcursor(3,6);												// set cursor position
		lcd_print("on");												// turn fan on
	}
	else
		lcd_print("off");												// otherwise turn cool off
}

void terminal(void)
{
	scr_setcursor(0,19);												// set cursor position
	scr_print("Temperature Control System Monitor Program");			// display heading
	scr_setcursor(3,34);												// set cursor position
	scr_print("LOCAL SYSTEM");											// display heading
	scr_setcursor(5,19);												// set cursor position
	scr_print("SETPOINT TEMPERATURE    TEMPERATURE READING");			// display heading
	scr_setcursor(6,25);												// set cursor position
	scr_write(hex2dec(setTemp));										// display set point
	scr_setcursor(6,28);												// set cursor position
	scr_print("deg C");													// display degrees Celsius			
	scr_setcursor(6,49);												// set cursor position
	scr_write(bob);														// display temperature
	scr_setcursor(6,52);												// set cursor position
	scr_print("deg C");													// display degrees Celsius	
	scr_setcursor(9,20);												// set cursor position
	scr_print("HEATING SYSTEM     FAN     COOLING SYSTEM");				// display heading
	scr_setcursor(10,39);												// set cursor position
	if (hex2dec(setTemp) != hex2dec(tempReading))						// if temperatures are not equal
		scr_print("on");												// turn fan on
	else
		scr_print("off");												// otherwise turn fan off
	scr_setcursor(10,25);												// set cursor position
	if (hex2dec(setTemp) > hex2dec(tempReading))						// if set point is higher than temperature reading
	{
		scr_print("on");												// turn heat on
		scr_setcursor(10,39);											// set cursor position
		scr_print("on");												// turn fan on
	}
	else
		scr_print("off");												// otherwise turn heat off
	scr_setcursor(10,52);												// set cursor position
	if (hex2dec(setTemp) < hex2dec(tempReading))						// if set point is lower than temperature reading
	{
		scr_print("on");												// turn cool on
		scr_setcursor(10,39);											// set cursor position
		scr_print("on");												// turn fan on
	}
	else
		scr_print("off");												// otherwise turn cool off
	scr_setcursor(15,34);												// set cursor position
	scr_print("REMOTE SYSTEM");											// display heading
	scr_setcursor(17,28);												// set cursor position
	scr_print("SETPOINT     TEMPERATURE");								// display heading
}

void clean (void)
{
	LEDRH = 0;															// clears Red Leds													
	LEDRL = 0;															// clears Red Leds
	LEDG = 0;															// clears green leds
	HEX0 = 0;								
	HEX1 = 0;
	HEX2 = 0;
	HEX3 = 0;	
}
