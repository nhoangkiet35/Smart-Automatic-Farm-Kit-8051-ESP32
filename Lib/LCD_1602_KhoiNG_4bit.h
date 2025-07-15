#define LCD_data P0

sbit rs=P2^6;			/* Register select pin */
sbit rw=P2^5;			/* Read/Write pin */
sbit en=P2^7;			/* Enable pin */

void delay(unsigned char count){
	int i,j;
	for(i=0;i<count;i++)
	for(j=0;j<112;j++);
}

void LCD_command(unsigned char cmd) {
	LCD_data = (LCD_data & 0x0F) | (cmd & 0xF0);
	rs=0;			/* command reg. */
	rw=0;			/* Write operation */
	en=1; 
	delay(1);
	en=0;
	delay(2);
	
	LCD_data = (LCD_data & 0x0F) | (cmd << 4);
	en=1; 
	delay(1);
	en=0;
	delay(5);
}

void LCD_write_char(unsigned char char_data){
	LCD_data = (LCD_data & 0x0F) | (char_data & 0xF0);
	rs=1;			/* command reg. */
	rw=0;			/* Write operation */
	en=1; 
	delay(1);
	en=0;
	delay(2);
	
	LCD_data = (LCD_data & 0x0F) | (char_data << 4);
	en=1; 
	delay(1);
	en=0;
	delay(5);
}

void LCD_write_string(unsigned char *str){
	int i;
	for(i=0;str[i]!=0;i++)  /* Send each char of string till the NULL */
	{
		LCD_write_char (str[i]);  /* Call LCD data write */
	}
}

void LCD_string_xy(char row, char pos, char *str){
	if (row == 0)
		LCD_command((pos & 0x0F)|0x80);
	else if (row == 1)
		LCD_command((pos & 0x0F)|0xC0);
		LCD_write_string(str);	/* Call LCD string function */
}

void LCD_init() {
	delay(20);		/* LCD Power ON Initialization time >15ms */
	LCD_command (0x02);	/* Initialization of 16X2 LCD in 8bit mode */
	LCD_command (0x28);	/* Initialization of 16X2 LCD in 4bit mode */
	LCD_command (0x0C);	/* Display ON Cursor OFF */
	LCD_command (0x06);	/* Auto Increment cursor */
	LCD_command (0x01);	/* clear display */
	LCD_command (0x80);	/* dat con tro vi tri dau hang */
}
