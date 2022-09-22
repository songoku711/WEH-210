/* 
 * File:   _1311500000_menu_StepWashNum.c
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
#define MENUMANAGER_STEPWASHNUM_INTERNALDATALENGTH                    (uint8_t)10U

#define MENUMANAGER_STEPWASHNUM_INTERNALSTATE_IDX                     0U
#define MENUMANAGER_STEPWASHNUM_COUNTER_IDX                           1U
#define MENUMANAGER_STEPWASHNUM_CURPOS_IDX                            2U
#define MENUMANAGER_STEPWASHNUM_SEQIDX_IDX                            3U
#define MENUMANAGER_STEPWASHNUM_STEPIDX_IDX                           4U
#define MENUMANAGER_STEPWASHNUM_VALUE_MIN_IDX                         5U
#define MENUMANAGER_STEPWASHNUM_VALUE_MAX_IDX                         6U
#define MENUMANAGER_STEPWASHNUM_VALUE_IDX                             7U
#define MENUMANAGER_STEPWASHNUM_UNITVAL_IDX                           8U

#define MenuManager_StepWashNum_InternalState                         MenuManager_GetInternalDataPtr(MENUMANAGER_STEPWASHNUM_INTERNALSTATE_IDX)
#define MenuManager_StepWashNum_Counter                               MenuManager_GetInternalDataPtr(MENUMANAGER_STEPWASHNUM_COUNTER_IDX)
#define MenuManager_StepWashNum_CurPos                                MenuManager_GetInternalDataPtr(MENUMANAGER_STEPWASHNUM_CURPOS_IDX)
#define MenuManager_StepWashNum_SeqIdx                                MenuManager_GetInternalDataPtr(MENUMANAGER_STEPWASHNUM_SEQIDX_IDX)
#define MenuManager_StepWashNum_StepIdx                               MenuManager_GetInternalDataPtr(MENUMANAGER_STEPWASHNUM_STEPIDX_IDX)
#define MenuManager_StepWashNum_ValueMin                              MenuManager_GetInternalDataPtr(MENUMANAGER_STEPWASHNUM_VALUE_MIN_IDX)
#define MenuManager_StepWashNum_ValueMax                              MenuManager_GetInternalDataPtr(MENUMANAGER_STEPWASHNUM_VALUE_MAX_IDX)
#define MenuManager_StepWashNum_Value                                 MenuManager_GetInternalDataPtr(MENUMANAGER_STEPWASHNUM_VALUE_IDX)
#define MenuManager_StepWashNum_UnitVal(idx)                          MenuManager_GetInternalDataPtr(MENUMANAGER_STEPWASHNUM_UNITVAL_IDX + idx)



#define MENUMANAGER_STEPWASHNUM_INTERNALSTATE_READY                   (uint32_t)0U
#define MENUMANAGER_STEPWASHNUM_INTERNALSTATE_RUNNING                 (uint32_t)1U
#define MENUMANAGER_STEPWASHNUM_INTERNALSTATE_DONE                    (uint32_t)2U
#define MENUMANAGER_STEPWASHNUM_INTERNALSTATE_EXITING                 (uint32_t)3U

#define MENUMANAGER_STEPWASHNUM_COUNTER_MAX                           (uint32_t)50U

#define MENUMANAGER_STEPWASHNUM_CURPOS_XPOS                           (uint32_t)10U
#define MENUMANAGER_STEPWASHNUM_CURPOS_YPOS                           (uint32_t)5U

#define MENUMANAGER_STEPWASHNUM_UNIT_XPOS                             (uint32_t)1U
#define MENUMANAGER_STEPWASHNUM_UNIT_YPOS                             (uint32_t)3U

#define MENUMANAGER_STEPWASHNUM_UNITVAL_LENGTH                        (uint32_t)2U



/** Menu manager main titles and child menu titles */
static const uint8_t MenuManager_StepWashNum_MainTitleStr[] =         "WASH SPIN NUM";



/** Menu manager button event mapping array */
static MenuManager_ButEventMapStruct MenuManager_StepWashNum_ButEventMap[6] =
{
  { IOMANAGER_BUTTON_STATE_START,                                     MENUMANAGER_EVENT_START_BUT             },
  { IOMANAGER_BUTTON_STATE_STOP,                                      MENUMANAGER_EVENT_STOP_BUT              },
  { IOMANAGER_BUTTON_STATE_UP,                                        MENUMANAGER_EVENT_UP_BUT                },
  { IOMANAGER_BUTTON_STATE_DOWN,                                      MENUMANAGER_EVENT_DOWN_BUT              },
  { IOMANAGER_BUTTON_STATE_ADD,                                       MENUMANAGER_EVENT_ADD_BUT               },
  { IOMANAGER_BUTTON_STATE_SUB,                                       MENUMANAGER_EVENT_SUB_BUT               }
};

/** Menu manager button event mapping configuration */
static MenuManager_ButEventMapConfStruct MenuManager_StepWashNum_ButEventMapConf =
{
  .butEventMapNum             = (uint8_t)6U,
  .butEventMapCfg             = &MenuManager_StepWashNum_ButEventMap
};



/** Menu manager event handlers */
static Fsm_GuardType MenuManager_StepWashNum_Entry                    (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_StepWashNum_Exit                     (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_StepWashNum_StartBut                 (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_StepWashNum_StopBut                  (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_StepWashNum_UpBut                    (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_StepWashNum_DownBut                  (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_StepWashNum_AddBut                   (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_StepWashNum_SubBut                   (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);

/** Menu manager state machine */
Fsm_EventEntryStruct MenuManager_StepWashNum_StateMachine[9] =
{
  FSM_TRIGGER_ENTRY             (                                     MenuManager_StepWashNum_Entry                                                   ),
  FSM_TRIGGER_EXIT              (                                     MenuManager_StepWashNum_Exit                                                    ),
  FSM_TRIGGER_TRANSITION        ( MENUMANAGER_EVENT_PREV,                                                     MENUMANAGER_STATE_PROGRAM_MAIN_SETUP    ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_START_BUT,        MenuManager_StepWashNum_StartBut                                                ),
  FSM_TRIGGER_TRANSITION_ACTION ( MENUMANAGER_EVENT_STOP_BUT,         MenuManager_StepWashNum_StopBut,        MENUMANAGER_STATE_PROGRAM_MAIN_SETUP    ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_UP_BUT,           MenuManager_StepWashNum_UpBut                                                   ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_DOWN_BUT,         MenuManager_StepWashNum_DownBut                                                 ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_ADD_BUT,          MenuManager_StepWashNum_AddBut                                                  ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_SUB_BUT,          MenuManager_StepWashNum_SubBut                                                  )
};



/*===============================================================================================
*                                   LOCAL FUNCTION PROTOTYPES
===============================================================================================*/

static void MenuManager_StepWashNum_LcdShowMainTitle(void);
static void MenuManager_StepWashNum_LcdShowAdjust(void);
static void MenuManager_StepWashNum_LcdShowDone(void);

static void MenuManager_StepWashNum_SubMainFunction(void);
static void MenuManager_StepWashNum_SubTickHandler(void);



/*===============================================================================================
*                                       LOCAL FUNCTIONS
===============================================================================================*/

static void MenuManager_StepWashNum_LcdShowMainTitle(void)
{
  /* Print main title */
  MenuManager_Common_LcdClearAdjustMenuStatic();
  MenuManager_Common_LcdShowMainTitle(MenuManager_StepWashNum_MainTitleStr);
  MenuManager_Common_LcdShowOkExitChoice();
  MenuManager_Common_LcdUpdateAdjustMenuStatic();
}

/*=============================================================================================*/
static void MenuManager_StepWashNum_LcdShowAdjust(void)
{
  MenuManager_Common_LcdClearAdjustMenuDynamic();

  MenuManager_Common_LcdShowAdjustUnitVal
  (
    MENUMANAGER_STEPWASHNUM_CURPOS_XPOS,
    MENUMANAGER_STEPWASHNUM_CURPOS_YPOS,
    &(MenuManager_StepWashNum_UnitVal(0)),
    MENUMANAGER_STEPWASHNUM_UNITVAL_LENGTH
  );
  
  MenuManager_Common_LcdShowAdjustArrow
  (
    MENUMANAGER_STEPWASHNUM_CURPOS_XPOS,
    MENUMANAGER_STEPWASHNUM_CURPOS_YPOS,
    MenuManager_StepWashNum_CurPos
  );

  LCD_SetCursorPos(MENUMANAGER_STEPWASHNUM_UNIT_XPOS, MENUMANAGER_STEPWASHNUM_UNIT_YPOS, LCD_CURSOR_BY_FONT);
  LCD_PutString((uint8_t *)MenuManager_Common_TimeStr);

  MenuManager_Common_LcdUpdateAdjustMenuDynamic();
}

/*=============================================================================================*/
static void MenuManager_StepWashNum_LcdShowDone(void)
{
  MenuManager_Common_LcdClearAll();

  MenuManager_Common_LcdShowAdjustUnitVal
  (
    MENUMANAGER_STEPWASHNUM_CURPOS_XPOS,
    MENUMANAGER_STEPWASHNUM_CURPOS_YPOS,
    &(MenuManager_StepWashNum_UnitVal(0)),
    MENUMANAGER_STEPWASHNUM_UNITVAL_LENGTH
  );

  LCD_SetCursorPos(MENUMANAGER_STEPWASHNUM_UNIT_XPOS, MENUMANAGER_STEPWASHNUM_UNIT_YPOS, LCD_CURSOR_BY_FONT);
  LCD_PutString((uint8_t *)MenuManager_Common_TimeStr);

  MenuManager_Common_LcdUpdateAll();
}



/*=============================================================================================*/
static Fsm_GuardType MenuManager_StepWashNum_Entry(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  MenuManager_Common_ProgramSetupStruct* enterDataHierachy;
  uint8_t tempWashNum;
  HAL_StatusTypeDef retVal = HAL_OK;

  /* Check if previous state data hierachy is not empty */
  if (pFsmContext->dataHierachy != NULL)
  {
    if (pFsmContext->dataHierachy->dataId == MENUMANAGER_STATE_PROGRAM_MAIN_SETUP)
    {
      enterDataHierachy = (MenuManager_Common_ProgramSetupStruct *)(pFsmContext->dataHierachy);

      MenuManager_InternalDataPush(MENUMANAGER_STEPWASHNUM_INTERNALDATALENGTH);

      MenuManager_StepWashNum_InternalState = MENUMANAGER_STEPWASHNUM_INTERNALSTATE_READY;
      MenuManager_StepWashNum_Counter = (uint32_t)0U;
      MenuManager_StepWashNum_CurPos = (uint32_t)0U;

      MenuManager_StepWashNum_SeqIdx = enterDataHierachy->seqIdx;
      MenuManager_StepWashNum_StepIdx = enterDataHierachy->stepIdx;

      ProgramManager_NormStepConfig_WashNum_GetData((uint8_t)MenuManager_StepWashNum_SeqIdx, (uint8_t)MenuManager_StepWashNum_StepIdx, &tempWashNum);

      MenuManager_StepWashNum_ValueMin = (uint32_t)PROGRAMMANAGER_NORMSTEPCONFIG_WASHNUM_MIN;
      MenuManager_StepWashNum_ValueMax = (uint32_t)PROGRAMMANAGER_NORMSTEPCONFIG_WASHNUM_MAX;
      MenuManager_StepWashNum_Value = (uint32_t)tempWashNum;

      MenuManager_Common_DecToBcdConv
      (
        &(MenuManager_StepWashNum_Value),
        &(MenuManager_StepWashNum_UnitVal(0)),
        (uint8_t)MENUMANAGER_STEPWASHNUM_UNITVAL_LENGTH
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
    MenuManager_StepWashNum_LcdShowMainTitle();
    MenuManager_StepWashNum_LcdShowAdjust();

    MenuManager_SubMainFunction = MenuManager_StepWashNum_SubMainFunction;
    MenuManager_SubTickHandler = MenuManager_StepWashNum_SubTickHandler;

    return FSM_GUARD_TRUE;
  }

  return FSM_GUARD_FALSE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_StepWashNum_Exit(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  Fsm_DataHierachyStruct* dataHierachy;

  MenuManager_SubMainFunction = NULL;
  MenuManager_SubTickHandler = NULL;

  dataHierachy = (Fsm_DataHierachyStruct *)MenuManager_malloc(sizeof(Fsm_DataHierachyStruct));
  dataHierachy->dataId = MENUMANAGER_STATE_STEP_WASH_NUM;

  pFsmContext->dataHierachy = dataHierachy;

  /* Free internal data */
  MenuManager_InternalDataPop();
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_StepWashNum_StartBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_StepWashNum_InternalState == MENUMANAGER_STEPWASHNUM_INTERNALSTATE_READY)
  {
    MenuManager_StepWashNum_InternalState = MENUMANAGER_STEPWASHNUM_INTERNALSTATE_RUNNING;
  }
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_StepWashNum_StopBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_StepWashNum_InternalState != MENUMANAGER_STEPWASHNUM_INTERNALSTATE_READY)
  {
    return FSM_GUARD_FALSE;
  }
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_StepWashNum_UpBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  uint32_t tempUnitVal;
  uint32_t tempVal;

  if (MenuManager_StepWashNum_InternalState == MENUMANAGER_STEPWASHNUM_INTERNALSTATE_READY)
  {
    tempUnitVal = MenuManager_StepWashNum_UnitVal(MenuManager_StepWashNum_CurPos);

    MenuManager_StepWashNum_UnitVal(MenuManager_StepWashNum_CurPos) += (uint32_t)1U;

    if (MenuManager_StepWashNum_UnitVal(MenuManager_StepWashNum_CurPos) > (uint32_t)9U)
    {
      MenuManager_StepWashNum_UnitVal(MenuManager_StepWashNum_CurPos) = (uint32_t)0U;
    }

    MenuManager_Common_BcdToDecConv(&tempVal, &(MenuManager_StepWashNum_UnitVal(0)), (uint8_t)MENUMANAGER_STEPWASHNUM_UNITVAL_LENGTH);

    if ((tempVal <= MenuManager_StepWashNum_ValueMin) || (tempVal >= MenuManager_StepWashNum_ValueMax))
    {
      MenuManager_StepWashNum_UnitVal(MenuManager_StepWashNum_CurPos) = tempUnitVal;
    }

    MenuManager_StepWashNum_LcdShowAdjust();
  }

  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_StepWashNum_DownBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  uint32_t tempUnitVal;
  uint32_t tempVal;

  if (MenuManager_StepWashNum_InternalState == MENUMANAGER_STEPWASHNUM_INTERNALSTATE_READY)
  {
    tempUnitVal = MenuManager_StepWashNum_UnitVal(MenuManager_StepWashNum_CurPos);

    MenuManager_StepWashNum_UnitVal(MenuManager_StepWashNum_CurPos) -= (uint32_t)1U;

    if (MenuManager_StepWashNum_UnitVal(MenuManager_StepWashNum_CurPos) > (uint32_t)9U)
    {
      MenuManager_StepWashNum_UnitVal(MenuManager_StepWashNum_CurPos) = (uint32_t)9U;
    }

    MenuManager_Common_BcdToDecConv(&tempVal, &(MenuManager_StepWashNum_UnitVal(0)), (uint8_t)MENUMANAGER_STEPWASHNUM_UNITVAL_LENGTH);

    if ((tempVal <= MenuManager_StepWashNum_ValueMin) || (tempVal >= MenuManager_StepWashNum_ValueMax))
    {
      MenuManager_StepWashNum_UnitVal(MenuManager_StepWashNum_CurPos) = tempUnitVal;
    }

    MenuManager_StepWashNum_LcdShowAdjust();
  }
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_StepWashNum_AddBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_StepWashNum_InternalState == MENUMANAGER_STEPWASHNUM_INTERNALSTATE_READY)
  {
    if (MenuManager_StepWashNum_CurPos > (uint32_t)0U)
    {
      MenuManager_StepWashNum_CurPos -= (uint32_t)1U;
    }

    MenuManager_StepWashNum_LcdShowAdjust();
  }
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_StepWashNum_SubBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_StepWashNum_InternalState == MENUMANAGER_STEPWASHNUM_INTERNALSTATE_READY)
  {
    if (MenuManager_StepWashNum_CurPos < (MENUMANAGER_STEPWASHNUM_UNITVAL_LENGTH - (uint32_t)1U))
    {
      MenuManager_StepWashNum_CurPos += (uint32_t)1U;
    }
    
    MenuManager_StepWashNum_LcdShowAdjust();
  }
  
  return FSM_GUARD_TRUE;
}



/*=============================================================================================*/
static void MenuManager_StepWashNum_SubMainFunction(void)
{
  MenuManager_Common_ButEventMapHandler(&MenuManager_StepWashNum_ButEventMapConf);

  switch (MenuManager_StepWashNum_InternalState)
  {
    case MENUMANAGER_STEPWASHNUM_INTERNALSTATE_READY:
      break;
    case MENUMANAGER_STEPWASHNUM_INTERNALSTATE_RUNNING:
    {
      uint8_t tempWashNum;

      MenuManager_Common_BcdToDecConv(&(MenuManager_StepWashNum_Value), &(MenuManager_StepWashNum_UnitVal(0)), (uint8_t)MENUMANAGER_STEPWASHNUM_UNITVAL_LENGTH);

      tempWashNum = (uint8_t)MenuManager_StepWashNum_Value;

      ProgramManager_NormStepConfig_WashNum_SetData \
      ( \
        (uint8_t)MenuManager_StepWashNum_SeqIdx, \
        (uint8_t)MenuManager_StepWashNum_StepIdx, \
        &tempWashNum \
      );

      if ((uint8_t)MenuManager_StepWashNum_SeqIdx == ProgramManager_gAutoSeqConfig.sequenceIndex)
      {
        ProgramManager_NormStepConfig_WashNum_GetData \
        ( \
          (uint8_t)MenuManager_StepWashNum_SeqIdx, \
          (uint8_t)MenuManager_StepWashNum_StepIdx, \
          &((ProgramManager_gAutoSeqConfig.normStep)[MenuManager_StepWashNum_StepIdx].washNum) \
        );
      }

      MenuManager_StepWashNum_InternalState = MENUMANAGER_STEPWASHNUM_INTERNALSTATE_DONE;
      
      break;
    }
    case MENUMANAGER_STEPWASHNUM_INTERNALSTATE_DONE:
    {
      MenuManager_StepWashNum_LcdShowDone();

      MenuManager_StepWashNum_InternalState = MENUMANAGER_STEPWASHNUM_INTERNALSTATE_EXITING;

      break;
    }
    case MENUMANAGER_STEPWASHNUM_INTERNALSTATE_EXITING:
    default:
      break;
  }
}

/*=============================================================================================*/
static void MenuManager_StepWashNum_SubTickHandler(void)
{
  if (MenuManager_StepWashNum_InternalState == MENUMANAGER_STEPWASHNUM_INTERNALSTATE_EXITING)
  {
    MenuManager_StepWashNum_Counter += (uint32_t)1U;

    if (MenuManager_StepWashNum_Counter >= MENUMANAGER_STEPWASHNUM_COUNTER_MAX)
    {
      MenuManager_StepWashNum_Counter = (uint32_t)0U;
      
      Fsm_TriggerEvent(&MenuManager_FsmContext, (Fsm_EventType)MENUMANAGER_EVENT_PREV);
    }
  }
}



#ifdef  __cplusplus
}
#endif
