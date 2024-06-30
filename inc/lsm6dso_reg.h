/*
 ******************************************************************************
 * @file    lsm6dso_reg.h
 * @author  Sensors Software Solution Team
 * @brief   This file contains all the functions prototypes for the
 *          lsm6dso_reg.c driver.
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by ST under BSD 3-Clause license,
 * the "License"; You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *                        opensource.org/licenses/BSD-3-Clause
 *
 ******************************************************************************
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef LSM6DSO_REGS_H
#define LSM6DSO_REGS_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include <stddef.h>
#include <math.h>

/** I2C Device Address 8 bit format  if SA0=0 -> D5 if SA0=1 -> D7 **/
#define LSM6DSO_I2C_ADD_L 0xD5
#define LSM6DSO_I2C_ADD_H 0xD7

#define LSM6DSO_ID 0x6C

#define LSM6DSO_FUNC_CFG_ACCESS 0x01U
#define LSM6DSO_PIN_CTRL 0x02U

#define LSM6DSO_FIFO_CTRL1 0x07U
#define LSM6DSO_FIFO_CTRL2 0x08U
#define LSM6DSO_FIFO_CTRL3 0x09U
#define LSM6DSO_FIFO_CTRL4 0x0AU

#define LSM6DSO_COUNTER_BDR_REG1 0x0BU
#define LSM6DSO_COUNTER_BDR_REG2 0x0CU

#define LSM6DSO_INT1_CTRL 0x0D
#define LSM6DSO_INT2_CTRL 0x0EU

#define LSM6DSO_WHO_AM_I 0x0FU

#define LSM6DSO_CTRL1_XL 0x10U
#define LSM6DSO_CTRL2_G 0x11U
#define LSM6DSO_CTRL3_C 0x12U
#define LSM6DSO_CTRL4_C 0x13U
#define LSM6DSO_CTRL5_C 0x14U
#define LSM6DSO_CTRL6_C 0x15U
#define LSM6DSO_CTRL7_G 0x16U
#define LSM6DSO_CTRL8_XL 0x17U
#define LSM6DSO_CTRL9_XL 0x18U
#define LSM6DSO_CTRL10_C 0x19U

#define LSM6DSO_ALL_INT_SRC 0x1AU
#define LSM6DSO_WAKE_UP_SRC 0x1BU

#define LSM6DSO_TAP_SRC 0x1CU
#define LSM6DSO_D6D_SRC 0x1DU

#define LSM6DSO_STATUS_REG 0x1EU
#define LSM6DSO_STATUS_SPIAUX 0x1EU

#define LSM6DSO_OUT_TEMP_L 0x20U
#define LSM6DSO_OUT_TEMP_H 0x21U

#define LSM6DSO_OUTX_L_G 0x22U
#define LSM6DSO_OUTX_H_G 0x23U
#define LSM6DSO_OUTY_L_G 0x24U
#define LSM6DSO_OUTY_H_G 0x25U
#define LSM6DSO_OUTZ_L_G 0x26U
#define LSM6DSO_OUTZ_H_G 0x27U

#define LSM6DSO_OUTX_L_A 0x28U
#define LSM6DSO_OUTX_H_A 0x29U
#define LSM6DSO_OUTY_L_A 0x2AU
#define LSM6DSO_OUTY_H_A 0x2BU
#define LSM6DSO_OUTZ_L_A 0x2CU
#define LSM6DSO_OUTZ_H_A 0x2DU

#define LSM6DSO_EMB_FUNC_STATUS_MAINPAGE 0x35U
#define LSM6DSO_FSM_STATUS_A_MAINPAGE 0x36U
#define LSM6DSO_FSM_STATUS_B_MAINPAGE 0x37U

#define LSM6DSO_STATUS_MASTER_MAINPAGE 0x39U

#define LSM6DSO_FIFO_STATUS1 0x3AU
#define LSM6DSO_FIFO_STATUS2 0x3B

#define LSM6DSO_TIMESTAMP0 0x40U
#define LSM6DSO_TIMESTAMP1 0x41U
#define LSM6DSO_TIMESTAMP2 0x42U
#define LSM6DSO_TIMESTAMP3 0x43U

#define LSM6DSO_TAP_CFG0 0x56U
#define LSM6DSO_TAP_CFG1 0x57U
#define LSM6DSO_TAP_CFG2 0x58U
#define LSM6DSO_TAP_THS_6D 0x59U

#define LSM6DSO_INT_DUR2 0x5AU
#define LSM6DSO_WAKE_UP_THS 0x5BU
#define LSM6DSO_WAKE_UP_DUR 0x5CU
#define LSM6DSO_FREE_FALL 0x5DU
#define LSM6DSO_MD1_CFG 0x5EU
#define LSM6DSO_MD2_CFG 0x5FU

#define LSM6DSO_I3C_BUS_AVB 0x62U
#define LSM6DSO_INTERNAL_FREQ_FINE 0x63U

#define LSM6DSO_INT_OIS 0x6FU
#define LSM6DSO_CTRL1_OIS 0x70U
#define LSM6DSO_CTRL2_OIS 0x71U
#define LSM6DSO_CTRL3_OIS 0x72U

#define LSM6DSO_X_OFS_USR 0x73U
#define LSM6DSO_Y_OFS_USR 0x74U
#define LSM6DSO_Z_OFS_USR 0x75U
#define LSM6DSO_FIFO_DATA_OUT_TAG 0x78U

#define LSM6DSO_FIFO_DATA_OUT_X_L 0x79U
#define LSM6DSO_FIFO_DATA_OUT_X_H 0x7AU
#define LSM6DSO_FIFO_DATA_OUT_Y_L 0x7BU
#define LSM6DSO_FIFO_DATA_OUT_Y_H 0x7CU
#define LSM6DSO_FIFO_DATA_OUT_Z_L 0x7DU
#define LSM6DSO_FIFO_DATA_OUT_Z_H 0x7EU

#define LSM6DSO_EMB_FUNC_EN_A 0x04U
#define LSM6DSO_EMB_FUNC_EN_B 0x05U

#define LSM6DSO_PAGE_ADDRESS 0x08U
#define LSM6DSO_PAGE_VALUE 0x09U

#define LSM6DSO_EMB_FUNC_INT1 0x0AU

#define LSM6DSO_FSM_INT1_A 0x0BU
#define LSM6DSO_FSM_INT1_B 0x0CU

#define LSM6DSO_EMB_FUNC_INT2 0x0EU

#define LSM6DSO_FSM_INT2_A 0x0FU
#define LSM6DSO_FSM_INT2_B 0x10U

#define LSM6DSO_EMB_FUNC_STATUS 0x12U

#define LSM6DSO_FSM_STATUS_A 0x13U
#define LSM6DSO_FSM_STATUS_B 0x14U

#define LSM6DSO_PAGE_RW 0x17U

#define LSM6DSO_EMB_FUNC_FIFO_CFG 0x44U

#define LSM6DSO_FSM_ENABLE_A 0x46U
#define LSM6DSO_FSM_ENABLE_B 0x47U
#define LSM6DSO_FSM_LONG_COUNTER_L 0x48U
#define LSM6DSO_FSM_LONG_COUNTER_H 0x49U
#define LSM6DSO_FSM_LONG_COUNTER_CLEAR 0x4AU
#define LSM6DSO_FSM_OUTS1 0x4CU
#define LSM6DSO_FSM_OUTS2 0x4DU
#define LSM6DSO_FSM_OUTS3 0x4EU
#define LSM6DSO_FSM_OUTS4 0x4FU
#define LSM6DSO_FSM_OUTS5 0x50U
#define LSM6DSO_FSM_OUTS6 0x51U
#define LSM6DSO_FSM_OUTS7 0x52U
#define LSM6DSO_FSM_OUTS8 0x53U
#define LSM6DSO_FSM_OUTS9 0x54U
#define LSM6DSO_FSM_OUTS10 0x55U
#define LSM6DSO_FSM_OUTS11 0x56U
#define LSM6DSO_FSM_OUTS12 0x57U
#define LSM6DSO_FSM_OUTS13 0x58U
#define LSM6DSO_FSM_OUTS14 0x59U
#define LSM6DSO_FSM_OUTS15 0x5AU
#define LSM6DSO_FSM_OUTS16 0x5BU
#define LSM6DSO_EMB_FUNC_ODR_CFG_B 0x5FU

#define LSM6DSO_STEP_COUNTER_L 0x62U
#define LSM6DSO_STEP_COUNTER_H 0x63U

#define LSM6DSO_EMB_FUNC_SRC 0x64U
#define LSM6DSO_EMB_FUNC_INIT_A 0x66U
#define LSM6DSO_EMB_FUNC_INIT_B 0x67U

#define LSM6DSO_MAG_SENSITIVITY_L 0xBAU
#define LSM6DSO_MAG_SENSITIVITY_H 0xBBU
#define LSM6DSO_MAG_OFFX_L 0xC0U
#define LSM6DSO_MAG_OFFX_H 0xC1U
#define LSM6DSO_MAG_OFFY_L 0xC2U
#define LSM6DSO_MAG_OFFY_H 0xC3U
#define LSM6DSO_MAG_OFFZ_L 0xC4U
#define LSM6DSO_MAG_OFFZ_H 0xC5U
#define LSM6DSO_MAG_SI_XX_L 0xC6U
#define LSM6DSO_MAG_SI_XX_H 0xC7U
#define LSM6DSO_MAG_SI_XY_L 0xC8U
#define LSM6DSO_MAG_SI_XY_H 0xC9U
#define LSM6DSO_MAG_SI_XZ_L 0xCAU
#define LSM6DSO_MAG_SI_XZ_H 0xCBU
#define LSM6DSO_MAG_SI_YY_L 0xCCU
#define LSM6DSO_MAG_SI_YY_H 0xCDU
#define LSM6DSO_MAG_SI_YZ_L 0xCEU
#define LSM6DSO_MAG_SI_YZ_H 0xCFU
#define LSM6DSO_MAG_SI_ZZ_L 0xD0U
#define LSM6DSO_MAG_SI_ZZ_H 0xD1U
#define LSM6DSO_MAG_CFG_A 0xD4U
#define LSM6DSO_MAG_CFG_B 0xD5U

#define LSM6DSO_FSM_LC_TIMEOUT_L 0x17AU
#define LSM6DSO_FSM_LC_TIMEOUT_H 0x17BU
#define LSM6DSO_FSM_PROGRAMS 0x17CU
#define LSM6DSO_FSM_START_ADD_L 0x17EU
#define LSM6DSO_FSM_START_ADD_H 0x17FU

#define LSM6DSO_PEDO_CMD_REG 0x183U
#define LSM6DSO_PEDO_DEB_STEPS_CONF 0x184U
#define LSM6DSO_PEDO_SC_DELTAT_L 0x1D0U
#define LSM6DSO_PEDO_SC_DELTAT_H 0x1D1U

#define LSM6DSO_SENSOR_HUB_1 0x02U
#define LSM6DSO_SENSOR_HUB_2 0x03U
#define LSM6DSO_SENSOR_HUB_3 0x04U
#define LSM6DSO_SENSOR_HUB_4 0x05U
#define LSM6DSO_SENSOR_HUB_5 0x06U
#define LSM6DSO_SENSOR_HUB_6 0x07U
#define LSM6DSO_SENSOR_HUB_7 0x08U
#define LSM6DSO_SENSOR_HUB_8 0x09U
#define LSM6DSO_SENSOR_HUB_9 0x0AU
#define LSM6DSO_SENSOR_HUB_10 0x0BU
#define LSM6DSO_SENSOR_HUB_11 0x0CU
#define LSM6DSO_SENSOR_HUB_12 0x0DU
#define LSM6DSO_SENSOR_HUB_13 0x0EU
#define LSM6DSO_SENSOR_HUB_14 0x0FU
#define LSM6DSO_SENSOR_HUB_15 0x10U
#define LSM6DSO_SENSOR_HUB_16 0x11U
#define LSM6DSO_SENSOR_HUB_17 0x12U
#define LSM6DSO_SENSOR_HUB_18 0x13U

#define LSM6DSO_MASTER_CONFIG 0x14U

#define LSM6DSO_SLV0_ADD 0x15U
#define LSM6DSO_SLV0_SUBADD 0x16U
#define LSM6DSO_SLV0_CONFIG 0x17U

#define LSM6DSO_SLV1_ADD 0x18U
#define LSM6DSO_SLV1_SUBADD 0x19U
#define LSM6DSO_SLV1_CONFIG 0x1AU

#define LSM6DSO_SLV2_ADD 0x1BU
#define LSM6DSO_SLV2_SUBADD 0x1CU
#define LSM6DSO_SLV2_CONFIG 0x1DU

#define LSM6DSO_SLV3_ADD 0x1EU
#define LSM6DSO_SLV3_SUBADD 0x1FU
#define LSM6DSO_SLV3_CONFIG 0x20U

#define LSM6DSO_DATAWRITE_SLV0 0x21U
#define LSM6DSO_STATUS_MASTER 0x22U

#define LSM6DSO_START_FSM_ADD 0x0400U

  typedef enum
  {
    LSM6DSO_XL_UI_OFF = 0x00,       /* in power down */
    LSM6DSO_XL_UI_12Hz5_HP = 0x01,  /* @12Hz5 (high performance) */
    LSM6DSO_XL_UI_26Hz_HP = 0x02,   /* @26Hz  (high performance) */
    LSM6DSO_XL_UI_52Hz_HP = 0x03,   /* @52Hz  (high performance) */
    LSM6DSO_XL_UI_104Hz_HP = 0x04,  /* @104Hz (high performance) */
    LSM6DSO_XL_UI_208Hz_HP = 0x05,  /* @208Hz (high performance) */
    LSM6DSO_XL_UI_416Hz_HP = 0x06,  /* @416Hz (high performance) */
    LSM6DSO_XL_UI_833Hz_HP = 0x07,  /* @833Hz (high performance) */
    LSM6DSO_XL_UI_1667Hz_HP = 0x08, /* @1kHz66 (high performance) */
    LSM6DSO_XL_UI_3333Hz_HP = 0x09, /* @3kHz33 (high performance) */
    LSM6DSO_XL_UI_6667Hz_HP = 0x0A, /* @6kHz66 (high performance) */
  } XL_ODR;

  typedef enum
  {
    LSM6DSO_XL_UI_2g = 0,
    LSM6DSO_XL_UI_4g = 2,
    LSM6DSO_XL_UI_8g = 3,
    LSM6DSO_XL_UI_16g = 1, /* OIS full scale is also forced to be 16g */
  } XL_FS;

  typedef enum
  {
    LSM6DSO_GY_UI_OFF = 0x00,       /* in power down */
    LSM6DSO_GY_UI_12Hz5_HP = 0x01,  /* @12Hz5 (high performance) */
    LSM6DSO_GY_UI_26Hz_HP = 0x02,   /* @26Hz  (high performance) */
    LSM6DSO_GY_UI_52Hz_HP = 0x03,   /* @52Hz  (high performance) */
    LSM6DSO_GY_UI_104Hz_HP = 0x04,  /* @104Hz (high performance) */
    LSM6DSO_GY_UI_208Hz_HP = 0x05,  /* @208Hz (high performance) */
    LSM6DSO_GY_UI_416Hz_HP = 0x06,  /* @416Hz (high performance) */
    LSM6DSO_GY_UI_833Hz_HP = 0x07,  /* @833Hz (high performance) */
    LSM6DSO_GY_UI_1667Hz_HP = 0x08, /* @1kHz66 (high performance) */
    LSM6DSO_GY_UI_3333Hz_HP = 0x09, /* @3kHz33 (high performance) */
    LSM6DSO_GY_UI_6667Hz_HP = 0x0A, /* @6kHz66 (high performance) */
  } GY_ODR;

  typedef enum
  {
    LSM6DSO_GY_UI_125dps = 1,
    LSM6DSO_GY_UI_250dps = 0,
    LSM6DSO_GY_UI_500dps = 2,
    LSM6DSO_GY_UI_1000dps = 4,
    LSM6DSO_GY_UI_2000dps = 6,
  } GY_FS;

  typedef enum
  {
    LSM6DSO_NO_DECIMATION = 0,
    LSM6DSO_DEC_1 = 1,
    LSM6DSO_DEC_8 = 2,
    LSM6DSO_DEC_32 = 3,
  } DEC_TS_BATCH;

  typedef enum
  {
    LSM6DSO_TEMP_NOT_BATCHED = 0,
    LSM6DSO_TEMP_BATCHED_AT_1Hz6 = 1,
    LSM6DSO_TEMP_BATCHED_AT_12Hz5 = 2,
    LSM6DSO_TEMP_BATCHED_AT_52Hz = 3,
  } ODR_T_BATCH;

  typedef enum
  {
    LSM6DSO_BYPASS_MODE = 0,
    LSM6DSO_FIFO_MODE = 1,
    LSM6DSO_STREAM_TO_FIFO_MODE = 3,
    LSM6DSO_BYPASS_TO_STREAM_MODE = 4,
    LSM6DSO_STREAM_MODE = 6,
    LSM6DSO_BYPASS_TO_FIFO_MODE = 7,
  } FIFO_MODE;

  typedef enum
  {
    LSM6DSO_XL_NOT_BATCHED = 0,
    LSM6DSO_XL_BATCHED_AT_12Hz5 = 1,
    LSM6DSO_XL_BATCHED_AT_26Hz = 2,
    LSM6DSO_XL_BATCHED_AT_52Hz = 3,
    LSM6DSO_XL_BATCHED_AT_104Hz = 4,
    LSM6DSO_XL_BATCHED_AT_208Hz = 5,
    LSM6DSO_XL_BATCHED_AT_417Hz = 6,
    LSM6DSO_XL_BATCHED_AT_833Hz = 7,
    LSM6DSO_XL_BATCHED_AT_1667Hz = 8,
    LSM6DSO_XL_BATCHED_AT_3333Hz = 9,
    LSM6DSO_XL_BATCHED_AT_6667Hz = 10,
    LSM6DSO_XL_BATCHED_AT_6Hz5 = 11,
  } BDR_XL;

  typedef enum
  {
    LSM6DSO_GY_NOT_BATCHED = 0,
    LSM6DSO_GY_BATCHED_AT_12Hz5 = 1,
    LSM6DSO_GY_BATCHED_AT_26Hz = 2,
    LSM6DSO_GY_BATCHED_AT_52Hz = 3,
    LSM6DSO_GY_BATCHED_AT_104Hz = 4,
    LSM6DSO_GY_BATCHED_AT_208Hz = 5,
    LSM6DSO_GY_BATCHED_AT_417Hz = 6,
    LSM6DSO_GY_BATCHED_AT_833Hz = 7,
    LSM6DSO_GY_BATCHED_AT_1667Hz = 8,
    LSM6DSO_GY_BATCHED_AT_3333Hz = 9,
    LSM6DSO_GY_BATCHED_AT_6667Hz = 10,
    LSM6DSO_GY_BATCHED_AT_6Hz5 = 11,
  } BDR_GY;

  typedef enum
  {
    INT_DRDY_XL = 1,
    INT_DRDY_G = 2,
    INT_BOOT = 4,
    INT_FIFO_TH = 8,
    INT_FIFO_OVR = 16,
    INT_FIFO_FULL = 32,
    INT_CNT_BDR = 64,
    DEN_DRDY_flag = 128,
  } INT_CTRL;

#ifdef __cplusplus
}
#endif

#endif /*LSM6DSO_DRIVER_H */