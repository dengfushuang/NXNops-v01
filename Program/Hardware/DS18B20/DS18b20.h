#ifndef __DS18B20_H
#define __DS18B20_H 


#include "main.h"


#define  DQ        ((LPC_GPIO0->PIN>>31)&0x01) //DS18B20��DQ����
#define  DQ_OUT    LPC_GPIO0->DIR |= (1u<<31)   //��DQ����Ϊ���
#define  DQ_IN     LPC_GPIO0->DIR &= ~(1u<<31)  //��DQ����Ϊ����
#define  DQ_H      LPC_GPIO0->SET|=(1u<<31)     //����DQ��ƽ
#define  DQ_L      LPC_GPIO0->CLR|=(1u<<31)     //����DQ��ƽ

extern uint8 T_18b20flag;
   	
uint8 DS18B20_Reset(void);
void DS18B20_Write(uint8 cmd);
uint8 DS18B20_Read(void);
uint16 DS18B20_GetTemperature(uint8 T);
void DS18B20_Alarm(float Threshold_T,uint8 T);
extern uint16 DS18B20_Get_Tmp(void);
 
#endif

