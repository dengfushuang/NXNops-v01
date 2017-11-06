/******************************************************************************

                  ��Ȩ���� (C), 2001-2013, ���ֺ������ͨ�ż������޹�˾

 ******************************************************************************
  �� �� ��   : optics_judge.c
  �� �� ��   :
  ��    ��   :
  ��������   : 2013��4��26��
  ����޸�   :
  ��������   :
  �����б�   :
******************************************************************************/
#include "config.h"
#include "main.h"
#include "optics_judge.h"
#include "ADC.h"

uint8  OldState[CHANNEL_NUM]= {0};                      //��·ͨ����ʷ״̬
uint8  Optics_Bypass_flag[CHANNEL_NUM]= {1,1,1,1};            //�⹦���ж��л����Ʊ�ʾ:  1��ʾҪ�л�����· ,0��ʾҪ�л�����·

uint32 BackOSW_Time_Tick[CHANNEL_NUM]= {0};             //���м�ʱ����
uint32 BackAuto_Time_Tick[CHANNEL_NUM]= {0};            //�����Զ�ģʽ��ʱ����


/*****************************************************************************
**�� �� ��: optics_judge
**��������: �ѵ� ch1 ͨ���Ĺ⹦�� �͵� ch2 ͨ���Ĺ⹦���жϱȽ�,
             �ж��Ƿ���ڸ���ͨ�����л�����, �ٸ��� ��·���� �� �������� �ж�
             �Ƿ�����л��⿪�ص���������Optics_Bypass_flag������ 1���� 0��
**�������: uint8 Link_num  ��·��
             uint8 ch1  ��һ��ͨ��
             uint8 ch2  �ڶ���ͨ��
**�������: ��
**�� �� ֵ: ��
*****************************************************************************/
void optics_judge(uint8 Link_num , uint8 ch1 ,uint8 ch2 )
{
    if(EPROM.Autoflag[Link_num] == 1 )                  //�Զ�ģʽ
    {
        ChannelLED(Link_num*4,1);                       //�����Զ�ģʽָʾ��
        BackAuto_Time_Tick[Link_num] = 0;	            //���� �����Զ�ģʽ��ʱ

        //*********************************************************************************/
        //***************************** R1ͨ��R2��****************************************/
        //*********************************************************************************/
        if((power[ch1] > EPROM.q_power[ch1]) && (power[ch2] <= EPROM.q_power[ch2]))
        {
            if(OldState[Link_num] != 2)
            {
                BackOSW_Time_Tick[Link_num] = 0;		            //���� ������ʱ
                //switchTime = 0;		                            //���� �л���ʱ
                //****************************************************************************/
                //��·�����жϣ�
                //1.R1, R2���޹����·
                //2.ֻҪR1�޹����·
                //3.ֻҪR2�޹����·
                //4.R1, R2��һ�޹���·
                //****************************************************************************/
                if( EPROM.OSW_Condition[Link_num]==3 || EPROM.OSW_Condition[Link_num]==4 )
                {
                    Optics_Bypass_flag[Link_num]=1;
                }
                //****************************************************************************/
                //���������жϣ�
                //1.R1, R2���й�Ż���
                //2.ֻҪR1�й�ͻ���
                //3.ֻҪR2�й�ͻ���
                //4.R1, R2��һ�й����
                //��ֹ����·������ͻ��3-2��3-4��4-2��4-4��
                //****************************************************************************/
                else if( EPROM.BackOSW_Condition[Link_num]==2 || EPROM.BackOSW_Condition[Link_num]==4 )     //���й���
                {
                    if(EPROM.accessflag[Link_num]==1)
                    {
                        if((EPROM.BackOSW_Delay[Link_num] >= 0) && (BackOSW_Time_Tick[Link_num] == 0))
                        {
                            BackOSW_Time_Tick[Link_num] = 1;    //������ʱ
                        }
                    }
                }
                OldState[Link_num] = 2;
            }

            if(BackOSW_Time_Tick[Link_num] >= (EPROM.BackOSW_Delay[Link_num] *1000 + 1) )//������ʱ ʱ�䵽��
            {
                BackOSW_Time_Tick[Link_num] =0;
                Optics_Bypass_flag[Link_num]=0;
            }
        }

        //*********************************************************************************/
        //********************************* R1�ϣ�R2ͨ************************************/
        //*********************************************************************************/
        else if((power[ch1] <= EPROM.q_power[ch1]) && (power[ch2] > EPROM.q_power[ch2]))
        {
            if(OldState[Link_num] != 1)
            {
                BackOSW_Time_Tick[Link_num] = 0;		//���� ������ʱ
                //switchTime = 0;		                //���� �л���ʱ
                //****************************************************************************/
                //��·�����жϣ�
                //1.R1��R2���޹����·
                //2.ֻҪR1�޹����·
                //3.ֻҪR2�޹����·
                //4.R1��R2��һ�޹���·
                //****************************************************************************/
                if( EPROM.OSW_Condition[Link_num]==2 || EPROM.OSW_Condition[Link_num]==4 )
                {
                    Optics_Bypass_flag[Link_num]=1;
                }
                //****************************************************************************/
                //���������жϣ�
                //1.R1��R2���й�Ż���
                //2.ֻҪR1�й�ͻ���
                //3.ֻҪR2�й�ͻ���
                //4.R1��R2��һ�й����
                //��ֹ����·������ͻ��2-3��2-4��4-3��4-4��
                //****************************************************************************/
                else if( EPROM.BackOSW_Condition[Link_num]==3 || EPROM.BackOSW_Condition[Link_num]==4 )
                {
                    if(EPROM.accessflag[Link_num]==1)
                    {
                        if( (EPROM.BackOSW_Delay[Link_num] >= 0) && (BackOSW_Time_Tick[Link_num] == 0))
                        {
                            BackOSW_Time_Tick[Link_num] = 1; //������ʱ
                        }
                    }
                }
                OldState[Link_num] = 1;
            }
            if( BackOSW_Time_Tick[Link_num]  >= (EPROM.BackOSW_Delay[Link_num] *1000 + 1) )//������ʱ ʱ�䵽��
            {
                BackOSW_Time_Tick[Link_num] =0;
                Optics_Bypass_flag[Link_num]=0;
            }
        }
        //*********************************************************************************/
        //************************************ R1�ϣ�R2��  *******************************/
        //*********************************************************************************/
        else if((power[ch1] <= EPROM.q_power[ch1]) && (power[ch2] <= EPROM.q_power[ch2]))
        {
            if(OldState[Link_num] != 0)
            {
                BackOSW_Time_Tick[Link_num] = 0;		//���� ������ʱ
                //switchTime = 0;		                //���� �л���ʱ
                //****************************************************************************/
                //��·�����жϣ�
                //1.R1��R2���޹����·
                //2.ֻҪR1�޹����·
                //3.ֻҪR2�޹����·
                //4.R1��R2��һ�޹���·
                //****************************************************************************/
                Optics_Bypass_flag[Link_num]=1;

                OldState[Link_num] = 0;
            }
        }

        //*********************************************************************************/
        //************************************ R1ͨ��R2ͨ  *******************************/
        //*********************************************************************************/
        else if((power[ch1] > EPROM.q_power[ch1]) && (power[ch2] > EPROM.q_power[ch2]))
        {
            if(OldState[Link_num] != 3)
            {
                BackOSW_Time_Tick[Link_num] = 0;		//���� ������ʱ
                //switchTime = 0;		                //���� �л���ʱ

                //���й���
                if( EPROM.accessflag[Link_num]==1 )
                {
                    if((EPROM.BackOSW_Delay[Link_num] >= 0) && (BackOSW_Time_Tick[Link_num] == 0))
                    {
                        BackOSW_Time_Tick[Link_num] = 1;  //������ʱ
                    }
                }
                OldState[Link_num] = 3;
            }
            if(BackOSW_Time_Tick[Link_num] >= (EPROM.BackOSW_Delay[Link_num] *1000 + 1) )
            {
                BackOSW_Time_Tick[Link_num] =0;
                Optics_Bypass_flag[Link_num]=0;
            }
        }
    }

    //*********************************************************************************/
    //*************************************�ֶ�ģʽ************************************/
    //*********************************************************************************/
    else if(EPROM.Autoflag[Link_num] == 0)
    {
        ChannelLED(Link_num*4,0);               //���Զ�ģʽָʾ��
        BEE_OFF;                                //�ط�����
        BackOSW_Time_Tick[Link_num]   = 0;      //���� ������ʱ
//      switchTime = 0;                         //���� �л���ʱ

        if( (EPROM.Auto_Manual_delay[Link_num] > 0) && (BackAuto_Time_Tick[Link_num] == 0) )
        {
            BackAuto_Time_Tick[Link_num] = 1;   //������ʱ
        }

        if( BackAuto_Time_Tick[Link_num] >= (EPROM.Auto_Manual_delay[Link_num] *60000 + 1) )
        {
            BackAuto_Time_Tick[Link_num] =0;
            EPROM.Autoflag[Link_num] = 1;
            Save_To_EPROM((uint8 *)&EPROM.Autoflag[Link_num], 1);

            BEE_ON;                     //��������
            OSTimeDly(50);
            BEE_OFF;                    //�ط�����
        }
    }
}

