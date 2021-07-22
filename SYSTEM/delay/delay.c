#include "delay.h"
#include "sys.h"
#include "data.h"
#include "usart.h"
////////////////////////////////////////////////////////////////////////////////// 	 
/*
author:onceday,date:2021.5.15
(1)不使用滴答计时器，因为在中断中存在寄存器被重写的情况
(2)使用定时器查询模式来专门设计10us级别延迟，更高及ms级别应该使用OS定时器
(3)定时器3作为参考寄存器
*/
///////////////////////////////////////////////////////////////////////////////////

u32 Delay_count=0;

//该系统滴答计数器，输入频率sysclk/8=9Mhz，计数90000，即每10ms一次中断。	   
void delay_init(void)//采用滴答计时器搞定
{
	SysTick->LOAD=90000;//9000000Hz 数90000次，频率100Hz
	SysTick->VAL=0;//清零计数器
	SysTick->CTRL|=3;//启动计数器并且使能中断
}

void SysTick_Handler(void)
{
  u32 flag=SysTick->CTRL;
	Delay_count++;//10ms加一次，总计时为2400万秒
}
//不能轻易地使用寄存器模式，因为你无法判断会不会被重写

u32 get_ms(void)
{
	return (90000-SysTick->VAL)/9000+Delay_count*10;//返回ms级别时间
}

u32 get_us(void)
{
	return (90000-SysTick->VAL)/9+Delay_count*10000;//返回us级别时间
}

void delay_ms(u32 ms)
{		
	 int i;
	 u32 utimes=ms*1000;
	 delay_us(utimes);
}  
//延时nus,
//nus:要延时的us数.	
//nus:不能超过100us,变量类型限制住了	    								   
void delay_us(u32 nus)
{		
	u32 last_count=Delay_count;
	u32 last_cnt=(90000-SysTick->VAL);
	while(((Delay_count-last_count)*90000+(90000-SysTick->VAL)-last_cnt)<nus*9);						    
}  
			 
