/*
 * orkaMain.c
 *
 *  Created on: 7 Haz 2024
 *      Author: mehmet.dincer
 */

#include <API/System/Main.h>

static void hardwareInit(void);
Ltc681x_ts ltc_s;

int Main(void * arguments)
{
    hardwareInit();

    /*<<<<<<<<<<<<<<<<<<<<<<<<<<<<-Task Creation->>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
    //!< 10ms task
    BaseType_t status = pdFAIL;

    status = xTaskCreate(AE_task1ms,
                         "task_1ms",
                         (configMINIMAL_STACK_SIZE * 1),
                         NULL,
                         TASK_REAL_TIME,
                         &task_1ms);
    configASSERT(status);

    status = xTaskCreate(AE_task10ms,
                         "task_10ms",
                         (configMINIMAL_STACK_SIZE * 3),
                         NULL,
                         TASK_HIGh_PRIORITY,
                         &task_10ms);
    configASSERT(status);

    //!< 100ms task dummy task
    status = xTaskCreate(AE_task1500ms,
                         "task_1500ms",
                         (configMINIMAL_STACK_SIZE * 3),
                         NULL,
                         TASK_MIDDLE_PRIORITY,
                         &task_1500ms);
    configASSERT(status);

    vTaskStartScheduler();

    return 0;
}

static void hardwareInit(void)
{
    //used for ltc6812
    spiInit();

    //toggle to led
    gioInit();
}

/**
 * @brief initialize the ltc register, cell number, upper and lower threshold
 */
void ltcInit(void)
{
    //slave configuration
    for(uint8_t i = 0; i < SLAVE_NUMBER; i++)
    {
        ltc_s.afeSlaveStatus_s[i].systemConfig_s.adcopt_b = false;  //to used high speed adc reading
        ltc_s.afeSlaveStatus_s[i].systemConfig_s.refon_b = true;

        ltc_s.afeSlaveStatus_s[i].systemConfig_s.numberOfCell_u8 = 13u;
        ltc_s.afeSlaveStatus_s[i].systemConfig_s.gioPullStatus = ALL_GPIO_PULL_DOWN_OFF; //to set GPIO_x lines pull up GPIO_1 | GPIO_2...
    }

    //system configuration
    ltc_s.afeSystemStatus.balanceLowerThresholdInMah_f = 15.0f;                  //15mV
    ltc_s.afeSystemStatus.balanceUpperThresholdInMah_f = 30.0f;                  //30mV
    ltc_s.afeSystemStatus.balanceThresholdInMah_f = ltc_s.afeSystemStatus.balanceUpperThresholdInMah_f;
    ltc_s.afeSystemStatus.balanceVoltageLowerLimit_f = 3.2f;        //if voltage is lower than 3.2V do not make balance
    ltc_s.afeSystemStatus.balanceTemperatureUpperLimit_u16 = 120;    //if temperatyre is upper than 120 degree do not make balance

    //spiConfiguration
    ltc_s.spi_s.spiConfig_s.CSNR = FRAM_CS_PIN_0;
    ltc_s.spi_s.spiConfig_s.CS_HOLD = true;
    ltc_s.spi_s.spiConfig_s.DFSEL = SPI_FMT_0;
    ltc_s.spi_s.spiConfig_s.WDEL = true;

    ltc_s.spi_s.spiReg_s = spiREG3;

    //initialize system
    AE_ltcInit(&ltc_s);
}








