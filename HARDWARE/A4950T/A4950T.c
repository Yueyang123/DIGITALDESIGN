#include "A4950T.h"



//快衰减：电流快速衰减，此时为滑动模式
//慢衰减：电流慢速衰减，此时为制动模式
/*
IN1       IN2       function
PWM       0          前进，快速衰减
1         PWM        前进，慢衰减
0         PWM        后退，快衰减
PWM       1          后退，慢衰减
*/
//输出四路PWM
//CCR的值和ARR的值都有预缓冲，这意味需要等到更新事件时才会改变。
//没有开启中断
//void TIM4_PWM_Init(u16 arr,u16 psc)
//{		 					 
//  RCC->APB1ENR|=1<<2;       //TIM4时钟使能    
//	RCC->APB2ENR|=1<<3;       //PORTB时钟使能   
//	GPIOB->CRL&=0X00FFFFFF;   //PORTB6 7  8 9复用推挽输出
//	GPIOB->CRL|=0XBB000000;   //PORTB6 7  8 9复用推挽输出
//	GPIOB->CRH&=0XFFFFFF00;   //PORTB6 7  8 9复用推挽输出
//	GPIOB->CRH|=0X000000BB;   //PORTB6 7  8 9复用推挽输出

//	TIM4->ARR=arr;//设定计数器自动重装值 
//	TIM4->PSC=psc;//预分频器不分频
//    TIM4->CCMR1|=6<<4;//CH1 PWM1模式	
//	TIM4->CCMR1|=6<<12; //CH2 PWM1模式	
//	TIM4->CCMR2|=6<<4;//CH3 PWM1模式	
//	TIM4->CCMR2|=6<<12; //CH4 PWM1模式	
//	//PWM模式1，向上计数时，CNT<CCR时为有效电平，CNT>CCR时为无效电平

//	TIM4->CCMR1|=1<<3; //CH1预装载使能	  
//	TIM4->CCMR1|=1<<11;//CH2预装载使能	 
//	TIM4->CCMR2|=1<<3; //CH3预装载使能	  
//	TIM4->CCMR2|=1<<11;//CH4预装载使能
//	//开启了预装载模式，在更新事件时，新值被写入。
//	//这里默认复位后CC通道为输出模式	 
//	TIM4->CCER|=1<<0;  //CH1输出使能	
//	TIM4->CCER|=1<<4;  //CH2输出使能	   
//	TIM4->CCER|=1<<8;  //CH3输出使能	
//	TIM4->CCER|=1<<12; //CH4输出使能	
//	TIM4->CR1=0x80;  //ARPE使能 
//	TIM4->CR1|=0x01;   //使能定时器4 
//	//CR寄存器默认向上计数，边沿模式，且更新事件发生时不停止计数	
//} 

#include "PWM.h"
#include "sys.h"
#include "stm32f10x_tim.h"



void TIM4_PWM_Init(u16 arr,u16 psc)
{   
	  GPIO_InitTypeDef GPIO_Initure;
	  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	  TIM_OCInitTypeDef TIM4_CHxHandler;//定时器4通道x句柄
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE); //时钟使能
	  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE); 
 
    GPIO_Initure.GPIO_Pin=GPIO_Pin_6|GPIO_Pin_7;//配置IO
    GPIO_Initure.GPIO_Mode=GPIO_Mode_AF_PP;
    GPIO_Initure.GPIO_Speed=GPIO_Speed_50MHz; 
    GPIO_Init(GPIOB,&GPIO_Initure);
		GPIO_Initure.GPIO_Pin=GPIO_Pin_8|GPIO_Pin_9;
		GPIO_Init(GPIOB,&GPIO_Initure);
 
 	TIM_TimeBaseStructure.TIM_Period = arr;//配置定时器
	TIM_TimeBaseStructure.TIM_Prescaler =psc; 
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //向上计数模式
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); 
	
	TIM4_CHxHandler.TIM_OCMode = TIM_OCMode_PWM1; //选择定时器模式:TIM脉冲宽度调制模式2
 	TIM4_CHxHandler.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
	TIM4_CHxHandler.TIM_OCPolarity = TIM_OCPolarity_High; //输出比较极性高
																		 
	TIM_OC1Init(TIM4, &TIM4_CHxHandler); 		//配置pwm通道
	TIM_OC2Init(TIM4, &TIM4_CHxHandler);	
	TIM_OC3Init(TIM4, &TIM4_CHxHandler);	
	TIM_OC4Init(TIM4, &TIM4_CHxHandler);
	
	
	TIM_OC1PreloadConfig(TIM4,TIM_OCPreload_Enable);//预装载使能
	TIM_OC2PreloadConfig(TIM4,TIM_OCPreload_Enable);
	TIM_OC3PreloadConfig(TIM4,TIM_OCPreload_Enable);
	TIM_OC4PreloadConfig(TIM4,TIM_OCPreload_Enable);
	
	TIM_Cmd(TIM4,ENABLE);  //使能定时器
}		

void A4950T_Init()
{
	TIM4_PWM_Init(6399,0);
}

void set_pwmb1(u16 pwm)
{
    if(pwm>MAX_PWM)
        pwm=MAX_PWM;
    PWMB1=pwm;
}

void set_pwmb2(u16 pwm)
{
    if(pwm>MAX_PWM)
        pwm=MAX_PWM;
    PWMB2=pwm;
}
void set_pwma2(u16 pwm)
{
    if(pwm>MAX_PWM)
        pwm=MAX_PWM;
    PWMA2=pwm;
}
void set_pwma1(u16 pwm)
{
    if(pwm>MAX_PWM)
        pwm=MAX_PWM;
    PWMA1=pwm;
}

//慢衰减，正转，占空比越大，速度越小
void slow_forward_a(u16 ain2)
{
    PWMA1=MAX_PWM;
    PWMA2=MAX_PWM-ain2;
}
void slow_forward_b(u16 bin2)
{
    PWMB1=MAX_PWM;
    PWMB2=MAX_PWM-bin2;
}
//慢衰减，反转，占空比越大，速度越小
void slow_back_a(u16 ain1)
{
    PWMA1=MAX_PWM-ain1;
    PWMA2=MAX_PWM;
}
void slow_back_b(u16 bin1)
{
    PWMB1=MAX_PWM-bin1;
    PWMB2=MAX_PWM;
}
//慢衰减制动
void slow_stop(void)
{
    PWMA1=MAX_PWM;
    PWMA2=MAX_PWM;
    PWMB1=MAX_PWM;
    PWMB2=MAX_PWM;
}
//快衰减，正转，占空比越大，速度越大
void fast_forward_a(u16 ain)
{
    PWMA1=ain;
    PWMA2=0;
}
void fast_forward_b(u16 bin)
{
    PWMB1=bin;
    PWMB2=0;
}

void fast_back_a(u16 ain)
{

    PWMA1=0;
    PWMA2=ain;
}
void fast_back_b(u16 bin)
{
    PWMB1=0;
    PWMB2=bin;
}
void fast_stop(void)
{
    PWMA1=0;
    PWMA2=0;
    PWMB1=0;
    PWMB2=0;
}
