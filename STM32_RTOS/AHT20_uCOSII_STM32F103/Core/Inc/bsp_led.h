/**
  ******************************************************************************
  * @file    bsp_led.h
  * @brief   LED hardware mapping: LED1 = PB0, LED2 = PB1, active HIGH.
  *
  * Wiring:
  *   PB0 --- 220R --- LED1 anode (long leg), LED1 cathode --- GND
  *   PB1 --- 220R --- LED2 anode (long leg), LED2 cathode --- GND
  ******************************************************************************
  */

#ifndef __BSP_LED_H
#define __BSP_LED_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

#define LED1_PORT           GPIOB
#define LED1_PIN            GPIO_PIN_0

#define LED2_PORT           GPIOB
#define LED2_PIN            GPIO_PIN_1

typedef enum
{
    LED1 = 0,
    LED2 = 1
} Led_TypeDef;

void BSP_LED_Init(void);
void BSP_LED_On(Led_TypeDef led);
void BSP_LED_Off(Led_TypeDef led);

#ifdef __cplusplus
}
#endif

#endif /* __BSP_LED_H */
