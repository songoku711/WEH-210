/* 
 * File:   _1311610000_menu_StepTempModeCustom.c
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

/** Menu manager internal data definitions */
#define MENUMANAGER_STEPTEMPMODECUSTOM_INTERNALDATALENGTH             (uint8_t)11U

#define MENUMANAGER_STEPTEMPMODECUSTOM_INTERNALSTATE_IDX              0U
#define MENUMANAGER_STEPTEMPMODECUSTOM_COUNTER_IDX                    1U
#define MENUMANAGER_STEPTEMPMODECUSTOM_CURPOS_IDX                     2U
#define MENUMANAGER_STEPTEMPMODECUSTOM_SEQIDX_IDX                     3U
#define MENUMANAGER_STEPTEMPMODECUSTOM_STEPIDX_IDX                    4U
#define MENUMANAGER_STEPTEMPMODECUSTOM_VALUE_MIN_IDX                  5U
#define MENUMANAGER_STEPTEMPMODECUSTOM_VALUE_MAX_IDX                  6U
#define MENUMANAGER_STEPTEMPMODECUSTOM_VALUE_IDX                      7U
#define MENUMANAGER_STEPTEMPMODECUSTOM_UNITVAL_IDX                    8U

#define MenuManager_StepTempModeCustom_InternalState                  MenuManager_GetInternalDataPtr(MENUMANAGER_STEPTEMPMODECUSTOM_INTERNALSTATE_IDX)
#define MenuManager_StepTempModeCustom_Counter                        MenuManager_GetInternalDataPtr(MENUMANAGER_STEPTEMPMODECUSTOM_COUNTER_IDX)
#define MenuManager_StepTempModeCustom_CurPos                         MenuManager_GetInternalDataPtr(MENUMANAGER_STEPTEMPMODECUSTOM_CURPOS_IDX)
#define MenuManager_StepTempModeCustom_SeqIdx                         MenuManager_GetInternalDataPtr(MENUMANAGER_STEPTEMPMODECUSTOM_SEQIDX_IDX)
#define MenuManager_StepTempModeCustom_StepIdx                        MenuManager_GetInternalDataPtr(MENUMANAGER_STEPTEMPMODECUSTOM_STEPIDX_IDX)
#define MenuManager_StepTempModeCustom_ValueMin                       MenuManager_GetInternalDataPtr(MENUMANAGER_STEPTEMPMODECUSTOM_VALUE_MIN_IDX)
#define MenuManager_StepTempModeCustom_ValueMax                       MenuManager_GetInternalDataPtr(MENUMANAGER_STEPTEMPMODECUSTOM_VALUE_MAX_IDX)
#define MenuManager_StepTempModeCustom_Value                          MenuManager_GetInternalDataPtr(MENUMANAGER_STEPTEMPMODECUSTOM_VALUE_IDX)
#define MenuManager_StepTempModeCustom_UnitVal(idx)                   MenuManager_GetInternalDataPtr(MENUMANAGER_STEPTEMPMODECUSTOM_UNITVAL_IDX + idx)



#define MENUMANAGER_STEPTEMPMODECUSTOM_INTERNALSTATE_READY            (uint32_t)0U
#define MENUMANAGER_STEPTEMPMODECUSTOM_INTERNALSTATE_RUNNING          (uint32_t)1U
#define MENUMANAGER_STEPTEMPMODECUSTOM_INTERNALSTATE_DONE             (uint32_t)2U
#define MENUMANAGER_STEPTEMPMODECUSTOM_INTERNALSTATE_EXITING          (uint32_t)3U

#define MENUMANAGER_STEPTEMPMODECUSTOM_COUNTER_MAX                    (uint32_t)50U

#define MENUMANAGER_STEPTEMPMODECUSTOM_CURPOS_XPOS                    (uint32_t)8U
#define MENUMANAGER_STEPTEMPMODECUSTOM_CURPOS_YPOS                    (uint32_t)5U

#define MENUMANAGER_STEPTEMPMODECUSTOM_UNIT_XPOS                      (uint32_t)12U
#define MENUMANAGER_STEPTEMPMODECUSTOM_UNIT_YPOS                      (uint32_t)5U

#define MENUMANAGER_STEPTEMPMODECUSTOM_UNITVAL_LENGTH                 (uint32_t)3U



/** Menu manager main titles and child menu titles */
static const uint8_t MenuManager_StepTempModeCustom_MainTitleStr[] =  "USER TEMP THRES";



/** Menu manager button event mapping array */
static MenuManager_ButEventMapStruct MenuManager_StepTempModeCustom_ButEventMap[6] =
{
  { IOMANAGER_BUTTON_STATE_START,                                     MENUMANAGER_EVENT_START_BUT             },
  { IOMANAGER_BUTTON_STATE_STOP,                                      MENUMANAGER_EVENT_STOP_BUT              },
  { IOMANAGER_BUTTON_STATE_UP,                                        MENUMANAGER_EVENT_UP_BUT                },
  { IOMANAGER_BUTTON_STATE_DOWN,                                      MENUMANAGER_EVENT_DOWN_BUT              },
  { IOMANAGER_BUTTON_STATE_ADD,                                       MENUMANAGER_EVENT_ADD_BUT               },
  { IOMANAGER_BUTTON_STATE_SUB,                                       MENUMANAGER_EVENT_SUB_BUT               }
};

/** Menu manager button event mapping configuration */
static MenuManager_ButEventMapConfStruct MenuManager_StepTempModeCustom_ButEventMapConf =
{
  .butEventMapNum             = (uint8_t)6U,
  .butEventMapCfg             = &MenuManager_StepTempModeCustom_ButEventMap
};



/** Menu manager event handlers */
static Fsm_GuardType MenuManager_StepTempModeCustom_Entry             (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_StepTempModeCustom_Exit              (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_StepTempModeCustom_StartBut          (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_StepTempModeCustom_StopBut           (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_StepTempModeCustom_UpBut             (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_StepTempModeCustom_DownBut           (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_StepTempModeCustom_AddBut            (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_StepTempModeCustom_SubBut            (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);

/** Menu manager state machine */
Fsm_EventEntryStruct MenuManager_StepTempModeCustom_StateMachine[9] =
{
  FSM_TRIGGER_ENTRY             (                                     MenuManager_StepTempModeCustom_Entry                                            ),
  FSM_TRIGGER_EXIT              (                                     MenuManager_StepTempModeCustom_Exit                                             ),
  FSM_TRIGGER_TRANSITION        ( MENUMANAGER_EVENT_PREV,                                                     MENUMANAGER_STATE_STEP_NORM_SETUP       ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_START_BUT,        MenuManager_StepTempModeCustom_StartBut                                         ),
  FSM_TRIGGER_TRANSITION_ACTION ( MENUMANAGER_EVENT_STOP_BUT,         MenuManager_StepTempModeCustom_StopBut, MENUMANAGER_STATE_STEP_NORM_SETUP       ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_UP_BUT,           MenuManager_StepTempModeCustom_UpBut                                            ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_DOWN_BUT,         MenuManager_StepTempModeCustom_DownBut                                          ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_ADD_BUT,          MenuManager_StepTempModeCustom_AddBut                                           ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_SUB_BUT,          MenuManager_StepTempModeCustom_SubBut                                           )
};



/*===============================================================================================
*                                   LOCAL FUNCTION PROTOTYPES
===============================================================================================*/

static void MenuManager_StepTempModeCustom_LcdShowMainTitle(void);
static void MenuManager_StepTempModeCustom_LcdShowAdjust(void);
static void MenuManager_StepTempModeCustom_LcdShowDone(void);

static void MenuManager_StepTempModeCustom_SubMainFunction(void);
static void MenuManager_StepTempModeCustom_SubTickHandler(void);



/*===============================================================================================
*                                       LOCAL FUNCTIONS
===============================================================================================*/

static void MenuManager_StepTempModeCustom_LcdShowMainTitle(void)
{
  /* Print main title */
  MenuManager_Common_LcdClearAdjustMenuStatic();
  MenuManager_Common_LcdShowMainTitle(MenuManager_StepTempModeCustom_MainTitleStr);
  MenuManager_Common_LcdShowOkExitChoice();
  MenuManager_Common_LcdUpdateAdjustMenuStatic();
}

/*=============================================================================================*/
static void MenuManager_StepTempModeCustom_LcdShowAdjust(void)
{
  MenuManager_Common_LcdClearAdjustMenuDynamic();

  MenuManager_Common_LcdShowAdjustUnitVal
  (
    MENUMANAGER_STEPTEMPMODECUSTOM_CURPOS_XPOS,
    MENUMANAGER_STEPTEMPMODECUSTOM_CURPOS_YPOS,
    &(MenuManager_StepTempModeCustom_UnitVal(0)),
    MENUMANAGER_STEPTEMPMODECUSTOM_UNITVAL_LENGTH
  );
  
  MenuManager_Common_LcdShowAdjustArrow
  (
    MENUMANAGER_STEPTEMPMODECUSTOM_CURPOS_XPOS,
    MENUMANAGER_STEPTEMPMODECUSTOM_CURPOS_YPOS,
    MenuManager_StepTempModeCustom_CurPos
  );

  LCD_SetCursorPos(MENUMANAGER_STEPTEMPMODECUSTOM_UNIT_XPOS, MENUMANAGER_STEPTEMPMODECUSTOM_UNIT_YPOS, LCD_CURSOR_BY_FONT);
  if (ProgramManager_gParamConfig.machineFuncCfg.tempUnit == PROGRAMMANAGER_TEMP_UNIT_CELSIUS)
  {
    LCD_PutString((uint8_t *)MenuManager_Common_CelsiusDegStr);
  }
  else
  {
    LCD_PutString((uint8_t *)MenuManager_Common_FahrenheitDegStr);
  }

  MenuManager_Common_LcdUpdateAdjustMenuDynamic();
}

/*=============================================================================================*/
static void MenuManager_StepTempModeCustom_LcdShowDone(void)
{
  MenuManager_Common_LcdClearAll();

  MenuManager_Common_LcdShowAdjustUnitVal
  (
    MENUMANAGER_STEPTEMPMODECUSTOM_CURPOS_XPOS,
    MENUMANAGER_STEPTEMPMODECUSTOM_CURPOS_YPOS,
    &(MenuManager_StepTempModeCustom_UnitVal(0)),
    MENUMANAGER_STEPTEMPMODECUSTOM_UNITVAL_LENGTH
  );

  LCD_SetCursorPos(MENUMANAGER_STEPTEMPMODECUSTOM_UNIT_XPOS, MENUMANAGER_STEPTEMPMODECUSTOM_UNIT_YPOS, LCD_CURSOR_BY_FONT);
  if (ProgramManager_gParamConfig.machineFuncCfg.tempUnit == PROGRAMMANAGER_TEMP_UNIT_CELSIUS)
  {
    LCD_PutString((uint8_t *)MenuManager_Common_CelsiusDegStr);
  }
  else
  {
    LCD_PutString((uint8_t *)MenuManager_Common_FahrenheitDegStr);
  }

  MenuManager_Common_LcdUpdateAll();
}



/*=============================================================================================*/
static Fsm_GuardType MenuManager_StepTempModeCustom_Entry(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  MenuManager_Common_ProgramSetupStruct* enterDataHierachy;
  uint8_t tempTempThreshold;
  HAL_StatusTypeDef retVal = HAL_OK;

  /* Check if previous state data hierachy is not empty */
  if (pFsmContext->dataHierachy != NULL)
  {
    if (pFsmContext->dataHierachy->dataId == MENUMANAGER_STATE_STEP_TEMP_MODE)
    {
      enterDataHierachy = (MenuManager_Common_ProgramSetupStruct *)(pFsmContext->dataHierachy);

      MenuManager_InternalDataPush(MENUMANAGER_STEPTEMPMODECUSTOM_INTERNALDATALENGTH);

      MenuManager_StepTempModeCustom_InternalState = MENUMANAGER_STEPTEMPMODECUSTOM_INTERNALSTATE_READY;
      MenuManager_StepTempModeCustom_Counter = (uint32_t)0U;
      MenuManager_StepTempModeCustom_CurPos = (uint32_t)0U;

      MenuManager_StepTempModeCustom_SeqIdx = enterDataHierachy->seqIdx;
      MenuManager_StepTempModeCustom_StepIdx = enterDataHierachy->stepIdx;

      ProgramManager_NormStepConfig_TempThreshold_GetData
      (
        (uint8_t)MenuManager_StepTempModeCustom_SeqIdx,
        (uint8_t)MenuManager_StepTempModeCustom_StepIdx,
        &tempTempThreshold,
        ProgramManager_gParamConfig.machineFuncCfg.tempUnit
      );

      if (ProgramManager_gParamConfig.machineFuncCfg.tempUnit == PROGRAMMANAGER_TEMP_UNIT_CELSIUS)
      {
        MenuManager_StepTempModeCustom_ValueMin = (uint32_t)PROGRAMMANAGER_HEATTEMPSETUP_WATER_TEMP_C_MIN;
        MenuManager_StepTempModeCustom_ValueMax = (uint32_t)PROGRAMMANAGER_HEATTEMPSETUP_WATER_TEMP_C_MAX;
      }
      else
      {
        MenuManager_StepTempModeCustom_ValueMin = (uint32_t)PROGRAMMANAGER_HEATTEMPSETUP_WATER_TEMP_F_MIN;
        MenuManager_StepTempModeCustom_ValueMax = (uint32_t)PROGRAMMANAGER_HEATTEMPSETUP_WATER_TEMP_F_MAX;
      }

      MenuManager_StepTempModeCustom_Value = (uint32_t)tempTempThreshold;

      MenuManager_Common_DecToBcdConv
      (
        &(MenuManager_StepTempModeCustom_Value),
        &(MenuManager_StepTempModeCustom_UnitVal(0)),
        (uint8_t)MENUMANAGER_STEPTEMPMODECUSTOM_UNITVAL_LENGTH
      );
      
      /* Release previous state data hierachy */
      MenuManager_free(pFsmContext->dataHierachy);
      pFsmContext->dataHierachy = NULL;
    }
    else
    {
      retVal = HAL_ERROR;
    }
  }
  else
  {
    retVal = HAL_ERROR;
  }

  if (retVal == HAL_OK)
  {
    MenuManager_StepTempModeCustom_LcdShowMainTitle();
    MenuManager_StepTempModeCustom_LcdShowAdjust();

    MenuManager_SubMainFunction = MenuManager_StepTempModeCustom_SubMainFunction;
    MenuManager_SubTickHandler = MenuManager_StepTempModeCustom_SubTickHandler;

    return FSM_GUARD_TRUE;
  }

  return FSM_GUARD_FALSE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_StepTempModeCustom_Exit(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  Fsm_DataHierachyStruct* dataHierachy;

  MenuManager_SubMainFunction = NULL;
  MenuManager_SubTickHandler = NULL;

  dataHierachy = (Fsm_DataHierachyStruct *)MenuManager_malloc(sizeof(Fsm_DataHierachyStruct));
  dataHierachy->dataId = MENUMANAGER_STATE_STEP_TEMP_MODE_CUSTOM;

  pFsmContext->dataHierachy = dataHierachy;

  /* Free internal data */
  MenuManager_InternalDataPop();
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_StepTempModeCustom_StartBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_StepTempModeCustom_InternalState == MENUMANAGER_STEPTEMPMODECUSTOM_INTERNALSTATE_READY)
  {
    MenuManager_StepTempModeCustom_InternalState = MENUMANAGER_STEPTEMPMODECUSTOM_INTERNALSTATE_RUNNING;
  }
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_StepTempModeCustom_StopBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_StepTempModeCustom_InternalState != MENUMANAGER_STEPTEMPMODECUSTOM_INTERNALSTATE_READY)
  {
    return FSM_GUARD_FALSE;
  }
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_StepTempModeCustom_UpBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  uint32_t tempUnitVal;
  uint32_t tempVal;

  if (MenuManager_StepTempModeCustom_InternalState == MENUMANAGER_STEPTEMPMODECUSTOM_INTERNALSTATE_READY)
  {
    tempUnitVal = MenuManager_StepTempModeCustom_UnitVal(MenuManager_StepTempModeCustom_CurPos);

    MenuManager_StepTempModeCustom_UnitVal(MenuManager_StepTempModeCustom_CurPos) += (uint32_t)1U;

    if (MenuManager_StepTempModeCustom_UnitVal(MenuManager_StepTempModeCustom_CurPos) > (uint32_t)9U)
    {
      MenuManager_StepTempModeCustom_UnitVal(MenuManager_StepTempModeCustom_CurPos) = (uint32_t)0U;
    }

    MenuManager_Common_BcdToDecConv(&tempVal, &(MenuManager_StepTempModeCustom_UnitVal(0)), (uint8_t)MENUMANAGER_STEPTEMPMODECUSTOM_UNITVAL_LENGTH);

    if ((tempVal < MenuManager_StepTempModeCustom_ValueMin) || (tempVal > MenuManager_StepTempModeCustom_ValueMax))
    {
      MenuManager_StepTempModeCustom_UnitVal(MenuManager_StepTempModeCustom_CurPos) = tempUnitVal;
    }

    MenuManager_StepTempModeCustom_LcdShowAdjust();
  }

  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_StepTempModeCustom_DownBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  uint32_t tempUnitVal;
  uint32_t tempVal;

  if (MenuManager_StepTempModeCustom_InternalState == MENUMANAGER_STEPTEMPMODECUSTOM_INTERNALSTATE_READY)
  {
    tempUnitVal = MenuManager_StepTempModeCustom_UnitVal(MenuManager_StepTempModeCustom_CurPos);

    MenuManager_StepTempModeCustom_UnitVal(MenuManager_StepTempModeCustom_CurPos) -= (uint32_t)1U;

    if (MenuManager_StepTempModeCustom_UnitVal(MenuManager_StepTempModeCustom_CurPos) > (uint32_t)9U)
    {
      MenuManager_StepTempModeCustom_UnitVal(MenuManager_StepTempModeCustom_CurPos) = (uint32_t)9U;
    }

    MenuManager_Common_BcdToDecConv(&tempVal, &(MenuManager_StepTempModeCustom_UnitVal(0)), (uint8_t)MENUMANAGER_STEPTEMPMODECUSTOM_UNITVAL_LENGTH);

    if ((tempVal < MenuManager_StepTempModeCustom_ValueMin) || (tempVal > MenuManager_StepTempModeCustom_ValueMax))
    {
      MenuManager_StepTempModeCustom_UnitVal(MenuManager_StepTempModeCustom_CurPos) = tempUnitVal;
    }

    MenuManager_StepTempModeCustom_LcdShowAdjust();
  }
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_StepTempModeCustom_AddBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_StepTempModeCustom_InternalState == MENUMANAGER_STEPTEMPMODECUSTOM_INTERNALSTATE_READY)
  {
    if (MenuManager_StepTempModeCustom_CurPos > (uint32_t)0U)
    {
      MenuManager_StepTempModeCustom_CurPos -= (uint32_t)1U;
    }

    MenuManager_StepTempModeCustom_LcdShowAdjust();
  }
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_StepTempModeCustom_SubBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_StepTempModeCustom_InternalState == MENUMANAGER_STEPTEMPMODECUSTOM_INTERNALSTATE_READY)
  {
    if (MenuManager_StepTempModeCustom_CurPos < (MENUMANAGER_STEPTEMPMODECUSTOM_UNITVAL_LENGTH - (uint32_t)1U))
    {
      MenuManager_StepTempModeCustom_CurPos += (uint32_t)1U;
    }
    
    MenuManager_StepTempModeCustom_LcdShowAdjust();
  }
  
  return FSM_GUARD_TRUE;
}



/*=============================================================================================*/
static void MenuManager_StepTempModeCustom_SubMainFunction(void)
{
  MenuManager_Common_ButEventMapHandler(&MenuManager_StepTempModeCustom_ButEventMapConf);

  switch (MenuManager_StepTempModeCustom_InternalState)
  {
    case MENUMANAGER_STEPTEMPMODECUSTOM_INTERNALSTATE_READY:
      break;
    case MENUMANAGER_STEPTEMPMODECUSTOM_INTERNALSTATE_RUNNING:
    {
      uint8_t tempTempThreshold;

      MenuManager_Common_BcdToDecConv(&(MenuManager_StepTempModeCustom_Value), &(MenuManager_StepTempModeCustom_UnitVal(0)), (uint8_t)MENUMANAGER_STEPTEMPMODECUSTOM_UNITVAL_LENGTH);

      tempTempThreshold = (uint8_t)MenuManager_StepTempModeCustom_Value;

      ProgramManager_NormStepConfig_TempThreshold_SetData \
      ( \
        (uint8_t)MenuManager_StepTempModeCustom_SeqIdx, \
        (uint8_t)MenuManager_StepTempModeCustom_StepIdx, \
        &tempTempThreshold, \
        ProgramManager_gParamConfig.machineFuncCfg.tempUnit \
      );

      if ((uint8_t)MenuManager_StepTempModeCustom_SeqIdx == ProgramManager_gAutoSeqConfig.sequenceIndex)
      {
        ProgramManager_NormStepConfig_TempThreshold_GetData \
        ( \
          (uint8_t)MenuManager_StepTempModeCustom_SeqIdx, \
          (uint8_t)MenuManager_StepTempModeCustom_StepIdx, \
          &((ProgramManager_gAutoSeqConfig.normStep)[MenuManager_StepTempModeCustom_StepIdx].tempThreshold), \
          ProgramManager_gParamConfig.machineFuncCfg.tempUnit \
        );
      }
      
      MenuManager_StepTempModeCustom_InternalState = MENUMANAGER_STEPTEMPMODECUSTOM_INTERNALSTATE_DONE;
      
      break;
    }
    case MENUMANAGER_STEPTEMPMODECUSTOM_INTERNALSTATE_DONE:
    {
      MenuManager_StepTempModeCustom_LcdShowDone();

      MenuManager_StepTempModeCustom_InternalState = MENUMANAGER_STEPTEMPMODECUSTOM_INTERNALSTATE_EXITING;

      break;
    }
    case MENUMANAGER_STEPTEMPMODECUSTOM_INTERNALSTATE_EXITING:
    default:
      break;
  }
}

/*=============================================================================================*/
static void MenuManager_StepTempModeCustom_SubTickHandler(void)
{
  if (MenuManager_StepTempModeCustom_InternalState == MENUMANAGER_STEPTEMPMODECUSTOM_INTERNALSTATE_EXITING)
  {
    MenuManager_StepTempModeCustom_Counter += (uint32_t)1U;

    if (MenuManager_StepTempModeCustom_Counter >= MENUMANAGER_STEPTEMPMODECUSTOM_COUNTER_MAX)
    {
      MenuManager_StepTempModeCustom_Counter = (uint32_t)0U;
      
      Fsm_TriggerEvent(&MenuManager_FsmContext, (Fsm_EventType)MENUMANAGER_EVENT_PREV);
    }
  }
}



#ifdef  __cplusplus
}
#endif
