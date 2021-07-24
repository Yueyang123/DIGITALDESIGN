#ifndef SERVO_H
#define SERVO_H
#include "sys.h"

#define SERVO_K 2491.2f
#define SERVO_INIT  2800
#define SERVO   TIM1->CCR4  //�������
void SERVO_Init();
void SERVO_Set_Angle(float angle);
void SERVO_Set_ZERO();
#endif