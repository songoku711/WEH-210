/* 
 * File:   _1312100000_menu_StepBalanceTime.c
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
#define MENUMANAGER_STEPBALANCETIME_INTERNALDATALENGTH                (uint8_t)10U

#define MENUMANAGER_STEPBALANCETIME_INTERNALSTATE_IDX                 0U
#define MENUMANAGER_STEPBALANCETIME_COUNTER_IDX                       1U
#define MENUMANAGER_STEPBALANCETIME_CURPOS_IDX                        2U
#define MENUMANAGER_STEPBALANCETIME_SEQIDX_IDX                        3U
#define MENUMANAGER_STEPBALANCETIME_VALUE_MIN_IDX                     4U
#define MENUMANAGER_STEPBALANCETIME_VALUE_MAX_IDX                     5U
#define MENUMANAGER_STEPBALANCETIME_VALUE_IDX                         6U
#define MENUMANAGER_STEPBALANCETIME_UNITVAL_IDX                       7U

#define MenuManager_StepBalanceTime_InternalState                     MenuManager_GetInternalDataPtr(MENUMANAGER_STEPBALANCETIME_INTERNALSTATE_IDX)
#define MenuManager_StepBalanceTime_Counter                           MenuManager_GetInternalDataPtr(MENUMANAGER_STEPBALANCETIME_COUNTER_IDX)
#define MenuManager_StepBalanceTime_CurPos                            MenuManager_GetInternalDataPtr(MENUMANAGER_STEPBALANCETIME_CURPOS_IDX)
#define MenuManager_StepBalanceTime_SeqIdx                            MenuManager_GetInternalDataPtr(MENUMANAGER_STEPBALANCETIME_SEQIDX_IDX)
#define MenuManager_StepBalanceTime_ValueMin                          MenuManager_GetInternalDataPtr(MENUMANAGER_STEPBALANCETIME_VALUE_MIN_IDX)
#define MenuManager_StepBalanceTime_ValueMax                          MenuManager_GetInternalDataPtr(MENUMANAGER_STEPBALANCETIME_VALUE_MAX_IDX)
#define MenuManager_StepBalanceTime_Value                             MenuManager_GetInternalDataPtr(MENUMANAGER_STEPBALANCETIME_VALUE_IDX)
#define MenuManager_StepBalanceTime_UnitVal(idx)                      MenuManager_GetInternalDataPtr(MENUMANAGER_STEPBALANCETIME_UNITVAL_IDX + idx)



#define MENUMANAGER_STEPBALANCETIME_INTERNALSTATE_READY               (uint32_t)0U
#define MENUMANAGER_STEPBALANCETIME_INTERNALSTATE_RUNNING             (uint32_t)1U
#define MENUMANAGER_STEPBALANCETIME_INTERNALSTATE_DONE                (uint32_t)2U
#define MENUMANAGER_STEPBALANCETIME_INTERNALSTATE_EXITING             (uint32_t)3U

#define MENUMANAGER_STEPBALANCETIME_COUNTER_MAX                       (uint32_t)50U

#define MENUMANAGER_STEPBALANCETIME_CURPOS_XPOS                       (uint32_t)5U
#define MENUMANAGER_STEPBALANCETIME_CURPOS_YPOS                       (uint32_t)5U

#define MENUMANAGER_STEPBALANCETIME_UNIT_XPOS                         (uint32_t)9U
#define MENUMANAGER_STEPBALANCETIME_UNIT_YPOS                         (uint32_t)5U

#define MENUMANAGER_STEPBALANCETIME_UNITVAL_LENGTH                    (uint32_t)3U



/** Menu manager main titles and child menu titles */
static const uint8_t MenuManager_StepBalanceTime_MainTitleStr[] =     "USER BAL TIME";



/** Menu manager button event mapping array */
static MenuManager_ButEventMapStruct MenuManager_StepBalanceTime_ButEventMap[6] =
{
  { IOMANAGER_BUTTON_STATE_START,                                     MENUMANAGER_EVENT_START_BUT             },
  { IOMANAGER_BUTTON_STATE_STOP,                                      MENUMANAGER_EVENT_STOP_BUT              },
  { IOMANAGER_BUTTON_STATE_UP,                                        MENUMANAGER_EVENT_UP_BUT                },
  { IOMANAGER_BUTTON_STATE_DOWN,                                      MENUMANAGER_EVENT_DOWN_BUT              },
  { IOMANAGER_BUTTON_STATE_ADD,                                       MENUMANAGER_EVENT_ADD_BUT               },
  { IOMANAGER_BUTTON_STATE_SUB,                                       MENUMANAGER_EVENT_SUB_BUT               }
};

/** Menu manager button event mapping configuration */
static MenuManager_ButEventMapConfStruct MenuManager_StepBalanceTime_ButEventMapConf =
{
  .butEventMapNum             = (uint8_t)6U,
  .butEventMapCfg             = &MenuManager_StepBalanceTime_ButEventMap
};



/** Menu manager event handlers */
static Fsm_GuardType MenuManager_StepBalanceTime_Entry                (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_StepBalanceTime_Exit                 (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_StepBalanceTime_StartBut             (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_StepBalanceTime_StopBut              (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_StepBalanceTime_UpBut                (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_StepBalanceTime_DownBut              (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_StepBalanceTime_AddBut               (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_StepBalanceTime_SubBut               (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);

/** Menu manager state machine */
Fsm_EventEntryStruct MenuManager_StepBalanceTime_StateMachine[9] =
{
  FSM_TRIGGER_ENTRY             (                                     MenuManager_StepBalanceTime_Entry                                               ),
  FSM_TRIGGER_EXIT              (                                     MenuManager_StepBalanceTime_Exit                                                ),
  FSM_TRIGGER_TRANSITION        ( MENUMANAGER_EVENT_PREV,                                                     MENUMANAGER_STATE_STEP_EXTRACT_PARAM    ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_START_BUT,        MenuManager_StepBalanceTime_StartBut                                            ),
  FSM_TRIGGER_TRANSITION_ACTION ( MENUMANAGER_EVENT_STOP_BUT,         MenuManager_StepBalanceTime_StopBut,    MENUMANAGER_STATE_STEP_EXTRACT_PARAM    ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_UP_BUT,           MenuManager_StepBalanceTime_UpBut                                               ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_DOWN_BUT,         MenuManager_StepBalanceTime_DownBut                                             ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_ADD_BUT,          MenuManager_StepBalanceTime_AddBut                                              ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_SUB_BUT,          MenuManager_StepBalanceTime_SubBut                                              )
};



/*===============================================================================================
*                                   LOCAL FUNCTION PROTOTYPES
===============================================================================================*/

static void MenuManager_StepBalanceTime_LcdShowMainTitle(void);
static void MenuManager_StepBalanceTime_LcdShowAdjust(void);
static void MenuManager_StepBalanceTime_LcdShowDone(void);

static void MenuManager_StepBalanceTime_SubMainFunction(void);
static void MenuManager_StepBalanceTime_SubTickHandler(void);



/*===============================================================================================
*                                       LOCAL FUNCTIONS
===============================================================================================*/

static void MenuManager_StepBalanceTime_LcdShowMainTitle(void)
{
  /* Print main title */
  MenuManager_Common_LcdClearAdjustMenuStatic();
  MenuManager_Common_LcdShowMainTitle(MenuManager_StepBalanceTime_MainTitleStr);
  MenuManager_Common_LcdShowOkExitChoice();
  MenuManager_Common_LcdUpdateAdjustMenuStatic();
}

/*=============================================================================================*/
static void MenuManager_StepBalanceTime_LcdShowAdjust(void)
{
  MenuManager_Common_LcdClearAdjustMenuDynamic();

  MenuManager_Common_LcdShowAdjustUnitVal
  (
    MENUMANAGER_STEPBALANCETIME_CURPOS_XPOS,
    MENUMANAGER_STEPBALANCETIME_CURPOS_YPOS,
    &(MenuManager_StepBalanceTime_UnitVal(0)),
    MENUMANAGER_STEPBALANCETIME_UNITVAL_LENGTH
  );
  
  MenuManager_Common_LcdShowAdjustArrow
  (
    MENUMANAGER_STEPBALANCETIME_CURPOS_XPOS,
    MENUMANAGER_STEPBALANCETIME_CURPOS_YPOS,
    MenuManager_StepBalanceTime_CurPos
  );

  LCD_SetCursorPos(MENUMANAGER_STEPBALANCETIME_UNIT_XPOS, MENUMANAGER_STEPBALANCETIME_UNIT_YPOS, LCD_CURSOR_BY_FONT);
  LCD_PutString((uint8_t *)MenuManager_Common_SecondStr);

  MenuManager_Common_LcdUpdateAdjustMenuDynamic();
}

/*=============================================================================================*/
static void MenuManager_StepBalanceTime_LcdShowDone(void)
{
  MenuManager_Common_LcdClearAll();

  MenuManager_Common_LcdShowAdjustUnitVal
  (
    MENUMANAGER_STEPBALANCETIME_CURPOS_XPOS,
    MENUMANAGER_STEPBALANCETIME_CURPOS_YPOS,
    &(MenuManager_StepBalanceTime_UnitVal(0)),
    MENUMANAGER_STEPBALANCETIME_UNITVAL_LENGTH
  );

  LCD_SetCursorPos(MENUMANAGER_STEPBALANCETIME_UNIT_XPOS, MENUMANAGER_STEPBALANCETIME_UNIT_YPOS, LCD_CURSOR_BY_FONT);
  LCD_PutString((uint8_t *)MenuManager_Common_SecondStr);

  MenuManager_Common_LcdUpdateAll();
}



/*=============================================================================================*/
static Fsm_GuardType MenuManager_StepBalanceTime_Entry(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  MenuManager_Common_ProgramSetupStruct* enterDataHierachy;
  uint16_t tempBalanceTime;
  HAL_StatusTypeDef retVal = HAL_OK;

  /* Check if previous state data hierachy is not empty */
  if (pFsmContext->dataHierachy != NULL)
  {
    if (pFsmContext->dataHierachy->dataId == MENUMANAGER_STATE_STEP_EXTRACT_PARAM)
    {
      enterDataHierachy = (MenuManager_Common_ProgramSetupStruct *)(pFsmContext->dataHierachy);

      MenuManager_InternalDataPush(MENUMANAGER_STEPBALANCETIME_INTERNALDATALENGTH);

      MenuManager_StepBalanceTime_InternalState = MENUMANAGER_STEPBALANCETIME_INTERNALSTATE_READY;
      MenuManager_StepBalanceTime_Counter = (uint32_t)0U;
      MenuManager_StepBalanceTime_CurPos = (uint32_t)0U;

      MenuManager_StepBalanceTime_SeqIdx = enterDataHierachy->seqIdx;

      ProgramManager_ExtractStepConfig_BalanceTime_GetData
      (
        (uint8_t)MenuManager_StepBalanceTime_SeqIdx,
        &tempBalanceTime
      );

      MenuManager_StepBalanceTime_ValueMin = (uint32_t)PROGRAMMANAGER_EXTRACTSETUP_COMMON_TIME_MIN;
      MenuManager_StepBalanceTime_ValueMax = (uint32_t)PROGRAMMANAGER_EXTRACTSETUP_COMMON_TIME_MAX;

      MenuManager_StepBalanceTime_Value = (uint32_t)tempBalanceTime;

      MenuManager_Common_DecToBcdConv
      (
        &(MenuManager_StepBalanceTime_Value),
        &(MenuManager_StepBalanceTime_UnitVal(0)),
        (uint8_t)MENUMANAGER_STEPBALANCETIME_UNITVAL_LENGTH
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
    MenuManager_StepBalanceTime_LcdShowMainTitle();
    MenuManager_StepBalanceTime_LcdShowAdjust();

    MenuManager_SubMainFunction = MenuManager_StepBalanceTime_SubMainFunction;
    MenuManager_SubTickHandler = MenuManager_StepBalanceTime_SubTickHandler;

    return FSM_GUARD_TRUE;
  }

  return FSM_GUARD_FALSE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_StepBalanceTime_Exit(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  Fsm_DataHierachyStruct* dataHierachy;

  MenuManager_SubMainFunction = NULL;
  MenuManager_SubTickHandler = NULL;

  dataHierachy = (Fsm_DataHierachyStruct *)MenuManager_malloc(sizeof(Fsm_DataHierachyStruct));
  dataHierachy->dataId = MENUMANAGER_STATE_STEP_BALANCE_TIME;

  pFsmContext->dataHierachy = dataHierachy;

  /* Free internal data */
  MenuManager_InternalDataPop();
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_StepBalanceTime_StartBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_StepBalanceTime_InternalState == MENUMANAGER_STEPBALANCETIME_INTERNALSTATE_READY)
  {
    MenuManager_StepBalanceTime_InternalState = MENUMANAGER_STEPBALANCETIME_INTERNALSTATE_RUNNING;
  }
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_StepBalanceTime_StopBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_StepBalanceTime_InternalState != MENUMANAGER_STEPBALANCETIME_INTERNALSTATE_READY)
  {
    return FSM_GUARD_FALSE;
  }
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_StepBalanceTime_UpBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  uint32_t tempUnitVal;
  uint32_t tempVal;

  if (MenuManager_StepBalanceTime_InternalState == MENUMANAGER_STEPBALANCETIME_INTERNALSTATE_READY)
  {
    tempUnitVal = MenuManager_StepBalanceTime_UnitVal(MenuManager_StepBalanceTime_CurPos);

    MenuManager_StepBalanceTime_UnitVal(MenuManager_StepBalanceTime_CurPos) += (uint32_t)1U;

    if (MenuManager_StepBalanceTime_UnitVal(MenuManager_StepBalanceTime_CurPos) > (uint32_t)9U)
    {
      MenuManager_StepBalanceTime_UnitVal(MenuManager_StepBalanceTime_CurPos) = (uint32_t)0U;
    }

    MenuManager_Common_BcdToDecConv(&tempVal, &(MenuManager_StepBalanceTime_UnitVal(0)), (uint8_t)MENUMANAGER_STEPBALANCETIME_UNITVAL_LENGTH);

    if ((tempVal <= MenuManager_StepBalanceTime_ValueMin) || (tempVal >= MenuManager_StepBalanceTime_ValueMax))
    {
      MenuManager_StepBalanceTime_UnitVal(MenuManager_StepBalanceTime_CurPos) = tempUnitVal;
    }

    MenuManager_StepBalanceTime_LcdShowAdjust();
  }

  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_StepBalanceTime_DownBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  uint32_t tempUnitVal;
  uint32_t tempVal;

  if (MenuManager_StepBalanceTime_InternalState == MENUMANAGER_STEPBALANCETIME_INTERNALSTATE_READY)
  {
    tempUnitVal = MenuManager_StepBalanceTime_UnitVal(MenuManager_StepBalanceTime_CurPos);

    MenuManager_StepBalanceTime_UnitVal(MenuManager_StepBalanceTime_CurPos) -= (uint32_t)1U;

    if (MenuManager_StepBalanceTime_UnitVal(MenuManager_StepBalanceTime_CurPos) > (uint32_t)9U)
    {
      MenuManager_StepBalanceTime_UnitVal(MenuManager_StepBalanceTime_CurPos) = (uint32_t)9U;
    }

    MenuManager_Common_BcdToDecConv(&tempVal, &(MenuManager_StepBalanceTime_UnitVal(0)), (uint8_t)MENUMANAGER_STEPBALANCETIME_UNITVAL_LENGTH);

    if ((tempVal <= MenuManager_StepBalanceTime_ValueMin) || (tempVal >= MenuManager_StepBalanceTime_ValueMax))
    {
      MenuManager_StepBalanceTime_UnitVal(MenuManager_StepBalanceTime_CurPos) = tempUnitVal;
    }

    MenuManager_StepBalanceTime_LcdShowAdjust();
  }
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_StepBalanceTime_AddBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_StepBalanceTime_InternalState == MENUMANAGER_STEPBALANCETIME_INTERNALSTATE_READY)
  {
    if (MenuManager_StepBalanceTime_CurPos > (uint32_t)0U)
    {
      MenuManager_StepBalanceTime_CurPos -= (uint32_t)1U;
    }

    MenuManager_StepBalanceTime_LcdShowAdjust();
  }
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_StepBalanceTime_SubBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_StepBalanceTime_InternalState == MENUMANAGER_STEPBALANCETIME_INTERNALSTATE_READY)
  {
    if (MenuManager_StepBalanceTime_CurPos < (MENUMANAGER_STEPBALANCETIME_UNITVAL_LENGTH - (uint32_t)1U))
    {
      MenuManager_StepBalanceTime_CurPos += (uint32_t)1U;
    }
    
    MenuManager_StepBalanceTime_LcdShowAdjust();
  }
  
  return FSM_GUARD_TRUE;
}



/*=============================================================================================*/
static void MenuManager_StepBalanceTime_SubMainFunction(void)
{
  MenuManager_Common_ButEventMapHandler(&MenuManager_StepBalanceTime_ButEventMapConf);

  switch (MenuManager_StepBalanceTime_InternalState)
  {
    case MENUMANAGER_STEPBALANCETIME_INTERNALSTATE_READY:
      break;
    case MENUMANAGER_STEPBALANCETIME_INTERNALSTATE_RUNNING:
    {
      uint16_t tempBalanceTime;

      MenuManager_Common_BcdToDecConv(&(MenuManager_StepBalanceTime_Value), &(MenuManager_StepBalanceTime_UnitVal(0)), (uint8_t)MENUMANAGER_STEPBALANCETIME_UNITVAL_LENGTH);

      tempBalanceTime = (uint16_t)MenuManager_StepBalanceTime_Value;

      ProgramManager_ExtractStepConfig_BalanceTime_SetData \
      ( \
        (uint8_t)MenuManager_StepBalanceTime_SeqIdx, \
        &tempBalanceTime \
      );

      if ((uint8_t)MenuManager_StepBalanceTime_SeqIdx == ProgramManager_gAutoSeqConfig.sequenceIndex)
      {
        ProgramManager_ExtractStepConfig_BalanceTime_GetData \
        ( \
          (uint8_t)MenuManager_StepBalanceTime_SeqIdx, \
          &(ProgramManager_gAutoSeqConfig.extractStep.balanceTime) \
        );
      }

      MenuManager_StepBalanceTime_InternalState = MENUMANAGER_STEPBALANCETIME_INTERNALSTATE_DONE;
      
      break;
    }
    case MENUMANAGER_STEPBALANCETIME_INTERNALSTATE_DONE:
    {
      MenuManager_StepBalanceTime_LcdShowDone();

      MenuManager_StepBalanceTime_InternalState = MENUMANAGER_STEPBALANCETIME_INTERNALSTATE_EXITING;

      break;
    }
    case MENUMANAGER_STEPBALANCETIME_INTERNALSTATE_EXITING:
    default:
      break;
  }
}

/*=============================================================================================*/
static void MenuManager_StepBalanceTime_SubTickHandler(void)
{
  if (MenuManager_StepBalanceTime_InternalState == MENUMANAGER_STEPBALANCETIME_INTERNALSTATE_EXITING)
  {
    MenuManager_StepBalanceTime_Counter += (uint32_t)1U;

    if (MenuManager_StepBalanceTime_Counter >= MENUMANAGER_STEPBALANCETIME_COUNTER_MAX)
    {
      MenuManager_StepBalanceTime_Counter = (uint32_t)0U;
      
      Fsm_TriggerEvent(&MenuManager_FsmContext, (Fsm_EventType)MENUMANAGER_EVENT_PREV);
    }
  }
}



#ifdef  __cplusplus
}
#endif
