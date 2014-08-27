 #ifndef __5110LCD_H
 #define __5110LCD_H

 #include "stm32f10x.h"	
///////////////////////////////////////////////////////
//------------------------------------/				 //
// SCE    ------------PA5  //Ƭѡ					 //
// RST    ------------PA4  //λѡ					 //
//  D/C    ------------PA3  // 1д���ݣ�0дָ��		 //
// SD     ------------PA2	   //����				 //
// SCLK   ------------PA1	   //ʱ��				 //
// LCD    ------------PA0      //����				 //
///////////////////////////////////////////////////////
  /*
  PB9-RST
  PB8-CE
  PB7-DC
  PB6-DIN
  PB5-CLK
  PB2-LED
  */
 #define sce0   GPIO_ResetBits(GPIOB, GPIO_Pin_8)  //Ƭѡ
 #define res0   GPIO_ResetBits(GPIOB, GPIO_Pin_9)  //��λ,0��λ
 #define dc0    GPIO_ResetBits(GPIOB, GPIO_Pin_7)   //1д���ݣ�0дָ��
 #define sdin0  GPIO_ResetBits(GPIOB, GPIO_Pin_6)   //����
 #define sclk0  GPIO_ResetBits(GPIOB, GPIO_Pin_5)   //ʱ��


 #define sce1   GPIO_SetBits(GPIOB, GPIO_Pin_8)  //Ƭѡ
 #define res1   GPIO_SetBits(GPIOB, GPIO_Pin_9)  //��λ,0��λ
 #define dc1    GPIO_SetBits(GPIOB, GPIO_Pin_7)   //1д���ݣ�0дָ��
 #define sdin1  GPIO_SetBits(GPIOB, GPIO_Pin_6)   //����
 #define sclk1  GPIO_SetBits(GPIOB, GPIO_Pin_5)   //ʱ��
	    
void GPIO_Configuration_LCD(void);
void LCD_WriteByte(unsigned char dt, unsigned char command);
void LCD_Init(void);
void LCD_Set_XY(unsigned char X, unsigned char Y);
void LCD_Clear(void);
void LCD_WriteChar(unsigned char X,unsigned char Y,unsigned char c);
void LCD_WriteString_en(unsigned char X,unsigned char Y,unsigned char *s);
void LCD_WriteNumber(unsigned char x,unsigned char y, unsigned long number);
void LCD_write_chinese_string(unsigned char X, unsigned char Y,
                              unsigned char ch_with,unsigned char num,
							  unsigned char line,unsigned char row);
void LCD_Write_cn(unsigned char row, unsigned char page,unsigned char c); //row:�� page:ҳ dd:�ַ�

//void delay_ms(int t);
#endif
