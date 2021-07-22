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
#include "mpu6050.h"
#include "inv_mpu.h"
#include "inv_mpu_dmp_motion_driver.h" 
 
void show(void);//小显示屏刷新函数
void myshow(void);//我的展示函数

//串口1发送1个字符 
//c:要发送的字符
void usart1_send_char(u8 c)
{   	
	while(USART_GetFlagStatus(USART1,USART_FLAG_TC)==RESET); //循环发送,直到发送完毕   
	USART_SendData(USART1,c);  
} 
//传送数据给匿名四轴上位机软件(V2.6版本)
//fun:功能字. 0XA0~0XAF
//data:数据缓存区,最多28字节!!
//len:data区有效数据个数
void usart1_niming_report(u8 fun,u8*data,u8 len)
{
	u8 send_buf[32];
	u8 i;
	if(len>28)return;	//最多28字节数据 
	send_buf[len+3]=0;	//校验数置零
	send_buf[0]=0X88;	//帧头
	send_buf[1]=fun;	//功能字
	send_buf[2]=len;	//数据长度
	for(i=0;i<len;i++)send_buf[3+i]=data[i];			//复制数据
	for(i=0;i<len+3;i++)send_buf[len+3]+=send_buf[i];	//计算校验和	
	for(i=0;i<len+4;i++)usart1_send_char(send_buf[i]);	//发送数据到串口1 
}
//发送加速度传感器数据和陀螺仪数据
//aacx,aacy,aacz:x,y,z三个方向上面的加速度值
//gyrox,gyroy,gyroz:x,y,z三个方向上面的陀螺仪值
void mpu6050_send_data(short aacx,short aacy,short aacz,short gyrox,short gyroy,short gyroz)
{
	u8 tbuf[12]; 
	tbuf[0]=(aacx>>8)&0XFF;
	tbuf[1]=aacx&0XFF;
	tbuf[2]=(aacy>>8)&0XFF;
	tbuf[3]=aacy&0XFF;
	tbuf[4]=(aacz>>8)&0XFF;
	tbuf[5]=aacz&0XFF; 
	tbuf[6]=(gyrox>>8)&0XFF;
	tbuf[7]=gyrox&0XFF;
	tbuf[8]=(gyroy>>8)&0XFF;
	tbuf[9]=gyroy&0XFF;
	tbuf[10]=(gyroz>>8)&0XFF;
	tbuf[11]=gyroz&0XFF;
	usart1_niming_report(0XA1,tbuf,12);//自定义帧,0XA1
}	
//通过串口1上报结算后的姿态数据给电脑
//aacx,aacy,aacz:x,y,z三个方向上面的加速度值
//gyrox,gyroy,gyroz:x,y,z三个方向上面的陀螺仪值
//roll:横滚角.单位0.01度。 -18000 -> 18000 对应 -180.00  ->  180.00度
//pitch:俯仰角.单位 0.01度。-9000 - 9000 对应 -90.00 -> 90.00 度
//yaw:航向角.单位为0.1度 0 -> 3600  对应 0 -> 360.0度
void usart1_report_imu(short aacx,short aacy,short aacz,short gyrox,short gyroy,short gyroz,short roll,short pitch,short yaw)
{
	u8 tbuf[28]; 
	u8 i;
	for(i=0;i<28;i++)tbuf[i]=0;//清0
	tbuf[0]=(aacx>>8)&0XFF;
	tbuf[1]=aacx&0XFF;
	tbuf[2]=(aacy>>8)&0XFF;
	tbuf[3]=aacy&0XFF;
	tbuf[4]=(aacz>>8)&0XFF;
	tbuf[5]=aacz&0XFF; 
	tbuf[6]=(gyrox>>8)&0XFF;
	tbuf[7]=gyrox&0XFF;
	tbuf[8]=(gyroy>>8)&0XFF;
	tbuf[9]=gyroy&0XFF;
	tbuf[10]=(gyroz>>8)&0XFF;
	tbuf[11]=gyroz&0XFF;	
	tbuf[18]=(roll>>8)&0XFF;
	tbuf[19]=roll&0XFF;
	tbuf[20]=(pitch>>8)&0XFF;
	tbuf[21]=pitch&0XFF;
	tbuf[22]=(yaw>>8)&0XFF;
	tbuf[23]=yaw&0XFF;
	usart1_niming_report(0XAF,tbuf,28);//飞控显示帧,0XAF
} 
int main(void)
{
		u32 t,len;
		float pitch,roll,yaw; 		//欧拉角
		short aacx,aacy,aacz;		//加速度传感器原始数据
		short gyrox,gyroy,gyroz;	//陀螺仪原始数据
		short temp;
		unsigned long step;
    uart_init(500000);
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
		MPU_Init();
		while(mpu_dmp_init())
		{
			printf("DMP ERROR \n");
		} 
		while(1)
		{
			LED0=!LED0;
			myshow();
			OLED_Refresh_Gram();

			while(mpu_dmp_get_data(&pitch,&roll,&yaw)!=0){};  
			//printf("pitch=%f\troll=%f\tyaw=%f\r\n",pitch,roll,yaw);
				temp=MPU_Get_Temperature();	//得到温度值
				MPU_Get_Accelerometer(&aacx,&aacy,&aacz);	//得到加速度传感器数据
				MPU_Get_Gyroscope(&gyrox,&gyroy,&gyroz);	//得到陀螺仪数据
				//mpu6050_send_data(aacx,aacy,aacz,gyrox,gyroy,gyroz);//用自定义帧发送加速度和陀螺仪原始数据
			  //usart1_report_imu(aacx,aacy,aacz,gyrox,gyroy,gyroz,(int)(roll*100),(int)(pitch*100),(int)(yaw*10));
				dmp_get_pedometer_step_count(&step);
				printf("(%d,%d,%d,%d,%d,%d,%d,%d,%f,%f,%f)",step,temp,aacx,aacy,aacz,gyrox,gyroy,gyroz,pitch,roll,yaw);
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