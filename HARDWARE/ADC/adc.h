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

#define ADC_CONVERSION_TIME  12.5 //����ת��12.5����
#define ADC_SAMPLE_TIME_1p5  0 //��������1.5������
#define ADC_SAMPLE_TIME_7p5  1 //��������1.5������
#define ADC_SAMPLE_TIME_13p5  2 //��������1.5������
#define ADC_SAMPLE_TIME_28p5  3 //��������1.5������
#define ADC_SAMPLE_TIME_41p5  4 //��������1.5������
#define ADC_SAMPLE_TIME_55p5  5 //��������1.5������
#define ADC_SAMPLE_TIME_71p5  6 //��������1.5������
#define ADC_SAMPLE_TIME_239p5  7 //��������1.5������
#define MY_ADC_MAX_CHANNEL_NUMBER 8 //��ʱֻ��8��ͨ��
typedef struct __linear //�������
{
    float A;//ϵ��
    float C;//����
    float lvalue;//���ֵ
}_linear;
typedef struct __adc_data
{
    u8 channel;//ͨ������
    u8 fit_id;//�˲��㷨����
    float aorigin;//�˲�����ƽ��֮��ԭʼ����
    u16 anumber;//ƽ������
    _linear linear;//������ϲ����ͽ��
}_adc_data;

typedef struct __my_adc
{
    void (*init)(void);//��ʼ������
    void (*set_sequence)(u8* channel,u8 number);//����������Ϣ
    void (*set_sample_time)(u8 channel,u8 tid);//����ͨ������ʱ��
    void (*start)(u16 snumber);//��ʼת��
    void (*close)(void);//�ر�adc
    u8 channel[MY_ADC_MAX_CHANNEL_NUMBER];//���ʮ��������,������ֻ����8������
    u8 channel_number;//��Ҫת����ͨ������
    volatile u16 DMA_cycle;//�����ݻ��������ƣ�DMA��Ҫѭ���Ĵ���
    u16 DMA_left;//���������Ĵ�����Ŀ
    u16 now_number;//��ǰ�Ѳɼ�����������
    _adc_data* data[MY_ADC_MAX_CHANNEL_NUMBER];//����ָ�룬ָ���Ӧ���е����ݴ�����Ϣ��
    u8 stid[8];//����ʱ��
    u32 start_time;//��ʼ��ʱ��
    u32 spend_time;//���ѵ�ʱ��
}_my_adc;

extern _my_adc my_adc1;//ʵ������adc�ɼ��ṹ

#endif 


