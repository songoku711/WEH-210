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
#ifndef __FONT_H
#define __FONT_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
typedef struct
{
  uint8_t bitWidth;
  uint8_t bitHeight;
  const uint8_t* fontArr;
} Font_ConfigType;
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
extern Font_ConfigType font6x8;
extern Font_ConfigType font8x16;

/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define FONT_UP_ARROW_INDEX                     (uint8_t)0x0A
#define FONT_DOWN_ARROW_INDEX                   (uint8_t)0x0B
#define FONT_LEFT_ARROW_INDEX                   (uint8_t)0x0C
#define FONT_RIGHT_ARROW_INDEX                  (uint8_t)0x0D

#define FONT_NUMBER_TO_INV_CHAR_OFFSET          (uint8_t)0x10
#define FONT_NUMBER_TO_CHAR_OFFSET              (uint8_t)0x30

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __FONT_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
