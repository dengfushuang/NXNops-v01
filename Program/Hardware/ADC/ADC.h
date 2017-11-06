/******************************************************************************

                  ��Ȩ���� (C), 2001-2013, ���ֺ������ͨ�ż������޹�˾

 ******************************************************************************
  �� �� ��   : ADC.h
  �� �� ��   : ����
  ��    ��   : 123
  ��������   : 2013��4��19��
  ����޸�   :
  ��������   : ADC.c ��ͷ�ļ�
  �����б�   :
******************************************************************************/

#ifndef __ADC_H__
#define __ADC_H__
#include"main.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#define ADC_INIT    LPC_GPIO4->DIR|=(0XFFFU<<11);LPC_GPIO3->DIR|=0X3FU;\
                    LPC_GPIO3->DIR|=(0X3FU<<8);LPC_GPIO1->DIR|=(0XFFFU<<18);\
                    LPC_GPIO4->DIR|=0X07U;LPC_GPIO0->DIR|=(0X07U<<29);\
                    LPC_GPIO2->DIR|=(0X3FU<<26)  
                              
#define CHANNEL_PIN0(X)   LPC_GPIO0->CLR|=(7<<17);LPC_GPIO0->SET|=(X<<17)  //AD ��ȡ�������ƽ�
#define CHANNEL_PIN1(X)   LPC_GPIO0->CLR|=(7<<20);LPC_GPIO0->SET|=(X<<20)  //AD ��ȡ�������ƽ�
//#define CHANNEL_PIN2(X)   LPC_GPIO4->CLR|=(7<<17);LPC_GPIO4->SET|=(X<<17)  //AD ��ȡ�������ƽ�
//#define CHANNEL_PIN3(X)   LPC_GPIO4->CLR|=(7<<20);LPC_GPIO4->SET|=(X<<20)  //AD ��ȡ�������ƽ�
//#define CHANNEL_PIN4(X)   LPC_GPIO3->CLR|=(7<<11);LPC_GPIO3->SET|=(X<<11)  //AD ��ȡ�������ƽ�
//#define CHANNEL_PIN5(X)   LPC_GPIO3->CLR|=(7<<3); LPC_GPIO3->SET|=(X<<3)   //AD ��ȡ�������ƽ�
//#define CHANNEL_PIN6(X)   LPC_GPIO3->CLR|=7;      LPC_GPIO3->SET|=(X)      //AD ��ȡ�������ƽ�
//#define CHANNEL_PIN7(X)   LPC_GPIO3->CLR|=(7<<8); LPC_GPIO3->SET|=(X<<8)   //AD ��ȡ�������ƽ�

//#define CHANNEL_PIN8(X)    LPC_GPIO1->CLR|=(7<<27);LPC_GPIO1->SET|=(X<<27)  //AD ��ȡ�������ƽ�
//#define CHANNEL_PIN9(X)    LPC_GPIO1->CLR|=(7<<24);LPC_GPIO1->SET|=(X<<24)  //AD ��ȡ�������ƽ�
//#define CHANNEL_PIN10(X)   LPC_GPIO1->CLR|=(7<<21);LPC_GPIO1->SET|=(X<<21)  //AD ��ȡ�������ƽ�
//#define CHANNEL_PIN11(X)   LPC_GPIO1->CLR|=(7<<18);LPC_GPIO1->SET|=(X<<18)  //AD ��ȡ�������ƽ�
//#define CHANNEL_PIN12(X)   LPC_GPIO4->CLR|=7;      LPC_GPIO4->SET|=(X)      //AD ��ȡ�������ƽ�
//#define CHANNEL_PIN13(X)   LPC_GPIO0->CLR|=(7<<29);LPC_GPIO0->SET|=(X<<29)  //AD ��ȡ�������ƽ�
//#define CHANNEL_PIN14(X)   LPC_GPIO2->CLR|=(7<<26);LPC_GPIO2->SET|=(X<<26)  //AD ��ȡ�������ƽ�
//#define CHANNEL_PIN15(X)   LPC_GPIO2->CLR|=(7<<29);LPC_GPIO2->SET|=(X<<29)  //AD ��ȡ�������ƽ�

extern float power_warn[];
extern uint8 hardware_way[];
extern float power[];  
extern float power_count[]; 
extern float test_power[];
extern uint8 warn[];			//�澯״̬
 
 
extern void adcdeal(uint8 RTnum,uint32 way, float adc);
extern void ADC_int(uint32 rate);
extern uint32 addo(uint8 adnum);
extern void CHANNEL(uint8 RTnum, uint8 way_temp);
extern void optics_collect( uint8 RTnum ,uint8 way);
extern void log114_adcdeal(uint8 ADnum ,uint32 adc);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __ADC_H__ */
