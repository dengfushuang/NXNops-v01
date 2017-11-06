/******************************************************************************

                  版权所有 (C), 2001-2013, 桂林恒毅金宇通信技术有限公司

 ******************************************************************************
  文 件 名   : main.c
  版 本 号   : 初稿
  作    者   :
  生成日期   : 2017年8月7日
  最近修改   :
  功能描述   :
******************************************************************************/
#include "includes.h"

//********************************************************************************
//************************************全局变量定义*******************************
//********************************************************************************
            //模块软件版本号
const char SVersion[]="OS[01]-HW[V01.02.03]-HDATE[2015.09.01]-SW[V01.02.03]-SDATE[2017.10.23]";
uint8  cfm[20];                    //确认buf
uint8  run_flag;                   //LED 运行灯
uint8  scan_flag;                  //扫描标志
uint16 srch;                       //扫描实时通道
uint8  stime;                      //扫描时间
uint8  ADDR;                       //业务盘地址
uint8  OPS_CH;                     //光开关通道数

struct EPROM_DATA EPROM;           //保存EPROM设置参数的结构体
#ifdef USE_DEV_UART0
    uint8  uart0RcvBuf[UART_RCV_BUF_LEN];              //任务TaskUart0Revice 用的数组,uart0接收数据数组
    OS_EVENT *Uart0RcvMbox;            //串口 0 接收邮箱
    OS_STK TaskUart0CmdStk[TASK_STK_SIZE*2];
    OS_STK TaskUart0RcvStk[TASK_STK_SIZE*2];
#endif

#ifdef USE_DEV_UART1
    uint8  uart1RcvBuf[UART_RCV_BUF_LEN];           //uart1接受数据数组
    OS_EVENT *Uart1RcvMbox;            //串口 1 接收邮箱
    OS_STK TaskUart1CmdStk[TASK_STK_SIZE*2];
    OS_STK TaskUart1RcvStk[TASK_STK_SIZE*2];
#endif

#ifdef USE_DEV_UART2
    uint8  uart2RcvBuf[UART_RCV_BUF_LEN];           //uart2接受数据数组
    OS_EVENT *Uart2RcvMbox;            //串口2接收邮箱
    OS_STK TaskUart2CmdStk[TASK_STK_SIZE*2];
    OS_STK TaskUart2RcvStk[TASK_STK_SIZE*2];
#endif

#ifdef USE_DEV_ADC           //ADC接受数据
    OS_STK TaskADCRcvStk[TASK_STK_SIZE*2];
#endif

OS_STK TaskSvrStk[TASK_STK_SIZE*2]; 
OS_STK TaskWDTStk[TASK_STK_SIZE];

#ifdef USE_RUN_LED
    OS_STK TaskRunLEDStk[TASK_STK_SIZE];
#endif

/********************************************************************************************************
** 函数名称: restore_set()
** 功能描述: 出厂默认设置
********************************************************************************************************/
void restore_set(void)
{
	int i;
    EPROM.BPS = 9;            //串口波特率(115200)
	for(i=0;i<24;i++)
	{
		EPROM.Channel[i]=0;
	}
    //再给EEPROM初始化,防止EEPROM出问题,无法保存
    LPC1778_EEPROM_Init();
}

/********************************************************************************************************
** 函数名称: systemInt
** 功能描述: 初始化
********************************************************************************************************/
void READ_EPROM_Init(void)
{
	int i;
    LPC1778_EEPROM_Init();     //EEPROM 初始化
    for(i = 0 ; i < MAX_CHANNEL ; i++)
	{
		EPROM.Channel[i] = 0;
	}
    EEPROM_Read_Str( 0x00, (uint8 *)&EEPROM_BASE_ADDRESS, sizeof(struct EPROM_DATA) );
    delay_nms(20);
    //再读一次防止出现误码
    EEPROM_Read_Str( 0x00, (uint8 *)&EEPROM_BASE_ADDRESS, sizeof(struct EPROM_DATA) );
	  
	  EPROM.BPS = 3; //串口波特率(115200)
	  stime = 1;
}

/********************************************************************************************************
** 函数名称: HW_Init
** 功能描述: 硬件初始化
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
       //再读一次防止出现误码
        EEPROM_Read_Str( 0x00, (uint8 *)&EEPROM_BASE_ADDRESS, sizeof(struct EPROM_DATA) );
		ADDR = ((LPC_GPIO4->PIN>>0) & 0x1f) + 1;
	
    /*******************开启看门狗********************/
		/**LPC1778的看门狗使用内部RC时钟(500KHz),经过4次分频(500K/4=125K ,即十六进制为0X1E848)**/
		LPC_WDT->TC  = 0X1E848;    //设置WDT定时值为1秒.
		LPC_WDT->MOD = 0x03;       //设置WDT工作模式,启动WDT	
	
		LPC_GPIO4->CLR |= (1<<5);
		
	
//		/* 等待主控响应 */
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
** 函数名称: TaskInfo
** 功能描述: 发送状态
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
** 函数名称: TaskWDT
** 功能描述: 看门狗复位
** 说  明: 看门狗使用内部RC时钟(4MHz),经过4次分频。T=0x1000000*1us=1s (看门狗复位定时时间)
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
** 函数名称: TaskInfo
** 功能描述: 发送状态
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
** 函数名称: main
** 功能描述: 主函数
********************************************************************************************************/
int main (void)
{
		SystemInit();
		HW_Init();
	    UART0Put_str("os runing=====\r\n",14);
		IntDisAll();  //Note:由于使用UCOS, 在OS运行之前运行,注意别使能任何中断.
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

		//建立串口0的接收邮箱
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
