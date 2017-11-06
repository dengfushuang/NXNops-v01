#include "user_Task.h"


#ifdef USE_DEV_UART0
extern uint8  uart0RcvBuf[120];              //����TaskUart0Revice �õ�����,uart0������������
extern OS_EVENT *Uart0RcvMbox;            //���� 0 ��������
extern OS_STK TaskUart0CmdStk[TASK_STK_SIZE*2];
extern OS_STK TaskUart0RcvStk[TASK_STK_SIZE*2];
#endif

#ifdef USE_DEV_UART1
extern  uint8  uart1RcvBuf[120];           //uart1������������
extern  OS_EVENT *Uart1RcvMbox;            //���� 1 ��������
extern  OS_STK TaskUart1CmdStk[TASK_STK_SIZE*2];
extern  OS_STK TaskUart1RcvStk[TASK_STK_SIZE*2];
#endif



#ifdef USE_DEV_UART0

/********************************************************************************************************
** ��������: TaskUart0Cmd
** ��������: �������
********************************************************************************************************/
void TaskUart0Cmd(void* pdata)
{
		uint8  err;
	    int statu;
	    int str_len;

		OSTimeDly(500);            //�ȴ���ʱ
		while(1)
		{
			  OSMboxPend(Uart0RcvMbox, 0, &err);         // �ȴ�������������
	          statu = cmd_process((char*)&uart0RcvBuf,&str_len);  //ѡ��UART�����-2===ָ��У�����-1=====û�и�ָ�0===����0�����1x===����1�����x·��
			  switch(statu)
			  {
				  case -2:
				  
				  case -1:
				  
				  case 0 : UART0Put_str(uart0RcvBuf,str_len);break;
				  				 
#ifdef USE_DEV_UART1
				  case 1 :UART1Put_str(uart0RcvBuf,str_len);break;//�򴮿�1�������� ; 
#endif	   
				  default:break;
				  
			  }
//				if( (len = cmd_process((char*)&uart0RcvBuf)) > 0 )
//					  UART2Put_str(u2RcvBuf, len);
		}
}

/********************************************************************************************************
** ��������: TaskUart0Rcv
** ��������: ��COS-II�����񡣴�UART0�������ݣ���������һ֡���ݺ�ͨ����
**           Ϣ���䴫�͵�TaskStart����
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
				//while((*cp = UART2Getch()) != 'S') ;  // ��������ͷ
				cp++;   								              //������һ���ֽ�
				for (i = 0; i < 50; i++)
				{
						temp = UART0Getch();
						*cp++ = temp;
						if (temp =='\n')
						{
								while(i < 48)
								{
										*cp++ = 0;                //����ĺ��油0
										i++;
								}
								break;
						}
				}
				OSMboxAccept(Uart0RcvMbox);          //��� ����Uart0ReviceMbox
				OSMboxPost(Uart0RcvMbox, (void *)uart0RcvBuf);
		}
}


#endif


#ifdef USE_DEV_UART1
/********************************************************************************************************
** ��������: TaskUart1Cmd
** ��������: �������
********************************************************************************************************/
void TaskUart1Cmd(void* pdata)
{
		uint8  err;
	    int statu;
	    int str_len;

		OSTimeDly(500);            //�ȴ���ʱ
		while(1)
		{
			  OSMboxPend(Uart1RcvMbox, 0, &err);         // �ȴ�������������
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
** ��������: TaskUart1Rcv
** ��������: ��COS-II�����񡣴�UART1�������ݣ���������һ֡���ݺ�ͨ����
**           Ϣ���䴫�͵�TaskStart����
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
				//while((*cp = UART2Getch()) != 'S') ;  // ��������ͷ
				cp++;   								              //������һ���ֽ�
				for (i = 0; i < 50; i++)
				{
						temp = UART1Getch();
						*cp++ = temp;
						if (temp =='\n')
						{
								while(i < 48)
								{
										*cp++ = 0;                //����ĺ��油0
										i++;
								}
								break;
						}
				}
				OSMboxAccept(Uart1RcvMbox);          //��� ����Uart0ReviceMbox
				OSMboxPost(Uart1RcvMbox, (void *)uart1RcvBuf);
		}
}
#endif



