/**
  ******************************************************************************
  * @file           : port_uart.c
  * @author         : Gonzalo Gabriel Fernandez
  * @brief          : IMU Bridge UART port for STM32F1XX
  ******************************************************************************
  * @attention
  *
  * IMU Bridge UART port for STM32F1XX
  * Tested on STM32F103C8T6.
  *
  ******************************************************************************
  */

#include "port_uart.h"
#include "stm32f1xx_hal.h"

#include <assert.h>
#include <string.h>

UART_HandleTypeDef huart1;
static uint8_t pRxBuffer[UART_BUFFER_SIZE];

/**
 * @brief UART init function
*/
IMU_Bridge_StatusTypeDef UART_Init(void)
{
    huart1.Instance = USART1;
    huart1.Init.BaudRate = 115200;
    huart1.Init.WordLength = UART_WORDLENGTH_8B;
    huart1.Init.StopBits = UART_STOPBITS_1;
    huart1.Init.Parity = UART_PARITY_NONE;
    huart1.Init.Mode = UART_MODE_TX_RX;
    huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart1.Init.OverSampling = UART_OVERSAMPLING_16;

    if (HAL_UART_Init(&huart1) != HAL_OK) return IMU_BRIDGE_ERROR;

    HAL_UART_Receive_IT(&huart1, pRxBuffer, UART_BUFFER_SIZE);

    return IMU_BRIDGE_OK;
}

/**
 * @brief UART transmit data
*/
IMU_Bridge_StatusTypeDef UART_Transmit(uint8_t* pData, uint16_t size)
{
    if (HAL_UART_Transmit(&huart1, pData, size, 100) != HAL_OK) return IMU_BRIDGE_ERROR;
    return IMU_BRIDGE_OK;
}

/**
 * @brief Read UART RX buffer
*/
void UART_ReadRxBuffer(uint8_t* pData)
{
    assert(pData);
    pData[0] = pRxBuffer[0];
    pData[1] = pRxBuffer[1];
    pData[2] = pRxBuffer[2];
}

void HAL_UART_MspInit(UART_HandleTypeDef* uartHandle)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    if(uartHandle->Instance==USART1)
    {
        /* USART1 clock enable */
        __HAL_RCC_USART1_CLK_ENABLE();

        __HAL_RCC_GPIOA_CLK_ENABLE();
        /**USART1 GPIO Configuration
        PA9     ------> USART1_TX
        PA10    ------> USART1_RX
        */
        GPIO_InitStruct.Pin = GPIO_PIN_9;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    
        GPIO_InitStruct.Pin = GPIO_PIN_10;
        GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    
        /* USART1 interrupt Init */
        HAL_NVIC_SetPriority(USART1_IRQn, 0, 0);
        HAL_NVIC_EnableIRQ(USART1_IRQn);
    }
}

void HAL_UART_MspDeInit(UART_HandleTypeDef* uartHandle)
{
    if(uartHandle->Instance==USART1)
    {
        /* Peripheral clock disable */
        __HAL_RCC_USART1_CLK_DISABLE();

        /**USART1 GPIO Configuration
        PA9     ------> USART1_TX
        PA10    ------> USART1_RX
        */
        HAL_GPIO_DeInit(GPIOA, GPIO_PIN_9|GPIO_PIN_10);

        /* USART1 interrupt Deinit */
        HAL_NVIC_DisableIRQ(USART1_IRQn);
    }
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    IMU_Bridge_RxCallback();
    HAL_UART_Receive_IT(&huart1, pRxBuffer, UART_BUFFER_SIZE);
}


/**
  * @brief  Get systick.
  * @retval tick_t
  */
tick_t Sys_GetTick(void)
{
    return (tick_t)HAL_GetTick();
}