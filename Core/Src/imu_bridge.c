/**
  ******************************************************************************
  * @file           : imu_bridge.c
  * @author         : Gonzalo Gabriel Fernandez
  * @brief          : IMU Bridge software module
  ******************************************************************************
  * @attention
  *
  * IMU Bridge software module
  *
  ******************************************************************************
  */

#include "imu_bridge.h"
#include "port_uart.h"

#include <stdio.h>
#include <string.h>

uint8_t pCmdBuffer[4] = {'0', '0', '0', '\0'};

/**
 * @brief IMU Bridge init software module
*/
IMU_Bridge_StatusTypeDef IMU_Bridge_Init(void)
{
    IMU_Bridge_StatusTypeDef status;
    status = UART_Init();
    return status;
}

/**
 * @brief Send string to user
*/
IMU_Bridge_StatusTypeDef IMU_Bridge_SendString(char *pMsg)
{
    return UART_Transmit((uint8_t*)pMsg, strlen(pMsg));
}

/**
 * @brief Callback when a complete command is received
 * @note To be called by the UART port in a receptioon complete callback
*/
void IMU_Bridge_RxCallback(void)
{
    char msg[200]; 
    UART_ReadRxBuffer(pCmdBuffer);
    sprintf(msg, ">> CMD CALLBACK: %s\n\r", (char*)pCmdBuffer);
    IMU_Bridge_SendString(msg);
}

/**
 * @brief Get current command
*/
IMU_Bridge_CmdTypeDef IMU_Bridge_GetCmd(void)
{
    IMU_Bridge_CmdTypeDef cmd;
    if (strcmp((char*)pCmdBuffer, "STY") == 0) cmd = IMU_BRIDGE_CMD_SANITY;
    else if (strcmp((char*)pCmdBuffer, "INT") == 0) cmd = IMU_BRIDGE_CMD_INIT;
    else if (strcmp((char*)pCmdBuffer, "CFG") == 0) cmd = IMU_BRIDGE_CMD_CONFIG;
    else if (strcmp((char*)pCmdBuffer, "CG1") == 0) cmd = IMU_BRIDGE_CMD_CFG_GYRO_FS250;
    else if (strcmp((char*)pCmdBuffer, "CG2") == 0) cmd = IMU_BRIDGE_CMD_CFG_GYRO_FS500;
    else if (strcmp((char*)pCmdBuffer, "CG3") == 0) cmd = IMU_BRIDGE_CMD_CFG_GYRO_FS1000;
    else if (strcmp((char*)pCmdBuffer, "CG4") == 0) cmd = IMU_BRIDGE_CMD_CFG_GYRO_FS2000;
    else if (strcmp((char*)pCmdBuffer, "CA1") == 0) cmd = IMU_BRIDGE_CMD_CFG_ACCEL_FS2;
    else if (strcmp((char*)pCmdBuffer, "CA2") == 0) cmd = IMU_BRIDGE_CMD_CFG_ACCEL_FS4;
    else if (strcmp((char*)pCmdBuffer, "CA3") == 0) cmd = IMU_BRIDGE_CMD_CFG_ACCEL_FS8;
    else if (strcmp((char*)pCmdBuffer, "CA4") == 0) cmd = IMU_BRIDGE_CMD_CFG_ACCEL_FS16;
    else if (strcmp((char*)pCmdBuffer, "RDM") == 0) cmd = IMU_BRIDGE_CMD_READ_MODE;
    else if (strcmp((char*)pCmdBuffer, "AAW") == 0) cmd = IMU_BRIDGE_CMD_READ_ACCEL_ALL;
    else if (strcmp((char*)pCmdBuffer, "GAW") == 0) cmd = IMU_BRIDGE_CMD_READ_GYRO_ALL;
    else if (strcmp((char*)pCmdBuffer, "TMP") == 0) cmd = IMU_BRIDGE_CMD_READ_TEMP;
    else if (strcmp((char*)pCmdBuffer, "RTM") == 0) cmd = IMU_BRIDGE_CMD_REALTIME;
    else if (strcmp((char*)pCmdBuffer, "RTG") == 0) cmd = IMU_BRIDGE_CMD_REALTIME_GYRO;
    else if (strcmp((char*)pCmdBuffer, "RTA") == 0) cmd = IMU_BRIDGE_CMD_REALTIME_ACCEL;
    else if (strcmp((char*)pCmdBuffer, "RTT") == 0) cmd = IMU_BRIDGE_CMD_REALTIME_TEMP;
    else if (strcmp((char*)pCmdBuffer, "EXT") == 0) cmd = IMU_BRIDGE_CMD_EXIT;
    else cmd = IMU_BRIDGE_CMD_INVALID;
    pCmdBuffer[0] = '0';
    pCmdBuffer[1] = '0';
    pCmdBuffer[2] = '0';
    return cmd;
}
