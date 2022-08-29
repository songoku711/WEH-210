/* 
 * File:   MenuManager_Common.c
 * Author: Long
 *
 * Created on September 15, 2019, 11:06 AM
 */

#ifdef  __cplusplus
extern "C" {
#endif



/*===============================================================================================
*                                         INCLUDE FILES
* 1) system and project includes
* 2) needed interfaces from external units
* 3) internal and external interfaces from this unit
===============================================================================================*/

#include "menuManager.h"



/*===============================================================================================
*                                       DEFINES AND MACROS
===============================================================================================*/




/*===============================================================================================
*                                       GLOBAL VARIABLES
===============================================================================================*/

extern osMessageQueueId_t ioButtonStateQueueHandle;



/*===============================================================================================
*                                   LOCAL FUNCTION PROTOTYPES
===============================================================================================*/




/*===============================================================================================
*                                       LOCAL FUNCTIONS
===============================================================================================*/




/*===============================================================================================
*                                       GLOBAL FUNCTIONS
===============================================================================================*/

void MenuManager_Common_ButEventMapHandler(MenuManager_ButEventMapConfStruct *butEventMapConf)
{
  uint8_t butEventMapIdx;
  uint32_t ioButtonState;
  osStatus_t status;
  
  status = osMessageQueueGet(ioButtonStateQueueHandle, &ioButtonState, 0U, 0U);
  
  if (status == osOK)
  {
    for (butEventMapIdx = (uint8_t)0U; butEventMapIdx < butEventMapConf->butEventMapNum; butEventMapIdx++)
    {
      if (ioButtonState == (*(butEventMapConf->butEventMapCfg))[butEventMapIdx].butState)
      {
        Fsm_TriggerEvent(&MenuManager_FsmContext, (tFsmEvent)((*(butEventMapConf->butEventMapCfg))[butEventMapIdx].butEvent));
      
        break;
      }
    }
  }
}



/*=============================================================================================*/
tFsmGuard MenuManager_Common_Exit(tFsmContextPtr const pFsmContext, tFsmEvent event)
{
  MenuManager_SubMainFunction = NULL;
  MenuManager_SubTickHandler = NULL;
  
  return kFsmGuard_True;
}



/*=============================================================================================*/
uint32_t MenuManager_Common_LcdGetCurPosFirstTime(uint32_t listIndex, uint32_t listItemNumber)
{
  uint32_t cursorPos;
  
  if (listItemNumber <= (uint32_t)MENUMANAGER_COMMON_LCD_CURSOR_LENGTH)
  {
    cursorPos = listIndex + (uint32_t)MENUMANAGER_COMMON_LCD_CURSOR_MIN;
  }
  else
  {
    if (listIndex <= (listItemNumber - (uint32_t)MENUMANAGER_COMMON_LCD_CURSOR_LENGTH))
    {
      cursorPos = (uint32_t)MENUMANAGER_COMMON_LCD_CURSOR_MIN;
    }
    else
    {
      cursorPos = listIndex - (listItemNumber - (uint32_t)MENUMANAGER_COMMON_LCD_CURSOR_LENGTH) \
                            + (uint32_t)MENUMANAGER_COMMON_LCD_CURSOR_MIN;
    }
  }
  
  return cursorPos;
}

/*=============================================================================================*/
uint32_t MenuManager_Common_LcdGetStartListIndex(uint32_t listIndex, uint32_t cursorPos)
{
  return (listIndex - (cursorPos - (uint32_t)MENUMANAGER_COMMON_LCD_CURSOR_MIN));
}

/*=============================================================================================*/
uint32_t MenuManager_Common_LcdGetListLength(uint32_t listItemNumber)
{
  return (listItemNumber <= (uint32_t)MENUMANAGER_COMMON_LCD_CURSOR_LENGTH) ? \
          listItemNumber : \
          (uint32_t)MENUMANAGER_COMMON_LCD_CURSOR_LENGTH;
}

/*=============================================================================================*/
uint32_t MenuManager_Common_LcdMaxCurPos(uint32_t listItemNumber)
{
  return (listItemNumber <= (uint32_t)MENUMANAGER_COMMON_LCD_CURSOR_LENGTH) ? \
          (listItemNumber + (uint32_t)MENUMANAGER_COMMON_LCD_CURSOR_MIN - (uint32_t)1U) : \
          (uint32_t)MENUMANAGER_COMMON_LCD_CURSOR_MAX;
}

/*=============================================================================================*/
void MenuManager_Common_LcdClearAll(void)
{
  LCD_ClearPage(0, 8);
}

/*=============================================================================================*/
void MenuManager_Common_LcdUpdateAll(void)
{
  LCD_FillPage(0, 8, LCD_FILL_POLLING);
}

/*=============================================================================================*/
void MenuManager_Common_LcdClearElementMenuStatic(void)
{
  LCD_ClearPage(0, 2);
}

/*=============================================================================================*/
void MenuManager_Common_LcdClearElementMenuDynamic(void)
{
  LCD_ClearPage(2, 6);
}

/*=============================================================================================*/
void MenuManager_Common_LcdUpdateElementMenuStatic(void)
{
  LCD_FillPage(0, 2, LCD_FILL_IMMEDIATE);
}

/*=============================================================================================*/
void MenuManager_Common_LcdUpdateElementMenuDynamic(void)
{
  LCD_FillPage(2, 6, LCD_FILL_POLLING);
}

/*=============================================================================================*/
void MenuManager_Common_LcdClearAdjustMenuStatic(void)
{
  LCD_ClearPage(0, 4);
  LCD_ClearPage(7, 1);
}

/*=============================================================================================*/
void MenuManager_Common_LcdClearAdjustMenuDynamic(void)
{
  LCD_ClearPage(4, 3);
}

/*=============================================================================================*/
void MenuManager_Common_LcdUpdateAdjustMenuStatic(void)
{
  LCD_FillPage(0, 4, LCD_FILL_IMMEDIATE);
  LCD_FillPage(7, 1, LCD_FILL_IMMEDIATE);
}

/*=============================================================================================*/
void MenuManager_Common_LcdUpdateAdjustMenuDynamic(void)
{
  LCD_FillPage(4, 3, LCD_FILL_POLLING);
}

/*=============================================================================================*/
void MenuManager_Common_LcdClearCountdownMenuStatic(void)
{
  LCD_ClearPage(0, 4);
}

/*=============================================================================================*/
void MenuManager_Common_LcdClearCountdownMenuDynamic(void)
{
  LCD_ClearPage(4, 4);
}

/*=============================================================================================*/
void MenuManager_Common_LcdUpdateCountdownMenuStatic(void)
{
  LCD_FillPage(0, 4, LCD_FILL_IMMEDIATE);
}

/*=============================================================================================*/
void MenuManager_Common_LcdUpdateCountdownMenuDynamic(void)
{
  LCD_FillPage(4, 4, LCD_FILL_POLLING);
}

/*=============================================================================================*/
void MenuManager_Common_LcdShowMainTitle(const uint8_t *mainTitle)
{
  uint8_t xPos;

  xPos = LCD_GetXPosCenter((uint8_t *)mainTitle);

  LCD_SetFont(LCD_FONT_BIG);
  LCD_SetCursorPos(xPos, 0, LCD_CURSOR_IN_PIXEL);
  LCD_PutString((uint8_t *)mainTitle);
}

/*=============================================================================================*/
void MenuManager_Common_LcdShowChosenMark(uint8_t curPos)
{
  LCD_SetFont(LCD_FONT_SMALL);
	LCD_SetCursorPos(0, curPos, LCD_CURSOR_BY_FONT);
  LCD_PutChar(FONT_RIGHT_ARROW_INDEX);
}

/*=============================================================================================*/
void MenuManager_Common_LcdShowListElementName
(
	uint8_t* str,
	uint8_t length,
	uint8_t xPos,
	uint8_t yPos
)
{
	uint8_t index;
	uint8_t strLength;
	
	strLength = (uint8_t)(strlen(str));
	
  LCD_SetFont(LCD_FONT_SMALL);
	LCD_SetCursorPos(xPos, yPos, LCD_CURSOR_BY_FONT);
	
	if (strLength >= length)
	{
		for (index = (uint8_t)0U; index < length; index++)
		{
			LCD_PutChar(*(str + index));
		}
	}
	else
	{
		for (index = (uint8_t)0U; index < strLength; index++)
		{
			LCD_PutChar(*(str + index));
		}
		
		for (index = strLength; index < length; index++)
		{
			LCD_PutChar(' ');
		}
	}
}

/*=============================================================================================*/
void MenuManager_Common_LcdShowExitChoice(void)
{
  LCD_SetFont(LCD_FONT_SMALL);
  LCD_SetCursorPos(53, 56, LCD_CURSOR_IN_PIXEL);
  LCD_PutString("EXIT");
}

/*=============================================================================================*/
void MenuManager_Common_LcdShowOkExitChoice(void)
{
  LCD_SetFont(LCD_FONT_SMALL);
  
  LCD_SetCursorPos(37, 56, LCD_CURSOR_IN_PIXEL);
  LCD_PutString("OK");
  
  LCD_SetCursorPos(73, 56, LCD_CURSOR_IN_PIXEL);
  LCD_PutString("EXIT");
}

/*=============================================================================================*/
void MenuManager_Common_LcdShowStopExitChoice(void)
{
  LCD_SetFont(LCD_FONT_SMALL);
  
  LCD_SetCursorPos(31, 56, LCD_CURSOR_IN_PIXEL);
  LCD_PutString("STOP");
  
  LCD_SetCursorPos(73, 56, LCD_CURSOR_IN_PIXEL);
  LCD_PutString("EXIT");
}

/*=============================================================================================*/
void MenuManager_Common_LcdShowStartExitChoice(void)
{
  LCD_SetFont(LCD_FONT_SMALL);
  
  LCD_SetCursorPos(28, 56, LCD_CURSOR_IN_PIXEL);
  LCD_PutString("START");
  
  LCD_SetCursorPos(73, 56, LCD_CURSOR_IN_PIXEL);
  LCD_PutString("EXIT");
}

/*=============================================================================================*/
void MenuManager_Common_LcdShowScrollBar(uint8_t listIndex, uint8_t cursorPosition, uint8_t listItemNumber)
{
  LCD_SetFont(LCD_FONT_SMALL);
  
  if (listItemNumber < (uint8_t)6U)
  {
    LCD_SetCursorPos(122, 16, LCD_CURSOR_IN_PIXEL);
    LCD_PutChar(' ');
    
    LCD_SetCursorPos(122, 56, LCD_CURSOR_IN_PIXEL);
    LCD_PutChar(' ');
  }
  else
  {
    if (listIndex == (uint8_t)0U)
    {
      LCD_SetCursorPos(122, 16, LCD_CURSOR_IN_PIXEL);
      LCD_PutChar(' ');
      
      LCD_SetCursorPos(122, 56, LCD_CURSOR_IN_PIXEL);
      LCD_PutChar(FONT_DOWN_ARROW_INDEX);
    }
    else if (listIndex == (listItemNumber - (uint8_t)1U))
    {
      LCD_SetCursorPos(122, 16, LCD_CURSOR_IN_PIXEL);
      LCD_PutChar(FONT_UP_ARROW_INDEX);
      
      LCD_SetCursorPos(122, 56, LCD_CURSOR_IN_PIXEL);
      LCD_PutChar(' ');
    }
    else
    {
      LCD_SetCursorPos(122, 16, LCD_CURSOR_IN_PIXEL);
      LCD_PutChar(FONT_UP_ARROW_INDEX);
      
      LCD_SetCursorPos(122, 56, LCD_CURSOR_IN_PIXEL);
      LCD_PutChar(FONT_DOWN_ARROW_INDEX);
    }
  }
}



#ifdef  __cplusplus
}
#endif
