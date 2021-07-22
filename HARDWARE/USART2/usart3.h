#ifndef __USART3_H
#define __USART3_H   
#include "sys.h"  


#define USART3_MAX_RECV_LEN     200                
#define USART3_MAX_SEND_LEN     200             
#define USART3_RX_EN            0          

extern u8  USART3_RX_BUF[USART3_MAX_RECV_LEN];      
extern u8  USART3_TX_BUF[USART3_MAX_SEND_LEN];     
extern u16 USART3_RX_STA;                          

void USART3_Init(u32 bound);          
void TIM4_Set(u8 sta);
void TIM4_Init(u16 arr,u16 psc);
void UART_DMA_Config(DMA_Channel_TypeDef*DMA_CHx,u32 cpar,u32 cmar);
void UART_DMA_Enable(DMA_Channel_TypeDef*DMA_CHx,u8 len);
void u3_printf(char* fmt, ...);
#endif

