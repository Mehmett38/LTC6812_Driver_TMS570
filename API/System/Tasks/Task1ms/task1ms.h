/*
 * task10ms.h
 *
 *  Created on: 21 May 2024
 *      Author: mehmet.dincer
 */

#ifndef ORKA_SYSTEM_TASKS_TASK10MS_TASK1MS_H_
#define ORKA_SYSTEM_TASKS_TASK10MS_TASK1MS_H_

#include <API/System/Tasks/tasksHelper.h>

extern TaskHandle_t task_1ms;             //!< 10ms task handler

void AE_task1ms(void * param);


#endif /* ORKA_SYSTEM_TASKS_TASK10MS_TASK1MS_H_ */
