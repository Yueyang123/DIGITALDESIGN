#include "usart3.h"
#include "data.h"
#include "stdio.h"

u8 DATA_OPEN=0; //ȫ�ֿ���
//��printf������һ���Լ��Ļ�����
u8  printf_buffer[2][1024]={0};
u32 sum_printf=0;
u8* printf_data=printf_buffer[0]+3;//����ָ��
u8  printf_port_chose=0;//��׼����˿�
//�������´���,֧��printf����,������Ҫѡ��use MicroLIB	  
#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
	int handle; 
	/* Whatever you require here. If the only file you are using is */ 
	/* standard output using printf() for debugging, no file handling */ 
	/* is required. */ 
}; 
/* FILE is typedef�� d in stdio.h. */ 
FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//�ض���fputc���� 
int fputc(int ch, FILE *f)
{      
    *printf_data++=ch;//������������bug,����д���ڴ�Ŀ�аɣ�����
		return ch;
}

//����printf�����Ĳ���
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

//ר�Ŵ���printf�����Ļ���
u8 deal_printf_buffer(_data* data)
{
    volatile static u8 bc=0;
    if(data->id==printf_port_chose)
    {
        if(printf_data>printf_buffer[bc]+3)//���ڴ����͵��ַ����ͷ���
        {
			#ifndef TEXT_ON
            *printf_data++='$';//���������
			data->transfer->send_number=printf_data-printf_buffer[bc];//��ӡ�ַ�����
			#endif
			#ifdef TEXT_ON
            data->transfer->send_number=printf_data-printf_buffer[bc]-3;//��ӡ�ַ�����
			#endif
            printf_buffer[bc][2]=data->transfer->send_number-4;//д�뷢�͵���������
            #ifdef TEXT_ON
            sum_printf+=data->transfer->send_number;
            data->transfer->send=printf_buffer[bc]+3;//��ӡ��ʼ��ַ
			#endif
			#ifndef TEXT_ON
			data->transfer->send=printf_buffer[bc];//��ӡ��ʼ��ַ
			#endif
            bc=(bc+1)%2;
            printf_data=printf_buffer[bc]+3;//֡ͷ�Ѿ�д����
            
            return PRINTF_HAVE;
        }
    }
    return PRINTF_NONE;
}


//��ͨģʽ���������255�ֽ�
u8 mysend(_data* data,u8 order,u8* pointer,u8 length,u8 mod)
{
    u16 sum_data=length+DATA_INFO_LENGTH;//��¼���������ܳ���
    _data_start* data_start=NULL;//����֡���ݽṹ
    u8* wdata=NULL;
    u8 i=0;//���ݼ���
    if(data->send_buffer.wait_send_number+sum_data<=data->send_buffer.size-data->send_buffer.dead_send_number)//�Ƿ�д���
    {
        if(data->send_buffer.end_address-wdata+1<sum_data)//β���޷�����д��һ������ָ�ֱ����ת��ͷ��
        {
            data->send_buffer.dead_send_number=data->send_buffer.end_address-wdata+1;//�����µ�β������
            data->send_buffer.write_address=data->send_buffer.start_address;//��ȡͷ����ַ
        }
        
        data_start=(_data_start*)(data->send_buffer.write_address);//�������ݰ��ṹ
        data->send_buffer.write_address+=sum_data;//д��ָ���������µط�

        //д������
        data_start->data_head='@';
        data_start->order=order;
        data_start->size=length;
        wdata=data_start->data;//��ȡ����д��ָ��
        for(i=0;i<length;i++)
            *wdata++=*pointer++;//д������
        *wdata++='$';
        data->send_buffer.wait_send_number+=sum_data;//�������ַ�����
    }
    if(mod==SEND_IMMEDIATELY&&*(data->transfer->wait_send)==0)//��Ҫ����δ����״̬
    {
        deal_send_buffer(data);//���÷��ʹ�����
    }
	return 1;
}
//���մ����������ù��Ӻ�����ע��ָ��ͺ������ɣ��յ���Ӧָ����Զ�����
//����жϴ�����������������ʹ��ʱӦ�ò�Ҫ�ڹ��Ӻ����ж����ڵ�ʹ�õ�ֵд��
u8 mydeal(_data* data,u8 order,void (*func)(u8* pointer,u8 length))
{
    //һ��ָ��ֻ�ܶ�Ӧһ�����Ӻ���
    if(data->order[order]==data->default_function)//�޸�д���
    {
        data->order[order]=func;//���Ӻ�������
        return DEAL_FUNC_W;
    }
    else
    {
        data->order[order]=func;//���Ӻ�������д��
        return DEAL_FUNC_RW;
    }
}
void init_deal_hook(_data* data)
{
    int i=0;
    for(i=0;i<MAX_ORDER_NUMBER;i++)
        data->order[i]=data->default_function;//�ú�����ָ��Ӳ������
}
//�û���Ҫָ��ʵ������������Ϣ�ṹ�壬����ײ㣬���շ��ͻ������Լ����ȣ�Ĭ�Ϲ��Ӻ�����
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

	data->default_function=dfun;//Ĭ�Ϻ�������

	init_deal_hook(data);
	//ȫ������Ĭ�Ϻ���
	data->transfer->receive_info();//Ӳ��ȫ����ʼ����ɣ��������գ�������һֱ�ȴ���
}


//�����µķ���ָ�����ڷ���,�б�Ҫ������¼�������
//����ú���ʱ����ȷ��DMA����δ����
void deal_send_buffer(_data* data)
{
  if(*(data->transfer->wait_send)==0)//���Ϳ���ʱ����������
  {
    if(deal_printf_buffer(data)==PRINTF_NONE)//�����ȷ���printf����Ϣ
    {
        if(data->send_buffer.wait_send_number>=SEND_MIN)//��������Ϣ��С����Сָ��ȾͿ��Է���
        {
            
            data->transfer->send=data->send_buffer.send_address;//���ݷ�����ʼ����ָ��
            if(data->send_buffer.wait_send_number+data->send_buffer.send_address>data->send_buffer.real_end+1)//�ж��Ƿ�βͷ�л�
            {
                //βͷ�л��ˣ��ֶ�η���
                data->transfer->send_number=data->send_buffer.real_end-data->send_buffer.send_address+1;//���÷��͵��ֽ���
                #ifdef TEXT_ON
                data->sum_send+=data->transfer->send_number;//�ۼ��ܷ�������
                #endif
                data->send_buffer.wait_send_number-= data->transfer->send_number;//����������������Ӧ��С
                data->send_buffer.send_address=data->send_buffer.start_address;//�������µķ����׵�ַ
            }
            else
            {
                data->transfer->send_number=data->send_buffer.wait_send_number;//һ�η������е��ֽ�
                #ifdef TEXT_ON
                data->sum_send+=data->transfer->send_number;//�ۼ��ܷ�������
                #endif
                data->send_buffer.wait_send_number=0;//������ȫ
                data->send_buffer.send_address+=data->transfer->send_number;//�������µķ����׵�ַ
            }
            data->transfer->send_info();//���÷���Ӳ������������������

        }
				else
				{
					 //������������ַ���Ҫ���ͣ�������Ӳ������
				}						
    }
		else// if(*(data->transfer->wait_send)==0) //����printf�ַ�������
		{
				data->transfer->send_info();//���÷���Ӳ������������������
		}
  }
}

void deal_receive_buffer(_data* data,u16 real_receive)
{
    data->receive_buffer.receive_address+=real_receive;//������ָ����������λ��
    data->receive_buffer.wait_deal_number+=real_receive;//�������µĴ������ַ�
    #ifdef TEXT_ON
    data->sum_receive+=real_receive;//�ܽ����ַ�����
    #endif
    if(data->receive_buffer.end_address-data->receive_buffer.receive_address<RECEIVE_MAX)//ʣ��ɽ����ַ������������ַ�
    {
        data->receive_buffer.end_address= data->receive_buffer.receive_address-1;//��¼ʵ�ʽ����Ľ��ջ�������ַ
        data->receive_buffer.receive_address=data->receive_buffer.start_address;//�ض��򵽻�������ʼλ��
    }
    else
    {
        //�����κδ���������������ĵ�ַ���ɻ�����
    }
    #ifdef TEXT_ON
    if(data->receive_buffer.wait_deal_number>data->receive_buffer.size)//�����˸�д���
    {
        data->error|=ERROR_RECEIVE_OVER;//��д�����ʶ
        printf("\n��������д��\n");//������Ϣ����
    }
    #endif
    data->transfer->receive= data->receive_buffer.receive_address;
    data->transfer->receive_number=RECEIVE_MAX;//��������������������Ĭ�����������������
    data->transfer->receive_info();//���ý���Ӳ����������������һ�ֵĽ���
}
//�����µĽ��������ڽ��գ������������Ըոս������ݵĴ���
//������β������������β�����ᷢ���������ض����
u8 deal_receive_data(_data* data)
{
    //�ж�ָ���Ƿ�����
    u8* rdata=data->receive_buffer.deal_address;//��ȡ����ĵ�ַ
    u8* tdata=data->temp_buffer;//��ȡ��ʱ�������ĵ�ַ
    u16 count=0,i=0;//���������ʶ
    u16 sum_data=0,start_offset=0,left_data=0;//��ָ��Ⱥͽض�ָ����ʼƫ�Ƶ�ַ,β���ضϵ�������
    u16 max_length=data->receive_buffer.wait_deal_number;//��ȡ�������ַ�����
    _data_start* data_start=NULL;//���ݰ�֡�ṹ
    while(count<max_length)
    {
        if(rdata[count]=='@')//��⵽֡ͷ
        {
            data_start=(_data_start*)(rdata+count);//��ǰ��ֱַ������֡�ṹ
            sum_data=data_start->size+DATA_INFO_LENGTH;//��ȡָ���ܳ���
            if(sum_data<=max_length-count)//�ѽ������
            {
                left_data=data->receive_buffer.real_end-rdata-count+1;
                if(sum_data<=left_data)//�Ƿ����ݱ��ض�
                {    
                    if(data_start->data[data_start->size]=='$')//�����ַ�洢����֡β
                    {
                        //��������ȷ�����ݰ�
                        (*(data->order[data_start->order]))(data_start->data,data_start->size);//���ָ��ֱ�ӵ��ù��Ӻ������д���
                        //�û��������
                        count+=sum_data;
                        #ifdef TEXT_ON
                        data->real_deal+=sum_data;//�ۼ���ʵ�����ַ�
                        data->real_order+=1;//�ۻ�ָ������
                        #endif
                    }
                    else
                    {
                        count++;//�ⲻ�����ݰ�������ȫ������������ģ�����ֱ�������������
                        //����������ݣ���֡ͷ���ܲ������֡ͷ
                        #ifdef TEXT_ON
                        data->invalid_data++;
                        #endif
                    }
                }
                else 
                {
                    //���ݱ��ض�,��������ɣ����ж��Ƿ���Ч
                    //ʣ�µ����ݴ�ͷ��ʼ���գ�������Ҫ����µĽ�������ַ
                    start_offset=sum_data-(left_data)-1;//�����ʼƫ����
                    if(data->receive_buffer.start_address[start_offset]=='$')//�Ƿ���н�����
                    //��ָ��ȼ�ȥ��β����ָ��ȣ��Ǵ�ͷ��ʼ���յĳ��ȣ���һ�������ַƫ����
                    {
                        //��ȷ���ݰ�
                        for(i=0;i<left_data;i++)
                            *tdata++=rdata[count+i];//β����������д����ʱ������
                        rdata=data->receive_buffer.start_address;//��ȡ�����ض����ַ
                        for(i=0;i<=start_offset;i++)
                            *tdata++=rdata[i];//ͷ��������д����ʱ������
                        //�ض���������ʱ���������ؽ�
                        data_start=(_data_start*)(&(data->temp_buffer));//�ض�����Ϣͷָ�룬ָ����ʱ������
                        (*(data->order[data_start->order]))(data_start->data,data_start->size);//���ָ��ֱ�ӵ��ù��Ӻ������д���
                        //��ض�������
                        max_length-=(count+sum_data);//�õ�����ʣ�µĳ���
                        #ifdef TEXT_ON
                        data->real_deal+=sum_data;//�ۼ���ʵ�����ַ�
                        data->real_order+=1;//�ۻ�ָ������
                        #endif
                        rdata=data->receive_buffer.start_address+start_offset+1;//������µĴ����ַ
                        count=0;//��������0
                        tdata=(data->temp_buffer);//���¹�λ��ʱ������
                    }
                    else
                    {
                        count++;//���������ݰ��������ð�
                        #ifdef TEXT_ON
                        data->invalid_data++;
                        #endif
                    }
                }
            }
            else
            {
                //δ������ɣ����������ȴ���������ٴ���
                //��ʱ�յ���֡ͷ����û����ȫ������
                //ֹͣ�����������ɴ���ָ��ʹ�����������
                data->receive_buffer.deal_address=rdata+count;//���浱ǰ�Ĵ����ַ
                data->receive_buffer.wait_deal_number=max_length-count;//�������ַ�
                return DATA_HALF;
            }
        }
        else
        {
            //��֡ͷ���������ݣ���������
            //�����յ�������������Ҫ����
            count++;
            #ifdef TEXT_ON
            data->invalid_data++;
            #endif

        }
    }
    //�����������ˣ����ǳ����нض�����µ�Ψһ������ù����п��ܻᶪʧ������������
    //��¼�������
    data->receive_buffer.deal_address=rdata+count;
    data->receive_buffer.wait_deal_number=max_length-count;
    return 0;
}

//��һ�ִ�����,�������ݶ��Ḵ�Ƶ���ʱ�������д���
u8 deal_receive_data_2(_data* data)
{
    u8* rdata=data->receive_buffer.deal_address;//��ȡ��ǰ�����ַ
    u8* tdata=(data->temp_buffer);//��ʱ������
    u16 max_length=data->receive_buffer.wait_deal_number;//��ȡ�������ַ�����
    u16 count=0,sum_data=0,left_data=0,i=0,start_offset=0;//������
    _data_start* data_start=NULL;//������Ϣͷ
    while(count<max_length)
    {
        if(rdata[count]=='@')
        {
            data_start=(_data_start*)(rdata+count);//��ǰ��ֱַ������֡�ṹ
            sum_data=data_start->size+DATA_INFO_LENGTH;//��ȡָ���ܳ���
            if(sum_data<=max_length-count)//�ѽ������
            {
                left_data=data->receive_buffer.real_end-rdata-count+1;
                if(sum_data<=left_data)//�Ƿ����ݱ��ض�
                {    
                    if(data_start->data[data_start->size]=='$')//�����ַ�洢����֡β
                    {
                        //��ȷ���ݰ�
                        //������һ��bug���Ǿ�sum_data���ܻ����255��
                        for(i=0;i<sum_data;i++)
                            tdata[i]=rdata[count++];//���Ƶ���ʱ��������
                        data_start=(_data_start*)(tdata);//�ض���������Ϣ
                        (*(data->order[data_start->order]))(data_start->data,data_start->size);//���ָ��ֱ�ӵ��ù��Ӻ������д���

                    }
                    else
                    {
                        //�����ô����ַ�
                        count++;
                    }
				}
                else
                {
                    start_offset=sum_data-(left_data)-1;//�����ʼƫ����
                    if(data->receive_buffer.start_address[start_offset]=='$')//�Ƿ���н�����
                    {
                        //��ȷ���ݰ�
                        //ע�������ᵽ��bug
                         //��ȷ���ݰ�
                        for(i=0;i<left_data;i++)
                            tdata[i]=rdata[count+i];//β����������д����ʱ������
                        rdata=data->receive_buffer.start_address;//��ȡ�����ض����ַ
                        for(i=0;i<=start_offset;i++)
                            tdata[left_data+i]=rdata[i];//ͷ��������д����ʱ������
                        //�ض���������ʱ���������ؽ�
                        data_start=(_data_start*)(tdata);//�ض�����Ϣͷָ�룬ָ����ʱ������
                        (*(data->order[data_start->order]))(data_start->data,data_start->size);//���ָ��ֱ�ӵ��ù��Ӻ������д���
                        //��ض�������
                        max_length-=(count+sum_data);//�õ�����ʣ�µĳ���
                        rdata=data->receive_buffer.start_address+start_offset+1;//������µĴ����ַ
                        count=0;//��������0
                    }
                    else
                    {
                        //�����ô����ַ�
                        count++;
                    }
                }
            }
            else
            {
                //����δ�����꣬�˳��������
                data->receive_buffer.deal_address=rdata+count;//���浱ǰ�Ĵ����ַ
                data->receive_buffer.wait_deal_number=max_length-count;//�������ַ�
                return DATA_HALF;
            }
        }
        else
        {
            //����Ч�ַ�,����
            count++;
        }
    }
    //�����������ˣ����ǳ����нض�����µ�Ψһ������ù����п��ܻᶪʧ������������
    //��¼�������
    data->receive_buffer.deal_address=rdata+count;
    data->receive_buffer.wait_deal_number=max_length-count;
    return 0;
}

