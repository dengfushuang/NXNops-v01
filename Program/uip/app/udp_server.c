#include "main.h"
#include "udp_server.h"
#include "httpd.h"
#include "uip.h"
#include "uart0.h"
#include "uip.h"
#include <string.h>
#include "uip_arp.h"


extern  uip_udp_listen( u16_t lport);    //UDP��������

extern struct uip_eth_addr uip_ethaddr;   //UIP��MAC��ַ

const char CMD[7]= {0XA1,0X15,0X02,0,0,0XFF};
const char data1[4] = {0x00,0x02,0x00,0x02};
const char data2[7] = {0xA1,0XFD,0X00,0X09,0X23,0X01,0X08};
const char data3[4] = {0X0B,0XBB,0X00,0XFF};

/***********************************************************************************************
* ������  :udp_server_init()
* ����    :void
* ����ֵ  :void
* ��������:UDP��ʼ��������������������
************************************************************************************************/
void udp_server_init(void)
{
    uip_udp_listen(8800);
    uip_udp_listen(8600);
}
/***********************************************************************************************
* ������  :UIP_UDP_APPCALL()
* ����    :void
* ����ֵ  :void
* ��������:UDP��Ӧ���������ڻ�Ӧ��̫��ת���ڵ�����������PC��������
************************************************************************************************/
void UIP_UDP_APPCALL(void)
{
    uint16 sprintf_len;
    
    if(uip_udp_conn->lport== HTONS(8800) )
    {
        if(uip_newdata())   //|| uip_rexmit()
        {
            //�����ϵ�ZNetCom ��̫������ת���豸���ù����������������:  A1 15 02 00 00 FF
            //ģ���յ����Ӧ: 00 02 00 02  00 14 97 06 F1 D7   A1 FD 00 09 23 01 08 C0 A8 00 B2 0B BB 00 FF
            //                             |-------MAC-----|                        |---ԭIP--|

            if( uip_appdata[0]==0XA1 && uip_appdata[1]==0X15 && uip_appdata[2]==0X02 && uip_appdata[3]==0 && uip_appdata[4]==0 && uip_appdata[5]==0Xff )
            {
                memcpy(uip_appdata,data1,4);
                memcpy(&uip_appdata[4], uip_ethaddr.addr, 6);   //��ӦMAC��ַ����λ��
                memcpy(&uip_appdata[10],data2,7);
                memcpy(&uip_appdata[17], uip_hostaddr,4);       //��ӦIP��ַ����λ��
                memcpy(&uip_appdata[21],data3,4);
                uip_send(uip_appdata, 25);
            }
        }
    }
    else if( uip_udp_conn->lport == HTONS(8600) )
    {
        Send_LOG_conn = uip_udp_conn;
        if( uip_poll() )
        {
//            Print_LOG(uip_appdata, uint16 num);
            uip_send((char *)uip_appdata, sprintf_len);             
        }
        else if( uip_newdata() )                             //�յ��µ����� 
        {
//             if( ( sprintf_len = Cmd_process( (char*)&uip_appdata[0]) ) > 0 )
//                 uip_send( uip_appdata, sprintf_len );
                
            //if( LOG_PRINT_FLAG )    UDP_CMD = 1; 
        }
    }
}

