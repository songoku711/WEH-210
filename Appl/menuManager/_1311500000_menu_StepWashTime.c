/* 
 * File:   _1311500000_menu_StepWashTime.c
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
#define MENUMANAGER_STEPWASHTIME_INTERNALDATALENGTH                   (uint8_t)12U

#define MENUMANAGER_STEPWASHTIME_INTERNALSTATE_IDX                    0U
#define MENUMANAGER_STEPWASHTIME_COUNTER_IDX                          1U
#define MENUMANAGER_STEPWASHTIME_CURPOS_IDX                           2U
#define MENUMANAGER_STEPWASHTIME_SEQIDX_IDX                           3U
#define MENUMANAGER_STEPWASHTIME_STEPIDX_IDX                          4U
#define MENUMANAGER_STEPWASHTIME_VALUE_MIN_IDX                        5U
#define MENUMANAGER_STEPWASHTIME_VALUE_MAX_IDX                        6U
#define MENUMANAGER_STEPWASHTIME_VALUE_IDX                            7U
#define MENUMANAGER_STEPWASHTIME_UNITVAL_IDX                          8U

#define MenuManager_StepWashTime_InternalState                        MenuManager_GetInternalDataPtr(MENUMANAGER_STEPWASHTIME_INTERNALSTATE_IDX)
#define MenuManager_StepWashTime_Counter                              MenuManager_GetInternalDataPtr(MENUMANAGER_STEPWASHTIME_COUNTER_IDX)
#define MenuManager_StepWashTime_CurPos                               MenuManager_GetInternalDataPtr(MENUMANAGER_STEPWASHTIME_CURPOS_IDX)
#define MenuManager_StepWashTime_SeqIdx                               MenuManager_GetInternalDataPtr(MENUMANAGER_STEPWASHTIME_SEQIDX_IDX)
#define MenuManager_StepWashTime_StepIdx                              MenuManager_GetInternalDataPtr(MENUMANAGER_STEPWASHTIME_STEPIDX_IDX)
#define MenuManager_StepWashTime_ValueMin                             MenuManager_GetInternalDataPtr(MENUMANAGER_STEPWASHTIME_VALUE_MIN_IDX)
#define MenuManager_StepWashTime_ValueMax                             MenuManager_GetInternalDataPtr(MENUMANAGER_STEPWASHTIME_VALUE_MAX_IDX)
#define MenuManager_StepWashTime_Value                                MenuManager_GetInternalDataPtr(MENUMANAGER_STEPWASHTIME_VALUE_IDX)
#define MenuManager_StepWashTime_UnitVal(idx)                         MenuManager_GetInternalDataPtr(MENUMANAGER_STEPWASHTIME_UNITVAL_IDX + idx)



#define MENUMANAGER_STEPWASHTIME_INTERNALSTATE_READY                  (uint32_t)0U
#define MENUMANAGER_STEPWASHTIME_INTERNALSTATE_RUNNING                (uint32_t)1U
#define MENUMANAGER_STEPWASHTIME_INTERNALSTATE_DONE                   (uint32_t)2U
#define MENUMANAGER_STEPWASHTIME_INTERNALSTATE_EXITING                (uint32_t)3U

#define MENUMANAGER_STEPWASHTIME_COUNTER_MAX                          (uint32_t)50U

#define MENUMANAGER_STEPWASHTIME_CURPOS_XPOS                          (uint32_t)5U
#define MENUMANAGER_STEPWASHTIME_CURPOS_YPOS                          (uint32_t)5U

#define MENUMANAGER_STEPWASHTIME_UNIT_XPOS                            (uint32_t)10U
#define MENUMANAGER_STEPWASHTIME_UNIT_YPOS                            (uint32_t)5U

#define MENUMANAGER_STEPWASHTIME_UNITVAL_LENGTH                       (uint32_t)4U



/** Menu manager main titles and child menu titles */
static const uint8_t MenuManager_StepWashTime_MainTitleStr[] =        "WASH TIME";



/** Menu manager button event mapping array */
static MenuManager_ButEventMapStruct MenuManager_StepWashTime_ButEventMap[6] =
{
  { IOMANAGER_BUTTON_STATE_START,                                     MENUMANAGER_EVENT_START_BUT             },
  { IOMANAGER_BUTTON_STATE_STOP,                                      MENUMANAGER_EVENT_STOP_BUT              },
  { IOMANAGER_BUTTON_STATE_UP,                                        MENUMANAGER_EVENT_UP_BUT                },
  { IOMANAGER_BUTTON_STATE_DOWN,                                      MENUMANAGER_EVENT_DOWN_BUT              },
  { IOMANAGER_BUTTON_STATE_ADD,                                       MENUMANAGER_EVENT_ADD_BUT               },
  { IOMANAGER_BUTTON_STATE_SUB,                                       MENUMANAGER_EVENT_SUB_BUT               }
};

/** Menu manager button event mapping configuration */
static MenuManager_ButEventMapConfStruct MenuManager_StepWashTime_ButEventMapConf =
{
  .butEventMapNum             = (uint8_t)6U,
  .butEventMapCfg             = &MenuManager_StepWashTime_ButEventMap
};



/** Menu manager event handlers */
static Fsm_GuardType MenuManager_StepWashTime_Entry                   (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_StepWashTime_Exit                    (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_StepWashTime_StartBut                (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_StepWashTime_StopBut                 (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_StepWashTime_UpBut                   (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_StepWashTime_DownBut                 (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_StepWashTime_AddBut                  (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_StepWashTime_SubBut                  (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);

/** Menu manager state machine */
Fsm_EventEntryStruct MenuManager_StepWashTime_StateMachine[9] =
{
  FSM_TRIGGER_ENTRY             (                                     MenuManager_StepWashTime_Entry                                                  ),
  FSM_TRIGGER_EXIT              (                                     MenuManager_StepWashTime_Exit                                                   ),
  FSM_TRIGGER_TRANSITION        ( MENUMANAGER_EVENT_PREV,                                                     MENUMANAGER_STATE_STEP_NORM_SETUP       ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_START_BUT,        MenuManager_StepWashTime_StartBut                                               ),
  FSM_TRIGGER_TRANSITION_ACTION ( MENUMANAGER_EVENT_STOP_BUT,         MenuManager_StepWashTime_StopBut,       MENUMANAGER_STATE_STEP_NORM_SETUP       ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_UP_BUT,           MenuManager_StepWashTime_UpBut                                                  ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_DOWN_BUT,         MenuManager_StepWashTime_DownBut                                                ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_ADD_BUT,          MenuManager_StepWashTime_AddBut                                                 ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_SUB_BUT,          MenuManager_StepWashTime_SubBut                                                 )
};



/*===============================================================================================
*                                   LOCAL FUNCTION PROTOTYPES
===============================================================================================*/

static void MenuManager_StepWashTime_LcdShowMainTitle(void);
static void MenuManager_StepWashTime_LcdShowAdjust(void);
static void MenuManager_StepWashTime_LcdShowDone(void);

static void MenuManager_StepWashTime_SubMainFunction(void);
static void MenuManager_StepWashTime_SubTickHandler(void);



/*===============================================================================================
*                                       LOCAL FUNCTIONS
===============================================================================================*/

static void MenuManager_StepWashTime_LcdShowMainTitle(void)
{
  /* Print main title */
  MenuManager_Common_LcdClearAdjustMenuStatic();
  MenuManager_Common_LcdShowMainTitle(MenuManager_StepWashTime_MainTitleStr);
  MenuManager_Common_LcdShowOkExitChoice();
  MenuManager_Common_LcdUpdateAdjustMenuStatic();
}

/*=============================================================================================*/
static void MenuManager_StepWashTime_LcdShowAdjust(void)
{
  MenuManager_Common_LcdClearAdjustMenuDynamic();

  MenuManager_Common_LcdShowAdjustUnitVal
  (
    MENUMANAGER_STEPWASHTIME_CURPOS_XPOS,
    MENUMANAGER_STEPWASHTIME_CURPOS_YPOS,
    &(MenuManager_StepWashTime_UnitVal(0)),
    MENUMANAGER_STEPWASHTIME_UNITVAL_LENGTH
  );
  
  MenuManager_Common_LcdShowAdjustArrow
  (
    MENUMANAGER_STEPWASHTIME_CURPOS_XPOS,
    MENUMANAGER_STEPWASHTIME_CURPOS_YPOS,
    MenuManager_StepWashTime_CurPos
  );

  LCD_SetCursorPos(MENUMANAGER_STEPWASHTIME_UNIT_XPOS, MENUMANAGER_STEPWASHTIME_UNIT_YPOS, LCD_CURSOR_BY_FONT);
  LCD_PutString((uint8_t *)MenuManager_Common_SecondStr);

  MenuManager_Common_LcdUpdateAdjustMenuDynamic();
}

/*=============================================================================================*/
static void MenuManager_StepWashTime_LcdShowDone(void)
{
  MenuManager_Common_LcdClearAll();

  MenuManager_Common_LcdShowAdjustUnitVal
  (
    MENUMANAGER_STEPWASHTIME_CURPOS_XPOS,
    MENUMANAGER_STEPWASHTIME_CURPOS_YPOS,
    &(MenuManager_StepWashTime_UnitVal(0)),
    MENUMANAGER_STEPWASHTIME_UNITVAL_LENGTH
  );

  LCD_SetCursorPos(MENUMANAGER_STEPWASHTIME_UNIT_XPOS, MENUMANAGER_STEPWASHTIME_UNIT_YPOS, LCD_CURSOR_BY_FONT);
  LCD_PutString((uint8_t *)MenuManager_Common_SecondStr);

  MenuManager_Common_LcdUpdateAll();
}



/*=============================================================================================*/
static Fsm_GuardType MenuManager_StepWashTime_Entry(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  MenuManager_Common_ProgramSetupStruct* enterDataHierachy;
  uint16_t tempWashTime;
  HAL_StatusTypeDef retVal = HAL_OK;

  /* Check if previous state data hierachy is not empty */
  if (pFsmContext->dataHierachy != NULL)
  {
    if (pFsmContext->dataHierachy->dataId == MENUMANAGER_STATE_STEP_NORM_SETUP)
    {
      enterDataHierachy = (MenuManager_Common_ProgramSetupStruct *)(pFsmContext->dataHierachy);

      MenuManager_InternalDataPush(MENUMANAGER_STEPWASHTIME_INTERNALDATALENGTH);

      MenuManager_StepWashTime_InternalState = MENUMANAGER_STEPWASHTIME_INTERNALSTATE_READY;
      MenuManager_StepWashTime_Counter = (uint32_t)0U;
      MenuManager_StepWashTime_CurPos = (uint32_t)0U;

      MenuManager_StepWashTime_SeqIdx = enterDataHierachy->seqIdx;
      MenuManager_StepWashTime_StepIdx = enterDataHierachy->stepIdx;

      ProgramManager_NormStepConfig_WashTime_GetData((uint8_t)MenuManager_StepWashTime_SeqIdx, (uint8_t)MenuManager_StepWashTime_StepIdx, &tempWashTime);

      MenuManager_StepWashTime_ValueMin = (uint32_t)PROGRAMMANAGER_NORMSTEPCONFIG_WASHTIME_MIN;
      MenuManager_StepWashTime_ValueMax = (uint32_t)PROGRAMMANAGER_NORMSTEPCONFIG_WASHTIME_MAX;
      MenuManager_StepWashTime_Value = (uint32_t)tempWashTime;

      MenuManager_Common_DecToBcdConv
      (
        &(MenuManager_StepWashTime_Value),
        &(MenuManager_StepWashTime_UnitVal(0)),
        (uint8_t)MENUMANAGER_STEPWASHTIME_UNITVAL_LENGTH
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
    MenuManager_StepWashTime_LcdShowMainTitle();
    MenuManager_StepWashTime_LcdShowAdjust();

    MenuManager_SubMainFunction = MenuManager_StepWashTime_SubMainFunction;
    MenuManager_SubTickHandler = MenuManager_StepWashTime_SubTickHandler;

    return FSM_GUARD_TRUE;
  }

  return FSM_GUARD_FALSE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_StepWashTime_Exit(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  Fsm_DataHierachyStruct* dataHierachy;

  MenuManager_SubMainFunction = NULL;
  MenuManager_SubTickHandler = NULL;

  dataHierachy = (Fsm_DataHierachyStruct *)MenuManager_malloc(sizeof(Fsm_DataHierachyStruct));
  dataHierachy->dataId = MENUMANAGER_STATE_STEP_WASH_TIME;

  pFsmContext->dataHierachy = dataHierachy;

  /* Free internal data */
  MenuManager_InternalDataPop();
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_StepWashTime_StartBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_StepWashTime_InternalState == MENUMANAGER_STEPWASHTIME_INTERNALSTATE_READY)
  {
    MenuManager_StepWashTime_InternalState = MENUMANAGER_STEPWASHTIME_INTERNALSTATE_RUNNING;
  }
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_StepWashTime_StopBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_StepWashTime_InternalState != MENUMANAGER_STEPWASHTIME_INTERNALSTATE_READY)
  {
    return FSM_GUARD_FALSE;
  }
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_StepWashTime_UpBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  uint32_t tempUnitVal;
  uint32_t tempVal;

  if (MenuManager_StepWashTime_InternalState == MENUMANAGER_STEPWASHTIME_INTERNALSTATE_READY)
  {
    tempUnitVal = MenuManager_StepWashTime_UnitVal(MenuManager_StepWashTime_CurPos);

    MenuManager_StepWashTime_UnitVal(MenuManager_StepWashTime_CurPos) += (uint32_t)1U;

    if (MenuManager_StepWashTime_UnitVal(MenuManager_StepWashTime_CurPos) > (uint32_t)9U)
    {
      MenuManager_StepWashTime_UnitVal(MenuManager_StepWashTime_CurPos) = (uint32_t)0U;
    }

    MenuManager_Common_BcdToDecConv(&tempVal, &(MenuManager_StepWashTime_UnitVal(0)), (uint8_t)MENUMANAGER_STEPWASHTIME_UNITVAL_LENGTH);

    if ((tempVal <= MenuManager_StepWashTime_ValueMin) || (tempVal >= MenuManager_StepWashTime_ValueMax))
    {
      MenuManager_StepWashTime_UnitVal(MenuManager_StepWashTime_CurPos) = tempUnitVal;
    }

    MenuManager_StepWashTime_LcdShowAdjust();
  }

  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_StepWashTime_DownBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  uint32_t tempUnitVal;
  uint32_t tempVal;

  if (MenuManager_StepWashTime_InternalState == MENUMANAGER_STEPWASHTIME_INTERNALSTATE_READY)
  {
    tempUnitVal = MenuManager_StepWashTime_UnitVal(MenuManager_StepWashTime_CurPos);

    MenuManager_StepWashTime_UnitVal(MenuManager_StepWashTime_CurPos) -= (uint32_t)1U;

    if (MenuManager_StepWashTime_UnitVal(MenuManager_StepWashTime_CurPos) > (uint32_t)9U)
    {
      MenuManager_StepWashTime_UnitVal(MenuManager_StepWashTime_CurPos) = (uint32_t)9U;
    }

    MenuManager_Common_BcdToDecConv(&tempVal, &(MenuManager_StepWashTime_UnitVal(0)), (uint8_t)MENUMANAGER_STEPWASHTIME_UNITVAL_LENGTH);

    if ((tempVal <= MenuManager_StepWashTime_ValueMin) || (tempVal >= MenuManager_StepWashTime_ValueMax))
    {
      MenuManager_StepWashTime_UnitVal(MenuManager_StepWashTime_CurPos) = tempUnitVal;
    }

    MenuManager_StepWashTime_LcdShowAdjust();
  }
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_StepWashTime_AddBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_StepWashTime_InternalState == MENUMANAGER_STEPWASHTIME_INTERNALSTATE_READY)
  {
    if (MenuManager_StepWashTime_CurPos > (uint32_t)0U)
    {
      MenuManager_StepWashTime_CurPos -= (uint32_t)1U;
    }

    MenuManager_StepWashTime_LcdShowAdjust();
  }
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_StepWashTime_SubBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_StepWashTime_InternalState == MENUMANAGER_STEPWASHTIME_INTERNALSTATE_READY)
  {
    if (MenuManager_StepWashTime_CurPos < (MENUMANAGER_STEPWASHTIME_UNITVAL_LENGTH - (uint32_t)1U))
    {
      MenuManager_StepWashTime_CurPos += (uint32_t)1U;
    }
    
    MenuManager_StepWashTime_LcdShowAdjust();
  }
  
  return FSM_GUARD_TRUE;
}



/*=============================================================================================*/
static void MenuManager_StepWashTime_SubMainFunction(void)
{
  MenuManager_Common_ButEventMapHandler(&MenuManager_StepWashTime_ButEventMapConf);

  switch (MenuManager_StepWashTime_InternalState)
  {
    case MENUMANAGER_STEPWASHTIME_INTERNALSTATE_READY:
      break;
    case MENUMANAGER_STEPWASHTIME_INTERNALSTATE_RUNNING:
    {
      uint16_t tempWashNum;

      MenuManager_Common_BcdToDecConv(&(MenuManager_StepWashTime_Value), &(MenuManager_StepWashTime_UnitVal(0)), (uint8_t)MENUMANAGER_STEPWASHTIME_UNITVAL_LENGTH);

      tempWashNum = (uint16_t)MenuManager_StepWashTime_Value;

      ProgramManager_NormStepConfig_WashTime_SetData \
      ( \
        (uint8_t)MenuManager_StepWashTime_SeqIdx, \
        (uint8_t)MenuManager_StepWashTime_StepIdx, \
        &tempWashNum \
      );

      if ((uint8_t)MenuManager_StepWashTime_SeqIdx == ProgramManager_gAutoSeqConfig.sequenceIndex)
      {
        ProgramManager_NormStepConfig_WashTime_GetData \
        ( \
          (uint8_t)MenuManager_StepWashTime_SeqIdx, \
          (uint8_t)MenuManager_StepWashTime_StepIdx, \
          &((ProgramManager_gAutoSeqConfig.normStep)[MenuManager_StepWashTime_StepIdx].washTime) \
        );
      }

      MenuManager_StepWashTime_InternalState = MENUMANAGER_STEPWASHTIME_INTERNALSTATE_DONE;
      
      break;
    }
    case MENUMANAGER_STEPWASHTIME_INTERNALSTATE_DONE:
    {
      MenuManager_StepWashTime_LcdShowDone();

      MenuManager_StepWashTime_InternalState = MENUMANAGER_STEPWASHTIME_INTERNALSTATE_EXITING;

      break;
    }
    case MENUMANAGER_STEPWASHTIME_INTERNALSTATE_EXITING:
    default:
      break;
  }
}

/*=============================================================================================*/
static void MenuManager_StepWashTime_SubTickHandler(void)
{
  if (MenuManager_StepWashTime_InternalState == MENUMANAGER_STEPWASHTIME_INTERNALSTATE_EXITING)
  {
    MenuManager_StepWashTime_Counter += (uint32_t)1U;

    if (MenuManager_StepWashTime_Counter >= MENUMANAGER_STEPWASHTIME_COUNTER_MAX)
    {
      MenuManager_StepWashTime_Counter = (uint32_t)0U;
      
      Fsm_TriggerEvent(&MenuManager_FsmContext, (Fsm_EventType)MENUMANAGER_EVENT_PREV);
    }
  }
}



#ifdef  __cplusplus
}
#endif
