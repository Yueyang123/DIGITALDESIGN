#include "usart.h"
#include "delay.h"
#include "SERVO.h"
#define SERVO_K 2491.2f
#define SERVO_INIT  2800
#define SERVO   TIM1->CCR4  //舵机引脚
//SERVO正常控制范围 2000-3600
//时钟频率位PCLK2的2倍，APB2预分频器有分频情况下
//计数器频率=PLCK2/((arr+1)*(psc+1))
void TIM1_Init(u16 arr,u16 psc)  
{  
	RCC->APB2ENR|=1<<11;//TIM3时钟使能 
	RCC->APB2ENR|=1<<2;        //PORTA时钟使能 
	GPIOA->CRH&=0XFFFF0FFF;    //PORTA11复用输出
	GPIOA->CRH|=0X0000B000;    //PORTA11复用输出   
 	TIM1->ARR=arr;      //设定计数器自动重装值   
	TIM1->PSC=psc;      //预分频器
	TIM1->CCMR2|=6<<12;        //CH4 PWM1模式	
	TIM1->CCMR2|=1<<11;        //CH4预装载使能	   
	TIM1->CCER|=1<<12;         //CH4输出使能	   
  TIM1->BDTR |= 1<<15;       //TIM1必须要这句话才能输出PWM
	TIM1->DIER|=1<<0;   //允许更新中断
	//这里没有开启预装载，ARR是实时更新的。
	//UG这里也可须发更新中断
	//计数在更新时不会停止,默认向上计数
	//默认开启更新
	//这里额外添加了PWM输出功能。
	TIM1->CR1 = 0x80;   //ARPE使能 					   
	TIM1->CR1|=1<<0;    //使能定时器
	TIM1->CCR4=2800;
	NVIC_enable(3,TIM1_UP_IRQn);
	//抢占优先级3
} 

void TIM1_UP_IRQHandler(void)//耦合计时中断以及信息处理
{ 	
  if(TIM1->SR&(1<<0))//这里不用判断SR指向的中断，因为这个是专门的更新中断  
	{	
		//舵机PWM驱动
    }    
	TIM1->SR&=~(1<<0);//清除中断标志位
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
