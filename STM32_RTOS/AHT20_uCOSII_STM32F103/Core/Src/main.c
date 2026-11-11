/**
  ******************************************************************************
  * @file    main.c
  * @brief   STM32F103C8T6 + uC/OS-II V2.92 demo.
  *
  *  Clock    : HSE 8 MHz -> PLL x9 -> 72 MHz
  *  OS tick  : SysTick 1 ms (shared with HAL)
  *  LED1     : PB0, active HIGH, 1 s complete on/off cycle
  *  LED2     : PB1, active HIGH, 3 s complete on/off cycle
  *  USART1   : PA9 = TX, PA10 = RX, 115200 8N1, AHT20 result every 2 s
  *  AHT20    : software I2C, PB6 = SCL, PB7 = SDA, 7-bit address 0x38
  ******************************************************************************
  */

#include "main.h"
#include "gpio.h"
#include "usart.h"
#include "bsp_led.h"
#include "i2c_soft.h"
#include "aht20.h"
#include "app_tasks.h"
#include "ucos_ii.h"

static void SystemClock_Config(void);

int main(void)
{
    /* 1. HAL init: SysTick 1 ms, NVIC priority grouping 4, HAL_MspInit(). */
    HAL_Init();

    /* 2. HSE 8 MHz -> 72 MHz. */
    SystemClock_Config();

    /* 3. CubeMX-style peripheral initialization. */
    MX_GPIO_Init();
    MX_USART1_UART_Init();
    BSP_LED_Init();
    I2C_Init();                    /* PB6=SCL, PB7=SDA, software I2C */

    /* 4. Both PendSV and SysTick must be at the lowest priority (15).
     *    uC/OS-II defers context switches to PendSV. */
    HAL_NVIC_SetPriority(PendSV_IRQn, 15u, 0u);
    HAL_NVIC_SetPriority(SysTick_IRQn, 15u, 0u);

    USART1_SendString("\r\n");
    USART1_SendString("===== uC/OS-II V2.92 + AHT20 on STM32F103C8T6 =====\r\n");
    USART1_SendString("Clock: 72 MHz, Tick: 1 ms, UART: 115200 8N1\r\n");
    USART1_SendString("LED1=PB0 (1 s), LED2=PB1 (3 s), AHT20 task=2 s\r\n");
    USART1_SendString("AHT20: SCL=PB6, SDA=PB7, 7-bit addr=0x38\r\n");

    /* AHT20 needs >=40 ms after power-up.  AHT20_Init() also checks the
     * calibration bit and sends 0xBE 0x08 0x00 when needed. */
    if (AHT20_Init() != 0u)
    {
        USART1_SendString("AHT20 init: OK (addr 0x38, init cmd BE 08 00)\r\n\r\n");
    }
    else
    {
        USART1_SendString("AHT20 init: FAIL (check wiring/pull-ups)\r\n\r\n");
    }

#if (I2C_SIMULATE_ACK != 0)
    /* Simulation-only: generate a complete init + measure + read waveform for
     * the Keil Logic Analyzer, even though no AHT20 model exists in the
     * simulator.  Set I2C_SIMULATE_ACK back to 0 for real hardware. */
    AHT20_LogicAnalyzerDemo();
#endif

    /* 5. Stop the HAL tick while the kernel is being initialized.  The tick is
     *    resumed with interrupts disabled immediately before OSStart(). */
    HAL_SuspendTick();

    /* 6. Initialize uC/OS-II and create the three application tasks.
     *    OSInit() calls OSInitHookBegin(), which prepares the dedicated MSP stack. */
    OSInit();
    AppTaskCreate();

    /* 7. Enable the 1 ms tick with global interrupts still disabled, then start
     *    multitasking.  OSStartHighRdy() will enable interrupts after the first
     *    PendSV context switch has been set up. */
    __disable_irq();
    HAL_ResumeTick();
    SysTick->VAL = 0u;              /* Start a fresh 1 ms tick period. */
    OSStart();

    /* OSStart() must never return. */
    Error_Handler();
    while (1)
    {
    }
}

/**
  * @brief  System clock: HSE 8 MHz, PLL x9 = 72 MHz.
  *         AHB = 72 MHz, APB1 = 36 MHz, APB2 = 72 MHz.
  */
static void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    __HAL_RCC_AFIO_CLK_ENABLE();
    __HAL_RCC_PWR_CLK_ENABLE();

    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState       = RCC_HSE_ON;
    RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
    RCC_OscInitStruct.HSIState       = RCC_HSI_ON;
    RCC_OscInitStruct.PLL.PLLState   = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource  = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLMUL     = RCC_PLL_MUL9;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        Error_Handler();
    }

    RCC_ClkInitStruct.ClockType      = RCC_CLOCKTYPE_HCLK   | RCC_CLOCKTYPE_SYSCLK |
                                       RCC_CLOCKTYPE_PCLK1  | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource   = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider  = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
    {
        Error_Handler();
    }
}

/**
  * @brief  Fatal error: disable interrupts and stop for the debugger.
  */
void Error_Handler(void)
{
    __disable_irq();
    while (1)
    {
    }
}

#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t *file, uint32_t line)
{
    (void)file;
    (void)line;
    Error_Handler();
}
#endif
