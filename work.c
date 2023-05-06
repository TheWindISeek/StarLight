/**
 * @file work.c/UTF-8
 * @author @TheWindISeek 杨潇康
 * @brief 
 * 需要完成的功能
 *  1.把8个连起来流水
 *  2.流水一圈后开始闪烁
 *  3.流水速度越来越快
 *  4.流水带波浪效果
 * 
 *  5.多机通信
 * 
 * 
 * 重新确定思路
 * 更改思路如下
 * 做以下几个功能
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
 * 
 * 5.多机通信
 *      采用串口通信
 *      接受和发送
 *      待定
 * 
 * @version 0.1
 * @date 2023-05-05
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include <reg52.h>
#include <intrins.h>

#define uint unsigned int
#define uchar unsigned char

#define LED(i) LED##i

// LED
#define LED1_8 P2
#define LED9_16 P0
sbit LED17 = P3 ^ 6;
//无特殊说明 以下LEDi 分别指代 实物上的Di
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

//按键
sbit k1 = P3^2;
sbit k2 = P3^3;
sbit k3 = P3^4;
sbit k4 = P3^5;



//从SBUF接受到的数据
uchar receiveData = '0';
//转换为整数后的数据
int out = -1;

//灯亮度相关
uint light_level_1 = 320;//L1亮度
uint light_level_2 = 80;//L2亮度 L2 > L1
uint light_end = 400;//过后暗
uint light_count = 0;//计数器
uint current = 2;//当前在第几个灯


/**
 * @brief 函数声明
 * 初始化函数
 * 键盘扫描获取函数
 * 模拟延时函数
 * 点亮第i个灯
 * 熄灭第i个灯
 * 点亮所有灯
 * 熄灭所有灯
 * 左流水
 * 右流水
 */
void time0_mode1_init();
void usart_init();
unsigned char key_scan();
void delay(unsigned int ms);
void LED_light_i(unsigned int i);
void LED_dark_i(unsigned int i);
void LED_light(void);
void LED_dark(void);
void LED_leftLisp(unsigned int delayTime);
void LED_rightLisp(unsigned int delayTime);

void LeftDisp();
void RightDisp();
void breatheLight(int x);
void Delay(uint x);
void UsartInit();


void main()
{
    int c;
    UsartInit();
    while (1)
    {
        // (int)receiveData - 48 = receiveData - '0';'0' = 48
        out = (int)(receiveData)-48;
        if (out <= 0)
        {
            continue;
        }
        if (out >= 9)
        {
            breatheLight(5);
            continue;
        }
        c = out % 2;
        out = out + 1;
        switch (c)
        {
            case 0:
                LeftDisp();
                break;
            case 1:
                RightDisp();
                break;
        }
    }
}



//t0 mode1开启 1-65536 自动装载
void time0_mode1_init(void)
{
	TMOD=0x01;//工作方式1
	TH0=(65535-50000)/256;//1-65536
	TL0=(65535-50000)%256;      

	EA=1;       //开总中断  
	ET0=1;      //开定时器0中断  
	TR0=1;      //启动定时器0  
}

void Usart_init() {
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

unsigned char key_scan() {
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
void delay(unsigned int ms) {
    unsigned int j;
    while (ms--)
    {
        for(j = 0; j < 110; ++j)
            ;
    }
    
}
void LED_light_i(unsigned int i) {
    if(i <= 0 || i >= 18) return;
    
    if(i == 17) {
        LED17 = 0;
        return;
    }

    if(i <= 8) {
        //LED2
        (8-i);
        LED1^7 = 0;
    } else {
        //LED1

    }
}
void LED_dark_i(unsigned int i) {

}
void LED_light(void) {
    LED1_8 = 0x00;
    LED9_16 = 0x00;
    LED17 = 0;
}
void LED_dark(void) {
    LED1_8 = 0xff;
    LED9_16 = 0xff;
    LED17 = 1;
}
void LED_leftLisp(unsigned int delayTime);
void LED_rightLisp(unsigned int delayTime);



// 顺时针旋转半圈
void LeftDisp()
{
    uint i = 0;
    a = 0xf7;
    b = 0xfe;
    v = 517 - (40 * out);
    while (i < 9)
    {
        // 控制D1~D8的流水
        if (i < 4 && i >= 0)
        {
            //设置左边灯的亮灭
            P2 = a;
            //延时V
            Delay(v);
            //shift a
            a = _cror_(a, 1);
        }
        // D17
        else if (i == 4)
        {
            // i=0;
            P2 = 0xff;
            led17 = 0;
            Delay(v);
            led17 = 1;
        }
        //D9-D16
        else if (i > 4)
        {
            P0 = b;
            Delay(v);
            b = _crol_(b, 1);
            if (i == 8)
            {
                P0 = 0xff;
            }
        }
        i++;
        v -= 5;
    }
    TI = 1;
    printf("%d", out);
    while (!TI)
        ;
    TI = 0;
}
void RightDisp() // 逆时针旋转半圈
{
    uint i = 0;
    v = 517 - (40 * out);
    a = 0xf7;
    b = 0x7f;
    while (i < 10)
    {
        // 控制D1~D8的流水
        if (i < 5 && i >= 0)
        {
            P2 = a;
            Delay(v);
            a = _crol_(a, 1);
        }
        // D17
        if (i >= 5)
        {
            P2 = 0xff;
            P0 = b;
            Delay(v);
            b = _cror_(b, 1);
            if (i == 9)
            {
                P0 = 0xff;
            }
        }
        i++;
        v -= 5;
    }
    TI = 1;
    printf("%d", out);
    while (!TI)
        ;
    TI = 0;
}

void breatheLight(int x) {
    int i, j, k, m;
    while (1) {
        for (k = x; k >= 2; k--) {
            for (m = 3; m > 0; m--) {
                // 增加亮度
                for (i = 1; i <= 20; i++) {
                    for (j = 0; j < k; j++) {
                        P0 = 0xff; // 控制LED亮度
                        P2 = 0xff;
                        led17 = 0xff;
                        Delay(i); // 延时ims，调节呼吸间隔
                        P0 = 0x00;
                        P2 = 0x00;
                        led17 = 0x00;
                        Delay(20 - i);
                    }
                }
                // 增加亮度
                for (i = 20; i >= 1; i--) {
                    for (j = 0; j < k; j++){
                        P0 = 0xff; // 控制LED亮度
                        P2 = 0xff;
                        led17 = 0xff;
                        Delay(i); // 延时i ms，调节呼吸间隔
                        P0 = 0x00;
                        P2 = 0x00;
                        led17 = 0x00;
                        Delay(20 - i);
                    }
                }
            }
        }
    }
}

void Delay(uint x)
{
    uint j;
    for (; x > 0; x--)
    {
        for (j = 110; j > 0; j--)
            ;
    }
}

void UsartInit()
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

void Usart() interrupt 4 // 接口中断
{
    RI = 0; // 手动清零
    receiveData = SBUF;//SBUF 串口通信寄存器 写和读寄存器物理不一致 使用一致
}