#include "common.h"
/**
* ��ʼ���������
*/

UartDataStruct UploadTQ[UPLOAD_NODE_NUM];
uint8 current_upload_tsk;
uint8 last_upload_tsk;
uint8 Node_Inwaiting = 0;							//�����нڵ������
uint16 Pop_Count = 0;
uint8 Upload_Ack = 0;
uint8 Receive_Upload_Ack_Fail = 0;

void Clear_Node(UartDataStruct *node)
{
    node->address = 0;
    node->data = 0;
}
void Init_Upload_TQ(void)
{
    int i;
    for (i=0; i<UPLOAD_NODE_NUM; i++)
    {
        Clear_Node(&UploadTQ[i]);
    }
    current_upload_tsk = 0;
    last_upload_tsk = 0;
}
uint8 Empty_Node(UartDataStruct *node)
{
    if(node->address == 0&&node->data==0)
        return 1;
    else
        return 0;
}
void Clear_Buffer(uint8 current,uint8 last)
{
		int i=0;
		for(i=current;i<last;i++)
		{
				Clear_Node(&UploadTQ[i]);
		}
}
/**
* �������
*/

uint8 PostUploadNode(UartDataStruct node)
{
    if (Empty_Node(&UploadTQ[last_upload_tsk]))
    {
        UploadTQ[last_upload_tsk].address = node.address;
				UploadTQ[last_upload_tsk].data = node.data;
			  
        last_upload_tsk = (last_upload_tsk + 1) % UPLOAD_NODE_NUM;
				Node_Inwaiting++;							
        return TQ_SUCCESS;
    }
    else
    {
        //printf("TQ is FULL!\n");
				Usart1_PutChar(0x7D);
				Usart1_PutChar(0x7F);
				printf("Upload Quene Full\n");
				Clear_Buffer(0,UPLOAD_NODE_NUM);
        return TQ_FULL;		
    }
}

/**
* �������
*/
UartDataStruct PopUploadNode(void)
{
    UartDataStruct node;
    if (!Empty_Node(&UploadTQ[current_upload_tsk]))
    {
        node = UploadTQ[current_upload_tsk];
        //Clear_Node(&UploadTQ[current_upload_tsk]);
        current_upload_tsk = (current_upload_tsk + 1) % UPLOAD_NODE_NUM;
        return node;
    }
    else
    {
        //printf("TQ is EMPTY!\n");
        return node;
    }
}

uint8 rec;
void Upload_Data()
{
    uint16 time_out = 0;
		uint16 i=0;
	
		if(Node_Inwaiting == 0)		//û��Ҫ���͵Ľڵ�
		{
				return;
		}
		Usart1_PutChar(0x00);//���������ʱ���ղ���
		Usart1_PutChar(0x7D);
    Usart1_PutChar(0x7E);								//��ͷ
		Usart1_PutChar(Node_Inwaiting);			//����Ҫ���͵Ľڵ����
		for(i=0;i<Node_Inwaiting;i++)
		{
				Usart1_PutChar(UploadTQ[i].address >> 8);
        Usart1_PutChar(UploadTQ[i].address);
        Usart1_PutChar(UploadTQ[i].data);
		}
#if UART_INT_ENABLE
		while (Upload_Ack == 0)
		{
				if (time_out++ > 150)
				{
						Receive_Upload_Ack_Fail = 1;
						/*Usart1_PutChar(0x7D);
						Usart1_PutChar(0x7F);								//��ͷ
						printf("upload no ack");*/
						//return;
					break;
				}
				delay_ms(1);
		}
#else
		Receive_Upload_Ack_Fail = 0;
		while (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET)
		{
				if (time_out++ > 15000)
				{
						Receive_Upload_Ack_Fail = 1;				//���û���յ�ȷ�ϣ�����ָ����ͷ�����·���
						break;
				}
				delay_us(10);
		}
		if(USART1_Getchar()!='o')					//ȷ������λ'o'
		{
				Receive_Upload_Ack_Fail = 1;				//���û���յ�ȷ�ϣ�����ָ����ͷ�����·���
		}
				
#endif
		Upload_Ack = 0;
		Receive_Upload_Ack_Fail = 0;
		if(Node_Inwaiting > 10)
		{
				Init_Upload_TQ();
				Node_Inwaiting = 0;
		}
		if(Receive_Upload_Ack_Fail == 1)
		{
				Receive_Upload_Ack_Fail = 0;
				DebugMsg("upload no ack");
				return;
		}
		Node_Inwaiting = 0;
		return;
}

void UploadDrawData()
{
		Usart1_PutChar(0x7D);
		Usart1_PutChar(Draw_DataX>>8);
		Usart1_PutChar(Draw_DataX);
		Usart1_PutChar(Draw_DataY>>8);
		Usart1_PutChar(Draw_DataY);
}
