/**
 * Craeted by RainSure at 2022/09/29
 * Modified by JeffreySharp at 2023/5/4
 *
 **/
#include <reg52.h>

#define LED1 P2
#define LED2 P0
sbit LED17 = P3 ^ 6;

char status1[18] = {0x00, 0xfe, 0xfd, 0xfb, 0xf7, 0xef, 0xdf, 0xbf, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff}; // 控制LED1
char status2[18] = {0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x7f, 0xbf, 0xdf, 0xef, 0xf7, 0xfb, 0xfd, 0xfe, 0xff}; // 控制LED2
void delay(unsigned int ms)
{
    unsigned int j;
    while (ms--)
        for (j = 0; j < 110; ++j)
            ;
}
void Bright() // 保持常亮
{
    LED1 = 0x00;
    LED2 = 0x00;
    LED17 = 0;
}

void dark()
{
    LED1 = 0xff;
    LED2 = 0xff;
    LED17 = 1;
}

void Flash() // 保持闪烁
{
    unsigned int i;
    for (i = 0; i < 20; ++i)
    {

        LED1 = 0x00;
        LED2 = 0x00;
        LED17 = 0;
        delay(100);

        LED1 = 0xff;
        LED2 = 0xff;
        LED17 = 1;
        delay(100);
    }
}

void D1toD17() // D1到D17流水
{
    int i;
    for (i = 1; i <= 17; i++)
    {
        LED1 = status1[i];
        LED2 = status2[i];
        if (i == 17)
            LED17 = 0;
        else
            LED17 = 1;
        delay(200);
    }
    dark();
}

void D17toD1() // D17到D1流水
{
    int i;
    for (i = 17; i >= 1; i--)
    {
        LED1 = status1[i];
        LED2 = status2[i];
        if (i == 17)
            LED17 = 0;
        else
            LED17 = 1;
        delay(200);
    }
    dark();
}

void initDate()
{
    SCON = 0x50; // 数据位8位，停止位1位，起始位1位
    // 波特率设置
    TMOD = 0x20; // 用到定时/计数器的方式2
    TH1 = 0xFD;
    TL1 = 0xFD;
    TR1 = 1; // 启动定时器
}

void recived()
{
    char commit;

    while (1)
    {
        if (RI == 1)
        {
            RI = 0;
            commit = SBUF;
            if (commit == '1')
            {
                Bright();
            }
            if (commit == '2')
            {
                Flash();
            }
            if (commit == '3')
            {
                D1toD17();
            }
            if (commit == '4')
            {
                D17toD1();
            }
        }
    }
}

void main()
{
    while (1)
    {
        initDate();
        recived();
    }
}
