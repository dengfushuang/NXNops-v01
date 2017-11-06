//#include  "config.h"
//#include  "main.h"
//#include  "uart2.h"
//#include  "queue.h"
#include "includes.h"

static uint8 UART2SendBuf[256];
static OS_EVENT *Uart2Sem;


uint8 Uart2WriteFull(DataQueue *Buf, QUEUE_DATA_TYPE Data, uint8 Mod)
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


uint8_t UART2_Init(uint32 BPS)
{
    uint16 Fdiv;
	
    //ʹ����UART2��������Դ    
    LPC_SC->PCONP |= (1<<24);
    //Enable UART2 Pins.
    LPC_IOCON->P0_10 = 0x01;     //UART2_TXD
    LPC_IOCON->P0_11 = 0x01;     //UART2_RXD

    LPC_UART2->LCR = 0x80;                      // ������ʷ�Ƶ���ӼĴ���     
    Fdiv = (PeripheralClock / 16) / BPS;        // ���ò����� 
    LPC_UART2->DLM = Fdiv / 256;
	  LPC_UART2->DLL = Fdiv % 256;
    LPC_UART2->LCR = 0x03;                      // 8N1ģʽ����ֹ���ʷ�Ƶ���ӼĴ��� 
                                                // ������Ϊ8,1,n 
	  LPC_UART2->IER = 0x05;                      // ������պͷ����ж� 
    LPC_UART2->FCR = 0x87;                      // ��ʼ��FIFO 

    NVIC_EnableIRQ(UART2_IRQn);                 // Enable UART2 interrupt 
	  if (QueueCreate((void *)UART2SendBuf,
                     sizeof(UART2SendBuf),
                     NULL,
                     (uint8 (*)())Uart2WriteFull) 
                     == NOT_OK)
    {
        return FALSE;
    }
    Uart2Sem = OSSemCreate(0);
    if (Uart2Sem != NULL)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}


void UART2Putch(uint8 Data)
{
	  uint8 temp;
    OS_CPU_SR  cpu_sr;
	
    QueueWrite((void *)UART2SendBuf, Data);         /* ������� */
    OS_ENTER_CRITICAL();
    if ((LPC_UART2->LSR & 0x00000020) != 0)
    {                                               /* UART2���ͱ��ּĴ����� */
        QueueRead(&temp, UART2SendBuf);             /* ���������ӵ����� */
        LPC_UART2->THR = temp;
        LPC_UART2->IER = LPC_UART2->IER | 0x02;     /* �������ж� */
    }
    OS_EXIT_CRITICAL();
}


void UART2Put_str(uint8 *Data, uint16 NByte)
{
    while (NByte-- > 0)
        UART2Putch(*Data++);
}


void UART2Write_Str(uint8_t *Data)
{
    while (*Data != '\0' )
        UART2Putch(*Data++);
}


uint8 UART2Getch(void)
{
    uint8 err;
	
    while ((LPC_UART2->LSR & 0x00000001) == 0)
    {                                           /* û���յ����� */
        LPC_UART2->IER = LPC_UART2->IER | 0x01; /* ��������ж� */
			  OSSemPend(Uart2Sem, 0, &err); 
    }
    err = LPC_UART2->RBR;                       /* ��ȡ�յ������� */
    return err;
}


uint8 UART2Get(void)
{
    uint8 err;
	
    if ((LPC_UART2->LSR & 0x00000001) == 1)
			  err = LPC_UART2->RBR;
		else err = 0;
    return err;
}


void UART2_IRQHandler (void) 
{
    uint8 IIR, temp, i;
	  OS_CPU_SR  cpu_sr;
	
    OS_ENTER_CRITICAL();
    while(((IIR = LPC_UART2->IIR) & 0x01) == 0)         /* ���ж�δ������ */
    {
        switch (IIR & 0x0e)
        {
            case 0x02:                                  /* THRE�ж�    */
                for (i = 0; i < UART2_FIFO_LENGTH; i++) /* ����FIFO������� */
                {
                    if (QueueRead(&temp, UART2SendBuf) == QUEUE_OK)
                    {
                        LPC_UART2->THR = temp;
                    }
                    else
                    {
                        LPC_UART2->IER = LPC_UART2->IER & (~0x02);        /* ���пգ����ֹ�����ж� */
                    }
                }
                break;
            case 0x04:                                  /* �������ݿ��� */
                OSSemPost(Uart2Sem);                    /* ֪ͨ�������� */
                LPC_UART2->IER = LPC_UART2->IER & (~0x01);                /* ��ֹ���ռ��ַ���ʱ�ж� */
                break;
            case 0x06:                                  /* ������״̬   */
                temp = LPC_UART2->LSR;
                break;
            case 0x0c:                                  /* �ַ���ʱָʾ */
                OSSemPost(Uart2Sem);                    /* ֪ͨ�������� */
                LPC_UART2->IER = LPC_UART2->IER & (~0x01);                /* ��ֹ���ռ��ַ���ʱ�ж� */
                break;
            default :
            	  temp = LPC_UART2->LSR;
                break;
        }
    }

    OS_EXIT_CRITICAL();
}







