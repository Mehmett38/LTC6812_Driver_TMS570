/*
 * ltc681x.h
 *
 *  Created on: 10 Haz 2024
 *      Author: mehmet.dincer
 */

#ifndef API_USER_DEFINED_API_LTC6812_DRIVERLAYER_LTC681X_H_
#define API_USER_DEFINED_API_LTC6812_DRIVERLAYER_LTC681X_H_

#include <API/User Defined API/ASP/tms_asp.h>
#include <API/User Defined API/LTC6812/DriverLayer/crc/crc15.h>
#include <API/User Defined API/LTC6812/DriverLayer/temperature/temperature.h>
#include "spi.h"

//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<-MACROS->>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
#define SLAVE_NUMBER                    (7)
#define LTC_WATCHDOG_TIME               (2000) //2s watchdog time from datasheet
#define THERMAL_SHUTDOWN_TEMPERATURE    (130)  //If the temperature detected on the die goes above approximately 150°C,
                                               //the thermal shutdown circuit trips and resets the Configuration Register Groups
#define OPEN_WIRE_CELL_VOLTAGE          (0.100f)
//=================================REGISTER_MACROS=============================
#define ADCOPT_OFFSET                   (0u)
#define DTEN_OFFSET                     (1u)
#define REFON_OFFSET                    (2u)
#define GPIO1_OFFSET                    (3u)
#define DTMEN_OFFSET                    (3u)
#define DCTO_OFFSET                     (4u)

#define WRITE_BYTES_IN_REG              (6)
#define READ_BYTES_IN_REG               (8)
#define CMD_LEN                         (4)
#define ENABLE                          (1)
#define DISABLE                         (0)
#define GPIO_A_RESET                    (0xF8)
#define GPIO_B_RESET                    (0x0F)
#define CMD_LEN                         (4)         //cmd packet len 4byte
#define REGISTER_LEN                    (6)
#define TRANSMIT_LEN                    (8)
#define RECEIVE_LEN                     (8)
#define CELL_NUMBER                     (15)
#define GPIO_NUMBER                     (9)
#define OPEN_WIRE_VOLTAGE               (-0.150f)   //in datasheet write -0.400 but that value is not supply the open wire situation


//!< @refgroup GPIOA
#define GPIO_1_PULL_DOWN_ON             (0 << 0)
#define GPIO_2_PULL_DOWN_ON             (0 << 1)
#define GPIO_3_PULL_DOWN_ON             (0 << 2)
#define GPIO_4_PULL_DOWN_ON             (0 << 3)
#define GPIO_5_PULL_DOWN_ON             (0 << 4)
#define GPIO_6_PULL_DOWN_ON             (0 << 5)
#define GPIO_7_PULL_DOWN_ON             (0 << 6)
#define GPIO_8_PULL_DOWN_ON             (0 << 7)
#define GPIO_9_PULL_DOWN_ON             (0 << 8)
#define ALL_GPIO_PULL_DOWN_ON           (GPIO_1_PULL_DOWN_ON | GPIO_2_PULL_DOWN_ON | GPIO_3_PULL_DOWN_ON |  \
                                         GPIO_4_PULL_DOWN_ON | GPIO_5_PULL_DOWN_ON | GPIO_6_PULL_DOWN_ON |  \
                                         GPIO_7_PULL_DOWN_ON | GPIO_8_PULL_DOWN_ON | GPIO_9_PULL_DOWN_ON)

#define GPIO_1_PULL_DOWN_OFF            (1 << 0)
#define GPIO_2_PULL_DOWN_OFF            (1 << 1)
#define GPIO_3_PULL_DOWN_OFF            (1 << 2)
#define GPIO_4_PULL_DOWN_OFF            (1 << 3)
#define GPIO_5_PULL_DOWN_OFF            (1 << 4)
#define GPIO_6_PULL_DOWN_OFF            (1 << 5)
#define GPIO_7_PULL_DOWN_OFF            (1 << 6)
#define GPIO_8_PULL_DOWN_OFF            (1 << 7)
#define GPIO_9_PULL_DOWN_OFF            (1 << 8)
#define ALL_GPIO_PULL_DOWN_OFF          (GPIO_1_PULL_DOWN_OFF | GPIO_2_PULL_DOWN_OFF | GPIO_3_PULL_DOWN_OFF |  \
                                         GPIO_4_PULL_DOWN_OFF | GPIO_5_PULL_DOWN_OFF | GPIO_6_PULL_DOWN_OFF |  \
                                         GPIO_7_PULL_DOWN_OFF | GPIO_8_PULL_DOWN_OFF | GPIO_9_PULL_DOWN_OFF)

//!< @refgroup NTC_
#define NTC_1                           (1 << 0)
#define NTC_2                           (1 << 1)
#define NTC_3                           (1 << 2)
#define NTC_4                           (1 << 3)
#define NTC_5                           (1 << 4)
#define NTC_6                           (1 << 5)
#define NTC_7                           (1 << 6)
#define NTC_8                           (1 << 7)
#define NTC_9                           (1 << 8)

//!< @refgroup cell
#define CELL_1                          (1 << 0 )
#define CELL_2                          (1 << 1 )
#define CELL_3                          (1 << 2 )
#define CELL_4                          (1 << 3 )
#define CELL_5                          (1 << 4 )
#define CELL_6                          (1 << 5 )
#define CELL_7                          (1 << 6 )
#define CELL_8                          (1 << 7 )
#define CELL_9                          (1 << 8 )
#define CELL_10                         (1 << 9 )
#define CELL_11                         (1 << 10)
#define CELL_12                         (1 << 11)
#define CELL_13                         (1 << 12)
#define CELL_14                         (1 << 13)
#define CELL_15                         (1 << 14)

//!< @refgroup dcc
#define DCC_0                           (1 << 0)
#define DCC_1                           (1 << 1)
#define DCC_2                           (1 << 2)
#define DCC_3                           (1 << 3)
#define DCC_4                           (1 << 4)
#define DCC_5                           (1 << 5)
#define DCC_6                           (1 << 6)
#define DCC_7                           (1 << 7)
#define DCC_8                           (1 << 8)
#define DCC_9                           (1 << 9)
#define DCC_10                          (1 << 10)
#define DCC_11                          (1 << 11)
#define DCC_12                          (1 << 12)
#define DCC_13                          (1 << 13)
#define DCC_14                          (1 << 14)
#define DCC_15                          (1 << 15)
#define DCC_ALL                         (0xFFFF)

//!< @refgroup CH
#define CELL_ALL                        (0x00)
#define CELL_1_6_11                     (0x01)
#define CELL_2_7_12                     (0x02)
#define CELL_3_8_13                     (0x03)
#define CELL_4_9_14                     (0x04)
#define CELL_5_10_15                    (0x05)

//!< @refgroup CHG
#define GPIO_ALL                        (0x00)
#define GPIO_1_6                        (0x01)
#define GPIO_2_7                        (0x02)
#define GPIO_3_8                        (0x03)
#define GPIO_4_9                        (0x04)
#define GPIO_5_0                        (0x05)
#define GPIO_2ND_REFERANCE              (0x06)

//!< @refgroup CHSTA_
#define CHSTA_ALL                        (0x00)
#define CHSTA_SC                         (0x01)      //!< Sum of all cell
#define CHSTA_ITMP                       (0x02)      //!< Internal die temperature
#define CHSTA_VA                         (0x03)      //!< Analog power supply voltage

//!< @refgroup CHSTB_
#define CHSTB_ALL                        (0x00)
#define CHSTB_SC                         (0x01)      //!< Sum of all cell
#define CHSTB_ITMP                       (0x02)      //!< Internal die temperature
#define CHSTB_VA                         (0x03)      //!< Analog power supply voltage

//!<spi cs pin numbers
#define FRAM_CS_PIN_0                       (0u)
#define FRAM_CS_PIN_1                       (1u)
#define FRAM_CS_PIN_2                       (2u)
#define FRAM_CS_PIN_4                       (3u)
#define FRAM_CS_PIN_5                       (4u)
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<-FUNCTION MACROS->>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
#define AE_LTC_CS_ON(ltc)                  (ltc->spi_s.spiReg_s->PC3 &= ~(1 << 0)) //drive low the cs0 pin
#define AE_LTC_CS_OFF(ltc)                 (ltc->spi_s.spiReg_s->PC3 |= 1 << 0)    //drive hight the cs0 pins
#define AE_LTC_WATCHDOG_RESET(ltc)         (ltc->watchdogTime_u32 = 0)
#define ASSERT_LTC(status)                 if(status != LTC_OK) return status
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<-TYPE DEFINITIONS->>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<-ENUMS->>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
typedef enum{
    IN_BALANCE,
    BALANCE_NEEDED,
    BALANCING,
    BALANCE_STOPPED,
    OUT_OF_BALANCE_TEMPERATURE,
    OUT_OF_BALANCE_VOLTAGE
}BalanceStatus_te;

typedef enum{
    LTC_OK,
    LTC_WRONG_CRC,
    LTC_TIMEOUT,
    LTC_MUXFAIL,
    LTC_CELL_OPEN_WIRE,
    LTC_THERMAL_SHUTDOWN_WARNING,
    LTC_THERMAL_SHUTDOWN_OCCURED,
}LtcStatus_te;

typedef enum{
    MODE_422HZ = 0x00,  // ADCOPT = 0
    MODE_1KHZ = 0x10,   // ADCOPT = 1
    MODE_27KHZ = 0x01,  // ADCOPT = 0
    MODE_14KHZ = 0x11,  // ADCOPT = 1
    MODE_7KHZ = 0x02,
    MODE_3KHZ = 0x12,
    MODE_26HZ = 0x03,
    MODE_2KHZ = 0x13
}AdcMode_ts;

//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<-STRUCTURES->>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
//=================================registers================================
//!<Configuration Register Group A
typedef struct{
    uint16_t CFGAR0_u16;            //!<GPIO1-5(:5), REFON(:1), DTEN(:1), ADCOPT(:1)
    uint16_t CFGAR1_u16;            //!<VUV
    uint16_t CFGAR2_u16;            //!<VOV(:4), VUV(:4)
    uint16_t CFGAR3_u16;            //!<VOV
    uint16_t CFGAR4_u16;            //!<DCC1-8
    uint16_t CFGAR5_u16;            //!<DCTO0-4(:4), DCC9-12(:4)
}CFGAR_ts;

//!<Configuration Register Group B
typedef struct{
    uint16_t CFGBR0_u16;            //!<RSVD(:1), DCC13-15(:3), GPIO6-9(:4)
    uint16_t CFGBR1_u16;            //!<MUTE(:1), FDRF(:1), PS0-1(:2), DTMEN(:1), DCC0(:1), RSVD(:2)
    uint16_t CFGBR2_u16;            //!<RSVD
    uint16_t CFGBR3_u16;            //!<RSVD
    uint16_t CFGBR4_u16;            //!<RSVD
    uint16_t CFGBR5_u16;            //!<RSVD
}CFGBR_ts;

//!<Cell Voltage Register Group A
typedef struct{
    uint16_t CVAR0_u16;             //!<C1V0-7
    uint16_t CVAR1_u16;             //!<C1V8-15
    uint16_t CVAR2_u16;             //!<C2V0-7
    uint16_t CVAR3_u16;             //!<C2V8-15
    uint16_t CVAR4_u16;             //!<C3V0-7
    uint16_t CVAR5_u16;             //!<C3V8-15
}CVAR_ts;

//!<Cell Voltage Register Group B
typedef struct{
    uint16_t CVBR0_u16;             //!<C4V0-7
    uint16_t CVBR1_u16;             //!<C4V8-15
    uint16_t CVBR2_u16;             //!<C5V0-7
    uint16_t CVBR3_u16;             //!<C5V8-15
    uint16_t CVBR4_u16;             //!<C6V0-7
    uint16_t CVBR5_u16;             //!<C6V8-15
}CVBR_ts;

//!<Cell Voltage Register Group C
typedef struct{
    uint16_t CVCR0_u16;             //!<C7V0-7
    uint16_t CVCR1_u16;             //!<C7V8-15
    uint16_t CVCR2_u16;             //!<C8V0-7
    uint16_t CVCR3_u16;             //!<C8V8-15
    uint16_t CVCR4_u16;             //!<C9V0-7
    uint16_t CVCR5_u16;             //!<C9V8-15
}CVCR_ts;

//!<Cell Voltage Register Group D
typedef struct{
    uint16_t CVDR0_u16;             //!<C10V0-7
    uint16_t CVDR1_u16;             //!<C10V8-15
    uint16_t CVDR2_u16;             //!<C11V0-7
    uint16_t CVDR3_u16;             //!<C11V8-15
    uint16_t CVDR4_u16;             //!<C12V0-7
    uint16_t CVDR5_u16;             //!<C12V8-15
}CVDR_ts;

//!<Cell Voltage Register Group E
typedef struct{
    uint16_t CVER0_u16;             //!<C13V0-7
    uint16_t CVER1_u16;             //!<C13V8-15
    uint16_t CVER2_u16;             //!<C14V0-7
    uint16_t CVER3_u16;             //!<C14V8-15
    uint16_t CVER4_u16;             //!<C15V0-7
    uint16_t CVER5_u16;             //!<C15V8-15
}CVER_ts;

//!<Auxilary Register Group A
typedef struct{
    uint16_t AVAR0_u16;             //!<G1V0-7
    uint16_t AVAR1_u16;             //!<G1V8-15
    uint16_t AVAR2_u16;             //!<G2V0-7
    uint16_t AVAR3_u16;             //!<G2V8-15
    uint16_t AVAR4_u16;             //!<G3V0-7
    uint16_t AVAR5_u16;             //!<G3V8-15
}AVAR_ts;

//!<Auxilary Register Group B
typedef struct{
    uint16_t AVBR0_u16;             //!<G4V0-7
    uint16_t AVBR1_u16;             //!<G4V8-15
    uint16_t AVBR2_u16;             //!<G5V0-7
    uint16_t AVBR3_u16;             //!<G5V8-15
    uint16_t AVBR4_u16;             //!<REF0-7
    uint16_t AVBR5_u16;             //!<REF8-15
}AVBR_ts;

//!<Auxilary Register Group C
typedef struct{
    uint16_t AVCR0_u16;             //!<G6V0-7
    uint16_t AVCR1_u16;             //!<G6V8-15
    uint16_t AVCR2_u16;             //!<G7V0-7
    uint16_t AVCR3_u16;             //!<G7V8-15
    uint16_t AVCR4_u16;             //!<G8V0-7
    uint16_t AVCR5_u16;             //!<G8V8-15
}AVCR_ts;

//!<Auxilary Register Group D
typedef struct{
    uint16_t AVDR0_u16;             //!<G9V0-7
    uint16_t AVDR1_u16;             //!<G9V8-15
    uint16_t AVDR2_u16;             //!<RSVD
    uint16_t AVDR3_u16;             //!<RSVD
    uint16_t AVDR4_u16;             //!<RSVD
    uint16_t AVDR5_u16;             //!<RSVD
}AVDR_ts;

//!<Status Register Group A
typedef struct{
    uint16_t STAR0_u16;             //!<SC0-7
    uint16_t STAR1_u16;             //!<SC8-15
    uint16_t STAR2_u16;             //!<ITMP0-7
    uint16_t STAR3_u16;             //!<ITMP8-15
    uint16_t STAR4_u16;             //!<VA0-7
    uint16_t STAR5_u16;             //!<VA8-15
}STAR_ts;

//!<Status Register Group B
typedef struct{
    uint16_t STBR0_u16;             //!<VD0-7
    uint16_t STBR1_u16;             //!<VD8-15
    uint16_t STBR2_u16;             //!<
    uint16_t STBR3_u16;             //!<
    uint16_t STBR4_u16;             //!<
    uint16_t STBR5_u16;             //!<
}STBR_ts;

//!<Collection of the registers
typedef struct{
    volatile CFGAR_ts cfgAr_s;
    volatile CFGBR_ts cfgBr_s;
    volatile CVAR_ts cvAr_s;
    volatile CVBR_ts cvBr_s;
    volatile CVCR_ts cvCr_s;
    volatile CVDR_ts cvDr_s;
    volatile CVER_ts cvEr_s;
    volatile AVAR_ts avAr_s;
    volatile AVBR_ts avBr_s;
    volatile AVCR_ts avCr_s;
    volatile AVDR_ts avDr_s;
    volatile STAR_ts stAr_s;
    volatile STBR_ts stBr_s;
}AfeRegisters_ts;

//=============================================================================
typedef struct{
    float cell1_f;
    float cell2_f;
    float cell3_f;
    float cell4_f;
    float cell5_f;
    float cell6_f;
    float cell7_f;
    float cell8_f;
    float cell9_f;
    float cell10_f;
    float cell11_f;
    float cell12_f;
    float cell13_f;
    float cell14_f;
    float cell15_f;
 }CellVolt_ts;

 typedef struct{
     float gpio1_f;
     float gpio2_f;
     float gpio3_f;
     float gpio4_f;
     float gpio5_f;
     float ref_f;
     float gpio6_f;
     float gpio7_f;
     float gpio8_f;
     float gpio9_f;
 }GpioVolt_ts;

 typedef struct{
     float ntc1_f;
     float ntc2_f;
     float ntc3_f;
     float ntc4_f;
     float ntc5_f;
     float ntc6_f;
     float ntc7_f;
     float ntc8_f;
     float ntc9_f;
 }GioNtcTemperature_ts;

 typedef struct{
     uint8_t numberOfCell_u8;
     bool adcopt_b;
     bool refon_b;
     bool dischargeTimeMonitor_b;
     uint16_t gioPullStatus;
 }SystemConfig_ts;

 typedef struct{


 }StatusRegisterA_ts;

 //=============================================================================
 typedef struct{
     AfeRegisters_ts afeRegisters_s;
     SystemConfig_ts systemConfig_s;
     GioNtcTemperature_ts gioNtcTemperatures_s;
     CellVolt_ts cellVolts_s;
     GpioVolt_ts gioVolts_s;
     float sumOfCell_f;
     float internalDieTemperature_f;
     float analogPowerSupplyVoltage_f;
     float digitalPowerSuppyVoltage_f;
     float minCellVoltage_f;
     float maxCellVoltage_f;
     float meanCellVoltage_f;
     float minTemperature_f;
     float maxTemperature_f;
     bool thermalShutdownOccured_b;
     bool muxFail_b;
     uint16_t underVoltageFlags_u16;
     uint16_t overVoltageFlags_u16;
     uint16_t cellOverVoltageflag_u16;
     uint16_t cellUnderVoltageflag_u16;
     uint16_t maskedDcc_u16;
 }AfeSlaveStatus_ts;

 typedef struct{
     float sumOfSlaveCellVoltages_f;
     float minCellVoltage_f;
     float maxCellVoltage_f;
     float meanCellVoltage_f;
     float minTemperature_f;
     float maxTemperature_f;
     float maxInternalDieTemperature_f;
     float minMaxVoltageDifferenceInMah_f;
     float balanceUpperThresholdInMah_f;
     float balanceLowerThresholdInMah_f;
     float balanceThresholdInMah_f;
     float balanceVoltageLowerLimit_f;
     uint16_t balanceTemperatureUpperLimit_u16;
     bool thermalShutdownOccured_b;
     bool muxFail_b;
     BalanceStatus_te balanceStatus_e;
 }AfeSystemStatus_ts;

 typedef struct{
     spiDAT1_t spiConfig_s;           //spi config addres
     spiBASE_t * spiReg_s;              //spi register addres
 }Spi_ts;

//=============================================================================
typedef struct{
    AfeSlaveStatus_ts afeSlaveStatus_s[SLAVE_NUMBER];
    AfeSystemStatus_ts afeSystemStatus;
    Spi_ts spi_s;
    uint32_t watchdogTime_u32;
}Ltc681x_ts;

//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<-GLOBAL VARIABLES->>>>>>>>>>>>>>>>>>>>>>>>>>>>>
static uint16_t cmdWRCFGA_pu16[4]   = {0x00, 0x01, 0x3D, 0x6E}; // last 2 index CRC
static uint16_t cmdWRCFGB_pu16[4]   = {0x00, 0x24, 0xB1, 0x9E};
static uint16_t cmdRDCFGA_pu16[4]   = {0x00, 0x02, 0x2B, 0x0A};
static uint16_t cmdRDCFGB_pu16[4]   = {0x00, 0x26, 0x2C, 0xC8};
static uint16_t cmdRDCVA_pu16[4]    = {0x00, 0x04, 0x07, 0xC2};
static uint16_t cmdRDCVB_pu16[4]    = {0x00, 0x06, 0x9A, 0x94};
static uint16_t cmdRDCVC_pu16[4]    = {0x00, 0x08, 0x5E, 0x52};
static uint16_t cmdRDCVD_pu16[4]    = {0x00, 0x0A, 0xC3, 0x04};
static uint16_t cmdRDCVE_pu16[4]    = {0x00, 0x09, 0xD5, 0x60};
static uint16_t cmdRDAUXA_pu16[4]   = {0x00, 0x0C, 0xEF, 0xCC};
static uint16_t cmdRDAUXB_pu16[4]   = {0x00, 0x0E, 0x72, 0x9A};
static uint16_t cmdRDAUXC_pu16[4]   = {0x00, 0x0D, 0x64, 0xFE};
static uint16_t cmdRDAUXD_pu16[4]   = {0x00, 0x0F, 0xF9, 0xA8};
static uint16_t cmdRDSTATA_pu16[4]  = {0x00, 0x10, 0xED, 0x72};
static uint16_t cmdRDSTATB_pu16[4]  = {0x00, 0x12, 0x70, 0x24};
static uint16_t cmdWRSCTRL_pu16[4]  = {0x00, 0x14, 0x5C, 0xEC};
static uint16_t cmdWRPWM_pu16[4]    = {0x00, 0x20, 0x00, 0x00};
static uint16_t cmdWRPSB_pu16[4]    = {0x00, 0x1C, 0xB4, 0xE2};
static uint16_t cmdRDSCTRL_pu16[4]  = {0x00, 0x16, 0xC1, 0xBA};
static uint16_t cmdRDPWM_pu16[4]    = {0x00, 0x22, 0x9D, 0x56};
static uint16_t cmdRDPSB_pu16[4]    = {0x00, 0x1E, 0x29, 0xB4};
static uint16_t cmdSTSCTRL_pu16[4]  = {0x00, 0x19, 0x8E, 0x4E};
static uint16_t cmdCLRSCTRL_pu16[4] = {0x00, 0x18, 0x05, 0x7C};
static uint16_t cmdCLRCELL_pu16[4]  = {0x07, 0x11, 0xC9, 0xC0};
static uint16_t cmdCLRAUX_pu16[4]   = {0x07, 0x12, 0xDF, 0xA4};
static uint16_t cmdCLRSTAT_pu16[4]  = {0x07, 0x13, 0x54, 0x96};
static uint16_t cmdMute_pu16[4]     = {0x00, 0x28, 0xE8, 0x0E};
static uint16_t cmdUnMute_pu16[4]   = {0x00, 0x29, 0x63, 0x3C};
static uint16_t cmdPladc_pu16[4]    = {0x07, 0x14, 0xF3, 0x6C};
static uint16_t cmdClrCell_pu16[4]  = {0x07, 0x11, 0xC9, 0xC0};
static uint16_t cmdAdstat_pu16[4]   = {0x04, 0x68, 0xB3, 0xE2};

//<<<<<<<<<<<<<<<<<<<<<<<<<-FUNCTION PROTOTYPES->>>>>>>>>>>>>>>>>>>>>>>>>>
void AE_ltcInit(Ltc681x_ts * ltc_s);
void AE_ltcWatchdogCallback(Ltc681x_ts * ltc_s);
LtcStatus_te AE_ltcReadCellVoltage(Ltc681x_ts * ltc_s, AdcMode_ts mode, uint8_t dischargePermit, uint8_t CELL_, uint32_t timeout);
LtcStatus_te AE_ltcReadGpioVoltage(Ltc681x_ts * ltc_s, AdcMode_ts mode, uint8_t GPIO_, uint32_t timeout);
void AE_ltcReadTemperature(Ltc681x_ts * ltc_s, uint16_t NTC_ ,uint8_t PULL_);
LtcStatus_te AE_ltcReadStatusRegisterA(Ltc681x_ts * ltc_s, AdcMode_ts mode, uint8_t CHSTA_, uint32_t timeout);
LtcStatus_te AE_ltcReadStatusRegisterB(Ltc681x_ts * ltc_s, AdcMode_ts adcMode_s, uint8_t CHSTB_, uint32_t timeout);
void AE_ltcStopBalance(Ltc681x_ts * ltc_s);
void AE_ltcBalanceOperation(Ltc681x_ts * ltc_s);

#endif /* API_USER_DEFINED_API_LTC6812_DRIVERLAYER_LTC681X_H_ */
