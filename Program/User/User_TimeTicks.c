/******************************************************************************

                  ��Ȩ���� (C), 2001-2013, ���ֺ������ͨ�ż������޹�˾

 ******************************************************************************
  �� �� ��   : User_TimeTicks.c
  �� �� ��   : ����
  ��    ��   : 
  ��������   : 2013��5��7��
  ����޸�   :
  ��������   :  
  �����б�   :
              User_TimeTicks
******************************************************************************/
#include "includes.h"
#include "clock-arch.h"

extern uint32  BLCD_TimeTicks;
extern uint8   ICMP_time_en[CHANNEL_NUM];
extern uint16  ICMP_time_couter[CHANNEL_NUM];
extern uint32  BackOSW_Time_Tick[CHANNEL_NUM];
extern uint32  BackAuto_Time_Tick[CHANNEL_NUM];
extern uint32  BH_Tick[CHANNEL_NUM];

void  User_TimeTicks(void)
{

}





