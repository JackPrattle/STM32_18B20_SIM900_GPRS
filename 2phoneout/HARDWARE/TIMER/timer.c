/***********************************************************
��ѧ֮�� http://simcom.taobao.com/
��ϵ��ʽ��15296811438 �ֹ�
*************************************************************/

#include "timer.h"
#include "led.h"
#include "usart.h"
//#include "sysh.h"

u16  timer_count =0; 				//ÿ�ζ�ʱ���жϣ�timer_count++; 
u8	check_interval=1;				// 1���Ӽ��һ���¶�
u8  CheckTem_flag;	 				// ����¶ȱ�ǩ   main ������ͨ��CheckTem_flag���ж��Ƿ���Ҫ����¶�.


//ͨ�ö�ʱ���жϳ�ʼ��
//����ʱ��ѡ��ΪAPB1��2������APB1Ϊ36M
//arr���Զ���װֵ��
//psc��ʱ��Ԥ��Ƶ��
//����ʹ�õ��Ƕ�ʱ��3!
void Timerx_Init(u16 arr,u16 psc)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //ʱ��ʹ��

	TIM_TimeBaseStructure.TIM_Period = arr; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	 ������1000Ϊ100ms
	TIM_TimeBaseStructure.TIM_Prescaler =(psc); //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ  10Khz�ļ���Ƶ��  
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
 
	TIM_ITConfig(  //ʹ�ܻ���ʧ��ָ����TIM�ж�
		TIM3, //TIM2
		TIM_IT_Update  |  //TIM �ж�Դ
		TIM_IT_Trigger,   //TIM �����ж�Դ 
		ENABLE  //ʹ��
		);
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;  //TIM3�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;  //��ռ���ȼ�0��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;  //�����ȼ�3��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);  //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���

	TIM_Cmd(TIM3, ENABLE);  //ʹ��TIMx����
							 
}

void TIM3_IRQHandler(void)   //TIM3�ж�
{
	u16 i = 0;
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) //���ָ����TIM�жϷ������:TIM �ж�Դ 
	{
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);  //���TIMx���жϴ�����λ:TIM �ж�Դ

  		timer_count++;
		if(timer_count>60000) 		timer_count=0;  				// 6000�룬100���Ӷ�ʱ��0
		if(timer_count/check_interval%10==5)	CheckTem_flag=1;	//ÿcheck_interval ���ȡһ���¶�

//#ifdef UART1_DEBUG//�����������ڵ��ԡ�
		Uart1SendStr(DebugBuf_U2);
		for(i=0; i<MAXBUF; i++)
		{
			DebugBuf_U2[i] = 0;	
		}
		Debug2RcvCnt = 0;

		Uart2SendStr(DebugBuf_U1);
		for(i=0; i<MAXBUF; i++)
		{
			DebugBuf_U1[i] = 0;	
		}
		Debug1RcvCnt = 0;


//#endif		 
//		LED1=!LED1;
	}
}












