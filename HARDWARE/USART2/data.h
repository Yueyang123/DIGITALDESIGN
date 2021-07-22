#ifndef __DATA_H
#define __DATA_H

#include "sys.h"
//没有校验、密码、信息核查
//简易数据层处理程序

//下面这宏里面是测试代码
#define TEXT_ON  0

#ifndef NULL
#define NULL (void*)(0)
#endif
//done: 定义接收和发送的缓存区的数量和长度，这些一起确定总的缓冲区大小。
#define SEND_BUFFER_LENGTH          1024*2  //单个缓冲区大小
#define RECEIVE_BUFFER_LENGTH       1024*3  //
//note: 上面是默认缓冲区大小，实际使用时可以自己指定大小
#define MAX_ORDER_NUMBER            255     //最大指令数量
//note: 这个控制指令数量，较小可节约空间
#define DEFAULT_SPEED               921600  //默认传输波特率
//note: 未指定波特率的情况下，底层传输按这个波特率初始化
#define SEND_MIN                    5       //最短指令长度5字节
#define RECEIVE_MAX                 255     //最大的单次接收缓存
#define PRINTF_MIN                  10      //单次最小发送数量
//note: 这些是确定单次接收和发送的字符数量
#define DATA_INFO_LENGTH 4 //帧非数据部分大小
//note: 这个用于指定帧的标识部分，一般情况下默认即可


//这些都是状态宏函数
#define SEND_IMMEDIATELY    111 //立即发送
#define SEND_LATE           112 //稍后发送
#define PRINTF_HAVE         113 //存在待发送的字符
#define PRINTF_NONE         114 //不存在待发送字符
#define DATA_HALF           115 //数据包不完整
#define DEAL_FUNC_RW        116 //钩子函数覆写
#define DEAL_FUNC_W         117 //钩子函数正常写入

//下面是错误标识，用于记录错误，并反馈
#define ERROR_SEND_LATE          (1<<1)  //发送延时错误标识
#define ERROR_SEND_FAIL          (1<<2)  //发送失败错误标识
#define ERROR_RECEIVE_FAIL       (1<<1)  //接收失败错误标识
#define ERROR_RECEIVE_OVER       (1<<2)  //接收覆盖错误标识
#define ERROR_RECEIVE_KEY        (1<<3)  //接收密码错误标识
#define ERROR_DEAL_LATE          (1<<4)  //处理太慢错误标识

typedef struct __data_start //投影信息结构体，快速处理指令信息
{
    u8 data_head;       //帧头起始字符。默认@
    u8 order;           //指令代号
    u8 size;            //数据的大小,一次数据最大255
    u8 data[];          //数据区头地址，不定长数组
}_data_start;           //用于管理信息结构
//printf

//循环接收区，每次写完，截断后无需复杂处理，生成简单信息头就可以了
typedef struct __receive_buffer_info
{
    u8* start_address;          //doing: 当前缓冲区首地址
    u8* end_address;            //尾地址
    u8* real_end;               //真正结束的尾地址
    u8* deal_address;           //信息处理指针。指向已处理的最新地址
    u8* receive_address;        //指向最新接收起始区的指针
    u16 wait_deal_number;       //等待处理的字节数，用来指示是否覆盖读写了
    u8 deal_state;              //是否有覆盖读写
    u16 size;                   //doing: 当前缓冲区大小
                                //note: 可以做成动态分配的缓冲区
}_receive_buffer_info;  //doing: 传输缓冲区信息，是否准备就绪，缓冲区的数量和分块的地址，以及缓冲区数据指针的位置

//循环发送区，尾部设置发射死区，具有自动断包功能
typedef struct __send_buffer_info
{
    u8* start_address;          //doing: 当前缓冲区首地址
    u8* end_address;            //note: 缓冲区结束地址
    u8* write_address;          //写入指针
    u8* send_address;           //正在发送的数据区头地址
    u8* real_end;               //真正结尾的地址，在断保功能中发挥作用
    u16 wait_send_number;       //待发送的数量，用来指示是否写入太多了
    u16 dead_send_number;       //尾部死区大小
    u16 size;                   //doing: 当前缓冲区大小
}_send_buffer_info;             //doing: 传输缓冲区信息，是否准备就绪，缓冲区的数量和分块的地址，以及缓冲区数据指针的位置

//note: 底层传输不需要密码加密，只要数据层和应用层检查通过后就可以了


typedef struct __tranfer_hardware
{
    void (*init)(u32 speed);             //硬件初始化函数
    void (*set_speed)(u32 speed);   //速度，每秒传输的位数
    void (*open)(void);             //开启硬件
    void (*close)(void);            //关闭硬件
    u32 speed;                      //传输的速率
}_transfer_hardware;                //doing: 这里指令不用于数据层，必须严格分开

typedef struct __transfer 
{
    //需要调用的函数
    void (*get_error)(void);            //错误信息
    void (*send_info)(void);        //发送DMA更新程序
    void (*receive_info)(void);     //接收DMA更新程序
    _transfer_hardware hardware;        //当层底层传输对应的硬件
    u8* send;                           //当前发送信息缓冲区的地址
    u16 send_number;                    //当前发送数量
    u8* receive;                        //接收信息缓冲区的地址
    u16 receive_number;                 //当前接收数量
    volatile u32  *wait_send;           //硬件待发送字符数量
    u16 error;                          //传输错误标识
}_transfer;

typedef struct __data
{
    u8 id;//代号
    u16 error;//错误标识
#ifdef TEXT_ON
    u32 sum_send;//总发送字符
    u32 sum_receive;//总接收字符
    u32 real_deal;//总处理字符数
    u32 real_order;//总处理指令数
    u32 invalid_data;//接收的无效字符
#endif
    _transfer* transfer;                                    //底层初始化的硬件结构
    _receive_buffer_info receive_buffer;                    //一个循环缓冲区
    _send_buffer_info send_buffer;                          //一个循环发送区
    void (*default_function)(u8* pointer,u8 length);        //默认处理钩子函数，用户需要指定
    void (*order[MAX_ORDER_NUMBER])(u8* pointer,u8 length); //存储指令钩子函数
    u8 temp_buffer[255+DATA_INFO_LENGTH];                   //临时缓存区
}_data;

extern u8 DATA_OPEN;//全局开关
extern u32 sum_printf;

void data_init(_data* data,u8 id,_transfer* transfer,u8* receive,u16 receive_length,u8* send,u16 send_length,void (*dfun)(u8* p,u8 l));
u8 mysend(_data* data,u8 order,u8* pointer,u8 length,u8 mod);//普通指令包传输程序
u8 mydeal(_data* data,u8 order,void (*func)(u8* pointer,u8 length));//钩子挂钩函数
void deal_receive_buffer(_data* data,u16 real_receive);//接收区处理函数
u8 deal_receive_data(_data* data);//接收区数据处理函数
u8 deal_receive_data_2(_data* data);//接收区数据处理函数2
void deal_send_buffer(_data* data);//发送区处理
void init_deal_hook(_data* data);//初始化钩子函数
void set_printf(u8 order,u8 chose);//printf初始化函数

#endif

