#ifndef __USER_CONFIG_H
#define __USER_CONFIG_H
#ifdef __cplusplus
extern "C" {
#endif

	
/*****************************
�����߼��������user_Task.c�ļ�
***********************/	
/******�û�������********/
	

#define USE_DEV_UART0           //ʹ�ô���0
#define USE_DEV_UART1           //ʹ�ô���1
//#define USE_DEV_UART2           //ʹ�ô���2





#define MAIN_CPU   
#ifdef MAIN_CPU
    #define MASTER_CPU   //������CPU
#else
    #define SLAVE_CPU    //ҵ����CPU
#endif
#ifdef USE_CAT1025       
    #define   CAT1025		0xA0		/* CAT1025��I2C��ַ */
#endif


#ifdef __cplusplus
}
#endif

#endif
