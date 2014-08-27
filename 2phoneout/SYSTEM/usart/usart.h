#ifndef __USART_H
#define __USART_H
#include "stdio.h"
#include "string.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//Mini STM32������
//����1��ʼ��		   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2010/5/27
//�汾��V1.3
//��Ȩ���У�����ؾ���
//Copyright(C) ����ԭ�� 2009-2019
//All rights reserved
//********************************************************************************
//V1.3�޸�˵�� 
//֧����Ӧ��ͬƵ���µĴ��ڲ���������.
//�����˶�printf��֧��
//�����˴��ڽ��������.
//������printf��һ���ַ���ʧ��bug
//////////////////////////////////////////////////////////////////////////////////
#define UART1_DEBUG 
#define MAXRCV 255
#define MAXBUF 255

extern u8 GsmAtFlag;   		// ��ʾ����2�յ�AT (GSM)����.
extern u8 GsmRcvAt[MAXRCV]; 
extern u8 GsmRcv[MAXRCV];
extern u16 GsmRcvCnt;
extern u16 Debug1RcvCnt;
extern u8 DebugBuf_U1[MAXBUF];
extern u8 DebugBuf_U2[MAXBUF];
extern u16 Debug2RcvCnt;

	

void Uart1Init(u32 bound);
void Uart1SendHex(u8 ch);
void Uart1SendStr(u8* str);
void Uart2Init(u32 bound);
void Uart2SendHex(u8 ch);
void Uart2SendStr(u8* str);
void Uart3Init(u32 bound);
void Uart3SendHex(u8 ch);
void Uart3SendStr(u8* str);
void CleanGsmRcv(void);


u8 Hand(unsigned char *a);

#endif
