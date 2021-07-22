#include "adc.h"
#include "delay.h"
#include "stdio.h"

void Baterry_Adc_Init(void)
{    
  //�ȳ�ʼ��IO��
 	RCC->APB2ENR|=1<<3;    //ʹ��PORTB��ʱ�� 
	GPIOB->CRL&=0XFFFFFF0F;//PB1 anolog���� 
	RCC->APB2ENR|=1<<9;    //ADC1ʱ��ʹ��	  
	RCC->APB2RSTR|=1<<9;   //ADC1��λ
	RCC->APB2RSTR&=~(1<<9);//��λ����	    

	ADC1->CR1&=0XF0FFFF;   //����ģʽ����
	ADC1->CR1|=0<<16;      //��������ģʽ  
	ADC1->CR1&=~(1<<8);    //��ɨ��ģʽ	  
	ADC1->CR2&=~(1<<1);    //����ת��ģʽ
	ADC1->CR2&=~(7<<17);	   
	ADC1->CR2|=7<<17;	   //�������ת��  
	ADC1->CR2|=1<<20;      //ʹ�����ⲿ����(SWSTART)!!!	����ʹ��һ���¼�������
	ADC1->CR2&=~(1<<11);   //�Ҷ���	 
	ADC1->SQR1&=~(0XF<<20);
	ADC1->SQR1&=0<<20;     //1��ת���ڹ��������� Ҳ����ֻת����������1 			   
	//����ͨ��9�Ĳ���ʱ��
	ADC1->SMPR2&=0X0FFFFFFF; //����ʱ�����	  
	ADC1->SMPR2|=7<<27;      // 28.5����,��߲���ʱ�������߾�ȷ��	 
	//�������£�һ��ת��0.0315ms,
	ADC1->CR2|=1<<0;	    //����ADת����	 
	ADC1->CR2|=1<<3;        //ʹ�ܸ�λУ׼  
	while(ADC1->CR2&1<<3);  //�ȴ�У׼���� 			 
    //��λ��������ò���Ӳ���������У׼�Ĵ�������ʼ�����λ��������� 		 
	ADC1->CR2|=1<<2;        //����ADУ׼	   
	while(ADC1->CR2&1<<2);  //�ȴ�У׼���� 
	ADC1->SQR3&=0XFFFFFFE0;//��������1 ͨ��ch	
  	ADC1->SQR3|=9;
}		

u16 Get_Adc(u8 ch)//���ADC����   
{
	//����ת������	  		 			    
	ADC1->CR2|=1<<22;       //��������ת��ͨ�� 
	while(!(ADC1->SR&1<<1));//�ȴ�ת������	 	   
	return (ADC1->DR);		//����adcֵ	
}

float Get_battery_volt(void)//��ص�ѹ����  
{  
	float Volt; //��ص�ѹ
	Volt=(float)Get_Adc(Battery_Ch)*3.3*11/4096;//�����ѹ���������ԭ��ͼ�򵥷������Եõ�
	return Volt;
}


//��������adc���ɼ�����Ҫ���������ݺ��ֶ��ر�ת��
void my_adc1_init(void);//adc��ʼ������
void adc1_dma_init(void);//adc�ɼ��ṹ��ʼ��
void DMA_for_ADC1_init(void);//dma��ʼ��
void adc1_set_sequence(u8* channel,u8 number);//����������Ϣ
void adc1_set_sample_time(u8 channel,u8 tid);//���ò���ʱ����Ϣ
void my_adc1_close(void);//ADC����ת���ر�
void my_adc1_start(u16 anumber);//����adcת��,����Ϊƽ������,���е�ÿ��adc����ƽ������������ͬ��
//Ĭ�϶������ȫ��������
//��ָ�����ݽṹ��ƽ��������ʵ�ָ�С��Ŀ��ƽ��ֵ����

u16 adc1_buffer[1024];//adc���ݻ�����
#define adc1_max_number 1000 //����dma���ɼ�1000������
_my_adc my_adc1=
{
	.init=adc1_dma_init,
	.set_sequence=adc1_set_sequence,
	.set_sample_time=adc1_set_sample_time,
	.start=my_adc1_start,
	.close=my_adc1_close,
};
_adc_data data_ch16=//�ڲ��¶�ģ������ݴ�����Ϣ
{
	.channel=16,//ͨ������
    .fit_id=1,//�˲��㷨Ĭ����
    .aorigin=0,//�˲�����ƽ��֮��ԭʼ����
    .anumber=40,//ƽ������
    .linear=
	{
		.A=-0.1799160479,
		.C=337.639571237,
		.lvalue=0,
	},//������ϲ����ͽ��
};
_adc_data data_ch9=//�ڲ��¶�ģ������ݴ�����Ϣ
{
	.channel=9,//ͨ������
    .fit_id=1,//�˲��㷨Ĭ����
    .aorigin=0,//�˲�����ƽ��֮��ԭʼ����
    .anumber=40,//ƽ������
    .linear=
	{
		.A=0.0088623046875,
		.C=0,
		.lvalue=0,
	},//������ϲ����ͽ��
};

void adc1_dma_init(void)
{
	my_adc1_init();
	DMA_for_ADC1_init();
	my_adc1.stid[0]=(int)(ADC_CONVERSION_TIME+1.5);
	my_adc1.stid[1]=(int)(ADC_CONVERSION_TIME+7.5);
	my_adc1.stid[2]=(int)(ADC_CONVERSION_TIME+13.5);
	my_adc1.stid[3]=(int)(ADC_CONVERSION_TIME+28.5);
	my_adc1.stid[4]=(int)(ADC_CONVERSION_TIME+41.5);
	my_adc1.stid[5]=(int)(ADC_CONVERSION_TIME+55.5);
	my_adc1.stid[6]=(int)(ADC_CONVERSION_TIME+71.5);
	my_adc1.stid[7]=(int)(ADC_CONVERSION_TIME+239.5);
	my_adc1.channel[0]=9;//�����ͨ����
  my_adc1.data[0]=&data_ch9;
	my_adc1.channel[1]=16;//�ڲ��¶ȴ������ɼ�ͨ��
  my_adc1.data[1]=&data_ch16;
  my_adc1.channel_number=2;
}

//����ƽ���������Ҳ��ǧ�򼶱��ٸ����ݴ����������
//���ô�������жϺͰ봫���жϹ���˫�������ṹ
//DMA��ȡѭ��ģʽ��ֱ���ɼ�����һ�ֵ�����
void my_adc1_start(u16 anumber)
{
	u32 real_number=anumber*my_adc1.channel_number;
	if(DMA1_Channel1->CNDTR==0)//���δ�ɼ���ɣ�������������
	{
		if(real_number>adc1_max_number)
		{
			my_adc1.DMA_cycle=real_number/adc1_max_number;
			if(real_number%adc1_max_number!=0)
			{
				my_adc1.DMA_cycle++;//������Ҫ�ټ�һ��ѭ��
        my_adc1.DMA_left=real_number%adc1_max_number;
			}
			real_number=adc1_max_number;
			//�ɼ����ݳ�����1000����Ҫ�໺������Ϲ���
			DMA1_Channel1->CCR&=~(1<<0); 	//�ر�DMA���� 
			while(DMA1_Channel1->CCR&0X1);	//ȷ��DMA���Ա�����
			DMA1_Channel1->CCR|=1<<2;		//����봫���ж�
			DMA1_Channel1->CCR|=1<<5;		//ѭ��ģʽ
			DMA1_Channel1->CMAR=(u32)adc1_buffer; //DMA �洢��0��ַ
			DMA1_Channel1->CNDTR=real_number;//DMA �������������,Ϊ0�ݲ�����
			DMA1->IFCR|=15<<0;				//���Stream1�����жϱ�ʶ
			//warn: ��������ֶ�ָ��
			DMA1_Channel1->CCR|=1<<0;		//����DMA����
		}
		else//����ܲɼ���ĿС��1000����ֱ�Ӳɼ���ȫ
		{
			my_adc1.DMA_cycle=1;
      my_adc1.DMA_left=real_number;
			DMA1_Channel1->CCR&=~(1<<0); 	//�ر�DMA���� 
			while(DMA1_Channel1->CCR&0X1);	//ȷ��DMA���Ա�����
			DMA1_Channel1->CCR&=~(1<<2);		//����봫���ж�
			DMA1_Channel1->CCR&=~(1<<5);		//ѭ��ģʽ
			DMA1_Channel1->CMAR=(u32)adc1_buffer; //DMA �洢��0��ַ
			DMA1_Channel1->CNDTR=real_number;	 //DMA �������������,Ϊ0�ݲ�����
			DMA1->IFCR|=15<<0;				//���Stream1�����жϱ�ʶ
			//warn: ��������ֶ�ָ��
			DMA1_Channel1->CCR|=1<<0;		//����DMA����
		}
		ADC1->CR2|=(1<<1);   //��������ת��ģʽ
		ADC1->CR2|=1<<22;       //��������ת��ͨ��
		my_adc1.start_time=get_us();//�õ�us����ķ���ʱ��

	}
}

void adc1_data_deal(u8 bc)
{
	u16 i=0;
	static u32 sum[MY_ADC_MAX_CHANNEL_NUMBER]={0};
	static u8 j=0;
	u16 off_set=bc*adc1_max_number/2;
	if(my_adc1.DMA_cycle==1&&(bc==2||bc==3||bc==4))//��ʾ�Ѿ�����
	{
		if(bc==4)//����1000�ε����
		{
			while(i<my_adc1.DMA_left)
			{
				sum[j]+=adc1_buffer[i];
				i++;
				j=(j+1)%my_adc1.channel_number;
			}
			my_adc1.now_number+=my_adc1.DMA_left;//�ɼ���Ŀ�ۼ�
		}
		else if(bc==2)//ʣ����������С��500
		{
			off_set=0;
			while(i<my_adc1.DMA_left)
			{
				sum[j]+=adc1_buffer[i+off_set];
				i++;
				j=(j+1)%my_adc1.channel_number;
			}
			my_adc1.now_number+=my_adc1.DMA_left;//�ɼ���Ŀ�ۼ�
		}
		else if(bc==3)
		{
			off_set=adc1_max_number/2;
			while(i<my_adc1.DMA_left-off_set)
			{
				sum[j]+=adc1_buffer[i+off_set];
				i++;
				j=(j+1)%my_adc1.channel_number;
			}
			my_adc1.now_number+=(my_adc1.DMA_left-off_set);//�ɼ���Ŀ�ۼ�
		}
		for(i=0;i<my_adc1.channel_number;i++)
		{
			//����������ƽ���Լ�������ϵĵط�
			my_adc1.data[i]->anumber=my_adc1.now_number/my_adc1.channel_number;//���ۼƵ���Ŀ
			my_adc1.data[i]->aorigin=(double)sum[i]/my_adc1.data[i]->anumber;//����ƽ��ֵ
			my_adc1.data[i]->linear.lvalue=my_adc1.data[i]->linear.A*my_adc1.data[i]->aorigin+my_adc1.data[i]->linear.C;//��׼���������
			if(j!= 0)
			{
				//���ڼ�������
				printf("adc���ڼ�������");
			}
		}
		//�����ۼ�������
    my_adc1.now_number=0;
		for(i=0;i<MY_ADC_MAX_CHANNEL_NUMBER;i++)
		{
			sum[i]=0;
			j=0;
		}
	}
	else
	{
		while(i<500)
		{
			sum[j]+=adc1_buffer[i+off_set];
			i++;
			j=(j+1)%my_adc1.channel_number;
		}
		my_adc1.now_number+=500;//�ɼ���Ŀ��500
	}	
}

void adc1_data_back(void)
{
	u8 i=0;
	printf("�ɼ�����%d,�ܻ���ʱ��:%d us.\n",my_adc1.data[0]->anumber,my_adc1.spend_time);
	for(i=0;i<my_adc1.channel_number;i++)
	{
    printf("����ͨ��%d,ԭʼƽ������:%0.2f,�������:%0.6f*%0.2f+%0.6f=%0.6f.\n",my_adc1.data[i]->channel,my_adc1.data[i]->aorigin,my_adc1.data[i]->linear.A,my_adc1.data[i]->aorigin,my_adc1.data[i]->linear.C,my_adc1.data[i]->linear.lvalue);
	}
}
void  DMA1_Channel1_IRQHandler(void)//ADC�ɼ�DMA�ж�
{
	if(DMA1->ISR&(1<<1))//��������ж�
	{		
		if(my_adc1.DMA_cycle==1)
		{
			//�ɼ�����
			ADC1->CR2&=~(1<<1);//����CONT����ת��λ���ر�adc
			DMA1_Channel1->CCR&=~(1<<0); //�ر�DMA����
			if(DMA1_Channel1->CCR&(1<<5))//�ж��Ƿ���ѭ��ģʽ
				adc1_data_deal(3);
			else
				adc1_data_deal(4);//��ѭ��ģʽ������
			my_adc1.spend_time=get_us()-my_adc1.start_time;//�õ��ܻ���ʱ��
			
		}
		else
		{
			//�����ۻ�������������ƽ��ֵ��Ϣ
			adc1_data_deal(1);
		}
    my_adc1.DMA_cycle--;
	}
	else if(DMA1->ISR&(1<<2))//�봫���ж�
	{
		if(my_adc1.DMA_cycle==1&&my_adc1.DMA_left<=adc1_max_number/2)
		{
			//�ɼ�����
			ADC1->CR2&=~(1<<1);//����CONT����ת��λ���ر�adc
			DMA1_Channel1->CCR&=~(1<<0); //�ر�DMA����
			//���ô��������ؽ��
			adc1_data_deal(2);
			my_adc1.spend_time=get_us()-my_adc1.start_time;//�õ��ܻ���ʱ�� 
			//adc1_data_back();
		}
		else
		{
			//�����ۻ�������������ƽ��ֵ��Ϣ
			adc1_data_deal(1);
		}
	}
	DMA1->IFCR |=15<<0;//���ͨ��2�����жϱ�ʶ
}


 
void my_adc1_close(void)
{
	ADC1->CR2&=~(1<<1);//����CONT����ת��λ��
}

//������������Ķ��ͨ����adc
void my_adc1_init(void)//
{    
	//�ȳ�ʼ��IO��
  RCC->APB2ENR|=1<<3;    //ʹ��PORTB��ʱ�� 
	GPIOB->CRL&=0XFFFFFF0F;//PB1 anolog���� 
	//����ѡȡ�����IO��	 		 
	RCC->APB2ENR|=1<<9;    //ADC1ʱ��ʹ��	  
	RCC->APB2RSTR|=1<<9;   //ADC1��λ
	RCC->APB2RSTR&=~(1<<9);//��λ����	    
	//RCC->CFGR&=~(3<<14);   //��Ƶ��������	
	//SYSCLK/DIV2=12M ADCʱ������Ϊ12M,ADC���ʱ�Ӳ��ܳ���14M!
	//���򽫵���ADC׼ȷ���½�! 
	//RCC->CFGR|=2<<14;      	 

	ADC1->CR1=0;   //����ģʽ����
	//ADC1������Ĭ��ģʽ�£����жϲ�����������ADCģʽ  
	ADC1->CR2|=(1<<1);//����������ģʽ��ͨ��DMA�жϿ�����Ŀ
	ADC1->CR2|=(1<<8);//ʹ��DMAģʽ

	ADC1->CR2&=~(7<<17);	   
	ADC1->CR2|=7<<17;//�������ת��

	ADC1->CR2|=1<<20;  //ʹ�����ⲿ����(SWSTART)!!!	����ʹ��һ���¼�������
	ADC1->CR2&=~(1<<11);   //�Ҷ���
	ADC1->CR2|=(1<<23);//�����ڲ��¶ȴ�����ģ��	

	//ADC1->SQR1&=~(0XF<<20);
	//ADC1->SQR1&=0<<20;     //1��ת���ڹ��������� Ҳ����ֻת����������1 			   
	//����ͨ��8�Ĳ���ʱ��
	//ADC1->SMPR2&=0XF0FFFFFF;//ͨ������ʱ�����	  
	//ADC1->SMPR2|=7<<24;      //ͨ��  239.5����,��߲���ʱ�������߾�ȷ��	 

	ADC1->CR2|=1<<0;	    //����ADת����
	//������У׼	 
	ADC1->CR2|=1<<3;        //ʹ�ܸ�λУ׼  
	while(ADC1->CR2&1<<3);  //�ȴ�У׼���� 			 
    //��λ��������ò���Ӳ���������У׼�Ĵ�������ʼ�����λ��������� 		 
	ADC1->CR2|=1<<2;        //����ADУ׼	   
	while(ADC1->CR2&1<<2);  //�ȴ�У׼����
}	

void DMA_for_ADC1_init(void)
{ 
	RCC->AHBENR|=1<<0;//DMA1ʱ��ʹ�� 
	
	while(DMA1_Channel1->CCR&0X01);
	//�ȴ�DMA������ ����Enλ=0�����������ʱ��ſ���д�롣

	DMA1->IFCR |=15<<0;//���ͨ��1�����жϱ�ʶ

	//������ͨ��3����
	DMA1_Channel1->CPAR=(u32)&(ADC1->DR); //DMA�����ַ
	DMA1_Channel1->CMAR=0; 		//DMA �洢��0��ַ
	DMA1_Channel1->CNDTR=0;		//DMA �������������,Ϊ0�ݲ�����
	DMA1_Channel1->CCR=0;		//��ȫ����λCR�Ĵ���ֵ 
	                                                
	//��������жϺʹ����жϣ�ֱ��ģʽ�����ж��ڴ洢����ַ������������ã�

	//DMA1_Channel1->CCR|=1<<3;		//��������ж�ʹ��
	DMA1_Channel1->CCR|=1<<1;		//����������ж�
	DMA1_Channel1->CCR|=0<<2;		//������봫���ж�
	DMA1_Channel1->CCR|=0<<4;		//���ݴ��䷽�����赽�洢��ģʽ
	DMA1_Channel1->CCR|=0<<5;		//��ѭ��ģʽ
	DMA1_Channel1->CCR|=0<<6;		//���������ģʽ�������ַ�̶�
	DMA1_Channel1->CCR|=1<<7;		//�洢������ģʽ����ַ����ֵΪMsize=2�ֽ�
	DMA1_Channel1->CCR|=1<<8;		//�������ݳ���:16λ
	DMA1_Channel1->CCR|=1<<10;		//�洢�����ݳ���:16λ
	//UART��8λ��������
	DMA1_Channel1->CCR|=2<<12;		//һ�����ȼ�
	//ֱ��ģʽ�£�����ǿ��Ϊ0
  NVIC_enable(3,DMA1_Channel1_IRQn);//��4��3�����ȼ�
}

//���в���ģʽ�����б�����
void adc1_set_sequence(u8* channel,u8 number)
{
	u8 i=0;
	u8 real_num=0;//��Чͨ������
	if(number>MY_ADC_MAX_CHANNEL_NUMBER)
	{
		//���й���
		number=MY_ADC_MAX_CHANNEL_NUMBER;
	}
	for(i=0;i<number;i++)
	{
		if(channel[i]<=17)//���17��ͨ��
		{
			real_num++;
			if(i<6)//��һ���Ĵ�����
			{
				ADC1->SQR3&=~(31<<(5*i));//��������1 ͨ��ch	
  				ADC1->SQR3|=channel[i]<<(5*i);//д���Ӧ����
			}
			else if(i<12)
			{
				ADC1->SQR2&=~(31<<(5*(i-6)));//��������1 ͨ��ch	
  				ADC1->SQR2|=channel[i]<<(5*i);
			}
			else
			{
				ADC1->SQR1&=~(31<<(5*(i-12)));//��������1 ͨ��ch	
  				ADC1->SQR1|=channel[i]<<(5*i);
			}
		}
	}
	ADC1->SQR1&=~(15<<20);
	ADC1->SQR1|=(real_num-1)<<20;//0����1��ת����д����Чת����Ŀ
	//���ͻ�ִ��Ϣ
	printf("��Чת��ͨ����Ŀ��%d \n",real_num);
}

//����adcͨ����Ӧ�Ĳ�����ʱ��
//000��1.5���� 100��41.5����
//001��7.5���� 101��55.5����
//010��13.5���� 110��71.5����
//011��28.5���� 111��239.5����
//��ת��ʱ��12.5+sample��ADC clock=12Mhz��
void adc1_set_sample_time(u8 channel,u8 tid)
{
	u8 ch_offset=0;
	if(tid>7)//�Ƿ�����
		tid=7;//������ʱ��
	if(channel<10)//��SMPR2�Ĵ�������
	{
		ch_offset=channel*3;
		ADC1->SMPR2&=~(7<<(ch_offset));//ͨ������ʱ�����	  
		ADC1->SMPR2|=tid<<(ch_offset);//��ֵ
	}
	else
	{
		ch_offset=(channel-10)*3;//�ӵ�ʮ��ͨ����ʼ����
		ADC1->SMPR1&=~(7<<(ch_offset));//ͨ������ʱ�����	  
		ADC1->SMPR1|=tid<<(ch_offset);//��ֵ
	}
	//���������õ�ͨ����Ϣ
	printf("��%d��ͨ������ʱ������Ϊ%d���ڣ�����ʱ�䣺%0.2f us��\n",channel,my_adc1.stid[tid],(float)my_adc1.stid[tid]/12);	
}

//��ȡͨ��ch��ת��ֵ��ȡtimes��,Ȼ��ƽ�� 
//ch:ͨ�����
//times:��ȡ����
//����ֵ:ͨ��ch��times��ת�����ƽ��ֵ
int Get_Adc_Average(u8 ch,u8 times)
{
  static int last_val=0;
	int temp_val=0;
	u8 t;
	for(t=0;t<times;t++)
	{
		temp_val+=Get_Adc(ch);
		delay_us(200);
	}
	temp_val=temp_val/times;
  if((temp_val-last_val <-5)||(temp_val -last_val >5))
  {
    last_val=(last_val*4+temp_val)/5 ;
    return  temp_val;
  }
  else
    return last_val;
} 

	
void ADC_init(void)
{
		my_adc1.init();
    my_adc1.set_sequence(my_adc1.channel,my_adc1.channel_number);//ע��ͨ����Ŀ
    my_adc1.set_sample_time(my_adc1.data[0]->channel,ADC_SAMPLE_TIME_55p5);
    my_adc1.set_sample_time(my_adc1.data[1]->channel,ADC_SAMPLE_TIME_55p5);
}
	





