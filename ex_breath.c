#include"reg52.h"  

#define uint unsigned int       //宏定义  
#define uchar unsigned char  
	
//#define   Time0_Mode0 ON


#define LED1  P0
#define LED2  P2
sbit LED17=P3^6;  

unsigned int PWM_ON = 0;
unsigned int light_end = 200;
unsigned int light_count = 0;

uint delayTime = 5000;

void Delay(unsigned int ms)
{
	unsigned int i,j;
	for(i=0;i<ms;i++)
		for(j=0;j<110;j++)
	;
}

void Time0_Mode1(void)
{
	TMOD=0x01;  //设置T0为工作方式0  
	TH0=(65535-50000)/256;     //5000us延时  
	TL0=(65535-50000)%256;     
	/*Timer0工作在模式0下时为13位定时/计数器， 
	TL0只使用前5位,故对32进行求余/模    */  

	EA=1;       //开总中断  
	ET0=1;      //开定时器0中断  
	TR0=1;      //启动定时器0  
}

void main()  
{  
	unsigned int ON=0;
	Time0_Mode1();
	
    while(1)
	{
		for(ON=0;ON<light_end;ON+=20)
		{
			PWM_ON = ON;
			Delay(3);
		}
	}
}  
  
/******T0中断函数*******/  
void Timer0()interrupt 1  
{  
	TH0=(65535-delayTime)/256;     //5000us延时  
	TL0=(65535-delayTime)%256; 	

	if(PWM_COUNT == PWM_ON)
	{
		//关灯
		LED2 = 0xff;
		LED1 = 0xff;
	}
	else if(PWM_COUNT == light_end)
	{
		//开灯
		LED1 = 0x00;
		LED2 = 0x00;
		PWM_COUNT = 0;
	}
	PWM_COUNT++;
}  