/*
 * @Descripttion: 
 * @version: 
 * @Author: Yueyang
 * @email: 1700695611@qq.com
 * @Date: 2021-07-22 13:59:58
 * @LastEditors: Yueyang
 * @LastEditTime: 2021-07-23 12:56:49
 */
#include "usart.h"
#include "delay.h"
#include "SERVO.h"


void TIM1_Init(u16 arr,u16 psc)  
{  
	RCC->APB2ENR|=1<<11;
	RCC->APB2ENR|=1<<2;        
	GPIOA->CRH&=0XFFFF0FFF;    
	GPIOA->CRH|=0X0000B000;   
 	TIM1->ARR=arr;     
	TIM1->PSC=psc;     
	TIM1->CCMR2|=6<<12;      
	TIM1->CCMR2|=1<<11;      
	TIM1->CCER|=1<<12;        
  TIM1->BDTR |= 1<<15;      
	TIM1->DIER|=1<<0;  

	TIM1->CR1 = 0x80;   				   
	TIM1->CR1|=1<<0;   
	TIM1->CCR4=2800;
	NVIC_enable(3,TIM1_UP_IRQn);
} 

void TIM1_UP_IRQHandler(void)
{ 	
  if(TIM1->SR&(1<<0)) 
	{	
    }    
	TIM1->SR&=~(1<<0);
  TIM1->EGR&=~(1<<0);  
} 


void SERVO_Init()
{
	TIM1_Init(39999,35);//72000000/(400000*4);

}
void Set_Servo_Pwm(int servo)
{
     SERVO=servo;	
}
void SERVO_Set_Angle(float angle)
{
	float Angle;
	float servo;
	Angle=(angle-90)*3.14159/180;
	servo=(float)(SERVO_INIT+Angle*SERVO_K);  
	Set_Servo_Pwm(servo);
}

void SERVO_Set_ZERO()
{
	SERVO=SERVO_INIT;  
}
