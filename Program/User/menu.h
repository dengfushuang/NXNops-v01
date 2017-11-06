/******************************************************************************

                  ��Ȩ���� (C), 2001-2013, ���ֺ������ͨ�ż������޹�˾

 ******************************************************************************
  �� �� ��   : menu.h
  �� �� ��   : ����
  ��    ��   : 123
  ��������   : 2013��4��22��
  ����޸�   :
  ��������   : menu.c ��ͷ�ļ�
  �����б�   :
  �޸���ʷ   :
  1.��    ��   : 2013��4��22��
    ��    ��   : 123
    �޸�����   : �����ļ�

******************************************************************************/

#ifndef __MENU_H__
#define __MENU_H__


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#define Up 					1
#define Up_Long 		    2
#define Dwon				3
#define Dwon_Long			4
#define Enter				5
#define Enter_Long			6
#define Up_And_Dwon			7
#define Dwon_Long_long      8


extern unsigned char FirstLineDisIndex ;   //��Ļ��һ����ʾ��������
extern unsigned char SelectLine;           //��ǰѡ�����
extern unsigned char SelectIndex;          //��ǰѡ���ж�Ӧ��������
extern unsigned char LastIndex;            //���빦�ܺ���ǰ�������ţ��жϾ��幦��ʹ��
extern unsigned char Enter_Function;       //���빦�ܺ����ı�ʾ
extern unsigned char Enter_counter;        //��Enter�������ļ�������

extern void DISlm16032 (void);
extern void Function1(void);
extern void FunctionAction (void);
extern void show_Set_ok(void);
extern void KeyCodeAction (unsigned char KeyCode);
extern uint8 MenuCancelOption (void);
extern void MenuDownOneOption (void);
extern void MenuEnterOption (void);
extern unsigned char MenuLevelEnd (void);
extern unsigned char MenuLevelStart (void);
extern void MenuUpOneOption (void);
extern void NoThisFunction (void);
extern void showsuccess(void);
extern void Goto_mian_menu(void);
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __MENU_H__ */
