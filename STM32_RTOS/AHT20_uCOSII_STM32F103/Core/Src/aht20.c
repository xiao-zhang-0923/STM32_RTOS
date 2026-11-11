/**
  ******************************************************************************
  * @file    aht20.c
  * @brief   AHT20 driver: initialization, measurement trigger, 7-byte read,
  *          CRC-8 check and conversion to temperature/humidity.
  *
  * Datasheet conversion:
  *   RH  = raw_h / 2^20 * 100 %RH
  *   T   = raw_t / 2^20 * 200 - 50 degC
  *
  * Raw layout (data[0] is status):
  *   data[0] : status
  *   data[1] : RH[19:12]
  *   data[2] : RH[11:4]
  *   data[3] : RH[3:0] | T[19:16]
  *   data[4] : T[15:8]
  *   data[5] : T[7:0]
  *   data[6] : CRC-8 (poly 0x31, init 0xFF, over data[0..5])
  ******************************************************************************
  */

#include "aht20.h"
#include "i2c_soft.h"

volatile uint8_t g_aht20_ready = 0u;

static uint8_t AHT20_WriteCommand(uint8_t cmd, uint8_t p1, uint8_t p2)
{
    uint8_t buf[3];

    buf[0] = cmd;
    buf[1] = p1;
    buf[2] = p2;

    return I2C_Write(AHT20_I2C_ADDR_WRITE, buf, 3u);
}

uint8_t AHT20_CRC8(const uint8_t *data, uint8_t len)
{
    uint8_t crc = 0xFFu;
    uint8_t i;
    uint8_t bit;

    for (i = 0u; i < len; i++)
    {
        crc ^= data[i];
        for (bit = 0u; bit < 8u; bit++)
        {
            if ((crc & 0x80u) != 0u)
            {
                crc = (uint8_t)((crc << 1) ^ 0x31u);
            }
            else
            {
                crc = (uint8_t)(crc << 1);
            }
        }
    }
    return crc;
}

uint8_t AHT20_ReadStatus(uint8_t *status)
{
    if (status == 0)
    {
        return 0u;
    }
    return I2C_Read(AHT20_I2C_ADDR_READ, status, 1u);
}

uint8_t AHT20_Init(void)
{
    uint8_t status = 0u;

    /* Power-on stabilization: datasheet requires >= 40 ms before access. */
    HAL_Delay(40u);

    if (AHT20_ReadStatus(&status) == 0u)
    {
        g_aht20_ready = 0u;
        return 0u;
    }

    /* If the calibration bit is not set, send the initialization command. */
    if ((status & AHT20_STATUS_CALIBRATED) == 0u)
    {
        if (AHT20_WriteCommand(AHT20_CMD_INITIALIZE,
                               AHT20_CMD_INIT_PARAM1,
                               AHT20_CMD_INIT_PARAM2) == 0u)
        {
            g_aht20_ready = 0u;
            return 0u;
        }

        HAL_Delay(10u);

        if (AHT20_ReadStatus(&status) == 0u)
        {
            g_aht20_ready = 0u;
            return 0u;
        }
    }

    if ((status & AHT20_STATUS_CALIBRATED) != 0u)
    {
        g_aht20_ready = 1u;
        return 1u;
    }

    g_aht20_ready = 0u;
    return 0u;
}

uint8_t AHT20_StartMeasurement(void)
{
    return AHT20_WriteCommand(AHT20_CMD_TRIGGER,
                              AHT20_CMD_TRIGGER_PARAM1,
                              AHT20_CMD_TRIGGER_PARAM2);
}

uint8_t AHT20_ReadMeasurement(AHT20_Data_t *data)
{
    uint8_t  buf[7];
    uint32_t raw_h;
    uint32_t raw_t;

    if (data == 0)
    {
        return 0u;
    }

    if (I2C_Read(AHT20_I2C_ADDR_READ, buf, 7u) == 0u)
    {
        return 0u;
    }

    /* Status bit 7: busy.  Bit 3: calibrated. */
    if ((buf[0] & AHT20_STATUS_BUSY) != 0u)
    {
        return 0u;
    }

    raw_h = ((uint32_t)buf[1] << 12) |
            ((uint32_t)buf[2] << 4)  |
            ((uint32_t)buf[3] >> 4);

    raw_t = (((uint32_t)buf[3] & 0x0Fu) << 16) |
            ((uint32_t)buf[4] << 8) |
            ((uint32_t)buf[5]);

    data->humidity    = ((float)raw_h * 100.0f) / 1048576.0f;
    data->temperature = ((float)raw_t * 200.0f) / 1048576.0f - 50.0f;
    data->status      = buf[0];
    data->crc_ok      = (AHT20_CRC8(buf, 6u) == buf[6]) ? 1u : 0u;

    return 1u;
}

void AHT20_LogicAnalyzerDemo(void)
{
    uint8_t init_cmd[3] = {AHT20_CMD_INITIALIZE,
                           AHT20_CMD_INIT_PARAM1,
                           AHT20_CMD_INIT_PARAM2};
    uint8_t measure_cmd[3] = {AHT20_CMD_TRIGGER,
                              AHT20_CMD_TRIGGER_PARAM1,
                              AHT20_CMD_TRIGGER_PARAM2};
    uint8_t data[7];

    (void)I2C_Write(AHT20_I2C_ADDR_WRITE, init_cmd, 3u);
    HAL_Delay(10u);
    (void)I2C_Write(AHT20_I2C_ADDR_WRITE, measure_cmd, 3u);
    HAL_Delay(80u);
    (void)I2C_Read(AHT20_I2C_ADDR_READ, data, 7u);
}

uint8_t AHT20_SoftReset(void)
{
    uint8_t cmd = AHT20_CMD_SOFT_RESET;

    g_aht20_ready = 0u;
    if (I2C_Write(AHT20_I2C_ADDR_WRITE, &cmd, 1u) == 0u)
    {
        return 0u;
    }
    HAL_Delay(20u);
    return 1u;
}
