//#include  "config.h"
//#include  "main.h"
//#include  "uart1.h"
//#include  "queue.h"
#include "includes.h"

static uint8 UART1SendBuf[256];
static OS_EVENT *Uart1Sem;

extern uint32 SystemCoreClock;


uint8 Uart1WriteFull(DataQueue *Buf, QUEUE_DATA_TYPE Data, uint8 Mod)
{
    uint16 temp;

    Mod = Mod;

    temp = QueueSize((void *)Buf);
    while (temp <= QueueNData((void *)Buf))         /* �ȴ����ݶ��в��� */
    {
        OSTimeDly(2*5);
    }
    return QueueWrite((void *)Buf, Data);           /* ����������� */
}


uint8_t UART1_Init(uint32 BPS)
{
    uint16 Fdiv;
	
    //ʹ����UART1��������Դ    
    LPC_SC->PCONP |= (1<<4);
    //Enable UART1 Pins.
    LPC_IOCON->P0_15 = 0x01;     //UART1_TXD
    LPC_IOCON->P0_16 = 0x01;     //UART1_RXD

    LPC_UART1->LCR = 0x80;                      // ������ʷ�Ƶ���ӼĴ���     
    Fdiv = (PeripheralClock / 16) / BPS;        // ���ò����� 
    LPC_UART1->DLM = Fdiv / 256;
	  LPC_UART1->DLL = Fdiv % 256;
    LPC_UART1->LCR = 0x03;                      // 8N1ģʽ����ֹ���ʷ�Ƶ���ӼĴ��� 
                                                // ������Ϊ8,1,n 
	  LPC_UART1->IER = 0x05;                      // ������պͷ����ж� 
    LPC_UART1->FCR = 0x87;                      // ��ʼ��FIFO 

    NVIC_EnableIRQ(UART1_IRQn);                 // Enable UART1 interrupt 
	  if (QueueCreate((void *)UART1SendBuf,
                     sizeof(UART1SendBuf),
                     NULL,
                     (uint8 (*)())Uart1WriteFull) 
                     == NOT_OK)
    {
        return FALSE;
    }
    Uart1Sem = OSSemCreate(0);
    if (Uart1Sem != NULL)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}


void UART1Putch(uint8 Data)
{
	  uint8 temp;
    OS_CPU_SR  cpu_sr;
	
    QueueWrite((void *)UART1SendBuf, Data);         /* ������� */
    OS_ENTER_CRITICAL();
    if ((LPC_UART1->LSR & 0x00000020) != 0)
    {                                               /* UART0���ͱ��ּĴ����� */
        QueueRead(&temp, UART1SendBuf);             /* ���������ӵ����� */
        LPC_UART1->THR = temp;
        LPC_UART1->IER = LPC_UART1->IER | 0x02;     /* �������ж� */
    }
    OS_EXIT_CRITICAL();
}


void UART1Put_str(uint8 *Data, uint16 NByte)
{
    while (NByte-- > 0)
    {
        UART1Putch(*Data++);
    }
}


void UART1Write_Str(uint8_t *Data)
{
    while (*Data != '\0' )
    {
        UART1Putch(*Data++);
    }
}


uint8 UART1Getch(void)
{
    uint8 err;
	
    while ((LPC_UART1->LSR & 0x00000001) == 0)
    {                                           /* û���յ����� */
        LPC_UART1->IER = LPC_UART1->IER | 0x01; /* ��������ж� */
			  OSSemPend(Uart1Sem, 0, &err); 
    }
    err = LPC_UART1->RBR;                       /* ��ȡ�յ������� */
    return err;
}


uint8 UART1Get(void)
{
    uint8 err;
	
    if ((LPC_UART1->LSR & 0x00000001) == 0)
    {                                           /* û���յ����� */
        LPC_UART1->IER = LPC_UART1->IER | 0x01; /* ��������ж� */
			  OSSemPend(Uart1Sem, 1, &err); 
    }
    err = LPC_UART1->RBR;                       /* ��ȡ�յ������� */
    return err;
}


void UART1_IRQHandler (void) 
{
    uint8 IIR, temp, i;
	  OS_CPU_SR  cpu_sr;
	
    OS_ENTER_CRITICAL();

    while(((IIR = LPC_UART1->IIR) & 0x01) == 0)
    {                                                   /* ���ж�δ������ */
        switch (IIR & 0x0e)
        {
            case 0x02:                                  /* THRE�ж�    */
                for (i = 0; i < UART1_FIFO_LENGTH; i++) /* ����FIFO������� */
                {
                    if (QueueRead(&temp, UART1SendBuf) == QUEUE_OK)
                    {
                        LPC_UART1->THR = temp;
                    }
                    else
                    {
                        LPC_UART1->IER = LPC_UART1->IER & (~0x02);        /* ���пգ����ֹ�����ж� */
                    }
                }
                break;
            case 0x04:                                  /* �������ݿ��� */
                OSSemPost(Uart1Sem);                    /* ֪ͨ�������� */
                LPC_UART1->IER = LPC_UART1->IER & (~0x01);                /* ��ֹ���ռ��ַ���ʱ�ж� */
                break;
            case 0x06:                                  /* ������״̬   */
                temp = LPC_UART1->LSR;
                break;
            case 0x0c:                                  /* �ַ���ʱָʾ */
                OSSemPost(Uart1Sem);                    /* ֪ͨ�������� */
                LPC_UART1->IER = LPC_UART1->IER & (~0x01);                /* ��ֹ���ռ��ַ���ʱ�ж� */
                break;
            default :
            	  temp = LPC_UART1->LSR;
                break;
        }
    }
    //VICVectAddr = 0;            // ֪ͨ�жϿ������жϽ���
    OS_EXIT_CRITICAL();
}







