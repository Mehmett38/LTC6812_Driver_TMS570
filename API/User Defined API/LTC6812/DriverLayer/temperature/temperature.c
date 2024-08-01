/*
 * temperature.c
 *
 *  Created on: 31 Oca 2024
 *      Author: mehmet.dincer
 */


#include <API/User Defined API/LTC6812/DriverLayer/temperature/temperature.h>

/**
 * @brief calculate the temperature with NTC
 * @param[in] ADC voltage value
 * @param[in] refVolt value
 * @param[in] system type for parameter search @refgroup PULL_
 * @return temperature in celcius
 * @structure
 *      refVolt--------/\/\/\/\----|----/\/\/\/\--------GND
 *                    resistance   |      NTC
 *                                 |
 *                                 |
 *                                ADC
 */
double AE_calculateTemp(float adcValue, float refVolt, uint8_t PULL_)
{
    uint16_t adcVal;

    if(PULL_ == PULL_UP)
    {
        adcValue = refVolt - adcValue;             // take the resistance voltage
    }

    adcVal = (adcValue / refVolt) * NTC_ADC_MAX;

    float rntc = (float)NTC_R_SERIES / (((float)NTC_ADC_MAX / (float)adcVal ) - 1.0f);
    float temp;
    temp = rntc / (float)NTC_R_NOMINAL;
    temp = logf(temp);
    temp /= (float)NTC_BETA;
    temp += 1.0f / ((float)NTC_TEMP_NOMINAL + 273.15f);
    temp = 1.0f / temp;
    temp -= 273.15f;
    return temp;
}
