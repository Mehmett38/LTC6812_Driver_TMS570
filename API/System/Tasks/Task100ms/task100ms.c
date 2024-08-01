/*
 * task100ms.c
 *
 *  Created on: 21 May 2024
 *      Author: mehmet.dincer
 */

#include <API/System/Tasks/Task100ms/task100ms.h>

TaskHandle_t task_1500ms;             //!< 10ms task handler
static LtcStatus_te ltcStatus_e;
BalanceStatus_te balanceStatus_e;

extern void ltcInit(void);

void AE_task1500ms(void * param)
{
    TickType_t xLastWakeTime;
    const TickType_t xFrequency = 1500;

    // Initialise the xLastWakeTime variable with the current time.
    xLastWakeTime = xTaskGetTickCount ();

    //used RTOS tick function so declare in highest priority task
    ltcInit();

    for(;;)
    {
        vTaskDelayUntil(&xLastWakeTime, xFrequency);

        uint32_t taskStartTimestamp_u32 = AE_getTick();

        ltcStatus_e = AE_ltcCollectData(&ltc_s);

        if(ltcStatus_e == LTC_OK)
        {
            //balance operation
            AE_ltcBalanceOperation(&ltc_s);
        }
        else
        {
            //used for debugging
            int a = 10;
            (void)a;
        }

        AE_taskRuntimeOperation(&taskRuntime_s, &taskStartTimestamp_u32, TASK_1500MS);
    }
}
