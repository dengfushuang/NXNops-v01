///******************************************************************************

//                  ��Ȩ���� (C), 2001-2013, ���ֺ������ͨ�ż������޹�˾

// ******************************************************************************
//  �� �� ��   : main.h
//  �� �� ��   : ����
//  ��    ��   :  
//  ��������   : 2013��8��20��

//******************************************************************************/
//#include "includes.h"
//#include "timer.h"

//#ifndef __MAIN_H 
//#define __MAIN_H 

//#ifdef __cplusplus
//extern "C" {
//#endif

///******************************************************************************
//  �Զ�����±���
//*******************************************************************************/
//typedef unsigned char  uint8;                   /* defined for unsigned 8-bits integer variable 	�޷���8λ���ͱ���  */
//typedef signed   char  int8;                    /* defined for signed 8-bits integer variable		  �з���8λ���ͱ���  */
//typedef unsigned short uint16;                  /* defined for unsigned 16-bits integer variable 	�޷���16λ���ͱ��� */
//typedef signed   short int16;                   /* defined for signed 16-bits integer variable 		�з���16λ���ͱ��� */
//typedef unsigned int   uint32;                  /* defined for unsigned 32-bits integer variable 	�޷���32λ���ͱ��� */
//typedef signed   int   int32;                   /* defined for signed 32-bits integer variable 		�з���32λ���ͱ��� */
//typedef float          fp32;                    /* single precision floating point variable (32bits) �����ȸ�������32λ���ȣ� */
//typedef double         fp64;                    /* double precision floating point variable (64bits) ˫���ȸ�������64λ���ȣ� */

///*********************************************************************************************************
//  �忨����
//*********************************************************************************************************/
//#define TYPE_FSW
//#define FSW    0
//#define OTDR   1
//#define WDM    2
//#define OPM    3
//#define OS     4

///*********************************************************************************************************
//  TASK STACK SIZES  �����ջ��С
//*********************************************************************************************************/
//#define  TASK_STK_SIZE                128

///*********************************************************************************************************
//  ���Ŷ���
//*********************************************************************************************************/
//#ifdef   TYPE_OPM
//#define  RUN_LED_INIT    LPC_GPIO1->DIR |= (1u<<31)
//#define  RUN_LED_H       LPC_GPIO1->SET |= (1u<<31)
//#define  RUN_LED_L       LPC_GPIO1->CLR |= (1u<<31)
//#else
//#define  RUN_LED_INIT    LPC_GPIO3->DIR |= (1<<2)
//#define  RUN_LED_H       LPC_GPIO3->SET |= (1<<2)
//#define  RUN_LED_L       LPC_GPIO3->CLR |= (1<<2)
//#endif

//#define  ONLINE_PIN_INIT LPC_GPIO5->DIR |= (1<<4)
//#define  ONLINE          LPC_GPIO5->CLR |= (1<<4)
//#define  OFFLINE         LPC_GPIO5->SET |= (1<<4)

//#define  SW_CON_INIT     LPC_GPIO2->DIR |= (2<<5)
//#define  SW_CON1_H       LPC_GPIO2->SET |= (1<<5)
//#define  SW_CON1_L       LPC_GPIO2->CLR |= (1<<5)
//#define  SW_CON2_H       LPC_GPIO2->CLR |= (1<<6)
//#define  SW_CON2_L       LPC_GPIO2->CLR |= (1<<6)

//#define  OPS_PIN_INIT    LPC_GPIO4->DIR |= (0x1ff<<5) 
//#define  OPS_RST         LPC_GPIO4->CLR |= (1<<12)
//#define  OPS_NRST        LPC_GPIO4->SET |= (1<<12)

//#define  OTDR_INIT       LPC_GPIO2->DIR |= (1<<9);LPC_GPIO5->DIR |= (1<<0)
//#define  OTDR_PWR_ON     LPC_GPIO5->SET |= (1<<0)       
//#define  OTDR_PWR_OFF    LPC_GPIO5->CLR |= (1<<0)

//#define  CHANNEL_NUM       1            //��·���궨��
//#define  LOG_ADDR          1000         //��־�洢��ַ
//#define  EEPROM_BASE_ADDRESS  EPROM.BPS //flashģ��EPROM����ַ

//__packed struct EPROM_DATA {
//	
//		uint8  BPS;                  //���ڲ�����  1:2400 2:4800 3:9600 4:14400 5:19200 6:38400 7:56000 8:57600 9:115200
//		
//	  uint8  TYPE;                 //ҵ��������: 0 FSW, 1 OTDR, 2 WDM, 3 OPM
//		char   MN[9];                //ҵ���̻�����
//		uint16 maxch;                //���ͨ����
//	  uint16 sbch;                 //ɨ����ʼͨ��
//	  uint16 sech;                 //ɨ�����ͨ��
//};

//extern const  char  SVersion[];  //ģ������汾��
//extern const  char  type[][5];
//extern uint8  u2RcvBuf[];
//extern uint8  cfm[];
//extern uint8  ADDR;
//extern uint8  OPS_CH;
//extern uint8  scan_flag;
//extern uint8  stime;
//extern struct EPROM_DATA  EPROM;

//extern void   restore_set(void);             //�ָ��������ú���
//extern void   Reset_Handler(void);           //�����λ����

//#ifdef __cplusplus
//}
//#endif

//#endif
///*********************************************************************************************************
//  END FILE 
//*********************************************************************************************************/

