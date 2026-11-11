/*
*********************************************************************************************************
*                                                uC/OS-II
*                                          The Real-Time Kernel
*                                  uC/OS-II Configuration File for V2.9x
*
*                               (c) Copyright 2005-2014, Micrium, Weston, FL
*                                          All Rights Reserved
*
*  File    : os_cfg.h
*  Version : V2.92.11
*  Target  : STM32F103C8T6, 72 MHz, 1 ms tick
*
*  Notes   : Only the features used by this exercise are enabled. Event objects, memory partitions,
*            timers and the statistics task are disabled to keep the image small and easy to study.
*********************************************************************************************************
*/

#ifndef OS_CFG_H
#define OS_CFG_H

                                       /* ---------------------- MISCELLANEOUS ----------------------- */
#define OS_APP_HOOKS_EN           0u   /* Application-defined hooks are called from uC/OS-II hooks    */
#define OS_ARG_CHK_EN             1u   /* Enable (1) or Disable (0) argument checking                   */
#define OS_CPU_HOOKS_EN           1u   /* uC/OS-II hooks are found in the processor port files          */

#define OS_DEBUG_EN               0u   /* Disable debug variables (no os_dbg.c required)                */

#define OS_EVENT_MULTI_EN         0u   /* Do not include OSEventPendMulti()                             */
#define OS_EVENT_NAME_EN          0u   /* Do not enable names for Sem/Mutex/Mbox/Q                      */

#define OS_LOWEST_PRIO           15u   /* Priority 15 is reserved for the idle task                     */

#define OS_MAX_EVENTS             4u   /* Max. event control blocks (not used in this project)          */
#define OS_MAX_FLAGS              2u   /* Max. event flag groups                                        */
#define OS_MAX_MEM_PART           2u   /* Max. memory partitions                                        */
#define OS_MAX_QS                 2u   /* Max. queue control blocks                                     */
#define OS_MAX_TASKS              8u   /* Max. application tasks (3 used + idle)                        */

#define OS_SCHED_LOCK_EN          0u   /* OSSchedLock()/OSSchedUnlock() not used                        */

#define OS_TICK_STEP_EN           0u   /* Tick stepping for uC/OS-View not used                         */
#define OS_TICKS_PER_SEC       1000u   /* 1 ms system tick                                              */

#define OS_TLS_TBL_SIZE           0u   /* No thread-local storage                                       */

                                       /* --------------------- TASK STACK SIZE ---------------------- */
#define OS_TASK_TMR_STK_SIZE    128u   /* Timer task stack (timer disabled, value kept for header)      */
#define OS_TASK_STAT_STK_SIZE   128u   /* Statistics task stack (statistics disabled)                   */
#define OS_TASK_IDLE_STK_SIZE   128u   /* Idle task stack: 128 x 4 = 512 bytes                          */

                                       /* --------------------- TASK MANAGEMENT ---------------------- */
#define OS_TASK_CHANGE_PRIO_EN    0u   /* OSTaskChangePrio() not used                                   */
#define OS_TASK_CREATE_EN         1u   /* Include OSTaskCreate()                                        */
#define OS_TASK_CREATE_EXT_EN     1u   /* Include OSTaskCreateExt() for stack checking                  */
#define OS_TASK_DEL_EN            1u   /* Include OSTaskDel() so a returning task can delete itself     */
#define OS_TASK_NAME_EN           1u   /* Enable task names                                             */
#define OS_TASK_PROFILE_EN        0u   /* No run-time profiling fields in the TCB                       */
#define OS_TASK_QUERY_EN          0u   /* OSTaskQuery() not used                                        */
#define OS_TASK_REG_TBL_SIZE      1u   /* One task register                                             */
#define OS_TASK_STAT_EN           0u   /* Disable statistics task                                       */
#define OS_TASK_STAT_STK_CHK_EN   0u   /* No statistics-task stack checking                             */
#define OS_TASK_SUSPEND_EN        0u   /* OSTaskSuspend()/OSTaskResume() not used                       */
#define OS_TASK_SW_HOOK_EN        1u   /* Port assembly calls OSTaskSwHook(), so keep this enabled      */

                                       /* ----------------------- EVENT FLAGS ------------------------ */
#define OS_FLAG_EN                0u
#define OS_FLAG_ACCEPT_EN         0u
#define OS_FLAG_DEL_EN            0u
#define OS_FLAG_NAME_EN           0u
#define OS_FLAG_QUERY_EN          0u
#define OS_FLAG_WAIT_CLR_EN       0u
#define OS_FLAGS_NBITS           16u

                                       /* -------------------- MESSAGE MAILBOXES --------------------- */
#define OS_MBOX_EN                0u
#define OS_MBOX_ACCEPT_EN         0u
#define OS_MBOX_DEL_EN            0u
#define OS_MBOX_PEND_ABORT_EN     0u
#define OS_MBOX_POST_EN           0u
#define OS_MBOX_POST_OPT_EN       0u
#define OS_MBOX_QUERY_EN          0u

                                       /* --------------------- MEMORY MANAGEMENT -------------------- */
#define OS_MEM_EN                 0u
#define OS_MEM_NAME_EN            0u
#define OS_MEM_QUERY_EN           0u

                                       /* ---------------- MUTUAL EXCLUSION SEMAPHORES --------------- */
#define OS_MUTEX_EN               0u
#define OS_MUTEX_ACCEPT_EN        0u
#define OS_MUTEX_DEL_EN           0u
#define OS_MUTEX_QUERY_EN         0u

                                       /* ---------------------- MESSAGE QUEUES ---------------------- */
#define OS_Q_EN                   0u
#define OS_Q_ACCEPT_EN            0u
#define OS_Q_DEL_EN               0u
#define OS_Q_FLUSH_EN             0u
#define OS_Q_PEND_ABORT_EN        0u
#define OS_Q_POST_EN              0u
#define OS_Q_POST_FRONT_EN        0u
#define OS_Q_POST_OPT_EN          0u
#define OS_Q_QUERY_EN             0u

                                       /* ------------------------ SEMAPHORES ------------------------ */
#define OS_SEM_EN                 0u
#define OS_SEM_ACCEPT_EN          0u
#define OS_SEM_DEL_EN             0u
#define OS_SEM_PEND_ABORT_EN      0u
#define OS_SEM_QUERY_EN           0u
#define OS_SEM_SET_EN             0u

                                       /* --------------------- TIME MANAGEMENT ---------------------- */
#define OS_TIME_DLY_HMSM_EN       1u   /* OSTimeDlyHMSM() used by all three tasks                       */
#define OS_TIME_DLY_RESUME_EN     0u
#define OS_TIME_GET_SET_EN        1u
#define OS_TIME_TICK_HOOK_EN      0u

                                       /* --------------------- TIMER MANAGEMENT --------------------- */
#define OS_TMR_EN                 0u
#define OS_TMR_CFG_MAX           16u
#define OS_TMR_CFG_NAME_EN        0u
#define OS_TMR_CFG_WHEEL_SIZE     7u
#define OS_TMR_CFG_TICKS_PER_SEC 10u

#endif
