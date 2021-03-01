#define GPA1CON     (*( volatile  unsigned int *)0x11400020)
#define ULCON2      (*( volatile  unsigned int *)0x13820000)
#define UCON2       (*( volatile  unsigned int *)0x13820004)
#define UBRDIV2     (*( volatile  unsigned int *)0x13820028)
#define UFRACVAL2   (*( volatile  unsigned int *)0x1382002C)
#define UTRSTAT2    (*( volatile  unsigned int *)0x13820010)
#define URXH2       (*( volatile  unsigned int *)0x13820024)
#define UTXH2       (*( volatile  unsigned int *)0x13820020)
//---------------------------------------------------------------
#define GPX1CON 		(*(volatile unsigned int *)0x11000c20)
#define EXT_INT41CON 	(*(volatile unsigned int *)0x11000e04)
#define EXT_INT41MASK	(*(volatile unsigned int *)0x11000F04)
#define ICDISERI_CPU0	(*(volatile unsigned int *)0x10490104)
#define ICDIPTR_CPU0	(*(volatile unsigned int *)0x10490838)
//-------------------------------------------------
#define ICDIPR_CPU0		(*(volatile unsigned int *)0x10490438)
#define ICCICR_CPU0		(*(volatile unsigned int *)0x10480000)
#define ICCDDCR			(*(volatile unsigned int *)0x10490000)
#define ICCPMR_CPU0		(*(volatile unsigned int *)0x10480004)

#define ICCIAR_CPU0		(*(volatile unsigned int *)0x1048000C)
#define EXT_INT41_PEND	(*(volatile unsigned int *)0x11000f44)
#define ICDICPR_CPU0	(*(volatile unsigned int *)0x10490284)
#define ICCEOIR_CPU0	(*(volatile unsigned int *)0x10480010)
void InitUart()
{
	//step1:set gpa1_1 and gpa1_0 uart mode
	GPA1CON=GPA1CON&~(0xFF)|0x22;
	//step2:config uart:parity mode, stop bit,data bit
	ULCON2=0<<6 |0<<5 |0<<2 |3; 
	//ULCON2=ULCON2&~(1<<6) | 0<<5 |0<<2 |0x3;
	//step3:config uart recv and transmit data mode
	UCON2=UCON2&~(0xF)| (1<<2)|1;
	//step4:set baud rate
	UBRDIV2=53;
	UFRACVAL2=4;
}
//recv
char RecvFromUart()
{
	while(!(UTRSTAT2&1))
	{
		;//recv
	}
	return URXH2;
}

//send
void SendToUart(char c)
{
	while(!(UTRSTAT2&(1<<1)))
	{
		;//send
	}
	UTXH2=c;
}
void InitKey2()
{
	//CONFIG IT MODEL
	GPX1CON = GPX1CON | (0XF<<4);
	//TRIGGER MODEL FALL EDGE
	EXT_INT41CON = EXT_INT41CON & ~(0X7<<4) |(0X2<<4);
	//ENABLE IT PIN
	EXT_INT41MASK = EXT_INT41MASK &~(0X1<<1);
	//*******************************
	//ENABLE 57 IT
	ICDISERI_CPU0 |= (0X1<<25);
	//57NO-->CPU0
	ICDIPTR_CPU0 = ICDIPTR_CPU0 & ~(0XFF<<8) | (0X1<<8);
	//PRIORITY 12
	ICDIPR_CPU0 = ICDIPR_CPU0 & ~(0XFF<<8) | 12<<8;//余数是1则offset就是1
	//ENBALE CPU0
	ICCICR_CPU0 = 1;
	//----------------------------------------------------------
	ICCPMR_CPU0 = ICCPMR_CPU0 & ~(0XFF << 8) | (0Xff);//???????
	//
	ICCDDCR = 1;
	
}
void doirq()		//no args no returns
{
	int num = ICCIAR_CPU0 & (0x3ff);
	switch(num)
	{
		case 57:
		{
			SendToUart('h');
			SendToUart('e');
			SendToUart('l');
			SendToUart('l');
			SendToUart('o');
			//clear 
			EXT_INT41_PEND |= 1<<1;
			//gic 57 clear 
			ICDICPR_CPU0 |= 1<<25;//57-32
		}
	}
	ICCEOIR_CPU0 = ICCEOIR_CPU0 &~(0x3ff) | num;
}
int main()
{
	InitUart();
	InitKey2();
	while(1);
	
	return 0;
}
