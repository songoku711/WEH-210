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

const uint8_t MenuManager_Common_DoneStr[] =                          "DONE";

const uint8_t MenuManager_Common_EnableStr[] =                        "ENABLE";
const uint8_t MenuManager_Common_DisableStr[] =                       "DISABLE";

const uint8_t MenuManager_Common_NormalOnStr[] =                      "NORMAL ON";
const uint8_t MenuManager_Common_NormalOffStr[] =                     "NORMAL OFF";

const uint8_t MenuManager_Common_OnStr[] =                            "ON ";
const uint8_t MenuManager_Common_OffStr[] =                           "OFF";

const uint8_t MenuManager_Common_CelsiusDegStr[] =                    "*C";
const uint8_t MenuManager_Common_FahrenheitDegStr[] =                 "*F";

const uint8_t MenuManager_Common_SecondStr[] =                        "SECONDS";
const uint8_t MenuManager_Common_MinuteStr[] =                        "MINUTES";

const uint8_t MenuManager_Common_LevelStr[] =                         "LEVEL:";
const uint8_t MenuManager_Common_TimeStr[] =                          "TIMES:";

const uint8_t MenuManager_Common_ProgramTitleStr[] =                  "PROGRAM %d";
const uint8_t MenuManager_Common_ProgramStr[] =                       "Program %d";
const uint8_t MenuManager_Common_StepTitleStr[] =                     "STEP %d";
const uint8_t MenuManager_Common_StepStr[] =                          "Step %d";
const uint8_t MenuManager_Common_ProgramStepStr[] =                   "PROG %d STEP %d";



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
        Fsm_TriggerEvent(&MenuManager_FsmContext, (Fsm_EventType)((*(butEventMapConf->butEventMapCfg))[butEventMapIdx].butEvent));
      
        break;
      }
    }
  }
}



/*=============================================================================================*/
void MenuManager_Common_DecToBcdConv(uint32_t *inDec, uint32_t *outBcd, uint8_t unitNum)
{
  uint8_t tempUnitVal[10];
  uint8_t unitNumUsed = (uint8_t)0U;
  uint8_t unitNumIdx;
  uint32_t decVal = *inDec;

  while ((unitNumUsed < unitNum) && (decVal != (uint32_t)0U))
  {
    tempUnitVal[unitNumUsed++] = decVal % (uint32_t)10U;
    decVal = decVal / (uint32_t)10U;
  }

  for (unitNumIdx = (uint8_t)0U; unitNumIdx < (unitNum - unitNumUsed); unitNumIdx++)
  {
    *(outBcd + unitNumIdx) = (uint32_t)0U;
  }

  for (; unitNumUsed > (uint8_t)0U; unitNumUsed--)
  {
    *(outBcd + unitNum - unitNumUsed) = tempUnitVal[unitNumUsed - 1U];
  }
}

/*=============================================================================================*/
void MenuManager_Common_BcdToDecConv(uint32_t *outDec, uint32_t *inBcd, uint8_t unitNum)
{
  uint8_t unitNumIdx;
  uint32_t decVal;

  decVal = (uint32_t)0U;

  for (unitNumIdx = (uint8_t)0U; unitNumIdx < unitNum; unitNumIdx++)
  {
    decVal = decVal + *(inBcd + unitNumIdx);

    if (unitNumIdx < (unitNum - 1U))
    {
      decVal = decVal * (uint32_t)10U;
    }
  }

  *outDec = decVal;
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
  LCD_ClearPage(0, 2);
  LCD_ClearPage(7, 1);
}

/*=============================================================================================*/
void MenuManager_Common_LcdClearAdjustMenuDynamic(void)
{
  LCD_ClearPage(2, 5);
}

/*=============================================================================================*/
void MenuManager_Common_LcdUpdateAdjustMenuStatic(void)
{
  LCD_FillPage(0, 2, LCD_FILL_IMMEDIATE);
  LCD_FillPage(7, 1, LCD_FILL_IMMEDIATE);
}

/*=============================================================================================*/
void MenuManager_Common_LcdUpdateAdjustMenuDynamic(void)
{
  LCD_FillPage(2, 5, LCD_FILL_POLLING);
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
  
  LCD_SetFont(LCD_FONT_BIG);

  xPos = LCD_GetXPosCenter((uint8_t *)mainTitle);

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
void MenuManager_Common_LcdShowAdjustUnitVal
(
  uint32_t xPos,
  uint32_t yPos,
  uint32_t *unitVal,
  uint32_t length
)
{
  uint32_t index;

  LCD_SetFont(LCD_FONT_SMALL);

  for (index = (uint32_t)0U; index < length; index++)
  {
    LCD_SetCursorPos((uint8_t)(xPos + index), (uint8_t)(yPos), LCD_CURSOR_BY_FONT);
    LCD_PutChar((uint8_t)(*(unitVal + index) + FONT_NUMBER_TO_CHAR_OFFSET));
  }
}

/*=============================================================================================*/
void MenuManager_Common_LcdShowAdjustArrow
(
  uint32_t xPos,
  uint32_t yPos,
  uint32_t curPos
)
{
  LCD_SetFont(LCD_FONT_SMALL);
  
  LCD_SetCursorPos((uint8_t)(xPos + curPos), (uint8_t)(yPos - 1U), LCD_CURSOR_BY_FONT);
  LCD_PutChar(FONT_UP_ARROW_INDEX);
  
  LCD_SetCursorPos((uint8_t)(xPos + curPos), (uint8_t)(yPos + 1U), LCD_CURSOR_BY_FONT);
  LCD_PutChar(FONT_DOWN_ARROW_INDEX);
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

/*=============================================================================================*/
void MenuManager_Common_CheckDrainStepTimeConstraint(uint8_t seqIdx, uint8_t stepIdx, uint8_t drainStepIdx, uint16_t* data)
{
  uint16_t tempTime;

  /* Set t2 == 0 -> (Set t3 = 0) and (set t4 = 0) */
  /* Set t2 != 0 -> (Set t3 = auto if t3 prev = 0) and (set t4 = auto if t4 prev = 0) */
  /* Set t3 == 0 -> (Set t4 = 0) */
  /* Set t3 != 0 -> (Set t3 = 0 if t2 = 0) else (set t4 = auto if t4 prev = 0) */
  /* Set t4 != 0 -> (Set t4 = 0 if t2 = 0) */
  if (seqIdx >= PROGRAMMANAGER_SEQUENCE_NUM_MAX)
  {
    /* Change relative drain step following above constraint */
    if ((drainStepIdx == PROGRAMMANAGER_STEP_DRAINSTEP_EXTR_LVL2_DRAIN_IDX) && \
        (*data == (uint16_t)0U))
    {
      if (ProgramManager_gParamConfig.drainCfg.drainTime[PROGRAMMANAGER_STEP_DRAINSTEP_EXTR_LVL3_DRAIN_IDX] != (uint16_t)0U)
      {
        tempTime = (uint16_t)0U;
        ProgramManager_DrainSetup_DrainTime_SetData(PROGRAMMANAGER_STEP_DRAINSTEP_EXTR_LVL3_DRAIN_IDX, &tempTime);
        ProgramManager_DrainSetup_DrainTime_GetData(PROGRAMMANAGER_STEP_DRAINSTEP_EXTR_LVL3_DRAIN_IDX, &(ProgramManager_gParamConfig.drainCfg.drainTime)[PROGRAMMANAGER_STEP_DRAINSTEP_EXTR_LVL3_DRAIN_IDX]);
      }

      if (ProgramManager_gParamConfig.drainCfg.drainTime[PROGRAMMANAGER_STEP_DRAINSTEP_EXTR_LVL4_DRAIN_IDX] != (uint16_t)0U)
      {
        tempTime = (uint16_t)0U;
        ProgramManager_DrainSetup_DrainTime_SetData(PROGRAMMANAGER_STEP_DRAINSTEP_EXTR_LVL4_DRAIN_IDX, &tempTime);
        ProgramManager_DrainSetup_DrainTime_GetData(PROGRAMMANAGER_STEP_DRAINSTEP_EXTR_LVL4_DRAIN_IDX, &(ProgramManager_gParamConfig.drainCfg.drainTime)[PROGRAMMANAGER_STEP_DRAINSTEP_EXTR_LVL4_DRAIN_IDX]);
      }
    }
    else if ((drainStepIdx == PROGRAMMANAGER_STEP_DRAINSTEP_EXTR_LVL2_DRAIN_IDX) && \
             (*data != (uint16_t)0U))
    {
      if (ProgramManager_gParamConfig.drainCfg.drainTime[PROGRAMMANAGER_STEP_DRAINSTEP_EXTR_LVL3_DRAIN_IDX] == (uint16_t)0U)
      {
        tempTime = (uint16_t)PROGRAMMANAGER_DRAINSETUP_DRAINTIME_EXTR_LVL3_DRAIN_AUTO;
        ProgramManager_DrainSetup_DrainTime_SetData(PROGRAMMANAGER_STEP_DRAINSTEP_EXTR_LVL3_DRAIN_IDX, &tempTime);
        ProgramManager_DrainSetup_DrainTime_GetData(PROGRAMMANAGER_STEP_DRAINSTEP_EXTR_LVL3_DRAIN_IDX, &(ProgramManager_gParamConfig.drainCfg.drainTime)[PROGRAMMANAGER_STEP_DRAINSTEP_EXTR_LVL3_DRAIN_IDX]);
      }

      if (ProgramManager_gParamConfig.drainCfg.drainTime[PROGRAMMANAGER_STEP_DRAINSTEP_EXTR_LVL4_DRAIN_IDX] == (uint16_t)0U)
      {
        tempTime = (uint16_t)PROGRAMMANAGER_DRAINSETUP_DRAINTIME_EXTR_LVL4_DRAIN_AUTO;
        ProgramManager_DrainSetup_DrainTime_SetData(PROGRAMMANAGER_STEP_DRAINSTEP_EXTR_LVL4_DRAIN_IDX, &tempTime);
        ProgramManager_DrainSetup_DrainTime_GetData(PROGRAMMANAGER_STEP_DRAINSTEP_EXTR_LVL4_DRAIN_IDX, &(ProgramManager_gParamConfig.drainCfg.drainTime)[PROGRAMMANAGER_STEP_DRAINSTEP_EXTR_LVL4_DRAIN_IDX]);
      }
    }
    else if ((drainStepIdx == PROGRAMMANAGER_STEP_DRAINSTEP_EXTR_LVL3_DRAIN_IDX) && \
             (*data == (uint16_t)0U))
    {
      if (ProgramManager_gParamConfig.drainCfg.drainTime[PROGRAMMANAGER_STEP_DRAINSTEP_EXTR_LVL4_DRAIN_IDX] != (uint16_t)0U)
      {
        tempTime = (uint16_t)0U;
        ProgramManager_DrainSetup_DrainTime_SetData(PROGRAMMANAGER_STEP_DRAINSTEP_EXTR_LVL4_DRAIN_IDX, &tempTime);
        ProgramManager_DrainSetup_DrainTime_GetData(PROGRAMMANAGER_STEP_DRAINSTEP_EXTR_LVL4_DRAIN_IDX, &(ProgramManager_gParamConfig.drainCfg.drainTime)[PROGRAMMANAGER_STEP_DRAINSTEP_EXTR_LVL4_DRAIN_IDX]);
      }
    }
    else if ((drainStepIdx == PROGRAMMANAGER_STEP_DRAINSTEP_EXTR_LVL3_DRAIN_IDX) && \
             (*data != (uint16_t)0U))
    {
      if (ProgramManager_gParamConfig.drainCfg.drainTime[PROGRAMMANAGER_STEP_DRAINSTEP_EXTR_LVL2_DRAIN_IDX] == (uint16_t)0U)
      {
        *data = (uint16_t)0U;
      }
      else if (ProgramManager_gParamConfig.drainCfg.drainTime[PROGRAMMANAGER_STEP_DRAINSTEP_EXTR_LVL4_DRAIN_IDX] == (uint16_t)0U)
      {
        tempTime = (uint16_t)PROGRAMMANAGER_DRAINSETUP_DRAINTIME_EXTR_LVL4_DRAIN_AUTO;
        ProgramManager_DrainSetup_DrainTime_SetData(PROGRAMMANAGER_STEP_DRAINSTEP_EXTR_LVL4_DRAIN_IDX, &tempTime);
        ProgramManager_DrainSetup_DrainTime_GetData(PROGRAMMANAGER_STEP_DRAINSTEP_EXTR_LVL4_DRAIN_IDX, &(ProgramManager_gParamConfig.drainCfg.drainTime)[PROGRAMMANAGER_STEP_DRAINSTEP_EXTR_LVL4_DRAIN_IDX]);
      }
    }
    else if ((drainStepIdx == PROGRAMMANAGER_STEP_DRAINSTEP_EXTR_LVL4_DRAIN_IDX) && \
             (*data != (uint16_t)0U))
    {
      if ((ProgramManager_gParamConfig.drainCfg.drainTime[PROGRAMMANAGER_STEP_DRAINSTEP_EXTR_LVL2_DRAIN_IDX] == (uint16_t)0U) || \
          (ProgramManager_gParamConfig.drainCfg.drainTime[PROGRAMMANAGER_STEP_DRAINSTEP_EXTR_LVL3_DRAIN_IDX] == (uint16_t)0U))
      {
        *data = (uint16_t)0U;
      }
    }
    else
    {
      /* No change */
    }

    /* Update new drain step time */
    ProgramManager_DrainSetup_DrainTime_SetData(drainStepIdx, data);
    ProgramManager_DrainSetup_DrainTime_GetData(drainStepIdx, &(ProgramManager_gParamConfig.drainCfg.drainTime)[drainStepIdx]);

    /* Update drain off time */
    if (ProgramManager_gParamConfig.drainCfg.drainTime[PROGRAMMANAGER_STEP_DRAINSTEP_EXTR_LVL2_DRAIN_IDX] == (uint16_t)0U)
    {
      tempTime = (uint16_t)PROGRAMMANAGER_DRAINSETUP_DRAINOFFTIME_EXTR_LVL2_DRAIN_ZERO_AUTO;
      ProgramManager_DrainSetup_DrainOffTime_SetData(&tempTime);
      ProgramManager_DrainSetup_DrainOffTime_GetData(&(ProgramManager_gParamConfig.drainCfg.drainOffTime));
    }
    else if ((ProgramManager_gParamConfig.drainCfg.drainTime[PROGRAMMANAGER_STEP_DRAINSTEP_EXTR_LVL3_DRAIN_IDX] != (uint16_t)0U) && \
             (ProgramManager_gParamConfig.drainCfg.drainTime[PROGRAMMANAGER_STEP_DRAINSTEP_EXTR_LVL4_DRAIN_IDX] == (uint16_t)0U))
    {
      tempTime = (uint16_t)PROGRAMMANAGER_DRAINSETUP_DRAINOFFTIME_EXTR_LVL3_DRAIN_NZERO_AUTO;
      ProgramManager_DrainSetup_DrainOffTime_SetData(&tempTime);
      ProgramManager_DrainSetup_DrainOffTime_GetData(&(ProgramManager_gParamConfig.drainCfg.drainOffTime));
    }
    else if (ProgramManager_gParamConfig.drainCfg.drainTime[PROGRAMMANAGER_STEP_DRAINSTEP_EXTR_LVL4_DRAIN_IDX] != (uint16_t)0U)
    {
      tempTime = (uint16_t)PROGRAMMANAGER_DRAINSETUP_DRAINOFFTIME_EXTR_LVL4_DRAIN_NZERO_AUTO;
      ProgramManager_DrainSetup_DrainOffTime_SetData(&tempTime);
      ProgramManager_DrainSetup_DrainOffTime_GetData(&(ProgramManager_gParamConfig.drainCfg.drainOffTime));
    }
    else
    {
      /* No change */
    }
  }
  else
  {
    /* Change relative drain step following above constraint */
    if ((drainStepIdx == PROGRAMMANAGER_STEP_DRAINSTEP_EXTR_LVL2_DRAIN_IDX) && \
        (*data == (uint16_t)0U))
    {
      ProgramManager_NormStepConfig_DrainTime_GetData(seqIdx, stepIdx, PROGRAMMANAGER_STEP_DRAINSTEP_EXTR_LVL3_DRAIN_IDX, &tempTime);

      if (tempTime != (uint16_t)0U)
      {
        tempTime = (uint16_t)0U;
        ProgramManager_NormStepConfig_DrainTime_SetData(seqIdx, stepIdx, PROGRAMMANAGER_STEP_DRAINSTEP_EXTR_LVL3_DRAIN_IDX, &tempTime);

        if (ProgramManager_gAutoSeqConfig.sequenceIndex == seqIdx)
        {
          ProgramManager_NormStepConfig_DrainTime_GetData(seqIdx, stepIdx, PROGRAMMANAGER_STEP_DRAINSTEP_EXTR_LVL3_DRAIN_IDX, &(ProgramManager_gAutoSeqConfig.normStep[stepIdx].drainTime)[PROGRAMMANAGER_STEP_DRAINSTEP_EXTR_LVL3_DRAIN_IDX]);
        }
      }

      ProgramManager_NormStepConfig_DrainTime_GetData(seqIdx, stepIdx, PROGRAMMANAGER_STEP_DRAINSTEP_EXTR_LVL4_DRAIN_IDX, &tempTime);

      if (tempTime != (uint16_t)0U)
      {
        tempTime = (uint16_t)0U;
        ProgramManager_NormStepConfig_DrainTime_SetData(seqIdx, stepIdx, PROGRAMMANAGER_STEP_DRAINSTEP_EXTR_LVL4_DRAIN_IDX, &tempTime);

        if (ProgramManager_gAutoSeqConfig.sequenceIndex == seqIdx)
        {
          ProgramManager_NormStepConfig_DrainTime_GetData(seqIdx, stepIdx, PROGRAMMANAGER_STEP_DRAINSTEP_EXTR_LVL4_DRAIN_IDX, &(ProgramManager_gAutoSeqConfig.normStep[stepIdx].drainTime)[PROGRAMMANAGER_STEP_DRAINSTEP_EXTR_LVL4_DRAIN_IDX]);
        }
      }
    }
    else if ((drainStepIdx == PROGRAMMANAGER_STEP_DRAINSTEP_EXTR_LVL2_DRAIN_IDX) && \
             (*data != (uint16_t)0U))
    {
      ProgramManager_NormStepConfig_DrainTime_GetData(seqIdx, stepIdx, PROGRAMMANAGER_STEP_DRAINSTEP_EXTR_LVL3_DRAIN_IDX, &tempTime);

      if (tempTime == (uint16_t)0U)
      {
        tempTime = (uint16_t)PROGRAMMANAGER_DRAINSETUP_DRAINTIME_EXTR_LVL3_DRAIN_AUTO;
        ProgramManager_NormStepConfig_DrainTime_SetData(seqIdx, stepIdx, PROGRAMMANAGER_STEP_DRAINSTEP_EXTR_LVL3_DRAIN_IDX, &tempTime);

        if (ProgramManager_gAutoSeqConfig.sequenceIndex == seqIdx)
        {
          ProgramManager_NormStepConfig_DrainTime_GetData(seqIdx, stepIdx, PROGRAMMANAGER_STEP_DRAINSTEP_EXTR_LVL3_DRAIN_IDX, &(ProgramManager_gAutoSeqConfig.normStep[stepIdx].drainTime)[PROGRAMMANAGER_STEP_DRAINSTEP_EXTR_LVL3_DRAIN_IDX]);
        }
      }

      ProgramManager_NormStepConfig_DrainTime_GetData(seqIdx, stepIdx, PROGRAMMANAGER_STEP_DRAINSTEP_EXTR_LVL4_DRAIN_IDX, &tempTime);

      if (tempTime == (uint16_t)0U)
      {
        tempTime = (uint16_t)PROGRAMMANAGER_DRAINSETUP_DRAINTIME_EXTR_LVL4_DRAIN_AUTO;
        ProgramManager_NormStepConfig_DrainTime_SetData(seqIdx, stepIdx, PROGRAMMANAGER_STEP_DRAINSTEP_EXTR_LVL4_DRAIN_IDX, &tempTime);

        if (ProgramManager_gAutoSeqConfig.sequenceIndex == seqIdx)
        {
          ProgramManager_NormStepConfig_DrainTime_GetData(seqIdx, stepIdx, PROGRAMMANAGER_STEP_DRAINSTEP_EXTR_LVL4_DRAIN_IDX, &(ProgramManager_gAutoSeqConfig.normStep[stepIdx].drainTime)[PROGRAMMANAGER_STEP_DRAINSTEP_EXTR_LVL4_DRAIN_IDX]);
        }
      }
    }
    else if ((drainStepIdx == PROGRAMMANAGER_STEP_DRAINSTEP_EXTR_LVL3_DRAIN_IDX) && \
             (*data == (uint16_t)0U))
    {
      ProgramManager_NormStepConfig_DrainTime_GetData(seqIdx, stepIdx, PROGRAMMANAGER_STEP_DRAINSTEP_EXTR_LVL4_DRAIN_IDX, &tempTime);

      if (tempTime != (uint16_t)0U)
      {
        tempTime = (uint16_t)0U;
        ProgramManager_NormStepConfig_DrainTime_SetData(seqIdx, stepIdx, PROGRAMMANAGER_STEP_DRAINSTEP_EXTR_LVL4_DRAIN_IDX, &tempTime);

        if (ProgramManager_gAutoSeqConfig.sequenceIndex == seqIdx)
        {
          ProgramManager_NormStepConfig_DrainTime_GetData(seqIdx, stepIdx, PROGRAMMANAGER_STEP_DRAINSTEP_EXTR_LVL4_DRAIN_IDX, &(ProgramManager_gAutoSeqConfig.normStep[stepIdx].drainTime)[PROGRAMMANAGER_STEP_DRAINSTEP_EXTR_LVL4_DRAIN_IDX]);
        }
      }
    }
    else if ((drainStepIdx == PROGRAMMANAGER_STEP_DRAINSTEP_EXTR_LVL3_DRAIN_IDX) && \
             (*data != (uint16_t)0U))
    {
      ProgramManager_NormStepConfig_DrainTime_GetData(seqIdx, stepIdx, PROGRAMMANAGER_STEP_DRAINSTEP_EXTR_LVL2_DRAIN_IDX, &tempTime);

      if (tempTime == (uint16_t)0U)
      {
        *data = (uint16_t)0U;
      }
      else
      {
        ProgramManager_NormStepConfig_DrainTime_GetData(seqIdx, stepIdx, PROGRAMMANAGER_STEP_DRAINSTEP_EXTR_LVL4_DRAIN_IDX, &tempTime);

        if (tempTime == (uint16_t)0U)
        {
          tempTime = (uint16_t)PROGRAMMANAGER_DRAINSETUP_DRAINTIME_EXTR_LVL4_DRAIN_AUTO;
          ProgramManager_NormStepConfig_DrainTime_SetData(seqIdx, stepIdx, PROGRAMMANAGER_STEP_DRAINSTEP_EXTR_LVL4_DRAIN_IDX, &tempTime);

          if (ProgramManager_gAutoSeqConfig.sequenceIndex == seqIdx)
          {
            ProgramManager_NormStepConfig_DrainTime_GetData(seqIdx, stepIdx, PROGRAMMANAGER_STEP_DRAINSTEP_EXTR_LVL4_DRAIN_IDX, &(ProgramManager_gAutoSeqConfig.normStep[stepIdx].drainTime)[PROGRAMMANAGER_STEP_DRAINSTEP_EXTR_LVL4_DRAIN_IDX]);
          }
        }
      }
    }
    else if ((drainStepIdx == PROGRAMMANAGER_STEP_DRAINSTEP_EXTR_LVL4_DRAIN_IDX) && \
             (*data != (uint16_t)0U))
    {
      ProgramManager_NormStepConfig_DrainTime_GetData(seqIdx, stepIdx, PROGRAMMANAGER_STEP_DRAINSTEP_EXTR_LVL2_DRAIN_IDX, &tempTime);

      if (tempTime == (uint16_t)0U)
      {
        *data = (uint16_t)0U;
      }
      else
      {
        ProgramManager_NormStepConfig_DrainTime_GetData(seqIdx, stepIdx, PROGRAMMANAGER_STEP_DRAINSTEP_EXTR_LVL3_DRAIN_IDX, &tempTime);

        if (tempTime == (uint16_t)0U)
        {
          *data = (uint16_t)0U;
        }
      }
    }
    else
    {
      /* No change */
    }

    /* Update new drain step time */
    ProgramManager_NormStepConfig_DrainTime_SetData(seqIdx, stepIdx, drainStepIdx, data);

    if (ProgramManager_gAutoSeqConfig.sequenceIndex == seqIdx)
    {
      ProgramManager_NormStepConfig_DrainTime_GetData(seqIdx, stepIdx, drainStepIdx, &(ProgramManager_gAutoSeqConfig.normStep[stepIdx].drainTime)[drainStepIdx]);
    }

    /* Update drain off time */
    ProgramManager_NormStepConfig_DrainTime_GetData(seqIdx, stepIdx, PROGRAMMANAGER_STEP_DRAINSTEP_EXTR_LVL2_DRAIN_IDX, &tempTime);

    if (tempTime == (uint16_t)0U)
    {
      tempTime = (uint16_t)PROGRAMMANAGER_DRAINSETUP_DRAINOFFTIME_EXTR_LVL2_DRAIN_ZERO_AUTO;
      ProgramManager_NormStepConfig_DrainOffTime_SetData(seqIdx, stepIdx, &tempTime);

      if (ProgramManager_gAutoSeqConfig.sequenceIndex == seqIdx)
      {
        ProgramManager_NormStepConfig_DrainOffTime_GetData(seqIdx, stepIdx, &(ProgramManager_gAutoSeqConfig.normStep[stepIdx].drainOffTime));
      }
    }
    else
    {
      ProgramManager_NormStepConfig_DrainTime_GetData(seqIdx, stepIdx, PROGRAMMANAGER_STEP_DRAINSTEP_EXTR_LVL3_DRAIN_IDX, &tempTime);

      if (tempTime != (uint16_t)0U)
      {
        ProgramManager_NormStepConfig_DrainTime_GetData(seqIdx, stepIdx, PROGRAMMANAGER_STEP_DRAINSTEP_EXTR_LVL4_DRAIN_IDX, &tempTime);

        if (tempTime == (uint16_t)0U)
        {
          tempTime = (uint16_t)PROGRAMMANAGER_DRAINSETUP_DRAINOFFTIME_EXTR_LVL3_DRAIN_NZERO_AUTO;
          ProgramManager_NormStepConfig_DrainOffTime_SetData(seqIdx, stepIdx, &tempTime);

          if (ProgramManager_gAutoSeqConfig.sequenceIndex == seqIdx)
          {
            ProgramManager_NormStepConfig_DrainOffTime_GetData(seqIdx, stepIdx, &(ProgramManager_gAutoSeqConfig.normStep[stepIdx].drainOffTime));
          }
        }
        else
        {
          tempTime = (uint16_t)PROGRAMMANAGER_DRAINSETUP_DRAINOFFTIME_EXTR_LVL4_DRAIN_NZERO_AUTO;
          ProgramManager_NormStepConfig_DrainOffTime_SetData(seqIdx, stepIdx, &tempTime);

          if (ProgramManager_gAutoSeqConfig.sequenceIndex == seqIdx)
          {
            ProgramManager_NormStepConfig_DrainOffTime_GetData(seqIdx, stepIdx, &(ProgramManager_gAutoSeqConfig.normStep[stepIdx].drainOffTime));
          }
        }
      }
    }
  }
}



#ifdef  __cplusplus
}
#endif
