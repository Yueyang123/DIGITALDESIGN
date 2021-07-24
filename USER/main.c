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
#include "Control.h"
#include "mpu6050.h"
#include "PS2.h"
#include "inv_mpu.h"
#include "inv_mpu_dmp_motion_driver.h" 
 
void show(void);//С��ʾ��ˢ�º���
void myshow(void);//�ҵ�չʾ����


float pitch,roll,yaw; 		//ŷ����
short aacx,aacy,aacz;		//���ٶȴ�����ԭʼ����
short gyrox,gyroy,gyroz;	//������ԭʼ����



void motor_forth()//ǰ��
{
	Control_Set_Vec(200);
	Control_Set_Angle(0);
}
void motor_stop()
{
	Control_Set_Vec(0);
	Control_Set_Angle(0);
}
void motor_back()//����
{
	Control_Set_Vec(-200);
	Control_Set_Angle(0);
}

void motor_left()//��ת
{
	Control_Set_Vec(200);
	Control_Set_Angle(-3.14/6);
}


void motor_right()//��ת
{
	Control_Set_Vec(200);
	Control_Set_Angle(3.14/6);
}


int main(void)
{
		u32 key,len;

    uart_init(921600);
		USART3_Init(921600);
		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);	
    delay_init();//Delay init.
		ADC_init();
	  LED_Init();
		OLED_Init();			//��ʼ��OLED   
		SERVO_Init();	
		show();
	  A4950T_Init();
		Encoder_Init();
		Controler_Init();
		MPU_Init();
		PS2_Init();
//		while(mpu_dmp_init())
//		{
//			printf("DMP ERROR \n");
//		} 
		while(1)
		{
			
			myshow();
			OLED_Refresh_Gram();
			Control_Run();
					PS2_ClearData();   //�������
					key=0;
					key=PS2_DataKey();
					if(key!=0)
					{
						LED0=!LED0;
						char buffer[20]={0};
						sprintf(buffer," \r\n %d is pressed \r\n",key);
						u3_printf(buffer);
						if(key == PSB_PAD_UP)
						{
							 motor_forth();//ǰ��;
						}
						else if(key == PSB_PAD_DOWN)
						{
							motor_back();
						}
						else if(key == PSB_PAD_LEFT)
						{
							motor_left();
						}
						else if(key == PSB_PAD_RIGHT)
						{
							motor_right();
						}
						else if(key == PSB_RED)
						{
							motor_stop();
						}
						else
						PS2_Vibration(0x00,0x00);
					}
		}
}



//void show(void)
//{
//	OLED_ShowString(4,0, " STEP:");
//	OLED_ShowString(4,16,  "ROLL:");
//	OLED_ShowString(4,32,  "PIT:");
//	OLED_ShowString(4,48," YAW:");
//}
//void myshow(void)
//{
//				while(mpu_dmp_get_data(&pitch,&roll,&yaw)!=0){};  
//				printf("pitch=%f\troll=%f\tyaw=%f\r\n",pitch,roll,yaw);
//				temp=MPU_Get_Temperature();	//�õ��¶�ֵ
//				MPU_Get_Accelerometer(&aacx,&aacy,&aacz);	//�õ����ٶȴ���������
//				MPU_Get_Gyroscope(&gyrox,&gyroy,&gyroz);	//�õ�����������
//				mpu6050_send_data(aacx,aacy,aacz,gyrox,gyroy,gyroz);//���Զ���֡���ͼ��ٶȺ�������ԭʼ����
//			  usart3_report_imu(aacx,aacy,aacz,gyrox,gyroy,gyroz,(int)(roll*100),(int)(pitch*100),(int)(yaw*10));
//				dmp_get_pedometer_step_count(&step);
//    char buffer[20]={0};
//    sprintf(buffer,"%d ",step);
//    OLED_ShowString(60,0,(u8*)buffer);
//		sprintf(buffer,"%0.2f ",roll);
//    OLED_ShowString(60,16,(u8*)buffer);
//		sprintf(buffer,"%0.2f ",pitch);
//    OLED_ShowString(60,32,(u8*)buffer);
//		sprintf(buffer,"%0.2f ",yaw);
//    OLED_ShowString(60,48,(u8*)buffer);
//}
void show(void)
{
		OLED_ShowString(4,0, " LEFT:");
		OLED_ShowString(4,16," RIGHT:");
		OLED_ShowString(4,32," SERVO:");
		OLED_ShowString(4,48," DIS:");
}
void myshow(void)
{
    char buffer[20]={0};
    sprintf(buffer,"%d ",robotstatus.left_speed);
    OLED_ShowString(60,0,(u8*)buffer);
		sprintf(buffer,"%d ",robotstatus.right_speed);
    OLED_ShowString(60,16,(u8*)buffer);
		sprintf(buffer,"%d ",robotstatus.Servo);
    OLED_ShowString(60,32,(u8*)buffer);		
		sprintf(buffer,"%0.2f ",robotstatus.distance);
    OLED_ShowString(40,48,(u8*)buffer);		
				
//		char buffer1[50]={0};

//		sprintf(buffer1,"(%f,%d,%d,%d,%d,%d,%d,%d)",robotstatus.distance,right_controler.target,left_controler.target, robotstatus.motor_left,robotstatus.motor_right,robotstatus.Servo,robotstatus.left_speed,robotstatus.right_speed);
//		u3_printf(buffer1);
}