/*
 * @Descripttion: 
 * @version: 
 * @Author: Yueyang
 * @email: 1700695611@qq.com
 * @Date: 2021-07-23 12:01:34
 * @LastEditors: Yueyang
 * @LastEditTime: 2021-07-23 12:52:27
 */
#ifndef CONTROL_H
#define CONTROL_H

#define LENGTH_ROLL 20.72
#define Velocity_KP 12
#define Velocity_KI 5
#define Velocity_KD 0
#define T           0.156f
#define L           0.1445f

typedef int class_control;
//后轮转速控制控制器

typedef struct CONTROLER
{
    class_control current;
    class_control target;
    class_control (*controlway)(class_control current,class_control target);
    
}controler;


typedef struct RobotStatus{
	//PWM数据包 反应机器人当前状态
	int motor_left,motor_right,Servo;
	//机器人转速和角度
	float Velocity,Angle;
	//机器人真实转速
	int left_speed,right_speed;
	//机器人累计运行距离cm
	float distance;
	
}Robotstatus;

void Control_Run();
void Controler_Init();
void Control_Set_Vec(float velocity);
void Control_Set_Angle(float angle);
extern controler right_controler,left_controler;
extern Robotstatus robotstatus;
#endif