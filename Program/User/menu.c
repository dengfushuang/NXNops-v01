/******************************************************************************

                  ��Ȩ���� (C), 2001-2013, ���ֺ������ͨ�ż������޹�˾

 ******************************************************************************
  �� �� ��   : menu.c
  �� �� ��   : ����
  ��    ��   :
  ��������   : 2013��4��28��
  ����޸�   :
  ��������   : LCD16032���ж༶�˵�
******************************************************************************/
#include "config.h"
#include "main.h"
#include "menu.h"
#include "OSW.h"
#include "lpc177x_8x_eeprom.h"

#define MENULEVEL   12                  //��ǰ�˵������
#define OPTIONMETE  LOST_MAX_4-MAIN+1   //��ǰѡ�����������һ�����ܺ���ѡ��
#define Function    OPTIONMETE+1        //���ܺ���ѡ�� 

uint8 FirstLineDisIndex = 0;            //��Ļ��һ����ʾ��������
uint8 SelectLine = 0;                   //��ǰѡ�����
uint8 SelectIndex = 0;                  //��ǰѡ���ж�Ӧ��������
uint8 LastIndex = 0;                    //���빦�ܺ���ǰ�������ţ��жϾ��幦��ʹ��
uint8 Enter_Function = 0;               //���빦�ܺ����ı�ʾ
uint8 Enter_counter=0;                  //��Enter�������ļ�������
uint8 Key_temp=0;
uint16 Key_temp16=0;
float power_temp=0;

void show_Set_ok( void );
void FunctionAction (void);
uint8 MenuCancelOption (void);
extern void  OSTimeDly (uint32 ticks);

//-------------------------------------
//  �˵���������
//-------------------------------------
typedef enum
{
    //***************************���˵�********************************/
    MAIN = 0,
    //***************************��һ���˵�****************************/
    MODE,
    OSW_STARE,
    POWER,
    WAVE,
    BACK_AUTO,
    BACK_TYPE,
    BACK_DELAY,
    ICMP_INTERVAL,
    TIME_OUT,
    LOST_MAX,
    START_DELAY,
    CMD_ADDRESS,
    LCD_BTIME,
    BEE,
    UART_BPS,
    LANGUAGE,
    RESTORE,
    //***************************�ڶ����˵�****************************/
    MODE_1,
    MODE_2,
    MODE_3,
    MODE_4,
    OSW_STARE_1,
    OSW_STARE_2,
    OSW_STARE_3,
    OSW_STARE_4,
    POWER_1,
    POWER_2,
    POWER_3,
    POWER_4,
    POWER_5,
    POWER_6,
    POWER_7,
    POWER_8,
    WAVE_1,
    WAVE_2,
    WAVE_3,
    WAVE_4,
    WAVE_5,
    WAVE_6,
    WAVE_7,
    WAVE_8,
    BACK_AUTO_1,
    BACK_AUTO_2,
    BACK_AUTO_3,
    BACK_AUTO_4,
    BACK_TYPE_1,
    BACK_TYPE_2,
    BACK_TYPE_3,
    BACK_TYPE_4,
    BACK_DELAY_1,
    BACK_DELAY_2,
    BACK_DELAY_3,
    BACK_DELAY_4,
    ICMP_INTERVAL_1,
    ICMP_INTERVAL_2,
    ICMP_INTERVAL_3,
    ICMP_INTERVAL_4,
    TIME_OUT_1,
    TIME_OUT_2,
    TIME_OUT_3,
    TIME_OUT_4,
    LOST_MAX_1,
    LOST_MAX_2,
    LOST_MAX_3,
    LOST_MAX_4,

} MENU_NAME;

//--------------------------------------
//
//  �˵����������
//
//--------------------------------------
uint8 const Level[MENULEVEL][3] =
{
    //ÿ�����Ӧ {��ʼ������, ���������� , ѡ����Ŀ}

    {MAIN, MAIN, 1},
    {MODE, RESTORE, RESTORE-MODE+1},
    {MODE_1, MODE_2, 2},
    {OSW_STARE_1, OSW_STARE_2, 2},
    {POWER_1, POWER_8, 8},

    {WAVE_1, WAVE_8, 8},
    {BACK_AUTO_1, BACK_AUTO_2, 2},
    {BACK_TYPE_1, BACK_TYPE_2, 2},
    {BACK_DELAY_1 , BACK_DELAY_2, 2},
    {ICMP_INTERVAL_1, ICMP_INTERVAL_2, 2},
    
    {TIME_OUT_1,  TIME_OUT_2, 2},
    {LOST_MAX_1, LOST_MAX_2, 2},
};
//-------------------------------------
//  �˵��ṹ����
//-------------------------------------
struct Option
{
    uint8 KeyLevel;        //�˵�ѡ�������˵����
    uint8 EnterIndex;      //ѡ�����������
    uint8 CancelIndex;     //ѡ���˳�������
    uint8 KeyWord[20];     //�˵�ѡ��������������
    //uint8 WordMete;      //�˵�ѡ�����������ֽ���
    MENU_NAME name;
};

//-------------------------------------
//
// ����˵�ѡ��壬����һ���ṹ����
// �洢�ڱ���������ʡ�ڴ�RAM
//
//-------------------------------------
struct  Option const sOption[2][OPTIONMETE] =
{
    {
        //***************************���˵�********************************/
        {0,1,0, "      ���˵�        ",MAIN},

        //***************************��һ���˵�****************************/
        {1,MODE_1,MAIN,         "1.����ģʽ          ",MODE},
        {1,OSW_STARE_1,MAIN,    "2.ͨ��ѡ��          ",OSW_STARE},
        {1,POWER_1,MAIN,        "3.�л�����          ",POWER},
        {1,WAVE_1,MAIN,         "4.����ѡ��          ",WAVE},
        {1,BACK_AUTO_1,MAIN,    "5.�ֶ������Զ���ʱ  ",BACK_AUTO},
        {1,BACK_TYPE_1,MAIN,    "6.���з�ʽ����      ",BACK_TYPE},
        {1,BACK_DELAY_1,MAIN,   "7.������ʱ����      ",BACK_DELAY},
        {1,ICMP_INTERVAL_1,MAIN,"8.�������������    ",ICMP_INTERVAL},
        {1,TIME_OUT_1,MAIN,     "9.��������ʱ����    ",TIME_OUT},
        {1,LOST_MAX_1,MAIN,     "10. ����������·����",LOST_MAX},
        {1,Function,MAIN,       "11. ������ʱ����    ",START_DELAY},
        {1,Function,MAIN,       "12. �豸ָ���ַ����",CMD_ADDRESS},
        {1,Function,MAIN,       "13. LCD ����        ",LCD_BTIME},
        {1,Function,MAIN,       "14. �澯������      ",BEE},
        {1,Function,MAIN,       "15. ����������      ",UART_BPS},
        {1,Function,MAIN,       "16. ��������        ",LANGUAGE},
        {1,Function,MAIN,       "17. �ָ���������    ",RESTORE},

        //***************************�ڶ����˵�****************************/
        {2,Function,MODE,"1.1R����ģʽ        ",MODE_1},
        {2,Function,MODE,"2.2R����ģʽ        ",MODE_2},
        {2,Function,MODE,"3.3R����ģʽ        ",MODE_3},
        {2,Function,MODE,"4.4R����ģʽ        ",MODE_4},

        {3,Function,OSW_STARE,"1.1Rͨ��ѡ��        ",OSW_STARE_1},
        {3,Function,OSW_STARE,"2.2Rͨ��ѡ��        ",OSW_STARE_2},
        {3,Function,OSW_STARE,"3.3Rͨ��ѡ��        ",OSW_STARE_3},
        {3,Function,OSW_STARE,"4.4Rͨ��ѡ��        ",OSW_STARE_4},

        {4,Function,POWER,"1.1R1 �л�����      ",POWER_1},
        {4,Function,POWER,"2.1R2 �л�����      ",POWER_2},
        {4,Function,POWER,"3.1R3 �л�����      ",POWER_3},
        {4,Function,POWER,"4.1R4 �л�����      ",POWER_4},
        {4,Function,POWER,"5.2R1 �л�����      ",POWER_5},
        {4,Function,POWER,"6.2R2 �л�����      ",POWER_6},
        {4,Function,POWER,"7.2R3 �л�����      ",POWER_7},
        {4,Function,POWER,"8.2R4 �л�����      ",POWER_8},

        {5,Function,WAVE,"1.1R1 ����ѡ��      ",WAVE_1},
        {5,Function,WAVE,"2.1R2 ����ѡ��      ",WAVE_2},
        {5,Function,WAVE,"3.1R3 ����ѡ��      ",WAVE_3},
        {5,Function,WAVE,"4.1R4 ����ѡ��      ",WAVE_4},
        {5,Function,WAVE,"5.2R1 ����ѡ��      ",WAVE_5},
        {5,Function,WAVE,"6.2R2 ����ѡ��      ",WAVE_6},
        {5,Function,WAVE,"7.2R3 ����ѡ��      ",WAVE_7},
        {5,Function,WAVE,"8.2R4 ����ѡ��      ",WAVE_8},

        {6,Function,BACK_AUTO,"1.1R�ֶ������Զ���ʱ",BACK_AUTO_1},
        {6,Function,BACK_AUTO,"2.2R�ֶ������Զ���ʱ",BACK_AUTO_2},
        {6,Function,BACK_AUTO,"3.3R�ֶ������Զ���ʱ",BACK_AUTO_3},
        {6,Function,BACK_AUTO,"4.4R�ֶ������Զ���ʱ",BACK_AUTO_4},

        {7,Function,BACK_TYPE,"1.1R���з�ʽ        ",BACK_TYPE_1},
        {7,Function,BACK_TYPE,"2.2R���з�ʽ        ",BACK_TYPE_2},
        {7,Function,BACK_TYPE,"3.3R���з�ʽ        ",BACK_TYPE_3},
        {7,Function,BACK_TYPE,"4.4R���з�ʽ        ",BACK_TYPE_4},

        {8,Function,BACK_DELAY,"1.1R������ʱ        ",BACK_DELAY_1},
        {8,Function,BACK_DELAY,"2.2R������ʱ        ",BACK_DELAY_2},
        {8,Function,BACK_DELAY,"3.3R������ʱ        ",BACK_DELAY_3},
        {8,Function,BACK_DELAY,"4.4R������ʱ        ",BACK_DELAY_4},

        {9,Function,ICMP_INTERVAL,"1.1R����������      ",ICMP_INTERVAL_1},
        {9,Function,ICMP_INTERVAL,"2.2R����������      ",ICMP_INTERVAL_2},
        {9,Function,ICMP_INTERVAL,"3.3R����������      ",ICMP_INTERVAL_3},
        {9,Function,ICMP_INTERVAL,"4.4R����������      ",ICMP_INTERVAL_4},

        {10,Function,TIME_OUT,"1.1R��������ʱ      ",TIME_OUT_1},
        {10,Function,TIME_OUT,"2.2R��������ʱ      ",TIME_OUT_2},
        {10,Function,TIME_OUT,"3.3R��������ʱ      ",TIME_OUT_3},
        {10,Function,TIME_OUT,"4.4R��������ʱ      ",TIME_OUT_4},

        {11,Function,LOST_MAX,"1.1R����������·    ",LOST_MAX_1},
        {11,Function,LOST_MAX,"2.2R����������·    ",LOST_MAX_2},
        {11,Function,LOST_MAX,"3.3R����������·    ",LOST_MAX_3},
        {11,Function,LOST_MAX,"4.4R����������·    ",LOST_MAX_4},

    },
    {
        //***************************���˵�********************************/
        {0,1,0, "      ���˵�        ",MAIN},
        //***************************��һ���˵�****************************/
        {1,MODE_1,MAIN,         "1.Working Mode      ",MODE},
        {1,OSW_STARE_1,MAIN,    "2.Channel Select    ",OSW_STARE},
        {1,POWER_1,MAIN,        "3.Power Switch      ",POWER},
        {1,WAVE_1,MAIN,         "4.Wave Select       ",WAVE},
        {1,BACK_AUTO_1,MAIN,    "5.Man to Auto delay ",BACK_AUTO},
        {1,BACK_TYPE_1,MAIN,    "6.Back Mode set     ",BACK_TYPE},
        {1,BACK_DELAY_1,MAIN,   "7.Back delay set    ",BACK_DELAY},
        {1,ICMP_INTERVAL_1,MAIN,"8.Sent ICMP gap     ",ICMP_INTERVAL},
        {1,TIME_OUT_1,MAIN,     "9.Get ICMP Timeout  ",TIME_OUT},
        {1,LOST_MAX_1,MAIN,     "10.Last lost ICMP   ",LOST_MAX},
        {1,Function,MAIN,       "11.Power-on delay   ",START_DELAY},
        {1,Function,MAIN,       "12. Command Address ",CMD_ADDRESS},
        {1,Function,MAIN,       "13. LCD Backlight   ",LCD_BTIME},
        {1,Function,MAIN,       "14. Buzzer Alarm    ",BEE},
        {1,Function,MAIN,       "15. Baud Rate Set   ",UART_BPS},
        {1,Function,MAIN,       "16. Language        ",LANGUAGE},
        {1,Function,MAIN,       "17. Restore Set     ",RESTORE},

        //***************************�ڶ����˵�****************************/
        {2,Function,MODE,"1.1R Working Mode   ",MODE_1},
        {2,Function,MODE,"2.2R Working Mode   ",MODE_2},
        {2,Function,MODE,"3.3R Working Mode   ",MODE_3},
        {2,Function,MODE,"4.4R Working Mode   ",MODE_4},

        {3,Function,OSW_STARE,"1.1R Channel Select ",OSW_STARE_1},
        {3,Function,OSW_STARE,"2.2R Channel Select ",OSW_STARE_2},
        {3,Function,OSW_STARE,"3.3R Channel Select ",OSW_STARE_3},
        {3,Function,OSW_STARE,"4.4R Channel Select ",OSW_STARE_4},

        {4,Function,POWER,"1.1R1 Power Switch  ",POWER_1},
        {4,Function,POWER,"2.1R2 Power Switch  ",POWER_2},
        {4,Function,POWER,"3.1R3 Power Switch  ",POWER_3},
        {4,Function,POWER,"4.1R4 Power Switch  ",POWER_4},
        {4,Function,POWER,"5.2R1 Power Switch  ",POWER_5},
        {4,Function,POWER,"6.2R2 Power Switch  ",POWER_6},
        {4,Function,POWER,"7.2R3 Power Switch  ",POWER_7},
        {4,Function,POWER,"8.2R4 Power Switch  ",POWER_8},

        {5,Function,WAVE,"1.1R1 Wave Select   ",WAVE_1},
        {5,Function,WAVE,"2.1R2 Wave Select   ",WAVE_2},
        {5,Function,WAVE,"3.1R3 Wave Select   ",WAVE_3},
        {5,Function,WAVE,"4.1R4 Wave Select   ",WAVE_4},
        {5,Function,WAVE,"5.2R1 Wave Select   ",WAVE_5},
        {5,Function,WAVE,"6.2R2 Wave Select   ",WAVE_6},
        {5,Function,WAVE,"7.2R3 Wave Select   ",WAVE_7},
        {5,Function,WAVE,"8.2R4 Wave Select   ",WAVE_8},

        {6,Function,BACK_AUTO,"1.1R Man to Auto DLY",BACK_AUTO_1},
        {6,Function,BACK_AUTO,"2.2R Man to Auto DLY",BACK_AUTO_2},
        {6,Function,BACK_AUTO,"3.3R Man to Auto DLY",BACK_AUTO_3},
        {6,Function,BACK_AUTO,"4.4R Man to Auto DLY",BACK_AUTO_4},

        {7,Function,BACK_TYPE,"1.1R Back Mode      ",BACK_TYPE_1},
        {7,Function,BACK_TYPE,"2.2R Back Mode      ",BACK_TYPE_2},
        {7,Function,BACK_TYPE,"3.3R Back Mode      ",BACK_TYPE_3},
        {7,Function,BACK_TYPE,"4.4R Back Mode      ",BACK_TYPE_4},

        {8,Function,BACK_DELAY,"1.1R Back delay     ",BACK_DELAY_1},
        {8,Function,BACK_DELAY,"2.2R Back delay     ",BACK_DELAY_2},
        {8,Function,BACK_DELAY,"3.3R Back delay     ",BACK_DELAY_3},
        {8,Function,BACK_DELAY,"4.4R Back delay     ",BACK_DELAY_4},

        {9,Function,ICMP_INTERVAL,"1.1R ICMP gap set   ",ICMP_INTERVAL_1},
        {9,Function,ICMP_INTERVAL,"2.2R ICMP gap set   ",ICMP_INTERVAL_2},
        {9,Function,ICMP_INTERVAL,"3.3R ICMP gap set   ",ICMP_INTERVAL_3},
        {9,Function,ICMP_INTERVAL,"4.4R ICMP gap set   ",ICMP_INTERVAL_4},

        {10,Function,TIME_OUT,"1.1R Get ICMP TIMout",TIME_OUT_1},
        {10,Function,TIME_OUT,"2.2R Get ICMP TIMout",TIME_OUT_2},
        {10,Function,TIME_OUT,"3.3R Get ICMP TIMout",TIME_OUT_3},
        {10,Function,TIME_OUT,"4.4R Get ICMP TIMout",TIME_OUT_4},

        {11,Function,LOST_MAX,"1.1R Last lost ICMP ",LOST_MAX_1},
        {11,Function,LOST_MAX,"2.2R Last lost ICMP ",LOST_MAX_2},
        {11,Function,LOST_MAX,"3.3R Last lost ICMP ",LOST_MAX_3},
        {11,Function,LOST_MAX,"4.4R Last lost ICMP ",LOST_MAX_4},
    }
};

/*****************************************************************************
**�� �� ��: FunctionAction
**��������: ���幦��ɢת����
**�������: void
**�������: ��
**�� �� ֵ: ��
*****************************************************************************/
void FunctionAction (void)
{
    unsigned short q_power_temp;

    switch ( SelectIndex )
    {
        //**************************������ʱ����**************************/
    case START_DELAY:
        if( KeyStyle == Up )
        {
            Key_temp16++;
            if(Key_temp16>9999) Key_temp16=0;
        }
        else if( KeyStyle == Dwon)
        {
            Key_temp16--;
            if(Key_temp16==0xffff)  Key_temp16=9999;
        }
        else if( KeyStyle == Enter )
        {
            if( Enter_counter == 0 )        //������Enter���Ĵ���
            {
                Enter_counter=1;
                Key_temp16=EPROM.Start_delay;
            }
            else if( Enter_counter == 1 )   //������Enter���Ĵ���
            {
                EPROM.Start_delay=Key_temp16;
                Save_To_EPROM((uint8 *)&EPROM.Start_delay, 2);
                show_Set_ok();
            }
        }
        if ( EPROM.Language )
        {
            LCD_PutStr(0,0,"PWR-ON delay:      S");
            LCD_PutStr(1,0,"                    ");
            LCD_SetCursor(0,7);
            LCD_WriteData(Key_temp16%10000/1000+'0');
            LCD_WriteData(Key_temp16%1000/100+'0');
            LCD_WriteData(Key_temp16%100/10+'0');
            LCD_WriteData(Key_temp16%10+'0');
            LCD_SetCursor(0,8);
        }
        else
        {
            LCD_PutStr(0,0,"  ������ʱ:       ��");
            LCD_PutStr(1,0,"                    ");
            LCD_SetCursor(0,6);
            LCD_WriteData(Key_temp16%10000/1000+'0');
            LCD_WriteData(Key_temp16%1000/100+'0');
            LCD_WriteData(Key_temp16%100/10+'0');
            LCD_WriteData(Key_temp16%10+'0');
            LCD_SetCursor(0,7);
        }
        break;

        //****************************�µ�ַ����**************************/
    case CMD_ADDRESS:
        if( KeyStyle == Up )
        {
            Key_temp++;
            if(Key_temp>99) Key_temp=0;
        }
        else if( KeyStyle == Dwon)
        {
            Key_temp--;
            if(Key_temp==0xff) Key_temp=99;
        }
        else if( KeyStyle == Enter )
        {
            if ( Enter_counter == 0)
            {
                Enter_counter=1;
                Key_temp = EPROM.address;
            }
            else if( Enter_counter == 1 )  //������Enter���Ĵ���
            {
                EPROM.address = Key_temp;
                Save_To_EPROM((uint8 *)&EPROM.address, 1);
                show_Set_ok();
            }
        }
        if ( EPROM.Language )
        {
            LCD_PutStr(0,0,"New Address:         ");
        }
        else
        {
            LCD_PutStr(0,0,"��ָ���ַ:         ");
        }
        LCD_PutStr(1,0,"                    ");
        LCD_SetCursor(0,6);
        LCD_WriteData(Key_temp%100/10+'0');
        LCD_WriteData(Key_temp%10+'0');
        LCD_SetCursor(0,6);
        break;

        //*************************LCD ������ʱʱ������ **************************/
    case LCD_BTIME :

        if( KeyStyle == Up )
        {
            Key_temp++;
            if(Key_temp > 6 ) Key_temp=1;
        }
        else if( KeyStyle == Dwon)
        {
            Key_temp--;
            if(Key_temp == 0 ) Key_temp=6;
        }
        else if( KeyStyle == Enter )
        {
            if ( Enter_counter == 0 )
            {
                Enter_counter = 1;
                Key_temp = EPROM.LCDT;
            }
            else if(Enter_counter++ >=1)  //������Enter���Ĵ���
            {
                EPROM.LCDT = Key_temp;
                Save_To_EPROM((uint8 *)&EPROM.LCDT, 1);
                show_Set_ok();
            }
        }
        if ( EPROM.Language )
        {
            LCD_PutStr(0,0,"15s   | 30s   |  1m ");
            LCD_PutStr(1,0," 2m   |  5m   | Long");
        }
        else
        {
            LCD_PutStr(0,0,"15��  | 30��  |  1��");
            LCD_PutStr(1,0," 2��  |  5��  | ����");
        }
        switch ( Key_temp )
        {
        case 1 :
            LCD_SetCursor(0,0);
            break;
        case 2 :
            LCD_SetCursor(0,4);
            break;
        case 3 :
            LCD_SetCursor(0,8);
            break;
        case 4 :
            LCD_SetCursor(1,0);
            break;
        case 5 :
            LCD_SetCursor(1,4);
            break;
        case 6 :
            LCD_SetCursor(1,8);
            break;
        default:
            break;
        }
        break;

        //***************************�澯���������� **************************/
    case BEE :
        if( KeyStyle == Up || KeyStyle == Dwon)
        {
            Key_temp=(!Key_temp);
        }
        else if( KeyStyle == Enter )
        {
            if( Enter_counter == 0 )
            {
                Key_temp = EPROM.BEEflag;
                Enter_counter = 1;
            }
            else if( Enter_counter == 1 )
            {
                EPROM.BEEflag=Key_temp;
                Save_To_EPROM((uint8 *)&EPROM.BEEflag, 1);
                show_Set_ok();
            }
        }
        if ( EPROM.Language )
        {
            LCD_PutStr(0,0,"1.Turn on           ");
            LCD_PutStr(1,0,"2.Turn off          ");
        }
        else
        {
            LCD_PutStr(0,0,"1.����              ");
            LCD_PutStr(1,0,"2.�ر�              ");
        }
        LCD_SetCursor((!Key_temp),0);
        break;

        // *************************����������   **************************/
    case UART_BPS :
        if( KeyStyle == Up )
        {
            Key_temp++;
            if(Key_temp > 9 ) Key_temp=1;
        }
        else if( KeyStyle == Dwon)
        {
            Key_temp--;
            if(Key_temp == 0 ) Key_temp=9;
        }
        else if( KeyStyle == Enter )
        {
            if( Enter_counter == 0 )
            {
                Key_temp = EPROM.BPS;
                Enter_counter = 1;
            }
            else if( Enter_counter == 1 )
            {
                EPROM.BPS=Key_temp;
                Save_To_EPROM((uint8 *)&EPROM.BPS, 1);
                show_Set_ok();
            }
        }
        if ( EPROM.Language )
        {
            LCD_PutStr(0,0,"Baud rate:          ");
            LCD_PutStr(1,0,"Setting must restart");
        }
        else
        {
            LCD_PutStr(0,0,"  ������:           ");
            LCD_PutStr(1,0,"  ���ú���������Ч  ");
        }

        switch ( Key_temp )
        {
        case 1 :
            LCD_PutStr(0,5,"2400");
            break;
        case 2 :
            LCD_PutStr(0,5,"4800");
            break;
        case 3 :
            LCD_PutStr(0,5,"9600");
            break;
        case 4 :
            LCD_PutStr(0,5,"14400");
            break;
        case 5 :
            LCD_PutStr(0,5,"19200");
            break;
        case 6 :
            LCD_PutStr(0,5,"38400");
            break;
        case 7 :
            LCD_PutStr(0,5,"56000");
            break;
        case 8 :
            LCD_PutStr(0,5,"57600");
            break;
        case 9 :
            LCD_PutStr(0,5,"115200");
            break;
        default:
            break;
        }
        LCD_SetCursor(0,5);
        break;

        // *************************��������   **************************/
    case LANGUAGE :
        if( KeyStyle == Up || KeyStyle == Dwon)
        {
            Key_temp=(!Key_temp);
        }
        else if( KeyStyle == Enter )
        {
            if( Enter_counter == 0 )
            {
                Key_temp = EPROM.Language;
                Enter_counter = 1;
            }
            else if( Enter_counter == 1 )
            {
                EPROM.Language=Key_temp;
                Save_To_EPROM((uint8 *)&EPROM.Language, 1);
                show_Set_ok();
            }
        }
        LCD_PutStr(0,0,"1.����              ");
        LCD_PutStr(1,0,"2.English           ");
        LCD_SetCursor(Key_temp,0);
        break;

        // *************************�ָ���������   **************************/
    case RESTORE :
        if( KeyStyle == Up || KeyStyle == Dwon)
        {
            Key_temp=(!Key_temp);
        }
        else if( KeyStyle == Enter )
        {
            if( Enter_counter == 0 )
            {
                Enter_counter = 1;
                Key_temp = 0;
            }
            else if( Enter_counter == 1 )
            {
                if(Key_temp==1)
                {
                    restore_set();          //�ָ���������
                    if ( EPROM.Language )
                    {
                        LCD_PutStr(0,0,"   Restore Set OK   ");
                        LCD_PutStr(1,0,"                    ");
                    }
                    else
                    {
                        LCD_PutStr(0,0,"  �ָ��������óɹ�  ");
                        LCD_PutStr(1,0,"                    ");
                    }
                    OSTimeDly(1000);

                    Reset_Handler();    //��λ !!!!!!!!!!!!!!
                }
                else
                {
                    MenuCancelOption();
                    break;
                }

            }
        }
        if ( EPROM.Language )
        {
            LCD_PutStr(0,0,"1.Cancel            ");
            LCD_PutStr(1,0,"2.Confirm           ");
        }
        else
        {
            LCD_PutStr(0,0,"1.ȡ��              ");
            LCD_PutStr(1,0,"2.ȷ��              ");
        }
        LCD_SetCursor(Key_temp,0);
        break;

        // *************************4·�Ĺ���ģʽ ����  **************************/
    case MODE_1 :
    case MODE_2 :
    case MODE_3 :
    case MODE_4 :
        if( KeyStyle == Up || KeyStyle == Dwon)
        {
            Key_temp=(!Key_temp);
        }
        else if( KeyStyle == Enter )
        {
            if( Enter_counter == 0 )
            {
                Key_temp = EPROM.Autoflag[SelectIndex-MODE_1];
                Enter_counter = 1;
            }
            else if( Enter_counter == 1 )
            {
                EPROM.Autoflag[SelectIndex-MODE_1]=Key_temp;
                Save_To_EPROM((uint8 *)&EPROM.Autoflag[SelectIndex-MODE_1], 1);
                show_Set_ok();
            }
        }
        if ( EPROM.Language )
        {
            LCD_PutStr(0,0,"1.Auto Mode         ");
            LCD_PutStr(1,0,"2.Manual Mode       ");
        }
        else
        {
            LCD_PutStr(0,0,"1.�Զ�ģʽ          ");
            LCD_PutStr(1,0,"2.�ֶ�ģʽ          ");
        }

        LCD_SetCursor((!Key_temp),0);
        break;

        // ***************************4·��ͨ��ѡ�� ����  **************************/
    case OSW_STARE_1 :
    case OSW_STARE_2 :
    case OSW_STARE_3 :
    case OSW_STARE_4 :
        if( KeyStyle == Up || KeyStyle == Dwon )
        {
            Key_temp=(!Key_temp);
        }
        else if( KeyStyle == Enter )
        {
            if( Enter_counter == 0 )
            {
                Key_temp = OSW_state[SelectIndex-OSW_STARE_1];
                Enter_counter = 1;
            }
            else if( Enter_counter == 1 )
            {
                //�ȸ���Ϊ�ֶ�ģʽ
                EPROM.Autoflag[SelectIndex-OSW_STARE_1] = 0;
                Save_To_EPROM((uint8 *)&EPROM.Autoflag[SelectIndex-OSW_STARE_1], 1);

                if ( Key_temp == 1)         MostSwitch(SelectIndex-OSW_STARE_1);
                else if ( Key_temp == 0)    BypassSwitch(SelectIndex-OSW_STARE_1);
                show_Set_ok();
            }
        }
        if ( EPROM.Language )
        {
            LCD_PutStr(0,0,"1.Primary           ");
            LCD_PutStr(1,0,"2.Secondary         ");
        }
        else
        {
            LCD_PutStr(0,0,"1.��·-Pri (����)   ");
            LCD_PutStr(1,0,"2.��·-BPS          ");
        }
        LCD_SetCursor((!Key_temp),0);
        break;

        //*******************************�л����� ���� **************************/
    case POWER_1 :
    case POWER_2 :
    case POWER_3 :
    case POWER_4 :
    case POWER_5 :
    case POWER_6 :
    case POWER_7 :
    case POWER_8 :
        if( KeyStyle == Up )
        {
            power_temp += 0.5;
            if(power_temp > 30.0)  power_temp = -50.0;
        }
        else if( KeyStyle == Dwon)
        {
            power_temp -= 0.5;
            if(power_temp < -50.0)  power_temp = 30.0;
        }
        else if( KeyStyle == Enter )
        {
            if ( Enter_counter == 0 )
            {
                Enter_counter = 1;
                power_temp = EPROM.q_power[SelectIndex-POWER_1];
            }
            else if( Enter_counter == 1 )                 //������Enter���Ĵ���
            {
                EPROM.q_power[SelectIndex-POWER_1]=power_temp;
                Save_To_EPROM((uint8 *)&EPROM.q_power[SelectIndex-POWER_1], 4);
                show_Set_ok();
            }
        }
        if ( EPROM.Language )
        {
            LCD_PutStr(0,0,"SwitchPWR:       dBm");
            LCD_PutStr(1,0,"                    ");
        }
        else
        {
            LCD_PutStr(0,0,"�л�����:        dBm");
            LCD_PutStr(1,0,"                    ");
        }
        LCD_SetCursor(0,5);
        if(power_temp <0 )
            LCD_WriteData('-');
        q_power_temp = power_temp*100;           //�Ŵ�100��������ʾ
        LCD_WriteData(q_power_temp%10000/1000+'0');
        LCD_WriteData(q_power_temp%1000/100+'0');
        LCD_WriteData('.');
        LCD_WriteData(q_power_temp%100/10+'0');
        LCD_WriteData(q_power_temp%10+'0');
        LCD_SetCursor(0,5);
        break;

        //************************************����ѡ��**************************/
    case WAVE_1 :
    case WAVE_2 :
    case WAVE_3 :
    case WAVE_4 :
    case WAVE_5 :
    case WAVE_6 :
    case WAVE_7 :
    case WAVE_8 :
        if( KeyStyle == Up || KeyStyle == Dwon )
        {
            Key_temp=(!Key_temp);
        }
        else if( KeyStyle == Enter )
        {
            if ( Enter_counter == 0 )
            {
                Enter_counter = 1;
                Key_temp = EPROM.wavelength[SelectIndex-WAVE_1];
            }
            else if(Enter_counter == 1 )  //������Enter���Ĵ���
            {
                EPROM.wavelength[SelectIndex-WAVE_1] = Key_temp;
                Save_To_EPROM((uint8 *)&EPROM.wavelength[SelectIndex-WAVE_1], 1);
                show_Set_ok();
            }
        }
        if ( EPROM.Language )
        {
            LCD_PutStr(0,0,"Wave Select:850nm   ");
            LCD_PutStr(1,0,"Wave Select:1550nm  ");
        }
        else
        {
            LCD_PutStr(0,0,"  ����ѡ��: 850nm   ");
            LCD_PutStr(1,0,"  ����ѡ��: 1550nm  ");
        }
        LCD_SetCursor(Key_temp,7);
        break;

        //*********************************�ֶ������Զ���ʱ*************************/
    case BACK_AUTO_1 :
    case BACK_AUTO_2 :
    case BACK_AUTO_3 :
    case BACK_AUTO_4 :
        if( KeyStyle == Up )
        {
            Key_temp16++;
            if(Key_temp16>9999) Key_temp16=0;
        }
        else if( KeyStyle == Dwon)
        {
            Key_temp16--;
            if(Key_temp16==0xffff)  Key_temp16=9999;
        }
        else if( KeyStyle == Enter )
        {
            if ( Enter_counter == 0 )
            {
                Enter_counter=1;
                Key_temp16 = EPROM.Auto_Manual_delay[SelectIndex-BACK_AUTO_1];
            }
            else if(Enter_counter==1)  //������Enter���Ĵ���
            {
                EPROM.Auto_Manual_delay[SelectIndex-BACK_AUTO_1] = Key_temp16;
                Save_To_EPROM((uint8 *)&EPROM.Auto_Manual_delay[SelectIndex-BACK_AUTO_1], 2);
                show_Set_ok();
            }
        }
        if ( EPROM.Language )
        {
            LCD_PutStr(0,0,"Return Time:     m  ");
            LCD_PutStr(1,0,"0000m is no return  ");
        }
        else
        {
            LCD_PutStr(0,0,"  ����ʱ��:     ����");
            LCD_PutStr(1,0,"0000����Ϊ���Զ�����");
        }
        LCD_SetCursor(0,6);
        LCD_WriteData(Key_temp16%10000/1000+'0');
        LCD_WriteData(Key_temp16%1000/100+'0');
        LCD_WriteData(Key_temp16%100/10+'0');
        LCD_WriteData(Key_temp16%10+'0');
        LCD_SetCursor(0,7);
        break;

        //**********************************���з�ʽ����**************************/
    case BACK_TYPE_1 :
    case BACK_TYPE_2 :
    case BACK_TYPE_3 :
    case BACK_TYPE_4 :
        if( KeyStyle == Up || KeyStyle == Dwon)
        {
            Key_temp = (!Key_temp);
        }
        else if( KeyStyle == Enter )
        {
            if ( Enter_counter == 0 )
            {
                Enter_counter = 1;
                Key_temp = EPROM.accessflag[SelectIndex-BACK_TYPE_1];
            }
            else if(Enter_counter==1)  //������Enter���Ĵ���
            {
                EPROM.accessflag[SelectIndex-BACK_TYPE_1] = Key_temp;
                Save_To_EPROM((uint8 *)&EPROM.accessflag[SelectIndex-BACK_TYPE_1], 1);
                show_Set_ok();
            }
        }
        if ( EPROM.Language )
        {
            LCD_PutStr(0,0,"1.Auto Back         ");
            LCD_PutStr(1,0,"2.Manual Back       ");
        }
        else
        {
            LCD_PutStr(0,0,"1.�Զ�����          ");
            LCD_PutStr(1,0,"2.���Զ�����        ");
        }
        LCD_SetCursor( (!Key_temp), 0);
        break;

        //**********************************������ʱ����**************************/
    case BACK_DELAY_1 :
    case BACK_DELAY_2 :
    case BACK_DELAY_3 :
    case BACK_DELAY_4 :
        if( KeyStyle == Up )
        {
            Key_temp16++;
            if(Key_temp16>9999) Key_temp16=0;
        }
        else if( KeyStyle == Dwon)
        {
            Key_temp16--;
            if(Key_temp16==0xffff)  Key_temp16=9999;
        }
        else if( KeyStyle == Enter )
        {
            if ( Enter_counter==0 )
            {
                Enter_counter=1;
                Key_temp16 = EPROM.BackOSW_Delay[SelectIndex-BACK_DELAY_1];
            }
            else if(Enter_counter==1)  //������Enter���Ĵ���
            {
                EPROM.BackOSW_Delay[SelectIndex-BACK_DELAY_1]=Key_temp16;
                Save_To_EPROM((uint8 *)&EPROM.BackOSW_Delay[SelectIndex-BACK_DELAY_1], 2);
                show_Set_ok();
            }
        }
        if ( EPROM.Language )
        {
            LCD_PutStr(0,0,"Back delay:      S  ");
            LCD_PutStr(1,0,"                    ");
        }
        else
        {
            LCD_PutStr(0,0,"  ����ʱ��:       ��");
            LCD_PutStr(1,0,"                    ");
        }
        LCD_SetCursor(0,6);
        LCD_WriteData(Key_temp16%10000/1000+'0');
        LCD_WriteData(Key_temp16%1000/100+'0');
        LCD_WriteData(Key_temp16%100/10+'0');
        LCD_WriteData(Key_temp16%10+'0');
        LCD_SetCursor(0,7);
        break;


        //********************************����ʱ�������� **************************/
    case ICMP_INTERVAL_1 :
    case ICMP_INTERVAL_2 :
    case ICMP_INTERVAL_3 :
    case ICMP_INTERVAL_4 :
        if( KeyStyle == Up )
        {
            Key_temp16++;
            if(Key_temp16>9999) Key_temp16=0;
        }
        else if( KeyStyle == Dwon)
        {
            Key_temp16--;
            if(Key_temp16==0xffff)  Key_temp16=9999;
        }
        else if( KeyStyle == Enter )
        {
            if ( Enter_counter == 0 )
            {
                Enter_counter=1;
                Key_temp16 = EPROM.Send_ICMP_interval[SelectIndex-ICMP_INTERVAL_1];
            }
            else if(Enter_counter==1)  //������Enter���Ĵ���
            {
                EPROM.Send_ICMP_interval[SelectIndex-ICMP_INTERVAL_1] = Key_temp16;
                Save_To_EPROM((uint8 *)&EPROM.Send_ICMP_interval[SelectIndex-ICMP_INTERVAL_1], 2);
                show_Set_ok();
            }
        }
        if ( EPROM.Language )
        {
            LCD_PutStr(0,0,"Gap Time:     (10ms)");
            LCD_PutStr(1,0,"                    ");
        }
        else
        {
            LCD_PutStr(0,0,"���ʱ��:     (10ms)");
            LCD_PutStr(1,0,"                    ");
        }
        LCD_SetCursor(0,5);
        LCD_WriteData(Key_temp16%10000/1000+'0');
        LCD_WriteData(Key_temp16%1000/100+'0');
        LCD_WriteData(Key_temp16%100/10+'0');
        LCD_WriteData(Key_temp16%10+'0');
        LCD_SetCursor(0,6);
        break;

        //***********************************���ճ�ʱ����  **************************/
    case TIME_OUT_1 :
    case TIME_OUT_2 :
    case TIME_OUT_3 :
    case TIME_OUT_4 :
        if( KeyStyle == Up )
        {
            Key_temp16++;
            if(Key_temp16>9999) Key_temp16=0;
        }
        else if( KeyStyle == Dwon)
        {
            Key_temp16--;
            if(Key_temp16==0xffff)  Key_temp16=9999;
        }
        else if( KeyStyle == Enter )
        {
            if ( Enter_counter == 0 )
            {
                Enter_counter=1;
                Key_temp16 = EPROM.time_out[SelectIndex-TIME_OUT_1];
            }
            else if( Enter_counter == 1 )  //������Enter���Ĵ���
            {
                EPROM.time_out[SelectIndex-TIME_OUT_1] = Key_temp16;
                Save_To_EPROM((uint8 *)&EPROM.time_out[SelectIndex-TIME_OUT_1], 2);
                show_Set_ok();
            }
        }

        if ( EPROM.Language )
        {
            LCD_PutStr(0,0,"Timeout:      (10ms)");
            LCD_PutStr(1,0,"                    ");
        }
        else
        {
            LCD_PutStr(0,0,"��ʱʱ��:     (10ms)");
            LCD_PutStr(1,0,"                    ");
        }
        LCD_SetCursor(0,5);
        LCD_WriteData(Key_temp16%10000/1000+'0');
        LCD_WriteData(Key_temp16%1000/100+'0');
        LCD_WriteData(Key_temp16%100/10+'0');
        LCD_WriteData(Key_temp16%10+'0');
        LCD_SetCursor(0,6);
        break;

        //*********************************����������·����  **************************/
    case LOST_MAX_1 :
    case LOST_MAX_2 :
    case LOST_MAX_3 :
    case LOST_MAX_4 :
        if( KeyStyle == Up )
        {
            Key_temp16++;
            if(Key_temp16>9999) Key_temp16=0;
        }
        else if( KeyStyle == Dwon)
        {
            Key_temp16--;
            if(Key_temp16==0xffff)  Key_temp16=9999;
        }
        else if( KeyStyle == Enter )
        {
            if ( Enter_counter == 0 )
            {
                Enter_counter=1;
                Key_temp16 = EPROM.lost_max[SelectIndex-LOST_MAX_1];
            }
            else if(Enter_counter==1)  //������Enter���Ĵ���
            {
                EPROM.lost_max[SelectIndex-LOST_MAX_1] = Key_temp16;
                Save_To_EPROM((uint8 *)&EPROM.lost_max[SelectIndex-LOST_MAX_1], 2);
                show_Set_ok();
            }
        }
        if ( EPROM.Language )
        {
            LCD_PutStr(0,0,"Last lost ICMP:     ");
            LCD_PutStr(1,0,"                    ");
            LCD_SetCursor(0,8);
            LCD_WriteData(Key_temp16%10000/1000+'0');
            LCD_WriteData(Key_temp16%1000/100+'0');
            LCD_WriteData(Key_temp16%100/10+'0');
            LCD_WriteData(Key_temp16%10+'0');
            LCD_SetCursor(0,9);
        }
        else
        {
            LCD_PutStr(0,0,"��������:           ");
            LCD_PutStr(1,0,"                    ");
            LCD_SetCursor(0,5);
            LCD_WriteData(Key_temp16%10000/1000+'0');
            LCD_WriteData(Key_temp16%1000/100+'0');
            LCD_WriteData(Key_temp16%100/10+'0');
            LCD_WriteData(Key_temp16%10+'0');
            LCD_SetCursor(0,6);
        }
        break;

    default:
        break;
    }
}

/*****************************************************************************
**�� �� ��: show_Set_ok
**��������: ��ʾ ���óɹ� ����
**�������: void
**�������: ��
**�� �� ֵ: ��
*****************************************************************************/
void show_Set_ok( void )
{
    if ( EPROM.Language )
    {
        LCD_PutStr(0,0,"                    ");
        LCD_PutStr(1,0,"      Set OK        ");
    }
    else
    {
        LCD_PutStr(0,0,"                    ");
        LCD_PutStr(1,0,"      ���óɹ�      ");
    }
    OSTimeDly(1000);
}

/*****************************************************************************
**�� �� ��: DISlm16032
**��������: ˢ�²˵����ܺ���
**�������: void
**�������: ��
**�� �� ֵ: ��
*****************************************************************************/
void DISlm16032 (void)
{
    uint8 i;
    uint8 LineMete;

    LineMete = Level[sOption[EPROM.Language][SelectIndex].KeyLevel][2];     //ѭ����,��ʾ����
    LCD_WriteCmd(0x30);                                     //����Һ����ָͨ��ģʽ
    LCD_WriteCmd(0x01);                                     //����
    OSTimeDly(2);
    LCD_WriteCmd(0x06);                                     //ENTRY MODE

    do                                                      //�ֱ���ʾ���в˵���
    {
        LCD_SetCursor(0,0);
        for(i=0; i<20; i++)
            LCD_WriteData(sOption[EPROM.Language][FirstLineDisIndex].KeyWord[i]);
        if(--LineMete == 0) break;
        LCD_SetCursor(1,0);
        for(i=0; i<20; i++)
            LCD_WriteData(sOption[EPROM.Language][FirstLineDisIndex+1].KeyWord[i]);
        if(--LineMete == 0) break;
    }
    while(0);
    LCD_WriteCmd(0x0d);                                     //������ʾ��,�����ʾ��,��귴����ʾ��
    LCD_SetCursor(SelectLine-1,0);                          //��ѡ���й�귴����ʾ
}

/*****************************************************************************
**�� �� ��: MenuLevelStart
**��������: �жϵ�ǰ�����Ƿ��Ǳ���һ��ܺ���
**�������: void
**�������: ��
**�� �� ֵ: ����Bit��־���ǵ�һ���1�����Ƿ���0
*****************************************************************************/
uint8 MenuLevelStart (void)    //�жϵ�ǰ�����Ƿ�Ϊ��ǰ���һ��ѡ��
{
    uint8 i = MENULEVEL;
    do
    {
        i--;
        if(SelectIndex == Level[i][0]) return 1;
    }
    while(i);
    return 0;
}

/*****************************************************************************
**�� �� ��: MenuLevelEnd
**��������: �жϵ�ǰ�����Ƿ��Ǳ����һ��ܺ���
**�������: void
**�������: ��
**�� �� ֵ: û�в���������Bit��־�������һ���1�����Ƿ���0
*****************************************************************************/
uint8 MenuLevelEnd (void)
{
    uint8 i = MENULEVEL;
    do
    {
        i--;
        if(SelectIndex == Level[i][1]) return 1;
    }
    while(i);
    return 0;
}

/*****************************************************************************
**�� �� ��: MenuUpOneOption
**��������: �˵�����һ���
**�������: void
**�������: ��
**�� �� ֵ: ��
*****************************************************************************/
void MenuUpOneOption (void)
{
    if(Enter_Function != 1)
    {
        if(MenuLevelStart ())                                                   //�����ǰΪ����һ��
        {
            if(Level[sOption[EPROM.Language][SelectIndex].KeyLevel][2]>=2)                      //���ұ���ѡ����Ŀ���ڵ���2��
            {
                FirstLineDisIndex = Level[sOption[EPROM.Language][SelectIndex].KeyLevel][1]-1;  //��һ����ʾ������Ϊ�����ڶ���
                SelectIndex = Level[sOption[EPROM.Language][SelectIndex].KeyLevel][1];          //ѡ������Ϊ�����һ��
                SelectLine = 2;                                                 //���ѡ����Ϊ�ڶ���
                //DISlm16032 ();                                                //ˢ����Ļ��ʾ
            }
            else                                                                //���ѡ����Ŀ���������ĸ�
            {
                SelectIndex = Level[sOption[EPROM.Language][SelectIndex].KeyLevel][1];          //ѡ������Ϊ��ǰ�����һ��
                SelectLine = Level[sOption[EPROM.Language][SelectIndex].KeyLevel][2];           //��ʾ�б���Ŀ(���һ��)
                //DISlm16032 ();                                                //ˢ����Ļ��ʾ
            }
        }
        else                                                                    //�����ǰ���ǿ�ʼ����
        {
            if(SelectLine==1)                                                   //�����Ѿ�����Ļ���ϱ�һ��
            {
                FirstLineDisIndex--;                //��ʾ��������
                SelectIndex--;                      //ѡ�������Լ�
                SelectLine = 1;                     //ѡ���л��ǵ�һ��
                //DISlm16032 ();                    //ˢ����Ļ
            }
            else                                    //������ǵ�һ��
            {
                SelectLine--;                       //ѡ�����Լ�
                SelectIndex--;                      //ѡ�������Լ�
                //DISlm16032 ();                    //ˢ����Ļ��ʾ
            }
        }

        DISlm16032 ();                              //ˢ����Ļ��ʾ
    }
    else
    {
        FunctionAction ();                          //����ǹ���ѡ������빦�ܷ�֧�жϺ���
    }
}

/*****************************************************************************
**�� �� ��: MenuDownOneOption
**��������: �˵�����һ���
**�������: void
**�������: ��
**�� �� ֵ: ��
*****************************************************************************/
void MenuDownOneOption (void)
{
    if(Enter_Function != 1)
    {
        if(MenuLevelEnd ())                     //�����ǰ�Ǳ����һ������
        {
            FirstLineDisIndex = Level[sOption[EPROM.Language][SelectIndex].KeyLevel][0];    //��һ����ʾ����Ϊ����һ��ѡ��
            SelectIndex = Level[sOption[EPROM.Language][SelectIndex].KeyLevel][0];          //ѡ������Ϊ����һ��ѡ������
            SelectLine = 1;                     //ѡ����Ϊ��һ��
            //DISlm16032 ();                    //ˢ����ʾ
        }
        else                                    //���������������
        {
            if(SelectLine!=2)                   //�����ǰ������Ļ�����
            {
                SelectIndex++;                  //ѡ�������Լ�
                SelectLine++;                   //ѡ��������
                //DISlm16032 ();                  //ˢ����ʾ
            }
            else                                //�������Ļ�����
            {
                FirstLineDisIndex++;            //��һ����ʾ����
                SelectIndex++;                  //ѡ�������Լ�
                //DISlm16032();                 //ˢ����ʾ
            }
        }

        DISlm16032();                           //ˢ����ʾ
    }
    else
    {
        FunctionAction ();                      //����ǹ���ѡ������빦�ܷ�֧�жϺ���
    }
}

/*****************************************************************************
**�� �� ��: MenuEnterOption
**��������: ����ĳ��ܺ���
**�������: void
**�������: ��
**�� �� ֵ: ��
*****************************************************************************/
void MenuEnterOption (void)
{
    if(Enter_Function != 1)
    {
        if(sOption[EPROM.Language][SelectIndex].EnterIndex == Function)
        {
            LastIndex = SelectIndex;                            //��ǽ���ǰ�������ţ��Ա��жϾ��幦�ܣ�
            Enter_Function = 1;                                 //��ǽ���ɢת����
            FunctionAction ();                                  //����ǹ���ѡ������빦�ܷ�֧�жϺ���
        }
        else
        {
            SelectIndex = sOption[EPROM.Language][SelectIndex].EnterIndex;      //����ѡ������Ϊ֮ǰ�����Ŷ�Ӧ��������
            if(SelectIndex != Function)                         //�����ǰ�������ǹ���ѡ������
            {
                FirstLineDisIndex = Level[sOption[EPROM.Language][SelectIndex].KeyLevel][0]; //��һ����ʾΪ�������һ��
                SelectLine = 1;                                 //�趨��һ��Ϊѡ����
                DISlm16032 ();                                  //ˢ�²˵�
            }
        }
    }
    else
    {
        FunctionAction ();                                      //����ǹ���ѡ������빦�ܷ�֧�жϺ���
    }
}

/*****************************************************************************
**�� �� ��: MenuCancelOption
**��������:  �˵��˳����ܺ���
**�������: void
**�������: ��
**�� �� ֵ: û�в�����û�з���ֵ
*****************************************************************************/
uint8 MenuCancelOption (void)
{
    if(Enter_Function != 1)
    {
        SelectIndex = sOption[EPROM.Language][SelectIndex].CancelIndex; //ѡ������Ϊѡ�������
        if ( SelectIndex == 0 )
        {
            Menu_layer=0;                                              //�˳��ص� 0���˵�(���˵�)
            Trends_display=1;
            return 0;
        }
    }
    else
    {
        SelectIndex = LastIndex;                                    //��ǰ�������ŵ��ڽ��빦�ܺ���ǰ��������
        Enter_Function = 0;                                         //�˳����ܺ���
        Enter_counter = 0;                                          //�˳����ܺ���������Enter_counter
    }

    if(Level[sOption[EPROM.Language][SelectIndex].KeyLevel][2]>=2)                  //������ر�ѡ����Ŀ����2��
    {
        if(SelectIndex > Level[sOption[EPROM.Language][SelectIndex].KeyLevel][1]-1) //���ݷ���ѡ��ȷ����ʾ����
        {
            FirstLineDisIndex = Level[sOption[EPROM.Language][SelectIndex].KeyLevel][1]-1;
            SelectLine = 2-(Level[sOption[EPROM.Language][SelectIndex].KeyLevel][1]-SelectIndex);
        }
        else                                                        //һ����ʾ��ʽ
        {
            FirstLineDisIndex = SelectIndex;                        //��һ����ʾ����
            SelectLine = 1;                                         //ѡ���һ��
        }
    }
    else                                                            //������ر�ѡ����Ŀ����4��
    {
        FirstLineDisIndex = Level[sOption[EPROM.Language][SelectIndex].KeyLevel][0];   //��һ����ʾ����Ϊ����һ��
        SelectLine = SelectIndex - Level[sOption[EPROM.Language][SelectIndex].KeyLevel][0]+1; //ѡ���б�־Ϊ��ǰѡ��������Ӧ��
    }

    DISlm16032 ();                                                  //ˢ�²˵�
    return 0;
}

/*****************************************************************************
**�� �� ��: KeyCodeAction
**��������: �˵�������������ɢת����
**�������: uint8 KeyCode
**�������: ��
**�� �� ֵ: ��
*****************************************************************************/
void KeyCodeAction (uint8 KeyCode)
{
    switch (KeyCode)
    {
    case Up:
        MenuUpOneOption();
        break;              //��������ϰ�������˵����ϣ���������
    case Dwon:
        MenuDownOneOption();
        break;
    case Enter:
        MenuEnterOption();
        break;
    case Enter_Long:
        MenuCancelOption();
        break;
    default:
        break;
    }
}

/*****************************************************************************
**�� �� ��: Goto_mian_menu
**��������: LCD ��ʾ���س�ʼ���˵�����
**�������: void
**�������: ��
**�� �� ֵ: ��
*****************************************************************************/
void Goto_mian_menu(void)
{
    Menu_layer=0;
    Trends_display = 1;

    FirstLineDisIndex = 0;    //��Ļ��һ����ʾ��������
    SelectLine = 0;           //��ǰѡ�����
    SelectIndex = 0;          //��ǰѡ���ж�Ӧ��������
    LastIndex = 0;            //���빦�ܺ���ǰ�������ţ��жϾ��幦��ʹ��
    Enter_Function = 0;       //���빦�ܺ����ı�ʾ
    Enter_counter=0;          //��Enter�������ļ�������
    Menu_Index=0;
}

