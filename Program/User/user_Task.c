#include "user_Task.h"


#ifdef USE_DEV_UART0
extern uint8  uart0RcvBuf[120];              //任务TaskUart0Revice 用的数组,uart0接收数据数组
extern OS_EVENT *Uart0RcvMbox;            //串口 0 接收邮箱
extern OS_STK TaskUart0CmdStk[TASK_STK_SIZE*2];
extern OS_STK TaskUart0RcvStk[TASK_STK_SIZE*2];
#endif

#ifdef USE_DEV_UART1
extern  uint8  uart1RcvBuf[120];           //uart1接受数据数组
extern  OS_EVENT *Uart1RcvMbox;            //串口 1 接收邮箱
extern  OS_STK TaskUart1CmdStk[TASK_STK_SIZE*2];
extern  OS_STK TaskUart1RcvStk[TASK_STK_SIZE*2];
#endif



#ifdef USE_DEV_UART0

/********************************************************************************************************
** 函数名称: TaskUart0Cmd
** 功能描述: 命令解析
********************************************************************************************************/
void TaskUart0Cmd(void* pdata)
{
		uint8  err;
	    int statu;
	    int str_len;

		OSTimeDly(500);            //等待延时
		while(1)
		{
			  OSMboxPend(Uart0RcvMbox, 0, &err);         // 等待接收邮箱数据
	          statu = cmd_process((char*)&uart0RcvBuf,&str_len);  //选择UART输出，-2===指令校验错误，-1=====没有该指令；0===串口0输出，1x===串口1输出第x路；
			  switch(statu)
			  {
				  case -2:
				  
				  case -1:
				  
				  case 0 : UART0Put_str(uart0RcvBuf,str_len);break;
				  				 
#ifdef USE_DEV_UART1
				  case 1 :UART1Put_str(uart0RcvBuf,str_len);break;//向串口1发送数据 ; 
#endif	   
				  default:break;
				  
			  }
//				if( (len = cmd_process((char*)&uart0RcvBuf)) > 0 )
//					  UART2Put_str(u2RcvBuf, len);
		}
}

/********************************************************************************************************
** 函数名称: TaskUart0Rcv
** 功能描述: μCOS-II的任务。从UART0接收数据，当接收完一帧数据后通过消
**           息邮箱传送到TaskStart任务。
********************************************************************************************************/
void TaskUart0Rcv(void* pdata)
{
		uint8 *cp;
		uint8 i,temp;

		while(1)
		{
				cp = uart0RcvBuf;
			    do{
				    *cp = UART0Getch();
				}while(((*cp != 'S')&&(*cp != 'G')&&(*cp != 'C')&&(*cp != 'R')&&(*cp != 'O')));
				//while((*cp = UART2Getch()) != 'S') ;  // 接收数据头
				cp++;   								              //往下移一个字节
				for (i = 0; i < 50; i++)
				{
						temp = UART0Getch();
						*cp++ = temp;
						if (temp =='\n')
						{
								while(i < 48)
								{
										*cp++ = 0;                //空余的后面补0
										i++;
								}
								break;
						}
				}
				OSMboxAccept(Uart0RcvMbox);          //清空 邮箱Uart0ReviceMbox
				OSMboxPost(Uart0RcvMbox, (void *)uart0RcvBuf);
		}
}


#endif


#ifdef USE_DEV_UART1
/********************************************************************************************************
** 函数名称: TaskUart1Cmd
** 功能描述: 命令解析
********************************************************************************************************/
void TaskUart1Cmd(void* pdata)
{
		uint8  err;
	    int statu;
	    int str_len;

		OSTimeDly(500);            //等待延时
		while(1)
		{
			  OSMboxPend(Uart1RcvMbox, 0, &err);         // 等待接收邮箱数据
	          statu = cmd_process((char*)&uart1RcvBuf,&str_len);
			  switch(statu)
			  {
				  case -1 :break;
#ifdef 	USE_DEV_UART0		  
				  case 0 : UART0Put_str(uart0RcvBuf,str_len);
#endif
				      break;
                  default:break;				  
				  //case 2 : UART1Put_str(uart0RcvBuf,str_len);break;
				   
			  }
//				if( (len = cmd_process((char*)&uart0RcvBuf)) > 0 )
//					  UART2Put_str(u2RcvBuf, len);
		}
}
/********************************************************************************************************
** 函数名称: TaskUart1Rcv
** 功能描述: μCOS-II的任务。从UART1接收数据，当接收完一帧数据后通过消
**           息邮箱传送到TaskStart任务。
********************************************************************************************************/
void TaskUart1Rcv(void* pdata)
{
		uint8 *cp;
		uint8 i,temp;

		while(1)
		{
				cp = uart1RcvBuf;
			    do{
				    *cp = UART2Getch();
				}while(((*cp != 'S')&&(*cp != 'G')&&(*cp != 'C')&&(*cp != 'R')));
				//while((*cp = UART2Getch()) != 'S') ;  // 接收数据头
				cp++;   								              //往下移一个字节
				for (i = 0; i < 50; i++)
				{
						temp = UART1Getch();
						*cp++ = temp;
						if (temp =='\n')
						{
								while(i < 48)
								{
										*cp++ = 0;                //空余的后面补0
										i++;
								}
								break;
						}
				}
				OSMboxAccept(Uart1RcvMbox);          //清空 邮箱Uart0ReviceMbox
				OSMboxPost(Uart1RcvMbox, (void *)uart1RcvBuf);
		}
}
#endif



