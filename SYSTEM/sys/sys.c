#include "sys.h"

//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK Mini STM32开发板
//系统中断分组设置化		   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2012/9/10
//版本：V1.4
//版权所有，盗版必究。
//Copyright(C) 正点原子 2009-2019
//All rights reserved
//********************************************************************************  
//THUMB指令不支持汇编内联
//采用如下方法实现执行汇编指令WFI  
void WFI_SET(void)
{
	__ASM volatile("wfi");		  
}
//关闭所有中断
void INTX_DISABLE(void)
{		  
	__ASM volatile("cpsid i");
}
//开启所有中断
void INTX_ENABLE(void)
{
	__ASM volatile("cpsie i");		  
}
//设置栈顶地址
//addr:栈顶地址
__asm void MSR_MSP(u32 addr) 
{
    MSR MSP, r0 			//set Main Stack value
    BX r14
}
//设置NVIC 
//NVIC_Priority:抢占优先级;0~15
//NVIC_Priority的原则是,数值越小,越优先	
//请注意，低于os系统优先级的中断受限于OS管理
//高于os系统优先级的中断不受限制，但不能使用OSAPI函数。   
void NVIC_enable(u8 NVIC_Priority,u8 NVIC_Channel)	 
{
	NVIC->IP[NVIC_Channel]|=NVIC_Priority<<4;//设置响应优先级和抢断优先级   
	NVIC->ISER[NVIC_Channel/32]|=(1<<NVIC_Channel%32);//使能对应中断
	//NVIC_Channel是中断名，由官方头文件定义，正常情况下没有修改价值。
	//中断对应的地址是固定的   	    	  				   
}
void NVIC_disable(u8 NVIC_Channel)
{
	NVIC->ICER[NVIC_Channel/32]|=(1<<NVIC_Channel%32);//关闭对应中断
} 