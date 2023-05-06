#include<stdio.h>
#include<reg52.h>

#define LED1 P2
#define LED2 P0
sbit LED17 = P3^6;

unsigned char receiveData;

void initDate()
{
	SCON=0x50;//数据位8位，停止位1位，起始位1位
	//波特率设置 
	TMOD=0x20;//用到定时/计数器的方式2
	TH1=0xFD;
	TL1=0xFD;
	TR1=1; //启动定时器
}

void UsartInit() {
	SCON = 0x50;//设置为工作方式1;
	TMOD = 0x20;//设置计数器工作方式2;T1的M1M0=10;自动装载模式 无需中断服务函数
	PCON = 0x80;//波特率加倍	SMOD = 1
	TH1 = 0xf3;//计数器初始值设置 波特率设置为4800
	TL1 = 0xf3;
	ES = 1;//打开接收中断
	EA = 1;//打开总中断
	TR1 = 1;//打开计数器
}

void Usart() interrupt 4 
{
	unsigned char receiveData;
	receiveData = SBUF;
	RI = 0;
	SBUF = receiveData;
	while (!TI);//等待数据发送完毕
	TI = 0;
}

void main()
{
	
 	while(1)
	{
		initDate();
		recived();	
		switch(receiveData)
		{
		//	printf("%c\n", receiveData);
			case '1' : Bright(); break;
			case '2' : Flash(); break;
			case '3' : D1toD17(); break;
			case '4' : D17toD1(); break;
			default : break;
		}
	}
}
