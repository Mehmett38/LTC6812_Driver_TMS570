/*
 * task100ms.h
 *
 *  Created on: 21 May 2024
 *      Author: mehmet.dincer
 */

#ifndef API_SYSTEM_TASKS_TASK100MS_TASK100MS_H_
#define API_SYSTEM_TASKS_TASK100MS_TASK100MS_H_

#include <API/System/Tasks/tasksHelper.h>

extern TaskHandle_t task_1500ms;             //!< 100ms task handler

void AE_task1500ms(void * param);

#endif /* API_SYSTEM_TASKS_TASK100MS_TASK100MS_H_ */
