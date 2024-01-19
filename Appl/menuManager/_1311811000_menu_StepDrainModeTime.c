/* 
 * File:   _1311811000_menu_StepDrainModeTime.c
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
#define MENUMANAGER_STEPDRAINMODETIME_INTERNALDATALENGTH              (uint8_t)13U

#define MENUMANAGER_STEPDRAINMODETIME_SEQIDX_IDX                      0U
#define MENUMANAGER_STEPDRAINMODETIME_STEPIDX_IDX                     1U
#define MENUMANAGER_STEPDRAINMODETIME_DRAINIDX_IDX                    2U
#define MENUMANAGER_STEPDRAINMODETIME_INTERNALSTATE_IDX               3U
#define MENUMANAGER_STEPDRAINMODETIME_COUNTER_IDX                     4U
#define MENUMANAGER_STEPDRAINMODETIME_CURPOS_IDX                      5U
#define MENUMANAGER_STEPDRAINMODETIME_VALUE_MIN_IDX                   6U
#define MENUMANAGER_STEPDRAINMODETIME_VALUE_MAX_IDX                   7U
#define MENUMANAGER_STEPDRAINMODETIME_VALUE_IDX                       8U
#define MENUMANAGER_STEPDRAINMODETIME_UNITVAL_IDX                     9U

#define MenuManager_StepDrainModeTime_SeqIdx                          MenuManager_GetInternalDataPtr(MENUMANAGER_STEPDRAINMODETIME_SEQIDX_IDX)
#define MenuManager_StepDrainModeTime_StepIdx                         MenuManager_GetInternalDataPtr(MENUMANAGER_STEPDRAINMODETIME_STEPIDX_IDX)
#define MenuManager_StepDrainModeTime_DrainIdx                        MenuManager_GetInternalDataPtr(MENUMANAGER_STEPDRAINMODETIME_DRAINIDX_IDX)
#define MenuManager_StepDrainModeTime_InternalState                   MenuManager_GetInternalDataPtr(MENUMANAGER_STEPDRAINMODETIME_INTERNALSTATE_IDX)
#define MenuManager_StepDrainModeTime_Counter                         MenuManager_GetInternalDataPtr(MENUMANAGER_STEPDRAINMODETIME_COUNTER_IDX)
#define MenuManager_StepDrainModeTime_CurPos                          MenuManager_GetInternalDataPtr(MENUMANAGER_STEPDRAINMODETIME_CURPOS_IDX)
#define MenuManager_StepDrainModeTime_ValueMin                        MenuManager_GetInternalDataPtr(MENUMANAGER_STEPDRAINMODETIME_VALUE_MIN_IDX)
#define MenuManager_StepDrainModeTime_ValueMax                        MenuManager_GetInternalDataPtr(MENUMANAGER_STEPDRAINMODETIME_VALUE_MAX_IDX)
#define MenuManager_StepDrainModeTime_Value                           MenuManager_GetInternalDataPtr(MENUMANAGER_STEPDRAINMODETIME_VALUE_IDX)
#define MenuManager_StepDrainModeTime_UnitVal(idx)                    MenuManager_GetInternalDataPtr(MENUMANAGER_STEPDRAINMODETIME_UNITVAL_IDX + idx)



#define MENUMANAGER_STEPDRAINMODETIME_INTERNALSTATE_READY             (uint32_t)0U
#define MENUMANAGER_STEPDRAINMODETIME_INTERNALSTATE_RUNNING           (uint32_t)1U
#define MENUMANAGER_STEPDRAINMODETIME_INTERNALSTATE_DONE              (uint32_t)2U
#define MENUMANAGER_STEPDRAINMODETIME_INTERNALSTATE_EXITING           (uint32_t)3U

#define MENUMANAGER_STEPDRAINMODETIME_COUNTER_MAX                     (uint32_t)50U

#define MENUMANAGER_STEPDRAINMODETIME_CURPOS_XPOS                     (uint32_t)5U
#define MENUMANAGER_STEPDRAINMODETIME_CURPOS_YPOS                     (uint32_t)5U

#define MENUMANAGER_STEPDRAINMODETIME_UNIT_XPOS                       (uint32_t)10U
#define MENUMANAGER_STEPDRAINMODETIME_UNIT_YPOS                       (uint32_t)5U

#define MENUMANAGER_STEPDRAINMODETIME_UNITVAL_LENGTH                  (uint32_t)4U



/** Menu manager main titles and child menu titles */
static const uint8_t MenuManager_StepDrainModeTime_MainTitleStr[] =   "DRAIN TIME";



/** Menu manager button event mapping array */
static MenuManager_ButEventMapStruct MenuManager_StepDrainModeTime_ButEventMap[6] =
{
  { IOMANAGER_BUTTON_STATE_START,                                     MENUMANAGER_EVENT_START_BUT             },
  { IOMANAGER_BUTTON_STATE_STOP,                                      MENUMANAGER_EVENT_STOP_BUT              },
  { IOMANAGER_BUTTON_STATE_UP,                                        MENUMANAGER_EVENT_UP_BUT                },
  { IOMANAGER_BUTTON_STATE_DOWN,                                      MENUMANAGER_EVENT_DOWN_BUT              },
  { IOMANAGER_BUTTON_STATE_ADD,                                       MENUMANAGER_EVENT_ADD_BUT               },
  { IOMANAGER_BUTTON_STATE_SUB,                                       MENUMANAGER_EVENT_SUB_BUT               }
};

/** Menu manager button event mapping configuration */
static MenuManager_ButEventMapConfStruct MenuManager_StepDrainModeTime_ButEventMapConf =
{
  .butEventMapNum             = (uint8_t)6U,
  .butEventMapCfg             = &MenuManager_StepDrainModeTime_ButEventMap
};



/** Menu manager event handlers */
static Fsm_GuardType MenuManager_StepDrainModeTime_Entry              (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_StepDrainModeTime_Exit               (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_StepDrainModeTime_StartBut           (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_StepDrainModeTime_StopBut            (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_StepDrainModeTime_UpBut              (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_StepDrainModeTime_DownBut            (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_StepDrainModeTime_AddBut             (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_StepDrainModeTime_SubBut             (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);

/** Menu manager state machine */
Fsm_EventEntryStruct MenuManager_StepDrainModeTime_StateMachine[9] =
{
  FSM_TRIGGER_ENTRY             (                                     MenuManager_StepDrainModeTime_Entry                                             ),
  FSM_TRIGGER_EXIT              (                                     MenuManager_StepDrainModeTime_Exit                                              ),
  FSM_TRIGGER_TRANSITION        ( MENUMANAGER_EVENT_PREV,                                                     MENUMANAGER_STATE_STEP_DRAIN_MODE_CUSTOM),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_START_BUT,        MenuManager_StepDrainModeTime_StartBut                                          ),
  FSM_TRIGGER_TRANSITION_ACTION ( MENUMANAGER_EVENT_STOP_BUT,         MenuManager_StepDrainModeTime_StopBut,  MENUMANAGER_STATE_STEP_DRAIN_MODE_CUSTOM),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_UP_BUT,           MenuManager_StepDrainModeTime_UpBut                                             ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_DOWN_BUT,         MenuManager_StepDrainModeTime_DownBut                                           ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_ADD_BUT,          MenuManager_StepDrainModeTime_AddBut                                            ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_SUB_BUT,          MenuManager_StepDrainModeTime_SubBut                                            )
};



/*===============================================================================================
*                                   LOCAL FUNCTION PROTOTYPES
===============================================================================================*/

static void MenuManager_StepDrainModeTime_LcdShowMainTitle(void);
static void MenuManager_StepDrainModeTime_LcdShowAdjust(void);
static void MenuManager_StepDrainModeTime_LcdShowDone(void);

static void MenuManager_StepDrainModeTime_SubMainFunction(void);
static void MenuManager_StepDrainModeTime_SubTickHandler(void);



/*===============================================================================================
*                                       LOCAL FUNCTIONS
===============================================================================================*/

static void MenuManager_StepDrainModeTime_LcdShowMainTitle(void)
{
  /* Print main title */
  MenuManager_Common_LcdClearAdjustMenuStatic();
  MenuManager_Common_LcdShowMainTitle(MenuManager_StepDrainModeTime_MainTitleStr);
  MenuManager_Common_LcdShowOkExitChoice();
  MenuManager_Common_LcdUpdateAdjustMenuStatic();
}

/*=============================================================================================*/
static void MenuManager_StepDrainModeTime_LcdShowAdjust(void)
{
  MenuManager_Common_LcdClearAdjustMenuDynamic();

  MenuManager_Common_LcdShowAdjustUnitVal
  (
    MENUMANAGER_STEPDRAINMODETIME_CURPOS_XPOS,
    MENUMANAGER_STEPDRAINMODETIME_CURPOS_YPOS,
    &(MenuManager_StepDrainModeTime_UnitVal(0)),
    MENUMANAGER_STEPDRAINMODETIME_UNITVAL_LENGTH
  );
  
  MenuManager_Common_LcdShowAdjustArrow
  (
    MENUMANAGER_STEPDRAINMODETIME_CURPOS_XPOS,
    MENUMANAGER_STEPDRAINMODETIME_CURPOS_YPOS,
    MenuManager_StepDrainModeTime_CurPos
  );

  LCD_SetCursorPos(MENUMANAGER_STEPDRAINMODETIME_UNIT_XPOS, MENUMANAGER_STEPDRAINMODETIME_UNIT_YPOS, LCD_CURSOR_BY_FONT);
  LCD_PutString((uint8_t *)MenuManager_Common_SecondStr);

  MenuManager_Common_LcdUpdateAdjustMenuDynamic();
}

/*=============================================================================================*/
static void MenuManager_StepDrainModeTime_LcdShowDone(void)
{
  MenuManager_Common_LcdClearAll();

  MenuManager_Common_LcdShowAdjustUnitVal
  (
    MENUMANAGER_STEPDRAINMODETIME_CURPOS_XPOS,
    MENUMANAGER_STEPDRAINMODETIME_CURPOS_YPOS,
    &(MenuManager_StepDrainModeTime_UnitVal(0)),
    MENUMANAGER_STEPDRAINMODETIME_UNITVAL_LENGTH
  );

  LCD_SetCursorPos(MENUMANAGER_STEPDRAINMODETIME_UNIT_XPOS, MENUMANAGER_STEPDRAINMODETIME_UNIT_YPOS, LCD_CURSOR_BY_FONT);
  LCD_PutString((uint8_t *)MenuManager_Common_SecondStr);

  MenuManager_Common_LcdUpdateAll();
}



/*=============================================================================================*/
static Fsm_GuardType MenuManager_StepDrainModeTime_Entry(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  MenuManager_Common_ProgramSetupStruct* enterDataHierachy;
  uint16_t tempStepDrainTime;
  HAL_StatusTypeDef retVal = HAL_OK;

  /* Check if previous state data hierachy is not empty */
  if (pFsmContext->dataHierachy != NULL)
  {
    if (pFsmContext->dataHierachy->dataId == MENUMANAGER_STATE_STEP_DRAIN_MODE_CUSTOM)
    {
      enterDataHierachy = (MenuManager_Common_ProgramSetupStruct *)(pFsmContext->dataHierachy);

      MenuManager_InternalDataPush(MENUMANAGER_STEPDRAINMODETIME_INTERNALDATALENGTH);

      MenuManager_StepDrainModeTime_SeqIdx = enterDataHierachy->seqIdx;
      MenuManager_StepDrainModeTime_StepIdx = enterDataHierachy->stepIdx;
      MenuManager_StepDrainModeTime_DrainIdx = enterDataHierachy->drainIdx;

      MenuManager_StepDrainModeTime_InternalState = MENUMANAGER_STEPDRAINMODETIME_INTERNALSTATE_READY;
      MenuManager_StepDrainModeTime_Counter = (uint32_t)0U;
      MenuManager_StepDrainModeTime_CurPos = (uint32_t)0U;

      ProgramManager_NormStepConfig_DrainTime_GetData \
      ( \
        (uint8_t)MenuManager_StepDrainModeTime_SeqIdx,   \
        (uint8_t)MenuManager_StepDrainModeTime_StepIdx,  \
        (uint8_t)MenuManager_StepDrainModeTime_DrainIdx, \
        &tempStepDrainTime \
      );

      MenuManager_StepDrainModeTime_ValueMin = (uint32_t)ProgramManager_DrainStep_DrainTimeMin[MenuManager_StepDrainModeTime_DrainIdx];
      MenuManager_StepDrainModeTime_ValueMax = (uint32_t)ProgramManager_DrainStep_DrainTimeMax[MenuManager_StepDrainModeTime_DrainIdx];
      MenuManager_StepDrainModeTime_Value = (uint32_t)(tempStepDrainTime);

      MenuManager_Common_DecToBcdConv
      (
        &(MenuManager_StepDrainModeTime_Value),
        &(MenuManager_StepDrainModeTime_UnitVal(0)),
        (uint8_t)MENUMANAGER_STEPDRAINMODETIME_UNITVAL_LENGTH
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
    MenuManager_StepDrainModeTime_LcdShowMainTitle();
    MenuManager_StepDrainModeTime_LcdShowAdjust();

    MenuManager_SubMainFunction = MenuManager_StepDrainModeTime_SubMainFunction;
    MenuManager_SubTickHandler = MenuManager_StepDrainModeTime_SubTickHandler;

    return FSM_GUARD_TRUE;
  }

  return FSM_GUARD_FALSE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_StepDrainModeTime_Exit(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  Fsm_DataHierachyStruct* dataHierachy;

  MenuManager_SubMainFunction = NULL;
  MenuManager_SubTickHandler = NULL;

  dataHierachy = (Fsm_DataHierachyStruct *)MenuManager_malloc(sizeof(Fsm_DataHierachyStruct));
  dataHierachy->dataId = MENUMANAGER_STATE_STEP_DRAIN_MODE_TIME;

  pFsmContext->dataHierachy = dataHierachy;

  /* Free internal data */
  MenuManager_InternalDataPop();
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_StepDrainModeTime_StartBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_StepDrainModeTime_InternalState == MENUMANAGER_STEPDRAINMODETIME_INTERNALSTATE_READY)
  {
    MenuManager_StepDrainModeTime_InternalState = MENUMANAGER_STEPDRAINMODETIME_INTERNALSTATE_RUNNING;
  }
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_StepDrainModeTime_StopBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_StepDrainModeTime_InternalState != MENUMANAGER_STEPDRAINMODETIME_INTERNALSTATE_READY)
  {
    return FSM_GUARD_FALSE;
  }
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_StepDrainModeTime_UpBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  uint32_t tempUnitVal;
  uint32_t tempVal;

  if (MenuManager_StepDrainModeTime_InternalState == MENUMANAGER_STEPDRAINMODETIME_INTERNALSTATE_READY)
  {
    tempUnitVal = MenuManager_StepDrainModeTime_UnitVal(MenuManager_StepDrainModeTime_CurPos);

    MenuManager_StepDrainModeTime_UnitVal(MenuManager_StepDrainModeTime_CurPos) += (uint32_t)1U;

    if (MenuManager_StepDrainModeTime_UnitVal(MenuManager_StepDrainModeTime_CurPos) > (uint32_t)9U)
    {
      MenuManager_StepDrainModeTime_UnitVal(MenuManager_StepDrainModeTime_CurPos) = (uint32_t)0U;
    }

    MenuManager_Common_BcdToDecConv(&tempVal, &(MenuManager_StepDrainModeTime_UnitVal(0)), (uint8_t)MENUMANAGER_STEPDRAINMODETIME_UNITVAL_LENGTH);

    if ((tempVal < MenuManager_StepDrainModeTime_ValueMin) || (tempVal > MenuManager_StepDrainModeTime_ValueMax))
    {
      MenuManager_StepDrainModeTime_UnitVal(MenuManager_StepDrainModeTime_CurPos) = tempUnitVal;
    }

    MenuManager_StepDrainModeTime_LcdShowAdjust();
  }

  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_StepDrainModeTime_DownBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  uint32_t tempUnitVal;
  uint32_t tempVal;

  if (MenuManager_StepDrainModeTime_InternalState == MENUMANAGER_STEPDRAINMODETIME_INTERNALSTATE_READY)
  {
    tempUnitVal = MenuManager_StepDrainModeTime_UnitVal(MenuManager_StepDrainModeTime_CurPos);

    MenuManager_StepDrainModeTime_UnitVal(MenuManager_StepDrainModeTime_CurPos) -= (uint32_t)1U;

    if (MenuManager_StepDrainModeTime_UnitVal(MenuManager_StepDrainModeTime_CurPos) > (uint32_t)9U)
    {
      MenuManager_StepDrainModeTime_UnitVal(MenuManager_StepDrainModeTime_CurPos) = (uint32_t)9U;
    }

    MenuManager_Common_BcdToDecConv(&tempVal, &(MenuManager_StepDrainModeTime_UnitVal(0)), (uint8_t)MENUMANAGER_STEPDRAINMODETIME_UNITVAL_LENGTH);

    if ((tempVal < MenuManager_StepDrainModeTime_ValueMin) || (tempVal > MenuManager_StepDrainModeTime_ValueMax))
    {
      MenuManager_StepDrainModeTime_UnitVal(MenuManager_StepDrainModeTime_CurPos) = tempUnitVal;
    }

    MenuManager_StepDrainModeTime_LcdShowAdjust();
  }
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_StepDrainModeTime_AddBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_StepDrainModeTime_InternalState == MENUMANAGER_STEPDRAINMODETIME_INTERNALSTATE_READY)
  {
    if (MenuManager_StepDrainModeTime_CurPos > (uint32_t)0U)
    {
      MenuManager_StepDrainModeTime_CurPos -= (uint32_t)1U;
    }

    MenuManager_StepDrainModeTime_LcdShowAdjust();
  }
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_StepDrainModeTime_SubBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_StepDrainModeTime_InternalState == MENUMANAGER_STEPDRAINMODETIME_INTERNALSTATE_READY)
  {
    if (MenuManager_StepDrainModeTime_CurPos < (MENUMANAGER_STEPDRAINMODETIME_UNITVAL_LENGTH - (uint32_t)1U))
    {
      MenuManager_StepDrainModeTime_CurPos += (uint32_t)1U;
    }
    
    MenuManager_StepDrainModeTime_LcdShowAdjust();
  }
  
  return FSM_GUARD_TRUE;
}



/*=============================================================================================*/
static void MenuManager_StepDrainModeTime_SubMainFunction(void)
{
  MenuManager_Common_ButEventMapHandler(&MenuManager_StepDrainModeTime_ButEventMapConf);

  switch (MenuManager_StepDrainModeTime_InternalState)
  {
    case MENUMANAGER_STEPDRAINMODETIME_INTERNALSTATE_READY:
      break;
    case MENUMANAGER_STEPDRAINMODETIME_INTERNALSTATE_RUNNING:
    {
      uint16_t tempStepDrainTime;

      MenuManager_Common_BcdToDecConv(&(MenuManager_StepDrainModeTime_Value), &(MenuManager_StepDrainModeTime_UnitVal(0)), (uint8_t)MENUMANAGER_STEPDRAINMODETIME_UNITVAL_LENGTH);

      tempStepDrainTime = (uint16_t)MenuManager_StepDrainModeTime_Value;

      ProgramManager_NormStepConfig_DrainTime_SetData \
      ( \
        (uint8_t)MenuManager_StepDrainModeTime_SeqIdx,   \
        (uint8_t)MenuManager_StepDrainModeTime_StepIdx,  \
        (uint8_t)MenuManager_StepDrainModeTime_DrainIdx, \
        &tempStepDrainTime \
      );

      if (ProgramManager_gAutoSeqConfig.sequenceIndex == (uint8_t)MenuManager_StepDrainModeTime_SeqIdx)
      {
        ProgramManager_NormStepConfig_DrainTime_GetData \
        ( \
          (uint8_t)MenuManager_StepDrainModeTime_SeqIdx,   \
          (uint8_t)MenuManager_StepDrainModeTime_StepIdx,  \
          (uint8_t)MenuManager_StepDrainModeTime_DrainIdx, \
          &(ProgramManager_gAutoSeqConfig.normStep[MenuManager_StepDrainModeTime_StepIdx].drainTime)[MenuManager_StepDrainModeTime_DrainIdx] \
        );
      }

      MenuManager_StepDrainModeTime_InternalState = MENUMANAGER_STEPDRAINMODETIME_INTERNALSTATE_DONE;
      
      break;
    }
    case MENUMANAGER_STEPDRAINMODETIME_INTERNALSTATE_DONE:
    {
      MenuManager_StepDrainModeTime_LcdShowDone();

      MenuManager_StepDrainModeTime_InternalState = MENUMANAGER_STEPDRAINMODETIME_INTERNALSTATE_EXITING;

      break;
    }
    case MENUMANAGER_STEPDRAINMODETIME_INTERNALSTATE_EXITING:
    default:
      break;
  }
}

/*=============================================================================================*/
static void MenuManager_StepDrainModeTime_SubTickHandler(void)
{
  if (MenuManager_StepDrainModeTime_InternalState == MENUMANAGER_STEPDRAINMODETIME_INTERNALSTATE_EXITING)
  {
    MenuManager_StepDrainModeTime_Counter += (uint32_t)1U;

    if (MenuManager_StepDrainModeTime_Counter >= MENUMANAGER_STEPDRAINMODETIME_COUNTER_MAX)
    {
      MenuManager_StepDrainModeTime_Counter = (uint32_t)0U;
      
      Fsm_TriggerEvent(&MenuManager_FsmContext, (Fsm_EventType)MENUMANAGER_EVENT_PREV);
    }
  }
}



#ifdef  __cplusplus
}
#endif
