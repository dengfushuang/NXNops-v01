#ifndef __USER_TASK__H
#define __USER_TASK__H

#ifdef __cplusplus
extern "C" {
#endif

#include "includes.h"


extern void TaskUart0Cmd(void* pdata);
extern void TaskUart0Rcv(void* pdata);

extern void TaskUart1Cmd(void* pdata);
extern void TaskUart1Rcv(void* pdata);



#ifdef __cplusplus
}
#endif
#endif

