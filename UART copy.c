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

//LED
#define LED1 P2
#define LED2 P0
sbit LED17 = P3^6;
//按键
sbit KEY1  = P3^2;
sbit KEY2  = P3^3;
sbit KEY3  = P3^4;
sbit KEY4  = P3^5;


//
unsigned char P2Temp[]={0x7f,0x3f,0x1f,0x0f,0x07,0x03,0x01,0x00,0xfe,0xfc,0xf8,0xf0,0xe0,0xc0,0x80,0x00};
//串口接收到的数据 
uchar receiveData = '0';
//5000us延时
uint delayTime = 5000;

// PWM_ON:开灯的边界值	PWM关灯的边界值 PWM_COUNT计数
uint PWM_ON = 0;
uint light_end = 180;
uint light_count = 0;
uint PWM_LEVEL = 60;

void time0_mode0_init(void);
void time0_mode1_init(void);
void ser_init();
void delay(unsigned int i);
void LED_light();
void LED_blink();
void LED_leftDisp();
void LED_rightDisp();

void main() {  
	//选择t0的工作模式
	#ifdef Time0_Mode 
		time0_mode0_init();
	#else
		time0_mode1_init();
	#endif
	//选择串口工作方式
	ser_init();

    while(1) {
        switch(receiveData) {
			//LED全亮
            case '1':
				LED_light();
                break;
			//LED全闪烁
            case '2':
				LED_blink();
                break;
			//LED1->LED17流水
            case '3':
                LED_leftDisp();
				break;
			//LED17->LED1流水
            case '4':
				LED_rightDisp();
                break;
            default:
                break;
        }
        delay(30);
    }
}

//led全亮
void LED_light() {
	LED1 = 0xff;
	LED2 = 0xff;
	LED17 = 1;
}
//led全灭
void LED_dark() {
	LED1 = 0x00;
	LED2 = 0x00;
	LED17 = 0;
}
//led闪烁
void LED_blink() {
	PWM_ON = 20;
	light_end = 40;
}
//led1->17流水
void LED_leftDisp() {

}
//led17->1流水
void LED_rightDisp() {

}

//t0 中断函数; interrup 1 表示中断号;1 表示t0的中断
void time0() interrupt 1  
{  
	//中断触发后 重新置位
	#ifdef Time0_Mode0 
		TH0=(8192-delayTime)/32;     //5000us延时  
		TL0=(8192-delayTime)%32;
	#else
		TH0=(65535-delayTime)/256;     //5000us延时  
		TL0=(65535-delayTime)%256; 	
	#endif

    //中断一次计数一次
	PWM_COUNT++;
	if(PWM_COUNT == PWM_ON) {
		LED_light();
	} else if(PWM_COUNT == PWM_FRE) {
		LED_dark();
		PWM_COUNT = 0;
	}
}

//串口中断
void ser_interrupt() interrupt 4     //中断函数不用声明    
{  
    if(RI == 1)  
    {  
        RI = 0;  
        receiveData = SBUF;
//		SBUF = receive_ser;
//		while(!TI);
	}           
} 



void ser_init() {
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

//t0 mode0开启 1-8192 自动装载
void time0_mode0_init(void)
{
	TMOD=0x00;  //设置T0为工作方式0  
	TH0=(8192-delayTime)/32;     //5000us延时  
	TL0=(8192-delayTime)%32;     
	/*Timer0工作在模式0下时为13位定时/计数器， 
	TL0只使用前5位,故对32进行求余/模    */  

	EA=1;       //开总中断  
	ET0=1;      //开定时器0中断  
	TR0=1;      //启动定时器0  
}

//t0 mode1开启 1-65536 自动装载
void time0_mode1_init(void)
{
	TMOD=0x01;//工作方式1
	TH0=(65535-delayTime)/256;//1-65536
	TL0=(65535-delayTime)%256;      

	EA=1;       //开总中断  
	ET0=1;      //开定时器0中断  
	TR0=1;      //启动定时器0  
}

void delay(unsigned int i){
    unsigned int j;
    for(;i>0;i--)
        for(j=110;j>0;j--);
}