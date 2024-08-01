/*
 * task10ms.c
 *
 *  Created on: 21 May 2024
 *      Author: mehmet.dincer
 */

#include <API/System/Tasks/Task1ms/task1ms.h>

TaskHandle_t task_1ms;             //!< 10ms task handler

extern void ltcInit(void);

void AE_task1ms(void * param)
{
    TickType_t xLastWakeTime;
    const TickType_t xFrequency = 1;

    // Initialise the xLastWakeTime variable with the current time.
    xLastWakeTime = xTaskGetTickCount ();

    for(;;)
    {
        vTaskDelayUntil( &xLastWakeTime, xFrequency);

        uint32_t taskStartTimestamp_u32 = AE_getTick();

        if(ltc_s.watchdogTime_u32 > LTC_WATCHDOG_TIME)
        {
            AE_ltcWatchdogCallback(&ltc_s);
        }

        AE_taskRuntimeOperation(&taskRuntime_s, &taskStartTimestamp_u32, TASK_1MS);
    }
}



