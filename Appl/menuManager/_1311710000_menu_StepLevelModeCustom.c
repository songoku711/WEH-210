/* 
 * File:   _1311710000_menu_StepLevelModeCustom.c
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
#define MENUMANAGER_STEPLEVELMODECUSTOM_INTERNALDATALENGTH            (uint8_t)12U

#define MENUMANAGER_STEPLEVELMODECUSTOM_INTERNALSTATE_IDX             0U
#define MENUMANAGER_STEPLEVELMODECUSTOM_COUNTER_IDX                   1U
#define MENUMANAGER_STEPLEVELMODECUSTOM_CURPOS_IDX                    2U
#define MENUMANAGER_STEPLEVELMODECUSTOM_SEQIDX_IDX                    3U
#define MENUMANAGER_STEPLEVELMODECUSTOM_STEPIDX_IDX                   4U
#define MENUMANAGER_STEPLEVELMODECUSTOM_VALUE_MIN_IDX                 5U
#define MENUMANAGER_STEPLEVELMODECUSTOM_VALUE_MAX_IDX                 6U
#define MENUMANAGER_STEPLEVELMODECUSTOM_VALUE_IDX                     7U
#define MENUMANAGER_STEPLEVELMODECUSTOM_UNITVAL_IDX                   8U

#define MenuManager_StepLevelModeCustom_InternalState                 MenuManager_GetInternalDataPtr(MENUMANAGER_STEPLEVELMODECUSTOM_INTERNALSTATE_IDX)
#define MenuManager_StepLevelModeCustom_Counter                       MenuManager_GetInternalDataPtr(MENUMANAGER_STEPLEVELMODECUSTOM_COUNTER_IDX)
#define MenuManager_StepLevelModeCustom_CurPos                        MenuManager_GetInternalDataPtr(MENUMANAGER_STEPLEVELMODECUSTOM_CURPOS_IDX)
#define MenuManager_StepLevelModeCustom_SeqIdx                        MenuManager_GetInternalDataPtr(MENUMANAGER_STEPLEVELMODECUSTOM_SEQIDX_IDX)
#define MenuManager_StepLevelModeCustom_StepIdx                       MenuManager_GetInternalDataPtr(MENUMANAGER_STEPLEVELMODECUSTOM_STEPIDX_IDX)
#define MenuManager_StepLevelModeCustom_ValueMin                      MenuManager_GetInternalDataPtr(MENUMANAGER_STEPLEVELMODECUSTOM_VALUE_MIN_IDX)
#define MenuManager_StepLevelModeCustom_ValueMax                      MenuManager_GetInternalDataPtr(MENUMANAGER_STEPLEVELMODECUSTOM_VALUE_MAX_IDX)
#define MenuManager_StepLevelModeCustom_Value                         MenuManager_GetInternalDataPtr(MENUMANAGER_STEPLEVELMODECUSTOM_VALUE_IDX)
#define MenuManager_StepLevelModeCustom_UnitVal(idx)                  MenuManager_GetInternalDataPtr(MENUMANAGER_STEPLEVELMODECUSTOM_UNITVAL_IDX + idx)



#define MENUMANAGER_STEPLEVELMODECUSTOM_INTERNALSTATE_READY           (uint32_t)0U
#define MENUMANAGER_STEPLEVELMODECUSTOM_INTERNALSTATE_RUNNING         (uint32_t)1U
#define MENUMANAGER_STEPLEVELMODECUSTOM_INTERNALSTATE_DONE            (uint32_t)2U
#define MENUMANAGER_STEPLEVELMODECUSTOM_INTERNALSTATE_EXITING         (uint32_t)3U

#define MENUMANAGER_STEPLEVELMODECUSTOM_COUNTER_MAX                   (uint32_t)50U

#define MENUMANAGER_STEPLEVELMODECUSTOM_CURPOS_XPOS                   (uint32_t)8U
#define MENUMANAGER_STEPLEVELMODECUSTOM_CURPOS_YPOS                   (uint32_t)5U

#define MENUMANAGER_STEPLEVELMODECUSTOM_UNITVAL_LENGTH                (uint32_t)4U



/** Menu manager main titles and child menu titles */
static const uint8_t MenuManager_StepLevelModeCustom_MainTitleStr[] = "USER LEVEL THRS";



/** Menu manager button event mapping array */
static MenuManager_ButEventMapStruct MenuManager_StepLevelModeCustom_ButEventMap[6] =
{
  { IOMANAGER_BUTTON_STATE_START,                                     MENUMANAGER_EVENT_START_BUT             },
  { IOMANAGER_BUTTON_STATE_STOP,                                      MENUMANAGER_EVENT_STOP_BUT              },
  { IOMANAGER_BUTTON_STATE_UP,                                        MENUMANAGER_EVENT_UP_BUT                },
  { IOMANAGER_BUTTON_STATE_DOWN,                                      MENUMANAGER_EVENT_DOWN_BUT              },
  { IOMANAGER_BUTTON_STATE_ADD,                                       MENUMANAGER_EVENT_ADD_BUT               },
  { IOMANAGER_BUTTON_STATE_SUB,                                       MENUMANAGER_EVENT_SUB_BUT               }
};

/** Menu manager button event mapping configuration */
static MenuManager_ButEventMapConfStruct MenuManager_StepLevelModeCustom_ButEventMapConf =
{
  .butEventMapNum             = (uint8_t)6U,
  .butEventMapCfg             = &MenuManager_StepLevelModeCustom_ButEventMap
};



/** Menu manager event handlers */
static Fsm_GuardType MenuManager_StepLevelModeCustom_Entry            (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_StepLevelModeCustom_Exit             (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_StepLevelModeCustom_StartBut         (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_StepLevelModeCustom_StopBut          (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_StepLevelModeCustom_UpBut            (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_StepLevelModeCustom_DownBut          (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_StepLevelModeCustom_AddBut           (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_StepLevelModeCustom_SubBut           (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);

/** Menu manager state machine */
Fsm_EventEntryStruct MenuManager_StepLevelModeCustom_StateMachine[9] =
{
  FSM_TRIGGER_ENTRY             (                                     MenuManager_StepLevelModeCustom_Entry                                           ),
  FSM_TRIGGER_EXIT              (                                     MenuManager_StepLevelModeCustom_Exit                                            ),
  FSM_TRIGGER_TRANSITION        ( MENUMANAGER_EVENT_PREV,                                                       MENUMANAGER_STATE_STEP_NORM_SETUP     ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_START_BUT,        MenuManager_StepLevelModeCustom_StartBut                                        ),
  FSM_TRIGGER_TRANSITION_ACTION ( MENUMANAGER_EVENT_STOP_BUT,         MenuManager_StepLevelModeCustom_StopBut,  MENUMANAGER_STATE_STEP_NORM_SETUP     ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_UP_BUT,           MenuManager_StepLevelModeCustom_UpBut                                           ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_DOWN_BUT,         MenuManager_StepLevelModeCustom_DownBut                                         ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_ADD_BUT,          MenuManager_StepLevelModeCustom_AddBut                                          ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_SUB_BUT,          MenuManager_StepLevelModeCustom_SubBut                                          )
};



/*===============================================================================================
*                                   LOCAL FUNCTION PROTOTYPES
===============================================================================================*/

static void MenuManager_StepLevelModeCustom_LcdShowMainTitle(void);
static void MenuManager_StepLevelModeCustom_LcdShowAdjust(void);
static void MenuManager_StepLevelModeCustom_LcdShowDone(void);

static void MenuManager_StepLevelModeCustom_SubMainFunction(void);
static void MenuManager_StepLevelModeCustom_SubTickHandler(void);



/*===============================================================================================
*                                       LOCAL FUNCTIONS
===============================================================================================*/

static void MenuManager_StepLevelModeCustom_LcdShowMainTitle(void)
{
  /* Print main title */
  MenuManager_Common_LcdClearAdjustMenuStatic();
  MenuManager_Common_LcdShowMainTitle(MenuManager_StepLevelModeCustom_MainTitleStr);
  MenuManager_Common_LcdShowOkExitChoice();
  MenuManager_Common_LcdUpdateAdjustMenuStatic();
}

/*=============================================================================================*/
static void MenuManager_StepLevelModeCustom_LcdShowAdjust(void)
{
  MenuManager_Common_LcdClearAdjustMenuDynamic();

  MenuManager_Common_LcdShowAdjustUnitVal
  (
    MENUMANAGER_STEPLEVELMODECUSTOM_CURPOS_XPOS,
    MENUMANAGER_STEPLEVELMODECUSTOM_CURPOS_YPOS,
    &(MenuManager_StepLevelModeCustom_UnitVal(0)),
    MENUMANAGER_STEPLEVELMODECUSTOM_UNITVAL_LENGTH
  );
  
  MenuManager_Common_LcdShowAdjustArrow
  (
    MENUMANAGER_STEPLEVELMODECUSTOM_CURPOS_XPOS,
    MENUMANAGER_STEPLEVELMODECUSTOM_CURPOS_YPOS,
    MenuManager_StepLevelModeCustom_CurPos
  );

  MenuManager_Common_LcdUpdateAdjustMenuDynamic();
}

/*=============================================================================================*/
static void MenuManager_StepLevelModeCustom_LcdShowDone(void)
{
  MenuManager_Common_LcdClearAll();

  MenuManager_Common_LcdShowAdjustUnitVal
  (
    MENUMANAGER_STEPLEVELMODECUSTOM_CURPOS_XPOS,
    MENUMANAGER_STEPLEVELMODECUSTOM_CURPOS_YPOS,
    &(MenuManager_StepLevelModeCustom_UnitVal(0)),
    MENUMANAGER_STEPLEVELMODECUSTOM_UNITVAL_LENGTH
  );

  MenuManager_Common_LcdUpdateAll();
}



/*=============================================================================================*/
static Fsm_GuardType MenuManager_StepLevelModeCustom_Entry(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  MenuManager_Common_ProgramSetupStruct* enterDataHierachy;
  uint16_t tempLevelThreshold;
  HAL_StatusTypeDef retVal = HAL_OK;

  /* Check if previous state data hierachy is not empty */
  if (pFsmContext->dataHierachy != NULL)
  {
    if (pFsmContext->dataHierachy->dataId == MENUMANAGER_STATE_STEP_LEVEL_MODE)
    {
      enterDataHierachy = (MenuManager_Common_ProgramSetupStruct *)(pFsmContext->dataHierachy);

      MenuManager_InternalDataPush(MENUMANAGER_STEPLEVELMODECUSTOM_INTERNALDATALENGTH);

      MenuManager_StepLevelModeCustom_InternalState = MENUMANAGER_STEPLEVELMODECUSTOM_INTERNALSTATE_READY;
      MenuManager_StepLevelModeCustom_Counter = (uint32_t)0U;
      MenuManager_StepLevelModeCustom_CurPos = (uint32_t)0U;

      MenuManager_StepLevelModeCustom_SeqIdx = enterDataHierachy->seqIdx;
      MenuManager_StepLevelModeCustom_StepIdx = enterDataHierachy->stepIdx;

      ProgramManager_NormStepConfig_LevelThreshold_GetData
      (
        (uint8_t)MenuManager_StepLevelModeCustom_SeqIdx,
        (uint8_t)MenuManager_StepLevelModeCustom_StepIdx,
        &tempLevelThreshold
      );

      MenuManager_StepLevelModeCustom_ValueMin = (uint32_t)0U;
      MenuManager_StepLevelModeCustom_ValueMax = (uint32_t)PROGRAMMANAGER_FILLLEVELSETUP_WATER_LEVEL_MAX;

      MenuManager_StepLevelModeCustom_Value = (uint32_t)tempLevelThreshold;

      MenuManager_Common_DecToBcdConv
      (
        &(MenuManager_StepLevelModeCustom_Value),
        &(MenuManager_StepLevelModeCustom_UnitVal(0)),
        (uint8_t)MENUMANAGER_STEPLEVELMODECUSTOM_UNITVAL_LENGTH
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
    MenuManager_StepLevelModeCustom_LcdShowMainTitle();
    MenuManager_StepLevelModeCustom_LcdShowAdjust();

    MenuManager_SubMainFunction = MenuManager_StepLevelModeCustom_SubMainFunction;
    MenuManager_SubTickHandler = MenuManager_StepLevelModeCustom_SubTickHandler;

    return FSM_GUARD_TRUE;
  }

  return FSM_GUARD_FALSE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_StepLevelModeCustom_Exit(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  Fsm_DataHierachyStruct* dataHierachy;

  MenuManager_SubMainFunction = NULL;
  MenuManager_SubTickHandler = NULL;

  dataHierachy = (Fsm_DataHierachyStruct *)MenuManager_malloc(sizeof(Fsm_DataHierachyStruct));
  dataHierachy->dataId = MENUMANAGER_STATE_STEP_LEVEL_MODE_CUSTOM;

  pFsmContext->dataHierachy = dataHierachy;

  /* Free internal data */
  MenuManager_InternalDataPop();
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_StepLevelModeCustom_StartBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_StepLevelModeCustom_InternalState == MENUMANAGER_STEPLEVELMODECUSTOM_INTERNALSTATE_READY)
  {
    MenuManager_StepLevelModeCustom_InternalState = MENUMANAGER_STEPLEVELMODECUSTOM_INTERNALSTATE_RUNNING;
  }
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_StepLevelModeCustom_StopBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_StepLevelModeCustom_InternalState != MENUMANAGER_STEPLEVELMODECUSTOM_INTERNALSTATE_READY)
  {
    return FSM_GUARD_FALSE;
  }
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_StepLevelModeCustom_UpBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  uint32_t tempUnitVal;
  uint32_t tempVal;

  if (MenuManager_StepLevelModeCustom_InternalState == MENUMANAGER_STEPLEVELMODECUSTOM_INTERNALSTATE_READY)
  {
    tempUnitVal = MenuManager_StepLevelModeCustom_UnitVal(MenuManager_StepLevelModeCustom_CurPos);

    MenuManager_StepLevelModeCustom_UnitVal(MenuManager_StepLevelModeCustom_CurPos) += (uint32_t)1U;

    if (MenuManager_StepLevelModeCustom_UnitVal(MenuManager_StepLevelModeCustom_CurPos) > (uint32_t)9U)
    {
      MenuManager_StepLevelModeCustom_UnitVal(MenuManager_StepLevelModeCustom_CurPos) = (uint32_t)0U;
    }

    MenuManager_Common_BcdToDecConv(&tempVal, &(MenuManager_StepLevelModeCustom_UnitVal(0)), (uint8_t)MENUMANAGER_STEPLEVELMODECUSTOM_UNITVAL_LENGTH);

    if ((tempVal <= MenuManager_StepLevelModeCustom_ValueMin) || (tempVal >= MenuManager_StepLevelModeCustom_ValueMax))
    {
      MenuManager_StepLevelModeCustom_UnitVal(MenuManager_StepLevelModeCustom_CurPos) = tempUnitVal;
    }

    MenuManager_StepLevelModeCustom_LcdShowAdjust();
  }

  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_StepLevelModeCustom_DownBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  uint32_t tempUnitVal;
  uint32_t tempVal;

  if (MenuManager_StepLevelModeCustom_InternalState == MENUMANAGER_STEPLEVELMODECUSTOM_INTERNALSTATE_READY)
  {
    tempUnitVal = MenuManager_StepLevelModeCustom_UnitVal(MenuManager_StepLevelModeCustom_CurPos);

    MenuManager_StepLevelModeCustom_UnitVal(MenuManager_StepLevelModeCustom_CurPos) -= (uint32_t)1U;

    if (MenuManager_StepLevelModeCustom_UnitVal(MenuManager_StepLevelModeCustom_CurPos) > (uint32_t)9U)
    {
      MenuManager_StepLevelModeCustom_UnitVal(MenuManager_StepLevelModeCustom_CurPos) = (uint32_t)9U;
    }

    MenuManager_Common_BcdToDecConv(&tempVal, &(MenuManager_StepLevelModeCustom_UnitVal(0)), (uint8_t)MENUMANAGER_STEPLEVELMODECUSTOM_UNITVAL_LENGTH);

    if ((tempVal <= MenuManager_StepLevelModeCustom_ValueMin) || (tempVal >= MenuManager_StepLevelModeCustom_ValueMax))
    {
      MenuManager_StepLevelModeCustom_UnitVal(MenuManager_StepLevelModeCustom_CurPos) = tempUnitVal;
    }

    MenuManager_StepLevelModeCustom_LcdShowAdjust();
  }
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_StepLevelModeCustom_AddBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_StepLevelModeCustom_InternalState == MENUMANAGER_STEPLEVELMODECUSTOM_INTERNALSTATE_READY)
  {
    if (MenuManager_StepLevelModeCustom_CurPos > (uint32_t)0U)
    {
      MenuManager_StepLevelModeCustom_CurPos -= (uint32_t)1U;
    }

    MenuManager_StepLevelModeCustom_LcdShowAdjust();
  }
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_StepLevelModeCustom_SubBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_StepLevelModeCustom_InternalState == MENUMANAGER_STEPLEVELMODECUSTOM_INTERNALSTATE_READY)
  {
    if (MenuManager_StepLevelModeCustom_CurPos < (MENUMANAGER_STEPLEVELMODECUSTOM_UNITVAL_LENGTH - (uint32_t)1U))
    {
      MenuManager_StepLevelModeCustom_CurPos += (uint32_t)1U;
    }
    
    MenuManager_StepLevelModeCustom_LcdShowAdjust();
  }
  
  return FSM_GUARD_TRUE;
}



/*=============================================================================================*/
static void MenuManager_StepLevelModeCustom_SubMainFunction(void)
{
  MenuManager_Common_ButEventMapHandler(&MenuManager_StepLevelModeCustom_ButEventMapConf);

  switch (MenuManager_StepLevelModeCustom_InternalState)
  {
    case MENUMANAGER_STEPLEVELMODECUSTOM_INTERNALSTATE_READY:
      break;
    case MENUMANAGER_STEPLEVELMODECUSTOM_INTERNALSTATE_RUNNING:
    {
      uint16_t tempLevelThreshold;

      MenuManager_Common_BcdToDecConv(&(MenuManager_StepLevelModeCustom_Value), &(MenuManager_StepLevelModeCustom_UnitVal(0)), (uint8_t)MENUMANAGER_STEPLEVELMODECUSTOM_UNITVAL_LENGTH);

      tempLevelThreshold = (uint16_t)MenuManager_StepLevelModeCustom_Value;

      ProgramManager_NormStepConfig_LevelThreshold_SetData \
      ( \
        (uint8_t)MenuManager_StepLevelModeCustom_SeqIdx, \
        (uint8_t)MenuManager_StepLevelModeCustom_StepIdx, \
        &tempLevelThreshold \
      );

      if ((uint8_t)MenuManager_StepLevelModeCustom_SeqIdx == ProgramManager_gAutoSeqConfig.sequenceIndex)
      {
        ProgramManager_NormStepConfig_LevelThreshold_GetData \
        ( \
          (uint8_t)MenuManager_StepLevelModeCustom_SeqIdx, \
          (uint8_t)MenuManager_StepLevelModeCustom_StepIdx, \
          &((ProgramManager_gAutoSeqConfig.normStep)[MenuManager_StepLevelModeCustom_StepIdx].levelThreshold) \
        );
      }

      MenuManager_StepLevelModeCustom_InternalState = MENUMANAGER_STEPLEVELMODECUSTOM_INTERNALSTATE_DONE;
      
      break;
    }
    case MENUMANAGER_STEPLEVELMODECUSTOM_INTERNALSTATE_DONE:
    {
      MenuManager_StepLevelModeCustom_LcdShowDone();

      MenuManager_StepLevelModeCustom_InternalState = MENUMANAGER_STEPLEVELMODECUSTOM_INTERNALSTATE_EXITING;

      break;
    }
    case MENUMANAGER_STEPLEVELMODECUSTOM_INTERNALSTATE_EXITING:
    default:
      break;
  }
}

/*=============================================================================================*/
static void MenuManager_StepLevelModeCustom_SubTickHandler(void)
{
  if (MenuManager_StepLevelModeCustom_InternalState == MENUMANAGER_STEPLEVELMODECUSTOM_INTERNALSTATE_EXITING)
  {
    MenuManager_StepLevelModeCustom_Counter += (uint32_t)1U;

    if (MenuManager_StepLevelModeCustom_Counter >= MENUMANAGER_STEPLEVELMODECUSTOM_COUNTER_MAX)
    {
      MenuManager_StepLevelModeCustom_Counter = (uint32_t)0U;
      
      Fsm_TriggerEvent(&MenuManager_FsmContext, (Fsm_EventType)MENUMANAGER_EVENT_PREV);
    }
  }
}



#ifdef  __cplusplus
}
#endif
