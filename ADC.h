#ifndef ADC_H
#define ADC_H

#include "msp.h"

//function prototypes

void ADCInit(void);//initialize the ADC

float tempRead(void); //read temperature sensor

//function definitions
void ADCInit(void)
{
	//Ref_A settings
	REF_A ->CTL0 &= ~0x8; //enable temp sensor
	REF_A ->CTL0 |= 0x30; //set ref voltage
	REF_A ->CTL0 &= ~0x01; //enable ref voltage
	
	//do ADC stuff
	ADC14 ->CTL0 |= 0x10; //turn on the ADC
	ADC14 ->CTL0 &= ~0x02; //disable ADC
	ADC14 ->CTL0 |= 0x4180700; //no prescale, mclk, 192 SHT
	ADC14 ->CTL1 &= ~0x1F0000; //configure memory register 0
	ADC14 ->CTL1 |= 0x800000; //route temp sense
	ADC14 ->MCTL[0] |= 0x100; //vref pos int buffer
	ADC14 ->MCTL[0] |= 22; //channel 22
	ADC14 ->CTL0 |=0x02; //enable adc
		
	return;
} 

float tempRead(void)
{
	float temp; //temperature variable
	uint32_t cal30 = TLV ->ADC14_REF2P5V_TS30C; //calibration constant
	uint32_t cal85 = TLV ->ADC14_REF2P5V_TS85C; //calibration constant
	float calDiff = cal85 - cal30; //calibration difference
	ADC14 ->CTL0 |= 0x01; //start conversion
	while((ADC14 ->IFGR0) ==0) 
	{
		//wait for conversion
	}
	temp = ADC14 ->MEM[0]; //assign ADC value
	temp = (temp - cal30) * 55; //math for temperature per manual
	temp = (temp/calDiff) + 30; //math for temperature per manual
	
	return temp; //return temperature in degrees C
}

#endif
