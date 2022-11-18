/* 
 * File:   _1312400000_menu_StepHighExtractTime2.c
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
#define MENUMANAGER_STEPHIGHEXTRACTTIME2_INTERNALDATALENGTH           (uint8_t)10U

#define MENUMANAGER_STEPHIGHEXTRACTTIME2_INTERNALSTATE_IDX            0U
#define MENUMANAGER_STEPHIGHEXTRACTTIME2_COUNTER_IDX                  1U
#define MENUMANAGER_STEPHIGHEXTRACTTIME2_CURPOS_IDX                   2U
#define MENUMANAGER_STEPHIGHEXTRACTTIME2_SEQIDX_IDX                   3U
#define MENUMANAGER_STEPHIGHEXTRACTTIME2_VALUE_MIN_IDX                4U
#define MENUMANAGER_STEPHIGHEXTRACTTIME2_VALUE_MAX_IDX                5U
#define MENUMANAGER_STEPHIGHEXTRACTTIME2_VALUE_IDX                    6U
#define MENUMANAGER_STEPHIGHEXTRACTTIME2_UNITVAL_IDX                  7U

#define MenuManager_StepHighExtractTime2_InternalState                MenuManager_GetInternalDataPtr(MENUMANAGER_STEPHIGHEXTRACTTIME2_INTERNALSTATE_IDX)
#define MenuManager_StepHighExtractTime2_Counter                      MenuManager_GetInternalDataPtr(MENUMANAGER_STEPHIGHEXTRACTTIME2_COUNTER_IDX)
#define MenuManager_StepHighExtractTime2_CurPos                       MenuManager_GetInternalDataPtr(MENUMANAGER_STEPHIGHEXTRACTTIME2_CURPOS_IDX)
#define MenuManager_StepHighExtractTime2_SeqIdx                       MenuManager_GetInternalDataPtr(MENUMANAGER_STEPHIGHEXTRACTTIME2_SEQIDX_IDX)
#define MenuManager_StepHighExtractTime2_ValueMin                     MenuManager_GetInternalDataPtr(MENUMANAGER_STEPHIGHEXTRACTTIME2_VALUE_MIN_IDX)
#define MenuManager_StepHighExtractTime2_ValueMax                     MenuManager_GetInternalDataPtr(MENUMANAGER_STEPHIGHEXTRACTTIME2_VALUE_MAX_IDX)
#define MenuManager_StepHighExtractTime2_Value                        MenuManager_GetInternalDataPtr(MENUMANAGER_STEPHIGHEXTRACTTIME2_VALUE_IDX)
#define MenuManager_StepHighExtractTime2_UnitVal(idx)                 MenuManager_GetInternalDataPtr(MENUMANAGER_STEPHIGHEXTRACTTIME2_UNITVAL_IDX + idx)



#define MENUMANAGER_STEPHIGHEXTRACTTIME2_INTERNALSTATE_READY          (uint32_t)0U
#define MENUMANAGER_STEPHIGHEXTRACTTIME2_INTERNALSTATE_RUNNING        (uint32_t)1U
#define MENUMANAGER_STEPHIGHEXTRACTTIME2_INTERNALSTATE_DONE           (uint32_t)2U
#define MENUMANAGER_STEPHIGHEXTRACTTIME2_INTERNALSTATE_EXITING        (uint32_t)3U

#define MENUMANAGER_STEPHIGHEXTRACTTIME2_COUNTER_MAX                  (uint32_t)50U

#define MENUMANAGER_STEPHIGHEXTRACTTIME2_CURPOS_XPOS                  (uint32_t)5U
#define MENUMANAGER_STEPHIGHEXTRACTTIME2_CURPOS_YPOS                  (uint32_t)5U

#define MENUMANAGER_STEPHIGHEXTRACTTIME2_UNIT_XPOS                    (uint32_t)9U
#define MENUMANAGER_STEPHIGHEXTRACTTIME2_UNIT_YPOS                    (uint32_t)5U

#define MENUMANAGER_STEPHIGHEXTRACTTIME2_UNITVAL_LENGTH               (uint32_t)3U



/** Menu manager main titles and child menu titles */
static const uint8_t MenuManager_StepHighExtractTime2_MainTitleStr[] =  "USER HIGH EXT T2";



/** Menu manager button event mapping array */
static MenuManager_ButEventMapStruct MenuManager_StepHighExtractTime2_ButEventMap[6] =
{
  { IOMANAGER_BUTTON_STATE_START,                                     MENUMANAGER_EVENT_START_BUT             },
  { IOMANAGER_BUTTON_STATE_STOP,                                      MENUMANAGER_EVENT_STOP_BUT              },
  { IOMANAGER_BUTTON_STATE_UP,                                        MENUMANAGER_EVENT_UP_BUT                },
  { IOMANAGER_BUTTON_STATE_DOWN,                                      MENUMANAGER_EVENT_DOWN_BUT              },
  { IOMANAGER_BUTTON_STATE_ADD,                                       MENUMANAGER_EVENT_ADD_BUT               },
  { IOMANAGER_BUTTON_STATE_SUB,                                       MENUMANAGER_EVENT_SUB_BUT               }
};

/** Menu manager button event mapping configuration */
static MenuManager_ButEventMapConfStruct MenuManager_StepHighExtractTime2_ButEventMapConf =
{
  .butEventMapNum             = (uint8_t)6U,
  .butEventMapCfg             = &MenuManager_StepHighExtractTime2_ButEventMap
};



/** Menu manager event handlers */
static Fsm_GuardType MenuManager_StepHighExtractTime2_Entry           (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_StepHighExtractTime2_Exit            (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_StepHighExtractTime2_StartBut        (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_StepHighExtractTime2_StopBut         (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_StepHighExtractTime2_UpBut           (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_StepHighExtractTime2_DownBut         (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_StepHighExtractTime2_AddBut          (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_StepHighExtractTime2_SubBut          (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);

/** Menu manager state machine */
Fsm_EventEntryStruct MenuManager_StepHighExtractTime2_StateMachine[9] =
{
  FSM_TRIGGER_ENTRY             (                                     MenuManager_StepHighExtractTime2_Entry                                          ),
  FSM_TRIGGER_EXIT              (                                     MenuManager_StepHighExtractTime2_Exit                                           ),
  FSM_TRIGGER_TRANSITION        ( MENUMANAGER_EVENT_PREV,                                                       MENUMANAGER_STATE_STEP_EXTRACT_PARAM  ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_START_BUT,        MenuManager_StepHighExtractTime2_StartBut                                       ),
  FSM_TRIGGER_TRANSITION_ACTION ( MENUMANAGER_EVENT_STOP_BUT,         MenuManager_StepHighExtractTime2_StopBut, MENUMANAGER_STATE_STEP_EXTRACT_PARAM  ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_UP_BUT,           MenuManager_StepHighExtractTime2_UpBut                                          ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_DOWN_BUT,         MenuManager_StepHighExtractTime2_DownBut                                        ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_ADD_BUT,          MenuManager_StepHighExtractTime2_AddBut                                         ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_SUB_BUT,          MenuManager_StepHighExtractTime2_SubBut                                         )
};



/*===============================================================================================
*                                   LOCAL FUNCTION PROTOTYPES
===============================================================================================*/

static void MenuManager_StepHighExtractTime2_LcdShowMainTitle(void);
static void MenuManager_StepHighExtractTime2_LcdShowAdjust(void);
static void MenuManager_StepHighExtractTime2_LcdShowDone(void);

static void MenuManager_StepHighExtractTime2_SubMainFunction(void);
static void MenuManager_StepHighExtractTime2_SubTickHandler(void);



/*===============================================================================================
*                                       LOCAL FUNCTIONS
===============================================================================================*/

static void MenuManager_StepHighExtractTime2_LcdShowMainTitle(void)
{
  /* Print main title */
  MenuManager_Common_LcdClearAdjustMenuStatic();
  MenuManager_Common_LcdShowMainTitle(MenuManager_StepHighExtractTime2_MainTitleStr);
  MenuManager_Common_LcdShowOkExitChoice();
  MenuManager_Common_LcdUpdateAdjustMenuStatic();
}

/*=============================================================================================*/
static void MenuManager_StepHighExtractTime2_LcdShowAdjust(void)
{
  MenuManager_Common_LcdClearAdjustMenuDynamic();

  MenuManager_Common_LcdShowAdjustUnitVal
  (
    MENUMANAGER_STEPHIGHEXTRACTTIME2_CURPOS_XPOS,
    MENUMANAGER_STEPHIGHEXTRACTTIME2_CURPOS_YPOS,
    &(MenuManager_StepHighExtractTime2_UnitVal(0)),
    MENUMANAGER_STEPHIGHEXTRACTTIME2_UNITVAL_LENGTH
  );
  
  MenuManager_Common_LcdShowAdjustArrow
  (
    MENUMANAGER_STEPHIGHEXTRACTTIME2_CURPOS_XPOS,
    MENUMANAGER_STEPHIGHEXTRACTTIME2_CURPOS_YPOS,
    MenuManager_StepHighExtractTime2_CurPos
  );

  LCD_SetCursorPos(MENUMANAGER_STEPHIGHEXTRACTTIME2_UNIT_XPOS, MENUMANAGER_STEPHIGHEXTRACTTIME2_UNIT_YPOS, LCD_CURSOR_BY_FONT);
  LCD_PutString((uint8_t *)MenuManager_Common_SecondStr);

  MenuManager_Common_LcdUpdateAdjustMenuDynamic();
}

/*=============================================================================================*/
static void MenuManager_StepHighExtractTime2_LcdShowDone(void)
{
  MenuManager_Common_LcdClearAll();

  MenuManager_Common_LcdShowAdjustUnitVal
  (
    MENUMANAGER_STEPHIGHEXTRACTTIME2_CURPOS_XPOS,
    MENUMANAGER_STEPHIGHEXTRACTTIME2_CURPOS_YPOS,
    &(MenuManager_StepHighExtractTime2_UnitVal(0)),
    MENUMANAGER_STEPHIGHEXTRACTTIME2_UNITVAL_LENGTH
  );

  LCD_SetCursorPos(MENUMANAGER_STEPHIGHEXTRACTTIME2_UNIT_XPOS, MENUMANAGER_STEPHIGHEXTRACTTIME2_UNIT_YPOS, LCD_CURSOR_BY_FONT);
  LCD_PutString((uint8_t *)MenuManager_Common_SecondStr);

  MenuManager_Common_LcdUpdateAll();
}



/*=============================================================================================*/
static Fsm_GuardType MenuManager_StepHighExtractTime2_Entry(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  MenuManager_Common_ProgramSetupStruct* enterDataHierachy;
  uint16_t tempHighExtractTime2;
  HAL_StatusTypeDef retVal = HAL_OK;

  /* Check if previous state data hierachy is not empty */
  if (pFsmContext->dataHierachy != NULL)
  {
    if (pFsmContext->dataHierachy->dataId == MENUMANAGER_STATE_STEP_EXTRACT_PARAM)
    {
      enterDataHierachy = (MenuManager_Common_ProgramSetupStruct *)(pFsmContext->dataHierachy);

      MenuManager_InternalDataPush(MENUMANAGER_STEPHIGHEXTRACTTIME2_INTERNALDATALENGTH);

      MenuManager_StepHighExtractTime2_InternalState = MENUMANAGER_STEPHIGHEXTRACTTIME2_INTERNALSTATE_READY;
      MenuManager_StepHighExtractTime2_Counter = (uint32_t)0U;
      MenuManager_StepHighExtractTime2_CurPos = (uint32_t)0U;

      MenuManager_StepHighExtractTime2_SeqIdx = enterDataHierachy->seqIdx;

      ProgramManager_ExtractStepConfig_HighExtractTime2_GetData
      (
        (uint8_t)MenuManager_StepHighExtractTime2_SeqIdx,
        &tempHighExtractTime2
      );

      MenuManager_StepHighExtractTime2_ValueMin = (uint32_t)PROGRAMMANAGER_EXTRACTSETUP_COMMON_TIME_MIN;
      MenuManager_StepHighExtractTime2_ValueMax = (uint32_t)PROGRAMMANAGER_EXTRACTSETUP_COMMON_TIME_MAX;

      MenuManager_StepHighExtractTime2_Value = (uint32_t)tempHighExtractTime2;

      MenuManager_Common_DecToBcdConv
      (
        &(MenuManager_StepHighExtractTime2_Value),
        &(MenuManager_StepHighExtractTime2_UnitVal(0)),
        (uint8_t)MENUMANAGER_STEPHIGHEXTRACTTIME2_UNITVAL_LENGTH
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
    MenuManager_StepHighExtractTime2_LcdShowMainTitle();
    MenuManager_StepHighExtractTime2_LcdShowAdjust();

    MenuManager_SubMainFunction = MenuManager_StepHighExtractTime2_SubMainFunction;
    MenuManager_SubTickHandler = MenuManager_StepHighExtractTime2_SubTickHandler;

    return FSM_GUARD_TRUE;
  }

  return FSM_GUARD_FALSE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_StepHighExtractTime2_Exit(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  Fsm_DataHierachyStruct* dataHierachy;

  MenuManager_SubMainFunction = NULL;
  MenuManager_SubTickHandler = NULL;

  dataHierachy = (Fsm_DataHierachyStruct *)MenuManager_malloc(sizeof(Fsm_DataHierachyStruct));
  dataHierachy->dataId = MENUMANAGER_STATE_STEP_HIGH_EXTRACT_TIME_2;

  pFsmContext->dataHierachy = dataHierachy;

  /* Free internal data */
  MenuManager_InternalDataPop();
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_StepHighExtractTime2_StartBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_StepHighExtractTime2_InternalState == MENUMANAGER_STEPHIGHEXTRACTTIME2_INTERNALSTATE_READY)
  {
    MenuManager_StepHighExtractTime2_InternalState = MENUMANAGER_STEPHIGHEXTRACTTIME2_INTERNALSTATE_RUNNING;
  }
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_StepHighExtractTime2_StopBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_StepHighExtractTime2_InternalState != MENUMANAGER_STEPHIGHEXTRACTTIME2_INTERNALSTATE_READY)
  {
    return FSM_GUARD_FALSE;
  }
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_StepHighExtractTime2_UpBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  uint32_t tempUnitVal;
  uint32_t tempVal;

  if (MenuManager_StepHighExtractTime2_InternalState == MENUMANAGER_STEPHIGHEXTRACTTIME2_INTERNALSTATE_READY)
  {
    tempUnitVal = MenuManager_StepHighExtractTime2_UnitVal(MenuManager_StepHighExtractTime2_CurPos);

    MenuManager_StepHighExtractTime2_UnitVal(MenuManager_StepHighExtractTime2_CurPos) += (uint32_t)1U;

    if (MenuManager_StepHighExtractTime2_UnitVal(MenuManager_StepHighExtractTime2_CurPos) > (uint32_t)9U)
    {
      MenuManager_StepHighExtractTime2_UnitVal(MenuManager_StepHighExtractTime2_CurPos) = (uint32_t)0U;
    }

    MenuManager_Common_BcdToDecConv(&tempVal, &(MenuManager_StepHighExtractTime2_UnitVal(0)), (uint8_t)MENUMANAGER_STEPHIGHEXTRACTTIME2_UNITVAL_LENGTH);

    if ((tempVal <= MenuManager_StepHighExtractTime2_ValueMin) || (tempVal >= MenuManager_StepHighExtractTime2_ValueMax))
    {
      MenuManager_StepHighExtractTime2_UnitVal(MenuManager_StepHighExtractTime2_CurPos) = tempUnitVal;
    }

    MenuManager_StepHighExtractTime2_LcdShowAdjust();
  }

  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_StepHighExtractTime2_DownBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  uint32_t tempUnitVal;
  uint32_t tempVal;

  if (MenuManager_StepHighExtractTime2_InternalState == MENUMANAGER_STEPHIGHEXTRACTTIME2_INTERNALSTATE_READY)
  {
    tempUnitVal = MenuManager_StepHighExtractTime2_UnitVal(MenuManager_StepHighExtractTime2_CurPos);

    MenuManager_StepHighExtractTime2_UnitVal(MenuManager_StepHighExtractTime2_CurPos) -= (uint32_t)1U;

    if (MenuManager_StepHighExtractTime2_UnitVal(MenuManager_StepHighExtractTime2_CurPos) > (uint32_t)9U)
    {
      MenuManager_StepHighExtractTime2_UnitVal(MenuManager_StepHighExtractTime2_CurPos) = (uint32_t)9U;
    }

    MenuManager_Common_BcdToDecConv(&tempVal, &(MenuManager_StepHighExtractTime2_UnitVal(0)), (uint8_t)MENUMANAGER_STEPHIGHEXTRACTTIME2_UNITVAL_LENGTH);

    if ((tempVal <= MenuManager_StepHighExtractTime2_ValueMin) || (tempVal >= MenuManager_StepHighExtractTime2_ValueMax))
    {
      MenuManager_StepHighExtractTime2_UnitVal(MenuManager_StepHighExtractTime2_CurPos) = tempUnitVal;
    }

    MenuManager_StepHighExtractTime2_LcdShowAdjust();
  }
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_StepHighExtractTime2_AddBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_StepHighExtractTime2_InternalState == MENUMANAGER_STEPHIGHEXTRACTTIME2_INTERNALSTATE_READY)
  {
    if (MenuManager_StepHighExtractTime2_CurPos > (uint32_t)0U)
    {
      MenuManager_StepHighExtractTime2_CurPos -= (uint32_t)1U;
    }

    MenuManager_StepHighExtractTime2_LcdShowAdjust();
  }
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_StepHighExtractTime2_SubBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_StepHighExtractTime2_InternalState == MENUMANAGER_STEPHIGHEXTRACTTIME2_INTERNALSTATE_READY)
  {
    if (MenuManager_StepHighExtractTime2_CurPos < (MENUMANAGER_STEPHIGHEXTRACTTIME2_UNITVAL_LENGTH - (uint32_t)1U))
    {
      MenuManager_StepHighExtractTime2_CurPos += (uint32_t)1U;
    }
    
    MenuManager_StepHighExtractTime2_LcdShowAdjust();
  }
  
  return FSM_GUARD_TRUE;
}



/*=============================================================================================*/
static void MenuManager_StepHighExtractTime2_SubMainFunction(void)
{
  MenuManager_Common_ButEventMapHandler(&MenuManager_StepHighExtractTime2_ButEventMapConf);

  switch (MenuManager_StepHighExtractTime2_InternalState)
  {
    case MENUMANAGER_STEPHIGHEXTRACTTIME2_INTERNALSTATE_READY:
      break;
    case MENUMANAGER_STEPHIGHEXTRACTTIME2_INTERNALSTATE_RUNNING:
    {
      uint16_t tempHighExtractTime2;

      MenuManager_Common_BcdToDecConv(&(MenuManager_StepHighExtractTime2_Value), &(MenuManager_StepHighExtractTime2_UnitVal(0)), (uint8_t)MENUMANAGER_STEPHIGHEXTRACTTIME2_UNITVAL_LENGTH);

      tempHighExtractTime2 = (uint16_t)MenuManager_StepHighExtractTime2_Value;

      ProgramManager_ExtractStepConfig_HighExtractTime2_SetData \
      ( \
        (uint8_t)MenuManager_StepHighExtractTime2_SeqIdx, \
        &tempHighExtractTime2 \
      );

      if ((uint8_t)MenuManager_StepHighExtractTime2_SeqIdx == ProgramManager_gAutoSeqConfig.sequenceIndex)
      {
        ProgramManager_ExtractStepConfig_HighExtractTime2_GetData \
        ( \
          (uint8_t)MenuManager_StepHighExtractTime2_SeqIdx, \
          &(ProgramManager_gAutoSeqConfig.extractStep.highExtractTime2) \
        );
      }

      MenuManager_StepHighExtractTime2_InternalState = MENUMANAGER_STEPHIGHEXTRACTTIME2_INTERNALSTATE_DONE;
      
      break;
    }
    case MENUMANAGER_STEPHIGHEXTRACTTIME2_INTERNALSTATE_DONE:
    {
      MenuManager_StepHighExtractTime2_LcdShowDone();

      MenuManager_StepHighExtractTime2_InternalState = MENUMANAGER_STEPHIGHEXTRACTTIME2_INTERNALSTATE_EXITING;

      break;
    }
    case MENUMANAGER_STEPHIGHEXTRACTTIME2_INTERNALSTATE_EXITING:
    default:
      break;
  }
}

/*=============================================================================================*/
static void MenuManager_StepHighExtractTime2_SubTickHandler(void)
{
  if (MenuManager_StepHighExtractTime2_InternalState == MENUMANAGER_STEPHIGHEXTRACTTIME2_INTERNALSTATE_EXITING)
  {
    MenuManager_StepHighExtractTime2_Counter += (uint32_t)1U;

    if (MenuManager_StepHighExtractTime2_Counter >= MENUMANAGER_STEPHIGHEXTRACTTIME2_COUNTER_MAX)
    {
      MenuManager_StepHighExtractTime2_Counter = (uint32_t)0U;
      
      Fsm_TriggerEvent(&MenuManager_FsmContext, (Fsm_EventType)MENUMANAGER_EVENT_PREV);
    }
  }
}



#ifdef  __cplusplus
}
#endif
