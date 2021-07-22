#include "adc.h"
#include "delay.h"
#include "stdio.h"

void Baterry_Adc_Init(void)
{    
  //先初始化IO口
 	RCC->APB2ENR|=1<<3;    //使能PORTB口时钟 
	GPIOB->CRL&=0XFFFFFF0F;//PB1 anolog输入 
	RCC->APB2ENR|=1<<9;    //ADC1时钟使能	  
	RCC->APB2RSTR|=1<<9;   //ADC1复位
	RCC->APB2RSTR&=~(1<<9);//复位结束	    

	ADC1->CR1&=0XF0FFFF;   //工作模式清零
	ADC1->CR1|=0<<16;      //独立工作模式  
	ADC1->CR1&=~(1<<8);    //非扫描模式	  
	ADC1->CR2&=~(1<<1);    //单次转换模式
	ADC1->CR2&=~(7<<17);	   
	ADC1->CR2|=7<<17;	   //软件控制转换  
	ADC1->CR2|=1<<20;      //使用用外部触发(SWSTART)!!!	必须使用一个事件来触发
	ADC1->CR2&=~(1<<11);   //右对齐	 
	ADC1->SQR1&=~(0XF<<20);
	ADC1->SQR1&=0<<20;     //1个转换在规则序列中 也就是只转换规则序列1 			   
	//设置通道9的采样时间
	ADC1->SMPR2&=0X0FFFFFFF; //采样时间清空	  
	ADC1->SMPR2|=7<<27;      // 28.5周期,提高采样时间可以提高精确度	 
	//该周期下，一次转换0.0315ms,
	ADC1->CR2|=1<<0;	    //开启AD转换器	 
	ADC1->CR2|=1<<3;        //使能复位校准  
	while(ADC1->CR2&1<<3);  //等待校准结束 			 
    //该位由软件设置并由硬件清除。在校准寄存器被初始化后该位将被清除。 		 
	ADC1->CR2|=1<<2;        //开启AD校准	   
	while(ADC1->CR2&1<<2);  //等待校准结束 
	ADC1->SQR3&=0XFFFFFFE0;//规则序列1 通道ch	
  	ADC1->SQR3|=9;
}		

u16 Get_Adc(u8 ch)//电池ADC测量   
{
	//设置转换序列	  		 			    
	ADC1->CR2|=1<<22;       //启动规则转换通道 
	while(!(ADC1->SR&1<<1));//等待转换结束	 	   
	return (ADC1->DR);		//返回adc值	
}

float Get_battery_volt(void)//电池电压测量  
{  
	float Volt; //电池电压
	Volt=(float)Get_Adc(Battery_Ch)*3.3*11/4096;//电阻分压，具体根据原理图简单分析可以得到
	return Volt;
}


//连续采样adc，采集到需要数量的数据后，手动关闭转换
void my_adc1_init(void);//adc初始化函数
void adc1_dma_init(void);//adc采集结构初始化
void DMA_for_ADC1_init(void);//dma初始化
void adc1_set_sequence(u8* channel,u8 number);//设置序列信息
void adc1_set_sample_time(u8 channel,u8 tid);//设置采样时间信息
void my_adc1_close(void);//ADC连续转换关闭
void my_adc1_start(u16 anumber);//启动adc转换,参数为平均次数,序列的每个adc采样平均次数都是相同的
//默认多个采样全部由序列
//可指定数据结构的平均次数来实现更小数目的平均值计算

u16 adc1_buffer[1024];//adc数据缓冲区
#define adc1_max_number 1000 //单次dma最大采集1000次数据
_my_adc my_adc1=
{
	.init=adc1_dma_init,
	.set_sequence=adc1_set_sequence,
	.set_sample_time=adc1_set_sample_time,
	.start=my_adc1_start,
	.close=my_adc1_close,
};
_adc_data data_ch16=//内部温度模块的数据处理信息
{
	.channel=16,//通道代号
    .fit_id=1,//滤波算法默认无
    .aorigin=0,//滤波并且平均之后原始数据
    .anumber=40,//平均次数
    .linear=
	{
		.A=-0.1799160479,
		.C=337.639571237,
		.lvalue=0,
	},//线性拟合参数和结果
};
_adc_data data_ch9=//内部温度模块的数据处理信息
{
	.channel=9,//通道代号
    .fit_id=1,//滤波算法默认无
    .aorigin=0,//滤波并且平均之后原始数据
    .anumber=40,//平均次数
    .linear=
	{
		.A=0.0088623046875,
		.C=0,
		.lvalue=0,
	},//线性拟合参数和结果
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
	my_adc1.channel[0]=9;//电池是通道九
  my_adc1.data[0]=&data_ch9;
	my_adc1.channel[1]=16;//内部温度传感器采集通道
  my_adc1.data[1]=&data_ch16;
  my_adc1.channel_number=2;
}

//采样平均次数最高也就千万级别，再高数据存在溢出可能
//采用传输完成中断和半传输中断构成双缓冲区结构
//DMA采取循环模式，直到采集完了一轮的数据
void my_adc1_start(u16 anumber)
{
	u32 real_number=anumber*my_adc1.channel_number;
	if(DMA1_Channel1->CNDTR==0)//如果未采集完成，跳过本次启动
	{
		if(real_number>adc1_max_number)
		{
			my_adc1.DMA_cycle=real_number/adc1_max_number;
			if(real_number%adc1_max_number!=0)
			{
				my_adc1.DMA_cycle++;//额外需要再加一次循环
        my_adc1.DMA_left=real_number%adc1_max_number;
			}
			real_number=adc1_max_number;
			//采集数据超过了1000，需要多缓冲区配合工作
			DMA1_Channel1->CCR&=~(1<<0); 	//关闭DMA传输 
			while(DMA1_Channel1->CCR&0X1);	//确保DMA可以被设置
			DMA1_Channel1->CCR|=1<<2;		//允许半传输中断
			DMA1_Channel1->CCR|=1<<5;		//循环模式
			DMA1_Channel1->CMAR=(u32)adc1_buffer; //DMA 存储器0地址
			DMA1_Channel1->CNDTR=real_number;//DMA 传输的数据项数,为0暂不传输
			DMA1->IFCR|=15<<0;				//清空Stream1所有中断标识
			//warn: 这里必须手动指定
			DMA1_Channel1->CCR|=1<<0;		//开启DMA传输
		}
		else//如果总采集数目小于1000，则直接采集完全
		{
			my_adc1.DMA_cycle=1;
      my_adc1.DMA_left=real_number;
			DMA1_Channel1->CCR&=~(1<<0); 	//关闭DMA传输 
			while(DMA1_Channel1->CCR&0X1);	//确保DMA可以被设置
			DMA1_Channel1->CCR&=~(1<<2);		//允许半传输中断
			DMA1_Channel1->CCR&=~(1<<5);		//循环模式
			DMA1_Channel1->CMAR=(u32)adc1_buffer; //DMA 存储器0地址
			DMA1_Channel1->CNDTR=real_number;	 //DMA 传输的数据项数,为0暂不传输
			DMA1->IFCR|=15<<0;				//清空Stream1所有中断标识
			//warn: 这里必须手动指定
			DMA1_Channel1->CCR|=1<<0;		//开启DMA传输
		}
		ADC1->CR2|=(1<<1);   //开启连续转换模式
		ADC1->CR2|=1<<22;       //启动规则转换通道
		my_adc1.start_time=get_us();//得到us级别的返回时间

	}
}

void adc1_data_deal(u8 bc)
{
	u16 i=0;
	static u32 sum[MY_ADC_MAX_CHANNEL_NUMBER]={0};
	static u8 j=0;
	u16 off_set=bc*adc1_max_number/2;
	if(my_adc1.DMA_cycle==1&&(bc==2||bc==3||bc==4))//表示已经结束
	{
		if(bc==4)//低于1000次的情况
		{
			while(i<my_adc1.DMA_left)
			{
				sum[j]+=adc1_buffer[i];
				i++;
				j=(j+1)%my_adc1.channel_number;
			}
			my_adc1.now_number+=my_adc1.DMA_left;//采集项目累加
		}
		else if(bc==2)//剩余数据项数小于500
		{
			off_set=0;
			while(i<my_adc1.DMA_left)
			{
				sum[j]+=adc1_buffer[i+off_set];
				i++;
				j=(j+1)%my_adc1.channel_number;
			}
			my_adc1.now_number+=my_adc1.DMA_left;//采集项目累加
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
			my_adc1.now_number+=(my_adc1.DMA_left-off_set);//采集项目累加
		}
		for(i=0;i<my_adc1.channel_number;i++)
		{
			//这里是数据平均以及线性拟合的地方
			my_adc1.data[i]->anumber=my_adc1.now_number/my_adc1.channel_number;//已累计的数目
			my_adc1.data[i]->aorigin=(double)sum[i]/my_adc1.data[i]->anumber;//计算平均值
			my_adc1.data[i]->linear.lvalue=my_adc1.data[i]->linear.A*my_adc1.data[i]->aorigin+my_adc1.data[i]->linear.C;//标准的线性拟合
			if(j!= 0)
			{
				//存在计数错误
				printf("adc存在计数错误");
			}
		}
		//重置累计数据区
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
		my_adc1.now_number+=500;//采集项目加500
	}	
}

void adc1_data_back(void)
{
	u8 i=0;
	printf("采集次数%d,总花费时间:%d us.\n",my_adc1.data[0]->anumber,my_adc1.spend_time);
	for(i=0;i<my_adc1.channel_number;i++)
	{
    printf("采样通道%d,原始平均数据:%0.2f,线性拟合:%0.6f*%0.2f+%0.6f=%0.6f.\n",my_adc1.data[i]->channel,my_adc1.data[i]->aorigin,my_adc1.data[i]->linear.A,my_adc1.data[i]->aorigin,my_adc1.data[i]->linear.C,my_adc1.data[i]->linear.lvalue);
	}
}
void  DMA1_Channel1_IRQHandler(void)//ADC采集DMA中断
{
	if(DMA1->ISR&(1<<1))//传输完成中断
	{		
		if(my_adc1.DMA_cycle==1)
		{
			//采集结束
			ADC1->CR2&=~(1<<1);//清零CONT持续转换位。关闭adc
			DMA1_Channel1->CCR&=~(1<<0); //关闭DMA传输
			if(DMA1_Channel1->CCR&(1<<5))//判断是否处于循环模式
				adc1_data_deal(3);
			else
				adc1_data_deal(4);//非循环模式处理函数
			my_adc1.spend_time=get_us()-my_adc1.start_time;//得到总花费时间
			
		}
		else
		{
			//调用累积处理函数，生成平均值信息
			adc1_data_deal(1);
		}
    my_adc1.DMA_cycle--;
	}
	else if(DMA1->ISR&(1<<2))//半传输中断
	{
		if(my_adc1.DMA_cycle==1&&my_adc1.DMA_left<=adc1_max_number/2)
		{
			//采集结束
			ADC1->CR2&=~(1<<1);//清零CONT持续转换位。关闭adc
			DMA1_Channel1->CCR&=~(1<<0); //关闭DMA传输
			//调用处理函数返回结果
			adc1_data_deal(2);
			my_adc1.spend_time=get_us()-my_adc1.start_time;//得到总花费时间 
			//adc1_data_back();
		}
		else
		{
			//调用累积处理函数，生成平均值信息
			adc1_data_deal(1);
		}
	}
	DMA1->IFCR |=15<<0;//清空通道2所有中断标识
}


 
void my_adc1_close(void)
{
	ADC1->CR2&=~(1<<1);//清零CONT持续转换位。
}

//快速软件触发的多个通道的adc
void my_adc1_init(void)//
{    
	//先初始化IO口
  RCC->APB2ENR|=1<<3;    //使能PORTB口时钟 
	GPIOB->CRL&=0XFFFFFF0F;//PB1 anolog输入 
	//这里选取输入的IO口	 		 
	RCC->APB2ENR|=1<<9;    //ADC1时钟使能	  
	RCC->APB2RSTR|=1<<9;   //ADC1复位
	RCC->APB2RSTR&=~(1<<9);//复位结束	    
	//RCC->CFGR&=~(3<<14);   //分频因子清零	
	//SYSCLK/DIV2=12M ADC时钟设置为12M,ADC最大时钟不能超过14M!
	//否则将导致ADC准确度下降! 
	//RCC->CFGR|=2<<14;      	 

	ADC1->CR1=0;   //工作模式清零
	//ADC1工作在默认模式下，无中断产生，单纯的ADC模式  
	ADC1->CR2|=(1<<1);//工作在连续模式，通过DMA中断控制数目
	ADC1->CR2|=(1<<8);//使用DMA模式

	ADC1->CR2&=~(7<<17);	   
	ADC1->CR2|=7<<17;//软件控制转换

	ADC1->CR2|=1<<20;  //使用用外部触发(SWSTART)!!!	必须使用一个事件来触发
	ADC1->CR2&=~(1<<11);   //右对齐
	ADC1->CR2|=(1<<23);//开启内部温度传感器模块	

	//ADC1->SQR1&=~(0XF<<20);
	//ADC1->SQR1&=0<<20;     //1个转换在规则序列中 也就是只转换规则序列1 			   
	//设置通道8的采样时间
	//ADC1->SMPR2&=0XF0FFFFFF;//通道采样时间清空	  
	//ADC1->SMPR2|=7<<24;      //通道  239.5周期,提高采样时间可以提高精确度	 

	ADC1->CR2|=1<<0;	    //开启AD转换器
	//下面是校准	 
	ADC1->CR2|=1<<3;        //使能复位校准  
	while(ADC1->CR2&1<<3);  //等待校准结束 			 
    //该位由软件设置并由硬件清除。在校准寄存器被初始化后该位将被清除。 		 
	ADC1->CR2|=1<<2;        //开启AD校准	   
	while(ADC1->CR2&1<<2);  //等待校准结束
}	

void DMA_for_ADC1_init(void)
{ 
	RCC->AHBENR|=1<<0;//DMA1时钟使能 
	
	while(DMA1_Channel1->CCR&0X01);
	//等待DMA可配置 必须En位=0，即不传输的时候才可以写入。

	DMA1->IFCR |=15<<0;//清空通道1所有中断标识

	//下面是通道3配置
	DMA1_Channel1->CPAR=(u32)&(ADC1->DR); //DMA外设地址
	DMA1_Channel1->CMAR=0; 		//DMA 存储器0地址
	DMA1_Channel1->CNDTR=0;		//DMA 传输的数据项数,为0暂不传输
	DMA1_Channel1->CCR=0;		//先全部复位CR寄存器值 
	                                                
	//开启完成中断和错误中断（直接模式错误中断在存储器地址递增情况下无用）

	//DMA1_Channel1->CCR|=1<<3;		//传输错误中断使能
	DMA1_Channel1->CCR|=1<<1;		//允许传输完成中断
	DMA1_Channel1->CCR|=0<<2;		//不允许半传输中断
	DMA1_Channel1->CCR|=0<<4;		//数据传输方向，外设到存储器模式
	DMA1_Channel1->CCR|=0<<5;		//非循环模式
	DMA1_Channel1->CCR|=0<<6;		//外设非增量模式，外设地址固定
	DMA1_Channel1->CCR|=1<<7;		//存储器增量模式，地址增加值为Msize=2字节
	DMA1_Channel1->CCR|=1<<8;		//外设数据长度:16位
	DMA1_Channel1->CCR|=1<<10;		//存储器数据长度:16位
	//UART是8位对齐数据
	DMA1_Channel1->CCR|=2<<12;		//一般优先级
	//直接模式下，上面强制为0
  NVIC_enable(3,DMA1_Channel1_IRQn);//组4，3级优先级
}

//序列采样模式的序列表及长度
void adc1_set_sequence(u8* channel,u8 number)
{
	u8 i=0;
	u8 real_num=0;//有效通道数量
	if(number>MY_ADC_MAX_CHANNEL_NUMBER)
	{
		//序列过长
		number=MY_ADC_MAX_CHANNEL_NUMBER;
	}
	for(i=0;i<number;i++)
	{
		if(channel[i]<=17)//最多17个通道
		{
			real_num++;
			if(i<6)//第一个寄存器中
			{
				ADC1->SQR3&=~(31<<(5*i));//规则序列1 通道ch	
  				ADC1->SQR3|=channel[i]<<(5*i);//写入对应序列
			}
			else if(i<12)
			{
				ADC1->SQR2&=~(31<<(5*(i-6)));//规则序列1 通道ch	
  				ADC1->SQR2|=channel[i]<<(5*i);
			}
			else
			{
				ADC1->SQR1&=~(31<<(5*(i-12)));//规则序列1 通道ch	
  				ADC1->SQR1|=channel[i]<<(5*i);
			}
		}
	}
	ADC1->SQR1&=~(15<<20);
	ADC1->SQR1|=(real_num-1)<<20;//0代表1个转换，写入有效转换数目
	//发送回执信息
	printf("有效转换通道数目：%d \n",real_num);
}

//设置adc通道对应的采样的时间
//000：1.5周期 100：41.5周期
//001：7.5周期 101：55.5周期
//010：13.5周期 110：71.5周期
//011：28.5周期 111：239.5周期
//总转化时间12.5+sample，ADC clock=12Mhz。
void adc1_set_sample_time(u8 channel,u8 tid)
{
	u8 ch_offset=0;
	if(tid>7)//非法数字
		tid=7;//最大采样时间
	if(channel<10)//在SMPR2寄存器里面
	{
		ch_offset=channel*3;
		ADC1->SMPR2&=~(7<<(ch_offset));//通道采样时间清空	  
		ADC1->SMPR2|=tid<<(ch_offset);//赋值
	}
	else
	{
		ch_offset=(channel-10)*3;//从第十个通道开始算起
		ADC1->SMPR1&=~(7<<(ch_offset));//通道采样时间清空	  
		ADC1->SMPR1|=tid<<(ch_offset);//赋值
	}
	//返回已设置的通道信息
	printf("第%d个通道采样时间设置为%d周期，采样时间：%0.2f us。\n",channel,my_adc1.stid[tid],(float)my_adc1.stid[tid]/12);	
}

//获取通道ch的转换值，取times次,然后平均 
//ch:通道编号
//times:获取次数
//返回值:通道ch的times次转换结果平均值
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
    my_adc1.set_sequence(my_adc1.channel,my_adc1.channel_number);//注册通道数目
    my_adc1.set_sample_time(my_adc1.data[0]->channel,ADC_SAMPLE_TIME_55p5);
    my_adc1.set_sample_time(my_adc1.data[1]->channel,ADC_SAMPLE_TIME_55p5);
}
	





