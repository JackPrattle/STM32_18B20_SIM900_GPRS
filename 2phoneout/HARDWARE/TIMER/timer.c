/***********************************************************
酷学之家 http://simcom.taobao.com/
联系方式：15296811438 林工
*************************************************************/

#include "timer.h"
#include "led.h"
#include "usart.h"
//#include "sysh.h"

u16  timer_count =0; 				//每次定时器中断，timer_count++; 
u8	check_interval=1;				// 1秒钟检查一次温度
u8  CheckTem_flag;	 				// 检查温度标签   main 函数中通过CheckTem_flag来判断是否需要检查温度.


//通用定时器中断初始化
//这里时钟选择为APB1的2倍，而APB1为36M
//arr：自动重装值。
//psc：时钟预分频数
//这里使用的是定时器3!
void Timerx_Init(u16 arr,u16 psc)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //时钟使能

	TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	 计数到1000为100ms
	TIM_TimeBaseStructure.TIM_Prescaler =(psc); //设置用来作为TIMx时钟频率除数的预分频值  10Khz的计数频率  
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
 
	TIM_ITConfig(  //使能或者失能指定的TIM中断
		TIM3, //TIM2
		TIM_IT_Update  |  //TIM 中断源
		TIM_IT_Trigger,   //TIM 触发中断源 
		ENABLE  //使能
		);
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;  //TIM3中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;  //先占优先级0级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;  //从优先级3级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);  //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器

	TIM_Cmd(TIM3, ENABLE);  //使能TIMx外设
							 
}

void TIM3_IRQHandler(void)   //TIM3中断
{
	u16 i = 0;
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) //检查指定的TIM中断发生与否:TIM 中断源 
	{
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);  //清除TIMx的中断待处理位:TIM 中断源

  		timer_count++;
		if(timer_count>60000) 		timer_count=0;  				// 6000秒，100分钟定时清0
		if(timer_count/check_interval%10==5)	CheckTem_flag=1;	//每check_interval 秒读取一次温度

//#ifdef UART1_DEBUG//发送数据用于调试。
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












