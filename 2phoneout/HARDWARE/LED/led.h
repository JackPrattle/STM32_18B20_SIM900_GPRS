#ifndef __LED_H
#define __LED_H	 
#include "stm32f10x.h"
//Mini STM32开发板
//LED驱动代码			 
//正点原子@ALIENTEK
//2012/2/27

//LED端口定义
#define LED3 PBout(12)// PB12
#define LED4 PBout(13)// PB13	
#define LED5 PBout(14)// PB14
#define LED6 PBout(15)// PB15

#define LED3_ON LED3=0// PB12
#define LED4_ON LED4=0// PB13	
#define LED5_ON LED5=0// PB14
#define LED6_ON LED6=0// PB15

#define LED3_OFF LED3=1// PB12
#define LED4_OFF LED4=1// PB13	
#define LED5_OFF LED5=1// PB14
#define LED6_OFF LED6=1// PB15
	
void LED_Init(void);//初始化

		 				    
#endif
