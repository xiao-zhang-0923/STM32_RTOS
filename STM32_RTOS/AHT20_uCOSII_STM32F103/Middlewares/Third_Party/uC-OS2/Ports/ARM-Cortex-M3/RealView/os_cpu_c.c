/*
*********************************************************************************************************
*                                                uC/OS-II
*                                          The Real-Time Kernel
*
*                         (c) Copyright 2009-2013; Micrium, Inc.; Weston, FL
*                    All rights reserved.  Protected by international copyright laws.
*
*                                           ARM Cortex-M3 Port
*                                      STM32F103 / Keil MDK (ARMCC)
*
* File      : OS_CPU_C.C
* Version   : V2.92.11 (ported from the Micrium Cortex-M4 RealView port)
*
* Notes     : 1) This file provides the task stack initializer, the uC/OS-II hooks and the
*                SysTick handler/initializer for STM32F103.
*             2) HAL owns the 1 ms SysTick time base; SysTick_Handler() calls both
*                HAL_IncTick() and OS_CPU_SysTickHandler().
*********************************************************************************************************
*/

#define   OS_CPU_GLOBALS

#include  <ucos_ii.h>
#include  "stm32f1xx.h"

/*
*********************************************************************************************************
*                                      EXCEPTION (ISR) STACK
*********************************************************************************************************
*/

__align(8) OS_STK  OS_CPU_ExceptStk[OS_CPU_EXCEPT_STK_SIZE];
           OS_STK *OS_CPU_ExceptStkBase;

/*
*********************************************************************************************************
*                                       OS INITIALIZATION HOOK (BEGINNING)
*********************************************************************************************************
*/

#if OS_CPU_HOOKS_EN > 0u
void  OSInitHookBegin (void)
{
    INT32U   size;
    OS_STK  *pstk;

    pstk = &OS_CPU_ExceptStk[0];
    size = OS_CPU_EXCEPT_STK_SIZE;
    while (size > 0u) {
        size--;
        *pstk++ = (OS_STK)0;
    }

    OS_CPU_ExceptStkBase = (OS_STK *)&OS_CPU_ExceptStk[OS_CPU_EXCEPT_STK_SIZE];
    OS_CPU_ExceptStkBase = (OS_STK *)((OS_STK)(OS_CPU_ExceptStkBase) & 0xFFFFFFF8u);
}
#endif

/*
*********************************************************************************************************
*                                       OS INITIALIZATION HOOK (END)
*********************************************************************************************************
*/

#if OS_CPU_HOOKS_EN > 0u
void  OSInitHookEnd (void)
{
}
#endif

/*
*********************************************************************************************************
*                                          TASK CREATION HOOK
*********************************************************************************************************
*/

#if OS_CPU_HOOKS_EN > 0u
void  OSTaskCreateHook (OS_TCB *ptcb)
{
    (void)ptcb;
}
#endif

/*
*********************************************************************************************************
*                                           TASK DELETION HOOK
*********************************************************************************************************
*/

#if OS_CPU_HOOKS_EN > 0u
void  OSTaskDelHook (OS_TCB *ptcb)
{
    (void)ptcb;
}
#endif

/*
*********************************************************************************************************
*                                             IDLE TASK HOOK
*********************************************************************************************************
*/

#if OS_CPU_HOOKS_EN > 0u
void  OSTaskIdleHook (void)
{
}
#endif

/*
*********************************************************************************************************
*                                            TASK RETURN HOOK
*********************************************************************************************************
*/

#if OS_CPU_HOOKS_EN > 0u
void  OSTaskReturnHook (OS_TCB *ptcb)
{
    (void)ptcb;
}
#endif

/*
*********************************************************************************************************
*                                           STATISTIC TASK HOOK
*********************************************************************************************************
*/

#if OS_CPU_HOOKS_EN > 0u
void  OSTaskStatHook (void)
{
}
#endif

/*
*********************************************************************************************************
*                                        INITIALIZE A TASK'S STACK
*
* The Cortex-M3 hardware exception frame is created manually. Stack layout from low to high address:
*
*   R4 R5 R6 R7 R8 R9 R10 R11 | R0 R1 R2 R3 R12 LR PC xPSR
*   ^                         ^
*   OSTCBStkPtr               PSP after PendSV restores R4-R11
*
* LR is initialized to OS_TaskReturn so that a task which accidentally returns is handled safely.
*********************************************************************************************************
*/

OS_STK *OSTaskStkInit (void (*task)(void *p_arg), void *p_arg, OS_STK *ptos, INT16U opt)
{
    OS_STK *p_stk;

    (void)opt;
    p_stk = ptos + 1u;                                       /* Top of stack (high address)          */
    p_stk = (OS_STK *)((OS_STK)(p_stk) & 0xFFFFFFF8u);       /* 8-byte alignment                     */

    *(--p_stk) = (OS_STK)0x01000000uL;                       /* xPSR: Thumb bit set                  */
    *(--p_stk) = (OS_STK)task;                               /* PC: task entry point                 */
    *(--p_stk) = (OS_STK)OS_TaskReturn;                      /* LR: return hook                      */
    *(--p_stk) = (OS_STK)0x12121212uL;                       /* R12                                  */
    *(--p_stk) = (OS_STK)0x03030303uL;                       /* R3                                   */
    *(--p_stk) = (OS_STK)0x02020202uL;                       /* R2                                   */
    *(--p_stk) = (OS_STK)0x01010101uL;                       /* R1                                   */
    *(--p_stk) = (OS_STK)p_arg;                              /* R0: task argument                    */

    *(--p_stk) = (OS_STK)0x11111111uL;                       /* R11                                  */
    *(--p_stk) = (OS_STK)0x10101010uL;                       /* R10                                  */
    *(--p_stk) = (OS_STK)0x09090909uL;                       /* R9                                   */
    *(--p_stk) = (OS_STK)0x08080808uL;                       /* R8                                   */
    *(--p_stk) = (OS_STK)0x07070707uL;                       /* R7                                   */
    *(--p_stk) = (OS_STK)0x06060606uL;                       /* R6                                   */
    *(--p_stk) = (OS_STK)0x05050505uL;                       /* R5                                   */
    *(--p_stk) = (OS_STK)0x04040404uL;                       /* R4                                   */

    return (p_stk);
}

/*
*********************************************************************************************************
*                                           TASK SWITCH HOOK
*
* os_cpu_a.asm calls this symbol unconditionally, so it must always be linked.
*********************************************************************************************************
*/

#if OS_CPU_HOOKS_EN > 0u
void  OSTaskSwHook (void)
{
}
#endif

/*
*********************************************************************************************************
*                                           OS_TCBInit() HOOK
*********************************************************************************************************
*/

#if OS_CPU_HOOKS_EN > 0u
void  OSTCBInitHook (OS_TCB *ptcb)
{
    (void)ptcb;
}
#endif

/*
*********************************************************************************************************
*                                               TICK HOOK
*********************************************************************************************************
*/

#if (OS_CPU_HOOKS_EN > 0u) && (OS_TIME_TICK_HOOK_EN > 0u)
void  OSTimeTickHook (void)
{
}
#endif

/*
*********************************************************************************************************
*                                          SYS TICK HANDLER
*
* Called from SysTick_Handler() every 1 ms. This function tells uC/OS-II that an ISR has started,
* processes the kernel tick and lets OSIntExit() schedule a new task if necessary.
*********************************************************************************************************
*/

void  OS_CPU_SysTickHandler (void)
{
    OSIntEnter();
    OSTimeTick();
    OSIntExit();
}

/*
*********************************************************************************************************
*                                         INITIALIZE SYS TICK
*
* HAL already configures SysTick to 1 ms in HAL_Init() and HAL_RCC_ClockConfig().
* This function is provided for compatibility with the Micrium port; it is not called by main().
*********************************************************************************************************
*/

void  OS_CPU_SysTickInit (INT32U cnts)
{
    if (cnts == 0u) {
        return;
    }

    SysTick->LOAD = cnts - 1u;
    SysTick->VAL  = 0u;
    NVIC_SetPriority(SysTick_IRQn, 15u);
    SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk |
                    SysTick_CTRL_TICKINT_Msk   |
                    SysTick_CTRL_ENABLE_Msk;
}
