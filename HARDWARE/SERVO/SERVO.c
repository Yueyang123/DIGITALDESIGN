#include "usart.h"
#include "delay.h"
#include "SERVO.h"
#define SERVO_K 2491.2f
#define SERVO_INIT  2800
#define SERVO   TIM1->CCR4  //�������
//SERVO�������Ʒ�Χ 2000-3600
//ʱ��Ƶ��λPCLK2��2����APB2Ԥ��Ƶ���з�Ƶ�����
//������Ƶ��=PLCK2/((arr+1)*(psc+1))
void TIM1_Init(u16 arr,u16 psc)  
{  
	RCC->APB2ENR|=1<<11;//TIM3ʱ��ʹ�� 
	RCC->APB2ENR|=1<<2;        //PORTAʱ��ʹ�� 
	GPIOA->CRH&=0XFFFF0FFF;    //PORTA11�������
	GPIOA->CRH|=0X0000B000;    //PORTA11�������   
 	TIM1->ARR=arr;      //�趨�������Զ���װֵ   
	TIM1->PSC=psc;      //Ԥ��Ƶ��
	TIM1->CCMR2|=6<<12;        //CH4 PWM1ģʽ	
	TIM1->CCMR2|=1<<11;        //CH4Ԥװ��ʹ��	   
	TIM1->CCER|=1<<12;         //CH4���ʹ��	   
  TIM1->BDTR |= 1<<15;       //TIM1����Ҫ��仰�������PWM
	TIM1->DIER|=1<<0;   //��������ж�
	//����û�п���Ԥװ�أ�ARR��ʵʱ���µġ�
	//UG����Ҳ���뷢�����ж�
	//�����ڸ���ʱ����ֹͣ,Ĭ�����ϼ���
	//Ĭ�Ͽ�������
	//������������PWM������ܡ�
	TIM1->CR1 = 0x80;   //ARPEʹ�� 					   
	TIM1->CR1|=1<<0;    //ʹ�ܶ�ʱ��
	TIM1->CCR4=2800;
	NVIC_enable(3,TIM1_UP_IRQn);
	//��ռ���ȼ�3
} 

void TIM1_UP_IRQHandler(void)//��ϼ�ʱ�ж��Լ���Ϣ����
{ 	
  if(TIM1->SR&(1<<0))//���ﲻ���ж�SRָ����жϣ���Ϊ�����ר�ŵĸ����ж�  
	{	
		//���PWM����
    }    
	TIM1->SR&=~(1<<0);//����жϱ�־λ
  TIM1->EGR&=~(1<<0);  
} 


void SERVO_Init()
{
	TIM1_Init(39999,35);//72000000/(400000*4);

}
void Set_Pwm(int servo)
{
     SERVO=servo;	
}
void SERVO_Set_Angle(float angle)
{
	float Angle;
	float servo;
	Angle=(angle-90)*3.14159/180;
	servo=(float)(SERVO_INIT+Angle*SERVO_K);  
	Set_Pwm(servo);
}

void SERVO_Set_ZERO()
{
	SERVO=SERVO_INIT;  
}
