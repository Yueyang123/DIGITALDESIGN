#include "Encoder.h"
void TIM2_IRQHandler(void)
{ 		    		  			    
	if(TIM2->SR&0X0001)//����ж�
	{
		TIM2->CNT=10000; //��ʼֵ	    				   				     	    	
	}				   
	TIM2->SR&=~(1<<0);//����жϱ�־λ 	    
}

void TIM3_IRQHandler(void)
{ 		    		  			    
	if(TIM3->SR&0X0001)//����ж�
	{    
		TIM3->CNT=10000; //��ʼֵ				   				     	    	
	}				   
	TIM3->SR&=~(1<<0);//����жϱ�־λ 	    
}
//������ģʽ��ʼ����Ĭ���ı�Ƶ����
void Encoder_Init_TIM2(void)//С���װ�����
{
	RCC->APB1ENR|=1<<0;     //TIM2ʱ��ʹ��
	RCC->APB2ENR|=1<<2;    //ʹ��PORTAʱ��
	GPIOA->CRL&=0XFFFFFF00;//PA0 PA1
	GPIOA->CRL|=0X00000044;//��������


	TIM2->DIER|=1<<0;   //��������ж�				
	TIM2->PSC = 0x0;//Ԥ��Ƶ��
	TIM2->ARR = ENCODER_TIM_PERIOD;//�趨�������Զ���װֵ 
	TIM2->CR1 &=~(3<<8);// ѡ��ʱ�ӷ�Ƶ������Ƶ
	TIM2->CR1 &=~(3<<5);// ѡ�����ģʽ:���ض���ģʽ
	//���ض���ģʽ�ɷ���λ�Զ�ȷ�������ڱ�����ģʽ��
	//������Ӳ���Զ����㣬��λֻ��
	TIM2->CCMR1 |= 1<<0; //CC1S='01' IC1FP1ӳ�䵽TI1
	TIM2->CCMR1 |= 1<<8; //CC2S='01' IC2FP2ӳ�䵽TI2

	TIM2->CCER &= ~(1<<1);	 //CC1P='0'	 IC1FP1�����࣬IC1FP1=TI1
	TIM2->CCER &= ~(1<<5);	 //CC2P='0'	 IC2FP2�����࣬IC2FP2=TI2
    //���ﲻ���˲���ֱ��ʵ�ʵ�ֵ����ȡ
	TIM2->SMCR |= 3<<0;	 //SMS='011' ���е�������������غ��½�����Ч
	TIM2->CNT = 10000;
	TIM2->CR1 |= 0x01;    //CEN=1��ʹ�ܶ�ʱ��
	NVIC_enable(10,TIM2_IRQn);
}

void Encoder_Init_TIM3(void)//С���װ��ҵ��
{
	RCC->APB1ENR|=1<<1;     //TIM3ʱ��ʹ��
	RCC->APB2ENR|=1<<2;     //ʹ��PORTAʱ��
	GPIOA->CRL&=0X00FFFFFF; //PA6 PA7
	GPIOA->CRL|=0X44000000; //��������


	TIM3->DIER|=1<<0;   //��������ж�				
	TIM3->PSC = 0x0;//Ԥ��Ƶ��
	TIM3->ARR = ENCODER_TIM_PERIOD;//�趨�������Զ���װֵ
	TIM3->CNT=10000; //��ʼֵ
	TIM3->CR1 &=~(3<<8);// ѡ��ʱ�ӷ�Ƶ������Ƶ
	TIM3->CR1 &=~(3<<5);// ѡ�����ģʽ:���ض���ģʽ
	//���ض���ģʽ�ɷ���λ�Զ�ȷ�������ڱ�����ģʽ��
	//������Ӳ���Զ����㣬��λֻ��

	TIM3->CCMR1 |= 1<<0; //CC1S='01' IC1FP1ӳ�䵽TI1
	TIM3->CCMR1 |= 1<<8; //CC2S='01' IC2FP2ӳ�䵽TI2
	TIM3->CCER &= ~(1<<1);	 //CC1P='0'	 IC1FP1�����࣬IC1FP1=TI1
	TIM3->CCER &= ~(1<<5);	 //CC2P='0'	 IC2FP2�����࣬IC2FP2=TI2
	//���ﲻ���˲���ֱ��ʵ�ʵ�ֵ����ȡ
	TIM3->SMCR |= 3<<0;	 //SMS='011' ���е�������������غ��½�����Ч
	TIM3->CR1 |= 0x01;    //CEN=1��ʹ�ܶ�ʱ��
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