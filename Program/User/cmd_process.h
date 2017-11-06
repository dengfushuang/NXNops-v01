#ifndef __CMD_PROCESS_H_
#define __CMD_PROCESS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "includes.h"
typedef enum{
	ERR=0,
    GETVERSION,
    SETSW,
	CH,
	GETSWA,
	GETSW,
	RST,
	OKCH,
	OKSETSW,
	OKRST
}CMD_ID;

//extern uint16 cmd_process( char *sprintf_buf );
uint16 creat_ch(char *dest,char *src);
extern int cmd_process( char* sprintf_buf ,int *str_len);


#ifdef __cplusplus
}
#endif
#endif //__CMD_PROCESS_H_

