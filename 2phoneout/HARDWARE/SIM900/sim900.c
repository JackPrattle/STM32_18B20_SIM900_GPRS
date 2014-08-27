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
		//while(!Hand("SIM900"))//��GSMģ�����֣��ж��Ƿ��յ�GSMģ�鷵�ص�OK
		
		while(GsmAtFlag == 0)//��GSMģ�����֣��ж��Ƿ��յ�GSMģ�鷵�ص�OK		
		{
			Uart2SendStr("ATI\r");//�򿪻�������
			delay_s(1);		
		}
		CleanGsmRcv();//������ջ���
		while(!Hand("COPS:"))
		{
			Uart2SendStr("AT+COPS?\r");
			delay_s(1);		
		}
		CleanGsmRcv();//������ջ���
	}

void  SendSMS(u8 *str, u8 *cellnumber)
	{
		Uart2SendStr("AT+CMGF=1\r");
		delay_s(1);//��ʱ1��
		Uart2SendStr("AT+CSCS=\"GSM\"\r");
		delay_s(1);//��ʱ1��
		Uart2SendStr("AT+CSMP=17,167,0,0\r");//�豸����ģʽ
		delay_s(1);//��ʱ1��
				
		Uart2SendStr("AT+CMGS=\""); // wl: ��������ӵ��˴�
		Uart2SendStr(cellnumber); 	// wl: ��������ӵ��˴�
		Uart2SendStr("\"\r");		// wl: ��������ӵ��˴�
		delay_s(1);//��ʱ1��
		
		Uart2SendStr(str);
		Uart2SendStr(tempbuf);
		Uart2SendHex(0x1a);
		delay_s(3);//��ʱ3��
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
		Uart2SendStr("ATH\r");//����ǰ�Ȱѵ绰�Ҷϡ�
		delay_s(2);
		Uart2SendStr("ATD");
		Uart2SendStr(cellnumber);
		Uart2SendStr(";\r");
		delay_s(1);
		
		LCD_WriteString_en(0,3,"Cal           ");
		LCD_WriteString_en(18,3,cellnumber);
	
		Uart2SendStr("AT+CLCC=1\r");
		delay_s(1);
		while(!Hand("+CLCC: 1,0,0"))//����Ƿ��Ѵ�ͨ���յ�+CLCC: 1,0,0��֤���Ѿ���ͨ�绰�ˡ�
		{
			CleanGsmRcv();
			Uart2SendStr("AT+CLCC\r");//���Խ�ͨ״̬
			delay_s(2);			
		}		
			delay_s(3);			
		Uart2SendStr("ATH\r"); //��ͨ��ѵ绰�Ҷ�
		LCD_WriteString_en(0,3,"              ");
	}
