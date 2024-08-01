/*
 * task10ms.h
 *
 *  Created on: 21 May 2024
 *      Author: mehmet.dincer
 */

#ifndef API_SYSTEM_TASKS_TASK10MS_TASK10MS_H_
#define API_SYSTEM_TASKS_TASK10MS_TASK10MS_H_

#include <API/System/Tasks/tasksHelper.h>

extern TaskHandle_t task_10ms;             //!< 10ms task handler

void AE_task10ms(void * param);


#endif /* API_SYSTEM_TASKS_TASK10MS_TASK10MS_H_ */
