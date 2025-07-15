void UART_Init()
{
	SM0 = 0;
	SM1 = 1;
	//SCON = 0x40; 		// 0100 0000 - Mode 1
	TMOD = 0x20; 			// 0010 xxxx - Timer 1 8 bit auto reload
	TH1 = 0xFD; 			// 9600 Baud Rate
	TR1 = 1; 					// start Timer 1
	
	REN = 1;					// Receive Enable
	//SCON = 0x50; 		// 0101 0000 - Mode 1 and Receive Enable
}

void UART_Write (char c)
{
	SBUF = c ;
	while (TI == 0);
	TI = 0;
}

void UART_Write_String (char *str)
{
	unsigned char i = 0;
	while (str[i] != 0)
	{
		UART_Write (str[i]);
		i++;
	}
}

char UART_Read ()
{
	RI = 0;
	return SBUF;
}

char UART_ReadString (char *buffer, unsigned char maxLength)
{
	unsigned char i = 0;
	char c;

	while (1)
	{
		while (RI == 0);   // Wait for data
		c = SBUF;
		RI = 0;

		if (c == '\r' || c == '\n')  // End of input
			break;

		if (i < maxLength - 1)      // Protect against overflow
		
			buffer[i++] = c;
	}
	buffer[i] = '\0';  // Null-terminate the string
}
