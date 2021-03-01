#define GPX2CON (*(volatile unsigned int*)0x11000c40)
#define GPX2DAT (*(volatile unsigned int*)0x11000c44)

#define GPX1CON (*(volatile unsigned int*)0x11000c20)
#define GPX1DAT (*(volatile unsigned int*)0x11000c24)

#define GPF3CON (*(volatile unsigned int*)0x114001e0)
#define GPF3DAT (*(volatile unsigned int*)0x114001e4)



void ledinit()
{
	//led2
	GPX2CON &= ~(0xf<<28);
	GPX2CON |= (0x1<<28);
	//led3
	GPX1CON &= ~(0xf);
	GPX1CON |= 0x01;
	//led4 and led5
	GPF3CON &= ~(0xff<<16);
	GPF3CON |= 0x11<<16;
}

void led2_on()
{
	GPX2DAT |= (0x1<<7);
}
void led2_off()
{
	GPX2DAT &= ~(0x1<<7);
}

void led3_on()
{
	GPX1DAT |= 0x1;
}
void led3_off()
{
	GPX1DAT &= ~(0x1);
}


void led4_on()
{
	GPF3DAT |= (0x1<<4);
}
void led4_off()
{
	GPF3DAT &= ~(0x1<<4);
}

void led5_on()
{
	GPF3DAT |= (0x1<<5);
}
void led5_off()
{
	GPF3DAT &= ~(0x1<<5);
}


void delay()
{
	int i = 0,j = 0;
	for(i = 0;i < 10000;i++)
	{
		for(j = 0;j < 100;j++);
	}
}
int main()
{
	ledinit();
	while(1)
	{
		led5_off();
		led2_on();
		delay();
		led2_off();
		led3_on();
		delay();
		led3_off();
		led4_on();
		delay();
		led4_off();
		led5_on();
		delay();
	}
	return 0;
}



