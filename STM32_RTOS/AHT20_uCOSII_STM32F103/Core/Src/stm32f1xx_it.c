/**
  ******************************************************************************
  * @file    stm32f1xx_it.c
  * @brief   Cortex-M3 exception handlers and the 1 ms uC/OS-II tick.
  *
  * SysTick_Handler() serves two purposes:
  *   - HAL_IncTick() keeps HAL_GetTick()/HAL_Delay() working.
  *   - OS_CPU_SysTickHandler() runs OSTimeTick() and OSIntExit().
  *
  * PendSV_Handler() is a strong symbol exported by os_cpu_a.asm.  Do not define
  * it here: the assembly handler performs the real context switch and must be
  * the vector table entry.
  ******************************************************************************
  */

#include "main.h"
#include "stm32f1xx_it.h"
#include "os_cpu.h"

void NMI_Handler(void)
{
}

void HardFault_Handler(void)
{
    __disable_irq();
    while (1)
    {
    }
}

void MemManage_Handler(void)
{
    __disable_irq();
    while (1)
    {
    }
}

void BusFault_Handler(void)
{
    __disable_irq();
    while (1)
    {
    }
}

void UsageFault_Handler(void)
{
    __disable_irq();
    while (1)
    {
    }
}

void SVC_Handler(void)
{
}

void DebugMon_Handler(void)
{
}

void SysTick_Handler(void)
{
    HAL_IncTick();
    OS_CPU_SysTickHandler();
}
