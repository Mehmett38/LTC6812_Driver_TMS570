/*
 * task10ms.c
 *
 *  Created on: 21 May 2024
 *      Author: mehmet.dincer
 */
#include <API/System/Tasks/Task10ms/task10ms.h>

TaskHandle_t task_10ms;             //!< 10ms task handler

void AE_task10ms(void * param)
{
    TickType_t xLastWakeTime;
    const TickType_t xFrequency = 10;

    // Initialise the xLastWakeTime variable with the current time.
    xLastWakeTime = xTaskGetTickCount ();

    for(;;)
    {
        vTaskDelayUntil( &xLastWakeTime, xFrequency );

        uint32_t taskStartTimestamp_u32 = AE_getTick();

        uint32_t startTimeStamp_u32 = AE_getTick();

        AE_taskRuntimeOperation(&taskRuntime_s, &taskStartTimestamp_u32, TASK_10MS);
    }
}



