/*
 * ltcOperation.c
 *
 *  Created on: 12 Haz 2024
 *      Author: mehmet.dincer
 */

#include <API/User Defined API/LTC6812/OperationLayer/ltcOperation.h>


LtcStatus_te AE_ltcCollectData(Ltc681x_ts * ltc_s)
{
    if(ltc_s->afeSystemStatus.balanceStatus_e == BALANCING)
    {
        AE_ltcStopBalance(ltc_s);
    }

    LtcStatus_te ltcStatus_e;

    //read the cell voltages, and calculate the min, max, mean cell voltage in slave and system
    ltcStatus_e = AE_ltcReadCellVoltage(ltc_s, MODE_7KHZ, true, CELL_ALL, 10);
    ASSERT_LTC(ltcStatus_e);

    //read gpio voltages, Before the temperature reading, AE_ltcReadGpioVoltage must call
    ltcStatus_e = AE_ltcReadGpioVoltage(ltc_s, MODE_7KHZ, GPIO_ALL, 10);
    ASSERT_LTC(ltcStatus_e);
    AE_ltcReadTemperature(ltc_s, (/*NTC_1 |NTC_2| NTC_3|NTC_4 | NTC_5 | NTC_6 | NTC_7 | NTC_8 |*/ NTC_9) , PULL_DOWN);

    //read the status registerA, this register constains sum of cell, internal die temperature, analog supply voltage
    //max internal die temperature
    ltcStatus_e = AE_ltcReadStatusRegisterA(ltc_s, MODE_7KHZ, CHSTA_ALL, 10);
    if(ltc_s->afeSystemStatus.maxInternalDieTemperature_f > THERMAL_SHUTDOWN_TEMPERATURE)
    {
        ltcStatus_e = LTC_THERMAL_SHUTDOWN_WARNING;
    }
    ASSERT_LTC(ltcStatus_e);

    //read the status registerA, This register constains digital supply voltage, under and overvoltage flags
    ltcStatus_e = AE_ltcReadStatusRegisterB(ltc_s, MODE_7KHZ, CHSTB_ALL, 10);
    if(ltc_s->afeSystemStatus.muxFail_b)
    {
        ltcStatus_e = LTC_MUXFAIL;
        AE_ltcInit(ltc_s);
    }
    if(ltc_s->afeSystemStatus.thermalShutdownOccured_b)
    {
        ltcStatus_e = LTC_THERMAL_SHUTDOWN_OCCURED;
        AE_ltcInit(ltc_s);
    }
    ASSERT_LTC(ltcStatus_e);

    return ltcStatus_e;
}

