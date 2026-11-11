/**
  ******************************************************************************
  * @file    usart.h
  * @brief   USART1 (115200 8N1) declarations.
  ******************************************************************************
  */

#ifndef __USART_H
#define __USART_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

extern UART_HandleTypeDef huart1;

void MX_USART1_UART_Init(void);
void USART1_SendString(const char *str);

#ifdef __cplusplus
}
#endif

#endif /* __USART_H */
