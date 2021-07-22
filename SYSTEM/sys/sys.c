#include "sys.h"

//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK Mini STM32������
//ϵͳ�жϷ������û�		   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/9/10
//�汾��V1.4
//��Ȩ���У�����ؾ���
//Copyright(C) ����ԭ�� 2009-2019
//All rights reserved
//********************************************************************************  
//THUMBָ�֧�ֻ������
//�������·���ʵ��ִ�л��ָ��WFI  
void WFI_SET(void)
{
	__ASM volatile("wfi");		  
}
//�ر������ж�
void INTX_DISABLE(void)
{		  
	__ASM volatile("cpsid i");
}
//���������ж�
void INTX_ENABLE(void)
{
	__ASM volatile("cpsie i");		  
}
//����ջ����ַ
//addr:ջ����ַ
__asm void MSR_MSP(u32 addr) 
{
    MSR MSP, r0 			//set Main Stack value
    BX r14
}
//����NVIC 
//NVIC_Priority:��ռ���ȼ�;0~15
//NVIC_Priority��ԭ����,��ֵԽС,Խ����	
//��ע�⣬����osϵͳ���ȼ����ж�������OS����
//����osϵͳ���ȼ����жϲ������ƣ�������ʹ��OSAPI������   
void NVIC_enable(u8 NVIC_Priority,u8 NVIC_Channel)	 
{
	NVIC->IP[NVIC_Channel]|=NVIC_Priority<<4;//������Ӧ���ȼ����������ȼ�   
	NVIC->ISER[NVIC_Channel/32]|=(1<<NVIC_Channel%32);//ʹ�ܶ�Ӧ�ж�
	//NVIC_Channel���ж������ɹٷ�ͷ�ļ����壬���������û���޸ļ�ֵ��
	//�ж϶�Ӧ�ĵ�ַ�ǹ̶���   	    	  				   
}
void NVIC_disable(u8 NVIC_Channel)
{
	NVIC->ICER[NVIC_Channel/32]|=(1<<NVIC_Channel%32);//�رն�Ӧ�ж�
} 