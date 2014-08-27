/***********************************************************
��ѧ֮�� http://simcom.taobao.com/
��ϵ��ʽ��15296811438 �ֹ�
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
����˵����
������ʵ�ֿ����Զ�����绰10086
1.���ǵĳ���ȫ������ͨ����Ƭ������2��SIM900ͨѶ������1�������س���͹۲�SIM900���ص�����
������2�յ���SIM900���ص����ݻ�ͨ������1���͵������У����ڹ۲����ݣ���ʹ�ù����У����û
������Ҫ������ñ���ñ䶯��
2.�ڿ������Ϸ��������ֻ������Ӻ����ߣ��򿪵�Դ�����������
3.�ѳ����������ص���Ƭ���У�ʹ��KEIL4���أ���������ս̡̳�
4.��������Թ۲쵽���ڵ���������豸Ѱ�ҵ��źź��Զ�����10086�ĵ绰�����������������������
*************************************************************/

/*************************************************************

�����߼�
0,�ϵ�������,���ض��ֻ�listout[0]���Ͷ���,��ʾ�Ѿ������ɹ�.	

1.���ڼ��
ʹ�ü�ʱ��0, ÿ10����һ���¶�. �������δ�����ֵ,���ͱ�������listout[0-4].	

2. �յ��绰
���������listin[0-8]�Ҷϵ绰,���ظ���ǰ�¶�ֵ. 

------------------------------------------------------------
3. ��ȡSIM���绰,��SIM���е绰������.   need action
4. ���ڼ�� �������12�ν��. 
5. �յ�����			����
cxwd/CXWD			�������һ���¶�ֵ
cxwd1/CXWD1			�������һ���¶�ֵ
cxwd2/CXWD2			������������¶�ֵ
cxwd12/CXWD12		�������ʮ���δ��¶�ֵ
tzgj/TZGJ			ֹͣ�澯 30������ֹͣ�澯
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

u16 sms_num=5;						// �������ŷ��͸�Listout�е�ǰ�����û�
u16 alarm_thred =30;				// �����¶�
u16 abn_thred =5;					// �������޴���������󽫱���(ȥ��)
u16	abn_count=0;					// �����������޴���ͳ��(ȥ��)
u16	last_alarm=0;					// �����ϴα����¶�
u16	alarm_level=0;					// �������� 1, 2, 3, 4....  
u16 nor_count=0;					// �¶��������� . ���������������(>7200��	����)
				

u8 * Listout[] = {C_wl1,C_wl2,C_wys1,C_lr,C_qjm};
u8 * Listin[] ={C_wl1,C_wl2,C_wys1,C_wys2,C_lr,C_qjm,C_zjp,C_lwl,C_tj,'\0'}; // 
u8 Gsmtmp[MAXRCV] = {0};
int main(void)

{                    
	short temperature;    		//�¶ȱ���
	int i,j;
	
	SystemInit();				//STM32��ʼ��
	delay_init(72);	     		//��ʱ��ʼ��
	
	NVIC_Configuration();		//����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	
	Uart1Init(9600);			//���ڳ�ʼ��Ϊ9600
	Uart2Init(9600);			//���ڳ�ʼ��Ϊ9600
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	//LCD��ʼ��
	GPIO_Configuration_LCD();								//LCD��ʼ��
	LCD_Init();												//LCD��ʼ��
	LCD_Clear();											//LCD��ʼ��
	LCD_WriteString_en(0,0,"System Start  ");		    
	delay_s(1);
		
	Timerx_Init(1000,7199);		//10Khz�ļ���Ƶ�ʣ�1000��Ϊ100ms

	LED_Init();                 //��ʼ����LED���ӵ�Ӳ���ӿ�
	LED3_OFF;	LED4_OFF;	LED5_OFF;	LED6_OFF;
									 	
	LCD_WriteString_en(0,0,"DS18B20 Test  ");		    		//DS18B20��ʼ��
	delay_s(1);

	while(DS18B20_Init())
	{
		LCD_WriteString_en(0,0,"DS18B20 Error ");		    
		delay_ms(200);
    }                                                        
	LCD_WriteString_en(0,0,"DS18B20 OK    ");		    
	delay_s(1);
	
	PAout(11)=0;				//����GSMģ��
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
	CleanGsmRcv();//������ջ���
	
	while(1)  
	{   
		//------------------------
		//Check temperature.
		//ÿ����һ���¶�
		//�¶�������alarm_thred, alarm_thred+2, alarm_thred+4, alarm_thred+6..... ʱ����	   !WARNING!
		//�¶Ȼָ���alarm_thred, alarm_thred+2, alarm_thred+4, alarm_thred+6..... ʱ����Ϣ  !NOTIFICATION!
		//�¶��½���alarm_thred-1 ʱ�������
		//------------------------
		if(CheckTem_flag==1)						//ÿcheck_interval ���ȡһ���¶�
		{   						
			CheckTem_flag=0;						//��ձ��λ
			LED3=!LED3;								//ʹLED3��˸����ʾϵͳѭ���С�
			temperature=Temperaturepro();			//��ȡ�¶�
			LCD_WriteString_en(30,1,tempbuf);
			LCD_WriteString_en(36,2,"        ");	
			LCD_WriteNumber(36,2,abn_count);
			if (temperature>alarm_thred*10)			//�¶ȳ�����ֵ
			{	
				abn_count++;				//ȥ�����
				nor_count=0;				//�¶ȵ�����ֵ��������
				if ((abn_count >= abn_thred)&&(fabs(temperature-last_alarm)>=20))//(ȥ��)���ұ��ϴα����¶����2�� (30.1, 32.1, 34.1,36.1...)
				{
					if (temperature>last_alarm)
					{
						alarm_level++;
						for (i=0;i<sms_num;i++)				//����Ⱥ��
						{
							SendSMS_alert(alarm_level,Listout[i]);	//��������	
							//callyou(Listout[i]);
							CleanGsmRcv();					//���GSM���ջ���
							delay_s(10);					//���Ͷ���,�ӳ�ʮ��
						}
					}
					else 
					{
						alarm_level--;
						for (i=0;i<sms_num;i++)				//����Ⱥ��
						{
							SendSMS_recover(Listout[i]);//��ʾ����			
							CleanGsmRcv();					//���GSM���ջ���
							delay_s(10);					//���Ͷ���,�ӳ�ʮ��
						}
					}
					abn_count=0;
					last_alarm=temperature;					 //��¼�����¶�
				}
			}				 
			else							 	//�¶ȵ�����ֵ
			{
				nor_count++;				  //�¶ȵ�����ֵ����
				abn_count=0;				  //ȥ���������
				/*if (nor_count > 7200)		//ȷ���¶ȵ�����ֵ2Сʱ�������㱨���¶ȼ�¼.
				{
					nor_count=0;
					last_alarm=0;
				}				  				*/
				if 	((temperature<(alarm_thred -1)*10)&&(last_alarm>alarm_thred))//ȷ���¶ȵ���"��ֵ-1"��,�����㱨���¶ȼ�¼.
				{					
					for (i=0;i<sms_num;i++)				//����Ⱥ��
					{
						SendSMS_alertclr(Listout[i]);	//�����������	
						CleanGsmRcv();					//���GSM���ջ���
						delay_s(10);					//���Ͷ���,�ӳ�ʮ��
					}
					last_alarm=0;
					alarm_level=0;
				}	
			}
		}
		//------------------------
		//Check GSM received Ring signal
		//------------------------

		if ((GsmAtFlag==1)&&(strstr(GsmRcv,"RING")!=0))						//�յ�sim900��Ϣ
		{
			LCD_WriteString_en(0,3,"Check  Incoming");
			delay_s(1);
			LCD_WriteString_en(0,4,"NoMarching yet");
			Uart2SendStr("AT+CLCC\r");//����AT+CLCC/rȥ��ȡ�������	 
			delay_s(1);
			if (strstr(GsmRcv, "+CLCC: 1")!=0)
			{	
				strcpy(Gsmtmp,GsmRcv);	
				for (i=0;Listin[i]!='\0';i++)							
				{	
					LCD_WriteString_en(0,3,"Checking      ");
					LCD_WriteNumber(54,3,i+1);
					delay_s(1);
					if (strstr(Gsmtmp, Listin[i])!=0)	//������б���
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
						CleanGsmRcv();					//���GSM���ջ���
						break;
					}
				}
				Uart2SendStr("ATH\r");
				delay_s(2);	
				LCD_WriteString_en(0,3,"              ");
				LCD_WriteString_en(0,4,"              ");
			}
			CleanGsmRcv();//������ջ���
		}

	}
}
  
