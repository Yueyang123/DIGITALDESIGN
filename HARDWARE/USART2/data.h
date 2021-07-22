#ifndef __DATA_H
#define __DATA_H

#include "sys.h"
//û��У�顢���롢��Ϣ�˲�
//�������ݲ㴦�����

//������������ǲ��Դ���
#define TEXT_ON  0

#ifndef NULL
#define NULL (void*)(0)
#endif
//done: ������պͷ��͵Ļ������������ͳ��ȣ���Щһ��ȷ���ܵĻ�������С��
#define SEND_BUFFER_LENGTH          1024*2  //������������С
#define RECEIVE_BUFFER_LENGTH       1024*3  //
//note: ������Ĭ�ϻ�������С��ʵ��ʹ��ʱ�����Լ�ָ����С
#define MAX_ORDER_NUMBER            255     //���ָ������
//note: �������ָ����������С�ɽ�Լ�ռ�
#define DEFAULT_SPEED               921600  //Ĭ�ϴ��䲨����
//note: δָ�������ʵ�����£��ײ㴫�䰴��������ʳ�ʼ��
#define SEND_MIN                    5       //���ָ���5�ֽ�
#define RECEIVE_MAX                 255     //���ĵ��ν��ջ���
#define PRINTF_MIN                  10      //������С��������
//note: ��Щ��ȷ�����ν��պͷ��͵��ַ�����
#define DATA_INFO_LENGTH 4 //֡�����ݲ��ִ�С
//note: �������ָ��֡�ı�ʶ���֣�һ�������Ĭ�ϼ���


//��Щ����״̬�꺯��
#define SEND_IMMEDIATELY    111 //��������
#define SEND_LATE           112 //�Ժ���
#define PRINTF_HAVE         113 //���ڴ����͵��ַ�
#define PRINTF_NONE         114 //�����ڴ������ַ�
#define DATA_HALF           115 //���ݰ�������
#define DEAL_FUNC_RW        116 //���Ӻ�����д
#define DEAL_FUNC_W         117 //���Ӻ�������д��

//�����Ǵ����ʶ�����ڼ�¼���󣬲�����
#define ERROR_SEND_LATE          (1<<1)  //������ʱ�����ʶ
#define ERROR_SEND_FAIL          (1<<2)  //����ʧ�ܴ����ʶ
#define ERROR_RECEIVE_FAIL       (1<<1)  //����ʧ�ܴ����ʶ
#define ERROR_RECEIVE_OVER       (1<<2)  //���ո��Ǵ����ʶ
#define ERROR_RECEIVE_KEY        (1<<3)  //������������ʶ
#define ERROR_DEAL_LATE          (1<<4)  //����̫�������ʶ

typedef struct __data_start //ͶӰ��Ϣ�ṹ�壬���ٴ���ָ����Ϣ
{
    u8 data_head;       //֡ͷ��ʼ�ַ���Ĭ��@
    u8 order;           //ָ�����
    u8 size;            //���ݵĴ�С,һ���������255
    u8 data[];          //������ͷ��ַ������������
}_data_start;           //���ڹ�����Ϣ�ṹ
//printf

//ѭ����������ÿ��д�꣬�ضϺ����踴�Ӵ������ɼ���Ϣͷ�Ϳ�����
typedef struct __receive_buffer_info
{
    u8* start_address;          //doing: ��ǰ�������׵�ַ
    u8* end_address;            //β��ַ
    u8* real_end;               //����������β��ַ
    u8* deal_address;           //��Ϣ����ָ�롣ָ���Ѵ�������µ�ַ
    u8* receive_address;        //ָ�����½�����ʼ����ָ��
    u16 wait_deal_number;       //�ȴ�������ֽ���������ָʾ�Ƿ񸲸Ƕ�д��
    u8 deal_state;              //�Ƿ��и��Ƕ�д
    u16 size;                   //doing: ��ǰ��������С
                                //note: �������ɶ�̬����Ļ�����
}_receive_buffer_info;  //doing: ���仺������Ϣ���Ƿ�׼���������������������ͷֿ�ĵ�ַ���Լ�����������ָ���λ��

//ѭ����������β�����÷��������������Զ��ϰ�����
typedef struct __send_buffer_info
{
    u8* start_address;          //doing: ��ǰ�������׵�ַ
    u8* end_address;            //note: ������������ַ
    u8* write_address;          //д��ָ��
    u8* send_address;           //���ڷ��͵�������ͷ��ַ
    u8* real_end;               //������β�ĵ�ַ���ڶϱ������з�������
    u16 wait_send_number;       //�����͵�����������ָʾ�Ƿ�д��̫����
    u16 dead_send_number;       //β��������С
    u16 size;                   //doing: ��ǰ��������С
}_send_buffer_info;             //doing: ���仺������Ϣ���Ƿ�׼���������������������ͷֿ�ĵ�ַ���Լ�����������ָ���λ��

//note: �ײ㴫�䲻��Ҫ������ܣ�ֻҪ���ݲ��Ӧ�ò���ͨ����Ϳ�����


typedef struct __tranfer_hardware
{
    void (*init)(u32 speed);             //Ӳ����ʼ������
    void (*set_speed)(u32 speed);   //�ٶȣ�ÿ�봫���λ��
    void (*open)(void);             //����Ӳ��
    void (*close)(void);            //�ر�Ӳ��
    u32 speed;                      //���������
}_transfer_hardware;                //doing: ����ָ��������ݲ㣬�����ϸ�ֿ�

typedef struct __transfer 
{
    //��Ҫ���õĺ���
    void (*get_error)(void);            //������Ϣ
    void (*send_info)(void);        //����DMA���³���
    void (*receive_info)(void);     //����DMA���³���
    _transfer_hardware hardware;        //����ײ㴫���Ӧ��Ӳ��
    u8* send;                           //��ǰ������Ϣ�������ĵ�ַ
    u16 send_number;                    //��ǰ��������
    u8* receive;                        //������Ϣ�������ĵ�ַ
    u16 receive_number;                 //��ǰ��������
    volatile u32  *wait_send;           //Ӳ���������ַ�����
    u16 error;                          //��������ʶ
}_transfer;

typedef struct __data
{
    u8 id;//����
    u16 error;//�����ʶ
#ifdef TEXT_ON
    u32 sum_send;//�ܷ����ַ�
    u32 sum_receive;//�ܽ����ַ�
    u32 real_deal;//�ܴ����ַ���
    u32 real_order;//�ܴ���ָ����
    u32 invalid_data;//���յ���Ч�ַ�
#endif
    _transfer* transfer;                                    //�ײ��ʼ����Ӳ���ṹ
    _receive_buffer_info receive_buffer;                    //һ��ѭ��������
    _send_buffer_info send_buffer;                          //һ��ѭ��������
    void (*default_function)(u8* pointer,u8 length);        //Ĭ�ϴ����Ӻ������û���Ҫָ��
    void (*order[MAX_ORDER_NUMBER])(u8* pointer,u8 length); //�洢ָ��Ӻ���
    u8 temp_buffer[255+DATA_INFO_LENGTH];                   //��ʱ������
}_data;

extern u8 DATA_OPEN;//ȫ�ֿ���
extern u32 sum_printf;

void data_init(_data* data,u8 id,_transfer* transfer,u8* receive,u16 receive_length,u8* send,u16 send_length,void (*dfun)(u8* p,u8 l));
u8 mysend(_data* data,u8 order,u8* pointer,u8 length,u8 mod);//��ָͨ����������
u8 mydeal(_data* data,u8 order,void (*func)(u8* pointer,u8 length));//���ӹҹ�����
void deal_receive_buffer(_data* data,u16 real_receive);//������������
u8 deal_receive_data(_data* data);//���������ݴ�����
u8 deal_receive_data_2(_data* data);//���������ݴ�����2
void deal_send_buffer(_data* data);//����������
void init_deal_hook(_data* data);//��ʼ�����Ӻ���
void set_printf(u8 order,u8 chose);//printf��ʼ������

#endif

