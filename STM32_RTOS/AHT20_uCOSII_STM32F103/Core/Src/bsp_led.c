/**
  ******************************************************************************
  * @file    bsp_led.c
  * @brief   LED control for LED1 (PB0) and LED2 (PB1), active HIGH.
  ******************************************************************************
  */

#include "bsp_led.h"

void BSP_LED_Init(void)
{
    BSP_LED_Off(LED1);
    BSP_LED_Off(LED2);
}

void BSP_LED_On(Led_TypeDef led)
{
    if (led == LED1)
    {
        HAL_GPIO_WritePin(LED1_PORT, LED1_PIN, GPIO_PIN_SET);
    }
    else
    {
        HAL_GPIO_WritePin(LED2_PORT, LED2_PIN, GPIO_PIN_SET);
    }
}

void BSP_LED_Off(Led_TypeDef led)
{
    if (led == LED1)
    {
        HAL_GPIO_WritePin(LED1_PORT, LED1_PIN, GPIO_PIN_RESET);
    }
    else
    {
        HAL_GPIO_WritePin(LED2_PORT, LED2_PIN, GPIO_PIN_RESET);
    }
}
