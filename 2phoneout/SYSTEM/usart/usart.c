/***********************************************************
��ѧ֮�� http://simcom.taobao.com/
��ϵ��ʽ��15296811438 �ֹ�
*************************************************************/
#include "sys.h"
#include "usart.h"
  
//�������´���,֧��printf����,������Ҫѡ��use MicroLIB
#if 1
#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
_sys_exit(int x) 
{ 
	x = x; 
} 
//�ض���fputc���� 
int fputc(int ch, FILE *f)
{      
	while((USART1->SR&0X40)==0);//ѭ������,ֱ���������   
    USART1->DR = (u8) ch;      
	return ch;
}
#endif 

/*ʹ��microLib�ķ���*/
 /* 
int fputc(int ch, FILE *f)
{
	USART_SendData(USART1, (uint8_t) ch);

	while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET) {}	
   
    return ch;
}
int GetKey (void)  { 

    while (!(USART1->SR & USART_FLAG_RXNE));

    return ((int)(USART1->DR & 0x1FF));
}
*/

u8 GsmAtFlag =0;
u8 GsmRcv[MAXRCV] = {0};
u8 GsmRcvAt[MAXRCV] = {0};
u16 GsmRcvCnt = 0;
u16 Debug1RcvCnt = 0;
u8 DebugBuf_U1[MAXBUF] = {0};

u8 DebugBuf_U2[MAXBUF] = {0};
u16 Debug2RcvCnt = 0;



void Uart1Init(u32 bound){
    //GPIO�˿�����
    GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO, ENABLE);
     //USART1_TX   PA.9
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
   
    //USART1_RX	  PA.10
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);  

   //Usart1 NVIC ����

    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//

	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���USART1
  
   //USART ��ʼ������
   
	USART_InitStructure.USART_BaudRate = bound;//һ������Ϊ9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

    USART_Init(USART1, &USART_InitStructure);
   

    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//�����ж�
   
    USART_Cmd(USART1, ENABLE);                    //ʹ�ܴ��� 

}

void Uart1SendHex(u8 ch)
{
	while((USART1->SR&0X40)==0);//ѭ������,ֱ���������   
    USART1->DR = (u8) ch; 
}

void Uart1SendStr(u8* str)
{
	u16 cnt=0;
	while(*(str+cnt))	//�ж�һ�������Ƿ����
	{
		while((USART1->SR&0X40)==0);//ѭ������,ֱ���������   
	    USART1->DR = *(str+cnt);
		cnt++;	//׼������һ������
	}
}

void USART1_IRQHandler(void)                	//����1�жϷ������
{
	u8 tmp;
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //�����ж�
	{
		tmp =USART_ReceiveData(USART1);//(USART1->DR);	//��ȡ���յ�������

//#ifdef UART1_DEBUG
		//�����յ������ݷ���DebugBuf_U1���ڶ�ʱ���ڣ�DebugBuf_U1�ᱻ����2���͸�GSMģ�顣
		//����ͨ������1���͵���Ƭ��������GSMģ��Ϳ����յ��ˣ���ҪΪ�˵��Եķ��㡣
		DebugBuf_U1[Debug1RcvCnt] = tmp;
		Debug1RcvCnt++;
		if(Debug1RcvCnt>=MAXBUF-1)
		{
			Debug1RcvCnt = 0;	
		}
//#endif


		
	}
}


void Uart2Init(u32 bound)
{
    //GPIO�˿�����
    GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
     //USART2_TX   PA.2
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
   
    //USART2_RX	  PA.3
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);  

   //Usart1 NVIC ����

    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		//

	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���USART2
  
   //USART ��ʼ������
   
	USART_InitStructure.USART_BaudRate = bound;//һ������Ϊ9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

	USART_DeInit(USART2);
    USART_Init(USART2, &USART_InitStructure);
   

    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//�����ж�
   
    USART_Cmd(USART2, ENABLE);                    //ʹ�ܴ��� 
	
}

void USART2_IRQHandler(void)                	//����2�жϷ������
{
	u8 tmp;
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)  //�����ж�
	{
		tmp = USART_ReceiveData(USART2);//(USART1->DR);	//��ȡ���յ�������
		GsmRcv[GsmRcvCnt] = tmp;
		GsmRcvCnt++;
		//�յ���������ATָ�������ATָ������0x0a 0x0d��β�ġ��������жϣ��ڽ��յĹ������Ƿ��յ�0x0a 0x0d
		
		if(GsmRcv[GsmRcvCnt-2] == 0x0d && GsmRcv[GsmRcvCnt-1] == 0x0a && GsmRcvCnt >= 2)
		{	
			//һ���յ�0x0a 0x0d���ͽ����ݱ����������û����������жϡ�			
/*			for(i=0; i<GsmRcvCnt; i++)
			{
				GsmRcvAt[i] = GsmRcv[i];
				GsmRcv[i] = 0;	
			}
			GsmRcvAt[GsmRcvCnt]='\0';
			GsmRcvCnt = 0;
*/
			GsmAtFlag=1;//�յ���������atָ�ͨ�������־λ��1��������������֪��ȥ�ж��ˡ�			
		}
		
		if(GsmRcvCnt > MAXRCV)
		{
			GsmRcvCnt = 0;
		}
//#ifdef UART1_DEBUG
		//�����յ������ݷ���DebugBuf_U2���ڶ�ʱ���ڣ�DebugBuf_U2�ᱻ����1���͸�GSMģ�顣
		//����ͨ������1���͵���Ƭ��������GSMģ��Ϳ����յ��ˣ���ҪΪ�˵��Եķ��㡣
		DebugBuf_U2[Debug2RcvCnt] = tmp;
		Debug2RcvCnt++;
		if(Debug2RcvCnt>=MAXBUF-1)
		{
			Debug2RcvCnt = 0;	
		}
//#endif
	}
}

void Uart2SendHex(u8 ch)
{
		while (USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);//�������
		USART_SendData(USART2, ch);
}

void Uart2SendStr(u8* str)
{
	u16 cnt=0;
	while(*(str+cnt))	//�ж�һ�������Ƿ����
	{
		while (USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);//�������
		USART_SendData(USART2, *(str+cnt));
		cnt++;	//׼������һ������
	}
}



void Uart3Init(u32 bound)
{
    //GPIO�˿�����
    GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);  
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3 , ENABLE);
     //USART3_TX   PB.10
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
   
    //USART3_RX	  PB.11
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOB, &GPIO_InitStructure);  

   //Usart3 NVIC ����

    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//

	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���USART3
  
   //USART ��ʼ������

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);  //RCC_APB1_USART3ʹ��
	USART_InitStructure.USART_BaudRate = 9600;  //������
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;  //�����ֳ�8
	USART_InitStructure.USART_StopBits = USART_StopBits_1;  //ֹͣλΪ1
	USART_InitStructure.USART_Parity = USART_Parity_No;  //��żʧ��
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;  //����ʹ��  ����ʹ��
	USART_Init(USART3, &USART_InitStructure);

    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//�����ж�
   
    USART_Cmd(USART3, ENABLE);                    //ʹ�ܴ���
}

void USART3_IRQHandler(void)                	//����3�жϷ������
{

}

void Uart3SendHex(u8 ch)
{
	while((USART3->SR&0X40)==0);//ѭ������,ֱ���������   
    USART3->DR = (u8) ch; 
}

void Uart3SendStr(u8* str)
{
	u16 cnt=0;
	while(*(str+cnt))	//�ж�һ�������Ƿ����
	{
		while((USART3->SR&0X40)==0);//ѭ������,ֱ���������   
	    USART3->DR = *(str+cnt);
		cnt++;	//׼������һ������
	}
}

void CleanGsmRcv(void)
{
	u16 i = 0;
	for(i=0; i<=MAXRCV; i++)
	{
		GsmRcv[i] = 0;
	//	GsmRcvAt[i]=0;	
	}

	GsmRcvCnt = 0;
	GsmAtFlag = 0;	
}

u8 Hand(unsigned char *a)
{ 
    if(strstr(GsmRcv,a)!=NULL)
	    return 1;
	else
		return 0;
}

 
