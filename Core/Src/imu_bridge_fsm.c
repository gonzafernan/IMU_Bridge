/**
  ******************************************************************************
  * @file           : imu_bridge_fsm.c
  * @author         : Gonzalo Gabriel Fernandez
  * @brief          : IMU Bridge FSM
  ******************************************************************************
  * @attention
  *
  * IMU Bridge Finite State Machine
  *
  ******************************************************************************
  */

#include "imu_bridge_fsm.h"
#include "mpu9250.h"
#include "utils.h"

#include <stdio.h>
#include <string.h>

/**
 * @brief IMU Bridge FSM status enum
*/
typedef enum
{
    IMU_BRIDGE_FSM_INIT_STATE   = 0x00U,
    IMU_BRIDGE_FSM_DEINIT_STATE = 0x01U,
    IMU_BRIDGE_FSM_OP_STATE     = 0x02U,
    IMU_BRIDGE_FSM_ERROR_STATE  = 0x03U

} IMU_Bridge_FsmStateTypeDef;

/**
 * @brief IMU Bridge FSM inner Op state FSM
*/
typedef enum
{
    IMU_BRIDGE_FSM_OP_IDLE_STATE        = 0x00U,
    IMU_BRIDGE_FSM_OP_SANITY_STATE      = 0x01U,
    IMU_BRIDGE_FSM_OP_CONFIG_STATE      = 0x02U,
    IMU_BRIDGE_FSM_OP_READ_STATE        = 0x03U,
    IMU_BRIDGE_FSM_OP_REALTIME_STATE    = 0x04U,

} IMU_Bridge_OpStateTypeDef;

/* Private function prototypes -----------------------------------------------*/
static IMU_Bridge_StatusTypeDef IMU_Bridge_InitState(void);
static void IMU_Bridge_OpState_Entry(void);
static IMU_Bridge_StatusTypeDef IMU_Bridge_OpState(void);
static IMU_Bridge_StatusTypeDef IMU_Bridge_IdleState(void);
static void IMU_Bridge_IdleState_Entry(void);
static IMU_Bridge_StatusTypeDef IMU_Bridge_SanityState(void);
static void IMU_Bridge_ConfigState_Entry(void);
static IMU_Bridge_StatusTypeDef IMU_Bridge_ConfigState(void);
static void IMU_Bridge_ReadState_Entry(void);
static IMU_Bridge_StatusTypeDef IMU_Bridge_ReadState(void);
static void IMU_Bridge_RealTimeState_Entry(void);
static IMU_Bridge_StatusTypeDef IMU_Bridge_RealTimeState(void);
static bool checkExitEvent(IMU_Bridge_CmdTypeDef cmd);
static void hline(void);

/* Private variables ---------------------------------------------------------*/
static IMU_Bridge_FsmStateTypeDef bridge_fsm_state; /*!< IMU Bridge FSM status              */
static IMU_Bridge_OpStateTypeDef bridge_op_state;   /*!< IMU Bridge FSM status              */
static delay_t realtime_delay;                      /*!< Real Time delay (sys tick timer)   */
static IMU_Bridge_RealTime_SelTypeDef realtime_sel; /*!< Real Time mode sensor selection    */

/**
 * @brief IMU Bridge FSM initialization
*/
void IMU_Bridge_FsmInit(void)
{
    IMU_Bridge_Init();
    bridge_fsm_state = IMU_BRIDGE_FSM_INIT_STATE;
}

/**
 * @brief IMU Bridge FSM update
*/
IMU_Bridge_StatusTypeDef IMU_Bridge_FsmUpdate(void)
{
    IMU_Bridge_StatusTypeDef status = IMU_BRIDGE_ERROR;

    switch (bridge_fsm_state)
    {
    case IMU_BRIDGE_FSM_INIT_STATE:
        status = IMU_Bridge_InitState();
        bridge_fsm_state = IMU_BRIDGE_FSM_OP_STATE; 
        bridge_op_state = IMU_BRIDGE_FSM_OP_IDLE_STATE;
        IMU_Bridge_OpState_Entry();
        IMU_Bridge_IdleState_Entry();
        break;
    case IMU_BRIDGE_FSM_DEINIT_STATE:
        /* code */
        break;
    case IMU_BRIDGE_FSM_OP_STATE:
        status = IMU_Bridge_OpState();
        break;
    case IMU_BRIDGE_FSM_ERROR_STATE:
        /* code */
        break;
    default:
        status = IMU_BRIDGE_ERROR;
        break;
    }

    return status;
}

/**
 * @brief IMU Bridge Init state
*/
static IMU_Bridge_StatusTypeDef IMU_Bridge_InitState(void)
{
    char* msg = "INIT STATE\n\r";
    IMU_Bridge_SendString(msg);
    if (MPU9250_Init() != MPU9250_OK) return IMU_BRIDGE_ERROR;
    return IMU_BRIDGE_OK;
}

/**
 * @brief IMU Bridge Operational state entry
*/
static void IMU_Bridge_OpState_Entry(void)
{
    char* msg = "OP STATE\n\r";
    IMU_Bridge_SendString(msg);
}

/**
 * @brief IMU Bridge Operational state
*/
static IMU_Bridge_StatusTypeDef IMU_Bridge_OpState(void)
{
    IMU_Bridge_StatusTypeDef status = IMU_BRIDGE_ERROR;

    switch (bridge_op_state)
    {
    case IMU_BRIDGE_FSM_OP_SANITY_STATE:
        status = IMU_Bridge_SanityState();
        break;

    case IMU_BRIDGE_FSM_OP_READ_STATE:
        status = IMU_Bridge_ReadState();
        break;

    case IMU_BRIDGE_FSM_OP_IDLE_STATE:
        status = IMU_Bridge_IdleState();
        break;

    case IMU_BRIDGE_FSM_OP_CONFIG_STATE:
        status = IMU_Bridge_ConfigState();
        break;

    case IMU_BRIDGE_FSM_OP_REALTIME_STATE:
        status = IMU_Bridge_RealTimeState();
        break;
    
    default:
        status = IMU_BRIDGE_ERROR;
        break;
    }

    return status;

}

/**
 * @brief IMU Bridge Idle State entry
*/
static void IMU_Bridge_IdleState_Entry(void)
{
    char* msg = "IDLE STATE\n\r";
    IMU_Bridge_SendString(msg);
}

/**
 * @brief IMU Bridge Idle State
*/
static IMU_Bridge_StatusTypeDef IMU_Bridge_IdleState(void)
{
    switch (IMU_Bridge_GetCmd())
    {
    case IMU_BRIDGE_CMD_SANITY:
        bridge_op_state = IMU_BRIDGE_FSM_OP_SANITY_STATE;
        break;

    case IMU_BRIDGE_CMD_CONFIG:
        bridge_op_state = IMU_BRIDGE_FSM_OP_CONFIG_STATE;
        IMU_Bridge_ConfigState_Entry();
        break;
    
    case IMU_BRIDGE_CMD_READ_MODE:
        bridge_op_state = IMU_BRIDGE_FSM_OP_READ_STATE;
        IMU_Bridge_ReadState_Entry();
        break;
    
    case IMU_BRIDGE_CMD_REALTIME:
        bridge_op_state = IMU_BRIDGE_FSM_OP_REALTIME_STATE;
        IMU_Bridge_RealTimeState_Entry();
        break;

    default:
        break;
    }

    return IMU_BRIDGE_OK;
}

/**
 * @brief IMU Bridge Sanity State
*/
static IMU_Bridge_StatusTypeDef IMU_Bridge_SanityState(void)
{
    char* msg = "SANITY STATE\n\r";
    IMU_Bridge_SendString(msg);
    if (MPU9250_SanityCheck() != MPU9250_OK){
        char* ans = "---> Sanity Check: ERROR\n\r";
        IMU_Bridge_SendString(ans);
        return IMU_BRIDGE_ERROR;
    } 
    char* ans = "---> Sanity Check: OK\n\r";
    IMU_Bridge_SendString(ans);
    bridge_op_state = IMU_BRIDGE_FSM_OP_IDLE_STATE;   
    return IMU_BRIDGE_OK;
}

/**
 * @brief IMU Bridge Configuration State entry
*/
static void IMU_Bridge_ConfigState_Entry(void)
{
    char msg[100];
    uint8_t gyroConfig;
    uint8_t accelConfig;

    strcpy(msg, "CONFIG STATE\n\r");
    IMU_Bridge_SendString(msg);

    MPU9250_GyroReadConfig(&gyroConfig);
    MPU9250_AccelReadConfig(&accelConfig);
    sprintf(msg, "GYRO CONFIG WORD: 0x%X\n\rACCEL CONFIG WORD: 0x%x/n/r", gyroConfig, accelConfig);
    IMU_Bridge_SendString(msg);
}

/**
 * @brief IMU Bridge Configuration State
*/
static IMU_Bridge_StatusTypeDef IMU_Bridge_ConfigState(void)
{
    char msg[100];

    IMU_Bridge_CmdTypeDef next_cmd = IMU_Bridge_GetCmd();

    if (next_cmd == IMU_BRIDGE_CMD_CFG_GYRO_FS250)
    {
        MPU9250_GyroSetFullScale(MPU9250_GYRO_CONFIG_250DPS);
        strcpy(msg, "Setting Gyro Full Scale to 250 dps\n\r");
        IMU_Bridge_SendString(msg);
    }
    if (next_cmd == IMU_BRIDGE_CMD_CFG_GYRO_FS500)
    {
        MPU9250_GyroSetFullScale(MPU9250_GYRO_CONFIG_500DPS);
        strcpy(msg, "Setting Gyro Full Scale to 500 dps\n\r");
        IMU_Bridge_SendString(msg);
    }

    if (checkExitEvent(next_cmd)) bridge_op_state = IMU_BRIDGE_FSM_OP_IDLE_STATE;
    
    return IMU_BRIDGE_OK;
}

/**
 * @brief IMU Bridge Read State Entry
*/
static void IMU_Bridge_ReadState_Entry(void)
{
    char msg[200];
    strcpy(msg, "READ STATE\n\r");
    IMU_Bridge_SendString(msg);
}

/**
 * @brief IMU Bridge Read State
*/
static IMU_Bridge_StatusTypeDef IMU_Bridge_ReadState(void)
{
    char msg[200];
    uint16_t AxisX, AxisY, AxisZ, Temp;
    IMU_Bridge_CmdTypeDef next_cmd = IMU_Bridge_GetCmd();

    switch (next_cmd)
    {
    case IMU_BRIDGE_CMD_READ_ACCEL_ALL:
        hline();
        MPU9250_AccelReadRaw(&AxisX, &AxisY, &AxisZ);
        sprintf(msg, "ACCEL READ:\t%d\t%d\t%d\n\r", AxisX, AxisY, AxisZ);
        IMU_Bridge_SendString(msg);
        hline();
        break;
    
    case IMU_BRIDGE_CMD_READ_GYRO_ALL:
        hline();
        MPU9250_GyroReadRaw(&AxisX, &AxisY, &AxisZ);
        sprintf(msg, "GYRO READ:\t%d\t%d\t%d\n\r", AxisX, AxisY, AxisZ);
        IMU_Bridge_SendString(msg);
        hline();
        break;
    
    case IMU_BRIDGE_CMD_READ_TEMP:
        hline();
        MPU9250_TempReadRaw(&Temp);
        sprintf(msg, "TEMP READ:\t%d\n\r", Temp);
        IMU_Bridge_SendString(msg);
        hline();
        break;
    
    default:
        break;
    }

    if (checkExitEvent(next_cmd)) bridge_op_state = IMU_BRIDGE_FSM_OP_IDLE_STATE;
    
    return IMU_BRIDGE_OK;
}

/**
 * @brief Entry event to Real Time state
*/
static void IMU_Bridge_RealTimeState_Entry(void)
{
    char msg[200];
    realtime_sel = IMU_BRIDGE_REALTIME_ACCEL;
    hline();
    strcpy(msg, "REAL TIME STATE\n\r");
    IMU_Bridge_SendString(msg);
    delay_init(&realtime_delay, IMU_BRIDGE_REALTIME_PERIOD);  // Sys-tick based delay. It establishes the sampling rate
}

/**
 * @brief IMU Bridge Real Time state
*/
static IMU_Bridge_StatusTypeDef IMU_Bridge_RealTimeState(void)
{
    char msg[200];
    IMU_Bridge_CmdTypeDef next_cmd = IMU_Bridge_GetCmd();
    uint16_t AxisX, AxisY, AxisZ, Temp;

    switch (next_cmd)
    {
    case IMU_BRIDGE_CMD_REALTIME_GYRO:
        realtime_sel = IMU_BRIDGE_REALTIME_GYRO;
        break;

    case IMU_BRIDGE_CMD_REALTIME_ACCEL:
        realtime_sel = IMU_BRIDGE_REALTIME_ACCEL;
        break;

    case IMU_BRIDGE_CMD_REALTIME_TEMP:
        realtime_sel = IMU_BRIDGE_REALTIME_TEMP;
        break;
    
    default:
        break;
    }

    if (delay_read(&realtime_delay))
    {
        if (realtime_sel == IMU_BRIDGE_REALTIME_GYRO) MPU9250_GyroFetch();
        if (realtime_sel == IMU_BRIDGE_REALTIME_ACCEL) MPU9250_AccelFetch();
        if (realtime_sel == IMU_BRIDGE_REALTIME_TEMP) MPU9250_TempFetch();
    }

    if (MPU9250_IsDataReady())
    {
        if (realtime_sel == IMU_BRIDGE_REALTIME_GYRO){
            MPU9250_GyroReadFromBuffer(&AxisX, &AxisY, &AxisZ);
            sprintf(msg, "GYRO READ:\t%d\t%d\t%d\n\r", AxisX, AxisY, AxisZ);
            IMU_Bridge_SendString(msg);
        } 
        if (realtime_sel == IMU_BRIDGE_REALTIME_ACCEL){
            MPU9250_AccelReadFromBuffer(&AxisX, &AxisY, &AxisZ);
            sprintf(msg, "ACCEL READ:\t%d\t%d\t%d\n\r", AxisX, AxisY, AxisZ);
            IMU_Bridge_SendString(msg);
        } 
        if (realtime_sel == IMU_BRIDGE_REALTIME_TEMP){
            MPU9250_TempReadFromBuffer(&Temp);
            sprintf(msg, "TEMP READ:\t%d\n\r", Temp);
            IMU_Bridge_SendString(msg);
        } 
    }

    if (checkExitEvent(next_cmd)) bridge_op_state = IMU_BRIDGE_FSM_OP_IDLE_STATE;
    
    return IMU_BRIDGE_OK;
}

/**
 * @brief   Check for exit command event
 * @param   cmd: Command received
 * @retval  bool
*/
static bool checkExitEvent(IMU_Bridge_CmdTypeDef cmd)
{
    char msg[200];
    if (cmd == IMU_BRIDGE_CMD_EXIT)
    {
        hline();
        strcpy(msg, "EXIT\n\r");
        IMU_Bridge_SendString(msg);
        return true;
    }
    return false;
}

/**
 * @brief   Print horizontal line
*/
static void hline(void)
{
    char* msg = "======================================\n\r";
    IMU_Bridge_SendString(msg);
}