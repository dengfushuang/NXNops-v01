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


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

extern uint8 Optics_Bypass_flag[CHANNEL_NUM];

extern uint8 Autoflag[CHANNEL_NUM];              
extern uint32 BackAuto_Time_Tick[CHANNEL_NUM];
extern uint32 BackSW_Time_Tick[CHANNEL_NUM];

extern void optics_judge(uint8 Link_num , uint8 ch1 ,uint8 ch2 );

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __OPTICS_JUDGE_H__ */
