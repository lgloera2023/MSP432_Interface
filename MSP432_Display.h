#ifndef MSP432_DISPLAY_H
#define MSP432_DISPLAY_H

#include "msp.h"
#include <string.h>
#include <stdlib.h>

void display_INIT(void);
void transmitChar(char c);
void transmitString(char str[]);
int nextInt(void);
void printMenu(void);



/////////////////////////////////////////////////
// Display Initialization
// Pre-condition: None
// Post-condition: The UART module will be
//   configured and MSP432 ready to interface
//   with PC
void display_INIT(void) {
	EUSCI_A0 -> CTLW0 |= 1;  // Put in reset state
	EUSCI_A0 -> MCTLW = 0;  // Don't correct error
	EUSCI_A0 -> CTLW0 |= 0x80;  // Standard UART configuration
	EUSCI_A0 -> BRW = 26;  // 3MHz / 115200
	P1 -> SEL0 |= 0x0C;  // Set up UART module on pins 2 and 3 of port 1
	P1 -> SEL1 &= ~0x0C;
	EUSCI_A0 -> CTLW0 &= ~0x01;  // Take out of reset state
}



/////////////////////////////////////////////////
// Transmit Char
// Pre-condition: None
// Post-condition: The character will be sent to PC
//   via UART protocols
void transmitChar(char c) {
	EUSCI_A0 -> TXBUF = c;
	while ((EUSCI_A0 -> IFG & 0x02) == 0) {
		// Wait
	}
}



/////////////////////////////////////////////////
// Transmit String
// Pre-condition: The string must end with null
//   terminating character
// Post-condition: The string will be sent to PC
//   via UART protocols
void transmitString(char str[]) {
	int i = 0;
	while (str[i] != '\0') {
		EUSCI_A0 -> TXBUF = str[i];
		while ((EUSCI_A0 -> IFG & 0x02) == 0) {
			// Wait
		}
		++i;
	}
}



/////////////////////////////////////////////////
// Next Integer
// Pre-condition: UART module has been set up and
//   the input in buffer is an integer
// Post-condition: Returns the string in buffer
//   as an integer
int nextInt(void) {
	// Integers are 32 bit signed numbers
	// Largest number is 2147483647, which is 10 digits
	char integer[12];
	
	// Ensure we don't write out of bonds
	int i = 0;
	while (i < 11) {
		if ((EUSCI_A0 -> IFG & 1) != 0) {
			// Something is in buffer, need to pull it out
			integer[i] = EUSCI_A0 -> RXBUF;  // Place current character in string
			
			// Echo back to terminal
			transmitChar(integer[i]);
			
			// Check if end of number
			if (integer[i] == '\r') {
				break;
			} else {
				++i;
			}
		}
	}
	
	// Replace last character with null terminator for string functions
	integer[i] = '\0';
	
	// Set cursor on newline
	transmitString("\n\r");
	
	return atoi(integer);
}



/////////////////////////////////////////////////
// Print Menu
// Pre-condition: None
// Post-condition: The main menu will be printed
//   on terminal window of PC
void printMenu(void) {
	/*
	MSP432 Menu
	
	1. RGB Control
	2. Digital Input
	3. Temperature Reading
	*/
	char line[25] = "MSP432 Menu\n\r";
	transmitString(line);
	strcpy(line, "\n1. RGB Control\n\r");
	transmitString(line);
	strcpy(line, "2. Digital Input\n\r");
	transmitString(line);
	strcpy(line, "3. Temperature Reading\n\r");
	transmitString(line);
}

#endif
