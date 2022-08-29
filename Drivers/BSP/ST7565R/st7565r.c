/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "st7565r.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/** @defgroup ST7565R_cmdCodeNum ST7565R command code number
  * @brief ST7565R command code number
  * @{
  */
#define ST7565R_CMD_DISPLAY_ON_OFF_CODE          (uint8_t)0xAE
#define ST7565R_CMD_START_LINE_SET_CODE          (uint8_t)0x40
#define ST7565R_CMD_PAGE_ADDR_SET_CODE           (uint8_t)0xB0
#define ST7565R_CMD_COL_ADDR_H_SET_CODE          (uint8_t)0x10
#define ST7565R_CMD_COL_ADDR_L_SET_CODE          (uint8_t)0x00
#define ST7565R_CMD_ADC_SEL_CODE                 (uint8_t)0xA0
#define ST7565R_CMD_INVERSE_DISP_CODE            (uint8_t)0xA6
#define ST7565R_CMD_ALL_PIXELS_ON_OFF_CODE       (uint8_t)0xA4
#define ST7565R_CMD_BIAS_SEL_CODE                (uint8_t)0xA2
#define ST7565R_CMD_RD_MOD_WR_START_CODE         (uint8_t)0xE0
#define ST7565R_CMD_RD_MOD_WR_END_CODE           (uint8_t)0xEE
#define ST7565R_CMD_SOFT_RESET_CODE              (uint8_t)0xE2
#define ST7565R_CMD_COM_SEL_CODE                 (uint8_t)0xC0
#define ST7565R_CMD_PWR_CTRL_SET_CODE            (uint8_t)0x28
#define ST7565R_CMD_VREG_RATIO_SET_CODE          (uint8_t)0x20
#define ST7565R_CMD_EV_SET_CODE                  (uint8_t)0x81
#define ST7565R_CMD_SLEEP_CODE                   (uint8_t)0xAC
#define ST7565R_CMD_BOOST_RATIO_SET_CODE         (uint8_t)0xF8
#define ST7565R_CMD_NOP_CODE                     (uint8_t)0xE3
/**
  * @}
  */

/** @defgroup ST7565R_cmdMask ST7565R command mask
  * @brief ST7565R command data mask
  * @{
  */
#define ST7565R_CMD_DISPLAY_ON_OFF_MASK          (uint8_t)0x01

#define ST7565R_CMD_START_LINE_SET_MASK          (uint8_t)0x3F
#define ST7565R_CMD_START_LINE_SET_MAX           (uint8_t)(ST7565R_LCD_HEIGHT_PAGES_NUM * ST7565R_PAGE_PIXELS_NUM - 1U)

#define ST7565R_CMD_PAGE_ADDR_SET_MASK           (uint8_t)0x0F
#define ST7565R_CMD_PAGE_ADDR_SET_MAX            (uint8_t)ST7565R_LCD_HEIGHT_PAGES_NUM

#define ST7565R_CMD_COL_ADDR_H_SET_MASK          (uint8_t)0x0F
#define ST7565R_CMD_COL_ADDR_L_SET_MASK          (uint8_t)0x0F
#define ST7565R_CMD_COL_ADDR_SET_MAX             (uint8_t)(ST7565R_LCD_WIDTH_PIXELS_NUM -1U)

#define ST7565R_CMD_ADC_SEL_MASK                 (uint8_t)0x01

#define ST7565R_CMD_INVERSE_DISP_MASK            (uint8_t)0x01

#define ST7565R_CMD_ALL_PIXELS_ON_OFF_MASK       (uint8_t)0x01

#define ST7565R_CMD_BIAS_SEL_MASK                (uint8_t)0x01

#define ST7565R_CMD_COM_SEL_MASK                 (uint8_t)0x08

#define ST7565R_CMD_PWR_CTRL_BOOST_MASK          (uint8_t)0x04
#define ST7565R_CMD_PWR_CTRL_VOLREG_MASK         (uint8_t)0x02
#define ST7565R_CMD_PWR_CTRL_VOLFLW_MASK         (uint8_t)0x01

#define ST7565R_CMD_VREG_RATIO_SET_MASK          (uint8_t)0x07
#define ST7565R_CMD_VREG_RATIO_SET_MAX           (uint8_t)7U

#define ST7565R_CMD_EV_SET_MASK                  (uint8_t)0x3F
#define ST7565R_CMD_EV_SET_MAX                   (uint8_t)63U

#define ST7565R_CMD_SLEEP_MASK                   (uint8_t)0x01

#define ST7565R_CMD_BOOST_RATIO_SET_MASK         (uint8_t)0x03

#define ST7565R_CMD_READ_STATUS_BUSY_MASK        (uint8_t)0x80
#define ST7565R_CMD_READ_STATUS_ADC_MASK         (uint8_t)0x40
#define ST7565R_CMD_READ_STATUS_DISP_MASK        (uint8_t)0x20
#define ST7565R_CMD_READ_STATUS_RESET_MASK       (uint8_t)0x10
/**
  * @}
  */

/** @defgroup ST7565R_cmdType ST7565R command type
  * @brief ST7565R command code number
  * @{
  */
#define __ST7565R_CMD_DISPLAY_ON_OFF(x)          (uint8_t)(ST7565R_CMD_DISPLAY_ON_OFF_CODE | (x & ST7565R_CMD_DISPLAY_ON_OFF_MASK))
#define __ST7565R_CMD_START_LINE_SET(x)          (uint8_t)(ST7565R_CMD_START_LINE_SET_CODE | (x & ST7565R_CMD_START_LINE_SET_MASK))
#define __ST7565R_CMD_PAGE_ADDR_SET(x)           (uint8_t)(ST7565R_CMD_PAGE_ADDR_SET_CODE | (x & ST7565R_CMD_PAGE_ADDR_SET_MASK))
#define __ST7565R_CMD_COL_ADDR_H_SET(x)          (uint8_t)(ST7565R_CMD_COL_ADDR_H_SET_CODE | ((uint8_t)(x >> 4U) & ST7565R_CMD_COL_ADDR_H_SET_MASK))
#define __ST7565R_CMD_COL_ADDR_L_SET(x)          (uint8_t)(ST7565R_CMD_COL_ADDR_L_SET_CODE | (x & ST7565R_CMD_COL_ADDR_L_SET_MASK))
#define __ST7565R_CMD_ADC_SEL(x)                 (uint8_t)(ST7565R_CMD_ADC_SEL_CODE | (x & ST7565R_CMD_ADC_SEL_MASK))
#define __ST7565R_CMD_INVERSE_DISP(x)            (uint8_t)(ST7565R_CMD_INVERSE_DISP_CODE | (x & ST7565R_CMD_INVERSE_DISP_MASK))
#define __ST7565R_CMD_ALL_PIXELS_ON_OFF(x)       (uint8_t)(ST7565R_CMD_ALL_PIXELS_ON_OFF_CODE | (x & ST7565R_CMD_ALL_PIXELS_ON_OFF_MASK))
#define __ST7565R_CMD_BIAS_SEL(x)                (uint8_t)(ST7565R_CMD_BIAS_SEL_CODE | (x & ST7565R_CMD_BIAS_SEL_MASK))
#define __ST7565R_CMD_RD_MOD_WR_START()          (uint8_t)(ST7565R_CMD_RD_MOD_WR_START_CODE)
#define __ST7565R_CMD_RD_MOD_WR_END()            (uint8_t)(ST7565R_CMD_RD_MOD_WR_END_CODE)
#define __ST7565R_CMD_SOFT_RESET()               (uint8_t)(ST7565R_CMD_SOFT_RESET_CODE)
#define __ST7565R_CMD_COM_SEL(x)                 (uint8_t)(ST7565R_CMD_COM_SEL_CODE | (x & ST7565R_CMD_COM_SEL_MASK))
#define __ST7565R_CMD_PWR_CTRL_SET(x)            (uint8_t)(ST7565R_CMD_PWR_CTRL_SET_CODE | (x & (ST7565R_CMD_PWR_CTRL_BOOST_MASK | ST7565R_CMD_PWR_CTRL_VOLREG_MASK | ST7565R_CMD_PWR_CTRL_VOLFLW_MASK)))
#define __ST7565R_CMD_VREG_RATIO_SET(x)          (uint8_t)(ST7565R_CMD_VREG_RATIO_SET_CODE | (x & ST7565R_CMD_VREG_RATIO_SET_MASK))
#define __ST7565R_CMD_EV_MODE_SET()              (uint8_t)(ST7565R_CMD_EV_SET_CODE)
#define __ST7565R_CMD_EV_REG_SET(x)              (uint8_t)(x & ST7565R_CMD_EV_SET_MASK)
#define __ST7565R_CMD_SLEEP_PRED(x)              (uint8_t)(ST7565R_CMD_SLEEP_CODE | (x & ST7565R_CMD_SLEEP_MASK))
#define __ST7565R_CMD_SLEEP_FOLW()               (uint8_t)0x00
#define __ST7565R_CMD_BOOST_RATIO_MODE_SET()     (uint8_t)(ST7565R_CMD_BOOST_RATIO_SET_CODE)
#define __ST7565R_CMD_BOOST_RATIO_REG_SET(x)     (uint8_t)(x & ST7565R_CMD_BOOST_RATIO_SET_MASK)
#define __ST7565R_CMD_NOP()                      (uint8_t)(ST7565R_CMD_NOP_CODE)
/**
  * @}
  */

/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
#if ((defined ST7565R_CS_Pin) && (defined ST7565R_CS_Port))
  #define __ST7565R_CS_SET()        { ST7565R_CS_Port->ODR |= (uint32_t)ST7565R_CS_Pin; }
  #define __ST7565R_CS_RESET()      { ST7565R_CS_Port->ODR &= (uint32_t)(~((uint32_t)ST7565R_CS_Pin)); }
#else
  #error "ST7565R Chip Select port and pin are not defined"
#endif

#ifdef ST7565R_USE_RESET_PIN
#if ((defined ST7565R_RST_Pin) && (defined ST7565R_RST_Port))
  #define __ST7565R_RST_SET()       { ST7565R_RST_Port->ODR |= (uint32_t)ST7565R_RST_Pin; }
  #define __ST7565R_RST_RESET()     { ST7565R_RST_Port->ODR &= (uint32_t)(~((uint32_t)ST7565R_RST_Pin)); }
#else
  #error "ST7565R Hardware Reset port and pin are not defined"
#endif
#endif

#if ((defined ST7565R_A0_Pin) && (defined ST7565R_A0_Port))
  #define __ST7565R_A0_SET()        { ST7565R_A0_Port->ODR |= (uint32_t)ST7565R_A0_Pin; }
  #define __ST7565R_A0_RESET()      { ST7565R_A0_Port->ODR &= (uint32_t)(~((uint32_t)ST7565R_A0_Pin)); }
#else
  #error "ST7565R Command/Data Selection Control port and pin are not defined"
#endif

#if ((defined ST7565R_WR_Pin) && (defined ST7565R_WR_Port))
  #define __ST7565R_WR_SET()        { ST7565R_WR_Port->ODR |= (uint32_t)ST7565R_WR_Pin; }
  #define __ST7565R_WR_RESET()      { ST7565R_WR_Port->ODR &= (uint32_t)(~((uint32_t)ST7565R_WR_Pin)); }
#else
  #error "ST7565R Write Signal port and pin are not defined"
#endif

#if ((defined ST7565R_RD_Pin) && (defined ST7565R_RD_Port))
  #define __ST7565R_RD_SET()        { ST7565R_RD_Port->ODR |= (uint32_t)ST7565R_RD_Pin; }
  #define __ST7565R_RD_RESET()      { ST7565R_RD_Port->ODR &= (uint32_t)(~((uint32_t)ST7565R_RD_Pin)); }
#else
  #error "ST7565R Read Signal port and pin are not defined"
#endif

#ifdef ST7565R_USE_DATA_PIN_ARRAY
#if ((defined ST7565R_Dx_Mask) && (defined ST7565R_Dx_Shift) && (defined ST7565R_Dx_Port))
  #define __ST7565R_Dx_OUT(data) \
    { \
      uint32_t temp = ST7565R_Dx_Port->ODR; \
      \
      temp &= (uint32_t)(~ST7565R_Dx_Mask); \
      temp |= (uint32_t)(((uint32_t)data << ST7565R_Dx_Shift) & ST7565R_Dx_Mask); \
      \
      ST7565R_Dx_Port->ODR = temp; \
    }
#ifndef ST7565R_USE_DATA_PIN_OUTPUT_ONLY
  #define __ST7565R_Dx_IN(data) \
    { \
      data = (uint8_t)((ST7565R_Dx_Port->IDR & ST7565R_Dx_Mask) >> ST7565R_Dx_Shift); \
    }
  #define __ST7565R_Dx_DIR_IN() \
    { \
      uint32_t temp = ST7565R_Dx_Port->MODER; \
      \
      for (uint32_t index = 0; index < (uint32_t)8U; index++) \
      { \
        temp &= ~(GPIO_MODER_MODER0 << (index * 2U)); \
      } \
      \
      ST7565R_Dx_Port->MODER = temp; \
    }
  #define __ST7565R_Dx_DIR_OUT() \
    { \
      uint32_t temp = ST7565R_Dx_Port->MODER; \
      \
      for (uint32_t index = 0; index < (uint32_t)8U; index++) \
      { \
        temp &= ~(GPIO_MODER_MODER0 << (index * 2U)); \
        temp |= (MODE_OUTPUT << (index * 2U)); \
      } \
      \
      ST7565R_Dx_Port->MODER = temp; \
    }
#endif
#else
  #error "ST7565R Data bus mask, shift and port are not defined"
#endif
#else
#if ((defined ST7565R_D0_Mask) && (defined ST7565R_D0_Shift) && (defined ST7565R_D0_Port))
  #define __ST7565R_D0_SET()        { ST7565R_D0_Port->ODR |= (uint32_t)ST7565R_D0_Mask; }
  #define __ST7565R_D0_RESET()      { ST7565R_D0_Port->ODR &= (uint32_t)(~((uint32_t)ST7565R_D0_Mask)); }
#ifndef ST7565R_USE_DATA_PIN_OUTPUT_ONLY
  #define __ST7565R_D0_IN()         (uint8_t)((ST7565R_D0_Port->IDR & ST7565R_D0_Mask) >> ST7565R_D0_Shift)
  #define __ST7565R_D0_DIR_IN() \
    { \
      uint32_t temp = ST7565R_D0_Port->MODER; \
      \
      temp &= ~(GPIO_MODER_MODER0 << (ST7565R_D0_Shift * 2U)); \
      \
      ST7565R_D0_Port->MODER = temp; \
    }
  #define __ST7565R_D0_DIR_OUT() \
    { \
      uint32_t temp = ST7565R_D0_Port->MODER; \
      \
      temp &= ~(GPIO_MODER_MODER0 << (ST7565R_D0_Shift * 2U)); \
      temp |= (MODE_OUTPUT << (ST7565R_D0_Shift * 2U)); \
      \
      ST7565R_D0_Port->MODER = temp; \
    }
#endif
#else
  #error "ST7565R Data 0 port and pin are not defined"
#endif

#if ((defined ST7565R_D1_Mask) && (defined ST7565R_D1_Shift) && (defined ST7565R_D1_Port))
  #define __ST7565R_D1_SET()        { ST7565R_D1_Port->ODR |= (uint32_t)ST7565R_D1_Mask; }
  #define __ST7565R_D1_RESET()      { ST7565R_D1_Port->ODR &= (uint32_t)(~((uint32_t)ST7565R_D1_Mask)); }
#ifndef ST7565R_USE_DATA_PIN_OUTPUT_ONLY
  #define __ST7565R_D1_IN()         (uint8_t)((ST7565R_D1_Port->IDR & ST7565R_D1_Mask) >> ST7565R_D1_Shift)
  #define __ST7565R_D1_DIR_IN() \
    { \
      uint32_t temp = ST7565R_D1_Port->MODER; \
      \
      temp &= ~(GPIO_MODER_MODER0 << (ST7565R_D1_Shift * 2U)); \
      \
      ST7565R_D1_Port->MODER = temp; \
    }
  #define __ST7565R_D1_DIR_OUT() \
    { \
      uint32_t temp = ST7565R_D1_Port->MODER; \
      \
      temp &= ~(GPIO_MODER_MODER0 << (ST7565R_D1_Shift * 2U)); \
      temp |= (MODE_OUTPUT << (ST7565R_D1_Shift * 2U)); \
      \
      ST7565R_D1_Port->MODER = temp; \
    }
#endif
#else
  #error "ST7565R Data 1 port and pin are not defined"
#endif

#if ((defined ST7565R_D2_Mask) && (defined ST7565R_D2_Shift) && (defined ST7565R_D2_Port))
  #define __ST7565R_D2_SET()        { ST7565R_D2_Port->ODR |= (uint32_t)ST7565R_D2_Mask; }
  #define __ST7565R_D2_RESET()      { ST7565R_D2_Port->ODR &= (uint32_t)(~((uint32_t)ST7565R_D2_Mask)); }
#ifndef ST7565R_USE_DATA_PIN_OUTPUT_ONLY
  #define __ST7565R_D2_IN()         (uint8_t)((ST7565R_D2_Port->IDR & ST7565R_D2_Mask) >> ST7565R_D2_Shift)
  #define __ST7565R_D2_DIR_IN() \
    { \
      uint32_t temp = ST7565R_D2_Port->MODER; \
      \
      temp &= ~(GPIO_MODER_MODER0 << (ST7565R_D2_Shift * 2U)); \
      \
      ST7565R_D2_Port->MODER = temp; \
    }
  #define __ST7565R_D2_DIR_OUT() \
    { \
      uint32_t temp = ST7565R_D2_Port->MODER; \
      \
      temp &= ~(GPIO_MODER_MODER0 << (ST7565R_D2_Shift * 2U)); \
      temp |= (MODE_OUTPUT << (ST7565R_D2_Shift * 2U)); \
      \
      ST7565R_D2_Port->MODER = temp; \
    }
#endif
#else
  #error "ST7565R Data 2 port and pin are not defined"
#endif

#if ((defined ST7565R_D3_Mask) && (defined ST7565R_D3_Shift) && (defined ST7565R_D3_Port))
  #define __ST7565R_D3_SET()        { ST7565R_D3_Port->ODR |= (uint32_t)ST7565R_D3_Mask; }
  #define __ST7565R_D3_RESET()      { ST7565R_D3_Port->ODR &= (uint32_t)(~((uint32_t)ST7565R_D3_Mask)); }
#ifndef ST7565R_USE_DATA_PIN_OUTPUT_ONLY
  #define __ST7565R_D3_IN()         (uint8_t)((ST7565R_D3_Port->IDR & ST7565R_D3_Mask) >> ST7565R_D3_Shift)
  #define __ST7565R_D3_DIR_IN() \
    { \
      uint32_t temp = ST7565R_D3_Port->MODER; \
      \
      temp &= ~(GPIO_MODER_MODER0 << (ST7565R_D3_Shift * 2U)); \
      \
      ST7565R_D3_Port->MODER = temp; \
    }
  #define __ST7565R_D3_DIR_OUT() \
    { \
      uint32_t temp = ST7565R_D3_Port->MODER; \
      \
      temp &= ~(GPIO_MODER_MODER0 << (ST7565R_D3_Shift * 2U)); \
      temp |= (MODE_OUTPUT << (ST7565R_D3_Shift * 2U)); \
      \
      ST7565R_D3_Port->MODER = temp; \
    }
#endif
#else
  #error "ST7565R Data 3 port and pin are not defined"
#endif

#if ((defined ST7565R_D4_Mask) && (defined ST7565R_D4_Shift) && (defined ST7565R_D4_Port))
  #define __ST7565R_D4_SET()        { ST7565R_D4_Port->ODR |= (uint32_t)ST7565R_D4_Mask; }
  #define __ST7565R_D4_RESET()      { ST7565R_D4_Port->ODR &= (uint32_t)(~((uint32_t)ST7565R_D4_Mask)); }
#ifndef ST7565R_USE_DATA_PIN_OUTPUT_ONLY
  #define __ST7565R_D4_IN()         (uint8_t)((ST7565R_D4_Port->IDR & ST7565R_D4_Mask) >> ST7565R_D4_Shift)
  #define __ST7565R_D4_DIR_IN() \
    { \
      uint32_t temp = ST7565R_D4_Port->MODER; \
      \
      temp &= ~(GPIO_MODER_MODER0 << (ST7565R_D4_Shift * 2U)); \
      \
      ST7565R_D4_Port->MODER = temp; \
    }
  #define __ST7565R_D4_DIR_OUT() \
    { \
      uint32_t temp = ST7565R_D4_Port->MODER; \
      \
      temp &= ~(GPIO_MODER_MODER0 << (ST7565R_D4_Shift * 2U)); \
      temp |= (MODE_OUTPUT << (ST7565R_D4_Shift * 2U)); \
      \
      ST7565R_D4_Port->MODER = temp; \
    }
#endif
#else
  #error "ST7565R Data 4 port and pin are not defined"
#endif

#if ((defined ST7565R_D5_Mask) && (defined ST7565R_D5_Shift) && (defined ST7565R_D5_Port))
  #define __ST7565R_D5_SET()        { ST7565R_D5_Port->ODR |= (uint32_t)ST7565R_D5_Mask; }
  #define __ST7565R_D5_RESET()      { ST7565R_D5_Port->ODR &= (uint32_t)(~((uint32_t)ST7565R_D5_Mask)); }
#ifndef ST7565R_USE_DATA_PIN_OUTPUT_ONLY
  #define __ST7565R_D5_IN()         (uint8_t)((ST7565R_D5_Port->IDR & ST7565R_D5_Mask) >> ST7565R_D5_Shift)
  #define __ST7565R_D5_DIR_IN() \
    { \
      uint32_t temp = ST7565R_D5_Port->MODER; \
      \
      temp &= ~(GPIO_MODER_MODER0 << (ST7565R_D5_Shift * 2U)); \
      \
      ST7565R_D5_Port->MODER = temp; \
    }
  #define __ST7565R_D5_DIR_OUT() \
    { \
      uint32_t temp = ST7565R_D5_Port->MODER; \
      \
      temp &= ~(GPIO_MODER_MODER0 << (ST7565R_D5_Shift * 2U)); \
      temp |= (MODE_OUTPUT << (ST7565R_D5_Shift * 2U)); \
      \
      ST7565R_D5_Port->MODER = temp; \
    }
#endif
#else
  #error "ST7565R Data 5 port and pin are not defined"
#endif

#if ((defined ST7565R_D6_Mask) && (defined ST7565R_D6_Shift) && (defined ST7565R_D6_Port))
  #define __ST7565R_D6_SET()        { ST7565R_D6_Port->ODR |= (uint32_t)ST7565R_D6_Mask; }
  #define __ST7565R_D6_RESET()      { ST7565R_D6_Port->ODR &= (uint32_t)(~((uint32_t)ST7565R_D6_Mask)); }
#ifndef ST7565R_USE_DATA_PIN_OUTPUT_ONLY
  #define __ST7565R_D6_IN()         (uint8_t)((ST7565R_D6_Port->IDR & ST7565R_D6_Mask) >> ST7565R_D6_Shift)
  #define __ST7565R_D6_DIR_IN() \
    { \
      uint32_t temp = ST7565R_D6_Port->MODER; \
      \
      temp &= ~(GPIO_MODER_MODER0 << (ST7565R_D6_Shift * 2U)); \
      \
      ST7565R_D6_Port->MODER = temp; \
    }
  #define __ST7565R_D6_DIR_OUT() \
    { \
      uint32_t temp = ST7565R_D6_Port->MODER; \
      \
      temp &= ~(GPIO_MODER_MODER0 << (ST7565R_D6_Shift * 2U)); \
      temp |= (MODE_OUTPUT << (ST7565R_D6_Shift * 2U)); \
      \
      ST7565R_D6_Port->MODER = temp; \
    }
#endif
#else
  #error "ST7565R Data 6 port and pin are not defined"
#endif

#if ((defined ST7565R_D7_Mask) && (defined ST7565R_D7_Shift) && (defined ST7565R_D7_Port))
  #define __ST7565R_D7_SET()        { ST7565R_D7_Port->ODR |= (uint32_t)ST7565R_D7_Mask; }
  #define __ST7565R_D7_RESET()      { ST7565R_D7_Port->ODR &= (uint32_t)(~((uint32_t)ST7565R_D7_Mask)); }
#ifndef ST7565R_USE_DATA_PIN_OUTPUT_ONLY
  #define __ST7565R_D7_IN()         (uint8_t)((ST7565R_D7_Port->IDR & ST7565R_D7_Mask) >> ST7565R_D7_Shift)
  #define __ST7565R_D7_DIR_IN() \
    { \
      uint32_t temp = ST7565R_D7_Port->MODER; \
      \
      temp &= ~(GPIO_MODER_MODER0 << (ST7565R_D7_Shift * 2U)); \
      \
      ST7565R_D7_Port->MODER = temp; \
    }
  #define __ST7565R_D7_DIR_OUT() \
    { \
      uint32_t temp = ST7565R_D7_Port->MODER; \
      \
      temp &= ~(GPIO_MODER_MODER0 << (ST7565R_D7_Shift * 2U)); \
      temp |= (MODE_OUTPUT << (ST7565R_D7_Shift * 2U)); \
      \
      ST7565R_D7_Port->MODER = temp; \
    }
#endif
#else
  #error "ST7565R Data 7 port and pin are not defined"
#endif
#endif /* ST7565R_USE_DATA_PIN_ARRAY */

/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */
#ifndef ST7565R_USE_DATA_PIN_OUTPUT_ONLY
static uint8_t ST7565R_dataBusDir;
#endif
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
static void ST7565R_writeCommand(uint8_t* cmd, uint16_t length);
#ifndef ST7565R_USE_DATA_PIN_OUTPUT_ONLY
static void ST7565R_readStatus(uint8_t* status);
#endif
static void ST7565R_writeData(uint8_t* data, uint16_t length);
#ifndef ST7565R_USE_DATA_PIN_OUTPUT_ONLY
static void ST7565R_readData(uint8_t* data, uint16_t length);
#endif
static void ST7565R_writePattern(uint8_t data, uint16_t length);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/**
  * @brief  ST7565R write command function.
  * @param  cmd Command code number, refer to @ref ST7565R_cmdType
  * @param  length Command array length
  * @retval None
  */
static void ST7565R_writeCommand(uint8_t* cmd, uint16_t length)
{
  __ST7565R_A0_RESET();             /* A0 pin low for command data */
  __ST7565R_RD_SET();               /* Read pin high for not read */
  __ST7565R_WR_RESET();             /* Write pin low for write */
  
  __ST7565R_CS_SET();               /* Chip Select pin high for idle */
  
#ifndef ST7565R_USE_DATA_PIN_OUTPUT_ONLY
  /* Change data bus pin to output direction if not already be */
  if (ST7565R_dataBusDir != MODE_OUTPUT)
  {
#ifdef ST7565R_USE_DATA_PIN_ARRAY
    __ST7565R_Dx_DIR_OUT();
#else
    __ST7565R_D0_DIR_OUT();
    __ST7565R_D1_DIR_OUT();
    __ST7565R_D2_DIR_OUT();
    __ST7565R_D3_DIR_OUT();
    __ST7565R_D4_DIR_OUT();
    __ST7565R_D5_DIR_OUT();
    __ST7565R_D6_DIR_OUT();
    __ST7565R_D7_DIR_OUT();
#endif
    ST7565R_dataBusDir = MODE_OUTPUT;
  }
#endif
  
  for (uint16_t index = (uint16_t)0U; index < length; index++)
  {
    /* Prepare command data */
#ifdef ST7565R_USE_DATA_PIN_ARRAY
    __ST7565R_Dx_OUT(*(cmd + index));
#else
    if ((*(cmd + index) & (uint8_t)0x01) != (uint8_t)0U) { __ST7565R_D0_SET(); } else { __ST7565R_D0_RESET(); }
    if ((*(cmd + index) & (uint8_t)0x02) != (uint8_t)0U) { __ST7565R_D1_SET(); } else { __ST7565R_D1_RESET(); }
    if ((*(cmd + index) & (uint8_t)0x04) != (uint8_t)0U) { __ST7565R_D2_SET(); } else { __ST7565R_D2_RESET(); }
    if ((*(cmd + index) & (uint8_t)0x08) != (uint8_t)0U) { __ST7565R_D3_SET(); } else { __ST7565R_D3_RESET(); }
    if ((*(cmd + index) & (uint8_t)0x10) != (uint8_t)0U) { __ST7565R_D4_SET(); } else { __ST7565R_D4_RESET(); }
    if ((*(cmd + index) & (uint8_t)0x20) != (uint8_t)0U) { __ST7565R_D5_SET(); } else { __ST7565R_D5_RESET(); }
    if ((*(cmd + index) & (uint8_t)0x40) != (uint8_t)0U) { __ST7565R_D6_SET(); } else { __ST7565R_D6_RESET(); }
    if ((*(cmd + index) & (uint8_t)0x80) != (uint8_t)0U) { __ST7565R_D7_SET(); } else { __ST7565R_D7_RESET(); }
#endif
    
    __ST7565R_CS_RESET();           /* Chip Select pin low for start writing */
    
    __ASM ("nop");
    __ASM ("nop");
    
    __ST7565R_CS_SET();             /* Chip Select pin high for idle */
  }
  
  __ST7565R_WR_SET();               /* Write pin high for not write */
}

#ifndef ST7565R_USE_DATA_PIN_OUTPUT_ONLY
/**
  * @brief  ST7565R read status function.
  * @param  status Status data to be stored into
  * @retval None
  */
static void ST7565R_readStatus(uint8_t* status)
{
  __ST7565R_A0_RESET();             /* A0 pin low for command data */
  __ST7565R_RD_RESET();             /* Read pin low for read */
  __ST7565R_WR_SET();               /* Write pin high for not write */
  
  __ST7565R_CS_SET();               /* Chip Select pin high for idle */
  
  /* Change data bus pin to input direction if not already be */
  if (ST7565R_dataBusDir != MODE_INPUT)
  {
#ifdef ST7565R_USE_DATA_PIN_ARRAY
    __ST7565R_Dx_DIR_IN();
#else
    __ST7565R_D0_DIR_IN();
    __ST7565R_D1_DIR_IN();
    __ST7565R_D2_DIR_IN();
    __ST7565R_D3_DIR_IN();
    __ST7565R_D4_DIR_IN();
    __ST7565R_D5_DIR_IN();
    __ST7565R_D6_DIR_IN();
    __ST7565R_D7_DIR_IN();
#endif
    ST7565R_dataBusDir = MODE_INPUT;
  }
  
  __ST7565R_CS_RESET();             /* Chip Select pin low for start reading */
  
  __ASM ("nop");
  __ASM ("nop");
  
  /* Get status data */
#ifdef ST7565R_USE_DATA_PIN_ARRAY
  __ST7565R_Dx_IN(*status);
#else
  *status  = (uint8_t)(__ST7565R_D0_IN() << 0U);
  *status |= (uint8_t)(__ST7565R_D1_IN() << 1U);
  *status |= (uint8_t)(__ST7565R_D2_IN() << 2U);
  *status |= (uint8_t)(__ST7565R_D3_IN() << 3U);
  *status |= (uint8_t)(__ST7565R_D4_IN() << 4U);
  *status |= (uint8_t)(__ST7565R_D5_IN() << 5U);
  *status |= (uint8_t)(__ST7565R_D6_IN() << 6U);
  *status |= (uint8_t)(__ST7565R_D7_IN() << 7U);
#endif
  
  __ST7565R_CS_SET();               /* Chip Select pin high for idle */
  
  __ST7565R_RD_SET();               /* Read pin high for not read */
}
#endif

/**
  * @brief  ST7565R write display data function.
  * @param  data Data array pointer
  * @param  length Data array length
  * @retval None
  */
static void ST7565R_writeData(uint8_t* data, uint16_t length)
{
  __ST7565R_A0_SET();               /* A0 pin high for display data */
  __ST7565R_RD_SET();               /* Read pin high for not read */
  __ST7565R_WR_RESET();             /* Write pin low for write */
  
  __ST7565R_CS_SET();               /* Chip Select pin high for idle */
  
#ifndef ST7565R_USE_DATA_PIN_OUTPUT_ONLY
  /* Change data bus pin to output direction if not already be */
  if (ST7565R_dataBusDir != MODE_OUTPUT)
  {
#ifdef ST7565R_USE_DATA_PIN_ARRAY
    __ST7565R_Dx_DIR_OUT();
#else
    __ST7565R_D0_DIR_OUT();
    __ST7565R_D1_DIR_OUT();
    __ST7565R_D2_DIR_OUT();
    __ST7565R_D3_DIR_OUT();
    __ST7565R_D4_DIR_OUT();
    __ST7565R_D5_DIR_OUT();
    __ST7565R_D6_DIR_OUT();
    __ST7565R_D7_DIR_OUT();
#endif
    ST7565R_dataBusDir = MODE_OUTPUT;
  }
#endif
  
  for (uint16_t index = (uint16_t)0U; index < length; index++)
  {
    /* Prepare command data */
#ifdef ST7565R_USE_DATA_PIN_ARRAY
    __ST7565R_Dx_OUT(*(data + index));
#else
    if ((*(data + index) & (uint8_t)0x01) != (uint8_t)0U) { __ST7565R_D0_SET(); } else { __ST7565R_D0_RESET(); }
    if ((*(data + index) & (uint8_t)0x02) != (uint8_t)0U) { __ST7565R_D1_SET(); } else { __ST7565R_D1_RESET(); }
    if ((*(data + index) & (uint8_t)0x04) != (uint8_t)0U) { __ST7565R_D2_SET(); } else { __ST7565R_D2_RESET(); }
    if ((*(data + index) & (uint8_t)0x08) != (uint8_t)0U) { __ST7565R_D3_SET(); } else { __ST7565R_D3_RESET(); }
    if ((*(data + index) & (uint8_t)0x10) != (uint8_t)0U) { __ST7565R_D4_SET(); } else { __ST7565R_D4_RESET(); }
    if ((*(data + index) & (uint8_t)0x20) != (uint8_t)0U) { __ST7565R_D5_SET(); } else { __ST7565R_D5_RESET(); }
    if ((*(data + index) & (uint8_t)0x40) != (uint8_t)0U) { __ST7565R_D6_SET(); } else { __ST7565R_D6_RESET(); }
    if ((*(data + index) & (uint8_t)0x80) != (uint8_t)0U) { __ST7565R_D7_SET(); } else { __ST7565R_D7_RESET(); }
#endif
    
    __ST7565R_CS_RESET();           /* Chip Select pin low for start writing */
    
    __ASM ("nop");
    __ASM ("nop");
    
    __ST7565R_CS_SET();             /* Chip Select pin high for idle */
  }
  
  __ST7565R_WR_SET();               /* Write pin high for not write */
}

#ifndef ST7565R_USE_DATA_PIN_OUTPUT_ONLY
/**
  * @brief  ST7565R read display data function.
  * @param  data Data array pointer
  * @param  length Data array length
  * @retval None
  */
static void ST7565R_readData(uint8_t* data, uint16_t length)
{
  __ST7565R_A0_SET();               /* A0 pin high for display data */
  __ST7565R_RD_RESET();             /* Read pin low for read */
  __ST7565R_WR_SET();               /* Write pin high for not write */
  
  __ST7565R_CS_SET();               /* Chip Select pin high for idle */
  
  /* Change data bus pin to input direction if not already be */
  if (ST7565R_dataBusDir != MODE_INPUT)
  {
#ifdef ST7565R_USE_DATA_PIN_ARRAY
    __ST7565R_Dx_DIR_IN();
#else
    __ST7565R_D0_DIR_IN();
    __ST7565R_D1_DIR_IN();
    __ST7565R_D2_DIR_IN();
    __ST7565R_D3_DIR_IN();
    __ST7565R_D4_DIR_IN();
    __ST7565R_D5_DIR_IN();
    __ST7565R_D6_DIR_IN();
    __ST7565R_D7_DIR_IN();
#endif
    ST7565R_dataBusDir = MODE_INPUT;
  }
  
  for (uint16_t index = (uint16_t)0U; index < length; index++)
  {
    __ST7565R_CS_RESET();           /* Chip Select pin low for start reading */
    
    __ASM ("nop");
    __ASM ("nop");
    
    /* Get status data */
#ifdef ST7565R_USE_DATA_PIN_ARRAY
    __ST7565R_Dx_IN(*(data + index));
#else
    *(data + index)  = (uint8_t)(__ST7565R_D0_IN() << 0U);
    *(data + index) |= (uint8_t)(__ST7565R_D1_IN() << 1U);
    *(data + index) |= (uint8_t)(__ST7565R_D2_IN() << 2U);
    *(data + index) |= (uint8_t)(__ST7565R_D3_IN() << 3U);
    *(data + index) |= (uint8_t)(__ST7565R_D4_IN() << 4U);
    *(data + index) |= (uint8_t)(__ST7565R_D5_IN() << 5U);
    *(data + index) |= (uint8_t)(__ST7565R_D6_IN() << 6U);
    *(data + index) |= (uint8_t)(__ST7565R_D7_IN() << 7U);
#endif
  
    __ST7565R_CS_SET();             /* Chip Select pin high for idle */
  }
  
  __ST7565R_RD_SET();               /* Read pin high for not read */
}
#endif

/**
  * @brief  ST7565R write pattern into DRAM function.
  * @param  data Data value
  * @param  length Data array length
  * @retval None
  */
static void ST7565R_writePattern(uint8_t data, uint16_t length)
{
  __ST7565R_A0_SET();               /* A0 pin high for display data */
  __ST7565R_RD_SET();               /* Read pin high for not read */
  __ST7565R_WR_RESET();             /* Write pin low for write */
  
  __ST7565R_CS_SET();               /* Chip Select pin high for idle */
  
  /* Prepare data output */
#ifdef ST7565R_USE_DATA_PIN_ARRAY
  __ST7565R_Dx_OUT(data);
#else
  if ((data & (uint8_t)0x01) != (uint8_t)0U) { __ST7565R_D0_SET(); } else { __ST7565R_D0_RESET(); }
  if ((data & (uint8_t)0x02) != (uint8_t)0U) { __ST7565R_D1_SET(); } else { __ST7565R_D1_RESET(); }
  if ((data & (uint8_t)0x04) != (uint8_t)0U) { __ST7565R_D2_SET(); } else { __ST7565R_D2_RESET(); }
  if ((data & (uint8_t)0x08) != (uint8_t)0U) { __ST7565R_D3_SET(); } else { __ST7565R_D3_RESET(); }
  if ((data & (uint8_t)0x10) != (uint8_t)0U) { __ST7565R_D4_SET(); } else { __ST7565R_D4_RESET(); }
  if ((data & (uint8_t)0x20) != (uint8_t)0U) { __ST7565R_D5_SET(); } else { __ST7565R_D5_RESET(); }
  if ((data & (uint8_t)0x40) != (uint8_t)0U) { __ST7565R_D6_SET(); } else { __ST7565R_D6_RESET(); }
  if ((data & (uint8_t)0x80) != (uint8_t)0U) { __ST7565R_D7_SET(); } else { __ST7565R_D7_RESET(); }
#endif

  for (uint16_t colIndex = (uint16_t)0U; colIndex <= length; colIndex++)
  {
    __ST7565R_CS_RESET();         /* Chip Select pin low for start writing */
    
    __ASM ("nop");
    __ASM ("nop");
    
    __ST7565R_CS_SET();           /* Chip Select pin high for idle */
    
    __ASM ("nop");
    __ASM ("nop");
  }
  
  __ST7565R_WR_SET();             /* Write pin high for not write */
}

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  ST7565R initialization function
  * @retval Initialization result
  */
HAL_StatusTypeDef ST7565R_Init(void)
{
#ifdef ST7565R_USE_RESET_PIN
  __ST7565R_RST_SET();              /* Reset pin high */
#endif
  __ST7565R_A0_SET();               /* A0 pin high for display data */
  __ST7565R_RD_SET();               /* Read pin high for not read */
  __ST7565R_WR_SET();               /* Write pin high for not write */
  
  __ST7565R_CS_SET();               /* Chip Select pin high for idle */
  
#ifndef ST7565R_USE_DATA_PIN_OUTPUT_ONLY
#ifdef ST7565R_USE_DATA_PIN_ARRAY
  __ST7565R_Dx_DIR_OUT();
#else
  __ST7565R_D0_DIR_OUT();
  __ST7565R_D1_DIR_OUT();
  __ST7565R_D2_DIR_OUT();
  __ST7565R_D3_DIR_OUT();
  __ST7565R_D4_DIR_OUT();
  __ST7565R_D5_DIR_OUT();
  __ST7565R_D6_DIR_OUT();
  __ST7565R_D7_DIR_OUT();
#endif
  ST7565R_dataBusDir = MODE_OUTPUT;
#endif

    /* Clear data output */
#ifdef ST7565R_USE_DATA_PIN_ARRAY
  __ST7565R_Dx_OUT((uint8_t)0x00);
#else
  __ST7565R_D0_RESET();
  __ST7565R_D1_RESET();
  __ST7565R_D2_RESET();
  __ST7565R_D3_RESET();
  __ST7565R_D4_RESET();
  __ST7565R_D5_RESET();
  __ST7565R_D6_RESET();
  __ST7565R_D7_RESET();
#endif

  return HAL_OK;
}



/**
  * @brief  ST7565R set display on/off.
  * @param  onOff On/off state
  * @retval None
  */
void ST7565R_SetDisplayOnOff(uint8_t onOff)
{
  uint8_t cmd = __ST7565R_CMD_DISPLAY_ON_OFF(onOff);
  
  ST7565R_writeCommand(&cmd, (uint16_t)1U);
}

/**
  * @brief  ST7565R set display start line address of the display data RAM.
  * @param  line Start line address [0,63]
  * @retval None
  */
void ST7565R_SetDisplayStartLine(uint8_t line)
{
  if (line > ST7565R_CMD_START_LINE_SET_MAX)
    return;
  
  uint8_t cmd = __ST7565R_CMD_START_LINE_SET(line);
  
  ST7565R_writeCommand(&cmd, (uint16_t)1U);
}

/**
  * @brief  ST7565R set page address.
  * @param  page Page address [0,8]
  * @retval None
  */
void ST7565R_SetPageAddress(uint8_t page)
{
  if (page > ST7565R_CMD_PAGE_ADDR_SET_MAX)
    return;
  
  uint8_t cmd = __ST7565R_CMD_PAGE_ADDR_SET(page);
  
  ST7565R_writeCommand(&cmd, (uint16_t)1U);
}

/**
  * @brief  ST7565R set column address of the display data RAM.
  * @param  column Column address [0,131]
  * @retval None
  */
void ST7565R_SetColumnAddress(uint8_t column)
{
  if (column > ST7565R_CMD_COL_ADDR_SET_MAX)
    return;
  
  uint8_t cmd[2];
  
  cmd[0] = __ST7565R_CMD_COL_ADDR_H_SET(column);
  cmd[1] = __ST7565R_CMD_COL_ADDR_L_SET(column);
  
  ST7565R_writeCommand(cmd, (uint16_t)2U);
}

/**
  * @brief  ST7565R set Segment Driver Direction.
  * @param  adc Segment driver direction
  * @retval None
  */
void ST7565R_SetADC(uint8_t adc)
{
  uint8_t cmd = __ST7565R_CMD_ADC_SEL(adc);
  
  ST7565R_writeCommand(&cmd, (uint16_t)1U);
}

/**
  * @brief  ST7565R reverse the lit and unlit display without overwriting the contents of the display data RAM.
  * @param  inv Normal/Reverse display
  * @retval None
  */
void ST7565R_SetInverseDisplay(uint8_t inv)
{
  uint8_t cmd = __ST7565R_CMD_INVERSE_DISP(inv);
  
  ST7565R_writeCommand(&cmd, (uint16_t)1U);
}

/**
  * @brief  ST7565R force all display points ON regardless of the content of the display data RAM.
  * @param  onOff All pixels on/off
  * @retval None
  */
void ST7565R_SetAllPixelsOnOff(uint8_t onOff)
{
  uint8_t cmd = __ST7565R_CMD_ALL_PIXELS_ON_OFF(onOff);
  
  ST7565R_writeCommand(&cmd, (uint16_t)1U);
}

/**
  * @brief  ST7565R select the voltage bias ratio required for the liquid crystal display.
  * @param  bias Bias level low/high
  * @retval None
  */
void ST7565R_SetBias(uint8_t bias)
{
  uint8_t cmd = __ST7565R_CMD_BIAS_SEL(bias);
  
  ST7565R_writeCommand(&cmd, (uint16_t)1U);
}

/**
  * @brief  ST7565R start/end the read-modify-write mode.
  * @param  rmw read-modify-write start(1)/end(0) command
  * @retval None
  */
void ST7565R_SetReadModifyWrite(uint8_t rmw)
{
  uint8_t cmd = ((rmw != (uint8_t)0U) ? __ST7565R_CMD_RD_MOD_WR_START() : __ST7565R_CMD_RD_MOD_WR_END());
  
  ST7565R_writeCommand(&cmd, (uint16_t)1U);
}

/**
  * @brief  ST7565R hardware/software reset.
  * @param  reset Hardware(0)/Software(1) reset
  * @retval None
  */
void ST7565R_Reset(uint8_t reset)
{
#ifdef ST7565R_USE_RESET_PIN
  /* Hardware reset */
  if (reset == (uint8_t)0U)
  {
    __ST7565R_RST_RESET();          /* RST pin low for start hardware reset */
    
    /* Wait for Reset "L" pulse width */
    HAL_Delay(1);                   /* tRW should be greater than 2us */
    
    __ST7565R_RST_SET();            /* RST pin high for stop hardware reset */
    
    /* Wait for reset completion */
    HAL_Delay(1);                   /* tR should be greater than 2us */
  }
  /* Software reset */
  else
#endif
  {
    uint8_t cmd = __ST7565R_CMD_SOFT_RESET();
    
    ST7565R_writeCommand(&cmd, (uint16_t)1U);
  }
}

/**
  * @brief  ST7565R select the scan direction of the Common Output Mode (COM) output terminal.
  * @param  com Normal(0)/Reverse COM
  * @retval None
  */
void ST7565R_SetCOM(uint8_t com)
{
  uint8_t cmd = __ST7565R_CMD_COM_SEL(com);
  
  ST7565R_writeCommand(&cmd, (uint16_t)1U);
}

/**
  * @brief  ST7565R set the power supply circuit functions.
  * @param  boost Booster circuit on/off
  * @param  volreg Voltage regulator circuit on/off
  * @param  volflw Voltage follower circuit on/off
  * @retval None
  */
void ST7565R_SetPowerController(uint8_t boost, uint8_t volreg, uint8_t volflw)
{
  uint8_t cmd = __ST7565R_CMD_PWR_CTRL_SET(boost | volreg | volflw);
  
  ST7565R_writeCommand(&cmd, (uint16_t)1U);
}

/**
  * @brief  ST7565R set the V0 voltage regulator internal resistor ratio.
  * @param  ratio Voltage regulator ratio [0,7]
  * @retval None
  */
void ST7565R_SetVolRegRatio(uint8_t ratio)
{
  if (ratio > ST7565R_CMD_VREG_RATIO_SET_MAX)
    return;
  
  uint8_t cmd = __ST7565R_CMD_VREG_RATIO_SET(ratio);
  
  ST7565R_writeCommand(&cmd, (uint16_t)1U);
}

/**
  * @brief  ST7565R adjust the brightness of the liquid crystal display.
  * @param  vol Electronic volume [0,63]
  * @retval None
  */
void ST7565R_SetElecVolume(uint8_t vol)
{
  if (vol > ST7565R_CMD_EV_SET_MAX)
    return;
  
  uint8_t cmd[2];
  
  cmd[0] = __ST7565R_CMD_EV_MODE_SET();
  cmd[1] = __ST7565R_CMD_EV_REG_SET(vol);
  
  ST7565R_writeCommand(cmd, (uint16_t)2U);
}

/**
  * @brief  ST7565R stops all operations in the LCD display system, and as long as there are no accesses from the MPU, the
  *         consumption current is reduced to a value near the static current. The internal modes during sleep mode are as follows:
  *            1. The oscillator circuit and the LCD power supply circuit are halted.
  *            2. All liquid crystal drive circuits are halted, and the segment in common drive outputs output a VSS level.
  * @param  sleep Sleep(0)/Normal(1) mode
  * @retval None
  */
void ST7565R_Sleep(uint8_t sleep)
{
  uint8_t cmd[2];
  
  cmd[0] = __ST7565R_CMD_SLEEP_PRED(sleep);
  cmd[1] = __ST7565R_CMD_SLEEP_FOLW();
  
  ST7565R_writeCommand(cmd, (uint16_t)2U);
}

/**
  * @brief  ST7565R select step-up ratio.
  * @param  ratio Booster ratio (4x,5x,6x)
  * @retval None
  */
void ST7565R_SetBoosterRatio(uint8_t ratio)
{
  uint8_t cmd[2];
  
  cmd[0] = __ST7565R_CMD_BOOST_RATIO_MODE_SET();
  cmd[1] = __ST7565R_CMD_BOOST_RATIO_REG_SET(ratio);
  
  ST7565R_writeCommand(cmd, (uint16_t)2U);
}

/**
  * @brief  ST7565R write display data into DRAMs.
  * @retval None
  */
void ST7565R_WriteData(uint8_t* data, uint16_t length)
{
  ST7565R_writeData(data, length);
}

/**
  * @brief  ST7565R write display pattern into DRAMs.
  * @retval None
  */
void ST7565R_WritePattern(uint8_t data, uint16_t length)
{
  ST7565R_writePattern(data, length);
}

#ifndef ST7565R_USE_DATA_PIN_OUTPUT_ONLY
/**
  * @brief  ST7565R read display data from DRAMs.
  * @retval None
  */
void ST7565R_ReadData(uint8_t* data, uint16_t length)
{
  ST7565R_readData(data, length);
}
#endif

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
