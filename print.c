//51多种串口打印数据方式合集

#include <reg52.h> //包含头文件，一般情况不需要改动，头文件包含特殊功能寄存器的定义                       
#include <intrins.h>
#include <stdio.h>   //用于printf
#include <stdarg.h>  //用于vsprintf函数原型 

/*------------------------------------------------
                   函数声明
------------------------------------------------*/
void SendStr(unsigned char *s);
void uart_printf(const char *fmt,...);

/*------------------------------------------------
                    串口初始化
------------------------------------------------*/
void InitUART  (void)
{

    SCON  = 0x50;          // SCON: 模式 1, 8-bit UART, 使能接收 
    TMOD |= 0x20;               // TMOD: timer 1, mode 2, 8-bit 重装
    TH1   = 0xFD;               // TH1:  重装值 9600 波特率 晶振 11.0592MHz 
    TR1   = 1;                  // TR1:  timer 1 打开                        
    EA    = 1;                  //打开总中断
    // ES    = 1;                  //打开串口中断
}                           
/*------------------------------------------------
                    主函数
------------------------------------------------*/
void main (void)
{

//  int a=99;
 
 InitUART();

//  SendStr("UART te 请在发送区输入任意信息");
//  uart_printf("10进制:%d \n",a); 
 
   /*ES=0;
 TI=1;
 printf("%d\n",a);
 ES=1;*/

 ES    = 1;                  //打开串口中断
 while (1)                      
 {
  SendByte('a');

 }
}

/*------------------------------------------------
                    发送一个字节
------------------------------------------------*/
void SendByte(unsigned char c) 
{ 
    if(c=='\n')//如果遇到\n就换行 
    { 
        //发送CR(carriage return) 
        SBUF=0x0D;  
        while(!TI);//等待发送完成 
        TI=0; 
         
        //发送 LF(NL line feed,new line) 
        SBUF=0x0A;  
        while(!TI);//等待发送完成 
        TI=0;        
    } 
    else 
    { 
        SBUF=c; 
        while(!TI);//等待发送完成 
        TI=0; 
    } 
} 

/*------------------------------------------------
                    发送一个字符串
------------------------------------------------*/
void SendStr(unsigned char *s)
{
 while(*s!='\0')// \0 表示字符串结束标志，通过检测是否字符串末尾
  {
  SendByte(*s);
  s++;
  }
}

/*------------------------------------------------
                    发送一个指定类型数据
------------------------------------------------*/
void uart_printf(const char *fmt,...) 
{ 
 va_list ap; 
 char xdata string[1024];//访问内部拓展RAM，非访问外部RAM，不能超过内部拓展RAM大小(此处为1024) 

 va_start(ap,fmt); 
 vsprintf(string,fmt,ap);//此处也可以使用sprintf函数，用法差不多，稍加修改即可，此处略去 
 SendStr(string); 
 va_end(ap); 
} 

/*------------------------------------------------
                     串口中断程序
------------------------------------------------*/
void UART_SER (void) interrupt 4 //串行中断服务程序
{
  unsigned char Temp;          //定义临时变量
  
  if(RI)                        //判断是接收中断产生
  {
      RI = 0;                      //标志位清零
      Temp = SBUF;                 //读入缓冲区的值
      P1 = Temp;                   //把值输出到P1口，用于观察
      SBUF = Temp;                 //把接收到的值再发回电脑端
  }
   if(TI)                        //如果是发送标志位，清零
     TI=0;
}