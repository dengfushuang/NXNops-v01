/******************************************************************************

                 ��Ȩ���� (C), 2001-2013, ���ֺ������ͨ�ż������޹�˾

******************************************************************************
 �� �� ��   : ADC.c
 �� �� ��   : ����
 ��    ��   :
 ��������   : 2013��5��10��
 ����޸�   :
 ��������   :
 �����б�   :
             adcdeal
             ADC_int
             addo
             optics_collec
******************************************************************************/

#include "math.h"
#include "ADC.h"
#include "LPC177x_8x.h"
#include "uart0.h"

#define CHTEMP  3
#define minadc  40   	              //minadc=20;̫���ˣ�Ӧ�ÿ��Ǹ�һЩ��30~50������߿�����������
#define maxadc  1000

uint8 hardware_way[CHANNEL_NUM*2];  //��ʵ��Ӳ����λ
float power[CHANNEL_NUM*2];
float power_warn[CHANNEL_NUM*2];
float power_count[CHANNEL_NUM*2];
uint8_t warn[CHANNEL_NUM*2];			  //�澯״̬��0���澯    1�澯


/*****************************************************************************
**�� �� ��: ADC_int
**��������: ADC ��ʼ��
**�������: rate ADC�Ļ���Ӧ
**�������: ��
**�� �� ֵ: LPC_ADC->DR
*****************************************************************************/
void ADC_int(uint32 rate)
{
    //ʹ����ADC��������Դ
    LPC_SC->PCONP |= (1U << 12);

    // Enable  Ethernet Pins.
    LPC_IOCON->P0_23 = 0x01;     //ADC0[0]
    LPC_IOCON->P0_24 = 0x01;     //ADC0[1]
    LPC_IOCON->P0_25 = 0x01;     //ADC0[2]
    LPC_IOCON->P0_26 = 0x01;     //ADC0[3]
    LPC_IOCON->P1_30 = 0x03;     //ADC0[4]
    LPC_IOCON->P1_31 = 0x03;     //ADC0[5]
    LPC_IOCON->P0_12 = 0x03;     //ADC0[6]
    LPC_IOCON->P0_13 = 0x03;     //ADC0[7]

    LPC_GPIO0->DIR |= (1<<17);
		LPC_GPIO0->DIR |= (1<<18);
		LPC_GPIO0->DIR |= (1<<19);
		LPC_GPIO0->DIR |= (1<<20);
		LPC_GPIO0->DIR |= (1<<21);
		LPC_GPIO0->DIR |= (1<<22);
	
    //����ADCģ�����ã�����x<<n��ʾ��nλ����Ϊx(��x����һλ�������λ˳��)
    LPC_ADC->CR = 0;
    LPC_ADC->CR = (1 << 0)              |     // SEL = 0 ��ѡ��ͨ��1
                  ((PeripheralClock / 1000000 - 1) << 8) |     // CLKDIV = Fpclk / 1000000 - 1 ����ת��ʱ��Ϊ1MHz
                  (0 << 16)                    |     // BURST = 0 ���������ת������
                  (0 << 17)                    |     // CLKS = 0 ��ʹ��11clockת��
                  (1 << 21)                    |     // PDN = 1 �� ��������ģʽ(�ǵ���ת��ģʽ)
                  (0 << 22)                    |     // TEST1:0 = 00 ����������ģʽ(�ǲ���ģʽ)
                  (1 << 24)                    |     // START = 1 ������ADCת��
                  (0 << 27);                         // EDGE = 0 (CAP/MAT�����½��ش���ADCת��)

    while ((LPC_ADC->GDR & 0x80000000) == 0); // �ȴ�ת������
}


/*********************************************************************************************************
** ��������: void addo(uint8 ch)
** ��������: adcת��
** �䡡��: AD����
** �䡡��: ADC���
** ����ʱ�� :
********************************************************************************************************/
uint32 addo(uint8 ch)
{
    uint32 ADC_Data;

	  switch(ch)
		{
			  case 0:  LPC_ADC->CR = (LPC_ADC->CR&0x00FFFF00)|0x01|(1 << 24);  break;
				case 1:  LPC_ADC->CR = (LPC_ADC->CR&0x00FFFF00)|0x02|(1 << 24);  break;
//				case 2:  LPC_ADC->CR = (LPC_ADC->CR&0x00FFFF00)|0x04|(1 << 24);  break;
//				case 3:  LPC_ADC->CR = (LPC_ADC->CR&0x00FFFF00)|0x08|(1 << 24);  break;
//				case 4:  LPC_ADC->CR = (LPC_ADC->CR&0x00FFFF00)|0x10|(1 << 24);  break;
//				case 5:  LPC_ADC->CR = (LPC_ADC->CR&0x00FFFF00)|0x20|(1 << 24);  break;
//				case 6:  LPC_ADC->CR = (LPC_ADC->CR&0x00FFFF00)|0x40|(1 << 24);  break;
//				case 7:  LPC_ADC->CR = (LPC_ADC->CR&0x00FFFF00)|0x80|(1 << 24);  break;
			  default:  break;
		}

    while ((LPC_ADC->GDR & 0x80000000) == 0);            // �ȴ�ת������
    LPC_ADC->CR = LPC_ADC->CR | (1 << 24);               // �ٴ�����ת��
    while ((LPC_ADC->GDR & 0x80000000) == 0);            // �ȴ�ת������

    ADC_Data = LPC_ADC->GDR;                             // ��ȡADC���
    ADC_Data = (ADC_Data >> 6) & 0x3FF;                  // ��ȡADת��ֵ (bit4~bit15 Ϊ12λ��ADCֵ)������ֻȡ10λ��ADCֵ

    return ADC_Data;
}


/*****************************************************************************
**�� �� ��: adcdeal
**��������:
**�������: uint8 RTnum
             uint32 way_temp
             float  adc
**�������: ��
**�� �� ֵ:
**���ú���:
**��������:
*****************************************************************************/
void adcdeal(uint8 RTnum,uint32 way, float adc)
{
    if(adc > 800)
    {
        if(way > 0)
        {
            way = way - 1;
            adc = adc / 10;
        }
        else
        {
            way = way;
            adc = 800;
        }
    }
    else if(adc < 80)
    {
        if(way < 7)
        {
            way = way + 1;
            adc = adc * 10;
        }
        else
        {
            way = way;  	   //�������������
            adc = 80;
        }
    }
    //*****************************��ֹ����10��*****************************
    if(adc > 800)
    {
        if(way > 0)
        {
            way = way - 1;
            adc = adc / 10;
        }
        else
        {
            way = way;
            adc = 800;
        }
    }
    else if(adc < 80)
    {
        if(way < 7)
        {
            way = way + 1;
            adc = adc * 10;
        }
        else
        {
            way = way;  	 //�������������
            adc = 80;
        }
    }

    if(way >= CHTEMP)      //��ֵ
    {
        adc = 800.0 / adc;
        adc = 10 * log10(adc);
        adc = adc + 10 * (way - CHTEMP);
        adc = (-1) * adc;
    }
    else                 //��ֵ
    {
        adc = adc / 80.0;
        adc = 10 * log10(adc);
        adc = adc + 10 * ((CHTEMP - 1) - way);
    }

    //adc += ((float)EPROM.ADC_just[RTnum][EPROM.wavelength[RTnum]])/10.0; //����У׼����ϵ��

    if(adc > 30.0)      adc = 30.0;
    if(adc < -50.0)     adc = -50.0;
		
    power[RTnum] = adc;
    if(power[RTnum] <= EPROM.q_power[RTnum])
    {
			  power_warn[RTnum] = power[RTnum];
        warn[RTnum] = 1;
    }
    else
    {
        warn[RTnum] = 0;
    }
}


/*****************************************************************************
**�� �� ��: CHANNEL
**��������:
**�������: uint8 RTnum
            uint8 way_temp
**�������: ��
**�� �� ֵ:
*****************************************************************************/
void CHANNEL(uint8 RTnum, uint8 way_temp)
{
    switch ( RTnum )
    {
				case 0 :
						CHANNEL_PIN0(way_temp);
						break;
				case 1 :
						CHANNEL_PIN1(way_temp);
						break;
//				case 2 :
//						CHANNEL_PIN2(way_temp);
//						break;
//				case 3 :
//						CHANNEL_PIN3(way_temp);
//						break;
//				case 4 :
//						CHANNEL_PIN4(way_temp);
//						break;
//				case 5 :
//						CHANNEL_PIN5(way_temp);
//						break;
//				case 6 :
//						CHANNEL_PIN6(way_temp);
//						break;
//				case 7 :
//						CHANNEL_PIN7(way_temp);
//						break;
				default:
						break;
    }
}


/*****************************************************************************
 �� �� ��  : optics_collect
 ��������  : ���ƻ�����·���ɼ����ź�ת���ɹ⹦��ֵ
 �������  : uint8 RTnum
             uint8 way
 �������  : ��
 �� �� ֵ  :
*****************************************************************************/
void optics_collect(uint8_t RTnum , uint8_t way_temp)
{
    uint16_t ADC_Data;
    float  ADC_just_temp;
    OS_CPU_SR  cpu_sr;

ADC_STAR:
    ADC_Data = (uint16_t)addo(RTnum);
    if (ADC_Data < minadc)                  //ADת��ֵС��20����ͨ��
    {
        if (way_temp < 7)
        {
            way_temp ++;

            OS_ENTER_CRITICAL();
            CHANNEL(RTnum, way_temp);       //�趨��1ͨ��
            OS_EXIT_CRITICAL();	

            OSTimeDly(10);
            goto ADC_STAR;
        }
    }
    else if (ADC_Data > maxadc)             //ADת��ֵ����1000����ͨ��
    {
        if (way_temp > 0)
        {
            way_temp --;

            OS_ENTER_CRITICAL();
            CHANNEL(RTnum, way_temp);       //�趨��1ͨ��
            OS_EXIT_CRITICAL();

            OSTimeDly(10);                  //������ʱʱ�䣬��̽�����͵�·�йأ���Ӱ�쵽�л�ʱ�䡣ȡ����Ϊ�ˡ�
            goto ADC_STAR;
        }
    }

    ADC_just_temp = (float)ADC_Data;
    //����У׼����ϵ��
//    if (EPROM.ADC_just[RTnum][EPROM.wavelength[RTnum]] > 0)
//    {
//        for (i = 0 ; i < EPROM.ADC_just[RTnum][EPROM.wavelength[RTnum]] ; i++)
//        {
//            ADC_just_temp = ADC_just_temp * 1.023293;
//        }
//    }
//    else if (EPROM.ADC_just[RTnum][EPROM.wavelength[RTnum]] < 0)
//    {
//        for (i = 0 ; i < (~(EPROM.ADC_just[RTnum][EPROM.wavelength[RTnum]] - 1)) ; i++)
//        {
//            ADC_just_temp = ADC_just_temp * 0.977237;
//        }
//    }

    adcdeal(RTnum, way_temp, ADC_just_temp);
    hardware_way[RTnum] = way_temp;   //�ٰ���ʵ�ĵ�λ�ŵ�ȫ�ֱ��� hardware_way
}

