/***********************************************************
酷学之家 http://simcom.taobao.com/
联系方式：15296811438 林工
*************************************************************/
#include "stm32f10x.h"
#include "led.h"
#include "timer.h"
#include "delay.h"
#include "sys.h"
#include "key.h"
#include "usart.h"
#include "string.h"
#include "5110LCD.h"
#include "ds18b20.h"
#include "sim900.h"
#include "math.h"
#include "string.h"
/************************************************************
程序说明：
本程序实现开机自动拨打电话10086
1.我们的程序全部都是通过单片机串口2与SIM900通讯，串口1用于下载程序和观察SIM900返回的数据
（串口2收到的SIM900返回的数据会通过串口1发送到电脑中，用于观察数据），使用过程中，如果没
有特殊要求跳线帽不用变动。
2.在开发板上放入您的手机卡，接好天线，打开电源，接入耳机。
3.把程序编译后下载到单片机中，使用KEIL4下载，具体请参照教程。
4.开机后可以观察到串口的输出。当设备寻找到信号后，自动拨打10086的电话，接入耳机可以听到声音。
*************************************************************/

/*************************************************************

程序逻辑
0,上电启动后,向特定手机listout[0]发送短信,表示已经启动成功.	

1.定期检测
使用计时器0, 每10秒检测一次温度. 连续三次大于阈值,发送报警短信listout[0-4].	

2. 收到电话
如果来电在listin[0-8]挂断电话,并回复当前温度值. 

------------------------------------------------------------
3. 读取SIM卡电话,向SIM卡中电话发报警.   need action
4. 定期检测 保存最近12次结果. 
5. 收到短信			动作
cxwd/CXWD			发送最近一次温度值
cxwd1/CXWD1			发送最近一次温度值
cxwd2/CXWD2			发送最近两次温度值
cxwd12/CXWD12		发送最近十二次次温度值
tzgj/TZGJ			停止告警 30分钟内停止告警
*************************************************************/
#define C_wl1 "13810222293"
#define C_wl2 "13810154315"
#define C_wys1 "13811696749" 
#define C_lr "13910601016"	
#define C_qjm "18600511228" 
#define C_wys2 "13301006536" 
#define C_lwl "13911572648"
#define C_zjp "18610257810"
#define C_tj "13911066733"

u16 sms_num=5;						// 报警短信发送给Listout中的前几个用户
u16 alarm_thred =30;				// 报警温度
u16 abn_thred =5;					// 连续超限次数。满足后将报警(去噪)
u16	abn_count=0;					// 缓存连续超限次数统计(去噪)
u16	last_alarm=0;					// 缓存上次报警温度
u16	alarm_level=0;					// 报警级别 1, 2, 3, 4....  
u16 nor_count=0;					// 温度正常计数 . 报警级别清零计数(>7200后	清零)
				

u8 * Listout[] = {C_wl1,C_wl2,C_wys1,C_lr,C_qjm};
u8 * Listin[] ={C_wl1,C_wl2,C_wys1,C_wys2,C_lr,C_qjm,C_zjp,C_lwl,C_tj,'\0'}; // 
u8 Gsmtmp[MAXRCV] = {0};
int main(void)

{                    
	short temperature;    		//温度变量
	int i,j;
	
	SystemInit();				//STM32初始化
	delay_init(72);	     		//延时初始化
	
	NVIC_Configuration();		//设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	
	Uart1Init(9600);			//串口初始化为9600
	Uart2Init(9600);			//串口初始化为9600
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	//LCD初始化
	GPIO_Configuration_LCD();								//LCD初始化
	LCD_Init();												//LCD初始化
	LCD_Clear();											//LCD初始化
	LCD_WriteString_en(0,0,"System Start  ");		    
	delay_s(1);
		
	Timerx_Init(1000,7199);		//10Khz的计数频率，1000次为100ms

	LED_Init();                 //初始化与LED连接的硬件接口
	LED3_OFF;	LED4_OFF;	LED5_OFF;	LED6_OFF;
									 	
	LCD_WriteString_en(0,0,"DS18B20 Test  ");		    		//DS18B20初始化
	delay_s(1);

	while(DS18B20_Init())
	{
		LCD_WriteString_en(0,0,"DS18B20 Error ");		    
		delay_ms(200);
    }                                                        
	LCD_WriteString_en(0,0,"DS18B20 OK    ");		    
	delay_s(1);
	
	PAout(11)=0;				//启动GSM模块
	LCD_WriteString_en(0,0,"GSM Starting  ");
	delay_s(1);
	sim900_ini();
	LCD_WriteString_en(0,0,"GSM on net    ");
	
	LCD_WriteString_en(0,1,"Temp:");		        	
	LCD_WriteString_en(0,2,"Count:");		        	
	
	for (i=0;i<10;i++)	
		{
		Temperaturepro();	
		delay_s(1);
		}

	SendSMS_start(Listout[0]);
	CleanGsmRcv();//清除接收缓存
	
	while(1)  
	{   
		//------------------------
		//Check temperature.
		//每秒检查一次温度
		//温度上升到alarm_thred, alarm_thred+2, alarm_thred+4, alarm_thred+6..... 时报警	   !WARNING!
		//温度恢复到alarm_thred, alarm_thred+2, alarm_thred+4, alarm_thred+6..... 时报消息  !NOTIFICATION!
		//温度下降到alarm_thred-1 时报警解除
		//------------------------
		if(CheckTem_flag==1)						//每check_interval 秒读取一次温度
		{   						
			CheckTem_flag=0;						//清空标记位
			LED3=!LED3;								//使LED3闪烁，表示系统循环中。
			temperature=Temperaturepro();			//获取温度
			LCD_WriteString_en(30,1,tempbuf);
			LCD_WriteString_en(36,2,"        ");	
			LCD_WriteNumber(36,2,abn_count);
			if (temperature>alarm_thred*10)			//温度超过阈值
			{	
				abn_count++;				//去噪计数
				nor_count=0;				//温度低于阈值计数清零
				if ((abn_count >= abn_thred)&&(fabs(temperature-last_alarm)>=20))//(去噪)而且比上次报警温度相差2度 (30.1, 32.1, 34.1,36.1...)
				{
					if (temperature>last_alarm)
					{
						alarm_level++;
						for (i=0;i<sms_num;i++)				//短信群发
						{
							SendSMS_alert(alarm_level,Listout[i]);	//报警短信	
							//callyou(Listout[i]);
							CleanGsmRcv();					//清除GSM接收缓存
							delay_s(10);					//发送短信,延迟十秒
						}
					}
					else 
					{
						alarm_level--;
						for (i=0;i<sms_num;i++)				//短信群发
						{
							SendSMS_recover(Listout[i]);//提示短信			
							CleanGsmRcv();					//清除GSM接收缓存
							delay_s(10);					//发送短信,延迟十秒
						}
					}
					abn_count=0;
					last_alarm=temperature;					 //记录报警温度
				}
			}				 
			else							 	//温度低于阈值
			{
				nor_count++;				  //温度低于阈值计数
				abn_count=0;				  //去噪计数清零
				/*if (nor_count > 7200)		//确保温度低于阈值2小时后再清零报警温度记录.
				{
					nor_count=0;
					last_alarm=0;
				}				  				*/
				if 	((temperature<(alarm_thred -1)*10)&&(last_alarm>alarm_thred))//确保温度低于"阈值-1"后,再清零报警温度记录.
				{					
					for (i=0;i<sms_num;i++)				//短信群发
					{
						SendSMS_alertclr(Listout[i]);	//报警清除短信	
						CleanGsmRcv();					//清除GSM接收缓存
						delay_s(10);					//发送短信,延迟十秒
					}
					last_alarm=0;
					alarm_level=0;
				}	
			}
		}
		//------------------------
		//Check GSM received Ring signal
		//------------------------

		if ((GsmAtFlag==1)&&(strstr(GsmRcv,"RING")!=0))						//收到sim900消息
		{
			LCD_WriteString_en(0,3,"Check  Incoming");
			delay_s(1);
			LCD_WriteString_en(0,4,"NoMarching yet");
			Uart2SendStr("AT+CLCC\r");//发送AT+CLCC/r去读取来电号码	 
			delay_s(1);
			if (strstr(GsmRcv, "+CLCC: 1")!=0)
			{	
				strcpy(Gsmtmp,GsmRcv);	
				for (i=0;Listin[i]!='\0';i++)							
				{	
					LCD_WriteString_en(0,3,"Checking      ");
					LCD_WriteNumber(54,3,i+1);
					delay_s(1);
					if (strstr(Gsmtmp, Listin[i])!=0)	//在许可列表中
					{	
						for (j=0;j<11;j++)		numin[j]=Listin[i][j];
						numin[11]='\0';
						LCD_WriteString_en(0,3,"Marching Found");
						LCD_WriteString_en(0,4,"              ");
						LCD_WriteString_en(0,4,numin);
						delay_s(1);
						Uart2SendStr("ATH\r");	
						delay_s(1);
						SendSMS_query(numin);
						CleanGsmRcv();					//清除GSM接收缓存
						break;
					}
				}
				Uart2SendStr("ATH\r");
				delay_s(2);	
				LCD_WriteString_en(0,3,"              ");
				LCD_WriteString_en(0,4,"              ");
			}
			CleanGsmRcv();//清除接收缓存
		}

	}
}
  
