#define GPD0CON 	(*(volatile unsigned int*)0x114000a0)
#define TCFG0	 	(*(volatile unsigned int*)0x139D0000)
#define TCFG1	 	(*(volatile unsigned int*)0x139D0004)
#define TCNTB0	 	(*(volatile unsigned int*)0x139D000C)
#define TCMPTB0 	(*(volatile unsigned int*)0x139D0010)
#define TCON	 	(*(volatile unsigned int*)0x139D0008)
void pwminit()
{
	//配置管脚为PWM输出，一般的gpio管脚不能直接驱动蜂鸣器的。配置的这个也不是普通的out，是TOUT
	GPD0CON |= 0x2;
	//外部输入的频率是100mhz，经过2级分频25----4----变成1m，
	TCFG0 = TCFG0 &~(0XFF) |24;
	//2JI FENPIN 4
	TCFG1 = TCFG1 &~(0XF) | 0X2;
	//2500的波形个数实际就设置了输出的波形是400hz，1000000/400=2500。2500个1m的波形才能产生1个400hz的波形
	TCNTB0 = 2500;
	//2500*1/2---实际就设置了占空比为1/2
	TCMPTB0 = 1250;
	TCON = TCON |1<<3 |1<<1 |1  ; //第1位必须先拉高再拉低才可以,拉高刷新count_reg(清零)与cmp_reg里面的值		
	TCON = TCON &~(1<<1) ;//然后拉低，要不一直刷新也不行
}
void delay()
{
	int i = 1000,j = 1000;
	while(i--)
	{
		while(j--);
	}
}
int main()
{
	pwminit();
	int i = 1;
	int arr[] = {100,800,9000,1000,1500,8000,2200,17000,18000,15000,10000,50};
	while(1)
	{
		for(i = 1;i < 20;i++)
		{
			TCNTB0 = arr[i];
			TCMPTB0 = arr[i]/2;
			delay();
		}
		
	}
}


