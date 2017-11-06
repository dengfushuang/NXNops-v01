
#ifndef  __INCLUDES_H__
#define  __INCLUDES_H__
#ifdef __cplusplus
extern "C" {
#endif
	
	
/*****用户配置头文件****/
#include "user_config.h"
#include "config.h"
#include "user_Task.h"


/* 系统头文件Includes ------------------------------------------------------------------*/
#include  <stdio.h>
#include  <string.h>
#include  <ctype.h>
#include  <stdlib.h>
#include  <stdarg.h>
#include  <math.h>

#include  <ucos_ii.h>
#include  <os_cpu.h>
#include  <cpu.h>

/********************************/
/*      uC/OS-II specital code  */
/*      uC/OS-II的特殊代码      */
/********************************/

#define     USER_USING_MODE    0x10                    /*  User mode ,ARM 32BITS CODE 用户模式,ARM代码                  */
// 
                                                     /*  Chosen one from 0x10,0x30,0x1f,0x3f.只能是0x10,0x30,0x1f,0x3f之一       */
#if (OS_VIEW_MODULE == DEF_ENABLED)
#include  <os_viewc.h>
#include  <os_view.h>
#endif
#include "os_bsp.h"




/*****驱动头文件*****/
#include  "LPC177x_8x.h"
#include "lpc177x_8x_eeprom.h"
#include  "lpc17xx_gpio.h"
#include  "lpc17xx_uart.h"
#include  "lpc17xx_pinsel.h"
#include "core_cm3.h"




/******自定义头文件******/
#include "delay.h"
#include "queue.h"
#include "uart0.h"
#include "uart1.h"
#include "uart2.h"
#include "drv595.h"
#include "Cmd_process.h"
#include "function.h"

#ifdef __cplusplus
}
#endif

#endif

/*********************************************************************************************************
      END FILE
*********************************************************************************************************/


