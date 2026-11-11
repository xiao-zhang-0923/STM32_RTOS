/**
  ******************************************************************************
  * @file    usart.c
  * @brief   USART1 at 115200 8N1 and a simple blocking string sender.
  ******************************************************************************
  */

#include "usart.h"
#include <string.h>

UART_HandleTypeDef huart1;

void MX_USART1_UART_Init(void)
{
    huart1.Instance          = USART1;
    huart1.Init.BaudRate     = 115200;
    huart1.Init.WordLength   = UART_WORDLENGTH_8B;
    huart1.Init.StopBits     = UART_STOPBITS_1;
    huart1.Init.Parity       = UART_PARITY_NONE;
    huart1.Init.Mode         = UART_MODE_TX_RX;
    huart1.Init.HwFlowCtl    = UART_HWCONTROL_NONE;
    huart1.Init.OverSampling = UART_OVERSAMPLING_16;

    if (HAL_UART_Init(&huart1) != HAL_OK)
    {
        Error_Handler();
    }
}

void USART1_SendString(const char *str)
{
    if (str == NULL)
    {
        return;
    }

    (void)HAL_UART_Transmit(&huart1,
                            (uint8_t *)str,
                            (uint16_t)strlen(str),
                            100u);
}
