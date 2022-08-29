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
#include "lcd.h"

/* Private includes ----------------------------------------------------------*/
#include "st7565r.h"

#include "tim.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/**
  * @brief  LCD operation state enumeration
  */
enum
{
  LCD_STATE_UNINIT = (uint8_t)0U,
  LCD_STATE_HARDWARE_RESET,
  LCD_STATE_SETUP_PARAMETER,
  LCD_STATE_CLEAR_DRAM,
  LCD_STATE_DISPLAY_ON,
  LCD_STATE_READY,
  LCD_STATE_ERROR = (uint8_t)0xFF
};

/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
#define LCD_PAGES_FILLED_PER_LOOP                     (uint8_t)2U

/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
static Font_ConfigType* lcd_fontCfg;
static uint8_t lcd_xPos, lcd_yPos;

#ifdef LCD_USE_GRAPHIC_MODE
static uint8_t lcd_buf[ST7565R_LCD_WIDTH_PIXELS_NUM * ST7565R_LCD_HEIGHT_PAGES_NUM];
static uint8_t lcd_startPage;
static uint8_t lcd_currentPage;
static uint8_t lcd_pageNum;

static uint8_t lcd_isFilled;
#endif

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
#ifdef LCD_USE_GRAPHIC_MODE
static void LCD_drawPixel(uint8_t xPos, uint8_t yPos, uint8_t isSet);
#endif

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
#ifdef LCD_USE_GRAPHIC_MODE
/**
  * @brief  LCD set pixel into display buffer.
  * @param  xPos  Horizontal position
  * @param  yPos  Vertical position
  * @param  isSet Indicate this pixel to be set (1) or clear (0)
  * @retval None
  */
static void LCD_drawPixel(uint8_t xPos, uint8_t yPos, uint8_t isSet)
{
	/* Set pixel */
	if (isSet != (uint8_t)0U)
  {
		lcd_buf[xPos + (yPos / ST7565R_PAGE_PIXELS_NUM) * ST7565R_LCD_WIDTH_PIXELS_NUM] |= (uint8_t)0x01 << (yPos % ST7565R_PAGE_PIXELS_NUM);
	}
  else
  {
		lcd_buf[xPos + (yPos / ST7565R_PAGE_PIXELS_NUM) * ST7565R_LCD_WIDTH_PIXELS_NUM] &= (uint8_t)(~((uint8_t)0x01 << (yPos % ST7565R_PAGE_PIXELS_NUM)));
	}
}
#endif

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  LCD initialization function
  * @retval Initialize state
  */
HAL_StatusTypeDef LCD_Init(void)
{
  uint8_t pageIndex;
  
  /* Initialize LCD internal variable */
  lcd_xPos = (uint8_t)0U;
  lcd_yPos = (uint8_t)0U;
  
  lcd_fontCfg = &font6x8;
      
#ifdef LCD_USE_GRAPHIC_MODE
  memset(lcd_buf, (uint8_t)0U, ST7565R_LCD_WIDTH_PIXELS_NUM * ST7565R_LCD_HEIGHT_PAGES_NUM);
  
  lcd_startPage = (uint8_t)0U;
  lcd_currentPage = (uint8_t)0U;
  lcd_pageNum = (uint8_t)0U;
  
  lcd_isFilled = (uint8_t)1U;
#endif
      
  /* Reset hardware */
  ST7565R_Reset((uint8_t)0U);
  
  /* LCD bias setting */
  ST7565R_SetBias(ST7565R_BiasHigh);
  /* ADC selection */
  ST7565R_SetADC(ST7565R_ADCNormal);
  /* Common output mode selection */
  ST7565R_SetCOM(ST7565R_COMDirReverse);
  
  /* Set volatage regulator ratio */
  ST7565R_SetVolRegRatio((uint8_t)5U);
  /* Electronic volume control */
  ST7565R_SetElecVolume((uint8_t)10U);
  
  /* Power control setting */
  ST7565R_SetPowerController(ST7565R_PwrCtrlBoosterOn, ST7565R_PwrCtrlVolRegOn, ST7565R_PwrCtrlVolFlwOn);
  
  /* Select booster ratio */
  ST7565R_SetBoosterRatio(ST7565R_BoostRatio4x);
  
  /* Set initial display start line */
  ST7565R_SetDisplayStartLine((uint8_t)0U);
  
  /* Clear DRAM */
  for (pageIndex = (uint8_t)0U; pageIndex < ST7565R_LCD_HEIGHT_PAGES_NUM; pageIndex++)
  {
    ST7565R_SetPageAddress(pageIndex);
    ST7565R_SetColumnAddress((uint8_t)0U);
    
    ST7565R_WritePattern((uint8_t)0U, ST7565R_LCD_WIDTH_PIXELS_NUM);
  }
  
  /* Set initial page address */
  ST7565R_SetPageAddress((uint8_t)0U);
  /* Set initial column address */
  ST7565R_SetColumnAddress((uint8_t)0U);
  
  /* Set display ON */
  ST7565R_SetDisplayOnOff(ST7565R_DisplayOn);
  
  return HAL_OK;
}

#ifdef LCD_USE_GRAPHIC_MODE
/**
  * @brief  LCD clear display buffer.
  * @param  startPage Start page
  * @param  pageNum number of page to be cleared
  * @retval None
  */
void LCD_ClearPage(uint8_t startPage, uint8_t pageNum)
{
  memset \
  ( \
    (lcd_buf + (uint32_t)((uint32_t)startPage * ST7565R_LCD_WIDTH_PIXELS_NUM)), \
    (uint8_t)0U, \
    (uint32_t)((uint32_t)pageNum * ST7565R_LCD_WIDTH_PIXELS_NUM) \
  );
}

/**
  * @brief  LCD fill screen.
  * @param  startPage Start page
  * @param  pageNum Number of page to be filled
  * @param  fillNow Perform fill screen immediately (1) or polling fill main function (0)
  * @retval None
  */
void LCD_FillPage(uint8_t startPage, uint8_t pageNum, uint8_t fillNow)
{
  lcd_startPage = startPage;
  lcd_currentPage = startPage;
  
  lcd_pageNum = pageNum;
  
  if (fillNow == (uint8_t)0U)
  {
    lcd_isFilled = (uint8_t)0U;
  }
  else
  {
    for (; lcd_currentPage < (lcd_startPage + lcd_pageNum); lcd_currentPage++)
    {
      /* Set initial page address */
      ST7565R_SetPageAddress(lcd_currentPage);
      /* Set initial column address */
      ST7565R_SetColumnAddress((uint8_t)0U);
      
      ST7565R_WriteData((lcd_buf + (lcd_currentPage * ST7565R_LCD_WIDTH_PIXELS_NUM)), ST7565R_LCD_WIDTH_PIXELS_NUM);
    }
  }
}

/**
  * @brief  LCD fill main function, should be called periodically.
  * @retval None
  */
void LCD_FillMain(void)
{
  uint8_t pageAddr;
  
  if (lcd_isFilled == (uint8_t)0U)
  {
    for (pageAddr = (uint8_t)0U; (pageAddr < LCD_PAGES_FILLED_PER_LOOP) && (lcd_currentPage < (lcd_startPage + lcd_pageNum)); pageAddr++, lcd_currentPage++)
    {
      /* Set initial page address */
      ST7565R_SetPageAddress(lcd_currentPage);
      /* Set initial column address */
      ST7565R_SetColumnAddress((uint8_t)0U);
      
      ST7565R_WriteData((lcd_buf + (lcd_currentPage * ST7565R_LCD_WIDTH_PIXELS_NUM)), ST7565R_LCD_WIDTH_PIXELS_NUM);
    }
    
    if (lcd_currentPage >= (lcd_startPage + lcd_pageNum))
    {
      lcd_isFilled = (uint8_t)1U;
    }
  }
}
#endif

/**
  * @brief  LCD set font type.
  * @param  fontIdx Font index
  * @retval None
  */
void LCD_SetFont(uint8_t fontIdx)
{
  switch (fontIdx)
  {
    case LCD_FONT_SMALL:
    {
      lcd_fontCfg = &font6x8;
      break;
    }
    case LCD_FONT_BIG:
    {
      lcd_fontCfg = &font8x16;
      break;
    }
    default:
    {
      lcd_fontCfg = &font6x8;
      break;
    }
  }
}

/**
  * @brief  LCD set cursor position.
  * @param  xPos Horizontal position
  * @param  yPos Vertical position
  * @param  inPixel Horizontal position in pixel (1) or font width times (0)
  * @retval None
  */
void LCD_SetCursorPos(uint8_t xPos, uint8_t yPos, uint8_t xPosInPixel)
{
#ifdef LCD_USE_GRAPHIC_MODE
  if (xPosInPixel != (uint8_t)0U)
  {
    /* In pixel */
    lcd_xPos = xPos;
    lcd_yPos = yPos;
  }
  else
  {
    /* In font width/height times */
    lcd_xPos = xPos * lcd_fontCfg->bitWidth;
    lcd_yPos = yPos * lcd_fontCfg->bitHeight;
  }
#else
  /* Calculate page address from vertical position */
  lcd_yPos = yPos * (lcd_fontCfg->bitHeight / ST7565R_PAGE_PIXELS_NUM);
  
  /* Calculate column address from horizontal position */
  if (xPosInPixel == (uint8_t)0U)
  {
    /* By font width times */
    lcd_xPos = xPos * lcd_fontCfg->bitWidth;
  }
  else
  {
    /* By pixel */
    lcd_xPos = xPos;
  }
  
  ST7565R_SetPageAddress(lcd_yPos);
  ST7565R_SetColumnAddress(lcd_xPos);
#endif
}

/**
  * @brief  LCD print a character.
  * @param  chr ASCII Character index
  * @retval None
  */
void LCD_PutChar(uint8_t chr)
{
#ifdef LCD_USE_GRAPHIC_MODE
  uint32_t xIndex, yIndex, byteVal;
	
	/* Check available space in display buffer */
	if ( \
      (ST7565R_LCD_WIDTH_PIXELS_NUM < (lcd_xPos + lcd_fontCfg->bitWidth))  || \
      (ST7565R_LCD_HEIGHT_PIXELS_NUM < (lcd_yPos + lcd_fontCfg->bitHeight)) \
     )
  {
		/* Not enough space, return immediately */
		return;
	}
	
	/* Go through font */
  if (lcd_fontCfg == &font6x8)
  {
    for (xIndex = (uint32_t)0U; xIndex < lcd_fontCfg->bitWidth; xIndex++) 
    {
      byteVal = lcd_fontCfg->fontArr[(uint32_t)chr * (uint32_t)(lcd_fontCfg->bitWidth) + xIndex];
      
      for (yIndex = (uint32_t)0U; yIndex < lcd_fontCfg->bitHeight; yIndex++)
      {
        LCD_drawPixel(lcd_xPos + xIndex, lcd_yPos + yIndex, ((byteVal >> yIndex) & (uint8_t)0x01));
      }
    }
  }
  else if (lcd_fontCfg == &font8x16)
  {
    for (xIndex = (uint32_t)0U; xIndex < lcd_fontCfg->bitWidth; xIndex++) 
    {
      byteVal = lcd_fontCfg->fontArr[(uint32_t)chr * (uint32_t)(lcd_fontCfg->bitWidth) * (uint32_t)2U + xIndex];
      
      for (yIndex = (uint32_t)0U; yIndex < (lcd_fontCfg->bitHeight / 2U); yIndex++)
      {
        LCD_drawPixel(lcd_xPos + xIndex, lcd_yPos + yIndex, ((byteVal >> yIndex) & (uint8_t)0x01));
      }
      
      byteVal = lcd_fontCfg->fontArr[(uint32_t)chr * (uint32_t)(lcd_fontCfg->bitWidth) * (uint32_t)2U + (uint32_t)(lcd_fontCfg->bitWidth) + xIndex];
      
      for (yIndex = (uint32_t)0U; yIndex < (lcd_fontCfg->bitHeight / 2U); yIndex++)
      {
        LCD_drawPixel(lcd_xPos + xIndex, lcd_yPos + (lcd_fontCfg->bitHeight / 2U) + yIndex, ((byteVal >> yIndex) & (uint8_t)0x01));
      }
    }
  }
  else
  {
    /* Should not get into here */
  }
	
	/* Increase pointer */
	lcd_xPos += lcd_fontCfg->bitWidth;
#else
  uint8_t pageNum;
  
  pageNum = lcd_fontCfg->bitHeight / ST7565R_PAGE_PIXELS_NUM;
  
  if (pageNum == (uint8_t)1U)
  {
    if ((lcd_xPos + lcd_fontCfg->bitWidth) < ST7565R_LCD_WIDTH_PIXELS_NUM)
    {
      ST7565R_WriteData((uint8_t *)(&((lcd_fontCfg->fontArr)[chr * lcd_fontCfg->bitWidth])), lcd_fontCfg->bitWidth);
      
      lcd_xPos += lcd_fontCfg->bitWidth;
    }
  }
  else if (pageNum == (uint8_t)2U)
  {
    if ( \
        ((lcd_xPos + lcd_fontCfg->bitWidth) < ST7565R_LCD_WIDTH_PIXELS_NUM) && \
        ((lcd_yPos + pageNum) <= ST7565R_LCD_HEIGHT_PAGES_NUM) \
       )
    {
      ST7565R_WriteData((uint8_t *)(&((lcd_fontCfg->fontArr)[chr * lcd_fontCfg->bitWidth * pageNum])), lcd_fontCfg->bitWidth);
      
      lcd_yPos++;
      
      ST7565R_SetPageAddress(lcd_yPos);
      ST7565R_SetColumnAddress(lcd_xPos);
      
      ST7565R_WriteData((uint8_t *)(&((lcd_fontCfg->fontArr)[chr * lcd_fontCfg->bitWidth * pageNum + lcd_fontCfg->bitWidth])), lcd_fontCfg->bitWidth);
    
      lcd_xPos = lcd_xPos + lcd_fontCfg->bitWidth;
      lcd_yPos--;
      
      ST7565R_SetPageAddress(lcd_yPos);
      ST7565R_SetColumnAddress(lcd_xPos);
    }
  }
  else
  {
    /* Should not get into here */
  }
#endif
}

/**
  * @brief  LCD print string.
  * @param  str String pointer
  * @retval None
  */
void LCD_PutString(uint8_t* str)
{
#ifdef LCD_USE_GRAPHIC_MODE
  uint8_t strLength, chrIndex;
  uint32_t xIndex, yIndex, byteVal;
  
  strLength = strlen((char *)str);
	
	/* Check available space in display buffer */
	if ( \
      (ST7565R_LCD_WIDTH_PIXELS_NUM < (lcd_xPos + lcd_fontCfg->bitWidth * strLength)) || \
      (ST7565R_LCD_HEIGHT_PIXELS_NUM < (lcd_yPos + lcd_fontCfg->bitHeight)) \
     )
  {
		/* Not enough space, return immediately */
		return;
	}
	
	/* Go through font */
  if (lcd_fontCfg == &font6x8)
  {
    for (chrIndex = (uint8_t)0U; chrIndex < strLength; chrIndex++)
    {
      for (xIndex = (uint32_t)0U; xIndex < lcd_fontCfg->bitWidth; xIndex++) 
      {
        byteVal = lcd_fontCfg->fontArr[(uint32_t)(*(str + chrIndex)) * (uint32_t)(lcd_fontCfg->bitWidth) + xIndex];
        
        for (yIndex = (uint32_t)0U; yIndex < lcd_fontCfg->bitHeight; yIndex++)
        {
          LCD_drawPixel(lcd_xPos + xIndex, lcd_yPos + yIndex, ((byteVal >> yIndex) & (uint8_t)0x01));
        }
      }
      
      /* Increase pointer */
      lcd_xPos += lcd_fontCfg->bitWidth;
    }
  }
  else if (lcd_fontCfg == &font8x16)
  {
    for (chrIndex = (uint8_t)0U; chrIndex < strLength; chrIndex++)
    {
      for (xIndex = (uint32_t)0U; xIndex < lcd_fontCfg->bitWidth; xIndex++) 
      {
        byteVal = lcd_fontCfg->fontArr[(uint32_t)(*(str + chrIndex)) * (uint32_t)(lcd_fontCfg->bitWidth) * (uint32_t)2U + xIndex];
        
        for (yIndex = (uint32_t)0U; yIndex < (lcd_fontCfg->bitHeight / 2U); yIndex++)
        {
          LCD_drawPixel(lcd_xPos + xIndex, lcd_yPos + yIndex, ((byteVal >> yIndex) & (uint8_t)0x01));
        }
        
        byteVal = lcd_fontCfg->fontArr[(uint32_t)(*(str + chrIndex)) * (uint32_t)(lcd_fontCfg->bitWidth) * (uint32_t)2U + (uint32_t)(lcd_fontCfg->bitWidth) + xIndex];
        
        for (yIndex = (uint32_t)0U; yIndex < (lcd_fontCfg->bitHeight / 2U); yIndex++)
        {
          LCD_drawPixel(lcd_xPos + xIndex, lcd_yPos + (lcd_fontCfg->bitHeight / 2U) + yIndex, ((byteVal >> yIndex) & (uint8_t)0x01));
        }
      }
      
      /* Increase pointer */
      lcd_xPos += lcd_fontCfg->bitWidth;
    }
  }
  else
  {
    /* Should not get into here */
  }
#else
  uint8_t strLength, availLength, length;
  uint8_t pageNum;
  
  pageNum = lcd_fontCfg->bitHeight / ST7565R_PAGE_PIXELS_NUM;
  
  if (pageNum == (uint8_t)1U)
  {
    strLength = strlen(str);
    availLength = (ST7565R_LCD_WIDTH_PIXELS_NUM - lcd_xPos) / lcd_fontCfg->bitWidth;
    
    length = (strLength <= availLength) ? strLength : availLength;
    
    for (uint8_t index = (uint8_t)0U; index < length; index++)
    {
      ST7565R_WriteData((uint8_t *)(&((lcd_fontCfg->fontArr)[*(str + index) * lcd_fontCfg->bitWidth])), lcd_fontCfg->bitWidth);
    }
    
    lcd_xPos += (lcd_fontCfg->bitWidth * length);
  }
  else if (pageNum == (uint8_t)2U)
  {
    if ((lcd_yPos + pageNum) <= ST7565R_LCD_HEIGHT_PAGES_NUM)
    {
      strLength = strlen(str);
      availLength = (ST7565R_LCD_WIDTH_PIXELS_NUM - lcd_xPos) / lcd_fontCfg->bitWidth;
      
      length = (strLength <= availLength) ? strLength : availLength;
      
      for (uint8_t index = (uint8_t)0U; index < length; index++)
      {
        ST7565R_WriteData((uint8_t *)(&((lcd_fontCfg->fontArr)[*(str + index) * lcd_fontCfg->bitWidth * pageNum])), lcd_fontCfg->bitWidth);
      
        lcd_yPos++;
        
        ST7565R_SetPageAddress(lcd_yPos);
        ST7565R_SetColumnAddress(lcd_xPos);
        
        ST7565R_WriteData((uint8_t *)(&((lcd_fontCfg->fontArr)[*(str + index) * lcd_fontCfg->bitWidth * pageNum + lcd_fontCfg->bitWidth])), lcd_fontCfg->bitWidth);
      
        lcd_xPos = lcd_xPos + lcd_fontCfg->bitWidth;
        lcd_yPos--;
        
        ST7565R_SetPageAddress(lcd_yPos);
        ST7565R_SetColumnAddress(lcd_xPos);
      }
    }
  }
#endif
}

/**
  * @brief  LCD get horizontal position to align given string into center.
  * @param  str String to be center
  * @retval Horizontal position
  */
uint8_t LCD_GetXPosCenter(uint8_t* str)
{
  uint8_t length;
	uint8_t xPos;
	
	length = (uint8_t)(strlen((char *)str));
  
  xPos = (uint8_t)((uint8_t)ST7565R_LCD_WIDTH_PIXELS_NUM - (uint8_t)(length * lcd_fontCfg->bitWidth)) / (uint8_t)2U;
  
  return xPos;
}

#ifdef LCD_USE_GRAPHIC_MODE
/**
  * @brief  LCD draw horizontal line.
  * @param  length Line length to the right
  * @retval None
  */
void LCD_PutHorizontalLine(uint8_t length)
{
  uint8_t index;
  
  /* Check available space in display buffer */
  if (ST7565R_LCD_WIDTH_PIXELS_NUM < (lcd_xPos + length))
  {
    /* Not enough space, return immediately */
    return;
  }
  
  for (index = (uint8_t)0U; index < length; index++)
  {
    LCD_drawPixel(lcd_xPos + index, lcd_yPos, (uint8_t)1U);
  }
}

/**
  * @brief  LCD draw vertical line.
  * @param  length Line length down
  * @retval None
  */
void LCD_PutVerticalLine(uint8_t length)
{
  uint8_t index;
  
  /* Check available space in display buffer */
  if (ST7565R_LCD_HEIGHT_PIXELS_NUM < (lcd_yPos + length))
  {
    /* Not enough space, return immediately */
    return;
  }
  
  for (index = (uint8_t)0U; index < length; index++)
  {
    LCD_drawPixel(lcd_xPos, lcd_yPos + index, (uint8_t)1U);
  }
}

/**
  * @brief  LCD draw rectangle.
  * @param  width Rectangle width to the right
  * @param  height Rectangle height down
  * @retval None
  */
void LCD_PutRectangle(uint8_t width, uint8_t height)
{
  uint8_t curXPos, curYPos;
  
  curXPos = lcd_xPos;
  curYPos = lcd_yPos;
  
  LCD_PutHorizontalLine(width);
  LCD_PutVerticalLine(height);
  
  LCD_SetCursorPos(curXPos, curYPos + height - 1U, (uint8_t)1U);
  LCD_PutHorizontalLine(width);
  
  LCD_SetCursorPos(curXPos + width - 1U, curYPos, (uint8_t)1U);
  LCD_PutVerticalLine(height);
  
  LCD_SetCursorPos(curXPos, curYPos, (uint8_t)1U);
}
#endif

/**
  * @brief  LCD set contrast.
  * @param  contrast Contrast value [0,100]
  * @retval None
  */
void LCD_SetContrast(uint8_t contrast)
{
  TIM2_SetDutyCycle(contrast);
}



/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
