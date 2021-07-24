#ifndef __TIMER_H
#define __TIMER_H
#include "sys.h"
#define MAX_PWM   6400//此时PWM10KHz
#define PWMB2   TIM4->CCR1  //PB6
#define PWMB1   TIM4->CCR2  //PB7

#define PWMA2   TIM4->CCR3  //PB8
#define PWMA1   TIM4->CCR4  //PB9
void A4950T_Init();

void fast_back_a(u16 ain);
void fast_stop(void);
void fast_forward_a(u16 ain);
void slow_stop(void);
void slow_back_a(u16 ain1);
void slow_forward_a(u16 ain2);
void fast_back_b(u16 bin);
void fast_stop(void);
void fast_forward_b(u16 bin);
void slow_stop(void);
void slow_back_b(u16 bin1);
void slow_forward_b(u16 bin2);
void set_pwmb2(u16 pwm);
void set_pwmb1(u16 pwm);
void set_pwma2(u16 pwm);
void set_pwma1(u16 pwm);
void motor_forth();//前行
#endif