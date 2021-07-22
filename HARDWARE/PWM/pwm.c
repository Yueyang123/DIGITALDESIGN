#include "PWM.h"
#include "sys.h"
#include "stm32f10x_tim.h"
void TIM4_PWM_Init(u16 arr,u16 psc)
{   
        TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
        TIM_OCInitTypeDef TIM_OCInitStructure;
        GPIO_InitTypeDef    GPIO_InitStrucyure;

        RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);  //使能定时器4时钟
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE); // 使能GPIO
 
 
        GPIO_InitStrucyure.GPIO_Mode=GPIO_Mode_Out_PP;        //改为复用推挽输出GPIO_Mode_AF_PP
        GPIO_InitStrucyure.GPIO_Pin=GPIO_Pin_7;
        GPIO_InitStrucyure.GPIO_Speed=GPIO_Speed_50MHz;
        GPIO_Init(GPIOB,&GPIO_InitStrucyure);                 //初始化io口
	
	      GPIO_InitStrucyure.GPIO_Mode=GPIO_Mode_Out_PP;        //改为复用推挽输出GPIO_Mode_AF_PP
        GPIO_InitStrucyure.GPIO_Pin=GPIO_Pin_6;
        GPIO_InitStrucyure.GPIO_Speed=GPIO_Speed_50MHz;
        GPIO_Init(GPIOB,&GPIO_InitStrucyure);    
	
	
	

 
	TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //设置用来作为TIMx时钟频率除数的预分频值 
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; //选择定时器模式:TIM脉冲宽度调制模式2
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //输出极性:TIM输出比较极性高
	TIM_OC1Init(TIM4, &TIM_OCInitStructure);        //通道一对应右轮信号输入
 
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; //选择定时器模式:TIM脉冲宽度调制模式2
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //输出极性:TIM输出比较极性高
	TIM_OC2Init(TIM4, &TIM_OCInitStructure);          //通道2对应左轮信号输入



  TIM_OC1PreloadConfig(TIM4,TIM_OCPreload_Enable);
	TIM_OC2PreloadConfig(TIM4,TIM_OCPreload_Enable);


 
 
 			
	TIM_Cmd(TIM4,ENABLE);  //使能定时器
	
	
 
    }


		