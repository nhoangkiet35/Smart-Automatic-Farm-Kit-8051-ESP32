// Thinh Design

#include<stdio.h>

sbit SoilSensor=P2^1;		/* Connect YL-69 output Pin to P2.1 Pin */
int I_RH;
int D_RH;
int I_Temp;
int D_Temp;
int CheckSum; 

void timer_delay20ms()		/* Timer0 delay function */
{
	TMOD = 0x01;
	TH0 = 0xB8;		/* Load higher 8-bit in TH0 */
	TL0 = 0x0C;		/* Load lower 8-bit in TL0 */
	TR0 = 1;		/* Start timer0 */
	while(TF0 == 0);	/* Wait until timer0 flag set */
	TR0 = 0;		/* Stop timer0 */
	TF0 = 0;		/* Clear timer0 flag */
}

void timer_delay30us()		/* Timer0 delay function */
{
	TMOD = 0x01;		/* Timer0 mode1 (16-bit timer mode) */
	TH0 = 0xFF;		/* Load higher 8-bit in TH0 */
	TL0 = 0xF1;		/* Load lower 8-bit in TL0 */
	TR0 = 1;		/* Start timer0 */
	while(TF0 == 0);	/* Wait until timer0 flag set */
	TR0 = 0;		/* Stop timer0 */
	TF0 = 0;		/* Clear timer0 flag */
}

void Request()			/* Microcontroller send request */
{
	SoilSensor = 0;		/* set to low pin */
	timer_delay20ms();	/* wait for 20ms */
	SoilSensor = 1;		/* set to high pin */
}

void Response()			/* Receive response from SoilSensor */
{
	while(SoilSensor==1);
	while(SoilSensor==0);
	while(SoilSensor==1);
}

int Receive_data()		/* Receive data */
{
	int q,c=0;	
	for (q=0; q<8; q++)
	{
		while(SoilSensor==0);/* check received bit 0 or 1 */
		timer_delay30us();
		if(SoilSensor == 1)	/* If high pulse is greater than 30ms */
		c = (c<<1)|(0x01);/* Then its logic HIGH */
		else		/* otherwise its logic LOW */
		c = (c<<1);
		while(SoilSensor==1);
	}
	return c;
}
