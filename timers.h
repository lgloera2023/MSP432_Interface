#ifndef TIMERS_H
#define TIMERS_H

#include "msp.h"

void delay(int sec);

/////////////////////////////////////////////////
// Delay
// Pre-condition: SysTick timer has been
//   initialized.
// Post-condition: Launchpad won't do anything
//   for the given amount of seconds.
void delay(int sec) {
	// Clock is 3 MHz, so multiply sec by 3000000 to get number of counts
	SysTick -> LOAD = (sec * 3000000) - 1;
	
	// Enable count down
	SysTick -> CTRL |= 0x01;
	while ((SysTick -> CTRL & 0x10000) == 0) {
		// Wait
	}
	
	// Disable count down
	SysTick -> CTRL &= ~0x01;
}

#endif
