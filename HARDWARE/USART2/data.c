#include "usart3.h"
#include "data.h"
#include "stdio.h"

u8 DATA_OPEN=0; //全局开关
//给printf函数来一个自己的缓冲区
u8  printf_buffer[2][1024]={0};
u32 sum_printf=0;
u8* printf_data=printf_buffer[0]+3;//数据指针
u8  printf_port_chose=0;//标准输出端口
//加入以下代码,支持printf函数,而不需要选择use MicroLIB	  
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 
	/* Whatever you require here. If the only file you are using is */ 
	/* standard output using printf() for debugging, no file handling */ 
	/* is required. */ 
}; 
/* FILE is typedef’ d in stdio.h. */ 
FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//重定义fputc函数 
int fputc(int ch, FILE *f)
{      
    *printf_data++=ch;//这是最致命的bug,享受写爆内存的快感吧！！！
		return ch;
}

//设置printf函数的参数
void set_printf(u8 order,u8 chose)
{
    printf_buffer[0][0]='@';
    printf_buffer[0][1]=order;
    printf_buffer[0][2]=0;
    printf_buffer[1][0]='@';
    printf_buffer[1][1]=order;
    printf_buffer[1][2]=0;
    printf_port_chose=chose;
}

//专门处理printf函数的缓冲
u8 deal_printf_buffer(_data* data)
{
    volatile static u8 bc=0;
    if(data->id==printf_port_chose)
    {
        if(printf_data>printf_buffer[bc]+3)//存在待发送的字符，就发送
        {
			#ifndef TEXT_ON
            *printf_data++='$';//加入结束符
			data->transfer->send_number=printf_data-printf_buffer[bc];//打印字符数量
			#endif
			#ifdef TEXT_ON
            data->transfer->send_number=printf_data-printf_buffer[bc]-3;//打印字符数量
			#endif
            printf_buffer[bc][2]=data->transfer->send_number-4;//写入发送的数据数量
            #ifdef TEXT_ON
            sum_printf+=data->transfer->send_number;
            data->transfer->send=printf_buffer[bc]+3;//打印起始地址
			#endif
			#ifndef TEXT_ON
			data->transfer->send=printf_buffer[bc];//打印起始地址
			#endif
            bc=(bc+1)%2;
            printf_data=printf_buffer[bc]+3;//帧头已经写入了
            
            return PRINTF_HAVE;
        }
    }
    return PRINTF_NONE;
}


//普通模式单次最大发送255字节
u8 mysend(_data* data,u8 order,u8* pointer,u8 length,u8 mod)
{
    u16 sum_data=length+DATA_INFO_LENGTH;//记录发送数据总长度
    _data_start* data_start=NULL;//生成帧数据结构
    u8* wdata=NULL;
    u8 i=0;//数据计数
    if(data->send_buffer.wait_send_number+sum_data<=data->send_buffer.size-data->send_buffer.dead_send_number)//是否覆写情况
    {
        if(data->send_buffer.end_address-wdata+1<sum_data)//尾部无法完整写入一个数据指令，直接跳转到头部
        {
            data->send_buffer.dead_send_number=data->send_buffer.end_address-wdata+1;//生成新的尾部死区
            data->send_buffer.write_address=data->send_buffer.start_address;//获取头部地址
        }
        
        data_start=(_data_start*)(data->send_buffer.write_address);//生成数据包结构
        data->send_buffer.write_address+=sum_data;//写入指针移向最新地方

        //写入数据
        data_start->data_head='@';
        data_start->order=order;
        data_start->size=length;
        wdata=data_start->data;//获取数据写入指针
        for(i=0;i<length;i++)
            *wdata++=*pointer++;//写入数据
        *wdata++='$';
        data->send_buffer.wait_send_number+=sum_data;//待发送字符增加
    }
    if(mod==SEND_IMMEDIATELY&&*(data->transfer->wait_send)==0)//需要处于未发送状态
    {
        deal_send_buffer(data);//调用发送处理函数
    }
	return 1;
}
//接收处理函数，采用钩子函数，注册指令和函数即可，收到对应指令会自动处理。
//面对中断处理的特殊情况，具体使用时应该不要在钩子函数中对正在的使用的值写入
u8 mydeal(_data* data,u8 order,void (*func)(u8* pointer,u8 length))
{
    //一个指令只能对应一个钩子函数
    if(data->order[order]==data->default_function)//无覆写填充
    {
        data->order[order]=func;//钩子函数勾上
        return DEAL_FUNC_W;
    }
    else
    {
        data->order[order]=func;//钩子函数重新写入
        return DEAL_FUNC_RW;
    }
}
void init_deal_hook(_data* data)
{
    int i=0;
    for(i=0;i<MAX_ORDER_NUMBER;i++)
        data->order[i]=data->default_function;//该函数由指定硬件决定
}
//用户需要指定实例化的数据信息结构体，传输底层，接收发送缓冲区以及长度，默认钩子函数等
void data_init(_data* data,u8 id,_transfer* transfer,u8* receive,u16 receive_length,u8* send,u16 send_length,void (*dfun)(u8* p,u8 l))
{
	data->id=id;
	data->error=0;
	#ifdef TEXT_ON
	data->sum_send=0;
	data->sum_receive=0;
	data->real_deal=0;
	data->real_order=0;
	data->invalid_data=0;
	#endif
	data->transfer=transfer;

	data->receive_buffer.start_address=receive;
	data->receive_buffer.end_address=receive+receive_length;
	data->receive_buffer.real_end=data->receive_buffer.end_address;
	data->receive_buffer.deal_address=data->receive_buffer.start_address;
	data->receive_buffer.receive_address=data->receive_buffer.start_address;
	data->receive_buffer.wait_deal_number=0;
	data->receive_buffer.deal_state=0;
	data->receive_buffer.size=receive_length;

	data->send_buffer.start_address=send;
	data->send_buffer.end_address=send+send_length;
	data->send_buffer.write_address=data->send_buffer.start_address;
	data->send_buffer.send_address=data->send_buffer.start_address;
	data->send_buffer.real_end=data->send_buffer.end_address;
	data->send_buffer.wait_send_number=0;
	data->send_buffer.dead_send_number=0;
	data->send_buffer.size=send_length;

	data->default_function=dfun;//默认函数勾上

	init_deal_hook(data);
	//全部勾上默认函数
	data->transfer->receive_info();//硬件全部初始化完成，开启接收，接收是一直等待的
}


//生成新的发送指针用于发送,有必要的情况下继续发送
//进入该函数时必须确保DMA发送未启用
void deal_send_buffer(_data* data)
{
  if(*(data->transfer->wait_send)==0)//发送空闲时才启动发送
  {
    if(deal_printf_buffer(data)==PRINTF_NONE)//总是先发送printf的信息
    {
        if(data->send_buffer.wait_send_number>=SEND_MIN)//待发送信息不小于最小指令长度就可以发送
        {
            
            data->transfer->send=data->send_buffer.send_address;//传递发送起始区的指针
            if(data->send_buffer.wait_send_number+data->send_buffer.send_address>data->send_buffer.real_end+1)//判断是否尾头切换
            {
                //尾头切换了，分多次发送
                data->transfer->send_number=data->send_buffer.real_end-data->send_buffer.send_address+1;//设置发送的字节数
                #ifdef TEXT_ON
                data->sum_send+=data->transfer->send_number;//累计总发射数量
                #endif
                data->send_buffer.wait_send_number-= data->transfer->send_number;//待发送数量减少相应大小
                data->send_buffer.send_address=data->send_buffer.start_address;//以上最新的发送首地址
            }
            else
            {
                data->transfer->send_number=data->send_buffer.wait_send_number;//一次发送所有的字节
                #ifdef TEXT_ON
                data->sum_send+=data->transfer->send_number;//累计总发射数量
                #endif
                data->send_buffer.wait_send_number=0;//发送完全
                data->send_buffer.send_address+=data->transfer->send_number;//移向最新的发送首地址
            }
            data->transfer->send_info();//调用发送硬件处理函数，开启发送

        }
				else
				{
					 //这种情况下无字符需要发送，不调用硬件函数
				}						
    }
		else// if(*(data->transfer->wait_send)==0) //存在printf字符待发送
		{
				data->transfer->send_info();//调用发送硬件处理函数，开启发送
		}
  }
}

void deal_receive_buffer(_data* data,u16 real_receive)
{
    data->receive_buffer.receive_address+=real_receive;//将接收指针移向最新位置
    data->receive_buffer.wait_deal_number+=real_receive;//计算最新的待处理字符
    #ifdef TEXT_ON
    data->sum_receive+=real_receive;//总接收字符总数
    #endif
    if(data->receive_buffer.end_address-data->receive_buffer.receive_address<RECEIVE_MAX)//剩余可接受字符少于最大接收字符
    {
        data->receive_buffer.end_address= data->receive_buffer.receive_address-1;//记录实际结束的接收缓冲区地址
        data->receive_buffer.receive_address=data->receive_buffer.start_address;//重定向到缓冲区起始位置
    }
    else
    {
        //无需任何处理，继续接着上面的地址生成缓冲区
    }
    #ifdef TEXT_ON
    if(data->receive_buffer.wait_deal_number>data->receive_buffer.size)//发生了覆写情况
    {
        data->error|=ERROR_RECEIVE_OVER;//覆写错误标识
        printf("\n接收区覆写！\n");//发送消息提醒
    }
    #endif
    data->transfer->receive= data->receive_buffer.receive_address;
    data->transfer->receive_number=RECEIVE_MAX;//单次最大接收数量，这是默认情况的最大接收数量
    data->transfer->receive_info();//调用接收硬件处理函数，开启新一轮的接收
}
//生成新的接收区用于接收，并立即启动对刚刚接收数据的处理
//接收区尾部有死区，即尾部不会发生数据区截断情况
u8 deal_receive_data(_data* data)
{
    //判断指令是否完整
    u8* rdata=data->receive_buffer.deal_address;//获取处理的地址
    u8* tdata=data->temp_buffer;//获取临时缓冲区的地址
    u16 count=0,i=0;//处理计数标识
    u16 sum_data=0,start_offset=0,left_data=0;//总指令长度和截断指令起始偏移地址,尾部截断的数据量
    u16 max_length=data->receive_buffer.wait_deal_number;//获取待处理字符数量
    _data_start* data_start=NULL;//数据包帧结构
    while(count<max_length)
    {
        if(rdata[count]=='@')//检测到帧头
        {
            data_start=(_data_start*)(rdata+count);//当前地址直接生成帧结构
            sum_data=data_start->size+DATA_INFO_LENGTH;//获取指令总长度
            if(sum_data<=max_length-count)//已接收完成
            {
                left_data=data->receive_buffer.real_end-rdata-count+1;
                if(sum_data<=left_data)//是否数据被截断
                {    
                    if(data_start->data[data_start->size]=='$')//这个地址存储的是帧尾
                    {
                        //这里是正确的数据包
                        (*(data->order[data_start->order]))(data_start->data,data_start->size);//针对指令直接调用钩子函数进行处理。
                        //用户处理完后
                        count+=sum_data;
                        #ifdef TEXT_ON
                        data->real_deal+=sum_data;//累计真实处理字符
                        data->real_order+=1;//累积指令处理情况
                        #endif
                    }
                    else
                    {
                        count++;//这不是数据包，但是全程是连续处理的，所以直接舍弃这段数据
                        //舍弃这个数据，该帧头可能并非真的帧头
                        #ifdef TEXT_ON
                        data->invalid_data++;
                        #endif
                    }
                }
                else 
                {
                    //数据被截断,但接受完成，先判断是否有效
                    //剩下的数据从头开始接收，所以需要算出新的结束符地址
                    start_offset=sum_data-(left_data)-1;//算出起始偏移量
                    if(data->receive_buffer.start_address[start_offset]=='$')//是否具有结束符
                    //总指令长度减去结尾处的指令长度，是从头开始接收的长度，减一则是其地址偏移量
                    {
                        //正确数据包
                        for(i=0;i<left_data;i++)
                            *tdata++=rdata[count+i];//尾部数据重新写入临时缓冲区
                        rdata=data->receive_buffer.start_address;//读取数据重定向地址
                        for(i=0;i<=start_offset;i++)
                            *tdata++=rdata[i];//头部数据重写入临时缓冲区
                        //截断数据在临时缓冲区被重建
                        data_start=(_data_start*)(&(data->temp_buffer));//重定向信息头指针，指向临时缓冲区
                        (*(data->order[data_start->order]))(data_start->data,data_start->size);//针对指令直接调用钩子函数进行处理。
                        //跨截断区处理
                        max_length-=(count+sum_data);//得到最新剩下的长度
                        #ifdef TEXT_ON
                        data->real_deal+=sum_data;//累计真实处理字符
                        data->real_order+=1;//累积指令处理情况
                        #endif
                        rdata=data->receive_buffer.start_address+start_offset+1;//算出最新的处理地址
                        count=0;//计数量归0
                        tdata=(data->temp_buffer);//重新归位临时缓冲区
                    }
                    else
                    {
                        count++;//非完整数据包，跳过该包
                        #ifdef TEXT_ON
                        data->invalid_data++;
                        #endif
                    }
                }
            }
            else
            {
                //未接收完成，放弃处理，等待接受完成再处理
                //此时收到了帧头，但没有完全的数据
                //停止处理，从新生成处理指针和待处理数据量
                data->receive_buffer.deal_address=rdata+count;//保存当前的处理地址
                data->receive_buffer.wait_deal_number=max_length-count;//待处理字符
                return DATA_HALF;
            }
        }
        else
        {
            //非帧头，错误数据，丢弃处理
            //这是收到错误数据了需要跳过
            count++;
            #ifdef TEXT_ON
            data->invalid_data++;
            #endif

        }
    }
    //完整处理完了，这是除了有截断情况下的唯一结果，该过程中可能会丢失大量无用数据
    //记录处理情况
    data->receive_buffer.deal_address=rdata+count;
    data->receive_buffer.wait_deal_number=max_length-count;
    return 0;
}

//另一种处理函数,所有数据都会复制到临时缓冲区中处理
u8 deal_receive_data_2(_data* data)
{
    u8* rdata=data->receive_buffer.deal_address;//获取当前处理地址
    u8* tdata=(data->temp_buffer);//临时缓存区
    u16 max_length=data->receive_buffer.wait_deal_number;//获取待处理字符数量
    u16 count=0,sum_data=0,left_data=0,i=0,start_offset=0;//计数量
    _data_start* data_start=NULL;//生成信息头
    while(count<max_length)
    {
        if(rdata[count]=='@')
        {
            data_start=(_data_start*)(rdata+count);//当前地址直接生成帧结构
            sum_data=data_start->size+DATA_INFO_LENGTH;//获取指令总长度
            if(sum_data<=max_length-count)//已接收完成
            {
                left_data=data->receive_buffer.real_end-rdata-count+1;
                if(sum_data<=left_data)//是否数据被截断
                {    
                    if(data_start->data[data_start->size]=='$')//这个地址存储的是帧尾
                    {
                        //正确数据包
                        //这里有一个bug，那就sum_data可能会大于255，
                        for(i=0;i<sum_data;i++)
                            tdata[i]=rdata[count++];//复制到临时缓冲区中
                        data_start=(_data_start*)(tdata);//重定向数据信息
                        (*(data->order[data_start->order]))(data_start->data,data_start->size);//针对指令直接调用钩子函数进行处理。

                    }
                    else
                    {
                        //跳过该错误字符
                        count++;
                    }
				}
                else
                {
                    start_offset=sum_data-(left_data)-1;//算出起始偏移量
                    if(data->receive_buffer.start_address[start_offset]=='$')//是否具有结束符
                    {
                        //正确数据包
                        //注意上面提到的bug
                         //正确数据包
                        for(i=0;i<left_data;i++)
                            tdata[i]=rdata[count+i];//尾部数据重新写入临时缓冲区
                        rdata=data->receive_buffer.start_address;//读取数据重定向地址
                        for(i=0;i<=start_offset;i++)
                            tdata[left_data+i]=rdata[i];//头部数据重写入临时缓冲区
                        //截断数据在临时缓冲区被重建
                        data_start=(_data_start*)(tdata);//重定向信息头指针，指向临时缓冲区
                        (*(data->order[data_start->order]))(data_start->data,data_start->size);//针对指令直接调用钩子函数进行处理。
                        //跨截断区处理
                        max_length-=(count+sum_data);//得到最新剩下的长度
                        rdata=data->receive_buffer.start_address+start_offset+1;//算出最新的处理地址
                        count=0;//计数量归0
                    }
                    else
                    {
                        //跳过该错误字符
                        count++;
                    }
                }
            }
            else
            {
                //数据未接受完，退出处理程序
                data->receive_buffer.deal_address=rdata+count;//保存当前的处理地址
                data->receive_buffer.wait_deal_number=max_length-count;//待处理字符
                return DATA_HALF;
            }
        }
        else
        {
            //非有效字符,跳过
            count++;
        }
    }
    //完整处理完了，这是除了有截断情况下的唯一结果，该过程中可能会丢失大量无用数据
    //记录处理情况
    data->receive_buffer.deal_address=rdata+count;
    data->receive_buffer.wait_deal_number=max_length-count;
    return 0;
}

