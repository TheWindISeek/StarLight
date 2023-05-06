/**
 * @file UART.c
 * @author @TheWindISeek
 * @brief 串口调试助手控制心型灯
 *          发送'1' LED全亮
 *          发送'2' LED全闪烁
 *          发送'3' LED1->LED17流水
 *          发送'4' LED17->LED1流水
 * @encoding utf-8
 * @version 0.1
 * @date 2023-05-03
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include <reg52.h>

#define uint  unsigned int  
#define uchar unsigned char  

#define LED1 P2
#define LED2 P0

sbit KEY1  = P3^2;
sbit KEY2  = P3^3;
sbit KEY3  = P3^4;
sbit KEY4  = P3^5;
sbit LED17 = P3^6;

unsigned char P2Temp[]={0x7f,0x3f,0x1f,0x0f,0x07,0x03,0x01,0x00,0xfe,0xfc,0xf8,0xf0,0xe0,0xc0,0x80,0x00};
uchar Flow_Mode;     //串口接收到的数据  

void init_ser()  
{   
    TMOD=0X20;//定时器1工作在方式2    
    TH1=0XFD;  
    TL1=0XFD;  //波特率9600,51单片机大多用这个  
    TR1=1;  //定时器/计数器1启动  
    EA=1;    //开放总中断  
    SM0=0;  
    SM1=1;   //串行口方式1  
    REN=1;//允许串口接收  
    ES=1;//开串口 中断  
}


void Delay(unsigned int ms)
{
	unsigned int i,j;
	for(i=0;i<ms;i++)
		for(j=0;j<110;j++)
	;
}

void KEY_Scan(void)
{
	if(KEY1 == 0)
	{
		Delay(1);
		if(KEY1 ==0)
		{
			Flow_Mode = 1;
			while(!KEY1);
		}
	}
	if(KEY2 == 0)
	{
		Delay(1);
		if(KEY2 ==0)
		{
			Flow_Mode = 2;
			while(!KEY2);
		}
	}
	if(KEY3 == 0)
	{
		Delay(1);
		if(KEY3 ==0)
		{
			Flow_Mode = 3;
			while(!KEY3);
		}
	}
	if(KEY4 == 0)
	{
		Delay(1);
		if(KEY4 ==0)
		{
			Flow_Mode = 4;
			while(!KEY4);
		}
	}	
}

void main()  
{  
	char i=0;
	unsigned char Old_Mode;
    init_ser();  
	Flow_Mode = 0;
    while(1)  
    {  
		KEY_Scan();
		if(i<8)
		{
			LED17 = 1;
			LED2 = 0xff;
			LED1 = P2Temp[i];

		}
		else if(i==8)
		{
			LED17 =0;
			LED1 = 0x00;
			LED2 = 0xff;
		}
		else if(i>8)
		{
			LED17 = 0;
			LED2 = P2Temp[i-1];
			LED1 = 0x00;
		}
		Delay(10);
		SBUF = i;		 
		switch(Flow_Mode)
		{
			case 1:
				i++;
				if(i==17)
					i=0;
				Old_Mode = 1;
			break;
				
			case 2:
				i--;
				if(i<0)
					i=16;
				Old_Mode = 2;
			break;		
			
			case 3:				
			break;
			
			case 4:
				Flow_Mode = Old_Mode;
			break;	
			
			default:
				Flow_Mode = 1;
			break;
		}
    }    
}

void ser_int() interrupt 4     //中断函数不用声明    
{  
    if(RI==1)  
    {  
        RI=0;  
        Flow_Mode=SBUF;    
//		SBUF = receive_ser;
//		while(!TI);
	}           
} 