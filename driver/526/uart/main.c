#define GPA1CON     (*( volatile  unsigned int *)0x11400020)
#define ULCON2      (*( volatile  unsigned int *)0x13820000)
#define UCON2       (*( volatile  unsigned int *)0x13820004)
#define UBRDIV2     (*( volatile  unsigned int *)0x13820028)
#define UFRACVAL2   (*( volatile  unsigned int *)0x1382002C)


#define UTRSTAT2    (*( volatile  unsigned int *)0x13820010)
#define URXH2       (*( volatile  unsigned int *)0x13820024)
#define UTXH2       (*( volatile  unsigned int *)0x13820020)



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

int main()
{
	InitUart();
	char c=0;
	while(1)
	{
		c=RecvFromUart();
		SendToUart(c+1);
	}
}


