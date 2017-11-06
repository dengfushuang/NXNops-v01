#ifndef __USER_CONFIG_H
#define __USER_CONFIG_H
#ifdef __cplusplus
extern "C" {
#endif

	
/*****************************
任务逻辑代码详见user_Task.c文件
***********************/	
/******用户配置区********/
	

#define USE_DEV_UART0           //使用串口0
#define USE_DEV_UART1           //使用串口1
//#define USE_DEV_UART2           //使用串口2





#define MAIN_CPU   
#ifdef MAIN_CPU
    #define MASTER_CPU   //主控制CPU
#else
    #define SLAVE_CPU    //业务盘CPU
#endif
#ifdef USE_CAT1025       
    #define   CAT1025		0xA0		/* CAT1025的I2C地址 */
#endif


#ifdef __cplusplus
}
#endif

#endif
