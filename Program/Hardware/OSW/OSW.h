/******************************************************************************

                  ��Ȩ���� (C), 2001-2013, ���ֺ������ͨ�ż������޹�˾

 ******************************************************************************
  �� �� ��   : OSW.h
  �� �� ��   : ����
  ��    ��   :
  ��������   : 2013��5��31��
  ����޸�   :
  ��������   : OSW.C ��ͷ�ļ�
  �����б�   :
  �޸���ʷ   :
******************************************************************************/

#ifndef __OSW_H__
#define __OSW_H__

#include "main.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */


//���Ŷ���
//#define	 MostSwitch_For_All     LPC_GPIO4->CLR |=    //���е�ͨ�������л�����·
//#define	 BypassSwitch_For_All   LPC_GPIO4->SET |=    //���е�ͨ�������л�����·

//#define	 MostSwitch_For(ch)     LPC_GPIO4->CLR |= (1<<ch)
//#define	 BypassSwitch_For(ch)   LPC_GPIO4->CLR |= (1<<ch)

#define  SW_CON_PORT_INIT LPC_GPIO4->DIR|=(0X0FU<<23);LPC_GPIO4->DIR|=(0X0FU<<28)

#define  SW_CON1_H      LPC_GPIO1->SET|=(1U<<15)      //�⿪�������ſ��Ƶ�ƽ
#define  SW_CON1_L      LPC_GPIO1->CLR|=(1U<<15)
#define  SW_CON2_H      LPC_GPIO1->SET|=(1U<<16)
#define  SW_CON2_L      LPC_GPIO1->CLR|=(1U<<16)
#define  SW_CON3_H      LPC_GPIO3->SET|=(1U<<24)      //�⿪�������ſ��Ƶ�ƽ
#define  SW_CON3_L      LPC_GPIO3->CLR|=(1U<<24)
#define  SW_CON4_H      LPC_GPIO3->SET|=(1U<<25)
#define  SW_CON4_L      LPC_GPIO3->CLR|=(1U<<25)

#define  SW_CON5_H      LPC_GPIO4->SET|=(1U<<28)      //�⿪�������ſ��Ƶ�ƽ
#define  SW_CON5_L      LPC_GPIO4->CLR|=(1U<<28)
#define  SW_CON6_H      LPC_GPIO4->SET|=(1U<<29)
#define  SW_CON6_L      LPC_GPIO4->CLR|=(1U<<29)
#define  SW_CON7_H      LPC_GPIO4->SET|=(1U<<30)      //�⿪�������ſ��Ƶ�ƽ
#define  SW_CON7_L      LPC_GPIO4->CLR|=(1U<<30)
#define  SW_CON8_H      LPC_GPIO4->SET|=(1U<<31)
#define  SW_CON8_L      LPC_GPIO4->CLR|=(1U<<31)

#define  SW_CON9_H      LPC_GPIO2->SET|=(1U<<22)      //�⿪�������ſ��Ƶ�ƽ
#define  SW_CON9_L      LPC_GPIO2->CLR|=(1U<<22)
#define  SW_CON10_H     LPC_GPIO2->SET|=(1U<<23)
#define  SW_CON10_L     LPC_GPIO2->CLR|=(1U<<23)
#define  SW_CON11_H     LPC_GPIO2->SET|=(1U<<24)      //�⿪�������ſ��Ƶ�ƽ
#define  SW_CON11_L     LPC_GPIO2->CLR|=(1U<<24)
#define  SW_CON12_H     LPC_GPIO2->SET|=(1U<<25)
#define  SW_CON12_L     LPC_GPIO2->CLR|=(1U<<25)

#define  SW_CON13_H     LPC_GPIO3->SET|=(1U<<23)      //�⿪�������ſ��Ƶ�ƽ
#define  SW_CON13_L     LPC_GPIO3->CLR|=(1U<<23)
#define  SW_CON14_H     LPC_GPIO3->SET|=(1U<<24)
#define  SW_CON14_L     LPC_GPIO3->CLR|=(1U<<24)
#define  SW_CON15_H     LPC_GPIO3->SET|=(1U<<25)      //�⿪�������ſ��Ƶ�ƽ
#define  SW_CON15_L     LPC_GPIO3->CLR|=(1U<<25)
#define  SW_CON16_H     LPC_GPIO3->SET|=(1U<<26)
#define  SW_CON16_L     LPC_GPIO3->CLR|=(1U<<26)


extern void BypassSwitch(uint8 ch);
extern void MostSwitch(uint8 ch);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __OSW_H__ */
