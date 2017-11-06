/**********************************************************************************************************
*�ļ�����Cmd_process.c
*����ʱ��: 2012-10-18
*��������: �����ں�TCP�����úͲ�ѯָ��
*��   �� ��
*��   ˾ ��
**********************************************************************************************************/
#include "cmd_process.h"
/********************************************************************************************************
** ��������: cmd_check
** ��������: У���ַ������Ƿ���ȷ
********************************************************************************************************/
static uint16 cmd_check(const char* sprintf_buf)
{
    uint16 i;
	uint16 cnt=0;
	uint16 sum=0;
	unsigned char checknum;
	for(i = 2 ;i < UART_RCV_BUF_LEN;i++)
	{
		if((sprintf_buf[i] == 0x0D)&&(sprintf_buf[i+1] == 0x0a))
		{
			checknum = sprintf_buf[i-1]&0xff;  //ȡ��У����
			cnt = i-1;                    //ָ���ֽ���
            break;			
		}
	}
	if(cnt)
	{
		for(i = 0 ;i < cnt ;i++)
		{
		   sum+=sprintf_buf[i];       //�ֽ����
		}
		if(checknum==(sum&0xff))    //У����Ƚ�
		{
			cnt+=3;                //У����ȷ
		}else
		{
			cnt = 0;                //У��ʧ��
		}
	}
	return cnt;
}
/********************************************************************************************************
** ��������: creat_ch
** ��������: ���ַ���ת��Ϊ��У������ַ������� ����+У���루1�ֽڣ�+0x0D+0x0A
********************************************************************************************************/
uint16 creat_ch(char *dest,char *src)
{
	char *cp;
	uint16 cnt = 0;
	uint16 sum = 0;
	cp = src;
	while(*cp != '\0')
	{
		sum+=*cp;
		cnt++;
		cp++;
	}
	
	memcpy(dest,src,cnt);
	dest[cnt] &= 0x00;
	dest[cnt] |= (sum&0xff);
	dest[cnt+1] = 0x0D;
	dest[cnt+2] = 0x0A;
	cnt+=3;
	return cnt;
}
/********************************************************************************************************
** ��������: cmd_process
** ��������: ָ�����
********************************************************************************************************/
int cmd_process( char* sprintf_buf ,int *str_len)
{
	
	int send_To_uart = 0;      //ѡ��UART�����-2===ָ��У�����-1=====û�и�ָ�0===����0�����1x===����1�����x·��
	CMD_ID cmdid=ERR;         //ָ��ö��
	uint16 len;               //�ַ�������
	
	uint8 i=0;
	uint8 num=0;              //����ͨ���ţ��洢�ַ���ת��������
	uint8 channel1=0,channel2=0;  //�洢������
	char string_Cache[12]={'\0'}; //�ַ�������
	char string_Buf[UART_RCV_BUF_LEN]={0}; //�洢δ��У������ַ���
	len = cmd_check(sprintf_buf);   //ָ��У��
	if(!len)
	{
		//*str_len = creat_ch(sprintf_buf,"CHECK_ERROR");
		err(0);
		return -1;                           //У��ʧ�ܷ���0��
	}
	if(strstr(&sprintf_buf[0], "GETVERSION") != NULL)
	{
		cmdid = GETVERSION;      //��ȡ�汾����
	}
	else if(strstr(&sprintf_buf[0], "OK") != NULL)
	{
		if(sprintf_buf[2]=='C')
		{
			cmdid = OKCH;       //ҵ���̷��ص�ͨ�����óɹ�
		}
		else if(sprintf_buf[2]=='S')
		{
			cmdid = OKSETSW;     //ҵ���̷��ض�ͨ�����óɹ�
		}
		else if(sprintf_buf[2]=='R')
		{
			cmdid = OKRST;
		}
	}
	else if(strstr(&sprintf_buf[0], "SETSW") != NULL)
	{
		cmdid = SETSW;           //��·�л��⿪��
	}
	else if(strstr(&sprintf_buf[0], "CH") != NULL)
	{
		cmdid = CH;              //��·�л��⿪��
	}
	else if(strstr(&sprintf_buf[0], "GETSW") != NULL)
	{
		if(sprintf_buf[5]=='A')   //��ѯ���й�·
		{
			cmdid = GETSWA;
		}else
		{
			cmdid = GETSW;        //��ѯ��Ӧ��·
		}
	}
	else if(strstr(&sprintf_buf[0], "RST") != NULL)
	{
		cmdid = RST;              //��·��λ
	}
#ifdef MASTER_CPU
	
    switch(cmdid)
	{
		case ERR:{
			       err(1);
		           send_To_uart = -2;                           //�Ӵ���0���� 
		         }
			break;
	    /****���ذ汾��****/
		// GETVERSION
		case GETVERSION:{
			            //�򴮿�0���ذ汾�ţ�
		                *str_len = creat_ch(sprintf_buf,(char *)SVersion); 
			            send_To_uart = 0;
			            //�Ӵ���0����
		                }
		    break;
		/****���ù�ͨ��ת����ҵ����****/
		// SETSW xx~xx,xx~xx
		case SETSW:{
			         *str_len =len;
		             send_To_uart = 0;
		           }
		    break;
		/****���õ�·��ͨ��ת����ҵ����****/
	    //CHxx~CHyy
		case CH:{
			       *str_len =len;
		           send_To_uart = 0;
		        }
		    break;
		/****��ѯ��ͨ��*****/
		//GETSWxx---��ѯ��·��GETSWA-----��ѯ����ͨ��
		case GETSW:
		case GETSWA:{
			     
			         if(cmdid==GETSW)        //��·��ѯָ��
					 {   
						 num = (uint8)(sprintf_buf[5]-'0')*10;
			             num += (uint8)(sprintf_buf[6]-'0');
						 if(num == 0)
						 {
							 ;
						 }
						 else
					     {
							 if(EPROM.Channel[num-1] != 0)  //numΪ����ͨ����EPROM.Channel[num-1]Ϊ���ͨ�� 
							 {
								//ת��Ϊ�ַ�������ʽ��CHxx~CHyy;
								sprintf(string_Buf,"%s%02d%s%s%02d","CH",(int )num,"~","CH",(int)EPROM.Channel[num-1]);
							 }else
							 {
								sprintf(string_Buf,"%s%02d%s%s%02d","CH",(int )num,"~","CH",num);
							 } 
						 }
					 }
                     else if(cmdid==GETSWA)  //����ͨ����ѯָ��
					 {					 
						 for(i =0;i< MAX_CHANNEL;i++) //MAX_CHANNEL�������ͨ������
						 {   	
							 if(EPROM.Channel[i]!=0)  //EPROM.Channel[i]==0��ʾͨ��i���ӶϿ�
							 {
								sprintf(string_Cache,"%s%02d%s%s%02d","CH",(int)(i+1),"~","CH",(int)EPROM.Channel[i]);
							 }else
                             {
								sprintf(string_Cache,"%s%02d%s%s%02d","CH",(int)(i+1),"~","CH",0);
								
							 }
                             strcat(string_Buf,string_Cache);							 
						 }
					 }
					 
					/****���ַ��������У�����0D 0A****/
					*str_len =(int)creat_ch(sprintf_buf,string_Buf);
                    send_To_uart = 0;					 
		           }
		    break;
	    /****��λָ��****/
	    case RST:{
			         reset();   //�Ӵ���1���͵�ҵ�����CPU
				 }
		    break;
		/****��ָ��Ϊҵ���̷��أ�һ�������ɹ������ظ�ָ���ʽ���£�****/
		/**** OKCHxx~CHyy+У����+0x0D+0x0A ****/
		case OKCH:{       //��ͨ�����óɹ�
			        channel1 = ((uint8)(sprintf_buf[4]-'0'))*10+((uint8)(sprintf_buf[5]-'0'));   
			        channel2 = ((uint8)(sprintf_buf[9]-'0'))*10+((uint8)(sprintf_buf[10]-'0'));
			        EPROM.Channel[channel1-1]=channel2; 
			        Save_To_EPROM((uint8 *)&EPROM.Channel[channel1-1],1);//�洢ͨ������״̬����channelͨ�����ӵ�channel2ͨ��
			        *str_len = len;
			        send_To_uart = 0;
		        }
		    break;
		/**** ��ͨ������****/
		/**** OKSETSW xx~yy,nn~mm,ii~jj+У����+0x0D+0x0A ****/
		case OKSETSW:{
			      for(i=8;i<len-6;i+=6)
			      {
					 channel1 = ((uint8)(sprintf_buf[i]-'0'))*10+((uint8)(sprintf_buf[i+1]-'0'));   
			         channel2 = ((uint8)(sprintf_buf[i+3]-'0'))*10+((uint8)(sprintf_buf[i+4]-'0'));
			         EPROM.Channel[channel1-1]=channel2;
			         Save_To_EPROM((uint8 *)&EPROM.Channel[channel1-1],1);  
				  }
				  
				  *str_len = len;
			      //*str_len = creat_ch(sprintf_buf,"OK"); 
			      send_To_uart = 0;
		        }
		    break;
		case OKRST:{
			    
		      }
		    break;
		default:break;
	}
#endif
    return send_To_uart;
}



