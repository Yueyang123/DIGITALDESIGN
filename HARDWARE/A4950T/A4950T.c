#include "A4950T.h"



//��˥������������˥������ʱΪ����ģʽ
//��˥������������˥������ʱΪ�ƶ�ģʽ
/*
IN1       IN2       function
PWM       0          ǰ��������˥��
1         PWM        ǰ������˥��
0         PWM        ���ˣ���˥��
PWM       1          ���ˣ���˥��
*/
//�����·PWM
//CCR��ֵ��ARR��ֵ����Ԥ���壬����ζ��Ҫ�ȵ������¼�ʱ�Ż�ı䡣
//û�п����ж�
//void TIM4_PWM_Init(u16 arr,u16 psc)
//{		 					 
//  RCC->APB1ENR|=1<<2;       //TIM4ʱ��ʹ��    
//	RCC->APB2ENR|=1<<3;       //PORTBʱ��ʹ��   
//	GPIOB->CRL&=0X00FFFFFF;   //PORTB6 7  8 9�����������
//	GPIOB->CRL|=0XBB000000;   //PORTB6 7  8 9�����������
//	GPIOB->CRH&=0XFFFFFF00;   //PORTB6 7  8 9�����������
//	GPIOB->CRH|=0X000000BB;   //PORTB6 7  8 9�����������

//	TIM4->ARR=arr;//�趨�������Զ���װֵ 
//	TIM4->PSC=psc;//Ԥ��Ƶ������Ƶ
//    TIM4->CCMR1|=6<<4;//CH1 PWM1ģʽ	
//	TIM4->CCMR1|=6<<12; //CH2 PWM1ģʽ	
//	TIM4->CCMR2|=6<<4;//CH3 PWM1ģʽ	
//	TIM4->CCMR2|=6<<12; //CH4 PWM1ģʽ	
//	//PWMģʽ1�����ϼ���ʱ��CNT<CCRʱΪ��Ч��ƽ��CNT>CCRʱΪ��Ч��ƽ

//	TIM4->CCMR1|=1<<3; //CH1Ԥװ��ʹ��	  
//	TIM4->CCMR1|=1<<11;//CH2Ԥװ��ʹ��	 
//	TIM4->CCMR2|=1<<3; //CH3Ԥװ��ʹ��	  
//	TIM4->CCMR2|=1<<11;//CH4Ԥװ��ʹ��
//	//������Ԥװ��ģʽ���ڸ����¼�ʱ����ֵ��д�롣
//	//����Ĭ�ϸ�λ��CCͨ��Ϊ���ģʽ	 
//	TIM4->CCER|=1<<0;  //CH1���ʹ��	
//	TIM4->CCER|=1<<4;  //CH2���ʹ��	   
//	TIM4->CCER|=1<<8;  //CH3���ʹ��	
//	TIM4->CCER|=1<<12; //CH4���ʹ��	
//	TIM4->CR1=0x80;  //ARPEʹ�� 
//	TIM4->CR1|=0x01;   //ʹ�ܶ�ʱ��4 
//	//CR�Ĵ���Ĭ�����ϼ���������ģʽ���Ҹ����¼�����ʱ��ֹͣ����	
//} 

#include "PWM.h"
#include "sys.h"
#include "stm32f10x_tim.h"



void TIM4_PWM_Init(u16 arr,u16 psc)
{   
	  GPIO_InitTypeDef GPIO_Initure;
	  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	  TIM_OCInitTypeDef TIM4_CHxHandler;//��ʱ��4ͨ��x���
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE); //ʱ��ʹ��
	  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE); 
 
    GPIO_Initure.GPIO_Pin=GPIO_Pin_6|GPIO_Pin_7;//����IO
    GPIO_Initure.GPIO_Mode=GPIO_Mode_AF_PP;
    GPIO_Initure.GPIO_Speed=GPIO_Speed_50MHz; 
    GPIO_Init(GPIOB,&GPIO_Initure);
		GPIO_Initure.GPIO_Pin=GPIO_Pin_8|GPIO_Pin_9;
		GPIO_Init(GPIOB,&GPIO_Initure);
 
 	TIM_TimeBaseStructure.TIM_Period = arr;//���ö�ʱ��
	TIM_TimeBaseStructure.TIM_Prescaler =psc; 
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //���ϼ���ģʽ
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); 
	
	TIM4_CHxHandler.TIM_OCMode = TIM_OCMode_PWM1; //ѡ��ʱ��ģʽ:TIM�����ȵ���ģʽ2
 	TIM4_CHxHandler.TIM_OutputState = TIM_OutputState_Enable; //�Ƚ����ʹ��
	TIM4_CHxHandler.TIM_OCPolarity = TIM_OCPolarity_High; //����Ƚϼ��Ը�
																		 
	TIM_OC1Init(TIM4, &TIM4_CHxHandler); 		//����pwmͨ��
	TIM_OC2Init(TIM4, &TIM4_CHxHandler);	
	TIM_OC3Init(TIM4, &TIM4_CHxHandler);	
	TIM_OC4Init(TIM4, &TIM4_CHxHandler);
	
	
	TIM_OC1PreloadConfig(TIM4,TIM_OCPreload_Enable);//Ԥװ��ʹ��
	TIM_OC2PreloadConfig(TIM4,TIM_OCPreload_Enable);
	TIM_OC3PreloadConfig(TIM4,TIM_OCPreload_Enable);
	TIM_OC4PreloadConfig(TIM4,TIM_OCPreload_Enable);
	
	TIM_Cmd(TIM4,ENABLE);  //ʹ�ܶ�ʱ��
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

//��˥������ת��ռ�ձ�Խ���ٶ�ԽС
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
//��˥������ת��ռ�ձ�Խ���ٶ�ԽС
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
//��˥���ƶ�
void slow_stop(void)
{
    PWMA1=MAX_PWM;
    PWMA2=MAX_PWM;
    PWMB1=MAX_PWM;
    PWMB2=MAX_PWM;
}
//��˥������ת��ռ�ձ�Խ���ٶ�Խ��
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
