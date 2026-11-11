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
* File      : OS_CPU.H
* Version   : V2.92.11 (ported from the Micrium Cortex-M4 RealView port)
*
* Notes     : 1) This port targets the ARMv7-M Cortex-M3 core of STM32F103.
*             2) No FPU context save is required on STM32F103 (Cortex-M3 has no FPU).
*             3) The kernel tick is supplied by the 1 ms SysTick interrupt configured by HAL.
*********************************************************************************************************
*/

#ifndef  OS_CPU_H
#define  OS_CPU_H

#ifdef   OS_CPU_GLOBALS
#define  OS_CPU_EXT
#else
#define  OS_CPU_EXT  extern
#endif

/*
*********************************************************************************************************
*                                      EXCEPTION (ISR) STACK
*********************************************************************************************************
*/

#ifndef  OS_CPU_EXCEPT_STK_SIZE
#define  OS_CPU_EXCEPT_STK_SIZE    256u          /* 256 x 4 bytes = 1 KB dedicated MSP stack          */
#endif

/*
*********************************************************************************************************
*                                               DEFINES
*********************************************************************************************************
*/

#define  OS_CPU_ARM_FP_EN                     0u /* Cortex-M3 has no FPU                               */

/*
*********************************************************************************************************
*                                              DATA TYPES
*                                         (Compiler Specific)
*********************************************************************************************************
*/

typedef unsigned char  BOOLEAN;
typedef unsigned char  INT8U;                    /* Unsigned  8 bit quantity                           */
typedef signed   char  INT8S;                    /* Signed    8 bit quantity                           */
typedef unsigned short INT16U;                   /* Unsigned 16 bit quantity                           */
typedef signed   short INT16S;                   /* Signed   16 bit quantity                           */
typedef unsigned int   INT32U;                   /* Unsigned 32 bit quantity                           */
typedef signed   int   INT32S;                   /* Signed   32 bit quantity                           */
typedef float          FP32;                     /* Single precision floating point                    */
typedef double         FP64;                     /* Double precision floating point                    */

typedef unsigned int   OS_STK;                   /* Each stack entry is 32-bit wide                    */
typedef unsigned int   OS_CPU_SR;                /* CPU status register (PRIMASK) is 32 bits           */

/*
*********************************************************************************************************
*                                      CRITICAL SECTION MANAGEMENT
*
* Method #3: save PRIMASK in a local variable, disable interrupts, then restore PRIMASK.
*********************************************************************************************************
*/

#define  OS_CRITICAL_METHOD   3u

#if OS_CRITICAL_METHOD == 3u
#define  OS_ENTER_CRITICAL()  {cpu_sr = OS_CPU_SR_Save();}
#define  OS_EXIT_CRITICAL()   {OS_CPU_SR_Restore(cpu_sr);}
#endif

/*
*********************************************************************************************************
*                                        Cortex-M3 Miscellaneous
*********************************************************************************************************
*/

#define  OS_STK_GROWTH        1u                  /* Stack grows from HIGH to LOW memory on ARM         */
#define  OS_TASK_SW()         OSCtxSw()

/*
*********************************************************************************************************
*                                          GLOBAL VARIABLES
*********************************************************************************************************
*/

OS_CPU_EXT  OS_STK   OS_CPU_ExceptStk[OS_CPU_EXCEPT_STK_SIZE];
OS_CPU_EXT  OS_STK  *OS_CPU_ExceptStkBase;

/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*/

#if OS_CRITICAL_METHOD == 3u
OS_CPU_SR  OS_CPU_SR_Save    (void);
void       OS_CPU_SR_Restore (OS_CPU_SR cpu_sr);
#endif

void  OSCtxSw                (void);
void  OSIntCtxSw             (void);
void  OSStartHighRdy         (void);

void  OS_CPU_PendSVHandler   (void);

void  OS_CPU_SysTickHandler  (void);
void  OS_CPU_SysTickInit     (INT32U cnts);

#endif
