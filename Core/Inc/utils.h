/**
  ******************************************************************************
  * @file           : api_delay.h
  * @brief          : Non-blocking delay module header
  * @authors        : Segundo Molina Abeniacar and Gonzalo Gabriel Fernandez
  ******************************************************************************
  */

#ifndef __UTILS_H
#define __UTILS_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>

/* Exported types ------------------------------------------------------------*/
typedef bool bool_t;
typedef uint32_t tick_t;

/* Exported types ------------------------------------------------------------*/
typedef struct {
  tick_t start_time;
  tick_t duration;
  bool_t running;
} delay_t;

/* Exported functions prototypes ---------------------------------------------*/
void delay_init(delay_t* delay, tick_t duration);
bool_t delay_read(delay_t* delay);
void delay_write(delay_t* delay, tick_t duration);

#ifdef __cplusplus
}
#endif

#endif /* __UTILS_H */