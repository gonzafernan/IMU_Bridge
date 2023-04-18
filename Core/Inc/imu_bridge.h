/**
  ******************************************************************************
  * @file           : imu_bridge.h
  * @author         : Gonzalo Gabriel Fernandez
  * @brief          : IMU Bridge software module header
  ******************************************************************************
  * @attention
  *
  * IMU Bridge software module
  *
  ******************************************************************************
  */

#ifndef __MPU9250_BRIDGE_H
#define __MPU9250_BRIDGE_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief IMU Bridge status
*/
typedef enum
{
    IMU_BRIDGE_OK       = 0x00U,
    IMU_BRIDGE_ERROR    = 0x01U

} IMU_Bridge_StatusTypeDef;

/**
 * @brief IMU Bridge commands
*/
typedef enum
{
    IMU_BRIDGE_CMD_SANITY           = 0x00U,
    IMU_BRIDGE_CMD_CONFIG           = 0x01U,
    IMU_BRIDGE_CMD_READ_MODE        = 0x02U,
    IMU_BRIDGE_CMD_READ_ACCEL_ALL   = 0x03U,
    IMU_BRIDGE_CMD_READ_TEMP        = 0x04U,
    IMU_BRIDGE_CMD_EXIT             = 0x05U,
    IMU_BRIDGE_CMD_INVALID          = 0xFFU,

} IMU_Bridge_CmdTypeDef;

IMU_Bridge_StatusTypeDef IMU_Bridge_Init(void);
IMU_Bridge_StatusTypeDef IMU_Bridge_SendString(char *pMsg);
IMU_Bridge_CmdTypeDef IMU_Bridge_GetCmd(void);


void IMU_Bridge_RxCallback(void);

#ifdef __cplusplus
}
#endif

#endif /* __MPU9250_BRIDGE_H */