/*
 * @Descripttion: 
 * @version: 
 * @Author: Yueyang
 * @email: 1700695611@qq.com
 * @Date: 2021-07-23 12:01:24
 * @LastEditors: Yueyang
 * @LastEditTime: 2021-07-23 13:00:41
 */
#include "Control.h"
#include "SERVO.h"
#include "Encoder.h"
#include "usart.h"
#include "A4950T.h"
#include <math.h>
controler right_controler,left_controler;
controler servo_controler;
Robotstatus robotstatus;

//0-700
void Control_Set_Vec(float velocity)
{
	robotstatus.Velocity=velocity;
}

//-3.14/2-3.14/2
void Control_Set_Angle(float angle)
{
	robotstatus.Angle=angle;
}
/**************************************************************************
函数功能：增量PI控制器
入口参数：编码器测量值，目标速度
返回  值：电机PWM
根据增量式离散PID公式 
pwm+=Kp[e（k）-e(k-1)]+Ki*e(k)+Kd[e(k)-2e(k-1)+e(k-2)]
e(k)代表本次偏差 
e(k-1)代表上一次的偏差  以此类推 
pwm代表增量输出
在我们的速度控制闭环系统里面，只使用PI控制
pwm+=Kp[e（k）-e(k-1)]+Ki*e(k)
**************************************************************************/
int Incremental_PI_A(int Encoder,int Target)
{ 	
	 static int Bias,Pwm,Last_bias;
	 Bias=Target-Encoder;                //计算偏差
	 Pwm+=Velocity_KP*(Bias-Last_bias)+Velocity_KI*Bias;   //增量式PI控制器
	 Last_bias=Bias;	                   //保存上一次偏差 
	 return Pwm;                         //增量输出
}
int Incremental_PI_B(int Encoder,int Target)
{ 	
	 static int Bias,Pwm,Last_bias;
	 Bias=Target-Encoder;                //计算偏差
	 Pwm+=Velocity_KP*(Bias-Last_bias)+Velocity_KI*Bias;   //增量式PI控制器
	 Last_bias=Bias;	                   //保存上一次偏差 
	 return Pwm;                         //增量输出
}
int Incremental_SERVO(int Encoder,int Target)
{ 	
	 return Target;                         //增量输出
}
void Controler_Init()
{
    right_controler.target=0;
    right_controler.current=0;
    right_controler.controlway=Incremental_PI_B;
    left_controler.target=0;
    left_controler.current=0;
    left_controler.controlway=Incremental_PI_B;
    servo_controler.target=2800;
    servo_controler.current=2800;
    servo_controler.controlway=Incremental_SERVO;
	

		robotstatus.motor_left=0;
		robotstatus.motor_right=0;
		robotstatus.left_speed=0;
		robotstatus.right_speed=0;
		robotstatus.Angle=0;
		robotstatus.Velocity=0.5;
		robotstatus.Servo=2800;
}


//双轮驱动转换计算差速
void Control_Kinematic_Analysis(float velocity,float angle)
{
  //逻辑方向和实际方向是反的
		right_controler.target=velocity*(1-T*tan(angle)/2/L); 
		left_controler.target=velocity*(1+T*tan(angle)/2/L);      //后轮差速
		servo_controler.target=SERVO_INIT+angle*SERVO_K;                    //舵机转向   
}
void Set_Pwm(int motor_a,int motor_b,int servo)
{

  if(motor_a<0)//右电机，对应left
        fast_back_a(-motor_a);
    else
        fast_forward_a(motor_a);
		
    if(motor_b<0)
    	 fast_back_b(-motor_b);
    else
     	 fast_forward_b(motor_b);
     SERVO=servo;	
}
void Xianfu_Pwm(void)
{	
	int Amplitude=6000;    //===PWM满幅是6400 限制在6900
  if(robotstatus.motor_left<-Amplitude)robotstatus.motor_left=-Amplitude;	
	if(robotstatus.motor_left>Amplitude)  robotstatus.motor_left=Amplitude;
  if(robotstatus.motor_left>-3600&&robotstatus.motor_left <3600)
      robotstatus.motor_left=0;
	if(robotstatus.motor_right<-Amplitude) robotstatus.motor_right=-Amplitude;	
	if(robotstatus.motor_right>Amplitude)  robotstatus.motor_right=Amplitude;	
  if(robotstatus.motor_right>-3600&&robotstatus.motor_right<3600)
    robotstatus.motor_right=0;
	if(robotstatus.Servo<(SERVO_INIT-2000))     robotstatus.Servo=SERVO_INIT-2000;	  //舵机限幅
	if(robotstatus.Servo>(SERVO_INIT+2000))     robotstatus.Servo=SERVO_INIT+2000;		  //舵机限幅
}

//完成一次速度和方向的控制
//从可以理解的方向和速度数据转换成PWM数据
void Control_Run()
{
	  right_controler.current=Read_Encoder(2)-10000;
    left_controler.current=10000-Read_Encoder(3);
		robotstatus.distance+=3.14*2*3.3*(right_controler.current+left_controler.current)/(2*390.0f);
    Control_Kinematic_Analysis(robotstatus.Velocity ,robotstatus.Angle);
		robotstatus.left_speed=left_controler.current;
		robotstatus.right_speed=right_controler.current;
		robotstatus.motor_left=left_controler.controlway(left_controler.current,left_controler.target);//===速度闭环控制计算电机A最终PWM
		robotstatus.motor_right=right_controler.controlway(right_controler.current,right_controler.target);//===速度闭环控制计算电机B最终PWM
		servo_controler.target=servo_controler.controlway(servo_controler.current,servo_controler.target);
		robotstatus.Servo=servo_controler.target;
		Xianfu_Pwm();//===PWM限幅
		Set_Pwm(robotstatus.motor_left,robotstatus.motor_right,robotstatus.Servo);//===赋值给PWM寄存器  
}
