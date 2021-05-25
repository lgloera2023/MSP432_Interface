#ifndef REQUESTPROCESSING_H
#define REQUESTPROCESSING_H

#include "msp.h"
#include "MSP432_Display.h"
#include "timers.h"
#include "ADC.h"
#include <stdio.h>

void rgb_INIT(void);
void input_INIT(void);
void rgbRequest(void);
void digitalInputRequest(void);
void tempRequest(void);



/////////////////////////////////////////////////
// RGB Initialization
// Pre-condition: None
// Post-condition: RGB on board is ready for use.
void rgb_INIT(void) {
	// Enable DIO for 2.0, 2.1, and 2.2
	P2->SEL0 &= ~0x07;  // Set selector bits to 0
	P2->SEL1 &= ~0x07;
	
	// Make 2.0, 2.1, and 2.2 output pins
	P2->DIR |= 0x07;
	
	// Start out with all LEDs off
	P2->OUT &= ~0x07;
}



/////////////////////////////////////////////////
// Input Initialization
// Pre-condition: None
// Post-condition: The two buttons are ready to read
void input_INIT(void) {
	// Enable DIO for Button 1 on pin 1.1 and Button 2 on pin 1.4
	P1->SEL0 &= ~0x12;  // Set selector bits to 0
	P1->SEL1 &= ~0x12;
	
	// Make 1.1 and 1.4 input pins
	P1->DIR &= ~0x12;
	
	// Enable internal pull-up resistor for the active low buttons
	P1->REN |= 0x12;
	P1->OUT |= 0x12;
}



/////////////////////////////////////////////////
// RGB Request
// Pre-condition: RGB, Systick, and UART have
//   been set up.
// Post-condition: Do something with LED.
void rgbRequest(void) {
	int rgbColor = 7;
	int toggleTime = 0;
	int numBlinks = 0;
	int i;

	transmitString("Enter combination of RGB (1 - 7):");
	rgbColor = nextInt();
	if (rgbColor < 1 || rgbColor > 7) {
		transmitString("Invalid Input.\n\r");
		return;
	}

	transmitString("Enter Toggle Time:");
	toggleTime = nextInt();
	if (toggleTime < 1) {
		transmitString("Invalid Input.\n\r");
		return;
	}

	transmitString("Enter Number of Blinks:");
	numBlinks = nextInt();
	if (rgbColor < 1) {
		transmitString("Invalid Input.\n\r");
		return;
	}
	
	transmitString("Blinking LED...\n\r");
	
	// Blink LED
	for (i = 0; i < numBlinks; ++i) {
		// Turn on LED for specified time
		P2 -> OUT |= rgbColor;
		delay(toggleTime);
		
		// Turn off LED and wait for specified time if not last blink
		P2 -> OUT &= ~0x07;
		if (i < numBlinks - 1) {
			delay(toggleTime);
		}
	}
	
	transmitString("Done\n\r");
}



/////////////////////////////////////////////////
// Digital Input Request
// Pre-condition: UART and the buttons on Launchpad
//   have been initialized.
// Post-condition: Report which button is currently
//   pressed, if any.
void digitalInputRequest(void) {
	// Active low, so IN will equal 0x12 if no button is pressed
	int isPressed = P1->IN & 0x12;  // 0001 0010
	switch(isPressed) {
		case 0x02:
			transmitString("Button 2 pressed.\n\r");
			break;
		case 0x10:
			transmitString("Button 1 pressed.\n\r");
			break;
		case 0x12:
			transmitString("No Button pressed.\n\r");
			break;
		default:
			transmitString("Buttons 1 and 2 pressed.\n\r");
			break;
	}
}



/////////////////////////////////////////////////
// Temperature Request
// Pre-condition: UART and TIMER32_1 have been
//   initialized.
// Post-condition: Report a temperature reading
//   every second for 1 to 5 times
void tempRequest(void) {
	// Ask user how many readings they want
	int numReadings, i;
	transmitString("Enter Number of Temperature Readings (1 - 5):");
	numReadings = nextInt();
	
	// Check valid input
	if (numReadings > 0 && numReadings < 6) {
		// Initialize line to be transmitted
		char line[34];
		
		for (i = 0; i < numReadings; ++i) {
			// Get the temperature
			float temp = tempRead();
			
			// Format string with float for celsius and fahrenheit
			sprintf(line, "Reading %d: %.2f C & %.2f F\n\r", i+1, temp, 9./5. * temp + 32);  // Fahrenheit = 9/5 * Celsius + 32
			transmitString(line);
			
			// Wait a second if not the last one
			if (i < numReadings - 1) {
				TIMER32_1 -> LOAD = 3000000 - 1;  // One second since 3MHz clock
				TIMER32_1 -> CONTROL |= 0x80;  // start timer
				while ((TIMER32_1 -> RIS & 0x01) == 0) {
					// Wait
				}
				// Reset count flag
				TIMER32_1 -> INTCLR = 0;
			}
		}
		
	} else {
		transmitString("Invalid Input.\n\r");
	}
}

#endif
