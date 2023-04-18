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
    IMU_BRIDGE_FSM_OP_IDLE_STATE    = 0x00U,
    IMU_BRIDGE_FSM_OP_SANITY_STATE  = 0x01U,
    IMU_BRIDGE_FSM_OP_CONFIG_STATE  = 0x02U,
    IMU_BRIDGE_FSM_OP_READ_STATE    = 0x03U,

} IMU_Bridge_OpStateTypeDef;

/* Private function prototypes -----------------------------------------------*/
static IMU_Bridge_StatusTypeDef IMU_Bridge_InitState(void);
static IMU_Bridge_StatusTypeDef IMU_Bridge_OpState(void);
static IMU_Bridge_StatusTypeDef IMU_Bridge_IdleState(void);
static IMU_Bridge_StatusTypeDef IMU_Bridge_SanityState(void);
static IMU_Bridge_StatusTypeDef IMU_Bridge_ConfigState(void);
static IMU_Bridge_StatusTypeDef IMU_Bridge_ReadState(void);

/* Private variables ---------------------------------------------------------*/
static IMU_Bridge_FsmStateTypeDef bridge_fsm_state; /*!< IMU Bridge FSM status  */
static IMU_Bridge_OpStateTypeDef bridge_op_state;   /*!< IMU Bridge FSM status  */

static uint8_t temp_buffer[2];

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
 * @brief IMU Bridge Operational state
*/
static IMU_Bridge_StatusTypeDef IMU_Bridge_OpState(void)
{
    char* msg = "OP STATE\n\r";
    IMU_Bridge_SendString(msg);

    IMU_Bridge_StatusTypeDef status = IMU_BRIDGE_ERROR;

    switch (bridge_op_state)
    {
    case IMU_BRIDGE_FSM_OP_SANITY_STATE:
        //status = MPU9250_Bridge_SanityState();
        //bridge_op_state = BRIDGE_FSM_OP_IDLE_STATE;
        status = IMU_Bridge_SanityState();
        break;

    case IMU_BRIDGE_FSM_OP_READ_STATE:
        status = IMU_Bridge_ReadState();
        break;

    case IMU_BRIDGE_FSM_OP_IDLE_STATE:
        //status = MPU9250_AccelRead(&hmpu1);
        //bridge_op_state = BRIDGE_FSM_OP_SANITY_STATE;
        status = IMU_Bridge_IdleState();
        break;

    case IMU_BRIDGE_FSM_OP_CONFIG_STATE:
        status = IMU_Bridge_ConfigState();
        break;
    
    default:
        status = IMU_BRIDGE_ERROR;
        break;
    }

    return status;

}

/**
 * @brief IMU Bridge Idle State
*/
static IMU_Bridge_StatusTypeDef IMU_Bridge_IdleState(void)
{
    char* msg = "\tIDLE STATE\n\r";
    IMU_Bridge_SendString(msg);

    switch (IMU_Bridge_GetCmd())
    {
    case IMU_BRIDGE_CMD_SANITY:
        bridge_op_state = IMU_BRIDGE_FSM_OP_SANITY_STATE;
        break;

    case IMU_BRIDGE_CMD_CONFIG:
        bridge_op_state = IMU_BRIDGE_FSM_OP_CONFIG_STATE;
        break;
    
    case IMU_BRIDGE_CMD_READ_MODE:
        bridge_op_state = IMU_BRIDGE_FSM_OP_READ_STATE;
        break;
    
    default:
        break;
    }

    //hmpu1.Address = MPU9250_I2C_ADDRESS_1;
    //hmpu1.I2C_Timeout = 150;
    //if (MPU9250_Init(&hmpu1) != MPU9250_OK) return IMU_BRIDGE_ERROR;
    return IMU_BRIDGE_OK;
}

/**
 * @brief IMU Bridge Sanity State
*/
static IMU_Bridge_StatusTypeDef IMU_Bridge_SanityState(void)
{
    char* msg = "\tSANITY STATE\n\r";
    IMU_Bridge_SendString(msg);
    if (MPU9250_SanityCheck() != MPU9250_OK){
        char* ans = "\t---> Sanity Check: ERROR\n\r";
        IMU_Bridge_SendString(ans);
        return IMU_BRIDGE_ERROR;
    } 
    char* ans = "\t---> Sanity Check: OK\n\r";
    IMU_Bridge_SendString(ans);
    bridge_op_state = IMU_BRIDGE_FSM_OP_IDLE_STATE;   
    return IMU_BRIDGE_OK;
}

/**
 * @brief IMU Bridge Configuration State
*/
static IMU_Bridge_StatusTypeDef IMU_Bridge_ConfigState(void)
{
    char msg[100];
    strcpy(msg, "\tCONFIG STATE\n\r");
    IMU_Bridge_SendString(msg);


    if (IMU_Bridge_GetCmd() == IMU_BRIDGE_CMD_EXIT)
    {
        bridge_op_state = IMU_BRIDGE_FSM_OP_IDLE_STATE;
        strcpy(msg, "EXIT\n\r");
        IMU_Bridge_SendString(msg);
    }
    return IMU_BRIDGE_OK;
}

/**
 * @brief IMU Bridge Read State
*/
static IMU_Bridge_StatusTypeDef IMU_Bridge_ReadState(void)
{
    char msg[200];
    strcpy(msg, "\tREAD STATE\n\r");
    IMU_Bridge_SendString(msg);
    IMU_Bridge_CmdTypeDef next_cmd = IMU_Bridge_GetCmd();

    if (next_cmd == IMU_BRIDGE_CMD_READ_ACCEL_ALL)
    {
        uint16_t AccelX, AccelY, AccelZ;
        MPU9250_AccelReadRaw(&AccelX, &AccelY, &AccelZ);
        strcpy(msg, "======================================\n\r");
        IMU_Bridge_SendString(msg);
        sprintf(msg, "ACCEL READ:\t%d\t%d\t%d\n\r", AccelX, AccelY, AccelZ);
        IMU_Bridge_SendString(msg);
        strcpy(msg, "======================================\n\r");
        IMU_Bridge_SendString(msg);
    }
    if (next_cmd == IMU_BRIDGE_CMD_READ_TEMP)
    {
        MPU9250_TempRead(temp_buffer);
    }
    if (MPU9250_IsDataReady())
    {
        uint16_t temp = (temp_buffer[0] << 8) | temp_buffer[1];
        strcpy(msg, "======================================\n\r");
        IMU_Bridge_SendString(msg);
        sprintf(msg, "TEMP READ:\t%d\n\r", temp);
        IMU_Bridge_SendString(msg);
        strcpy(msg, "======================================\n\r");
        IMU_Bridge_SendString(msg);
    }
    if (next_cmd == IMU_BRIDGE_CMD_EXIT)
    {
        bridge_op_state = IMU_BRIDGE_FSM_OP_IDLE_STATE;
        strcpy(msg, "EXIT\n\r");
        IMU_Bridge_SendString(msg);
    }
    return IMU_BRIDGE_OK;
}