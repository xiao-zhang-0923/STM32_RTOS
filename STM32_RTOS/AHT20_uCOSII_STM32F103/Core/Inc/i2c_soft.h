/**
  ******************************************************************************
  * @file    i2c_soft.h
  * @brief   Bit-banged I2C master on PB6 (SCL) / PB7 (SDA).
  *
  * The two global variables g_i2c_scl and g_i2c_sda are updated on every pin
  * change.  They are intended for the Keil Logic Analyzer: add both variables
  * as "Bit" signals to see the I2C waveform in the simulator.
  ******************************************************************************
  */

#ifndef __I2C_SOFT_H
#define __I2C_SOFT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

#define I2C_SCL_PORT        GPIOB
#define I2C_SCL_PIN         GPIO_PIN_6

#define I2C_SDA_PORT        GPIOB
#define I2C_SDA_PIN         GPIO_PIN_7

/*
 * Simulation-only switch.
 *   0 = normal hardware behavior: a missing ACK aborts the transfer.
 *   1 = pretend every byte is ACKed.  Use this ONLY with the Keil simulator
 *       plus AHT20_LogicAnalyzerDemo() to capture the complete command bytes;
 *       set it back to 0 before downloading to real hardware.
 */
#ifndef I2C_SIMULATE_ACK
#define I2C_SIMULATE_ACK    0 
#endif

/* Logic Analyzer observation points: 0 = low, 1 = released/high. */
extern volatile uint8_t g_i2c_scl;
extern volatile uint8_t g_i2c_sda;

void    I2C_Init(void);
uint8_t I2C_Probe(uint8_t addr);
uint8_t I2C_Write(uint8_t addr, const uint8_t *data, uint16_t len);
uint8_t I2C_Read(uint8_t addr, uint8_t *data, uint16_t len);

#ifdef __cplusplus
}
#endif

#endif /* __I2C_SOFT_H */
