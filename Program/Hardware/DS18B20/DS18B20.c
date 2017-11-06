#include "main.h"
#include "DS18B20.h"
#include "delay.h"


uint8 T_18b20flag;

/*********************************************************************************************************
** ��������: DS18B20_Reset()
** ��������: DS18B20��λ
** �䡡��:   Ack��DS18B20״̬
********************************************************************************************************/
uint8 DS18B20_Reset(void)
{
  uint8 Ack;
	
//	DQ_OUT;
	DQ_L;
	delay_nus(700);
	DQ_H;
	
	delay_nus(75);				//��������>480us��λ�����豸
	DQ_IN;
	delay_nus(1);
	Ack = DQ;
/*	if(Ack)
	{
        UART0Write_Str("ER\n");    
    }
    else
    {
        UART0Write_Str("OK\n");    
    }
    */
    DQ_OUT;
	delay_nus(500);	
	DQ_H;
	//delay_nus(2);
	return Ack;
}

/*********************************************************************************************************
** ��������: DS18B20_Write()
** ��������: ��DS18B20дָ��
** �䡡��:   cmd��DS18B20����ָ��
********************************************************************************************************/
void DS18B20_Write(uint8 cmd)
{
  uint8 i,data;  
  
	for(i=0;i<8;i++)  
	{  
		data = cmd>>i;
		data &= 0x01;
		//DQ_OUT;
		DQ_L;
		delay_nus(2);
		if(data==1)
		{
			DQ_H;
		}
		else
		{
			DQ_L;
		}
		delay_nus(65);
		DQ_H;
		delay_nus(2);
	}

}

/*********************************************************************************************************
** ��������: uint8 DS18B20_Read(void)
** ��������: ��DS18B20������
** �䡡��:   data��DS18B20����
********************************************************************************************************/
uint8 DS18B20_Read(void)
{
	uint8 i; 
	uint8 data=0;

	for(i=0;i<8;i++)
	{
		//DQ_OUT;
    DQ_L;
    delay_nus(2);
	  DQ_H;
		
	  delay_nus(10);
	  DQ_IN;
	  delay_nus(1);
		if(DQ==1)
		{
            data |= 0x01<<i;
		}
		else
		{
            data |= 0x00<<i;
		}
        
		delay_nus(60);
		DQ_OUT;
		delay_nus(1);
		DQ_H;
	}
	return data;
}

/*********************************************************************************************************
** ��������: float DS18B20_GetTemp(uint8 T)
** ��������: ��ȡ�¶�ֵ
** ��  �룺  c���������±�����¶ȣ�t���Ի����±�����¶�
** �䡡��:   temperature_c�������¶ȣ���temperature_f�������¶ȣ�
********************************************************************************************************/
uint16 DS18B20_GetTemperature(uint8 T)
{  
	 int temperature_c,temperature_f;
	 uint8 temperature_msb,temperature_lsb;
	 float tt;
   DS18B20_Reset();
	 DS18B20_Write(0xcc);
	 DS18B20_Write(0x44);
	// delay_nus(110);
	 DS18B20_Reset();
	 DS18B20_Write(0xcc);
	 DS18B20_Write(0xbe);
	 temperature_lsb = DS18B20_Read();
	// delay_nus(5);
	 temperature_msb = DS18B20_Read();
/*	 if(temperature_msb <= 0x80)
	 {
	   temperature_lsb = temperature_lsb/2;
	 }
	 temperature_msb = temperature_msb & 0x80;
	 if(temperature_msb >= 0x80)
	 {
	   temperature_lsb = (~temperature_lsb)+1;
		 temperature_lsb = (temperature_lsb/2);
		 temperature_lsb = ((-1)*temperature_lsb);
	 }
	 temperature_c = (int)temperature_lsb;
	 temperature_f = 32+temperature_c*9/5;
    if(T==0)
	 return temperature_c;
	 else 
	 return temperature_f;
	 */
    temperature_c = temperature_msb;                //�ȰѸ߰�λ��Ч���ݸ���temp
    temperature_c <<= 8;                            //������8λ���ݴ�temp�Ͱ�λ�Ƶ��߰�λ
    temperature_c = temperature_c|temperature_lsb;  //���ֽںϳ�һ�����ͱ���
    if(temperature_c > 2047)T_18b20flag = 0;
    else                    T_18b20flag = 1;
    
    tt = (float)temperature_c * 0.0625;     //�õ���ʵʮ�����¶�ֵ
                                    //��ΪDS18B20���Ծ�ȷ��0.0625��
                                    //���Զ������ݵ����λ�������0.0625��
    temperature_c = tt*10.0+0.5;    //�Ŵ�ʮ��
    temperature_f = 32+temperature_c*9/5;
	  if(T==0)
	  return temperature_c;
	  else 
	  return temperature_f;
}

/*********************************************************************************************************
** ��������: void DS18B20_Alarm(float Threshold_T)
** ��������: ��ȡ�¶�ֵ
** ��  �룺  Threshold_T���¶ȱ�����ֵ��c���������±�����¶ȣ�t���Ի����±�����¶�
********************************************************************************************************/
void DS18B20_Alarm(float Threshold_T,uint8 T)
{
  float temperature;
	
	temperature = DS18B20_GetTemperature(T);
	if(temperature >= Threshold_T)
	{
	  if(BEE_ON_flag ==1)
		{
		  BEE_ON;
			delay_nms(200);
			BEE_OFF;
			BEE_ON;
			delay_nms(200);
			BEE_OFF;
		}
		else BEE_OFF;
	}
}


void init_ds18b20(void)
{
    //DQ_H;                     //DQ��λ,��ҪҲ���С�
    //delay_nus(7);                  //������ʱ
    DQ_L;                    //��Ƭ����������
    delay_nus(700);                //��ȷ��ʱ��ά������480us
    DQ_H;                    //�ͷ����ߣ�������������
    delay_nus(75);                //�˴���ʱ���㹻,ȷ������DS18B20�����������塣
 /*   DQ_IN;
    delay_nus(1); 
    if(DQ)
    {
        UART0Write_Str("ER\n");    
    }
    else
    {
        UART0Write_Str("OK\n");    
    }
    DQ_OUT;
    */
    delay_nus(495);
    DQ_H;
    delay_nus(2);
}
/*********************************************
 Ds18b20_read_byte
*********************************************/
uint8 Ds18b20_read_byte()
{
    uint8 i,dat;

    for(i=8;i>0;i--)
    {
     //   DQ_H;
      //  delay_nus(2);
       DQ_L;                  //���������ͣ�Ҫ��1us֮���ͷ�����
                               //��Ƭ��Ҫ�ڴ��½��غ��15us�ڶ����ݲŻ���Ч��
       delay_nus(2);                 //����ά����1us,��ʾ��ʱ��ʼ
       dat >>= 1;               //�ô������϶�����λ���ݣ����δӸ�λ�ƶ�����λ��
       DQ_H;                  //�ͷ����ߣ��˺�DS18B20���������,�����ݴ��䵽������
       delay_nus(5);                 //��ʱ7us,�˴������Ƽ��Ķ�ʱ��ͼ�������ѿ���������ʱ��ŵ���ʱ����15us�ڵ���󲿷�
       DQ_IN;//���ó�����
       delay_nus(1);
       if(DQ==1)                   //���������в���
       {
        dat |= 0x80;            //������Ϊ1,��DQΪ1,�ǾͰ�dat�����λ��1;��Ϊ0,�򲻽��д���,����Ϊ0
       } 
       else
       {
        dat &= 0x7f;
       }
       delay_nus(65);
        DQ_OUT;
        delay_nus(1);
        DQ_H;
        delay_nus(2);
    }
    return (dat);
}
/*********************************************
 Ds18b20_write_byte
*********************************************/
void Ds18b20_write_byte(uint8 dat)
{
    uint8 i,temp;
    for(i=8;i>0;i--)
    {
     //   DQ_H;
     //   delay_nus(2);
       DQ_L;                       //��������
       delay_nus(2);               //����ά����1us,��ʾдʱ��(����д0ʱ���д1ʱ��)��ʼ
       //DQ = dat&0x01;            //���ֽڵ����λ��ʼ����
       temp = dat&0x01;            //ָ��dat�����λ���������,�������������ߺ��15us��,
       if(temp == 0x01)            //��Ϊ15us��DS18B20������߲�����
       {
          DQ_H;
       }
       else
       {
          DQ_L;
       }
       delay_nus(60);              //������дʱ���������60us
       DQ_H;                       //д���,�����ͷ�����,
       dat >>= 1;
       delay_nus(2);
    }
}


uint16 DS18B20_Get_Tmp(void)       //��ȡ�¶�get the temperature
{
    float tt;
    uint8 a,b;
    uint16 temp;
    
    init_ds18b20();                //��ʼ��
    Ds18b20_write_byte(0xcc);      //����ROMָ��
    Ds18b20_write_byte(0x44);      //�¶�ת��ָ��
    init_ds18b20();                //��ʼ��
    Ds18b20_write_byte(0xcc);      //����ROMָ��
    Ds18b20_write_byte(0xbe);      //���ݴ���ָ��
    a = Ds18b20_read_byte();       //��ȡ���ĵ�һ���ֽ�Ϊ�¶�LSB
    b = Ds18b20_read_byte();       //��ȡ���ĵ�һ���ֽ�Ϊ�¶�MSB
    init_ds18b20();
    temp = b;                      //�ȰѸ߰�λ��Ч���ݸ���temp
    temp <<= 8;                    //������8λ���ݴ�temp�Ͱ�λ�Ƶ��߰�λ
    temp = temp|a;                 //���ֽںϳ�һ�����ͱ���

    if(temp > 2047) T_18b20flag = 0;//-
    else            T_18b20flag = 1;//+
     
    tt = temp*0.0625;              //�õ���ʵʮ�����¶�ֵ
                                   //��ΪDS18B20���Ծ�ȷ��0.0625��
                                   //���Զ������ݵ����λ�������0.0625��
																		
    temp = tt*10+0.5;              //�Ŵ�ʮ��
                                   //��������Ŀ�Ľ�С������һλҲת��Ϊ����ʾ����
                                   //ͬʱ����һ���������������
    return temp;
}






