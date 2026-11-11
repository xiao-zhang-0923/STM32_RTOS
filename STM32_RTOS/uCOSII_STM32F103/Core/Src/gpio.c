/**
  ******************************************************************************
  * @file    gpio.c
  * @brief   PB0 and PB1 push-pull outputs for the two LEDs.
  *
  * Both pins are driven LOW before being configured as outputs, so the LEDs
  * start in the OFF state and do not flash during GPIO initialization.
  ******************************************************************************
  */

#include "gpio.h"

void MX_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    __HAL_RCC_GPIOB_CLK_ENABLE();

    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0 | GPIO_PIN_1, GPIO_PIN_RESET);

    GPIO_InitStruct.Pin   = GPIO_PIN_0 | GPIO_PIN_1;
    GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull  = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}
