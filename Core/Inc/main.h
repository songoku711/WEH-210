/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define SIG_LED_5_Pin GPIO_PIN_2
#define SIG_LED_5_GPIO_Port GPIOE
#define SIG_LED_4_Pin GPIO_PIN_3
#define SIG_LED_4_GPIO_Port GPIOE
#define SIG_LED_3_Pin GPIO_PIN_4
#define SIG_LED_3_GPIO_Port GPIOE
#define SIG_LED_2_Pin GPIO_PIN_5
#define SIG_LED_2_GPIO_Port GPIOE
#define SIG_LED_1_Pin GPIO_PIN_6
#define SIG_LED_1_GPIO_Port GPIOE
#define LCD_BK_Pin GPIO_PIN_1
#define LCD_BK_GPIO_Port GPIOA
#define LCD_nCS_Pin GPIO_PIN_2
#define LCD_nCS_GPIO_Port GPIOA
#define BUT_DRAIN_Pin GPIO_PIN_8
#define BUT_DRAIN_GPIO_Port GPIOE
#define BUT_DRAIN_EXTI_IRQn EXTI9_5_IRQn
#define BUT_WASH_Pin GPIO_PIN_9
#define BUT_WASH_GPIO_Port GPIOE
#define BUT_WASH_EXTI_IRQn EXTI9_5_IRQn
#define BUT_FUNC_Pin GPIO_PIN_10
#define BUT_FUNC_GPIO_Port GPIOE
#define BUT_FUNC_EXTI_IRQn EXTI15_10_IRQn
#define BUT_EXTRACT_Pin GPIO_PIN_11
#define BUT_EXTRACT_GPIO_Port GPIOE
#define BUT_EXTRACT_EXTI_IRQn EXTI15_10_IRQn
#define BUT_SUPP_1_Pin GPIO_PIN_12
#define BUT_SUPP_1_GPIO_Port GPIOE
#define BUT_SUPP_1_EXTI_IRQn EXTI15_10_IRQn
#define BUT_COOL_Pin GPIO_PIN_13
#define BUT_COOL_GPIO_Port GPIOE
#define BUT_COOL_EXTI_IRQn EXTI15_10_IRQn
#define BUT_SUPP_2_Pin GPIO_PIN_14
#define BUT_SUPP_2_GPIO_Port GPIOE
#define BUT_SUPP_2_EXTI_IRQn EXTI15_10_IRQn
#define BUT_HEAT_Pin GPIO_PIN_15
#define BUT_HEAT_GPIO_Port GPIOE
#define BUT_HEAT_EXTI_IRQn EXTI15_10_IRQn
#define LCD_nRES_Pin GPIO_PIN_12
#define LCD_nRES_GPIO_Port GPIOB
#define LCD_A0_Pin GPIO_PIN_13
#define LCD_A0_GPIO_Port GPIOB
#define LCD_nWR_Pin GPIO_PIN_14
#define LCD_nWR_GPIO_Port GPIOB
#define LCD_nRD_Pin GPIO_PIN_15
#define LCD_nRD_GPIO_Port GPIOB
#define LCD_D0_Pin GPIO_PIN_8
#define LCD_D0_GPIO_Port GPIOD
#define LCD_D1_Pin GPIO_PIN_9
#define LCD_D1_GPIO_Port GPIOD
#define LCD_D2_Pin GPIO_PIN_10
#define LCD_D2_GPIO_Port GPIOD
#define LCD_D3_Pin GPIO_PIN_11
#define LCD_D3_GPIO_Port GPIOD
#define LCD_D4_Pin GPIO_PIN_12
#define LCD_D4_GPIO_Port GPIOD
#define LCD_D5_Pin GPIO_PIN_13
#define LCD_D5_GPIO_Port GPIOD
#define LCD_D6_Pin GPIO_PIN_14
#define LCD_D6_GPIO_Port GPIOD
#define LCD_D7_Pin GPIO_PIN_15
#define LCD_D7_GPIO_Port GPIOD
#define RS485_TX_Pin GPIO_PIN_9
#define RS485_TX_GPIO_Port GPIOA
#define RS485_RX_Pin GPIO_PIN_10
#define RS485_RX_GPIO_Port GPIOA
#define RS485_CS_Pin GPIO_PIN_12
#define RS485_CS_GPIO_Port GPIOA
#define DBG_TX_Pin GPIO_PIN_10
#define DBG_TX_GPIO_Port GPIOC
#define DBG_RX_Pin GPIO_PIN_11
#define DBG_RX_GPIO_Port GPIOC
#define BUT_DOWN_Pin GPIO_PIN_0
#define BUT_DOWN_GPIO_Port GPIOD
#define BUT_DOWN_EXTI_IRQn EXTI0_IRQn
#define BUT_START_Pin GPIO_PIN_1
#define BUT_START_GPIO_Port GPIOD
#define BUT_START_EXTI_IRQn EXTI1_IRQn
#define BUT_UP_Pin GPIO_PIN_2
#define BUT_UP_GPIO_Port GPIOD
#define BUT_UP_EXTI_IRQn EXTI2_IRQn
#define BUT_MENU_Pin GPIO_PIN_3
#define BUT_MENU_GPIO_Port GPIOD
#define BUT_MENU_EXTI_IRQn EXTI3_IRQn
#define BUT_STOP_Pin GPIO_PIN_4
#define BUT_STOP_GPIO_Port GPIOD
#define BUT_STOP_EXTI_IRQn EXTI4_IRQn
#define BUZZER_Pin GPIO_PIN_5
#define BUZZER_GPIO_Port GPIOD
#define HC595_CS_Pin GPIO_PIN_7
#define HC595_CS_GPIO_Port GPIOD
#define HC595_SCK_Pin GPIO_PIN_3
#define HC595_SCK_GPIO_Port GPIOB
#define HC595_MOSI_Pin GPIO_PIN_5
#define HC595_MOSI_GPIO_Port GPIOB
#define MEM_SCL_Pin GPIO_PIN_6
#define MEM_SCL_GPIO_Port GPIOB
#define MEM_SDA_Pin GPIO_PIN_7
#define MEM_SDA_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
