/*
 * tms_asp.c
 *
 *  Created on: 7 Haz 2024
 *      Author: mehmet.dincer
 */

#include <API/User Defined API/ASP/tms_asp.h>


uint32_t tickNumber_u32;

/**
 * @brief wait in time(ms)
 * @param[in] time to wait
 * @return none
 */
void AE_delay(uint32_t time)
{
    uint32_t startTime_u32 = AE_getTick();

    while((AE_getTick() - startTime_u32) < time);
}

/**
 * @brief get the
 */
uint32_t AE_getTick(void)
{
    return tickNumber_u32;
}


