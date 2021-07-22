#ifndef __ADC_H
#define __ADC_H	
#include "sys.h"
#define Battery_Ch 9
u16 Get_Adc(u8 ch);
float Get_battery_volt(void);   
void Angle_Adc_Init(void);   
void  Baterry_Adc_Init(void);
int Get_Adc_Average(u8 ch,u8 times);
void adc1_data_back(void);
void ADC_init(void);

#define ADC_CONVERSION_TIME  12.5 //采样转换12.5周期
#define ADC_SAMPLE_TIME_1p5  0 //采样周期1.5个周期
#define ADC_SAMPLE_TIME_7p5  1 //采样周期1.5个周期
#define ADC_SAMPLE_TIME_13p5  2 //采样周期1.5个周期
#define ADC_SAMPLE_TIME_28p5  3 //采样周期1.5个周期
#define ADC_SAMPLE_TIME_41p5  4 //采样周期1.5个周期
#define ADC_SAMPLE_TIME_55p5  5 //采样周期1.5个周期
#define ADC_SAMPLE_TIME_71p5  6 //采样周期1.5个周期
#define ADC_SAMPLE_TIME_239p5  7 //采样周期1.5个周期
#define MY_ADC_MAX_CHANNEL_NUMBER 8 //暂时只用8个通道
typedef struct __linear //线性拟合
{
    float A;//系数
    float C;//常数
    float lvalue;//拟合值
}_linear;
typedef struct __adc_data
{
    u8 channel;//通道代号
    u8 fit_id;//滤波算法代号
    float aorigin;//滤波并且平均之后原始数据
    u16 anumber;//平均次数
    _linear linear;//线性拟合参数和结果
}_adc_data;

typedef struct __my_adc
{
    void (*init)(void);//初始化函数
    void (*set_sequence)(u8* channel,u8 number);//设置序列信息
    void (*set_sample_time)(u8 channel,u8 tid);//设置通道采样时间
    void (*start)(u16 snumber);//开始转换
    void (*close)(void);//关闭adc
    u8 channel[MY_ADC_MAX_CHANNEL_NUMBER];//最多十六个序列,但这里只开了8个序列
    u8 channel_number;//需要转换的通道数量
    volatile u16 DMA_cycle;//受数据缓冲区限制，DMA需要循环的次数
    u16 DMA_left;//额外多出来的传输数目
    u16 now_number;//当前已采集的数据项数
    _adc_data* data[MY_ADC_MAX_CHANNEL_NUMBER];//数据指针，指向对应序列的数据处理信息，
    u8 stid[8];//采样时间
    u32 start_time;//起始的时间
    u32 spend_time;//花费的时间
}_my_adc;

extern _my_adc my_adc1;//实例化的adc采集结构

#endif 


