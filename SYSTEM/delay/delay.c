#include "delay.h"
#include "sys.h"
#include "data.h"
#include "usart.h"
////////////////////////////////////////////////////////////////////////////////// 	 
/*
author:onceday,date:2021.5.15
(1)��ʹ�õδ��ʱ������Ϊ���ж��д��ڼĴ�������д�����
(2)ʹ�ö�ʱ����ѯģʽ��ר�����10us�����ӳ٣����߼�ms����Ӧ��ʹ��OS��ʱ��
(3)��ʱ��3��Ϊ�ο��Ĵ���
*/
///////////////////////////////////////////////////////////////////////////////////

u32 Delay_count=0;

//��ϵͳ�δ������������Ƶ��sysclk/8=9Mhz������90000����ÿ10msһ���жϡ�	   
void delay_init(void)//���õδ��ʱ���㶨
{
	SysTick->LOAD=90000;//9000000Hz ��90000�Σ�Ƶ��100Hz
	SysTick->VAL=0;//���������
	SysTick->CTRL|=3;//��������������ʹ���ж�
}

void SysTick_Handler(void)
{
  u32 flag=SysTick->CTRL;
	Delay_count++;//10ms��һ�Σ��ܼ�ʱΪ2400����
}
//�������׵�ʹ�üĴ���ģʽ����Ϊ���޷��жϻ᲻�ᱻ��д

u32 get_ms(void)
{
	return (90000-SysTick->VAL)/9000+Delay_count*10;//����ms����ʱ��
}

u32 get_us(void)
{
	return (90000-SysTick->VAL)/9+Delay_count*10000;//����us����ʱ��
}

void delay_ms(u32 ms)
{		
	 int i;
	 u32 utimes=ms*1000;
	 delay_us(utimes);
}  
//��ʱnus,
//nus:Ҫ��ʱ��us��.	
//nus:���ܳ���100us,������������ס��	    								   
void delay_us(u32 nus)
{		
	u32 last_count=Delay_count;
	u32 last_cnt=(90000-SysTick->VAL);
	while(((Delay_count-last_count)*90000+(90000-SysTick->VAL)-last_cnt)<nus*9);						    
}  
			 
