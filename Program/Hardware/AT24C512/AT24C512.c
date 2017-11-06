#include "main.H"
#include "AT24C512.H"
#include "LPC177x_8x.h"

/**********************************************************************************************************
*�ļ�����eeprom.c
*����·����Hardware/eeprom/eeprom.c
*����ʱ��: 2012-08-06
*��������: ����CAT1025 eepromоƬ��������Դ�ļ�
*��   �� �� ũ����
*��   ˾ �� ��¡����
**********************************************************************************************************/
/**********************************************************************************************************
*Ӳ���ܽ��������£�
* SDA--I2C0_SDA--P0.27
* SCL--I2C0_SCL--P0.28
* REST--REST
*
***********************************************************************************************************/
uint8     IC2_ERR;
uint8     ADD_STATE=0;
uint8     Clear_FLAG=0;

/* ����ΪI2C����ʱ����Ҫ�ı��� */
volatile  uint8  I2C_sla;           // �ӻ���ַ(��������ַ)
volatile  uint16 I2C_suba;          // 16λ���ӵ�ַ
volatile  uint8  *I2C_buf;          // ���ݻ�����ָ�� (������ʱ�ᱻ����)
volatile  uint8  I2C_num;           // �������ݸ��� (�ᱻ����)
volatile  uint8  I2C_end;           // ����������־��Ϊ1ʱ��ʾ����������Ϊ0xFFʱ��ʾ����ʧ�� (�ᱻ����)
volatile  uint8  I2C_suba_en;       // �ӵ�ַʹ�ܿ��ƣ�������ʱ������Ϊ1��д����ʱ������Ϊ2 (�ᱻ����)


/****************************************************************************
* ��    �ƣ�I2C_Init()
* ��    �ܣ���ģʽI2C��ʼ����������ʼ�����ж�Ϊ����IRQ�жϡ�
* ��ڲ�����fi2c		��ʼ��I2C�������ʣ����ֵΪ400K
* ���ڲ�������
****************************************************************************/
void  AT24C512_I2C_Init(void)
{
    LPC_SC->PCONP |= (1 << 7);              
    LPC_IOCON->P0_27 = 0x01;                            // ����I2C0���ƿ���Ч
    LPC_IOCON->P0_28 = 0x01;   
    
    LPC_I2C0->SCLH = (PeripheralClock/400000) / 2;	    // ����I2Cʱ��Ϊfi2c
    LPC_I2C0->SCLL = (PeripheralClock/400000) / 2;
    LPC_I2C0->CONCLR = 0x6C;                            // ������ƼĴ���
    LPC_I2C0->CONSET = 0x40;						    // ʹ����I2C

    LPC_I2C0->ADR0 = AT24C512;

    NVIC_EnableIRQ(I2C0_IRQn);
}

/****************************************************************************
* ���ƣ�IRQ_I2C()
* ���ܣ�I2C�жϣ�ͨ���ж�I2C״̬�ֽ�����Ӧ�Ĳ�����
* ��ڲ�������
* ���ڲ�������
****************************************************************************/
void I2C0_IRQHandler(void)  
{
    uint8  sta;
    OS_CPU_SR  cpu_sr;
    OS_ENTER_CRITICAL();
    
    sta = LPC_I2C0->STAT;                                     // ����I2C״̬��
    switch(sta)
    {
    case  0x08:                                               // ��������ʼ����
        if(1==I2C_suba_en) LPC_I2C0->DAT = I2C_sla&0xFE;      // ָ���ӵ�ַ��ʱ����д���ַ
        else    LPC_I2C0->DAT = I2C_sla;                      // ����ֱ�ӷ��ʹӻ���ַ
        LPC_I2C0->CONCLR = 0x28;                              // SI=0
        break;

    case  0x10:
        LPC_I2C0->DAT = I2C_sla;                              // ���������ߺ󣬷��ʹӵ�ַ
        LPC_I2C0->CONCLR = 0x28;                              // SI=0
        break;

    case  0x18:                                                 // �ѷ���SLA+W�����ѽ���Ӧ��
        if(0==I2C_suba_en)                                      // ���ӵ�ַ����ֱ�ӷ�������
        {
            if(I2C_num>0)
            {
                if( Clear_FLAG )    LPC_I2C0->DAT = *I2C_buf;
                else                LPC_I2C0->DAT = *I2C_buf++;             
                LPC_I2C0->CONCLR = 0x28;
                I2C_num--;
            }
            else
            {
                LPC_I2C0->CONSET = 0x10;                      // �����ݷ��ͣ���������
                LPC_I2C0->CONCLR = 0x28;
                I2C_end = 1;                                    // �������߲���������־
            }
            break;
        }
        
        if(1==I2C_suba_en)                                      // �����ӵ�ַ
        {
            LPC_I2C0->DAT = I2C_suba>>8;                        //�ȷ����߰�λ�ĵ�ַ
            LPC_I2C0->CONCLR = 0x28;
        }
        else if(2==I2C_suba_en)
        {
            LPC_I2C0->DAT = I2C_suba>>8;                        //�ȷ����߰�λ�ĵ�ַ
            LPC_I2C0->CONCLR = 0x28;
            //I2C_suba_en = 0;                                  // �ӵ�ַ������
        }
        break;

    case  0x28:                                                 // �ѷ���I2C���ݣ������յ�Ӧ��
        if(0==I2C_suba_en)                                      // ���ӵ�ַ����ֱ�ӷ�������
        {
            if(I2C_num>0)
            {
                if( Clear_FLAG )    LPC_I2C0->DAT = *I2C_buf;
                else                LPC_I2C0->DAT = *I2C_buf++;   
                LPC_I2C0->CONCLR = 0x28;
                I2C_num--;
            }
            else
            {
                LPC_I2C0->CONSET = 0x10;                      // �����ݷ��ͣ���������
                LPC_I2C0->CONCLR = 0x28;
                I2C_end = 1;
            }
            break;
        }
        
        if(1==I2C_suba_en)                                      
        {
            if(ADD_STATE == 0)
            {
                LPC_I2C0->DAT = I2C_suba&0X00FF;              //�ٷ����ߵ�λ�ĵ�ַ
                LPC_I2C0->CONCLR = 0x28;
                ADD_STATE = 1;
            }
            else
            {
                LPC_I2C0->CONSET = 0x20;                      // ����ָ����ַ������������������
                LPC_I2C0->CONCLR = 0x08;
                ADD_STATE = 0;
                I2C_suba_en = 0;                              // �ӵ�ַ������
            }                                 
        }
        else if(2==I2C_suba_en)
        {
            LPC_I2C0->DAT = I2C_suba&0X00FF;                  //�ٷ����Ͱ�λ�ĵ�ַ
            LPC_I2C0->CONCLR = 0x28;
            I2C_suba_en = 0;                                  // �ӵ�ַ������
        }
        break;


    case  0x20:
        IC2_ERR=1;
        LPC_I2C0->CONCLR = 0x28;                              // ���߽��벻��Ѱַ��ģʽ
        I2C_end = 0xFF;                                       // ���߳���,���ñ�־
        break;
    case  0x30:
        IC2_ERR=2;
        LPC_I2C0->CONCLR = 0x28;                              // ���߽��벻��Ѱַ��ģʽ
        I2C_end = 0xFF;                                       // ���߳���,���ñ�־
        break;
    case  0x38:
        IC2_ERR=3;
        LPC_I2C0->CONCLR = 0x28;                              // ���߽��벻��Ѱַ��ģʽ
        I2C_end = 0xFF;                                       // ���߳���,���ñ�־
        break;


    case  0x40:                                               // ������SLA+R�����ѽ��յ�Ӧ��
        if(1==I2C_num)                                        // ���һ�ֽڣ��������ݺ��ͷ�Ӧ���ź�
        {
            LPC_I2C0->CONCLR = 0x2C;                          // AA=0�����յ����ݺ������Ӧ��
        }
        else                                                  // �������ݲ�����Ӧ���ź�
        {
            LPC_I2C0->CONSET = 0x04;                          // AA=1�����յ����ݺ����Ӧ��
            LPC_I2C0->CONCLR = 0x28;
        }
        break;

    case  0x50:
        *I2C_buf++ = LPC_I2C0->DAT;                           // ��ȡ����
        I2C_num--;
        if(1==I2C_num)
        {
            LPC_I2C0->CONCLR = 0x2C;                          // AA=0�����յ����ݺ������Ӧ��
        }
        else
        {
            LPC_I2C0->CONSET = 0x04;                          // AA=1�����յ����ݺ����Ӧ��
            LPC_I2C0->CONCLR = 0x28;
        }
        break;

    case  0x58:
        *I2C_buf++ = LPC_I2C0->DAT;                           // ��ȡ���һ�ֽ�����
        LPC_I2C0->CONSET = 0x10;                              // ��������
        LPC_I2C0->CONCLR = 0x28;
        I2C_end = 1;
        break;

    case  0x48:
        IC2_ERR=4;
        LPC_I2C0->CONCLR = 0x28;                              // ���߽��벻��Ѱַ��ģʽ
        I2C_end = 0xFF;
        break;

    default:
        LPC_I2C0->CONCLR = 0x28;
        break;
    }

    OS_EXIT_CRITICAL();
}

/****************************************************************************
* ���ƣ�ISendByte()
* ���ܣ������ӵ�ַ��������һ�ֽ����ݡ�
* ��ڲ�����sla		������ַ
*           dat		Ҫ���͵�����
* ���ڲ���������ֵΪ0ʱ��ʾ����Ϊ1ʱ��ʾ������ȷ��
* ˵����ʹ��ǰҪ��ʼ����I2C���Ź��ܺ�I2C�жϣ�����ʹ��I2C��ģʽ
****************************************************************************/
uint8  ISendByte(uint8 sla, uint8 dat)
{
	/* �������� */
	I2C_sla = sla;		// д������������ַ
	I2C_buf = &dat;		// �����͵�����
	I2C_num = 1;			// ����1�ֽ�����
	I2C_suba_en = 0;		// ���ӵ�ַ
	I2C_end = 0;

	LPC_I2C0->CONCLR = 0x2C;
	LPC_I2C0->CONSET = 0x60;             // ����Ϊ����������������

	//NVIC_EnableIRQ(I2C0_IRQn);  //VICIntEnable =  1 << 9;
	while(0==I2C_end);
	//NVIC_DisableIRQ(I2C0_IRQn); //VICIntEnClr = 1 << 9; //��ֹ��I2c�ж�

	if(1==I2C_end) return(1);
	else           return(0);
}

/****************************************************************************
* ���ƣ�ISendStr()
* ���ܣ������ӵ�ַ�������Ͷ��ֽ����ݡ�
* ��ڲ�����sla		�����ӻ���ַ
*          suba		�����ӵ�ַ
*          s		���ݷ��ͻ�����ָ��
*          no		�������ݸ���
* ���ڲ���������ֵΪ0ʱ��ʾ����Ϊ1ʱ��ʾ������ȷ��
* ˵����ʹ��ǰҪ��ʼ����I2C���Ź��ܺ�I2C�жϣ�����ʹ��I2C��ģʽ
****************************************************************************/
uint8  ISendStr(uint8 sla, uint16 suba, uint8 *s, uint8 no)
{
	/* �������� */
	I2C_sla = sla;		// д������������ַ
	I2C_suba = suba;		// �ӵ�ַ
	I2C_buf = s;
	I2C_num = no;
	I2C_suba_en = 2;		// ���ӵ�ַд
	I2C_end = 0;

	LPC_I2C0->CONCLR = 0x2C;
	LPC_I2C0->CONSET = 0x60;             // ����Ϊ����������������

	//NVIC_EnableIRQ(I2C0_IRQn);  //VICIntEnable =  1 << 9;
	while(0==I2C_end);
	//NVIC_DisableIRQ(I2C0_IRQn); //VICIntEnClr = 1 << 9; //��ֹ��I2c�ж�

	if(1==I2C_end)
	{
		return(1);
	}
	if(I2C_end == 0xff)
	{
	  return(0);
	}
	else    return(0);
}

/****************************************************************************
* ���ƣ�IRcvByte()
* ���ܣ������ӵ�ַ������ȡһ�ֽ����ݡ�
* ��ڲ�����sla		������ַ
*          dat		�������ݵı���ָ��
* ���ڲ���������ֵΪ0ʱ��ʾ��������Ϊ1ʱ��ʾ������ȷ��
* ˵����ʹ��ǰҪ��ʼ����I2C���Ź��ܺ�I2C�жϣ�����ʹ��I2C��ģʽ
****************************************************************************/
uint8  IRcvByte(uint8 sla, uint8 *dat)
{
	/* �������� */
	I2C_sla = sla+1;		// ��������������ַ
	I2C_buf = dat;
	I2C_num = 1;
	I2C_suba_en = 0;		// ���ӵ�ַ
	I2C_end = 0;

	LPC_I2C0->CONCLR = 0x2C;
	LPC_I2C0->CONSET = 0x60;             // ����Ϊ����������������
	//NVIC_EnableIRQ(I2C0_IRQn);  //VICIntEnable =  1 << 9;
	while(I2C_end == 0);
	// NVIC_DisableIRQ(I2C0_IRQn); //VICIntEnClr = 1 << 9; //��ֹ��I2c�ж�

	if(I2C_end == 1)     return(1);
	else                 return(0);
}

/****************************************************************************
* ���ƣ�IRcvStr()
* ���ܣ������ӵ�ַ������ȡ���ֽ����ݡ�
* ��ڲ�����sla		������ַ
*          suba		�����ӵ�ַ
*          s		���ݽ��ջ�����ָ��
*	   	     no		��ȡ���ݸ���
* ���ڲ���������ֵΪ0ʱ��ʾ��������Ϊ1ʱ��ʾ������ȷ��
* ˵����ʹ��ǰҪ��ʼ����I2C���Ź��ܺ�I2C�жϣ�����ʹ��I2C��ģʽ
****************************************************************************/
uint8  IRcvStr(uint8 sla, uint16 suba, uint8 *s, uint8 no)
{
	if(no == 0) return(0);

	/* �������� */
	I2C_sla  = sla+1;		    // ��������������ַ
	I2C_suba = suba;
	I2C_buf  = s;
	I2C_num  = no;
	I2C_suba_en = 1;		    // ���ӵ�ַ��
	I2C_end = 0;

	LPC_I2C0->CONCLR = 0x2C;
	LPC_I2C0->CONSET = 0x60;             // ����Ϊ����������������

	//NVIC_EnableIRQ(I2C0_IRQn);  //VICIntEnable = 1 << 9;
	while(I2C_end == 0);
	//NVIC_DisableIRQ(I2C0_IRQn); //VICIntEnClr = 1 << 9; //��ֹ��I2c�ж�

	if(I2C_end == 1)
	{
		return(1);
	}
	if(I2C_end == 0xff)
	{
		return(0);
	}
	else                
	return(0);
}

/*****************************************************************************
**�� �� ��: Read_AT24C512
**��������:  
**�������: AT24C512  
             page*128  
             s         
             num       
**�������: ��
**�� �� ֵ: ��
*****************************************************************************/
/*
void Read_AT24C512(AT24C512, uint16 page, uint8* data, uint16 num)
{
    if(page > 512) page = 0;
    
    IRcvStr(AT24C512, page*128, data, num);
}
*/
/*****************************************************************************
**�� �� ��: AT24C512_Clear
**��������: ��� AT24C512 �����е�����
**�������: void  
**�������: ��
**�� �� ֵ: ��
*****************************************************************************/
void AT24C512_Clear(void)
{
	uint16 i;
	uint8  data=0xff;
	
	Clear_FLAG = 1;
	for ( i = 0 ; i <512 ; i++ )
	{
		ISendStr(AT24C512, i, &data, 128);
		i+=128;
		OSTimeDly(20);
	} 
	
	Clear_FLAG = 0;
	EPROM.AT24C512_Page = 0;
	EPROM.AT24C512_NUM = 0;
	Save_To_EPROM((uint8 *)&EPROM.AT24C512_Page, 4);
	OSTimeDly(20);
}





