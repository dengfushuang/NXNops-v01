/******************************************************************************

                  ��Ȩ���� (C), 2001-2013, ���ֺ������ͨ�ż������޹�˾

 ******************************************************************************
  �� �� ��   : main.c
  �� �� ��   : ����
  ��    ��   :
  ��������   : 2017��8��7��
  ����޸�   :
  ��������   :
******************************************************************************/
#include "includes.h"

//********************************************************************************
//************************************ȫ�ֱ�������*******************************
//********************************************************************************
            //ģ������汾��
const char SVersion[]="OS[01]-HW[V01.02.03]-HDATE[2015.09.01]-SW[V01.02.03]-SDATE[2017.10.23]";
uint8  cfm[20];                    //ȷ��buf
uint8  run_flag;                   //LED ���е�
uint8  scan_flag;                  //ɨ���־
uint16 srch;                       //ɨ��ʵʱͨ��
uint8  stime;                      //ɨ��ʱ��
uint8  ADDR;                       //ҵ���̵�ַ
uint8  OPS_CH;                     //�⿪��ͨ����

struct EPROM_DATA EPROM;           //����EPROM���ò����Ľṹ��
#ifdef USE_DEV_UART0
    uint8  uart0RcvBuf[UART_RCV_BUF_LEN];              //����TaskUart0Revice �õ�����,uart0������������
    OS_EVENT *Uart0RcvMbox;            //���� 0 ��������
    OS_STK TaskUart0CmdStk[TASK_STK_SIZE*2];
    OS_STK TaskUart0RcvStk[TASK_STK_SIZE*2];
#endif

#ifdef USE_DEV_UART1
    uint8  uart1RcvBuf[UART_RCV_BUF_LEN];           //uart1������������
    OS_EVENT *Uart1RcvMbox;            //���� 1 ��������
    OS_STK TaskUart1CmdStk[TASK_STK_SIZE*2];
    OS_STK TaskUart1RcvStk[TASK_STK_SIZE*2];
#endif

#ifdef USE_DEV_UART2
    uint8  uart2RcvBuf[UART_RCV_BUF_LEN];           //uart2������������
    OS_EVENT *Uart2RcvMbox;            //����2��������
    OS_STK TaskUart2CmdStk[TASK_STK_SIZE*2];
    OS_STK TaskUart2RcvStk[TASK_STK_SIZE*2];
#endif

#ifdef USE_DEV_ADC           //ADC��������
    OS_STK TaskADCRcvStk[TASK_STK_SIZE*2];
#endif

OS_STK TaskSvrStk[TASK_STK_SIZE*2]; 
OS_STK TaskWDTStk[TASK_STK_SIZE];

#ifdef USE_RUN_LED
    OS_STK TaskRunLEDStk[TASK_STK_SIZE];
#endif

/********************************************************************************************************
** ��������: restore_set()
** ��������: ����Ĭ������
********************************************************************************************************/
void restore_set(void)
{
	int i;
    EPROM.BPS = 9;            //���ڲ�����(115200)
	for(i=0;i<24;i++)
	{
		EPROM.Channel[i]=0;
	}
    //�ٸ�EEPROM��ʼ��,��ֹEEPROM������,�޷�����
    LPC1778_EEPROM_Init();
}

/********************************************************************************************************
** ��������: systemInt
** ��������: ��ʼ��
********************************************************************************************************/
void READ_EPROM_Init(void)
{
	int i;
    LPC1778_EEPROM_Init();     //EEPROM ��ʼ��
    for(i = 0 ; i < MAX_CHANNEL ; i++)
	{
		EPROM.Channel[i] = 0;
	}
    EEPROM_Read_Str( 0x00, (uint8 *)&EEPROM_BASE_ADDRESS, sizeof(struct EPROM_DATA) );
    delay_nms(20);
    //�ٶ�һ�η�ֹ��������
    EEPROM_Read_Str( 0x00, (uint8 *)&EEPROM_BASE_ADDRESS, sizeof(struct EPROM_DATA) );
	  
	  EPROM.BPS = 3; //���ڲ�����(115200)
	  stime = 1;
}

/********************************************************************************************************
** ��������: HW_Init
** ��������: Ӳ����ʼ��
********************************************************************************************************/
void HW_Init(void)
{	
		ONLINE_PIN_INIT;
	    OFFLINE;
	    RUN_LED_INIT;
		OTDR_INIT;
	    OTDR_PWR_ON;
		OPS_PIN_INIT;
		OPS_RST;
		READ_EPROM_Init();
		UART0Init();
	
#ifdef  USE_DEV_UART1
		UART1_Init(115200);
#endif
	
#ifdef  USE_DEV_UART2
		UART2_Init(115200);
#endif
  	    ONLINE;
	 	OPS_NRST;
	    delay_nms(700);
	    EEPROM_Read_Str( 0x00, (uint8 *)&EEPROM_BASE_ADDRESS, sizeof(struct EPROM_DATA) );
		delay_nms(20);
       //�ٶ�һ�η�ֹ��������
        EEPROM_Read_Str( 0x00, (uint8 *)&EEPROM_BASE_ADDRESS, sizeof(struct EPROM_DATA) );
		ADDR = ((LPC_GPIO4->PIN>>0) & 0x1f) + 1;
	
    /*******************�������Ź�********************/
		/**LPC1778�Ŀ��Ź�ʹ���ڲ�RCʱ��(500KHz),����4�η�Ƶ(500K/4=125K ,��ʮ������Ϊ0X1E848)**/
		LPC_WDT->TC  = 0X1E848;    //����WDT��ʱֵΪ1��.
		LPC_WDT->MOD = 0x03;       //����WDT����ģʽ,����WDT	
	
		LPC_GPIO4->CLR |= (1<<5);
		
	
//		/* �ȴ�������Ӧ */
//	  for(i=0; i<3; i++)
//    {
//		    while((buf[i] = UART2Get()) == 0)
//					  continue;
//		}
//		if( ((buf[0] - '0')*10 + (buf[1] - '0')) != ADDR )
//				Reset_Handler();
//		sprintf((char *)cfm, "<%02d_%01u_%s>\n", ADDR, EPROM.TYPE, EPROM.MN);
//		UART1Write_Str((uint8 *)cfm);
}

/********************************************************************************************************
** ��������: TaskInfo
** ��������: ����״̬
********************************************************************************************************/
void scan(void)
{
	  uint8  i, temp;
	
		if(scan_flag)
		{
			begin:
			  if(scan_flag == 1)
				{
					for(i=0; i<7; i++)
					{
							temp = ((EPROM.sbch - 1) >> i) & 0x01;
							if(temp)
									LPC_GPIO4->SET |= (1<<(5+i));
							else
									LPC_GPIO4->CLR |= (1<<(5+i));
					}
					srch = EPROM.sbch;
					OPS_CH = EPROM.sbch - 1;
					scan_flag = 2;
				}
				else
				{
					  srch++;
					  if(EPROM.sech > EPROM.sbch)
						{
								if(srch > EPROM.sech)
								{
										scan_flag = 1;
										goto begin;
								}
						}
						else if(EPROM.sech < EPROM.sbch)
						{
								if(srch > EPROM.maxch)
										srch = 1;
								if((srch > EPROM.sech) && (srch < EPROM.sbch))
								{
										scan_flag = 1;
										goto begin;
								}
						}
						else 
						{
								scan_flag = 1;
								goto begin;
						}
						OPS_CH = srch - 1;
						for(i=0; i<7; i++)
						{
								temp = ((srch - 1) >> i) & 0x01;
								if(temp)
										LPC_GPIO4->SET |= (1<<(5+i));
								else
										LPC_GPIO4->CLR |= (1<<(5+i));
						}
				}
		}
}

/********************************************************************************************************
** ��������: TaskWDT
** ��������: ���Ź���λ
** ˵  ��: ���Ź�ʹ���ڲ�RCʱ��(4MHz),����4�η�Ƶ��T=0x1000000*1us=1s (���Ź���λ��ʱʱ��)
********************************************************************************************************/
void TaskWDT(void *pdata)
{
		while(1)
		{
				LPC_WDT->FEED = 0xAA;
				LPC_WDT->FEED = 0x55;
				
				if(run_flag)
				{
						RUN_LED_H;
						run_flag = 0;
				}
				else
				{
						RUN_LED_L;
						run_flag = 1;
				}
				OSTimeDly(500);
		}
}

/********************************************************************************************************
** ��������: TaskInfo
** ��������: ����״̬
********************************************************************************************************/
void TaskSvr(void* pdata)
{
		while(1)
		{
			  scan();
				OSTimeDly(100 * stime);
		}
}


/*******************************************************************************************************
** ��������: main
** ��������: ������
********************************************************************************************************/
int main (void)
{
		SystemInit();
		HW_Init();
	    UART0Put_str("os runing=====\r\n",14);
		IntDisAll();  //Note:����ʹ��UCOS, ��OS����֮ǰ����,ע���ʹ���κ��ж�.
		OSInit();
		OS_CPU_SysTickInit(SystemCoreClock/OS_TICKS_PER_SEC);

		OSTaskCreate(TaskWDT,      (void *)0, &TaskWDTStk[TASK_STK_SIZE - 1],        1);
	    //OSTaskCreate(TaskSvr,      (void *)0, &TaskSvrStk[TASK_STK_SIZE*2 - 1],      2);
#ifdef USE_DEV_UART0
	    OSTaskCreate(TaskUart0Cmd, (void *)0, &TaskUart0CmdStk[TASK_STK_SIZE*2 - 1], 3);
		OSTaskCreate(TaskUart0Rcv, (void *)0, &TaskUart0RcvStk[TASK_STK_SIZE*2 - 1], 4);
#endif
	
#ifdef USE_DEV_UART1
	    OSTaskCreate(TaskUart1Rcv, (void *)0, &TaskUart1RcvStk[TASK_STK_SIZE*2 - 1], 5);
	    OSTaskCreate(TaskUart1Cmd, (void *)0, &TaskUart1CmdStk[TASK_STK_SIZE*2 - 1], 6);
#endif

		//��������0�Ľ�������
#ifdef USE_DEV_UART0
		Uart0RcvMbox = OSMboxCreate(NULL);
		if(Uart0RcvMbox == NULL)
			  return 1;
#endif		
		OSStart();
		return 0;
}

/********************************************************************************************************
**                            End Of File
********************************************************************************************************/
