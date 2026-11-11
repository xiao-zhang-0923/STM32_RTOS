;
;********************************************************************************************************
;                                                uC/OS-II
;                                          The Real-Time Kernel
;
;                         (c) Copyright 2009-2013; Micrium, Inc.; Weston, FL
;                    All rights reserved.  Protected by international copyright laws.
;
;                                           ARM Cortex-M3 Port
;                                      STM32F103 / Keil MDK (ARMCC)
;
; File      : OS_CPU_A.ASM
; Version   : V2.92.11 (ported from the Micrium Cortex-M4 RealView port)
;
; Notes     : 1) Context switches are deferred to PendSV, the lowest priority exception.
;             2) The Cortex-M3 hardware stacks R0-R3, R12, LR, PC and xPSR automatically.
;                This file only saves/restores R4-R11 and switches PSP.
;             3) No floating-point context is saved because STM32F103 has no FPU.
;********************************************************************************************************

;********************************************************************************************************
;                                          PUBLIC FUNCTIONS
;********************************************************************************************************

    EXTERN  OSRunning
    EXTERN  OSPrioCur
    EXTERN  OSPrioHighRdy
    EXTERN  OSTCBCur
    EXTERN  OSTCBHighRdy
    EXTERN  OSTaskSwHook
    EXTERN  OS_CPU_ExceptStkBase

    EXPORT  OS_CPU_SR_Save
    EXPORT  OS_CPU_SR_Restore
    EXPORT  OSStartHighRdy
    EXPORT  OSCtxSw
    EXPORT  OSIntCtxSw
    EXPORT  OS_CPU_PendSVHandler
    EXPORT  PendSV_Handler

;********************************************************************************************************
;                                               EQUATES
;********************************************************************************************************

NVIC_INT_CTRL   EQU     0xE000ED04          ; Interrupt control state register (ICSR)
NVIC_SYSPRI14   EQU     0xE000ED22          ; System handler priority register for PendSV
NVIC_PENDSV_PRI EQU     0xFF                ; PendSV priority: lowest (0xF0 in a 4-bit implementation)
NVIC_PENDSVSET  EQU     0x10000000          ; ICSR.PENDSVSET

;********************************************************************************************************
;                                     CODE GENERATION DIRECTIVES
;********************************************************************************************************

    AREA |.text|, CODE, READONLY, ALIGN=2
    THUMB
    PRESERVE8

;********************************************************************************************************
;                                   CRITICAL SECTION METHOD 3 FUNCTIONS
;
; OS_CPU_SR  OS_CPU_SR_Save(void);
; void       OS_CPU_SR_Restore(OS_CPU_SR cpu_sr);
;********************************************************************************************************

OS_CPU_SR_Save
    MRS     R0, PRIMASK                     ; R0 = previous interrupt mask
    CPSID   I                               ; Disable interrupts (except faults)
    BX      LR

OS_CPU_SR_Restore
    MSR     PRIMASK, R0                     ; Restore previous interrupt mask
    BX      LR

;********************************************************************************************************
;                                         START MULTITASKING
;                                      void OSStartHighRdy(void)
;
; 1) Set PendSV to the lowest priority.
; 2) Set PSP = 0 so the first PendSV knows that no task context must be saved.
; 3) Point MSP at the dedicated exception stack.
; 4) Set OSRunning = TRUE, pend PendSV, enable interrupts.
;********************************************************************************************************

OSStartHighRdy
    LDR     R0, =NVIC_SYSPRI14
    LDR     R1, =NVIC_PENDSV_PRI
    STRB    R1, [R0]

    MOVS    R0, #0
    MSR     PSP, R0                         ; PSP = 0 marks the first context switch

    LDR     R0, =OS_CPU_ExceptStkBase
    LDR     R1, [R0]
    MSR     MSP, R1                         ; Use the dedicated handler stack

    LDR     R0, =OSRunning
    MOVS    R1, #1
    STRB    R1, [R0]                        ; OSRunning = TRUE

    LDR     R0, =NVIC_INT_CTRL
    LDR     R1, =NVIC_PENDSVSET
    STR     R1, [R0]                        ; Pend PendSV: perform the first context switch

    CPSIE   I                               ; Enable interrupts; PendSV runs immediately
OSStartHang
    B       OSStartHang                     ; Must never return here

;********************************************************************************************************
;                       PERFORM A CONTEXT SWITCH (From task level) - OSCtxSw()
;********************************************************************************************************

OSCtxSw
    LDR     R0, =NVIC_INT_CTRL
    LDR     R1, =NVIC_PENDSVSET
    STR     R1, [R0]
    BX      LR

;********************************************************************************************************
;                   PERFORM A CONTEXT SWITCH (From interrupt level) - OSIntCtxSw()
;********************************************************************************************************

OSIntCtxSw
    LDR     R0, =NVIC_INT_CTRL
    LDR     R1, =NVIC_PENDSVSET
    STR     R1, [R0]
    BX      LR

;********************************************************************************************************
;                                       HANDLE PendSV EXCEPTION
;                                   void OS_CPU_PendSVHandler(void)
;
; PendSV is the actual context switch. On entry the hardware has already stacked the interrupted
; thread's R0-R3, R12, LR, PC and xPSR on the Process Stack (PSP).
;********************************************************************************************************

OS_CPU_PendSVHandler
PendSV_Handler
    CPSID   I                               ; No interruption during the context switch
    MRS     R0, PSP                         ; R0 = current task's PSP
    CBZ     R0, PendSV_Handler_nosave       ; PSP == 0: first switch, nothing to save

    SUBS    R0, R0, #0x20                   ; Reserve space for R4-R11 (8 words)
    STM     R0, {R4-R11}                    ; Save R4-R11 on the task's own stack

    LDR     R1, =OSTCBCur
    LDR     R1, [R1]
    STR     R0, [R1]                        ; OSTCBCur->OSTCBStkPtr = SP

PendSV_Handler_nosave
    PUSH    {R14}                           ; Save EXC_RETURN
    LDR     R0, =OSTaskSwHook
    BLX     R0                              ; OSTaskSwHook()
    POP     {R14}

    LDR     R0, =OSPrioCur                  ; OSPrioCur = OSPrioHighRdy
    LDR     R1, =OSPrioHighRdy
    LDRB    R2, [R1]
    STRB    R2, [R0]

    LDR     R0, =OSTCBCur                   ; OSTCBCur = OSTCBHighRdy
    LDR     R1, =OSTCBHighRdy
    LDR     R2, [R1]
    STR     R2, [R0]

    LDR     R0, [R2]                        ; R0 = OSTCBHighRdy->OSTCBStkPtr
    LDM     R0, {R4-R11}                    ; Restore R4-R11 of the new task
    ADDS    R0, R0, #0x20
    MSR     PSP, R0                         ; PSP now points at the hardware exception frame

    ORR     LR, LR, #0xF4                   ; Return to Thread mode using PSP
    CPSIE   I
    BX      LR                              ; Exception return restores R0-R3, R12, LR, PC, xPSR

    END
