/**
  ******************************************************************************
  * @file    aht20.h
  * @brief   AHT20 temperature/humidity sensor driver (I2C, 7-bit address 0x38).
  *
  * AHT20 command summary:
  *   Initialization : 0xBE 0x08 0x00, wait >= 10 ms
  *   Trigger measure: 0xAC 0x33 0x00, wait >= 80 ms
  *   Read           : 7 bytes from 0x71
  *   Soft reset     : 0xBA
  *
  * Write address = 0x70, read address = 0x71 (8-bit forms of 0x38).
  ******************************************************************************
  */

#ifndef __AHT20_H
#define __AHT20_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

#define AHT20_I2C_ADDR_7BIT      0x38u
#define AHT20_I2C_ADDR_WRITE     0x70u
#define AHT20_I2C_ADDR_READ      0x71u

#define AHT20_CMD_INITIALIZE     0xBEu
#define AHT20_CMD_INIT_PARAM1    0x08u
#define AHT20_CMD_INIT_PARAM2    0x00u

#define AHT20_CMD_TRIGGER        0xACu
#define AHT20_CMD_TRIGGER_PARAM1 0x33u
#define AHT20_CMD_TRIGGER_PARAM2 0x00u

#define AHT20_CMD_SOFT_RESET     0xBAu

#define AHT20_STATUS_BUSY        0x80u
#define AHT20_STATUS_CALIBRATED  0x08u

typedef struct
{
    float   temperature;   /* degrees Celsius */
    float   humidity;      /* %RH */
    uint8_t status;        /* status byte read together with the data */
    uint8_t crc_ok;        /* 1 = CRC matches, 0 = CRC error */
} AHT20_Data_t;

/* Set to 1 after a successful AHT20_Init(); cleared by the application on error. */
extern volatile uint8_t g_aht20_ready;

uint8_t AHT20_Init(void);
uint8_t AHT20_ReadStatus(uint8_t *status);
uint8_t AHT20_StartMeasurement(void);
uint8_t AHT20_ReadMeasurement(AHT20_Data_t *data);
uint8_t AHT20_SoftReset(void);
uint8_t AHT20_CRC8(const uint8_t *data, uint8_t len);

/* Simulation-only transaction for the Keil Logic Analyzer.  It sends the
 * initialization command, the measurement command and a 7-byte read without
 * requiring a real AHT20.  Use together with I2C_SIMULATE_ACK = 1. */
void AHT20_LogicAnalyzerDemo(void);

#ifdef __cplusplus
}
#endif

#endif /* __AHT20_H */
