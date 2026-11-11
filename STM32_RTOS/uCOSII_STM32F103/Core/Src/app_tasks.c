/**
  ******************************************************************************
  * @file    app_tasks.c
  * @brief   Three uC/OS-II application tasks.
  *
  *  Task 1 (priority 4): LED1 on PB0, complete on/off cycle = 1 s
  *                        ON 500 ms, OFF 500 ms, then repeat.
  *  Task 2 (priority 5): LED2 on PB1, complete on/off cycle = 3 s
  *                        ON 1500 ms, OFF 1500 ms, then repeat.
  *  Task 3 (priority 6): send the UTF-8 message on USART1 every 2 s.
  *
  *  Each task is a "periodic" task: the sum of its delays equals its period.
  *  OSTimeDlyHMSM() blocks the task, so lower-priority tasks get the CPU while
  *  a task is waiting; this is the key difference from a bare-metal delay loop.
  ******************************************************************************
  */

#include "app_tasks.h"
#include "app_cfg.h"
#include "bsp_led.h"
#include "usart.h"
#include "ucos_ii.h"

#define LED1_HALF_PERIOD_SEC      0u
#define LED1_HALF_PERIOD_MS     500u
#define LED2_HALF_PERIOD_SEC      1u
#define LED2_HALF_PERIOD_MS     500u
#define UART_PERIOD_SEC           2u

/* 8-byte aligned stacks: the Cortex-M3 exception frame requires 8-byte alignment. */
__align(8) static OS_STK Task1Stk[APP_TASK_STK_SIZE];
__align(8) static OS_STK Task2Stk[APP_TASK_STK_SIZE];
__align(8) static OS_STK Task3Stk[APP_TASK_STK_SIZE];

static void Task1_LED1(void *p_arg);
static void Task2_LED2(void *p_arg);
static void Task3_UART(void *p_arg);

/*
 * UTF-8 bytes for:
 *   "hello uc/OS! 欢迎来到RTOS多任务环境！\r\n"
 *
 * The string is written as hex escapes so the transmitted bytes do not depend
 * on the source-file encoding used by Keil.  Select UTF-8 in the serial terminal.
 */
static const char UART_MSG[] =
    "hello uc/OS! "
    "\xE6\xAC\xA2"                 /* Huan  */
    "\xE8\xBF\x8E"                 /* Ying  */
    "\xE6\x9D\xA5"                 /* Lai   */
    "\xE5\x88\xB0"                 /* Dao   */
    "RTOS"
    "\xE5\xA4\x9A"                 /* Duo   */
    "\xE4\xBB\xBB"                 /* Ren   */
    "\xE5\x8A\xA1"                 /* Wu    */
    "\xE7\x8E\xAF"                 /* Huan  */
    "\xE5\xA2\x83"                 /* Jing  */
    "\xEF\xBC\x81"                 /* !     */
    "\r\n";

void AppTaskCreate(void)
{
    INT8U err;

    err = OSTaskCreateExt(Task1_LED1,
                          (void *)0,
                          &Task1Stk[APP_TASK_STK_SIZE - 1u],
                          APP_TASK_LED1_PRIO,
                          APP_TASK_LED1_PRIO,
                          &Task1Stk[0],
                          APP_TASK_STK_SIZE,
                          (void *)0,
                          OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);
    if (err != OS_ERR_NONE)
    {
        Error_Handler();
    }

    err = OSTaskCreateExt(Task2_LED2,
                          (void *)0,
                          &Task2Stk[APP_TASK_STK_SIZE - 1u],
                          APP_TASK_LED2_PRIO,
                          APP_TASK_LED2_PRIO,
                          &Task2Stk[0],
                          APP_TASK_STK_SIZE,
                          (void *)0,
                          OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);
    if (err != OS_ERR_NONE)
    {
        Error_Handler();
    }

    err = OSTaskCreateExt(Task3_UART,
                          (void *)0,
                          &Task3Stk[APP_TASK_STK_SIZE - 1u],
                          APP_TASK_UART_PRIO,
                          APP_TASK_UART_PRIO,
                          &Task3Stk[0],
                          APP_TASK_STK_SIZE,
                          (void *)0,
                          OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);
    if (err != OS_ERR_NONE)
    {
        Error_Handler();
    }

    /* Names are optional; they make the task list easier to read in a debugger. */
    OSTaskNameSet(APP_TASK_LED1_PRIO, (INT8U *)"LED1_1s", &err);
    OSTaskNameSet(APP_TASK_LED2_PRIO, (INT8U *)"LED2_3s", &err);
    OSTaskNameSet(APP_TASK_UART_PRIO, (INT8U *)"UART_2s", &err);
}

/**
  * @brief  Task 1: 1 s complete LED1 on/off cycle (500 ms ON, 500 ms OFF).
  */
static void Task1_LED1(void *p_arg)
{
    (void)p_arg;

    for (;;)
    {
        BSP_LED_On(LED1);
        (void)OSTimeDlyHMSM(0u, 0u, LED1_HALF_PERIOD_SEC, LED1_HALF_PERIOD_MS);

        BSP_LED_Off(LED1);
        (void)OSTimeDlyHMSM(0u, 0u, LED1_HALF_PERIOD_SEC, LED1_HALF_PERIOD_MS);
    }
}

/**
  * @brief  Task 2: 3 s complete LED2 on/off cycle (1500 ms ON, 1500 ms OFF).
  */
static void Task2_LED2(void *p_arg)
{
    (void)p_arg;

    for (;;)
    {
        BSP_LED_On(LED2);
        (void)OSTimeDlyHMSM(0u, 0u, LED2_HALF_PERIOD_SEC, LED2_HALF_PERIOD_MS);

        BSP_LED_Off(LED2);
        (void)OSTimeDlyHMSM(0u, 0u, LED2_HALF_PERIOD_SEC, LED2_HALF_PERIOD_MS);
    }
}

/**
  * @brief  Task 3: send the welcome message every 2 s.
  */
static void Task3_UART(void *p_arg)
{
    (void)p_arg;

    for (;;)
    {
        USART1_SendString(UART_MSG);
        (void)OSTimeDlyHMSM(0u, 0u, UART_PERIOD_SEC, 0u);
    }
}
