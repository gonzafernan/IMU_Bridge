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

#define IMU_BRIDGE_REALTIME_PERIOD  100

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
    IMU_BRIDGE_CMD_SANITY,
    IMU_BRIDGE_CMD_CONFIG,
    IMU_BRIDGE_CMD_CFG_GYRO_FS250,
    IMU_BRIDGE_CMD_CFG_GYRO_FS500,
    IMU_BRIDGE_CMD_CFG_GYRO_FS1000,
    IMU_BRIDGE_CMD_CFG_GYRO_FS2000,
    IMU_BRIDGE_CMD_CFG_ACCEL_FS2,
    IMU_BRIDGE_CMD_CFG_ACCEL_FS4,
    IMU_BRIDGE_CMD_CFG_ACCEL_FS8,
    IMU_BRIDGE_CMD_CFG_ACCEL_FS16,
    IMU_BRIDGE_CMD_READ_MODE,
    IMU_BRIDGE_CMD_READ_ACCEL_ALL,
    IMU_BRIDGE_CMD_READ_GYRO_ALL,
    IMU_BRIDGE_CMD_READ_TEMP,
    IMU_BRIDGE_CMD_REALTIME,
    IMU_BRIDGE_CMD_REALTIME_GYRO,
    IMU_BRIDGE_CMD_REALTIME_ACCEL,
    IMU_BRIDGE_CMD_REALTIME_TEMP,
    IMU_BRIDGE_CMD_EXIT,
    IMU_BRIDGE_CMD_INVALID

} IMU_Bridge_CmdTypeDef;

/**
 * @brief Real Time mode sensor selection
*/
typedef enum{
	IMU_BRIDGE_REALTIME_GYRO,
	IMU_BRIDGE_REALTIME_ACCEL,
	IMU_BRIDGE_REALTIME_TEMP

} IMU_Bridge_RealTime_SelTypeDef;

IMU_Bridge_StatusTypeDef IMU_Bridge_Init(void);
IMU_Bridge_StatusTypeDef IMU_Bridge_SendString(char *pMsg);
IMU_Bridge_CmdTypeDef IMU_Bridge_GetCmd(void);


void IMU_Bridge_RxCallback(void);

#ifdef __cplusplus
}
#endif

#endif /* __MPU9250_BRIDGE_H */