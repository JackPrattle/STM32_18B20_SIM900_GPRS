#include "sim900.h"
#include "usart.h"
#include "delay.h"
#include "ds18b20.h"
#include "5110LCD.h"
#include "sys.h"
u8 * sms_start = "Device Started.\rTesting room current temperature:\r";
u8 * sms_CuTe = "Testing room current temperature: ";
																			
u8 * sms_alert1 = "    ! Alert !    \rTesting room temperature rises to :\r";
u8 * sms_alert2 = "   !! Alert !!   \rTesting room temperature rises to :\r";
u8 * sms_alert3 = "  !!! Alert !!!  \rTesting room temperature rises to :\r";
u8 * sms_alert4 = " !!!! Alert !!!! \rTesting room temperature rises to :\r";																				  

u8 * sms_recover = "***Notification***\rTesting room temperature falls to:\r";

u8 * sms_alertclr = ":-) Alarm Clear :-)\rTesting room temperature falls to:\r"	;

u8 numin[12]={0};
int i;

void sim900_ini(void)	
	{
		CleanGsmRcv();	
		//while(!Hand("SIM900"))//与GSM模块握手，判断是否收到GSM模块返回的OK
		
		while(GsmAtFlag == 0)//与GSM模块握手，判断是否收到GSM模块返回的OK		
		{
			Uart2SendStr("ATI\r");//打开回显命令
			delay_s(1);		
		}
		CleanGsmRcv();//清除接收缓存
		while(!Hand("COPS:"))
		{
			Uart2SendStr("AT+COPS?\r");
			delay_s(1);		
		}
		CleanGsmRcv();//清除接收缓存
	}

void  SendSMS(u8 *str, u8 *cellnumber)
	{
		Uart2SendStr("AT+CMGF=1\r");
		delay_s(1);//延时1秒
		Uart2SendStr("AT+CSCS=\"GSM\"\r");
		delay_s(1);//延时1秒
		Uart2SendStr("AT+CSMP=17,167,0,0\r");//设备短信模式
		delay_s(1);//延时1秒
				
		Uart2SendStr("AT+CMGS=\""); // wl: 将变量添加到此处
		Uart2SendStr(cellnumber); 	// wl: 将变量添加到此处
		Uart2SendStr("\"\r");		// wl: 将变量添加到此处
		delay_s(1);//延时1秒
		
		Uart2SendStr(str);
		Uart2SendStr(tempbuf);
		Uart2SendHex(0x1a);
		delay_s(3);//延时3秒
	}

void  SendSMS_start(u8 *cellnumber)
	{
		LCD_WriteString_en(0,3,"SSM           ");
		LCD_WriteString_en(18,3,cellnumber);
		SendSMS( sms_start, cellnumber);
		LCD_WriteString_en(0,3,"              ");
	}	
												
void  SendSMS_alert(u8 level, u8 *cellnumber)
	{	
		LCD_WriteString_en(0,3,"ASM           ");
		LCD_WriteString_en(18,3,cellnumber);
		if (level==1) SendSMS( sms_alert1, cellnumber);
		if (level==2) SendSMS( sms_alert2, cellnumber);
		if (level==3) SendSMS( sms_alert3, cellnumber);
		if (level>3) SendSMS( sms_alert4, cellnumber);
		LCD_WriteString_en(0,3,"              ");
	}

void  SendSMS_recover(u8 *cellnumber)
	{	
		LCD_WriteString_en(0,3,"RSM           ");
		LCD_WriteString_en(18,3,cellnumber);
		SendSMS( sms_recover, cellnumber);
		LCD_WriteString_en(0,3,"              ");
	}

void SendSMS_alertclr (u8 *cellnumber)
	{	
		LCD_WriteString_en(0,3,"CSM           ");
		LCD_WriteString_en(18,3,cellnumber);
		SendSMS( sms_alertclr, cellnumber);
		LCD_WriteString_en(0,3,"              ");
	}


void  SendSMS_query(u8 *cellnumber)
	{	
		LCD_WriteString_en(0,3,"QSM           ");
		LCD_WriteString_en(18,3,cellnumber);		
		SendSMS( sms_CuTe, cellnumber);	
		LCD_WriteString_en(0,3,"              ");
	}

void  CallYou(u8 *cellnumber)
	{
		Uart2SendStr("ATH\r");//拨打前先把电话挂断。
		delay_s(2);
		Uart2SendStr("ATD");
		Uart2SendStr(cellnumber);
		Uart2SendStr(";\r");
		delay_s(1);
		
		LCD_WriteString_en(0,3,"Cal           ");
		LCD_WriteString_en(18,3,cellnumber);
	
		Uart2SendStr("AT+CLCC=1\r");
		delay_s(1);
		while(!Hand("+CLCC: 1,0,0"))//检查是否已打通，收到+CLCC: 1,0,0，证明已经打通电话了。
		{
			CleanGsmRcv();
			Uart2SendStr("AT+CLCC\r");//测试接通状态
			delay_s(2);			
		}		
			delay_s(3);			
		Uart2SendStr("ATH\r"); //接通后把电话挂断
		LCD_WriteString_en(0,3,"              ");
	}
