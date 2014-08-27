#ifndef __SIM900_H
#define __SIM900_H
#include "sys.h"


extern u8 numin[12];
void  sim900_ini(void); 

void  SendSMS(u8 *str, u8 *cellnumber);

void  SendSMS_start(u8 *cellnumber);
void  SendSMS_alert(u8 level, u8 *cellnumber);
void  SendSMS_query(u8 *cellnumber);
void  SendSMS_recover(u8 *cellnumber);
void  SendSMS_alertclr (u8 *cellnumber);

void  CallYou(u8 *cellnumber);
void  hang (void);
void  Getnumber(void);
 
#endif
