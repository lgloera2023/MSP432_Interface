#include "MSP432_Display.h"
#include "requestProcessing.h"

int main(void) {
	// Setup
	int choice = 0;
	display_INIT();
	ADCInit();
	rgb_INIT();
	input_INIT();
	SysTick -> CTRL |= 0x04;  // System clock, no interupt, enable off
	TIMER32_1 -> CONTROL |= 0x43;  // Periodic, 32-bit, oneshot
	
	// Main Loop
	while (1) {
		printMenu();
		
		// Get action
		choice = nextInt();
		switch (choice) {
			case 1:
				// RGB
				rgbRequest();
				break;
			case 2:
				// Check buttons
				digitalInputRequest();
				break;
			case 3:
				// Temperature readings
				tempRequest();
				break;
			default:
				transmitString("Invalid Input.\n\r");
				break;
		}
		
		// Newline
		transmitString("\n\r");
	}
	
	int inputGetter = P1 -> IN & 0xC0;
	switch (inputGetter) {
		case 0x80:
			// Pin 6 is LOW
			EUSCI_A0 -> TXBUF = '!';
			while ((EUSCI_A0 -> IFG & 0x02) == 0) {
				// Wait
			}
			break;
		case 0x40:
			// Pin 7 is LOW
			EUSCI_A0 -> TXBUF = '?';
			while ((EUSCI_A0 -> IFG & 0x02) == 0) {
				// Wait
			}
			break;
		default:
			// Both are LOW
			EUSCI_A0 -> TXBUF = '&';
			while ((EUSCI_A0 -> IFG & 0x02) == 0) {
				// Wait
			}
			break;
	}
	
}
