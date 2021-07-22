#ifndef __PWM__H
#define __PWM__H
#include "sys.h"
#include "stm32f10x_tim.h"


void TIM4_PWM_Init(u16 arr,u16 psc);
void TIM3_Int_Init(u16 arr,u16 psc);
void TIM3_PWM_Init(u16 arr,u16 psc);

#endif
