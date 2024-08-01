/*
 * taskHelper.c
 *
 *  Created on: 13 Haz 2024
 *      Author: mehmet.dincer
 */

#include <API/System/Tasks/tasksHelper.h>

TaskRunTime_ts taskRuntime_s = {0};

/**
 * @brief calculate the task duration, timeout count, ...
 * @param[in] taskRuntimeGlobal variable
 * @param[in] task start timestamp
 */
void AE_taskRuntimeOperation(TaskRunTime_ts * taskRuntime_s, uint32_t * taskStartTimestamp_u32, Tasks_te taskName_e)
{
    switch(taskName_e)
    {
        case (TASK_1MS):
        {
            taskRuntime_s->taskTotalWorkingTime_ts.task1ms_u32 += 1;
            taskRuntime_s->taskWorkingDuration_s.task1ms_u32 = *taskStartTimestamp_u32 - taskRuntime_s->taskPreviousTimeStamp_s.task1ms_u32;
            taskRuntime_s->taskTimeoutCount_s.task1ms_u32 += (taskRuntime_s->taskWorkingDuration_s.task1ms_u32 > TASK_1MS ) ? 1 : 0;
            taskRuntime_s->taskPreviousTimeStamp_s.task1ms_u32 = *taskStartTimestamp_u32;
            taskRuntime_s->taskMaxWorkingDuration_s.task1ms_u32 = (taskRuntime_s->taskWorkingDuration_s.task1ms_u32 > taskRuntime_s->taskMaxWorkingDuration_s.task1ms_u32) ?
                                                                   taskRuntime_s->taskWorkingDuration_s.task1ms_u32 : taskRuntime_s->taskMaxWorkingDuration_s.task1ms_u32;
            break;
        }
        case (TASK_10MS):
        {
            taskRuntime_s->taskTotalWorkingTime_ts.task10ms_u32 += 1;
            taskRuntime_s->taskWorkingDuration_s.task10ms_u32 = *taskStartTimestamp_u32 - taskRuntime_s->taskPreviousTimeStamp_s.task10ms_u32;
            taskRuntime_s->taskTimeoutCount_s.task10ms_u32 += (taskRuntime_s->taskWorkingDuration_s.task10ms_u32 > TASK_10MS ) ? 1 : 0;
            taskRuntime_s->taskPreviousTimeStamp_s.task10ms_u32 = *taskStartTimestamp_u32;
            taskRuntime_s->taskMaxWorkingDuration_s.task10ms_u32 = (taskRuntime_s->taskWorkingDuration_s.task10ms_u32 > taskRuntime_s->taskMaxWorkingDuration_s.task10ms_u32) ?
                                                                   taskRuntime_s->taskWorkingDuration_s.task10ms_u32 : taskRuntime_s->taskMaxWorkingDuration_s.task10ms_u32;
            break;
        }
        case (TASK_1500MS):
        {
            taskRuntime_s->taskTotalWorkingTime_ts.task1500ms_u32 += 1;
            taskRuntime_s->taskWorkingDuration_s.task1500ms_u32 = *taskStartTimestamp_u32 - taskRuntime_s->taskPreviousTimeStamp_s.task1500ms_u32;
            taskRuntime_s->taskTimeoutCount_s.task1500ms_u32 += (taskRuntime_s->taskWorkingDuration_s.task1500ms_u32 > TASK_1500MS ) ? 1 : 0;
            taskRuntime_s->taskPreviousTimeStamp_s.task1500ms_u32 = *taskStartTimestamp_u32;
            taskRuntime_s->taskMaxWorkingDuration_s.task1500ms_u32 = (taskRuntime_s->taskWorkingDuration_s.task1500ms_u32 > taskRuntime_s->taskMaxWorkingDuration_s.task1500ms_u32) ?
                                                                   taskRuntime_s->taskWorkingDuration_s.task1500ms_u32 : taskRuntime_s->taskMaxWorkingDuration_s.task1500ms_u32;
            break;
        }
    }
}

