/* 
 * File:   _1311412000_menu_StepWashStopTime.c
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
#define MENUMANAGER_STEPWASHSTOPTIME_INTERNALDATALENGTH               (uint8_t)11U

#define MENUMANAGER_STEPWASHSTOPTIME_INTERNALSTATE_IDX                0U
#define MENUMANAGER_STEPWASHSTOPTIME_COUNTER_IDX                      1U
#define MENUMANAGER_STEPWASHSTOPTIME_CURPOS_IDX                       2U
#define MENUMANAGER_STEPWASHSTOPTIME_SEQIDX_IDX                       3U
#define MENUMANAGER_STEPWASHSTOPTIME_STEPIDX_IDX                      4U
#define MENUMANAGER_STEPWASHSTOPTIME_VALUE_MIN_IDX                    5U
#define MENUMANAGER_STEPWASHSTOPTIME_VALUE_MAX_IDX                    6U
#define MENUMANAGER_STEPWASHSTOPTIME_VALUE_IDX                        7U
#define MENUMANAGER_STEPWASHSTOPTIME_UNITVAL_IDX                      8U

#define MenuManager_StepWashStopTime_InternalState                    MenuManager_GetInternalDataPtr(MENUMANAGER_STEPWASHSTOPTIME_INTERNALSTATE_IDX)
#define MenuManager_StepWashStopTime_Counter                          MenuManager_GetInternalDataPtr(MENUMANAGER_STEPWASHSTOPTIME_COUNTER_IDX)
#define MenuManager_StepWashStopTime_CurPos                           MenuManager_GetInternalDataPtr(MENUMANAGER_STEPWASHSTOPTIME_CURPOS_IDX)
#define MenuManager_StepWashStopTime_SeqIdx                           MenuManager_GetInternalDataPtr(MENUMANAGER_STEPWASHSTOPTIME_SEQIDX_IDX)
#define MenuManager_StepWashStopTime_StepIdx                          MenuManager_GetInternalDataPtr(MENUMANAGER_STEPWASHSTOPTIME_STEPIDX_IDX)
#define MenuManager_StepWashStopTime_ValueMin                         MenuManager_GetInternalDataPtr(MENUMANAGER_STEPWASHSTOPTIME_VALUE_MIN_IDX)
#define MenuManager_StepWashStopTime_ValueMax                         MenuManager_GetInternalDataPtr(MENUMANAGER_STEPWASHSTOPTIME_VALUE_MAX_IDX)
#define MenuManager_StepWashStopTime_Value                            MenuManager_GetInternalDataPtr(MENUMANAGER_STEPWASHSTOPTIME_VALUE_IDX)
#define MenuManager_StepWashStopTime_UnitVal(idx)                     MenuManager_GetInternalDataPtr(MENUMANAGER_STEPWASHSTOPTIME_UNITVAL_IDX + idx)



#define MENUMANAGER_STEPWASHSTOPTIME_INTERNALSTATE_READY              (uint32_t)0U
#define MENUMANAGER_STEPWASHSTOPTIME_INTERNALSTATE_RUNNING            (uint32_t)1U
#define MENUMANAGER_STEPWASHSTOPTIME_INTERNALSTATE_DONE               (uint32_t)2U
#define MENUMANAGER_STEPWASHSTOPTIME_INTERNALSTATE_EXITING            (uint32_t)3U

#define MENUMANAGER_STEPWASHSTOPTIME_COUNTER_MAX                      (uint32_t)50U

#define MENUMANAGER_STEPWASHSTOPTIME_CURPOS_XPOS                      (uint32_t)5U
#define MENUMANAGER_STEPWASHSTOPTIME_CURPOS_YPOS                      (uint32_t)5U

#define MENUMANAGER_STEPWASHSTOPTIME_UNIT_XPOS                        (uint32_t)9U
#define MENUMANAGER_STEPWASHSTOPTIME_UNIT_YPOS                        (uint32_t)5U

#define MENUMANAGER_STEPWASHSTOPTIME_UNITVAL_LENGTH                   (uint32_t)3U



/** Menu manager main titles and child menu titles */
static const uint8_t MenuManager_StepWashStopTime_MainTitleStr[] =    "USER WASH STOP T";



/** Menu manager button event mapping array */
static MenuManager_ButEventMapStruct MenuManager_StepWashStopTime_ButEventMap[6] =
{
  { IOMANAGER_BUTTON_STATE_START,                                     MENUMANAGER_EVENT_START_BUT             },
  { IOMANAGER_BUTTON_STATE_STOP,                                      MENUMANAGER_EVENT_STOP_BUT              },
  { IOMANAGER_BUTTON_STATE_UP,                                        MENUMANAGER_EVENT_UP_BUT                },
  { IOMANAGER_BUTTON_STATE_DOWN,                                      MENUMANAGER_EVENT_DOWN_BUT              },
  { IOMANAGER_BUTTON_STATE_ADD,                                       MENUMANAGER_EVENT_ADD_BUT               },
  { IOMANAGER_BUTTON_STATE_SUB,                                       MENUMANAGER_EVENT_SUB_BUT               }
};

/** Menu manager button event mapping configuration */
static MenuManager_ButEventMapConfStruct MenuManager_StepWashStopTime_ButEventMapConf =
{
  .butEventMapNum             = (uint8_t)6U,
  .butEventMapCfg             = &MenuManager_StepWashStopTime_ButEventMap
};



/** Menu manager event handlers */
static Fsm_GuardType MenuManager_StepWashStopTime_Entry               (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_StepWashStopTime_Exit                (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_StepWashStopTime_StartBut            (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_StepWashStopTime_StopBut             (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_StepWashStopTime_UpBut               (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_StepWashStopTime_DownBut             (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_StepWashStopTime_AddBut              (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_StepWashStopTime_SubBut              (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);

/** Menu manager state machine */
Fsm_EventEntryStruct MenuManager_StepWashStopTime_StateMachine[9] =
{
  FSM_TRIGGER_ENTRY             (                                     MenuManager_StepWashStopTime_Entry                                              ),
  FSM_TRIGGER_EXIT              (                                     MenuManager_StepWashStopTime_Exit                                               ),
  FSM_TRIGGER_TRANSITION        ( MENUMANAGER_EVENT_PREV,                                                     MENUMANAGER_STATE_STEP_WASH_MODE_CUSTOM ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_START_BUT,        MenuManager_StepWashStopTime_StartBut                                           ),
  FSM_TRIGGER_TRANSITION_ACTION ( MENUMANAGER_EVENT_STOP_BUT,         MenuManager_StepWashStopTime_StopBut,   MENUMANAGER_STATE_STEP_WASH_MODE_CUSTOM ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_UP_BUT,           MenuManager_StepWashStopTime_UpBut                                              ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_DOWN_BUT,         MenuManager_StepWashStopTime_DownBut                                            ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_ADD_BUT,          MenuManager_StepWashStopTime_AddBut                                             ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_SUB_BUT,          MenuManager_StepWashStopTime_SubBut                                             )
};



/*===============================================================================================
*                                   LOCAL FUNCTION PROTOTYPES
===============================================================================================*/

static void MenuManager_StepWashStopTime_LcdShowMainTitle(void);
static void MenuManager_StepWashStopTime_LcdShowAdjust(void);
static void MenuManager_StepWashStopTime_LcdShowDone(void);

static void MenuManager_StepWashStopTime_SubMainFunction(void);
static void MenuManager_StepWashStopTime_SubTickHandler(void);



/*===============================================================================================
*                                       LOCAL FUNCTIONS
===============================================================================================*/

static void MenuManager_StepWashStopTime_LcdShowMainTitle(void)
{
  /* Print main title */
  MenuManager_Common_LcdClearAdjustMenuStatic();
  MenuManager_Common_LcdShowMainTitle(MenuManager_StepWashStopTime_MainTitleStr);
  MenuManager_Common_LcdShowOkExitChoice();
  MenuManager_Common_LcdUpdateAdjustMenuStatic();
}

/*=============================================================================================*/
static void MenuManager_StepWashStopTime_LcdShowAdjust(void)
{
  MenuManager_Common_LcdClearAdjustMenuDynamic();

  MenuManager_Common_LcdShowAdjustUnitVal
  (
    MENUMANAGER_STEPWASHSTOPTIME_CURPOS_XPOS,
    MENUMANAGER_STEPWASHSTOPTIME_CURPOS_YPOS,
    &(MenuManager_StepWashStopTime_UnitVal(0)),
    MENUMANAGER_STEPWASHSTOPTIME_UNITVAL_LENGTH
  );
  
  MenuManager_Common_LcdShowAdjustArrow
  (
    MENUMANAGER_STEPWASHSTOPTIME_CURPOS_XPOS,
    MENUMANAGER_STEPWASHSTOPTIME_CURPOS_YPOS,
    MenuManager_StepWashStopTime_CurPos
  );

  LCD_SetCursorPos(MENUMANAGER_STEPWASHSTOPTIME_UNIT_XPOS, MENUMANAGER_STEPWASHSTOPTIME_UNIT_YPOS, LCD_CURSOR_BY_FONT);
  LCD_PutString((uint8_t *)MenuManager_Common_SecondStr);

  MenuManager_Common_LcdUpdateAdjustMenuDynamic();
}

/*=============================================================================================*/
static void MenuManager_StepWashStopTime_LcdShowDone(void)
{
  MenuManager_Common_LcdClearAll();

  MenuManager_Common_LcdShowAdjustUnitVal
  (
    MENUMANAGER_STEPWASHSTOPTIME_CURPOS_XPOS,
    MENUMANAGER_STEPWASHSTOPTIME_CURPOS_YPOS,
    &(MenuManager_StepWashStopTime_UnitVal(0)),
    MENUMANAGER_STEPWASHSTOPTIME_UNITVAL_LENGTH
  );

  LCD_SetCursorPos(MENUMANAGER_STEPWASHSTOPTIME_UNIT_XPOS, MENUMANAGER_STEPWASHSTOPTIME_UNIT_YPOS, LCD_CURSOR_BY_FONT);
  LCD_PutString((uint8_t *)MenuManager_Common_SecondStr);

  MenuManager_Common_LcdUpdateAll();
}



/*=============================================================================================*/
static Fsm_GuardType MenuManager_StepWashStopTime_Entry(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  MenuManager_Common_ProgramSetupStruct* enterDataHierachy;
  uint16_t tempWashStopTime;
  HAL_StatusTypeDef retVal = HAL_OK;

  /* Check if previous state data hierachy is not empty */
  if (pFsmContext->dataHierachy != NULL)
  {
    if (pFsmContext->dataHierachy->dataId == MENUMANAGER_STATE_STEP_WASH_MODE_CUSTOM)
    {
      enterDataHierachy = (MenuManager_Common_ProgramSetupStruct *)(pFsmContext->dataHierachy);

      MenuManager_InternalDataPush(MENUMANAGER_STEPWASHSTOPTIME_INTERNALDATALENGTH);

      MenuManager_StepWashStopTime_InternalState = MENUMANAGER_STEPWASHSTOPTIME_INTERNALSTATE_READY;
      MenuManager_StepWashStopTime_Counter = (uint32_t)0U;
      MenuManager_StepWashStopTime_CurPos = (uint32_t)0U;

      MenuManager_StepWashStopTime_SeqIdx = enterDataHierachy->seqIdx;
      MenuManager_StepWashStopTime_StepIdx = enterDataHierachy->stepIdx;

      ProgramManager_NormStepConfig_WashStopTime_GetData((uint8_t)MenuManager_StepWashStopTime_SeqIdx, (uint8_t)MenuManager_StepWashStopTime_StepIdx, &tempWashStopTime);

      MenuManager_StepWashStopTime_ValueMin = (uint32_t)PROGRAMMANAGER_WASHSETUP_COMMON_TIME_MIN;
      MenuManager_StepWashStopTime_ValueMax = (uint32_t)PROGRAMMANAGER_WASHSETUP_COMMON_TIME_MAX;
      MenuManager_StepWashStopTime_Value = (uint32_t)tempWashStopTime;

      MenuManager_Common_DecToBcdConv
      (
        &(MenuManager_StepWashStopTime_Value),
        &(MenuManager_StepWashStopTime_UnitVal(0)),
        (uint8_t)MENUMANAGER_STEPWASHSTOPTIME_UNITVAL_LENGTH
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
    MenuManager_StepWashStopTime_LcdShowMainTitle();
    MenuManager_StepWashStopTime_LcdShowAdjust();

    MenuManager_SubMainFunction = MenuManager_StepWashStopTime_SubMainFunction;
    MenuManager_SubTickHandler = MenuManager_StepWashStopTime_SubTickHandler;

    return FSM_GUARD_TRUE;
  }

  return FSM_GUARD_FALSE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_StepWashStopTime_Exit(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  Fsm_DataHierachyStruct* dataHierachy;

  MenuManager_SubMainFunction = NULL;
  MenuManager_SubTickHandler = NULL;

  dataHierachy = (Fsm_DataHierachyStruct *)MenuManager_malloc(sizeof(Fsm_DataHierachyStruct));
  dataHierachy->dataId = MENUMANAGER_STATE_STEP_WASH_STOP_TIME;

  pFsmContext->dataHierachy = dataHierachy;

  /* Free internal data */
  MenuManager_InternalDataPop();
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_StepWashStopTime_StartBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_StepWashStopTime_InternalState == MENUMANAGER_STEPWASHSTOPTIME_INTERNALSTATE_READY)
  {
    MenuManager_StepWashStopTime_InternalState = MENUMANAGER_STEPWASHSTOPTIME_INTERNALSTATE_RUNNING;
  }
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_StepWashStopTime_StopBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_StepWashStopTime_InternalState != MENUMANAGER_STEPWASHSTOPTIME_INTERNALSTATE_READY)
  {
    return FSM_GUARD_FALSE;
  }
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_StepWashStopTime_UpBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  uint32_t tempUnitVal;
  uint32_t tempVal;

  if (MenuManager_StepWashStopTime_InternalState == MENUMANAGER_STEPWASHSTOPTIME_INTERNALSTATE_READY)
  {
    tempUnitVal = MenuManager_StepWashStopTime_UnitVal(MenuManager_StepWashStopTime_CurPos);

    MenuManager_StepWashStopTime_UnitVal(MenuManager_StepWashStopTime_CurPos) += (uint32_t)1U;

    if (MenuManager_StepWashStopTime_UnitVal(MenuManager_StepWashStopTime_CurPos) > (uint32_t)9U)
    {
      MenuManager_StepWashStopTime_UnitVal(MenuManager_StepWashStopTime_CurPos) = (uint32_t)0U;
    }

    MenuManager_Common_BcdToDecConv(&tempVal, &(MenuManager_StepWashStopTime_UnitVal(0)), (uint8_t)MENUMANAGER_STEPWASHSTOPTIME_UNITVAL_LENGTH);

    if ((tempVal <= MenuManager_StepWashStopTime_ValueMin) || (tempVal >= MenuManager_StepWashStopTime_ValueMax))
    {
      MenuManager_StepWashStopTime_UnitVal(MenuManager_StepWashStopTime_CurPos) = tempUnitVal;
    }

    MenuManager_StepWashStopTime_LcdShowAdjust();
  }

  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_StepWashStopTime_DownBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  uint32_t tempUnitVal;
  uint32_t tempVal;

  if (MenuManager_StepWashStopTime_InternalState == MENUMANAGER_STEPWASHSTOPTIME_INTERNALSTATE_READY)
  {
    tempUnitVal = MenuManager_StepWashStopTime_UnitVal(MenuManager_StepWashStopTime_CurPos);

    MenuManager_StepWashStopTime_UnitVal(MenuManager_StepWashStopTime_CurPos) -= (uint32_t)1U;

    if (MenuManager_StepWashStopTime_UnitVal(MenuManager_StepWashStopTime_CurPos) > (uint32_t)9U)
    {
      MenuManager_StepWashStopTime_UnitVal(MenuManager_StepWashStopTime_CurPos) = (uint32_t)9U;
    }

    MenuManager_Common_BcdToDecConv(&tempVal, &(MenuManager_StepWashStopTime_UnitVal(0)), (uint8_t)MENUMANAGER_STEPWASHSTOPTIME_UNITVAL_LENGTH);

    if ((tempVal <= MenuManager_StepWashStopTime_ValueMin) || (tempVal >= MenuManager_StepWashStopTime_ValueMax))
    {
      MenuManager_StepWashStopTime_UnitVal(MenuManager_StepWashStopTime_CurPos) = tempUnitVal;
    }

    MenuManager_StepWashStopTime_LcdShowAdjust();
  }
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_StepWashStopTime_AddBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_StepWashStopTime_InternalState == MENUMANAGER_STEPWASHSTOPTIME_INTERNALSTATE_READY)
  {
    if (MenuManager_StepWashStopTime_CurPos > (uint32_t)0U)
    {
      MenuManager_StepWashStopTime_CurPos -= (uint32_t)1U;
    }

    MenuManager_StepWashStopTime_LcdShowAdjust();
  }
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_StepWashStopTime_SubBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_StepWashStopTime_InternalState == MENUMANAGER_STEPWASHSTOPTIME_INTERNALSTATE_READY)
  {
    if (MenuManager_StepWashStopTime_CurPos < (MENUMANAGER_STEPWASHSTOPTIME_UNITVAL_LENGTH - (uint32_t)1U))
    {
      MenuManager_StepWashStopTime_CurPos += (uint32_t)1U;
    }
    
    MenuManager_StepWashStopTime_LcdShowAdjust();
  }
  
  return FSM_GUARD_TRUE;
}



/*=============================================================================================*/
static void MenuManager_StepWashStopTime_SubMainFunction(void)
{
  MenuManager_Common_ButEventMapHandler(&MenuManager_StepWashStopTime_ButEventMapConf);

  switch (MenuManager_StepWashStopTime_InternalState)
  {
    case MENUMANAGER_STEPWASHSTOPTIME_INTERNALSTATE_READY:
      break;
    case MENUMANAGER_STEPWASHSTOPTIME_INTERNALSTATE_RUNNING:
    {
      uint16_t tempWashStopTime;

      MenuManager_Common_BcdToDecConv(&(MenuManager_StepWashStopTime_Value), &(MenuManager_StepWashStopTime_UnitVal(0)), (uint8_t)MENUMANAGER_STEPWASHSTOPTIME_UNITVAL_LENGTH);

      tempWashStopTime = (uint16_t)MenuManager_StepWashStopTime_Value;

      ProgramManager_NormStepConfig_WashStopTime_SetData \
      ( \
        (uint8_t)MenuManager_StepWashStopTime_SeqIdx, \
        (uint8_t)MenuManager_StepWashStopTime_StepIdx, \
        &tempWashStopTime \
      );

      if ((uint8_t)MenuManager_StepWashStopTime_SeqIdx == ProgramManager_gAutoSeqConfig.sequenceIndex)
      {
        ProgramManager_NormStepConfig_WashStopTime_GetData \
        ( \
          (uint8_t)MenuManager_StepWashStopTime_SeqIdx, \
          (uint8_t)MenuManager_StepWashStopTime_StepIdx, \
          &((ProgramManager_gAutoSeqConfig.normStep)[MenuManager_StepWashStopTime_StepIdx].washStopTime) \
        );
      }

      MenuManager_StepWashStopTime_InternalState = MENUMANAGER_STEPWASHSTOPTIME_INTERNALSTATE_DONE;
      
      break;
    }
    case MENUMANAGER_STEPWASHSTOPTIME_INTERNALSTATE_DONE:
    {
      MenuManager_StepWashStopTime_LcdShowDone();

      MenuManager_StepWashStopTime_InternalState = MENUMANAGER_STEPWASHSTOPTIME_INTERNALSTATE_EXITING;

      break;
    }
    case MENUMANAGER_STEPWASHSTOPTIME_INTERNALSTATE_EXITING:
    default:
      break;
  }
}

/*=============================================================================================*/
static void MenuManager_StepWashStopTime_SubTickHandler(void)
{
  if (MenuManager_StepWashStopTime_InternalState == MENUMANAGER_STEPWASHSTOPTIME_INTERNALSTATE_EXITING)
  {
    MenuManager_StepWashStopTime_Counter += (uint32_t)1U;

    if (MenuManager_StepWashStopTime_Counter >= MENUMANAGER_STEPWASHSTOPTIME_COUNTER_MAX)
    {
      MenuManager_StepWashStopTime_Counter = (uint32_t)0U;
      
      Fsm_TriggerEvent(&MenuManager_FsmContext, (Fsm_EventType)MENUMANAGER_EVENT_PREV);
    }
  }
}



#ifdef  __cplusplus
}
#endif
