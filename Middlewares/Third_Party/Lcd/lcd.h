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
#ifndef __LCD_H
#define __LCD_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "font.h"

#include <string.h>
/* USER CODE END Includes */

/* Exported macro ------------------------------------------------------------*/
/**
  * @brief  This define to indicate use LCD in graphic mode (write by pixel in buffer)
  *         Commentting this will use LCD in default text mode (to reduce memory)
  */
#define LCD_USE_GRAPHIC_MODE

/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported types ------------------------------------------------------------*/
/**
  * @brief  LCD font index enumeration
  */
enum
{
  LCD_FONT_SMALL = (uint8_t)0U,
  LCD_FONT_BIG
};

enum
{
  LCD_CURSOR_BY_FONT = (uint8_t)0U,
  LCD_CURSOR_IN_PIXEL
};

#ifdef LCD_USE_GRAPHIC_MODE
enum
{
  LCD_FILL_POLLING = (uint8_t)0U,
  LCD_FILL_IMMEDIATE
};
#endif

/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported functions prototypes ---------------------------------------------*/
void LCD_SetFont(uint8_t fontIdx);
void LCD_SetCursorPos(uint8_t xPos, uint8_t yPos, uint8_t xPosInPixel);
void LCD_PutChar(uint8_t chr);
void LCD_PutString(uint8_t* str);
uint8_t LCD_GetXPosCenter(uint8_t* str);
#ifdef LCD_USE_GRAPHIC_MODE
void LCD_PutHorizontalLine(uint8_t length);
void LCD_PutVerticalLine(uint8_t length);
void LCD_PutRectangle(uint8_t width, uint8_t height);
#endif

HAL_StatusTypeDef LCD_Init(void);

void LCD_SetContrast(uint8_t contrast);

#ifdef LCD_USE_GRAPHIC_MODE
void LCD_ClearPage(uint8_t startPage, uint8_t pageNum);
void LCD_FillPage(uint8_t startPage, uint8_t pageNum, uint8_t fillNow);
void LCD_FillMain(void);
#endif

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __LCD_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
