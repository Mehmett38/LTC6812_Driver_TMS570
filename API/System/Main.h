/*
 * orkaMain.h
 *
 *  Created on: 7 Haz 2024
 *      Author: mehmet.dincer
 */

#ifndef API_SYSTEM_MAIN_H_
#define API_SYSTEM_MAIN_H_

#include <API/System/Tasks/Task100ms/task100ms.h>
#include <API/System/Tasks/Task10ms/task10ms.h>
#include <API/System/Tasks/Task1ms/task1ms.h>
#include <API/User Defined API/ASP/tms_asp.h>
#include <API/User Defined API/LTC6812/DriverLayer/ltc681x.h>
#include "spi.h"
#include "gio.h"

int Main(void * arguments);


#endif /* API_SYSTEM_MAIN_H_ */
