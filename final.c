/**
 * @file work.c/UTF-8
 * @author @TheWindISeek 杨潇康
 * @brief
 * 首先是做波浪的效果，背景音乐放黄河大合唱
 * 然后由电脑发送1，开始闪烁，背景放那种紧急急迫的音乐。
 * 然后电脑发送2，整个灯开始呼吸，放同呼吸啥的哥。
 * 再然后电脑发送3，开始流水，从17到9和1，背景音乐放我和你。
 * 然后最后发送4，做一个整体的流水，从第一个开始到最后，背景音乐放祖国统一。
 * 之后亮度不断变亮，寓意着国家越来越好。
 * 就说我们这个寓意的是抗战
 * 先黄河大合唱。。。
 * 然后中华民族到了危机关头
 * 然后整个国家同呼吸共命运
 * 然后我和你一块战斗
 * 然后祖国统一
 *
 * 完成思路
 *  1.确定函数
 *  2.与LED相关函数命名为
 *      LED_op()
 *      例如
 *      LED_light()全亮
 * 3.流水速度越来越快
 *      LED_leftLisp(unsigned int delayTime);//每两个灯之间的延迟时间
 *      LED_rightLisp(unsigned int delayTime);//同上
 * 4.流水带波浪效果
 *      17个灯
 *      第i个灯亮度为L1
 *      第i-1个灯亮度为L2
 *      L2 > L1
 *      1 < i <= 17
 *      函数实现思路
 *          两个on变量
 *          一个固定的stop常量
 *          每次切换时不再让灯全亮 而是控制单个灯的亮灭
 *          LED_light_i(unsigned int i);//点亮第i个灯(1<= i <= 17)
 * 5.呼吸灯
 * 6.亮度不断变亮
 * 7.按下按键进行流水
 *
 * @version 0.2
 * @date 2023-05-05
 *
 * @copyright Copyright (c) 2023
 *
 */
#include <reg52.h>

#define uint unsigned int
#define uchar unsigned char

#define delayTime 5
// LED
#define LED1_8 P2
#define LED9_16 P0
sbit LED17 = P3 ^ 6;
// 无特殊说明 以下LEDi 分别指代 实物上的Di
sbit LED1 = LED1_8 ^ 7;
sbit LED2 = LED1_8 ^ 6;
sbit LED3 = LED1_8 ^ 5;
sbit LED4 = LED1_8 ^ 4;
sbit LED5 = LED1_8 ^ 3;
sbit LED6 = LED1_8 ^ 2;
sbit LED7 = LED1_8 ^ 1;
sbit LED8 = LED1_8 ^ 0;
sbit LED9 = LED9_16 ^ 7;
sbit LED10 = LED9_16 ^ 6;
sbit LED11 = LED9_16 ^ 5;
sbit LED12 = LED9_16 ^ 4;
sbit LED13 = LED9_16 ^ 3;
sbit LED14 = LED9_16 ^ 2;
sbit LED15 = LED9_16 ^ 1;
sbit LED16 = LED9_16 ^ 0;

// 按键
sbit k1 = P3 ^ 2;
sbit k2 = P3 ^ 3;
sbit k3 = P3 ^ 4;
sbit k4 = P3 ^ 5;

// 从SBUF接受到的数据
uchar receiveData = '0';

// 灯亮度相关
uint is_wave = 0;   // 波浪
uint is_breath = 0; // 呼吸
uint light_level = 320;
uint light_level_1 = 320; // L1亮度
uint light_level_2 = 80;  // L2亮度 L2 > L1
uint light_end = 400;     // 过后暗
uint light_i = 80;
uint light_count = 0; // 计数器
uint current = 2;     // 当前在第几个灯

// D0 D1-D8 D9-D16 D17
unsigned char status[18] = {
    0x00,
    0x7f, 0xbf, 0xdf, 0xef, 0xf7, 0xfb, 0xfd, 0xfe,
    0x7f, 0xbf, 0xdf, 0xef, 0xf7, 0xfb, 0xfd, 0xfe,
    0xff};

void time0_mode1_init();     // time0 mode1 初始化
void usart_init();           // 串口初始化
unsigned char key_scan();    // 按键获取
void delay(unsigned int ms); // 模拟延迟

void LED_light_i(unsigned int i);            // 点亮第i个灯
void LED_dark_i(unsigned int i);             // 关闭第i个灯
void LED_light(void);                        // 全部点亮
void LED_dark(void);                         // 全部熄灭
void LED_leftLisp(unsigned int delay_time);  // 左流水D16 -> D9
void LED_rightLisp(unsigned int delay_time); // 右流水D8 -> D1

void LED_wave();       // 波浪
void LED_breath();     // 呼吸
void LED_blink();      // 闪烁
void LED_disp();       // 17-9 17-1 流水
void LED_asynDisp();   // 按下按键1一个方向 按下2一个方向
void LED_dark2light(); // 不断变亮

void breatheLight(int x);

void main()
{
    usart_init();
    time0_mode1_init();
    while (1)
    {
       // LED_wave();
        switch (receiveData)
        {
        case '0':
            // 波浪的效果
            LED_wave();
            break;
        case '1':
            // 呼吸
            LED_breath();
            break;
        case '2':
            // 闪烁
            LED_blink();
            break;
        case '3':
            // 17-9 17-1流水
            LED_disp();
            break;
        case '4':
            // 异步流水 具体实现 按下按键1一个方向 按下2一个方向
            LED_asynDisp();
            break;
        case '5':
            // 不断变亮
            LED_dark2light();
            break;
        case '6':
            break;
        default:
            break;
        }
    }
}
// 波浪
void LED_wave()
{
    uint i;
    is_wave = 1;
    LED_dark();
    for (i = 9; i <= 17; ++i)
    {
        current = i;
        delay(800);
    }
    for (i = 8; i >= 3; --i)
    {
        current = i;
        delay(800);
    }
    LED_light();
    is_wave = 0;
    receiveData = '0'; // 切换状态
}
// 呼吸
void LED_breath()
{
    breatheLight(10);
    receiveData = '2';
}
// 闪烁
void LED_blink()
{
    uint i;

    for (i = 0; i < 8; i++)
    {
        LED_light();
        delay(1000);
        LED_dark();
        delay(1000);
    }

    receiveData = '3';
}
// D17 D9 D1流水
void LED_disp()
{
    uint i;

    LED_dark();
    delay(10000);
    LED_light_i(17);
    for (i = 8; i >= 1; --i)
    {
        LED_light_i(i);
        LED_light_i(8 + i);
        delay(1000);
    }

    receiveData = '4';
}
// 按键控制流水
void LED_asynDisp()
{

    receiveData = '5';
}
// 逐渐变亮
void LED_dark2light()
{
    uint i;

    is_breath = 1;
    for (i = 0; i <= 3; ++i)
    {
        light_level -= i * light_i;
        delay(10000);
    }

    receiveData = '0' - 1;
}

// t0 mode1开启 1-65536 自动装载
void time0_mode1_init(void)
{
    TMOD = 0x01;                 // 工作方式1
    TH0 = (65535 - 50000) / 256; // 1-65536
    TL0 = (65535 - 50000) % 256;

    EA = 1;  // 开总中断
    ET0 = 1; // 开定时器0中断
    TR0 = 1; // 启动定时器0
}

void usart_init()
{
    SM0 = 0;
    SM1 = 1;      // 串口工作方式1,  8位UART波特率可变
    TR1 = 1;      // 启动定时器1
    TMOD |= 0x20; // 定时器1，工作模式2 8位自动重装

    // SCON=0X50;//设置为工作方式1
    //  TMOD = 0x20;//设置计数器工作方式2
    // PCON=0X80;//波特率加倍

    TH1 = 0xFD; // 计数器初值设置，波特率为9600
    TL1 = 0xFD;
    REN = 1;
    TR1 = 1; // 初始化串口通信，打开计数器
    ES = 1;  // 打开接收中断
    EA = 1;  // 打开总中断
}

unsigned char key_scan()
{
    unsigned char key = 0;
    if (k1 == 0)
    {
        delay(1);
        if (k1 == 0)
        {
            key = 1;
            while (!k1)
                ;
        }
    }

    if (k2 == 0)
    {
        delay(1);
        if (k2 == 0)
        {
            key = 2;
            while (!k2)
                ;
        }
    }
    if (k3 == 0)
    {
        delay(1);
        if (k3 == 0)
        {
            key = 3;
            while (!k3)
                ;
        }
    }
    if (k4 == 0)
    {
        delay(1);
        if (k4 == 0)
        {
            key = 4;
            while (!k4)
                ;
        }
    }

    return key;
}

void delay(unsigned int ms)
{
    while (ms--)
        ;
}

void LED_light_i(unsigned int i)
{
    unsigned char current_status;
    if (i > 17 || i < 0)
        return;

    if (i == 17)
    {
        LED17 = 0;
    }
    else
    {
        if (i <= 8)
        {
            current_status = LED1_8;
            current_status &= status[i];
            LED1_8 = current_status;
        }
        else
        {
            current_status = LED9_16;
            current_status &= status[i];
            LED9_16 = current_status;
        }
    }
}

void LED_dark_i(unsigned int i)
{
    unsigned char current_status;
    if (i > 17 || i < 0)
        return;

    if (i == 17)
    {
        LED17 = 0;
    }
    else
    {
        if (i <= 8)
        {
            current_status = LED1_8;
            current_status |= ~status[i];
            LED1_8 = current_status;
        }
        else
        {
            current_status = LED9_16;
            current_status |= ~status[i];
            LED9_16 = current_status;
        }
    }
}

void LED_light(void)
{
    LED1_8 = 0x00;
    LED9_16 = 0x00;
    LED17 = 0;
}

void LED_dark(void)
{
    LED1_8 = 0xff;
    LED9_16 = 0xff;
    LED17 = 1;
}

void LED_leftLisp(unsigned int delay_time)
{
    uint i;
    for (i = 16; i >= 9; --i)
    {
        LED_light_i(i);
        delay(delay_time);
    }
}

void LED_rightLisp(unsigned int delay_time)
{
    uint i;
    for (i = 8; i >= 1; --i)
    {
        LED_light_i(i);
        delay(delay_time);
    }
}

void breatheLight(int x)
{
    int i, j, k, tms = 10;
    while (tms--)
    {
        for (k = x; k >= 2; k--)
        {
            // 增加亮度
            for (i = 5; i <= 15; i++)
            {
                for (j = 0; j < k; j++)
                {
                    LED_dark();
                    delay(i); // 延时ims，调节呼吸间隔
                    LED_light();
                    delay(20 - i);
                }
            }
            // 增加亮度
            for (i = 20; i >= 1; i--)
            {
                for (j = 0; j < k; j++)
                {
                    LED_dark();
                    delay(i); // 延时i ms，调节呼吸间隔
                    LED_light();
                    delay(20 - i);
                }
            }
        }
    }
}

void Usart() interrupt 4
{
    RI = 0; // 手动清零
    receiveData = SBUF;
}

void Time0() interrupt 1
{
    TH0 = (65535 - delayTime) / 256;
    TL0 = (65535 - delayTime) % 256;

    if (!is_wave && !is_breath)
        return;

    light_count++;
    if (is_wave)
    {
        if (light_count == light_level_2)
        {
            LED_light_i(current);
        }
        if (light_count == light_level_1)
        {
            if (current == 16)
            {
                LED_light_i(17);
                LED_light_i(8);
            }
            else if (current == 17)
            {
                LED_light_i(8);
                LED_light_i(7);
            }
            else if (current >= 9)
            {
                LED_light_i(current + 1);
                LED_light_i(current + 2);
            }
            else
            {
                LED_light_i(current - 1);
                LED_light_i(current - 2);
            }
        }
    }
    if (is_breath)
    {
        if (light_count == light_level)
        {
            LED_light();
        }
    }

    if (light_count == light_end)
    {
        LED_dark();
        light_count = 0;
    }
}