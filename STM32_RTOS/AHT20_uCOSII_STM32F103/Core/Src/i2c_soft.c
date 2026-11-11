/**
  ******************************************************************************
  * @file    i2c_soft.c
  * @brief   Software I2C master for AHT20 on PB6/PB7.
  *
  * Timing: about 100 kHz (5 us low + 5 us high per bit).  The bus is open-drain;
  * the AHT20 module normally provides 4.7 kOhm pull-ups to 3.3 V.  Internal
  * pull-ups are enabled as a weak fallback, but external pull-ups are preferred.
  *
  * Keil Logic Analyzer:
  *   1. Select "Use Simulator" in Options for Target -> Debug.
  *   2. View -> Analysis Windows -> Logic Analyzer -> Setup.
  *   3. Add g_i2c_scl and g_i2c_sda, display type "Bit".
  ******************************************************************************
  */

#include "i2c_soft.h"

volatile uint8_t g_i2c_scl = 1u;
volatile uint8_t g_i2c_sda = 1u;

/*
 * DWT cycle-counter delay.  At 72 MHz, 1 us = 72 cycles.
 * DWT must be enabled once in I2C_Init().
 */
static void I2C_DelayUs(uint32_t us)
{
    uint32_t start;
    uint32_t cycles;

    if (us == 0u)
    {
        return;
    }

    start  = DWT->CYCCNT;
    cycles = us * (SystemCoreClock / 1000000u);

    while ((DWT->CYCCNT - start) < cycles)
    {
        /* busy wait, about 100 kHz SCL */
    }
}

static void I2C_SetSCL(uint8_t level)
{
    HAL_GPIO_WritePin(I2C_SCL_PORT, I2C_SCL_PIN,
                      (level != 0u) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    g_i2c_scl = (level != 0u) ? 1u : 0u;
}

static void I2C_SetSDA(uint8_t level)
{
    HAL_GPIO_WritePin(I2C_SDA_PORT, I2C_SDA_PIN,
                      (level != 0u) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    g_i2c_sda = (level != 0u) ? 1u : 0u;
}

static uint8_t I2C_ReadSDA(void)
{
    return (HAL_GPIO_ReadPin(I2C_SDA_PORT, I2C_SDA_PIN) == GPIO_PIN_SET) ? 1u : 0u;
}

/*
 * Wait until the slave releases SCL (clock stretching).  AHT20 normally does
 * not stretch, but the timeout prevents a stuck bus from hanging the task.
 */
static uint8_t I2C_WaitSCLHigh(void)
{
    uint32_t timeout = 1000u;

    while (HAL_GPIO_ReadPin(I2C_SCL_PORT, I2C_SCL_PIN) == GPIO_PIN_RESET)
    {
        if (--timeout == 0u)
        {
            return 0u;
        }
        I2C_DelayUs(1u);
    }
    return 1u;
}

static uint8_t I2C_Start(void)
{
    I2C_SetSDA(1u);
    I2C_SetSCL(1u);
    I2C_DelayUs(5u);
    if (I2C_WaitSCLHigh() == 0u)
    {
        return 0u;
    }

    /* START: SDA falls while SCL is high. */
    I2C_SetSDA(0u);
    I2C_DelayUs(5u);
    I2C_SetSCL(0u);
    I2C_DelayUs(5u);
    return 1u;
}

static void I2C_Stop(void)
{
    I2C_SetSCL(0u);
    I2C_SetSDA(0u);
    I2C_DelayUs(5u);

    /* STOP: SDA rises while SCL is high. */
    I2C_SetSCL(1u);
    (void)I2C_WaitSCLHigh();
    I2C_DelayUs(5u);
    I2C_SetSDA(1u);
    I2C_DelayUs(5u);
}

static uint8_t I2C_WriteByte(uint8_t byte)
{
    uint8_t bit;
    uint8_t ack;

    for (bit = 0u; bit < 8u; bit++)
    {
        I2C_SetSDA((byte & 0x80u) ? 1u : 0u);
        byte <<= 1;
        I2C_DelayUs(5u);

        I2C_SetSCL(1u);
        if (I2C_WaitSCLHigh() == 0u)
        {
            return 0u;
        }
        I2C_DelayUs(5u);
        I2C_SetSCL(0u);
        I2C_DelayUs(5u);
    }

    /* 9th clock: release SDA and sample ACK (low = ACK). */
    I2C_SetSDA(1u);
    I2C_DelayUs(5u);
    I2C_SetSCL(1u);
    if (I2C_WaitSCLHigh() == 0u)
    {
        return 0u;
    }
    I2C_DelayUs(5u);
    ack = (I2C_ReadSDA() == 0u) ? 1u : 0u;
    I2C_SetSCL(0u);
    I2C_DelayUs(5u);

#if (I2C_SIMULATE_ACK != 0)
    ack = 1u;   /* Simulation only: no AHT20 slave model in Keil. */
#endif

    return ack;
}

static uint8_t I2C_ReadByte(uint8_t ack)
{
    uint8_t byte = 0u;
    uint8_t bit;

    I2C_SetSDA(1u);
    for (bit = 0u; bit < 8u; bit++)
    {
        I2C_DelayUs(5u);
        I2C_SetSCL(1u);
        if (I2C_WaitSCLHigh() == 0u)
        {
            return 0u;
        }
        I2C_DelayUs(2u);
        byte = (uint8_t)((byte << 1) | I2C_ReadSDA());
        I2C_DelayUs(3u);
        I2C_SetSCL(0u);
        I2C_DelayUs(5u);
    }

    /* 9th clock: ACK (SDA low) or NACK (SDA released). */
    I2C_SetSDA((ack != 0u) ? 0u : 1u);
    I2C_DelayUs(5u);
    I2C_SetSCL(1u);
    (void)I2C_WaitSCLHigh();
    I2C_DelayUs(5u);
    I2C_SetSCL(0u);
    I2C_DelayUs(5u);
    I2C_SetSDA(1u);

    return byte;
}

void I2C_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    __HAL_RCC_GPIOB_CLK_ENABLE();

    /* Enable the DWT cycle counter used by I2C_DelayUs(). */
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CYCCNT = 0u;
    DWT->CTRL  |= DWT_CTRL_CYCCNTENA_Msk;

    /* Release both lines before changing the pin direction. */
    HAL_GPIO_WritePin(I2C_SCL_PORT, I2C_SCL_PIN, GPIO_PIN_SET);
    HAL_GPIO_WritePin(I2C_SDA_PORT, I2C_SDA_PIN, GPIO_PIN_SET);
    g_i2c_scl = 1u;
    g_i2c_sda = 1u;

    GPIO_InitStruct.Pin   = I2C_SCL_PIN | I2C_SDA_PIN;
    GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_OD;
    GPIO_InitStruct.Pull  = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    I2C_DelayUs(10u);
}

uint8_t I2C_Probe(uint8_t addr)
{
    uint8_t ack;

    if (I2C_Start() == 0u)
    {
        return 0u;
    }
    ack = I2C_WriteByte(addr);
    I2C_Stop();
    return ack;
}

uint8_t I2C_Write(uint8_t addr, const uint8_t *data, uint16_t len)
{
    uint16_t i;

    if ((data == 0) || (len == 0u))
    {
        return 0u;
    }
    if (I2C_Start() == 0u)
    {
        return 0u;
    }
    if (I2C_WriteByte(addr) == 0u)
    {
        I2C_Stop();
        return 0u;
    }
    for (i = 0u; i < len; i++)
    {
        if (I2C_WriteByte(data[i]) == 0u)
        {
            I2C_Stop();
            return 0u;
        }
    }
    I2C_Stop();
    return 1u;
}

uint8_t I2C_Read(uint8_t addr, uint8_t *data, uint16_t len)
{
    uint16_t i;

    if ((data == 0) || (len == 0u))
    {
        return 0u;
    }
    if (I2C_Start() == 0u)
    {
        return 0u;
    }
    if (I2C_WriteByte(addr) == 0u)
    {
        I2C_Stop();
        return 0u;
    }
    for (i = 0u; i < len; i++)
    {
        data[i] = I2C_ReadByte((i < (uint16_t)(len - 1u)) ? 1u : 0u);
    }
    I2C_Stop();
    return 1u;
}
