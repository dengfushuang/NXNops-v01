/**********************************************************************************************************
*文件名：Cmd_process.c
*创建时间: 2012-10-18
*功能描述: 处理串口和TCP的设置和查询指令
*作   者 ：
*公   司 ：
**********************************************************************************************************/
#include "cmd_process.h"
/********************************************************************************************************
** 函数名称: cmd_check
** 功能描述: 校验字符数组是否正确
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
			checknum = sprintf_buf[i-1]&0xff;  //取得校验码
			cnt = i-1;                    //指令字节数
            break;			
		}
	}
	if(cnt)
	{
		for(i = 0 ;i < cnt ;i++)
		{
		   sum+=sprintf_buf[i];       //字节求和
		}
		if(checknum==(sum&0xff))    //校验码比较
		{
			cnt+=3;                //校验正确
		}else
		{
			cnt = 0;                //校验失败
		}
	}
	return cnt;
}
/********************************************************************************************************
** 函数名称: creat_ch
** 功能描述: 把字符串转化为带校验码的字符串数据 数据+校验码（1字节）+0x0D+0x0A
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
** 函数名称: cmd_process
** 功能描述: 指令解析
********************************************************************************************************/
int cmd_process( char* sprintf_buf ,int *str_len)
{
	
	int send_To_uart = 0;      //选择UART输出，-2===指令校验错误，-1=====没有该指令；0===串口0输出，1x===串口1输出第x路；
	CMD_ID cmdid=ERR;         //指令枚举
	uint16 len;               //字符串长度
	
	uint8 i=0;
	uint8 num=0;              //输入通道号，存储字符串转化的整数
	uint8 channel1=0,channel2=0;  //存储计算结果
	char string_Cache[12]={'\0'}; //字符串缓存
	char string_Buf[UART_RCV_BUF_LEN]={0}; //存储未加校验码的字符串
	len = cmd_check(sprintf_buf);   //指令校验
	if(!len)
	{
		//*str_len = creat_ch(sprintf_buf,"CHECK_ERROR");
		err(0);
		return -1;                           //校验失败返回0；
	}
	if(strstr(&sprintf_buf[0], "GETVERSION") != NULL)
	{
		cmdid = GETVERSION;      //获取版本命令
	}
	else if(strstr(&sprintf_buf[0], "OK") != NULL)
	{
		if(sprintf_buf[2]=='C')
		{
			cmdid = OKCH;       //业务盘返回单通道设置成功
		}
		else if(sprintf_buf[2]=='S')
		{
			cmdid = OKSETSW;     //业务盘返回多通道设置成功
		}
		else if(sprintf_buf[2]=='R')
		{
			cmdid = OKRST;
		}
	}
	else if(strstr(&sprintf_buf[0], "SETSW") != NULL)
	{
		cmdid = SETSW;           //多路切换光开关
	}
	else if(strstr(&sprintf_buf[0], "CH") != NULL)
	{
		cmdid = CH;              //单路切换光开关
	}
	else if(strstr(&sprintf_buf[0], "GETSW") != NULL)
	{
		if(sprintf_buf[5]=='A')   //查询所有光路
		{
			cmdid = GETSWA;
		}else
		{
			cmdid = GETSW;        //查询对应光路
		}
	}
	else if(strstr(&sprintf_buf[0], "RST") != NULL)
	{
		cmdid = RST;              //光路复位
	}
#ifdef MASTER_CPU
	
    switch(cmdid)
	{
		case ERR:{
			       err(1);
		           send_To_uart = -2;                           //从串口0发送 
		         }
			break;
	    /****返回版本号****/
		// GETVERSION
		case GETVERSION:{
			            //向串口0返回版本号；
		                *str_len = creat_ch(sprintf_buf,(char *)SVersion); 
			            send_To_uart = 0;
			            //从串口0发送
		                }
		    break;
		/****设置光通道转发到业务盘****/
		// SETSW xx~xx,xx~xx
		case SETSW:{
			         *str_len =len;
		             send_To_uart = 0;
		           }
		    break;
		/****设置单路光通道转发到业务盘****/
	    //CHxx~CHyy
		case CH:{
			       *str_len =len;
		           send_To_uart = 0;
		        }
		    break;
		/****查询光通道*****/
		//GETSWxx---查询单路，GETSWA-----查询所有通道
		case GETSW:
		case GETSWA:{
			     
			         if(cmdid==GETSW)        //单路查询指令
					 {   
						 num = (uint8)(sprintf_buf[5]-'0')*10;
			             num += (uint8)(sprintf_buf[6]-'0');
						 if(num == 0)
						 {
							 ;
						 }
						 else
					     {
							 if(EPROM.Channel[num-1] != 0)  //num为输入通道，EPROM.Channel[num-1]为输出通道 
							 {
								//转化为字符串，格式：CHxx~CHyy;
								sprintf(string_Buf,"%s%02d%s%s%02d","CH",(int )num,"~","CH",(int)EPROM.Channel[num-1]);
							 }else
							 {
								sprintf(string_Buf,"%s%02d%s%s%02d","CH",(int )num,"~","CH",num);
							 } 
						 }
					 }
                     else if(cmdid==GETSWA)  //所有通道查询指令
					 {					 
						 for(i =0;i< MAX_CHANNEL;i++) //MAX_CHANNEL最大输入通道数。
						 {   	
							 if(EPROM.Channel[i]!=0)  //EPROM.Channel[i]==0表示通道i连接断开
							 {
								sprintf(string_Cache,"%s%02d%s%s%02d","CH",(int)(i+1),"~","CH",(int)EPROM.Channel[i]);
							 }else
                             {
								sprintf(string_Cache,"%s%02d%s%s%02d","CH",(int)(i+1),"~","CH",0);
								
							 }
                             strcat(string_Buf,string_Cache);							 
						 }
					 }
					 
					/****在字符串中添加校验码和0D 0A****/
					*str_len =(int)creat_ch(sprintf_buf,string_Buf);
                    send_To_uart = 0;					 
		           }
		    break;
	    /****复位指令****/
	    case RST:{
			         reset();   //从串口1发送到业务控制CPU
				 }
		    break;
		/****该指令为业务盘返回，一旦操作成功，返回该指令，格式如下：****/
		/**** OKCHxx~CHyy+校验码+0x0D+0x0A ****/
		case OKCH:{       //单通道设置成功
			        channel1 = ((uint8)(sprintf_buf[4]-'0'))*10+((uint8)(sprintf_buf[5]-'0'));   
			        channel2 = ((uint8)(sprintf_buf[9]-'0'))*10+((uint8)(sprintf_buf[10]-'0'));
			        EPROM.Channel[channel1-1]=channel2; 
			        Save_To_EPROM((uint8 *)&EPROM.Channel[channel1-1],1);//存储通道连接状态，即channel通道连接到channel2通道
			        *str_len = len;
			        send_To_uart = 0;
		        }
		    break;
		/**** 多通道设置****/
		/**** OKSETSW xx~yy,nn~mm,ii~jj+校验码+0x0D+0x0A ****/
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



