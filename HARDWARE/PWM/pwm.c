#include "PWM.h"
#include "sys.h"
#include "stm32f10x_tim.h"
void TIM4_PWM_Init(u16 arr,u16 psc)
{   
        TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
        TIM_OCInitTypeDef TIM_OCInitStructure;
        GPIO_InitTypeDef    GPIO_InitStrucyure;

        RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);  //ʹ�ܶ�ʱ��4ʱ��
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE); // ʹ��GPIO
 
 
        GPIO_InitStrucyure.GPIO_Mode=GPIO_Mode_Out_PP;        //��Ϊ�����������GPIO_Mode_AF_PP
        GPIO_InitStrucyure.GPIO_Pin=GPIO_Pin_7;
        GPIO_InitStrucyure.GPIO_Speed=GPIO_Speed_50MHz;
        GPIO_Init(GPIOB,&GPIO_InitStrucyure);                 //��ʼ��io��
	
	      GPIO_InitStrucyure.GPIO_Mode=GPIO_Mode_Out_PP;        //��Ϊ�����������GPIO_Mode_AF_PP
        GPIO_InitStrucyure.GPIO_Pin=GPIO_Pin_6;
        GPIO_InitStrucyure.GPIO_Speed=GPIO_Speed_50MHz;
        GPIO_Init(GPIOB,&GPIO_InitStrucyure);    
	
	
	

 
	TIM_TimeBaseStructure.TIM_Period = arr; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ 
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; //ѡ��ʱ��ģʽ:TIM�����ȵ���ģʽ2
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //�Ƚ����ʹ��
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //�������:TIM����Ƚϼ��Ը�
	TIM_OC1Init(TIM4, &TIM_OCInitStructure);        //ͨ��һ��Ӧ�����ź�����
 
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; //ѡ��ʱ��ģʽ:TIM�����ȵ���ģʽ2
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //�Ƚ����ʹ��
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //�������:TIM����Ƚϼ��Ը�
	TIM_OC2Init(TIM4, &TIM_OCInitStructure);          //ͨ��2��Ӧ�����ź�����



  TIM_OC1PreloadConfig(TIM4,TIM_OCPreload_Enable);
	TIM_OC2PreloadConfig(TIM4,TIM_OCPreload_Enable);


 
 
 			
	TIM_Cmd(TIM4,ENABLE);  //ʹ�ܶ�ʱ��
	
	
 
    }


		