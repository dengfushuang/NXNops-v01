/**
 * \addtogroup exampleapps
 * @{
 */

/**
 * \defgroup httpd Web server
 * @{
 *
 * The uIP web server is a very simplistic implementation of an HTTP
 * server. It can serve web pages and files from a read-only ROM
 * filesystem, and provides a very small scripting language.
 *
 * The script language is very simple and works as follows. Each
 * script line starts with a command character, either "i", "t", "c",
 * "#" or ".".  The "i" command tells the script interpreter to
 * "include" a file from the virtual file system and output it to the
 * web browser. The "t" command should be followed by a line of text
 * that is to be output to the browser. The "c" command is used to
 * call one of the C functions from the httpd-cgi.c file. A line that
 * starts with a "#" is ignored (i.e., the "#" denotes a comment), and
 * the "." denotes the last script line.
 *
 * The script that produces the file statistics page looks somewhat
 * like this:
 *
 \code
i /header.html
t <h1>File statistics</h1><br><table width="100%">
t <tr><td><a href="/index.html">/index.html</a></td><td>
c a /index.html
t </td></tr> <tr><td><a href="/cgi/files">/cgi/files</a></td><td>
c a /cgi/files
t </td></tr> <tr><td><a href="/cgi/tcp">/cgi/tcp</a></td><td>
c a /cgi/tcp
t </td></tr> <tr><td><a href="/404.html">/404.html</a></td><td>
c a /404.html
t </td></tr></table>
i /footer.plain
.
 \endcode
 *
 */


/**
 * \file
 * HTTP server.
 * \author Adam Dunkels <adam@dunkels.com>
 */

/*
 * Copyright (c) 2001, Adam Dunkels.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote
 *    products derived from this software without specific prior
 *    written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * This file is part of the uIP TCP/IP stack.
 *
 * $Id: httpd.c,v 1.28.2.6 2003/10/07 13:22:27 adam Exp $
 *
 */

#include <stdio.h>
#include "main.h"
#include <math.h>
#include <string.h>
#include "uip.h"
#include "httpd.h"
#include "fs.h"
#include "fsdata.h"
#include "cgi.h"

//#include "..\APP\config.h"

#include "uart0.h"
#include "eeprom.h"
#include "uip_arp.h"
//#define NULL (void *)0

/* The HTTP server states: */
#define HTTP_NOGET        0
#define HTTP_FILE         1
#define HTTP_TEXT         2
#define HTTP_FUNC         3
#define HTTP_END          4

#define   CAT1025		0xA0		// CAT1025的I2C地址

#ifdef DEBUG
#include <stdio.h>
#define PRINT(x) printf("%s", x)
#define PRINTLN(x) printf("%s\n", x)
#else /* DEBUG */
#define PRINT(x)
#define PRINTLN(x)
#endif /* DEBUG */

struct httpd_state *hs;

u8_t EPROM_temp[18];
extern const struct fsdata_file file_index_html;
extern const struct fsdata_file file_login_html;
extern const struct fsdata_file file_404_html;


static void next_scriptline(void);
static void next_scriptstate(void);

#define ISO_G        0x47
#define ISO_E        0x45
#define ISO_T        0x54
#define ISO_slash    0x2f   // "/"
#define ISO_c        0x63
#define ISO_g        0x67
#define ISO_i        0x69
#define ISO_space    0x20
#define ISO_nl       0x0a
#define ISO_cr       0x0d
#define ISO_a        0x61
#define ISO_t        0x74
#define ISO_hash     0x23  // "#"
#define ISO_period   0x2e  // "."
#define ISO_p        0x70
#define ISO_s        0x73
#define ISO_e        0x65
#define ISO_P        0X50
#define ISO_O        0X4F
#define ISO_S        0X53

//网页显示 "Set Success! Restart effect!" 界面
const  char shows_set_ok[]=
{
    "HTTP/1.0 200 OK\r\n"
    "Content-type: text/html\r\n\r\n"
    "<html>\r\n"
    "<h1 align=\"center\"><font color=\"#FF0000\">Set Success! Restart effect!</h1>\r\n"  //设置成功，重启生效!
    //"<p align=\"center\"><input type=\"button\" onclick=\"history.go(-1)\"value=\"back\"></P>"
    "</html>\r\n"
};
//网页显示 "Password err!Please Enter again!" 界面
const  char shows_password_err[]=
{
    "HTTP/1.0 200 OK\r\n"
    "Content-type: text/html\r\n\r\n"
    "<html>"
    "<h1 align=\"center\"><font color=\"#FF0000\"\">Password err!Please Enter again!</h1>"  //输入密码有误!重新输入!
    "<p align=\"center\"><input type=\"button\" onclick=\"history.go(-1)\"value=\"back\"></P>"   //返回
    "</html>"

};
//网页显示 "Password changing success!" 界面
const  char shows_password_ok[]=
{
    "HTTP/1.0 200 OK\r\n"
    "Content-type: text/html\r\n\r\n"
    "<html>"
    "<h1 align=\"center\"><font color=\"#FF0000\">Password changing success!</h1>"
    //"<p align=\"center\"><input type=\"button\" onclick=\"history.go(-1)\"value=\"back\"></P>"  //返回
    "</html>"

};
//模块复位完成，网页显示 "Please access 192.168.0.178" 界面
const  char shows_reset[]=
{
    "<html><style type=\"text/css\">.ws20"
    "{font-size: 27px;}.wpmd {font-size: 13px;font-family: 'Arial';font-style: normal;font-weight: normal;text-decoration: none;}"
    "</style><body><div  align=center><br><br><br><br><br><br><br><br><br><div class=\"wpmd\"><font class=\"ws20\" color=\"#FFCC00\">"
    "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; </font>"
    "<font class=\"ws20\" color=\"#3366FF\">Please access</font><font class=\"ws20\" color=\"#FFCC00\"></font>"
    "<font class=\"ws20\"><a href=\"http://%u.%u.%u.%u\" target=\"_parent\">%u.%u.%u.%u</a>"
    "</font><font class=\"ws20\" color=\"#3366FF\">!</font></div></div></body></html>"
};

/*-----------------------------------------------------------------------------------*/
/**
 * Initialize the web server.
 *
 * Starts to listen for incoming connection requests on TCP port 80.
 */
/*-----------------------------------------------------------------------------------*/
void httpd_init(void)
{
    fs_init();

    /* Listen to port 80. */
    uip_listen(HTONS(80));
}

//********************************************************************************************
//函数功能: 从CGI数据中读取有效的数据,并把CGI数据的ascii值转换十进制值
//输入参数: str1 为要查找的字符串的地址
//          str2 要查找的字符串
//          strlen 查找的字符串的长度
//返回值:   可以返回8bit或16bit的数据值
//********************************************************************************************
u16_t find_data(char* str1, char* str2,char strlen)
{
    char* s;
    u8_t  i;
    u16_t temp;

    s = strstr(str1,str2);
    if(s!=(void*)0)   //成功找到字符串，返回非空指针
    {
        s += strlen;
        for(i=0; s[i]!='&'; ++i);   //有"&"符号表示结束
        switch(i)
        {
        case 1:
            temp=( s[0]-'0' );
            break;
        case 2:
            temp=( (s[0]-'0')*10  + (s[1]-'0'));
            break;
        case 3:
            temp=( (s[0]-'0')*100 + (s[1]-'0')*10 + (s[2]-'0') );
            break;
        case 4:
            temp=( (s[0]-'0')*1000 +(s[1]-'0')*100 + (s[2]-'0')*10 + (s[3]-'0') );
            break;
        case 5:
            temp=( (s[0]-'0')*10000 +(s[1]-'0')*1000 + (s[2]-'0')*100 + (s[3]-'0')*10+(s[4]-'0') );
            break;
        }
        if(temp>65536) temp=0;
        return  temp;
    }
    else   return 0;
}

void get_IP_GW_SM(void)
{
    u16_t temp;

    EPROM_temp[0]=find_data((char*)&uip_appdata[10], "ftip1=", 6);  //取服务器IP
    EPROM_temp[1]=find_data((char*)&uip_appdata[16], "ftip2=", 6);
    EPROM_temp[2]=find_data((char*)&uip_appdata[22], "ftip3=", 6);
    EPROM_temp[3]=find_data((char*)&uip_appdata[28], "ftip4=", 6);

    EPROM_temp[4]=find_data((char*)&uip_appdata[34], "ftgw1=", 6);  //取客户机IP
    EPROM_temp[5]=find_data((char*)&uip_appdata[40], "ftgw2=", 6);
    EPROM_temp[6]=find_data((char*)&uip_appdata[46], "ftgw3=", 6);
    EPROM_temp[7]=find_data((char*)&uip_appdata[52], "ftgw4=", 6);

    EPROM_temp[8]=find_data((char*)&uip_appdata[58],  "ftsm1=", 6);  //取子网掩码
    EPROM_temp[9]=find_data((char*)&uip_appdata[64],  "ftsm2=", 6);
    EPROM_temp[10]=find_data((char*)&uip_appdata[70], "ftsm3=", 6);
    EPROM_temp[11]=find_data((char*)&uip_appdata[76], "ftsm4=", 6);
    //连接超时时间变量为16bit的变量
    temp=find_data((char*)&uip_appdata[82], "interval=", 9);     //取TCP连接超时时间
    EPROM_temp[16]=(u8_t)temp;
    EPROM_temp[17]=temp>>8;
}

void get_uart(void)
{
    u16_t temp;
    EPROM_temp[12]=find_data((char*)&uip_appdata[10], "bps=", 4);           //取波特率

    EPROM_temp[13] =(char) find_data((char*)&uip_appdata[15], "dabit=", 6); //取数据位
    EPROM_temp[13]+=(char) find_data((char*)&uip_appdata[21], "stbit=", 6); //取停止位
    EPROM_temp[13]+=(char) find_data((char*)&uip_appdata[27], "pabit=", 6); //取校验位
    //TCP端口号为16bit的变量
    temp=find_data((char*)&uip_appdata[33], "port=", 5);                    //取TCP端口号
    EPROM_temp[14]=(u8_t)temp;
    EPROM_temp[15]=temp>>8;
}

/*-----------------------------------------------------------------------------------*/
unsigned char password_temp[17];
void httpd_appcall(void)
{
    u8_t   EPROM_config[4];//EPROM中读取参数的缓存

    struct fs_file fsfile;
    u16_t i;
    u8_t  n1,n2;
    char* s;
    char* s1;
    char* s2;

    switch(uip_conn->lport)
    {
        /* This is the web server: */
    case HTONS(80):
        /* Pick out the application state from the uip_conn structure. */
        hs = (struct httpd_state *)(&(uip_conn->appstate));

        /* We use the uip_ test functions to deduce why we were
        called. If uip_connected() is non-zero, we were called
        because a remote host has connected to us. If
        uip_newdata() is non-zero, we were called because the
        remote host has sent us new data, and if uip_acked() is
        non-zero, the remote host has acknowledged the data we
        previously sent to it. */
        if(uip_connected())
        {
            /* Since we have just been connected with the remote host, we
             reset the state for this connection. The ->count variable
             contains the amount of data that is yet to be sent to the
             remote host, and the ->state is set to HTTP_NOGET to signal
             that we haven't received any HTTP GET request for this
             connection yet. */
            hs->state = HTTP_NOGET;
            hs->count = 0;
            return;

        }
        else if(uip_poll())
        {
            /* If we are polled ten times, we abort the connection. This is
             because we don't want connections lingering indefinately in
             the system. */
            if(hs->count++ >= 10)
            {
                uip_abort();
            }

            return;
        }
        else if(uip_newdata() && hs->state == HTTP_NOGET)
        {
            //********************************************************************************************
            //--------------------------接收网页浏览器以POST方式发出的信息-----------------------------
            //********************************************************************************************
            if(uip_appdata[0] == ISO_P ||
                    uip_appdata[1] == ISO_O ||
                    uip_appdata[2] == ISO_S ||
                    uip_appdata[3] == ISO_T)
            {
                // SendString2(uip_appdata, 1000);
                //-------------------------密码登录框------------------------
                if( (s = strstr((char*)uip_appdata,"DEVPASS="))!=NULL )
                {
                    s += 8;
                    for(i=0; s[i]!='&'; ++i)
                        password_temp[i] = s[i];
                    password_temp[i] = '\0';        //加结束符，在下面的strcmp函数用于判断结束
                    if( strcmp((char*)&password_temp,(char*)&EPROM.password)==0 )  //对比密码
                    {
                        fs_open(file_index_html.name, &fsfile);
                        goto  sent;                //比较密码正确，发出网页索引
                    }
                    else
                    {
                        //发送显示提示密码输入错误网页
                        uip_send(uip_appdata, sprintf((char *)uip_appdata,shows_password_err));
                        break;
                    }
                }
                //-------------------------密码修改框------------------------
                else if( (s = strstr((char*)uip_appdata,"ftpswd="))!=NULL )
                {
                    s += 7;
                    for(i=0; s[i]!='&'; ++i)
                        password_temp[i] = s[i];
                    password_temp[i] = '\0';                        //加结束符，在下面的strcmp函数用于判断结束

                    if( strcmp((char*)&password_temp,(char*)&EPROM.password)==0 )  //对输入的旧密码与原密码进行比较
                    {

                        if( (s1 = strstr((char*)uip_appdata,"ftnpswd="))!=NULL   \
                                &&(s2 = strstr((char*)uip_appdata,"ftrnpswd="))!=NULL )
                        {
                            s=s1+8;
                            for( n1=0; *s++ !='&'; n1++);           //计算第一次输入的密码的位数
                            *(&s[0]-1)='\0'; 						//加结束符，用于判断结束

                            s=s2+9;
                            for( n2=0; *s++ !='&'; n2++);           //计算第一次输入的密码的位数
                            *(&s[0]-1)='\0';						//加结束符用于判断结束

                            if(n1 != n2)   goto  show_eer;          //先比较两次输入的密码的长度是否相同

                            //再比较两次出入的密码的内容是否相同
                            if( strcmp((char*)(&s1[0]+8),(char*)(&s2[0]+9))==0 )
                            {
                                password_save((u8_t*)(&s2[0]+9), n2+1);//(n2+1)为多加了结束符，把密码存入EPROM
                                //发送显示提示密码输入成功网页
                                uip_send(uip_appdata, sprintf((char *)uip_appdata,shows_password_ok));
                                break;
                            }
                            else
                            {
show_eer:
                                uip_send(uip_appdata, sprintf((char *)uip_appdata,shows_password_err));
                                return;     //发送显示提示密码输入错误网页后直接退出
                            }
                        }
                    }
                    else    goto  show_eer;  //掉转到发送显示提示密码输入错误网页
                }
                return;
            }

            //********************************************************************************************
            //-------------------------------以下接收网页浏览器以GET方式发出的信息-----------------------
            //********************************************************************************************
            if(uip_appdata[0] != ISO_G ||
                    uip_appdata[1] != ISO_E ||
                    uip_appdata[2] != ISO_T ||
                    uip_appdata[3] != ISO_space)
            {
                /* If it isn't a GET, we abort the connection. */
                uip_abort();
                return;
            }

            //SendString("appdata[get]ok\n");  //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
            /* Find the file we are looking for. */
            for(i = 4; i < 40; ++i)
            {
                if(uip_appdata[i] == ISO_space ||
                        uip_appdata[i] == ISO_cr ||
                        uip_appdata[i] == ISO_nl)
                {
                    uip_appdata[i] = 0;
                    // SendString("crnl break\n");  //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
                    break;
                }
            }

            // Check for a request for "/".  查找请求符号"/"
            if(uip_appdata[4] == ISO_slash && uip_appdata[5] == 0)
            {
                //SendString("open file_index_html\n");  //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
                fs_open( file_login_html.name, &fsfile);
            }
            //---------------浏览器发出IP设置请求---------------------
            else if(uip_appdata[4] == ISO_slash &&
                    uip_appdata[5] == ISO_i &&
                    uip_appdata[6] == ISO_p &&
                    uip_appdata[7] == ISO_s &&
                    uip_appdata[8] == ISO_e &&
                    uip_appdata[9] == ISO_t &&
                    uip_appdata[10] == ISO_slash)
            {
                get_IP_GW_SM();
                IP_GW_SM_save((uint8 *)&EPROM_temp ,12);

                //保存TCP超时连接时间参数到EPROM中
                //ISendStr(CAT1025,(uint8 *)&EPROM.max_abort_timer - (uint8 *)&EPROM.MAC_addr, (uint8 *)&EPROM_temp[16], 2);
                OSTimeDly(2);

                uip_send(uip_appdata, sprintf((char *)uip_appdata,shows_set_ok));
                break;
            }
            //---------------浏览器发出串口设置请求---------------------
            else if(uip_appdata[4] == ISO_slash &&
                    uip_appdata[5] == 'u'  &&
                    uip_appdata[6] == 'a' &&
                    uip_appdata[7] == 'r' &&
                    uip_appdata[8] == 't' &&
                    uip_appdata[9] == 's' &&
                    uip_appdata[10] == 'e' &&
                    uip_appdata[11] == 't' &&
                    uip_appdata[12] == '/')
            {
                // SendString2(uip_appdata, 250);   //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
                get_uart();                      //从网页中读取串口设置数据

                /*	ISendStr(CAT1025,(uint8 *)&EPROM.uart_bps - (uint8 *)&EPROM.password, (uint8 *)&EPROM_temp[12], 2);
                	OSTimeDly(2);
                	ISendStr(CAT1025,(uint8 *)&EPROM.TCP_lport - (uint8 *)&EPROM.password, (uint8 *)&EPROM_temp[14], 2);
                	OSTimeDly(2); */

                UART_TCP_lport_bs_flag_save(&EPROM_temp[12] ,4);
                uip_send(uip_appdata, sprintf((char *)uip_appdata,shows_set_ok));
                break;
            }
            //---------------浏览器发出复位模块请求---------------------
            else if(uip_appdata[4] == ISO_slash &&
                    uip_appdata[5] == 'r'  &&
                    uip_appdata[6] == 'e' &&
                    uip_appdata[7] == 's' &&
                    uip_appdata[8] == 'e' &&
                    uip_appdata[9] == 't' )
            {
reset:          //复位之前先更新显示连接IP
                EEPROM_Read_Str((uint8 *)&EPROM.hostaddr - (uint8 *)&EPROM.MAC_addr, EPROM_config, 4);
                OSTimeDly (2);

                uip_send(uip_appdata, sprintf((char *)uip_appdata,shows_reset,
                                              EPROM_config[0],
                                              EPROM_config[1],
                                              EPROM_config[2],
                                              EPROM_config[3],

                                              EPROM_config[0],
                                              EPROM_config[1],
                                              EPROM_config[2],
                                              EPROM_config[3] ));

                Reset_flag=1;  //软件复位标示置1
                break;
            }
            //---------------浏览器发出恢复出厂设置请求---------------------
            else if( uip_appdata[4] == ISO_slash &&
                     uip_appdata[5] ==  'f'  &&
                     uip_appdata[6] ==  'a'  &&
                     uip_appdata[7] ==  'c'  &&
                     uip_appdata[8] ==  't'  &&
                     uip_appdata[9] ==  'o'  &&
                     uip_appdata[10] == 'r'  &&
                     uip_appdata[11] == 'y'  &&
                     uip_appdata[12] == '_'  &&
                     uip_appdata[13] == 's'  &&
                     uip_appdata[14] == 'e'  &&
                     uip_appdata[15] == 't'
                   )
            {
                restore_set();
                goto  reset;   //掉转到复位处
            }
            else
            {
                //按index.html 框架中的顺序打开相应的 html文件
                if(!fs_open((const char *)&uip_appdata[4], &fsfile))
                {
                    //SendString("open file_404_html\n");  //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
                    fs_open(file_404_html.name, &fsfile);
                }
            }
            
            //********************************************************************************************
            //--------------------------接收网页浏览器以CGI方式发出的信息-----------------------------
            //********************************************************************************************
            if(uip_appdata[4] == ISO_slash &&
                    uip_appdata[5] == ISO_c &&
                    uip_appdata[6] == ISO_g &&
                    uip_appdata[7] == ISO_i &&
                    uip_appdata[8] == ISO_slash)
            {
                /* If the request is for a file that starts with "/cgi/", we
                prepare for invoking a script. */
                hs->script = fsfile.dat;
                // SendString("is cgi\n");  //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
                next_scriptstate();
            }
            else
            {
sent:
                hs->script = NULL;
                /* The web server is now no longer in the HTTP_NOGET state, but
                in the HTTP_FILE state since is has now got the GET from
                the client and will start transmitting the file. */
                hs->state = HTTP_FILE;

                /* Point the file pointers in the connection state to point to
                the first byte of the file. */

                // SendString("state = HTTP_FILE\n");  //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
                hs->dataptr = fsfile.dat;
                hs->count = fsfile.len;
                //uart0_send(hs->dataptr, 50);//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
            }
        }

        if(hs->state != HTTP_FUNC)
        {
            // Check if the client (remote end) has acknowledged any data that
            //we've previously sent. If so, we move the file pointer further
            //into the file and send back more data. If we are out of data to
            //send, we close the connection.
            if( uip_acked() )    //有ack回应表示数据还没有发完
            {
                // SendString("uip_acked\n");   //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

                if(hs->count >= uip_mss())   //如果要发出的数据大于最大段的大小
                {
                    hs->count -= uip_mss();
                    hs->dataptr += uip_mss();
                }
                else  hs->count = 0;

                if (hs->count == 0)
                {
                    if(hs->script != NULL)
                    {
                        //SendString("next_scriptline scriptstate\n");  //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
                        next_scriptline();	  // 指向下一行
                        next_scriptstate();
                    }
                    else  uip_close();
                }
            }
        }
        else  //有的function的acked
        {
            /* Call the CGI function. */
            if(cgitab[hs->script[2] - ISO_a](uip_acked()))
            {
                /* If the function returns non-zero, we jump to the next line
                in the script. */
                /*如果函数返回非零值，我们跳到下一行
                在脚本。*/
                //SendString("script[2]-ISO_a\n");  //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
                next_scriptline();    // 指向下一行
                next_scriptstate();
            }
        }

        if(hs->state != HTTP_FUNC && !uip_poll())
        {
            /* Send a piece of data, but not more than the MSS of the
            connection. */
            //SendString("uip_send\n");  //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
            uip_send(hs->dataptr,hs->count > uip_mss()? uip_mss(): hs->count);
        }

        /* Finally, return to uIP. Our outgoing packet will soon be on its
           way... */
        return;

    default:
    {
        /* Should never happen. */
        uip_abort();
        //SendString("uip_abort\n");  //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        break;
    }
    }
}
/*-----------------------------------------------------------------------------------*/
/* next_scriptline():
 *
 * Reads the script until it finds a newline. */
static void next_scriptline(void)
{
    /* Loop until we find a newline character. */
    do
    {
        ++(hs->script);
    }
    while(hs->script[0] != ISO_nl && (hs->script[0] != '\r') ); //查找换行符

    /* Eat up the newline as well. */
    ++(hs->script);
}
/*-----------------------------------------------------------------------------------*/
/* next_sciptstate:
 *
 * Reads one line of script and decides what to do next.
 */
static void next_scriptstate(void)
{
    struct fs_file fsfile;
    u16_t i;

again:
    switch(hs->script[0])
    {
    case ISO_t:
        /* Send a text string. */
        hs->state = HTTP_TEXT;
        hs->dataptr = &hs->script[2];

        /* Calculate length of string. */
        for(i = 0; hs->dataptr[i] != ISO_nl; ++i);
        hs->count = i;
        break;
    case ISO_c:         //有" C "号注释的内容，进入HTTP_FUNC
        /* Call a function. */
        hs->state = HTTP_FUNC;            //进入HTTP_FUNC
        hs->dataptr = NULL;
        hs->count = 0;
        cgitab[hs->script[2] - ISO_a](0);
        break;
    case ISO_i:   //0x69     用于发送header.html和footer.plain(退出后直接发出去)
        /* Include a file. */
        hs->state = HTTP_FILE;
        if(!fs_open(&hs->script[2], &fsfile))
        {
            uip_abort();
        }
        hs->dataptr = fsfile.dat;
        hs->count = fsfile.len;
        break;
    case ISO_hash:    //有" # "号注释的内容，指向下一行
        /* Comment line. */
        next_scriptline();
        goto again;
        // break;
    case ISO_period:  //有" . "号注释的内容，结束
        /* End of script. */
        hs->state = HTTP_END;
        uip_close();
        break;
    default:
    {
        //uip_abort();
        hs->state = HTTP_END;
        uip_close();
    }
    break;
    }
}

/*-----------------------------------------------------------------------------------*/
/** @} */
/** @} */
