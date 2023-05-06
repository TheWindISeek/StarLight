/**
 * @file breath.c/UTF-8
 * @author @TheWindISeek
 * @brief 实现LED的呼吸效果 按键调节亮度
 * @version 0.1
 * @date 2023-05-03
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include <reg52.h>

#define uint unsigned int
#define uchar unsigned char

//LED灯
#define LED1 P0
#define LED2 P2
sbit LED17 = P3^6;

//按键
sbit k1 = P3^2;
sbit k2 = P3^3;
sbit k3 = P3^4;
sbit k4 = P3^5;

//50us延时
// uint delayTime = 50;
#define delayTime 5

// PWM_ON:开灯的边界值	PWM关灯的边界值 PWM_COUNT计数 PWM_LEVEL每一个等级开灯的持续时间的变化范围  PWM_NUM 一个循环内 需要改变多少次亮度
//随着PWM_ON的变大 灯的亮度越来越低
uint PWM_ON = 80;
uint light_end = 400;
uint light_count = 0;

uint PWM_LEVEL = 100;
uint PWM_NUM = 10;


void delay(unsigned int i);
unsigned char keyscan(void);
void Time0_Mode1(void);
void LED_light(void);
void LED_dark(void);

void main()  
{  
	uchar key = 0;
	Time0_Mode1();

    while(1){
        key=keyscan();
        
        switch(key) {
            case 1:
				PWM_ON = (PWM_ON + PWM_LEVEL >= light_end) ? PWM_ON : PWM_ON + PWM_LEVEL;
                break;
            case 2:
				PWM_ON = (PWM_ON - PWM_LEVEL <= PWM_LEVEL) ? PWM_ON : PWM_ON - PWM_LEVEL;
                break;
            case 3:
                break;
            case 4:
                break;
            default:
                break;
        }
    }
} 

//led全亮
void LED_light() {
	LED1 = 0x00;
	LED2 = 0x00;
	LED17 = 0;
}
//led全灭
void LED_dark() {
	LED1 = 0xff;
	LED2 = 0xff;
	LED17 = 1;
}

//t0 mode1开启 1-65536 自动装载
void Time0_Mode1(void)
{
	TMOD=0x01;//工作方式1
	TH0=(65535-50000)/256;//1-65536
	TL0=(65535-50000)%256;      

	EA=1;       //开总中断  
	ET0=1;      //开定时器0中断  
	TR0=1;      //启动定时器0  
}

//t0 中断函数; interrup 1 表示中断号;1 表示t0的中断
void Time0() interrupt 1  
{  
	//中断触发后 重新置位
	TH0=(65535-delayTime)/256;
	TL0=(65535-delayTime)%256; 	

    //中断一次计数一次
	PWM_COUNT++;
    //达到开的状态 将所有的灯打开
	if(PWM_COUNT == PWM_ON)
	{
        LED_light();
	}
    //达到关的状态 关闭所有的灯 根据开的时间长短来控制最后的效果
	else if(PWM_COUNT == light_end)
	{
        LED_dark();
		//重新开始计数
		PWM_COUNT = 0;
	}
}

void delay(unsigned int i){
    for(;i>0;i--);
}

unsigned char keyscan(void){
    unsigned char key = 0;
    if(k1 == 0){
        delay(1);
        if(k1==0){
            key=1;
            while(!k1);
        }
    }

    if(k2==0){
        delay(1);
        if(k2==0){
            key=2;
            while(!k2);
        }
    }
    if (k3==0)
    {
        delay(1);
        if(k3== 0){
            key=3;
            while(!k3);
        }
    }
    if(k4==0){
        delay(1);
        if(k4==0){
            key=4;
            while(!k4);
        }
    }

    return key;
}