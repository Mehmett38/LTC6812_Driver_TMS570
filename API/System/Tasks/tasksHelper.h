/*
 * tasksHelper.h
 *
 *  Created on: 21 May 2024
 *      Author: mehmet.dincer
 */

#ifndef API_SYSTEM_TASKS_TASKSHELPER_H_
#define API_SYSTEM_TASKS_TASKSHELPER_H_

#include <API/User Defined API/LTC6812/OperationLayer/ltcOperation.h>
#include "FreeRTOS.h"
#include "os_task.h"

extern Ltc681x_ts ltc_s;

typedef enum RtosPriorities_e
{
    TASK_IDLE,
    TASK_LOW_PRIOTIY,
    TASK_MIDDLE_PRIORITY,
    TASK_HIGh_PRIORITY,
    TASK_REAL_TIME
}RtosPriorities_te;

typedef enum Tasks_e
{
    TASK_1MS = 1u,
    TASK_10MS = 10u,
    TASK_1500MS = 1500u,
}Tasks_te;

typedef struct TaskTotalWorkingTime_s
{
    uint32_t task1ms_u32;
    uint32_t task10ms_u32;
    uint32_t task1500ms_u32;
}TaskTotalWorkingTime_ts;

typedef struct TaskPreviousTimeStamp_s
{
    uint32_t task1ms_u32;
    uint32_t task10ms_u32;
    uint32_t task1500ms_u32;
}TaskPreviousTimeStamp_ts;

typedef struct TaskWorkingTime_s
{
    uint16_t task1ms_u32;
    uint16_t task10ms_u32;
    uint16_t task1500ms_u32;
}TaskWorkingDuration_ts;

typedef struct TaskTimeoutCount_s
{
    uint32_t task1ms_u32;
    uint32_t task10ms_u32;
    uint32_t task1500ms_u32;
}TaskTimeoutCount_ts;

typedef struct TaskMaxWorkingDuration_s
{
    uint32_t task1ms_u32;
    uint32_t task10ms_u32;
    uint32_t task1500ms_u32;
}TaskMaxWorkingDuration_ts;

typedef struct TaskRunTime_s
{
    TaskTotalWorkingTime_ts taskTotalWorkingTime_ts;
    TaskWorkingDuration_ts taskWorkingDuration_s;
    TaskMaxWorkingDuration_ts taskMaxWorkingDuration_s;
    TaskTimeoutCount_ts taskTimeoutCount_s;
    TaskPreviousTimeStamp_ts taskPreviousTimeStamp_s;
}TaskRunTime_ts;

extern TaskRunTime_ts taskRuntime_s;


void AE_taskRuntimeOperation(TaskRunTime_ts * taskRuntime_s, uint32_t * taskStartTimestamp_u32, Tasks_te taskName_e);


#endif /* API_SYSTEM_TASKS_TASKSHELPER_H_ */
