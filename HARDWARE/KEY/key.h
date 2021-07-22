#ifndef __KEY_H
#define __KEY_H	 
#include "sys.h"
	 

//#define WK_UP PAin(0)	//PA0  WK_UP


#define KEY0   GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0)//读取按键3(WK_UP) 

 

#define KEY0_PRES 	1	//KEY0按下



void KEY_Init(void);//IO初始化
u8 KEY_Scan(void);  	//按键扫描函数					    
#endif
