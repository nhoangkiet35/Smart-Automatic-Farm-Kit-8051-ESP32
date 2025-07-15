sbit buzzer = P2^5;

void delay_beep(unsigned int time){//tinh bang us
	TMOD &=0xF0;
	TMOD |= 0x01;
	TH0 = (65536 - time) >> 8;
	TL0 = (65536 - time) & 0x00FF;
	TR0 = 1;
	while(TF0 == 0);
	TF0 = 0;
	TR0 = 0;
}
void delay_beep2(unsigned int time){
	while(time--){};
}
void beep(unsigned int period, unsigned int t){ // period tinh bang us, t tinh bang ms
	unsigned int i;
	for( i = 0; i < t;i++){
		buzzer = 1;
		delay_beep(period/2);
		buzzer = 0;
		delay_beep(period/2);
	}
}