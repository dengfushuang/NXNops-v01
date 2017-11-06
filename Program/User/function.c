#include "function.h"
/****错误：0-----指令校验错误；1------指令错误****/
void err(uint8 e)
{
	char er[10];
	uint8 len;
	CPU_SR cpu_sr;
	OS_ENTER_CRITICAL();
	len=sprintf(er,"%s%d","ERROR",e);
	UART0Put_str((uint8 *)er,len);
	OS_EXIT_CRITICAL();
}
void restore(){
    
        
}
void reset()
{
	uint32 i;
	LPC_WDT->TC  = 0XF424;    //设置WDT定时值为1秒.
	LPC_WDT->MOD = 0x03;
	for(i = 200 ; i > 2 ; i--);
	LPC_WDT->FEED = 0xAA;
	LPC_WDT->FEED = 0x55;
	for(i = 120000 ; i > 2 ;i--);
}
void light_reset(void)
{
	uint8 i;
	char a[8]={'\0'};
	for(i = 0 ; i < MAX_CHANNEL ; i++)
	{
		EPROM.Channel[i] = 0;
	}
	Save_To_EPROM((uint8 *)&EPROM.Channel[0],MAX_CHANNEL);
	i = creat_ch(a,"OK");
	UART0Put_str((uint8*)&a,i);
}

