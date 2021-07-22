#include "led.h"
#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "adc.h"
#include "usart3.h"
#include "OLED.h"
#include "SERVO.h"
#include "A4950T.h"
#include "Encoder.h"
void show(void);//小显示屏刷新函数
void myshow(void);//我的展示函数



int main(void)
{
		u32 t,len;
	float angle;
    uart_init(921600);
		USART3_Init(921600);
		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);	
    delay_init();//Delay init.
		ADC_init();
	  LED_Init();
		OLED_Init();			//初始化OLED   
		SERVO_Init();	
		show();
	  A4950T_Init();
		Encoder_Init();
		while(1)
		{
			delay_ms(1000);
			LED0=!LED0;
			slow_back_b(3000);
			slow_back_a(4000);
			myshow();
			OLED_Refresh_Gram();
			u3_printf("(%d,%d)",Read_Encoder(2)-10000,10000-Read_Encoder(3));
		}
}

void show(void)
{
    OLED_ShowString(4,0, " Power103 on ! ");
    OLED_ShowString(4,16,  " volt:");
    OLED_ShowString(4,32," Runing time:  ");
}
void myshow(void)
{
    char buffer[20]={0};
    sprintf(buffer,"%0.2f V",(float)Get_battery_volt());
    OLED_ShowString(60,16,(u8*)buffer);
		sprintf(buffer,"%0.2f s",(float)get_ms()/1000);
    OLED_ShowString(32,48,(u8*)buffer);
}