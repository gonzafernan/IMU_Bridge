/**
  ******************************************************************************
  * @file           : port_uart.h
  * @author         : Gonzalo Gabriel Fernandez
  * @brief          : IMU Bridge UART port header
  ******************************************************************************
  * @attention
  *
  * IMU Bridge UART port header, hardware independant.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __PORT_UART_H
#define __PORT_UART_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "imu_bridge.h"
#include "utils.h"

#include <stdint.h>

/* Defines -------------------------------------------------------------------*/
#define UART_BUFFER_SIZE    3

/* Exported functions --------------------------------------------------------*/
IMU_Bridge_StatusTypeDef UART_Init(void);
IMU_Bridge_StatusTypeDef UART_Transmit(uint8_t* pData, uint16_t size);
void UART_ReadRxBuffer(uint8_t* pData);
tick_t Sys_GetTick(void);

#ifdef __cplusplus
}
#endif

#endif /* __PORT_UART_H */