/**
  ******************************************************************************
  * @file    app_tasks.c
  * @brief   Three uC/OS-II application tasks for the AHT20 experiment.
  *
  *  Task 1 (priority 4): LED1 on PB0, complete on/off cycle = 1 s
  *                        ON 500 ms, OFF 500 ms, then repeat.
  *  Task 2 (priority 5): LED2 on PB1, complete on/off cycle = 3 s
  *                        ON 1500 ms, OFF 1500 ms, then repeat.
  *  Task 3 (priority 6): every 2 s read AHT20 temperature/humidity over
  *                        software I2C (PB6=SCL, PB7=SDA) and print on USART1.
  *
  *  AHT20 sequence in Task 3:
  *    1. write 0xAC 0x33 0x00 to 0x70 (trigger measurement)
  *    2. wait at least 80 ms (OSTimeDlyHMSM, not HAL_Delay)
  *    3. read 7 bytes from 0x71
  *    4. convert raw 20-bit values and print
  *
  *  The task period is measured with OSTimeGet(), so the 80 ms conversion
  *  time is included in the 2 s period instead of being added to it.
  ******************************************************************************
  */

#include "app_tasks.h"
#include "app_cfg.h"
#include "bsp_led.h"
#include "usart.h"
#include "aht20.h"
#include "ucos_ii.h"

#include <stdio.h>

#define LED1_HALF_PERIOD_SEC      0u
#define LED1_HALF_PERIOD_MS     500u
#define LED2_HALF_PERIOD_SEC      1u
#define LED2_HALF_PERIOD_MS     500u

#define AHT20_PERIOD_MS         2000u
#define AHT20_CONVERSION_MS       80u
#define AHT20_RETRY_MS            20u

/* 8-byte aligned stacks: the Cortex-M3 exception frame requires 8-byte alignment. */
__align(8) static OS_STK Task1Stk[APP_TASK_STK_SIZE];
__align(8) static OS_STK Task2Stk[APP_TASK_STK_SIZE];
__align(8) static OS_STK Task3Stk[APP_TASK_STK_SIZE];

static void Task1_LED1(void *p_arg);
static void Task2_LED2(void *p_arg);
static void Task3_AHT20(void *p_arg);

/**
  * @brief  Convert a float to a fixed-point string with two decimal places.
  *         Avoids printf("%f"), which pulls a large float formatting library
  *         into the image and is unnecessary for this display format.
  */
static void FixedToStr(char *buf, size_t size, float value)
{
    int32_t scaled;
    int32_t whole;
    int32_t frac;
    uint8_t negative = 0u;

    if (value >= 0.0f)
    {
        scaled = (int32_t)(value * 100.0f + 0.5f);
    }
    else
    {
        scaled = (int32_t)(value * 100.0f - 0.5f);
    }

    if (scaled < 0)
    {
        negative = 1u;
        scaled = -scaled;
    }

    whole = scaled / 100;
    frac  = scaled % 100;

    (void)snprintf(buf, size, "%s%d.%02d",
                   (negative != 0u) ? "-" : "",
                   (int)whole, (int)frac);
}

void AppTaskCreate(void)
{
    INT8U err;

    err = OSTaskCreateExt(Task1_LED1,
                          (void *)0,
                          &Task1Stk[APP_TASK_STK_SIZE - 1u],
                          APP_TASK_LED1_PRIO,
                          APP_TASK_LED1_PRIO,
                          &Task1Stk[0],
                          APP_TASK_STK_SIZE,
                          (void *)0,
                          OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);
    if (err != OS_ERR_NONE)
    {
        Error_Handler();
    }

    err = OSTaskCreateExt(Task2_LED2,
                          (void *)0,
                          &Task2Stk[APP_TASK_STK_SIZE - 1u],
                          APP_TASK_LED2_PRIO,
                          APP_TASK_LED2_PRIO,
                          &Task2Stk[0],
                          APP_TASK_STK_SIZE,
                          (void *)0,
                          OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);
    if (err != OS_ERR_NONE)
    {
        Error_Handler();
    }

    err = OSTaskCreateExt(Task3_AHT20,
                          (void *)0,
                          &Task3Stk[APP_TASK_STK_SIZE - 1u],
                          APP_TASK_UART_PRIO,
                          APP_TASK_UART_PRIO,
                          &Task3Stk[0],
                          APP_TASK_STK_SIZE,
                          (void *)0,
                          OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);
    if (err != OS_ERR_NONE)
    {
        Error_Handler();
    }

    /* Names are optional; they make the task list easier to read in a debugger. */
    OSTaskNameSet(APP_TASK_LED1_PRIO, (INT8U *)"LED1_1s", &err);
    OSTaskNameSet(APP_TASK_LED2_PRIO, (INT8U *)"LED2_3s", &err);
    OSTaskNameSet(APP_TASK_UART_PRIO, (INT8U *)"AHT20_2s", &err);
}

/**
  * @brief  Task 1: 1 s complete LED1 on/off cycle (500 ms ON, 500 ms OFF).
  */
static void Task1_LED1(void *p_arg)
{
    (void)p_arg;

    for (;;)
    {
        BSP_LED_On(LED1);
        (void)OSTimeDlyHMSM(0u, 0u, LED1_HALF_PERIOD_SEC, LED1_HALF_PERIOD_MS);

        BSP_LED_Off(LED1);
        (void)OSTimeDlyHMSM(0u, 0u, LED1_HALF_PERIOD_SEC, LED1_HALF_PERIOD_MS);
    }
}

/**
  * @brief  Task 2: 3 s complete LED2 on/off cycle (1500 ms ON, 1500 ms OFF).
  */
static void Task2_LED2(void *p_arg)
{
    (void)p_arg;

    for (;;)
    {
        BSP_LED_On(LED2);
        (void)OSTimeDlyHMSM(0u, 0u, LED2_HALF_PERIOD_SEC, LED2_HALF_PERIOD_MS);

        BSP_LED_Off(LED2);
        (void)OSTimeDlyHMSM(0u, 0u, LED2_HALF_PERIOD_SEC, LED2_HALF_PERIOD_MS);
    }
}

/**
  * @brief  Task 3: trigger an AHT20 conversion every 2 s and print the result.
  */
static void Task3_AHT20(void *p_arg)
{
    AHT20_Data_t data;
    char temp_str[16];
    char hum_str[16];
    char line[96];
    INT32U start;
    INT32U elapsed;
    uint8_t ok;

    (void)p_arg;

    for (;;)
    {
        start = OSTimeGet();

        /* Retry initialization if the sensor was absent at power-up. */
        if (g_aht20_ready == 0u)
        {
            (void)AHT20_Init();
        }

        ok = 0u;
        if (g_aht20_ready == 0u)
        {
            USART1_SendString("AHT20 init failed: check PB6/PB7 and 0x38\r\n");
        }
        else if (AHT20_StartMeasurement() == 0u)
        {
            USART1_SendString("AHT20 trigger NACK at 0x70\r\n");
            g_aht20_ready = 0u;
        }
        else
        {
            /* Conversion time: at least 80 ms.  Use an OS delay so lower
             * priority tasks can run while AHT20 is measuring. */
            (void)OSTimeDlyHMSM(0u, 0u, 0u, AHT20_CONVERSION_MS);

            if (AHT20_ReadMeasurement(&data) != 0u)
            {
                ok = 1u;
            }
            else
            {
                /* One short retry in case the status busy bit was still set. */
                (void)OSTimeDlyHMSM(0u, 0u, 0u, AHT20_RETRY_MS);
                if (AHT20_ReadMeasurement(&data) != 0u)
                {
                    ok = 1u;
                }
            }

            if (ok != 0u)
            {
                FixedToStr(temp_str, sizeof(temp_str), data.temperature);
                FixedToStr(hum_str,  sizeof(hum_str),  data.humidity);

                (void)snprintf(line, sizeof(line),
                               "T=%s C, RH=%s %%RH  [status=0x%02X CRC=%s]\r\n",
                               temp_str, hum_str, data.status,
                               (data.crc_ok != 0u) ? "OK" : "ERR");
                USART1_SendString(line);
            }
            else
            {
                USART1_SendString("AHT20 read failed (busy/NACK)\r\n");
            }
        }

        /* Keep a strict 2 s period: subtract the time already spent. */
        elapsed = OSTimeGet() - start;
        if (elapsed < AHT20_PERIOD_MS)
        {
            (void)OSTimeDly((INT16U)(AHT20_PERIOD_MS - elapsed));
        }
    }
}
