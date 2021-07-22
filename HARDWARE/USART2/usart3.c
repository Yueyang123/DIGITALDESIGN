#include "delay.h"
#include "usart3.h"
#include "stdarg.h"      
#include "stdio.h"       
#include "string.h"    
u8  USART3_RX_BUF[USART3_MAX_RECV_LEN];      
u8  USART3_TX_BUF[USART3_MAX_SEND_LEN]; 
u16 USART3_RX_STA=0;     
void USART3_IRQHandler(void)
{
    u8 res;     
    if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)
    {    
			res =USART_ReceiveData(USART3);     
        if(USART3_RX_STA<USART3_MAX_RECV_LEN)       
        {
//            TIM_SetCounter(TIM4,0);                  
//            if(USART3_RX_STA==0)TIM4_Set(1);     
            USART3_RX_BUF[USART3_RX_STA++]=res;    
        }else 
        {
            USART3_RX_STA|=1<<15;                  
        } 
    }                                            
}   
  
void USART3_Init(u32 bound)
{  

    NVIC_InitTypeDef NVIC_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);   
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);

    USART_DeInit(USART3);  
    //USART3_TX   PB.10
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; //PA.2
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; 
		GPIO_Init(GPIOB, &GPIO_InitStructure); 

		//USART2_RX   PA.3
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
		GPIO_Init(GPIOB, &GPIO_InitStructure);

    USART_InitStructure.USART_BaudRate = bound;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

    USART_Init(USART3, &USART_InitStructure); 

    USART_DMACmd(USART3,USART_DMAReq_Tx,ENABLE);  
    UART_DMA_Config(DMA1_Channel2,(u32)&USART3->DR,(u32)USART3_TX_BUF);
    USART_Cmd(USART3, ENABLE);  

//#ifdef USART3_RX_EN       
//		USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);

//    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
//    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2 ;
//    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;     
//    NVIC_Init(&NVIC_InitStructure); 
//    TIM4_Init(99,7199);     
//    USART3_RX_STA=0;        
//    TIM4_Set(0);           
//#endif      

}

void u3_printf(char* fmt,...)  
{  
    va_list ap;
    va_start(ap,fmt);
    sprintf((char*)USART3_TX_BUF,fmt,ap);
    va_end(ap);
    while(DMA_GetCurrDataCounter(DMA1_Channel2)!=0);    
    UART_DMA_Enable(DMA1_Channel2,strlen((const char*)USART3_TX_BUF)); 
}
         
//void TIM4_IRQHandler(void)
//{   
//    if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)
//    {                  
//        USART3_RX_STA|=1<<15;   
//        TIM_ClearITPendingBit(TIM4, TIM_IT_Update  );   
//        TIM4_Set(0);
//    }       
//}

//void TIM4_Set(u8 sta)
//{
//    if(sta)
//    {

//        TIM_SetCounter(TIM4,0);
//        TIM_Cmd(TIM4, ENABLE);  
//    }else TIM_Cmd(TIM4, DISABLE);   
//}
//    
//void TIM4_Init(u16 arr,u16 psc)
//{   
//    NVIC_InitTypeDef NVIC_InitStructure;
//    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

//    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);   
//    TIM_TimeBaseStructure.TIM_Period = arr;   
//    TIM_TimeBaseStructure.TIM_Prescaler =psc; 
//    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
//    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  
//    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); 

//    TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE ); 


//    NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
//    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;
//    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;         
//    NVIC_Init(&NVIC_InitStructure);

//}
        
void UART_DMA_Config(DMA_Channel_TypeDef*DMA_CHx,u32 cpar,u32 cmar)
{
    DMA_InitTypeDef DMA_InitStructure;
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);  
		DMA_DeInit(DMA_CHx);   
    DMA_InitStructure.DMA_PeripheralBaseAddr = cpar;  
    DMA_InitStructure.DMA_MemoryBaseAddr = cmar; 
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;  
    DMA_InitStructure.DMA_BufferSize = 0; 
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable; 
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; 
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal; 
    DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable; 
    DMA_Init(DMA_CHx, &DMA_InitStructure); 
} 

void UART_DMA_Enable(DMA_Channel_TypeDef*DMA_CHx,u8 len)
{
    DMA_Cmd(DMA_CHx, DISABLE );      
    DMA_SetCurrDataCounter(DMA_CHx,len); 
    DMA_Cmd(DMA_CHx, ENABLE);     
}      
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////                                    

