/*
 * Notification.c
 *
 *  Created on: 7 Haz 2024
 *      Author: mehmet.dincer
 */

#include <API/User Defined API/LTC6812/DriverLayer/ltc681x.h>
#include <stdint.h>
#include "FreeRTOS.h"
#include "os_task.h"

extern uint32_t tickNumber_u32;
extern Ltc681x_ts ltc_s;


/**
 * @brief 1ms interrupt
 */
void vApplicationTickHook( void )
{
    tickNumber_u32++;

    ++ltc_s.watchdogTime_u32;
}








