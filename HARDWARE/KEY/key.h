#ifndef __KEY_H
#define __KEY_H	 
#include "sys.h"
	 

//#define WK_UP PAin(0)	//PA0  WK_UP


#define KEY0   GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0)//��ȡ����3(WK_UP) 

 

#define KEY0_PRES 	1	//KEY0����



void KEY_Init(void);//IO��ʼ��
u8 KEY_Scan(void);  	//����ɨ�躯��					    
#endif
