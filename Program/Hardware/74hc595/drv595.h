#ifndef __DRV595_H
#define __DRV595_H

#include "LPC177x_8x.h"

#define  HC595_PORT_INIT    LPC_GPIO0->DIR|= (5<<19)|(1<<22) 

#define  HC595_SER_H		LPC_GPIO0->SET|=(1<<22)     	
#define  HC595_SER_L	    LPC_GPIO0->CLR|=(1<<22)

#define  HC595_RCLK_H       LPC_GPIO0->SET|=(1<<20)     
#define  HC595_RCLK_L	    LPC_GPIO0->CLR|=(1<<20) 

#define  HC595_SCLK_H		LPC_GPIO0->SET|=(1<<19)    	
#define  HC595_SCLK_L	    LPC_GPIO0->CLR|=(1<<19)

//#define  HC595_GL_SET     LPC_GPIO1->SET|=(1<<24)     	
//#define  HC595_GL_CLR		LPC_GPIO1->CLR|=(1<<24)

/*************************************************************************************************************
*�������ƣ�HC595_Shift()
*��   �� ��uint8_t dat
*����ֵ  : void
*������������74HC595����һ���ֽں���������
**************************************************************************************************************/
extern void HC595_Shift(uint16_t  dat);
extern void ChannelLED(uint8_t i ,uint8_t w);	 //iͨ����w��ֵ0����1�죬2��

#endif /*����*/
