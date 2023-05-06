#include <stdio.h>

//D8-D1 D9-D16
// unsigned char status[18] = {0x00, 0xfe, 0xfd, 0xf7, 0xef, 0xdf, 0xbf, 0x7f, 0x7f, 0xbf, 0xef, 0xf7, 0xfb, 0xfe, 0xff};

unsigned char LED1 = 0xff;
unsigned char LED2 = 0xff;

/**
 * @brief 
 * 坑爹
 * 难道运行完主函数又跳回头重新开始执行了吗
 * 
 */
//重写D0 D1-D8 D9-D16 D17
//默认点亮 D1_8 D9_16
unsigned char status[18] = {
    0x00, 
    0x7f, 0xbf, 0xdf, 0xef, 0xf7, 0xfd, 0xfb, 0xfe,
    0x7f, 0xbf, 0xdf, 0xef, 0xf7, 0xfd, 0xfb, 0xfe,
    0xff
                        };


#define light(i, LED) { \
    unsigned char current_status = (LED); \
    current_status &= status[(i)]; \
    LED = current_status;\
}

int main () {
    unsigned char q = 0x00;
    printf("%x\n", ~(0x7f));
    printf("%x\t%x\t%x\n", q, ~(0x7f), q | (~(0x7f)));
    //预期结果应该是 1 其他的
    printf("%x\n", q | (~0x7f));
    int i = 13;

    if(i <= 8) {
        light(8-i+1, LED2);
    } else {
        light(i, LED1);
    }
/**
 * @brief 
 * 		 
	LED(1) = 0;
	q = LED1;
	q = 0x11110111;
	LED1 = q;		  
	LED_light();
	LED_dark();
    q = LED1;
    q &= 0xfe;
    LED1 = 0xfe;
	//现在我只想点亮第D9而不改变其他灯的状态
	q = LED1;
	q = q & 0x7f;

	LED1 = q;
	LED_light();
	//只熄灭D9而不改变其他灯的状态
	q = LED1;

	q = q | (~(0x7f));
	LED1 = q;
	LED_dark();
	light(11);
	light(12);
 * 
 * 
 */
    return 0;
}