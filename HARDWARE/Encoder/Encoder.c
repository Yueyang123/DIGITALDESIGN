#include "Encoder.h"
void TIM2_IRQHandler(void)
{ 		    		  			    
	if(TIM2->SR&0X0001)//溢出中断
	{
		TIM2->CNT=10000; //初始值	    				   				     	    	
	}				   
	TIM2->SR&=~(1<<0);//清除中断标志位 	    
}

void TIM3_IRQHandler(void)
{ 		    		  			    
	if(TIM3->SR&0X0001)//溢出中断
	{    
		TIM3->CNT=10000; //初始值				   				     	    	
	}				   
	TIM3->SR&=~(1<<0);//清除中断标志位 	    
}
//编码器模式初始化，默认四倍频计数
void Encoder_Init_TIM2(void)//小车底板左电机
{
	RCC->APB1ENR|=1<<0;     //TIM2时钟使能
	RCC->APB2ENR|=1<<2;    //使能PORTA时钟
	GPIOA->CRL&=0XFFFFFF00;//PA0 PA1
	GPIOA->CRL|=0X00000044;//浮空输入


	TIM2->DIER|=1<<0;   //允许更新中断				
	TIM2->PSC = 0x0;//预分频器
	TIM2->ARR = ENCODER_TIM_PERIOD;//设定计数器自动重装值 
	TIM2->CR1 &=~(3<<8);// 选择时钟分频：不分频
	TIM2->CR1 &=~(3<<5);// 选择计数模式:边沿对齐模式
	//边沿对齐模式由方向位自动确定方向，在编码器模式下
	//方向由硬件自动计算，该位只读
	TIM2->CCMR1 |= 1<<0; //CC1S='01' IC1FP1映射到TI1
	TIM2->CCMR1 |= 1<<8; //CC2S='01' IC2FP2映射到TI2

	TIM2->CCER &= ~(1<<1);	 //CC1P='0'	 IC1FP1不反相，IC1FP1=TI1
	TIM2->CCER &= ~(1<<5);	 //CC2P='0'	 IC2FP2不反相，IC2FP2=TI2
    //这里不用滤波，直接实际的值来读取
	TIM2->SMCR |= 3<<0;	 //SMS='011' 所有的输入均在上升沿和下降沿有效
	TIM2->CNT = 10000;
	TIM2->CR1 |= 0x01;    //CEN=1，使能定时器
	NVIC_enable(10,TIM2_IRQn);
}

void Encoder_Init_TIM3(void)//小车底板右电机
{
	RCC->APB1ENR|=1<<1;     //TIM3时钟使能
	RCC->APB2ENR|=1<<2;     //使能PORTA时钟
	GPIOA->CRL&=0X00FFFFFF; //PA6 PA7
	GPIOA->CRL|=0X44000000; //浮空输入


	TIM3->DIER|=1<<0;   //允许更新中断				
	TIM3->PSC = 0x0;//预分频器
	TIM3->ARR = ENCODER_TIM_PERIOD;//设定计数器自动重装值
	TIM3->CNT=10000; //初始值
	TIM3->CR1 &=~(3<<8);// 选择时钟分频：不分频
	TIM3->CR1 &=~(3<<5);// 选择计数模式:边沿对齐模式
	//边沿对齐模式由方向位自动确定方向，在编码器模式下
	//方向由硬件自动计算，该位只读

	TIM3->CCMR1 |= 1<<0; //CC1S='01' IC1FP1映射到TI1
	TIM3->CCMR1 |= 1<<8; //CC2S='01' IC2FP2映射到TI2
	TIM3->CCER &= ~(1<<1);	 //CC1P='0'	 IC1FP1不反相，IC1FP1=TI1
	TIM3->CCER &= ~(1<<5);	 //CC2P='0'	 IC2FP2不反相，IC2FP2=TI2
	//这里不用滤波，直接实际的值来读取
	TIM3->SMCR |= 3<<0;	 //SMS='011' 所有的输入均在上升沿和下降沿有效
	TIM3->CR1 |= 0x01;    //CEN=1，使能定时器
	NVIC_enable(10,TIM3_IRQn);
}

void Encoder_Init()
{
	 Encoder_Init_TIM2();
	 Encoder_Init_TIM3();
}

int Read_Encoder(u8 TIMX)
{
    int Encoder_TIM;    
    switch(TIMX)
	 {
	  case 2:  	Encoder_TIM= (short)TIM2 -> CNT; TIM2->CNT=10000;break;
		case 3:  	Encoder_TIM= (short)TIM3 -> CNT;  TIM3->CNT=10000;break;		
		default:  Encoder_TIM=0;
	 }
		return Encoder_TIM;
}