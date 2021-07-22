#ifndef __DELAY_H
#define __DELAY_H 			   
#include "sys.h"  
 
void delay_init(void);
void delay_ms(u32 nms);
void delay_us(u32 nus);
u32 get_us(void);
u32 get_ms(void);
#endif





























