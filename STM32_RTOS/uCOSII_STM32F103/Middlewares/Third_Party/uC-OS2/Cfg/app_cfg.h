/*
*********************************************************************************************************
*                                            APPLICATION CONFIGURATION
*
*  File    : app_cfg.h
*  Target  : STM32F103C8T6 + uC/OS-II V2.92.11
*  Notes   : Application task priorities and stack sizes live here so they are easy to find.
*********************************************************************************************************
*/

#ifndef  APP_CFG_H
#define  APP_CFG_H

/*
*********************************************************************************************************
*                                       TASK PRIORITIES
*
* Smaller number = higher priority. Idle task uses OS_LOWEST_PRIO (15).
*********************************************************************************************************
*/

#define  APP_TASK_LED1_PRIO          4u
#define  APP_TASK_LED2_PRIO          5u
#define  APP_TASK_UART_PRIO          6u

/*
*********************************************************************************************************
*                                       TASK STACK SIZES
*
* Unit is OS_STK (4 bytes). 192 entries = 768 bytes per task.
*********************************************************************************************************
*/

#define  APP_TASK_STK_SIZE         192u

#endif
