/*
 * ltc681x.c
 *
 *  Created on: 10 Haz 2024
 *      Author: mehmet.dincer
 */

#include <API/User Defined API/LTC6812/DriverLayer/ltc681x.h>


static uint16_t txBuffer[SLAVE_NUMBER * TRANSMIT_LEN];
static uint16_t rxBuffer[SLAVE_NUMBER * RECEIVE_LEN];
static uint16_t dummy_u16 = 0xFF;                                       // dummy variable

static void AE_ltcWrite(Ltc681x_ts * ltc_s, uint16_t * txData_pu16, uint16_t cmd[4]);
static void AE_ltcCmdWrite(Ltc681x_ts * ltc_s, uint16_t cmd[4]);
static LtcStatus_te AE_ltcRead(Ltc681x_ts * ltc_s, uint16_t * rxData, uint16_t cmd[4]);
static uint16_t AE_ltcCmdRead(Ltc681x_ts * ltc_s, uint16_t cmd[4]);
static void AE_ltcWakeUpSleep(Ltc681x_ts * ltc_s);
static void AE_ltcWakeUpIdle(Ltc681x_ts * ltc_s);
static void AE_ltcStartCellAdc(Ltc681x_ts * ltc_s, AdcMode_ts adcMode, uint8_t dischargePermit, uint8_t CELL_);
static uint8_t AE_ltcAdcMeasureState(Ltc681x_ts * ltc_s);
static void AE_ltcFindMinCellVoltages(Ltc681x_ts * ltc_s);
static void AE_ltcFindMaxCellVoltages(Ltc681x_ts * ltc_s);
static void AE_ltcFindMeanCellVoltages(Ltc681x_ts * ltc_s);
static void AE_ltcStartGpioAdc(Ltc681x_ts * ltc_s, AdcMode_ts adcMode_s, uint8_t GPIO_);
static void AE_ltcSetAdcSamplingReg(Ltc681x_ts * ltc_s, AdcMode_ts adcMode_s);
static void AE_ltcFindMinTemperature(Ltc681x_ts * ltc_s, uint16_t NTC_);
static void AE_ltcFindMaxTemperature(Ltc681x_ts * ltc_s, uint16_t NTC_);
static void AE_ltcStartStatusAdc(Ltc681x_ts * ltc_s, AdcMode_ts adcMode_s, uint8_t CHSTA_);
static void AE_ltcStartDiagnAdc(Ltc681x_ts * ltc_s);
static void AE_ltcBalance(Ltc681x_ts * ltc_s);
static BalanceStatus_te AE_ltcCheckBalanceStatus(Ltc681x_ts * ltc_s);

/**
 * @brief initialize the ltc register, cell number, upper and lower threshold
 * @param[in] ltc global variable
 * @return if crc is wrong return LTC_WRONG_CRC else LTC_OK
 */
void AE_ltcInit(Ltc681x_ts * ltc_s)
{
    init_PEC15_Table();

    for(uint8_t i = 0; i < SLAVE_NUMBER; i++)
    {
        memset((void*)&ltc_s->afeSlaveStatus_s[i], 0, sizeof(AfeRegisters_ts));

        //!<Configuration Register A
        ltc_s->afeSlaveStatus_s[i].afeRegisters_s.cfgAr_s.CFGAR0_u16 |= (ltc_s->afeSlaveStatus_s[i].systemConfig_s.adcopt_b << ADCOPT_OFFSET) |
                                                                        (ltc_s->afeSlaveStatus_s[i].systemConfig_s.refon_b << REFON_OFFSET)   |
                                                                        ((ltc_s->afeSlaveStatus_s[i].systemConfig_s.gioPullStatus & 0x1F) << GPIO1_OFFSET);

        //!<Configuration Register B
        ltc_s->afeSlaveStatus_s[i].afeRegisters_s.cfgBr_s.CFGBR0_u16 |= ltc_s->afeSlaveStatus_s[i].systemConfig_s.gioPullStatus >> 5u;
    }

    AE_ltcWrite(ltc_s, (uint16_t*)&ltc_s->afeSlaveStatus_s[0].afeRegisters_s.cfgAr_s, cmdWRCFGA_pu16);
    AE_ltcWrite(ltc_s, (uint16_t*)&ltc_s->afeSlaveStatus_s[0].afeRegisters_s.cfgBr_s, cmdWRCFGB_pu16);

//    LtcStatus_te status = AE_ltcRead(ltc_s, rxBuffer, cmdRDCFGA_pu16);
//    status = AE_ltcRead(ltc_s, rxBuffer, cmdRDCFGB_pu16);
}

/**
 * @brief It fills the given values into the buffer by adding crc to the end and then sends them.
 * @param[in] transmitted datas address
 * @param[in] cmd command + 2byte crc
 * @return none
 */
void AE_ltcWrite(Ltc681x_ts * ltc_s, uint16_t * txData_pu16, uint16_t cmd[4])
{
    uint8_t bufferLen_u8 = SLAVE_NUMBER * TRANSMIT_LEN + CMD_LEN;
    uint16_t pec_u16;

    //assign command and pec
    for(int i = 0; i < CMD_LEN; i++)
    {
        txBuffer[i] = cmd[i];
    }

    for(int i = (SLAVE_NUMBER - 1); i >=0; i--)
    {
        for(int j = 0; j < REGISTER_LEN; j++)
        {
            txBuffer[j + CMD_LEN + (TRANSMIT_LEN * (SLAVE_NUMBER - i - 1))] = txData_pu16[(i * sizeof(AfeSlaveStatus_ts)/sizeof(uint16_t)) + j];
        }
        pec_u16 = AE_pec15((uint8_t*)&txData_pu16[i * sizeof(AfeSlaveStatus_ts)/sizeof(uint16_t)], REGISTER_LEN);
        txBuffer[CMD_LEN + TRANSMIT_LEN * (SLAVE_NUMBER - i - 1) + 6] = (pec_u16 >> 8) & 0xFF;   //+6 = pec0 index
        txBuffer[CMD_LEN + TRANSMIT_LEN * (SLAVE_NUMBER - i - 1) + 7] = (pec_u16 >> 0) & 0xFF;   //+7 = pec1 index
    }

    AE_ltcWakeUpSleep(ltc_s);

    AE_LTC_CS_ON(ltc_s);

    AE_LTC_WATCHDOG_RESET(ltc_s);
    spiTransmitData(ltc_s->spi_s.spiReg_s, &ltc_s->spi_s.spiConfig_s, bufferLen_u8, txBuffer);

    AE_LTC_CS_OFF(ltc_s);
}

/**
 * @brief write the cmd command
 * @param[in] cmd + pec
 * @return none
 */
void AE_ltcCmdWrite(Ltc681x_ts * ltc_s, uint16_t cmd[4])
{
    AE_ltcWakeUpSleep(ltc_s);

    AE_LTC_CS_ON(ltc_s);

    spiTransmitData(ltc_s->spi_s.spiReg_s, &ltc_s->spi_s.spiConfig_s, CMD_LEN, cmd);

    AE_LTC_CS_OFF(ltc_s);

    AE_delay(1);
}

/**
 * @brief Read the cmd register and assign to rxData address
 * @param[in] return data address
 * @param[in] cmd + byte crc
 * @return if crc is suitable with message return OK else CRC_ERROR
 */
static LtcStatus_te AE_ltcRead(Ltc681x_ts * ltc_s, uint16_t * rxData, uint16_t cmd[4])
{
    AE_ltcWakeUpSleep(ltc_s);
    AE_ltcWakeUpIdle(ltc_s);

    AE_delay(1);

    AE_LTC_CS_ON(ltc_s);

    spiTransmitData(ltc_s->spi_s.spiReg_s, &ltc_s->spi_s.spiConfig_s, 4, cmd);
    spiReceiveData(ltc_s->spi_s.spiReg_s, &ltc_s->spi_s.spiConfig_s, (SLAVE_NUMBER * RECEIVE_LEN), rxBuffer);

    AE_LTC_CS_OFF(ltc_s);

    for(int i = 0; i < SLAVE_NUMBER; i++)
    {
        uint16_t pec = AE_pec15((uint8_t*)&rxBuffer[RECEIVE_LEN * i], REGISTER_LEN);        // calculate the crc of the first 6 index

        uint16_t readPec = (rxBuffer[RECEIVE_LEN * i + 6] << 8) | (rxBuffer[RECEIVE_LEN * i + 7]);

        if(pec != readPec) return LTC_WRONG_CRC;

        for(int j = 0; j < RECEIVE_LEN; j++)
        {
            rxData[j + i * RECEIVE_LEN] = rxBuffer[RECEIVE_LEN * i + j];
        }
    }

    return LTC_OK;
}

/**
 * @brief read the some specific register
 * @param[in] cmd + pec
 * @return register value
 */
uint16_t AE_ltcCmdRead(Ltc681x_ts * ltc_s, uint16_t cmd[4])
{
    uint16_t returnVal;

    AE_LTC_CS_ON(ltc_s);

    spiTransmitData(ltc_s->spi_s.spiReg_s, &ltc_s->spi_s.spiConfig_s, 4, cmd);
    spiReceiveData(ltc_s->spi_s.spiReg_s, &ltc_s->spi_s.spiConfig_s, 1, &returnVal);

    AE_LTC_CS_OFF(ltc_s);

    return returnVal;
}

/**
 * @brief ltc6812 enter standby mode
 * @return none
 */
void AE_ltcWakeUpSleep(Ltc681x_ts * ltc_s)
{
    for(int i = 0; i < SLAVE_NUMBER; i++)
    {
        AE_LTC_CS_ON(ltc_s);

        AE_LTC_WATCHDOG_RESET(ltc_s);
        spiTransmitData(ltc_s->spi_s.spiReg_s, &ltc_s->spi_s.spiConfig_s, 1, &dummy_u16);

        AE_LTC_CS_OFF(ltc_s);
    }

//    AE_delay(1);                                          //!< 100us delay, t-wake time
}

/**
 * @brief isoSPI wake up from idle state and enters the READY state
 * @return none
 */
void AE_ltcWakeUpIdle(Ltc681x_ts * ltc_s)
{
    for(int i = 0; i < SLAVE_NUMBER; i++)
    {
        AE_LTC_CS_ON(ltc_s);

        AE_LTC_WATCHDOG_RESET(ltc_s);
        spiTransmitData(ltc_s->spi_s.spiReg_s, &ltc_s->spi_s.spiConfig_s, 1, &dummy_u16);

        AE_LTC_CS_OFF(ltc_s);
    }
//    AE_delay(1);                                           //!< 10us delay, t-ready time
}

/**
 * @brief ltc6812 has backdownd counter that duration is 2s. End of duration ltc6812 reset
 * @param[in] ltc global variable
 * @return none
 */
void AE_ltcWatchdogCallback(Ltc681x_ts * ltc_s)
{
    AE_LTC_WATCHDOG_RESET(ltc_s);
    AE_ltcInit(ltc_s);
}

void AE_ltcSetAdcSamplingReg(Ltc681x_ts * ltc_s, AdcMode_ts adcMode_s)
{
    //set the adcMode sampling speed
    if(ltc_s->afeSlaveStatus_s[0].afeRegisters_s.cfgAr_s.CFGAR0_u16 != (adcMode_s >> 4))  //check the ADCOPT bit
    {
        for(int i = 0; i < SLAVE_NUMBER; i++)
        {
            ltc_s->afeSlaveStatus_s[0].afeRegisters_s.cfgAr_s.CFGAR0_u16 &= ~(1 << ADCOPT_OFFSET);
            ltc_s->afeSlaveStatus_s[0].afeRegisters_s.cfgAr_s.CFGAR0_u16 |= adcMode_s >> 4;
        }
        AE_ltcWrite(ltc_s, (uint16_t*)&ltc_s->afeSlaveStatus_s[0].afeRegisters_s, cmdWRCFGA_pu16);
    }
}

/**
 * @brief Start Cell Voltage ADC Conversion and Poll Status
 * @param[in] ltcBat global variable
 * @param[in] 0 Discharge Not Permitted 1 Discharge Permitted
 * @param[in] cell selection for adc conversion for parameter search @refgroup CH
 * @return if crc is suitable with message retur ok else crcError
 */
void AE_ltcStartCellAdc(Ltc681x_ts * ltc_s, AdcMode_ts adcMode, uint8_t dischargePermit, uint8_t CELL_)
{
    uint16_t adcvReg = 0x0260;   //!< base value of ADCV register

    //set the adcMode sampling speed
    AE_ltcSetAdcSamplingReg(ltc_s, adcMode);

    adcvReg |= (adcMode << 7);
    adcvReg |= (dischargePermit << 4);
    adcvReg |= CELL_;

    uint16_t cmd[4] = {0};
    uint16_t pec;

    cmd[0] = (adcvReg >> 8) & 0x00FF;
    cmd[1] = (adcvReg >> 0) & 0x00FF;
    pec = AE_pec15((uint8_t*)cmd, 2);
    cmd[2] = (pec >> 8) & 0x00FF;
    cmd[3] = (pec >> 0) & 0x00FF;

    AE_ltcCmdWrite(ltc_s, cmd);
}

/**
 * @brief after starting to cell adc measure, we can control process is finish or not
 * @return if measure is completed return 1 else 0
 */
uint8_t AE_ltcAdcMeasureState(Ltc681x_ts * ltc_s)
{
    uint8_t status = (uint8_t)AE_ltcCmdRead(ltc_s, cmdPladc_pu16);
    return status;
}

/**
 * @brief read the selected cell voltages
 * @param[in] ltc global variable
 * @param[in] adc sampling speed
 * @param[in] If the discharge permitted (DCP) bit is high at the time of a cell measurement command, the S pin discharge states
    do not change during cell measurements. If the DCP bit is low, S pin discharge states will be disabled while the
    corresponding cell or adjacent cells are being measured.
 * @param[in] cell selection for adc conversion for parameter search @refgroup CH
 * @return if crc is suitable with message retur ok else crcError or timeout
 */
LtcStatus_te AE_ltcReadCellVoltage(Ltc681x_ts * ltc_s, AdcMode_ts mode, uint8_t dischargePermit, uint8_t CELL_, uint32_t timeout)
{
    LtcStatus_te ltcStatus;
    float * cellPtr_pf = NULL;                 // To access each index separately
    uint16_t * rxBufferPtr_pu16 = NULL;                  // to access the rx buffer's 8k index
    uint32_t timeStamp1_u32 = AE_getTick();

    AE_ltcStartCellAdc(ltc_s, mode, dischargePermit, CELL_);
    while(!AE_ltcAdcMeasureState(ltc_s))
    {
        if((AE_getTick() - timeStamp1_u32) > timeout) return LTC_TIMEOUT;
    }

    //!< voltage register Group A
    ltcStatus = AE_ltcRead(ltc_s, rxBuffer, cmdRDCVA_pu16);
    ASSERT_LTC(ltcStatus);

    for(int i = 0; i < SLAVE_NUMBER; i++)
    {
        cellPtr_pf = &ltc_s->afeSlaveStatus_s[i].cellVolts_s.cell1_f;
        rxBufferPtr_pu16 = &rxBuffer[i * RECEIVE_LEN];

        for(int j = 0; j < 3; j++)
        {
            cellPtr_pf[j] = ((rxBufferPtr_pu16[j*2] << 0) | (rxBufferPtr_pu16[j*2 + 1] << 8)) / 10000.0f;  //!< 10000 comes from datasheet
        }
    }

    //!< voltage register Group B
    ltcStatus = AE_ltcRead(ltc_s, rxBuffer, cmdRDCVB_pu16);
    ASSERT_LTC(ltcStatus);

    for(int i = 0; i < SLAVE_NUMBER; i++)
    {
        cellPtr_pf = &ltc_s->afeSlaveStatus_s[i].cellVolts_s.cell1_f;
        rxBufferPtr_pu16 = &rxBuffer[i * RECEIVE_LEN];

        for(int j = 0; j < 3; j++)
        {   //offset = 3
            cellPtr_pf[j + 3] = ((rxBufferPtr_pu16[j*2] << 0) | (rxBufferPtr_pu16[j*2 + 1] << 8)) / 10000.0f;  //!< 10000 comes from datasheet
        }
    }

    //!< voltage register Group C
    ltcStatus = AE_ltcRead(ltc_s, rxBuffer, cmdRDCVC_pu16);
    ASSERT_LTC(ltcStatus);

    for(int i = 0; i < SLAVE_NUMBER; i++)
    {
        cellPtr_pf = &ltc_s->afeSlaveStatus_s[i].cellVolts_s.cell1_f;
        rxBufferPtr_pu16 = &rxBuffer[i * RECEIVE_LEN];

        for(int j = 0; j < 3; j++)
        {   //offset = 6
            cellPtr_pf[j + 6] = ((rxBufferPtr_pu16[j*2] << 0) | (rxBufferPtr_pu16[j*2 + 1] << 8)) / 10000.0f;  //!< 10000 comes from datasheet
        }
    }

    //!< voltage register Group D
    ltcStatus = AE_ltcRead(ltc_s, rxBuffer, cmdRDCVD_pu16);
    ASSERT_LTC(ltcStatus);

    for(int i = 0; i < SLAVE_NUMBER; i++)
    {
        cellPtr_pf = &ltc_s->afeSlaveStatus_s[i].cellVolts_s.cell1_f;
        rxBufferPtr_pu16 = &rxBuffer[i * RECEIVE_LEN];

        for(int j = 0; j < 3; j++)
        {   //offset = 9
            cellPtr_pf[j + 9] = ((rxBufferPtr_pu16[j*2] << 0) | (rxBufferPtr_pu16[j*2 + 1] << 8)) / 10000.0f;  //!< 10000 comes from datasheet
        }
    }

    //!< voltage register Group E
    ltcStatus = AE_ltcRead(ltc_s, rxBuffer, cmdRDCVE_pu16);
    ASSERT_LTC(ltcStatus);

    for(int i = 0; i < SLAVE_NUMBER; i++)
    {
        cellPtr_pf = &ltc_s->afeSlaveStatus_s[i].cellVolts_s.cell1_f;
        rxBufferPtr_pu16 = &rxBuffer[i * RECEIVE_LEN];

        for(int j = 0; j < 3; j++)
        {   //offset = 12
            cellPtr_pf[j + 12] = ((rxBufferPtr_pu16[j*2] << 0) | (rxBufferPtr_pu16[j*2 + 1] << 8)) / 10000.0f;  //!< 10000 comes from datasheet
        }
    }

    AE_ltcFindMinCellVoltages(ltc_s);
    AE_ltcFindMaxCellVoltages(ltc_s);
    AE_ltcFindMeanCellVoltages(ltc_s);

    if(ltc_s->afeSystemStatus.minCellVoltage_f < OPEN_WIRE_CELL_VOLTAGE)
    {
        return LTC_CELL_OPEN_WIRE;
    }

    ltc_s->afeSystemStatus.minMaxVoltageDifferenceInMah_f = (ltc_s->afeSystemStatus.maxCellVoltage_f - ltc_s->afeSystemStatus.minCellVoltage_f) * 1000.0f;

    return ltcStatus;
}

/**
 * @brief find the min cell voltage in slave and min cell voltage in system and assign them ltc global variable
 * @param[in] bms global variable
 * @return none
 */
static void AE_ltcFindMinCellVoltages(Ltc681x_ts * ltc_s)
{
    float *cellPtr_pf = NULL;
    float minCellVoltageInSlave_f;

    for(int i = 0; i < SLAVE_NUMBER; i++)
    {
        cellPtr_pf = &ltc_s->afeSlaveStatus_s[i].cellVolts_s.cell1_f;
        minCellVoltageInSlave_f = ltc_s->afeSlaveStatus_s[i].cellVolts_s.cell1_f;

        for(int j = 1; j < ltc_s->afeSlaveStatus_s[i].systemConfig_s.numberOfCell_u8; j++)
        {
            if(cellPtr_pf[j] < minCellVoltageInSlave_f)
            {
                minCellVoltageInSlave_f = cellPtr_pf[j];
            }
        }
        ltc_s->afeSlaveStatus_s[i].minCellVoltage_f = minCellVoltageInSlave_f;

        if(i == 0)
            ltc_s->afeSystemStatus.minCellVoltage_f = ltc_s->afeSlaveStatus_s[0].minCellVoltage_f;
        else
        {
            if(ltc_s->afeSlaveStatus_s[i].minCellVoltage_f < ltc_s->afeSystemStatus.minCellVoltage_f)
            {
                ltc_s->afeSystemStatus.minCellVoltage_f = ltc_s->afeSlaveStatus_s[i].minCellVoltage_f;
            }
        }
    }
}

/**
 * @brief find the max cell voltage in slave and max cell voltage in system and assign them ltc global variable
 * @param[in] bms global variable
 * @return none
 */
static void AE_ltcFindMaxCellVoltages(Ltc681x_ts * ltc_s)
{
    float *cellPtr_pf = NULL;
    float maxCellVoltageInSlave_f;

    for(int i = 0; i < SLAVE_NUMBER; i++)
    {
        cellPtr_pf = &ltc_s->afeSlaveStatus_s[i].cellVolts_s.cell1_f;
        maxCellVoltageInSlave_f = ltc_s->afeSlaveStatus_s[i].cellVolts_s.cell1_f;

        for(int j = 1; j < ltc_s->afeSlaveStatus_s[i].systemConfig_s.numberOfCell_u8; j++)
        {
            if(cellPtr_pf[j] > maxCellVoltageInSlave_f)
            {
                maxCellVoltageInSlave_f = cellPtr_pf[j];
            }
        }
        ltc_s->afeSlaveStatus_s[i].maxCellVoltage_f = maxCellVoltageInSlave_f;

        if(i == 0)
            ltc_s->afeSystemStatus.maxCellVoltage_f = ltc_s->afeSlaveStatus_s[0].maxCellVoltage_f;
        else
        {
            if(ltc_s->afeSlaveStatus_s[i].maxCellVoltage_f > ltc_s->afeSystemStatus.maxCellVoltage_f)
            {
                ltc_s->afeSystemStatus.maxCellVoltage_f = ltc_s->afeSlaveStatus_s[i].maxCellVoltage_f;
            }
        }
    }
}

/**
 * @brief find the mean cell voltage in slave and mean cell voltage in system and assign them ltc global variable
 * @param[in] bms global variable
 * @return none
 */
static void AE_ltcFindMeanCellVoltages(Ltc681x_ts * ltc_s)
{
    float *cellPtr_pf = NULL;
    float meanCellVoltageInSlave_f = 0.0f;
    float meanCellVoltageInSystem_f = 0.0f;

    for(int i = 0; i < SLAVE_NUMBER; i++)
    {
        meanCellVoltageInSlave_f = 0.0f;
        cellPtr_pf = &ltc_s->afeSlaveStatus_s[i].cellVolts_s.cell1_f;

        for(int j = 0; j < ltc_s->afeSlaveStatus_s[i].systemConfig_s.numberOfCell_u8; j++)
        {
            meanCellVoltageInSlave_f += cellPtr_pf[j];
        }

        ltc_s->afeSlaveStatus_s[i].meanCellVoltage_f = meanCellVoltageInSlave_f / (float)ltc_s->afeSlaveStatus_s[i].systemConfig_s.numberOfCell_u8;
        meanCellVoltageInSystem_f += ltc_s->afeSlaveStatus_s[i].meanCellVoltage_f;
    }

    ltc_s->afeSystemStatus.meanCellVoltage_f = meanCellVoltageInSystem_f / (float)SLAVE_NUMBER;
}

/**
 * @brief Start the adc conversion for the GPIO pins
 * @param[in] ltc global variable
 * @param[in] adc speed selection
 * @param[in] gio pin that want to read, to parameter search
 * @return none
 */
void AE_ltcStartGpioAdc(Ltc681x_ts * ltc_s, AdcMode_ts adcMode_s, uint8_t GPIO_)
{
    uint16_t adax = 0x0460;     //!< adax register default value
    uint16_t cmd[4];
    uint16_t pec;

    //set the adcMode sampling speed
    AE_ltcSetAdcSamplingReg(ltc_s, adcMode_s);

    adax |= (adcMode_s & 0x0F) << 7;
    adax |= GPIO_;

    cmd[0] = (adax & 0xFF00) >> 8;
    cmd[1] = (adax & 0x00FF) >> 0;

    pec = AE_pec15((uint8_t*)cmd, 2);
    cmd[2] = (pec & 0xFF00) >> 8;
    cmd[3] = (pec & 0x00FF) >> 0;

    AE_ltcCmdWrite(ltc_s, cmd);
}

/**
 * @brief Read the GPIO1-9 pins voltage
 * @param[in] ltc global variable
 * @param[in] adc sampling speed
 * @param[in] gio pin that want to read, to parameter search @refgroup CHG
 * @param[in] timeout duration in ms
 */
LtcStatus_te AE_ltcReadGpioVoltage(Ltc681x_ts * ltc_s, AdcMode_ts adcMode_s, uint8_t GPIO_, uint32_t timeout)
{
    LtcStatus_te ltcStatus_e;
    float * gpioPtr_pf = NULL;         //to access each index seperately
    uint16_t *  rxBufferPtr_pu16 = NULL;
    uint32_t timeStamp1_u32 = AE_getTick();

    AE_ltcStartGpioAdc(ltc_s, adcMode_s, GPIO_);

    while(!AE_ltcAdcMeasureState(ltc_s))
    {
        if((AE_getTick() - timeStamp1_u32) > timeout) return LTC_TIMEOUT;
    }

    //!<GPIO voltage register Group A
    ltcStatus_e = AE_ltcRead(ltc_s, rxBuffer, cmdRDAUXA_pu16);
    ASSERT_LTC(ltcStatus_e);

    for(int i = 0; i < SLAVE_NUMBER; i++)
    {
        gpioPtr_pf = &ltc_s->afeSlaveStatus_s[i].gioVolts_s.gpio1_f;    // to access the voltage register index by index
        rxBufferPtr_pu16 = &rxBuffer[i * RECEIVE_LEN];

        for(int j = 0; j < 3; j++)
        {   //offset = 0
            gpioPtr_pf[j] = ((rxBufferPtr_pu16[j*2] << 0) | (rxBufferPtr_pu16[j*2 + 1] << 8)) / 10000.0f;  //!< 10000 comes from datasheet
        }
    }

    //!<GPIO voltage register Group B
    ltcStatus_e = AE_ltcRead(ltc_s, rxBuffer, cmdRDAUXB_pu16);
    ASSERT_LTC(ltcStatus_e);

    for(int i = 0; i < SLAVE_NUMBER; i++)
    {
        gpioPtr_pf = &ltc_s->afeSlaveStatus_s[i].gioVolts_s.gpio1_f;    // to access the voltage register index by index
        rxBufferPtr_pu16 = &rxBuffer[i * RECEIVE_LEN];

        for(int j = 0; j < 3; j++)
        {   //offset = 3
            gpioPtr_pf[j + 3] = ((rxBufferPtr_pu16[j*2] << 0) | (rxBufferPtr_pu16[j*2 + 1] << 8)) / 10000.0f;  //!< 10000 comes from datasheet
        }
    }

    //!<GPIO voltage register Group C
    ltcStatus_e = AE_ltcRead(ltc_s, rxBuffer, cmdRDAUXC_pu16);
    ASSERT_LTC(ltcStatus_e);

    for(int i = 0; i < SLAVE_NUMBER; i++)
    {
        gpioPtr_pf = &ltc_s->afeSlaveStatus_s[i].gioVolts_s.gpio1_f;    // to access the voltage register index by index
        rxBufferPtr_pu16 = &rxBuffer[i * RECEIVE_LEN];

        for(int j = 0; j < 3; j++)
        {   //offset = 6
            gpioPtr_pf[j + 6] = ((rxBufferPtr_pu16[j*2] << 0) | (rxBufferPtr_pu16[j*2 + 1] << 8)) / 10000.0f;  //!< 10000 comes from datasheet
        }
    }

    //!<GPIO voltage register Group D
    ltcStatus_e = AE_ltcRead(ltc_s, rxBuffer, cmdRDAUXD_pu16);
    ASSERT_LTC(ltcStatus_e);

    for(int i = 0; i < SLAVE_NUMBER; i++)
    {
        gpioPtr_pf = &ltc_s->afeSlaveStatus_s[i].gioVolts_s.gpio1_f;    // to access the voltage register index by index
        rxBufferPtr_pu16 = &rxBuffer[i * RECEIVE_LEN];

        for(int j = 0; j < 1; j++)
        {   //offset = 9
            gpioPtr_pf[j + 9] = ((rxBufferPtr_pu16[j*2] << 0) | (rxBufferPtr_pu16[j*2 + 1] << 8)) / 10000.0f;  //!< 10000 comes from datasheet
        }
    }

    return ltcStatus_e;
}

/**
 * @brief calculate the NTC temperatures and assign min, max temperatures
 * @param[in] ltc global variable
 * @param[in] NTC number that want to read for parameter search @refgroup NTC_
 * @param[in] ntc connection status for parameters search @refgroup PULL_
 * @return none
 * @note before using this function call the AE_ltcReadGpioVoltage function
 */
void AE_ltcReadTemperature(Ltc681x_ts * ltc_s, uint16_t NTC_ ,uint8_t PULL_)
{
    float * gpioPtr_pf = NULL;    //used to reach gio voltage value index by index
    float * ntcPtr_pf = NULL;     //used to reach ntc temperature value index by index
    float referanceVoltage_f;

    for(int i = 0; i < SLAVE_NUMBER; i++)
    {
        gpioPtr_pf = &ltc_s->afeSlaveStatus_s[i].gioVolts_s.gpio1_f;
        ntcPtr_pf = &ltc_s->afeSlaveStatus_s[i].gioNtcTemperatures_s.ntc1_f;
        referanceVoltage_f = ltc_s->afeSlaveStatus_s[i].gioVolts_s.ref_f;

        for(int j = 0; j < 10; j++) //10 = 9-GPIO voltage + 1-Referance voltage
        {
            if(j < 5)   //referance voltage index is 5 this value must pass
            {
                if((NTC_ >> j) & 0x01)
                {
                    ntcPtr_pf[j] = AE_calculateTemp(gpioPtr_pf[j], referanceVoltage_f, PULL_);
                }
            }
            else if(j > 5)
            {
                if((NTC_ >> (j - 1)) & 0x01)
                {
                    ntcPtr_pf[j - 1] = AE_calculateTemp(gpioPtr_pf[j], referanceVoltage_f, PULL_);
                }
            }
        }
    }

    AE_ltcFindMinTemperature(ltc_s, NTC_);
    AE_ltcFindMaxTemperature(ltc_s, NTC_);
}

/**
 * @brief find the min temperature in slave and system
 * @param[in] ltc global variable
 * @param[in] NTC number that want to read for parameter search @refgroup NTC_
 * @return none
 */
static void AE_ltcFindMinTemperature(Ltc681x_ts * ltc_s, uint16_t NTC_)
{
    float * ntcPtr_pf = NULL;

    for(int i = 0; i < SLAVE_NUMBER; i++)  //
    {
        float minTemperatureInSlave_f = 10000.0f;    //random variable
        ntcPtr_pf = &ltc_s->afeSlaveStatus_s[i].gioNtcTemperatures_s.ntc1_f;

        for(int j = 0; j < (sizeof(GioNtcTemperature_ts) / sizeof(float)); j++)
        {
            if((NTC_ >> j) & 0x01)
            {
                if(ntcPtr_pf[j] < minTemperatureInSlave_f)
                {
                    minTemperatureInSlave_f = ntcPtr_pf[j];
                }
            }
        }
        ltc_s->afeSlaveStatus_s[i].minTemperature_f = minTemperatureInSlave_f;

        if(i == 0)
        {
            ltc_s->afeSystemStatus.minTemperature_f = ltc_s->afeSlaveStatus_s[0].minTemperature_f;
        }
        else
        {
            if(ltc_s->afeSlaveStatus_s[i].minTemperature_f < ltc_s->afeSystemStatus.minTemperature_f)
            {
                ltc_s->afeSystemStatus.minTemperature_f = ltc_s->afeSlaveStatus_s[i].minTemperature_f;
            }
        }
    }
}

/**
 * @brief find the max temperature in slave and system
 * @param[in] ltc global variable
 * @param[in] NTC number that want to read for parameter search @refgroup NTC_
 * @return none
 */
static void AE_ltcFindMaxTemperature(Ltc681x_ts * ltc_s, uint16_t NTC_)
{
    float * ntcPtr_pf = NULL;

    for(int i = 0; i < SLAVE_NUMBER; i++)  //
    {
        float maxTemperatureInSlave_f = -10000.0f;    //random variable
        ntcPtr_pf = &ltc_s->afeSlaveStatus_s[i].gioNtcTemperatures_s.ntc1_f;

        for(int j = 0; j < (sizeof(GioNtcTemperature_ts) / sizeof(float)); j++)
        {
            if((NTC_ >> j) & 0x01)
            {
                if(ntcPtr_pf[j] > maxTemperatureInSlave_f)
                {
                    maxTemperatureInSlave_f = ntcPtr_pf[j];
                }
            }
        }
        ltc_s->afeSlaveStatus_s[i].maxTemperature_f = maxTemperatureInSlave_f;

        if(i == 0)
        {
            ltc_s->afeSystemStatus.maxTemperature_f = ltc_s->afeSlaveStatus_s[0].maxTemperature_f;
        }
        else
        {
            if(ltc_s->afeSlaveStatus_s[i].maxTemperature_f > ltc_s->afeSystemStatus.maxTemperature_f)
            {
                ltc_s->afeSystemStatus.maxTemperature_f = ltc_s->afeSlaveStatus_s[i].maxTemperature_f;
            }
        }
    }
}

/**
 * @brief before reading the status register, this function must be called
 * @param[in] ltc global variable
 * @param[in] adc speed selection
 * @param[in] Status group selection, for parameter search @refgroup CHST
 */
static void AE_ltcStartStatusAdc(Ltc681x_ts * ltc_s, AdcMode_ts adcMode_s, uint8_t CHSTA_)
{
    uint16_t adstat= 0x0468;    //!< ADSTAT base register
    uint16_t cmd[4];
    uint16_t pec;

    AE_ltcSetAdcSamplingReg(ltc_s, adcMode_s);

    adstat |= (adcMode_s & 0x0F) << 7;
    adstat |= CHSTA_;

    cmd[0] = (adstat & 0xFF00) >> 8;
    cmd[1] = (adstat & 0x00FF) >> 0;

    pec = AE_pec15((uint8_t*)cmd, 2);
    cmd[2] = (pec & 0xFF00) >> 8;
    cmd[3] = (pec & 0x00FF) >> 0;

    AE_ltcCmdWrite(ltc_s, cmd);
}

/**
 * @brief read the status registerA(SC : sum of cell, ITMP : internal die temperature, VA : analog power supply voltage)
 * @param[in] ltc global variable
 * @param[in] adc sampling speed
 * @param[in] Status group selection for param searc @refgroup CHSTA_
 */
LtcStatus_te AE_ltcReadStatusRegisterA(Ltc681x_ts * ltc_s, AdcMode_ts mode, uint8_t CHSTA_, uint32_t timeout)
{
    uint32_t timeStamp1_u32 = AE_getTick();
    LtcStatus_te ltcStatus_e;
    uint16_t * rxBufferPtr_pu16 = NULL;
    ltc_s->afeSystemStatus.sumOfSlaveCellVoltages_f = 0;
    ltc_s->afeSystemStatus.maxInternalDieTemperature_f = -1000.0f;

    AE_ltcStartStatusAdc(ltc_s, mode, CHSTA_);

    while(!AE_ltcAdcMeasureState(ltc_s))
    {
        if((AE_getTick() - timeStamp1_u32) > timeout) return LTC_TIMEOUT;
    }

    ltcStatus_e = AE_ltcRead(ltc_s, rxBuffer, cmdRDSTATA_pu16);
    ASSERT_LTC(ltcStatus_e);


    for(int i = 0; i < SLAVE_NUMBER; i++)
    {
        rxBufferPtr_pu16 = &rxBuffer[i * RECEIVE_LEN];

        //read the status registerA value
        ltc_s->afeSlaveStatus_s[i].sumOfCell_f = (rxBufferPtr_pu16[0] | (rxBufferPtr_pu16[1] << 8)) / 10000.0f * 30.0f;
        ltc_s->afeSlaveStatus_s[i].internalDieTemperature_f = (rxBufferPtr_pu16[2] | (rxBufferPtr_pu16[3] << 8)) / 76.0f - 276.0f;
        ltc_s->afeSlaveStatus_s[i].analogPowerSupplyVoltage_f = (rxBufferPtr_pu16[4] | (rxBufferPtr_pu16[5] << 8)) / 10000.0f;

        //assign the total system voltage and max internal die temperature in system
        if(ltc_s->afeSlaveStatus_s[i].internalDieTemperature_f > ltc_s->afeSystemStatus.maxInternalDieTemperature_f)
        {
            ltc_s->afeSystemStatus.maxInternalDieTemperature_f = ltc_s->afeSlaveStatus_s[i].internalDieTemperature_f;
        }
        ltc_s->afeSystemStatus.sumOfSlaveCellVoltages_f += ltc_s->afeSlaveStatus_s[i].sumOfCell_f;
    }

    return ltcStatus_e;
}

/**
 * @brief The diagnostic command DIAGN ensures the proper operation of each multiplexer channel.
 * The command cycles through all channels and sets the MUXFAIL bit to 1 in Status Register Group B
 * if any channel decoder fails. The MUXFAIL bit is set to 0 if the channel decoder passes the test.
 * The MUXFAIL is also set to 1 on power-up (POR) or after a CLRSTAT command.
 * @param[in] ltc global variable
 * @return none
 */
static void AE_ltcStartDiagnAdc(Ltc681x_ts * ltc_s)
{
    uint16_t diagn= 0x715;    //!< ADSTAT base register
    uint16_t cmd[4];
    uint16_t pec;

    cmd[0] = (diagn & 0xFF00) >> 8;
    cmd[1] = (diagn & 0x00FF) >> 0;

    pec = AE_pec15((uint8_t*)cmd, 2);
    cmd[2] = (pec & 0xFF00) >> 8;
    cmd[3] = (pec & 0x00FF) >> 0;

    AE_ltcCmdWrite(ltc_s, cmd);
}

/**
 * @brief read the status register B(VD : digital supply voltage, CxOV : cell overvoltage flags, CxUV : cell underVoltage flags)
 * @param[in] ltc global variable
 * @param[in] adc sampling speeed
 * @param[in] Status group selection for param searc @refgroup CHSTB_
 */
LtcStatus_te AE_ltcReadStatusRegisterB(Ltc681x_ts * ltc_s, AdcMode_ts adcMode_s, uint8_t CHSTB_, uint32_t timeout)
{
    uint32_t timeStamp1_u32 = AE_getTick();
    LtcStatus_te ltcStatus_e;
    uint16_t * rxBufferPtr_pu16 = NULL;

    //start diagnostic adc reading to check system stability
    AE_ltcStartDiagnAdc(ltc_s);
    while(!AE_ltcAdcMeasureState(ltc_s))
    {
        if((AE_getTick() - timeStamp1_u32) > timeout) return LTC_TIMEOUT;
    }

    //start status register B adc
    AE_ltcStartStatusAdc(ltc_s, adcMode_s, CHSTB_);
    while(!AE_ltcAdcMeasureState(ltc_s))
    {
        if((AE_getTick() - timeStamp1_u32) > timeout) return LTC_TIMEOUT;
    }


    ltcStatus_e = AE_ltcRead(ltc_s, rxBuffer, cmdRDSTATB_pu16);
    ASSERT_LTC(ltcStatus_e);

    for(int i = 0; i < SLAVE_NUMBER; i++)
    {
        rxBufferPtr_pu16 = &rxBuffer[i * RECEIVE_LEN];
        ltc_s->afeSlaveStatus_s[i].afeRegisters_s.stBr_s = *((STBR_ts*)rxBufferPtr_pu16);

        ltc_s->afeSlaveStatus_s[i].digitalPowerSuppyVoltage_f = (rxBufferPtr_pu16[0] << 8 | rxBufferPtr_pu16[1])/10000.0f;
        ltc_s->afeSlaveStatus_s[i].thermalShutdownOccured_b = rxBufferPtr_pu16[5] & 0x01;
        ltc_s->afeSlaveStatus_s[i].muxFail_b = (rxBufferPtr_pu16[5] >> 1) & 0x01;

        if(ltc_s->afeSlaveStatus_s[i].thermalShutdownOccured_b)
        {
            ltc_s->afeSystemStatus.thermalShutdownOccured_b = true;
        }
        if(ltc_s->afeSlaveStatus_s[i].muxFail_b)
        {
            ltc_s->afeSystemStatus.muxFail_b = true;
        }
    }

    return ltcStatus_e;
}

/**
 * @brief check the system balance status according to mean voltage, temperature, ...
 * @param[in] ltc global variable
 * @return balance status of the system
 */
BalanceStatus_te AE_ltcCheckBalanceStatus(Ltc681x_ts * ltc_s)
{
    if(ltc_s->afeSystemStatus.minCellVoltage_f < ltc_s->afeSystemStatus.balanceVoltageLowerLimit_f)
    {
        return OUT_OF_BALANCE_VOLTAGE;
    }
    else if(ltc_s->afeSystemStatus.maxTemperature_f > ltc_s->afeSystemStatus.balanceTemperatureUpperLimit_u16)
    {
        return OUT_OF_BALANCE_TEMPERATURE;
    }
    else if(ltc_s->afeSystemStatus.minMaxVoltageDifferenceInMah_f > ltc_s->afeSystemStatus.balanceThresholdInMah_f)
    {
        return BALANCE_NEEDED;
    }

    return IN_BALANCE;
}

void AE_ltcBalanceOperation(Ltc681x_ts * ltc_s)
{
    BalanceStatus_te balanceInput_e = AE_ltcCheckBalanceStatus(ltc_s);

    switch(balanceInput_e)
    {
        case(IN_BALANCE):
        {
            if(ltc_s->afeSystemStatus.balanceStatus_e == BALANCING)
            {
                AE_ltcStopBalance(ltc_s);
            }
            ltc_s->afeSystemStatus.balanceThresholdInMah_f = ltc_s->afeSystemStatus.balanceUpperThresholdInMah_f;
            ltc_s->afeSystemStatus.balanceStatus_e = IN_BALANCE;
            break;
        }
        case(OUT_OF_BALANCE_TEMPERATURE):
        {
            if(ltc_s->afeSystemStatus.balanceStatus_e == BALANCING)
            {
                AE_ltcStopBalance(ltc_s);
            }
            ltc_s->afeSystemStatus.balanceStatus_e = OUT_OF_BALANCE_TEMPERATURE;
            break;
        }
        case(OUT_OF_BALANCE_VOLTAGE):
        {
            if(ltc_s->afeSystemStatus.balanceStatus_e == BALANCING)
            {
                AE_ltcStopBalance(ltc_s);
            }
            ltc_s->afeSystemStatus.balanceStatus_e = OUT_OF_BALANCE_VOLTAGE;
            break;
        }
        case(BALANCE_NEEDED):
        {
            ltc_s->afeSystemStatus.balanceThresholdInMah_f = ltc_s->afeSystemStatus.balanceLowerThresholdInMah_f;
            AE_ltcBalance(ltc_s);
            ltc_s->afeSystemStatus.balanceStatus_e = BALANCING;
            break;
        }
    }
}

/**
 * @brief close the balance cell
 * @param[in] ltc global variable
 * @return none
 */
void AE_ltcStopBalance(Ltc681x_ts * ltc_s)
{
    //clear the DCC pins
    for(int i = 0; i < SLAVE_NUMBER; i++)
    {
        ltc_s->afeSlaveStatus_s[i].afeRegisters_s.cfgAr_s.CFGAR4_u16 &= ~(0xFF);
        ltc_s->afeSlaveStatus_s[i].afeRegisters_s.cfgAr_s.CFGAR5_u16 &= ~(0x0F);
        ltc_s->afeSlaveStatus_s[i].afeRegisters_s.cfgBr_s.CFGBR0_u16 &= ~(0x70);
    }

    AE_ltcWrite(ltc_s, (uint16_t*)&ltc_s->afeSlaveStatus_s[0].afeRegisters_s.cfgAr_s, cmdWRCFGA_pu16);
    AE_ltcWrite(ltc_s, (uint16_t*)&ltc_s->afeSlaveStatus_s[0].afeRegisters_s.cfgBr_s, cmdWRCFGB_pu16);

    ltc_s->afeSystemStatus.balanceStatus_e = BALANCE_STOPPED;
}

/**
 * @brief balance the cell that voltage is upper than mean voltage
 * @param[in] ltc global variable
 * @return none
 */
void AE_ltcBalance(Ltc681x_ts * ltc_s)
{
    float * cellPtr_pf = NULL;

    //clear the DCC pins
    for(int i = 0; i < SLAVE_NUMBER; i++)
    {
        ltc_s->afeSlaveStatus_s[i].afeRegisters_s.cfgAr_s.CFGAR4_u16 &= ~(0xFF);
        ltc_s->afeSlaveStatus_s[i].afeRegisters_s.cfgAr_s.CFGAR5_u16 &= ~(0x0F);
        ltc_s->afeSlaveStatus_s[i].afeRegisters_s.cfgBr_s.CFGBR0_u16 &= ~(0x70);
    }

    //take the cell that is bigger than mean voltage and enable DCC registers
    for(int i = 0; i < SLAVE_NUMBER; i++)
    {
        cellPtr_pf = &ltc_s->afeSlaveStatus_s[i].cellVolts_s.cell1_f;
        ltc_s->afeSlaveStatus_s[i].maskedDcc_u16 = 0;

        for(int j = 0; j < ltc_s->afeSlaveStatus_s[i].systemConfig_s.numberOfCell_u8; j++)
        {
            if(cellPtr_pf[j] > ltc_s->afeSystemStatus.meanCellVoltage_f)
            {
                ltc_s->afeSlaveStatus_s[i].maskedDcc_u16 |= 1 << j;
            }
        }

        ltc_s->afeSlaveStatus_s[i].afeRegisters_s.cfgAr_s.CFGAR4_u16 |= (ltc_s->afeSlaveStatus_s[i].maskedDcc_u16 >> 0) & 0xFF;
        ltc_s->afeSlaveStatus_s[i].afeRegisters_s.cfgAr_s.CFGAR5_u16 |= (ltc_s->afeSlaveStatus_s[i].maskedDcc_u16 >> 8) & 0x0F;
        ltc_s->afeSlaveStatus_s[i].afeRegisters_s.cfgBr_s.CFGBR0_u16 |= (ltc_s->afeSlaveStatus_s[i].maskedDcc_u16 >> 8) & 0xF0;
    }

    AE_ltcWrite(ltc_s, (uint16_t*)&ltc_s->afeSlaveStatus_s[0].afeRegisters_s.cfgAr_s, cmdWRCFGA_pu16);
    AE_ltcWrite(ltc_s, (uint16_t*)&ltc_s->afeSlaveStatus_s[0].afeRegisters_s.cfgBr_s, cmdWRCFGB_pu16);
}








