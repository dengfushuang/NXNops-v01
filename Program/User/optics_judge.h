/******************************************************************************

                  ��Ȩ���� (C), 2001-2013, ���ֺ������ͨ�ż������޹�˾

 ******************************************************************************
  �� �� ��   : optics_judge.h
  �� �� ��   : ����
  ��    ��   : 123
  ��������   : 2013��4��26��
  ����޸�   :
  ��������   : optics_judge.c ��ͷ�ļ�
  �����б�   :
******************************************************************************/

#ifndef __OPTICS_JUDGE_H__
#define __OPTICS_JUDGE_H__
#include "main.h"

extern uint8_t Optics_Bypass_flag[CHANNEL_NUM];
extern uint8_t Autoflag[CHANNEL_NUM];
extern uint32_t BackAuto_Time_Tick[CHANNEL_NUM];


extern void optics_judge(uint8_t Link_num , uint8_t ch1 , uint8_t ch2);




#endif /* __OPTICS_JUDGE_H__ */
