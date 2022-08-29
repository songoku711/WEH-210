/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : st7565r.h
  * @brief          : Header for st7565r.c file.
  *                   This file contains the common defines of ST7565R driver.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __ST7565R_H
#define __ST7565R_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "main.h"
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
enum
{
  ST7565R_DisplayOff = (uint8_t)0U,
  ST7565R_DisplayOn
};

enum
{
  ST7565R_ADCNormal = (uint8_t)0U,
  ST7565R_ADCReverse
};

enum
{
  ST7565R_InverseDispOff = (uint8_t)0U,
  ST7565R_InverseDispOn
};

enum
{
  ST7565R_AllPixelsOff = (uint8_t)0U,
  ST7565R_AllPixelsOn
};

enum
{
  ST7565R_BiasLow = (uint8_t)0U,
  ST7565R_BiasHigh
};

enum
{
  ST7565R_COMDirNormal = (uint8_t)0U,
  ST7565R_COMDirReverse = (uint8_t)0x08,
};

enum
{
  ST7565R_PwrCtrlBoosterOff = (uint8_t)0U,
  ST7565R_PwrCtrlBoosterOn = (uint8_t)0x04
};

enum
{
  ST7565R_PwrCtrlVolRegOff = (uint8_t)0U,
  ST7565R_PwrCtrlVolRegOn = (uint8_t)0x02
};

enum
{
  ST7565R_PwrCtrlVolFlwOff = (uint8_t)0U,
  ST7565R_PwrCtrlVolFlwOn
};

enum
{
  ST7565R_SleepOn = (uint8_t)0U,
  ST7565R_SleepOff
};

enum
{
  ST7565R_BoostRatio4x = (uint8_t)0U,
  ST7565R_BoostRatio5x,
  ST7565R_BoostRatio6x = (uint8_t)3U
};

enum
{
  ST7565R_StatusNotBusy = (uint8_t)0U,
  ST7565R_StatusBusy = (uint8_t)0x80
};

enum
{
  ST7565R_StatusADCReverse = (uint8_t)0U,
  ST7565R_StatusADCNormal = (uint8_t)0x40
};

enum
{
  ST7565R_StatusDispOn = (uint8_t)0U,
  ST7565R_StatusDispOff = (uint8_t)0x20
};

enum
{
  ST7565R_StatusOpState = (uint8_t)0U,
  ST7565R_StatusReset = (uint8_t)0x10
};

/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/**
  * @brief  This define to indicate number of pixels per page in vertical
  */
#define ST7565R_PAGE_PIXELS_NUM                  8U

/**
  * @brief  This define to indicate LCD height in pages
  */
#define ST7565R_LCD_HEIGHT_PAGES_NUM             8U

/**
  * @brief  This define to indicate LCD width in pixels
  */
#define ST7565R_LCD_WIDTH_PIXELS_NUM             128U

/**
  * @brief  This define to indicate LCD height in pixels
  */
#define ST7565R_LCD_HEIGHT_PIXELS_NUM            (ST7565R_LCD_HEIGHT_PAGES_NUM * ST7565R_PAGE_PIXELS_NUM)

/**
  * @brief  This define to indicate using RST pin for hardware reset
  */
#define ST7565R_USE_RESET_PIN

/**
  * @brief  This define to indicate using data pin for output purpose,
  *         This means no read status or data function available
  */
#define ST7565R_USE_DATA_PIN_OUTPUT_ONLY

/**
  * @brief  This define to indicate using consecutive data pin
  */
#define ST7565R_USE_DATA_PIN_ARRAY

/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
HAL_StatusTypeDef ST7565R_Init(void);

void ST7565R_SetDisplayOnOff(uint8_t onOff);
void ST7565R_SetDisplayStartLine(uint8_t line);
void ST7565R_SetPageAddress(uint8_t page);
void ST7565R_SetColumnAddress(uint8_t column);
void ST7565R_SetADC(uint8_t adc);
void ST7565R_SetInverseDisplay(uint8_t inv);
void ST7565R_SetAllPixelsOnOff(uint8_t onOff);
void ST7565R_SetBias(uint8_t bias);
void ST7565R_SetReadModifyWrite(uint8_t rmw);
void ST7565R_Reset(uint8_t reset);
void ST7565R_SetCOM(uint8_t com);
void ST7565R_SetPowerController(uint8_t boost, uint8_t volreg, uint8_t volflw);
void ST7565R_SetVolRegRatio(uint8_t ratio);
void ST7565R_SetElecVolume(uint8_t vol);
void ST7565R_Sleep(uint8_t sleep);
void ST7565R_SetBoosterRatio(uint8_t ratio);

void ST7565R_WriteData(uint8_t* data, uint16_t length);
void ST7565R_WritePattern(uint8_t data, uint16_t length);
#ifndef ST7565R_USE_DATA_PIN_OUTPUT_ONLY
void ST7565R_ReadData(uint8_t* data, uint16_t length);
#endif

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
/* ST7565R Chip Select port and pin */
#define ST7565R_CS_Pin        LCD_nCS_Pin
#define ST7565R_CS_Port       LCD_nCS_GPIO_Port

/* ST7565R Hardware Reset port and pin */
#ifdef ST7565R_USE_RESET_PIN
#define ST7565R_RST_Pin       LCD_nRES_Pin
#define ST7565R_RST_Port      LCD_nRES_GPIO_Port
#endif

/* ST7565R Command/Data Selection Control port and pin */
#define ST7565R_A0_Pin        LCD_A0_Pin
#define ST7565R_A0_Port       LCD_A0_GPIO_Port

/* ST7565R Write Signal port and pin */
#define ST7565R_WR_Pin        LCD_nWR_Pin
#define ST7565R_WR_Port       LCD_nWR_GPIO_Port

/* ST7565R Read Signal port and pin */
#define ST7565R_RD_Pin        LCD_nRD_Pin
#define ST7565R_RD_Port       LCD_nRD_GPIO_Port

#ifdef ST7565R_USE_DATA_PIN_ARRAY
/* ST7565R Data bus port, mask and shift */
#define ST7565R_Dx_Mask       (uint32_t)0x0000FF00
#define ST7565R_Dx_Shift      (uint32_t)8U
#define ST7565R_Dx_Port       GPIOD
#else
/* ST7565R Data 0 port and pin */
#define ST7565R_D0_Mask       LCD_D0_Pin
#define ST7565R_D0_Shift      (uint32_t)8U
#define ST7565R_D0_Port       LCD_D0_GPIO_Port

/* ST7565R Data 1 port and pin */
#define ST7565R_D1_Mask       LCD_D1_Pin
#define ST7565R_D1_Shift      (uint32_t)9U
#define ST7565R_D1_Port       LCD_D1_GPIO_Port

/* ST7565R Data 2 port and pin */
#define ST7565R_D2_Mask       LCD_D2_Pin
#define ST7565R_D2_Shift      (uint32_t)10U
#define ST7565R_D2_Port       LCD_D2_GPIO_Port

/* ST7565R Data 3 port and pin */
#define ST7565R_D3_Mask       LCD_D3_Pin
#define ST7565R_D3_Shift      (uint32_t)11U
#define ST7565R_D3_Port       LCD_D3_GPIO_Port

/* ST7565R Data 4 port and pin */
#define ST7565R_D4_Mask       LCD_D4_Pin
#define ST7565R_D4_Shift      (uint32_t)12U
#define ST7565R_D4_Port       LCD_D4_GPIO_Port

/* ST7565R Data 5 port and pin */
#define ST7565R_D5_Mask       LCD_D5_Pin
#define ST7565R_D5_Shift      (uint32_t)13U
#define ST7565R_D5_Port       LCD_D5_GPIO_Port

/* ST7565R Data 6 port and pin */
#define ST7565R_D6_Mask       LCD_D6_Pin
#define ST7565R_D6_Shift      (uint32_t)14U
#define ST7565R_D6_Port       LCD_D6_GPIO_Port

/* ST7565R Data 7 port and pin */
#define ST7565R_D7_Mask       LCD_D7_Pin
#define ST7565R_D7_Shift      (uint32_t)15U
#define ST7565R_D7_Port       LCD_D7_GPIO_Port
#endif /* ST7565R_USE_DATA_PIN_ARRAY */

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __ST7565R_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
