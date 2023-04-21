/**
  ******************************************************************************
  * @file           : imu_bridge_fsm.h
  * @author         : Gonzalo Gabriel Fernandez
  * @brief          : IMU Bridge FSM header
  ******************************************************************************
  * @attention
  *
  * IMU Bridge Finite State Machine
  *
  ******************************************************************************
  */

#ifndef __IMU_BRIDGE_FSM_H
#define __IMU_BRIDGE_FSM_H

#ifdef __cplusplus
extern "C" {
#endif

#include "imu_bridge.h"

void IMU_Bridge_FsmInit(void);
void IMU_Bridge_FsmUpdate(void);

#ifdef __cplusplus
}
#endif

#endif /* __IMU_BRIDGE_FSM_H */