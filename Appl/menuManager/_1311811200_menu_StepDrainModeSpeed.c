/* 
 * File:   _1311811200_menu_StepDrainModeSpeed.c
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
#define MENUMANAGER_STEPDRAINMODESPEED_INTERNALDATALENGTH             (uint8_t)8U

#define MENUMANAGER_STEPDRAINMODESPEED_SEQIDX_IDX                     0U
#define MENUMANAGER_STEPDRAINMODESPEED_STEPIDX_IDX                    1U
#define MENUMANAGER_STEPDRAINMODESPEED_DRAINIDX_IDX                   2U
#define MENUMANAGER_STEPDRAINMODESPEED_INTERNALSTATE_IDX              3U
#define MENUMANAGER_STEPDRAINMODESPEED_COUNTER_IDX                    4U
#define MENUMANAGER_STEPDRAINMODESPEED_VALUE_MIN_IDX                  5U
#define MENUMANAGER_STEPDRAINMODESPEED_VALUE_MAX_IDX                  6U
#define MENUMANAGER_STEPDRAINMODESPEED_VALUE_IDX                      7U

#define MenuManager_StepDrainModeSpeed_SeqIdx                         MenuManager_GetInternalDataPtr(MENUMANAGER_STEPDRAINMODESPEED_SEQIDX_IDX)
#define MenuManager_StepDrainModeSpeed_StepIdx                        MenuManager_GetInternalDataPtr(MENUMANAGER_STEPDRAINMODESPEED_STEPIDX_IDX)
#define MenuManager_StepDrainModeSpeed_DrainIdx                       MenuManager_GetInternalDataPtr(MENUMANAGER_STEPDRAINMODESPEED_DRAINIDX_IDX)
#define MenuManager_StepDrainModeSpeed_InternalState                  MenuManager_GetInternalDataPtr(MENUMANAGER_STEPDRAINMODESPEED_INTERNALSTATE_IDX)
#define MenuManager_StepDrainModeSpeed_Counter                        MenuManager_GetInternalDataPtr(MENUMANAGER_STEPDRAINMODESPEED_COUNTER_IDX)
#define MenuManager_StepDrainModeSpeed_ValueMin                       MenuManager_GetInternalDataPtr(MENUMANAGER_STEPDRAINMODESPEED_VALUE_MIN_IDX)
#define MenuManager_StepDrainModeSpeed_ValueMax                       MenuManager_GetInternalDataPtr(MENUMANAGER_STEPDRAINMODESPEED_VALUE_MAX_IDX)
#define MenuManager_StepDrainModeSpeed_Value                          MenuManager_GetInternalDataPtr(MENUMANAGER_STEPDRAINMODESPEED_VALUE_IDX)



#define MENUMANAGER_STEPDRAINMODESPEED_INTERNALSTATE_READY            (uint32_t)0U
#define MENUMANAGER_STEPDRAINMODESPEED_INTERNALSTATE_RUNNING          (uint32_t)1U
#define MENUMANAGER_STEPDRAINMODESPEED_INTERNALSTATE_DONE             (uint32_t)2U
#define MENUMANAGER_STEPDRAINMODESPEED_INTERNALSTATE_EXITING          (uint32_t)3U

#define MENUMANAGER_STEPDRAINMODESPEED_COUNTER_MAX                    (uint32_t)50U

#define MENUMANAGER_STEPDRAINMODESPEED_VALUE_XPOS                     (uint32_t)10U
#define MENUMANAGER_STEPDRAINMODESPEED_VALUE_YPOS                     (uint32_t)5U

#define MENUMANAGER_STEPDRAINMODESPEED_VALUE_LENGTH                   (uint32_t)1U

#define MENUMANAGER_STEPDRAINMODESPEED_UNIT_XPOS                      (uint32_t)1U
#define MENUMANAGER_STEPDRAINMODESPEED_UNIT_YPOS                      (uint32_t)3U



/** Menu manager main titles and child menu titles */
static const uint8_t MenuManager_StepDrainModeSpeed_MainTitleStr[] =  "DRAIN SPEED";



/** Menu manager button event mapping array */
static MenuManager_ButEventMapStruct MenuManager_StepDrainModeSpeed_ButEventMap[4] =
{
  { IOMANAGER_BUTTON_STATE_START,                                     MENUMANAGER_EVENT_START_BUT             },
  { IOMANAGER_BUTTON_STATE_STOP,                                      MENUMANAGER_EVENT_STOP_BUT              },
  { IOMANAGER_BUTTON_STATE_UP,                                        MENUMANAGER_EVENT_UP_BUT                },
  { IOMANAGER_BUTTON_STATE_DOWN,                                      MENUMANAGER_EVENT_DOWN_BUT              }
};

/** Menu manager button event mapping configuration */
static MenuManager_ButEventMapConfStruct MenuManager_StepDrainModeSpeed_ButEventMapConf =
{
  .butEventMapNum             = (uint8_t)4U,
  .butEventMapCfg             = &MenuManager_StepDrainModeSpeed_ButEventMap
};



/** Menu manager event handlers */
static Fsm_GuardType MenuManager_StepDrainModeSpeed_Entry             (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_StepDrainModeSpeed_Exit              (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_StepDrainModeSpeed_StartBut          (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_StepDrainModeSpeed_StopBut           (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_StepDrainModeSpeed_UpBut             (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_StepDrainModeSpeed_DownBut           (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);

/** Menu manager state machine */
Fsm_EventEntryStruct MenuManager_StepDrainModeSpeed_StateMachine[7] =
{
  FSM_TRIGGER_ENTRY             (                                     MenuManager_StepDrainModeSpeed_Entry                                            ),
  FSM_TRIGGER_EXIT              (                                     MenuManager_StepDrainModeSpeed_Exit                                             ),
  FSM_TRIGGER_TRANSITION        ( MENUMANAGER_EVENT_PREV,                                                     MENUMANAGER_STATE_STEP_DRAIN_MODE_PARAM ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_START_BUT,        MenuManager_StepDrainModeSpeed_StartBut                                         ),
  FSM_TRIGGER_TRANSITION_ACTION ( MENUMANAGER_EVENT_STOP_BUT,         MenuManager_StepDrainModeSpeed_StopBut, MENUMANAGER_STATE_STEP_DRAIN_MODE_PARAM ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_UP_BUT,           MenuManager_StepDrainModeSpeed_UpBut                                            ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_DOWN_BUT,         MenuManager_StepDrainModeSpeed_DownBut                                          )
};



/*===============================================================================================
*                                   LOCAL FUNCTION PROTOTYPES
===============================================================================================*/

static void MenuManager_StepDrainModeSpeed_LcdShowMainTitle(void);
static void MenuManager_StepDrainModeSpeed_LcdShowAdjust(void);
static void MenuManager_StepDrainModeSpeed_LcdShowDone(void);

static void MenuManager_StepDrainModeSpeed_SubMainFunction(void);
static void MenuManager_StepDrainModeSpeed_SubTickHandler(void);



/*===============================================================================================
*                                       LOCAL FUNCTIONS
===============================================================================================*/

static void MenuManager_StepDrainModeSpeed_LcdShowMainTitle(void)
{
  /* Print main title */
  MenuManager_Common_LcdClearAdjustMenuStatic();
  MenuManager_Common_LcdShowMainTitle(MenuManager_StepDrainModeSpeed_MainTitleStr);
  MenuManager_Common_LcdShowOkExitChoice();
  MenuManager_Common_LcdUpdateAdjustMenuStatic();
}

/*=============================================================================================*/
static void MenuManager_StepDrainModeSpeed_LcdShowAdjust(void)
{
  MenuManager_Common_LcdClearAdjustMenuDynamic();

  MenuManager_Common_LcdShowAdjustUnitVal
  (
    MENUMANAGER_STEPDRAINMODESPEED_VALUE_XPOS,
    MENUMANAGER_STEPDRAINMODESPEED_VALUE_YPOS,
    &(MenuManager_StepDrainModeSpeed_Value),
    MENUMANAGER_STEPDRAINMODESPEED_VALUE_LENGTH
  );
  
  MenuManager_Common_LcdShowAdjustArrow
  (
    MENUMANAGER_STEPDRAINMODESPEED_VALUE_XPOS,
    MENUMANAGER_STEPDRAINMODESPEED_VALUE_YPOS,
    (uint32_t)0U
  );

  LCD_SetCursorPos(MENUMANAGER_STEPDRAINMODESPEED_UNIT_XPOS, MENUMANAGER_STEPDRAINMODESPEED_UNIT_YPOS, LCD_CURSOR_BY_FONT);
  LCD_PutString((uint8_t *)MenuManager_Common_LevelStr);

  MenuManager_Common_LcdUpdateAdjustMenuDynamic();
}

/*=============================================================================================*/
static void MenuManager_StepDrainModeSpeed_LcdShowDone(void)
{
  MenuManager_Common_LcdClearAll();

  MenuManager_Common_LcdShowAdjustUnitVal
  (
    MENUMANAGER_STEPDRAINMODESPEED_VALUE_XPOS,
    MENUMANAGER_STEPDRAINMODESPEED_VALUE_YPOS,
    &(MenuManager_StepDrainModeSpeed_Value),
    MENUMANAGER_STEPDRAINMODESPEED_VALUE_LENGTH
  );

  LCD_SetCursorPos(MENUMANAGER_STEPDRAINMODESPEED_UNIT_XPOS, MENUMANAGER_STEPDRAINMODESPEED_UNIT_YPOS, LCD_CURSOR_BY_FONT);
  LCD_PutString((uint8_t *)MenuManager_Common_LevelStr);

  MenuManager_Common_LcdUpdateAll();
}



/*=============================================================================================*/
static Fsm_GuardType MenuManager_StepDrainModeSpeed_Entry(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  MenuManager_Common_ProgramSetupStruct* enterDataHierachy;
  ProgramManager_MotorSpeedType tempDrainSpeed;
  HAL_StatusTypeDef retVal = HAL_OK;

  /* Check if previous state data hierachy is not empty */
  if (pFsmContext->dataHierachy != NULL)
  {
    if (pFsmContext->dataHierachy->dataId == MENUMANAGER_STATE_STEP_DRAIN_MODE_PARAM)
    {
      enterDataHierachy = (MenuManager_Common_ProgramSetupStruct *)(pFsmContext->dataHierachy);

      MenuManager_InternalDataPush(MENUMANAGER_STEPDRAINMODESPEED_INTERNALDATALENGTH);

      MenuManager_StepDrainModeSpeed_SeqIdx = enterDataHierachy->seqIdx;
      MenuManager_StepDrainModeSpeed_StepIdx = enterDataHierachy->stepIdx;
      MenuManager_StepDrainModeSpeed_DrainIdx = enterDataHierachy->drainIdx;

      MenuManager_StepDrainModeSpeed_InternalState = MENUMANAGER_STEPDRAINMODESPEED_INTERNALSTATE_READY;
      MenuManager_StepDrainModeSpeed_Counter = (uint32_t)0U;

      ProgramManager_NormStepConfig_DrainStepConfig_DrainSpeed_GetData \
      ( \
        (uint8_t)MenuManager_StepDrainModeSpeed_SeqIdx,   \
        (uint8_t)MenuManager_StepDrainModeSpeed_StepIdx,  \
        (uint8_t)MenuManager_StepDrainModeSpeed_DrainIdx, \
        &tempDrainSpeed \
      );

      MenuManager_StepDrainModeSpeed_ValueMin = (uint32_t)PROGRAMMANAGER_MOTOR_SPEED_LEVEL_0;
      MenuManager_StepDrainModeSpeed_ValueMax = (uint32_t)(ProgramManager_gParamConfig.drainCfg.maxDrainExtrSpeed);
      MenuManager_StepDrainModeSpeed_Value = (uint32_t)(tempDrainSpeed);

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
    MenuManager_StepDrainModeSpeed_LcdShowMainTitle();
    MenuManager_StepDrainModeSpeed_LcdShowAdjust();

    MenuManager_SubMainFunction = MenuManager_StepDrainModeSpeed_SubMainFunction;
    MenuManager_SubTickHandler = MenuManager_StepDrainModeSpeed_SubTickHandler;

    return FSM_GUARD_TRUE;
  }

  return FSM_GUARD_FALSE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_StepDrainModeSpeed_Exit(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  Fsm_DataHierachyStruct* dataHierachy;

  MenuManager_SubMainFunction = NULL;
  MenuManager_SubTickHandler = NULL;

  dataHierachy = (Fsm_DataHierachyStruct *)MenuManager_malloc(sizeof(Fsm_DataHierachyStruct));
  dataHierachy->dataId = MENUMANAGER_STATE_STEP_DRAIN_MODE_SPEED;

  pFsmContext->dataHierachy = dataHierachy;

  /* Free internal data */
  MenuManager_InternalDataPop();
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_StepDrainModeSpeed_StartBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_StepDrainModeSpeed_InternalState == MENUMANAGER_STEPDRAINMODESPEED_INTERNALSTATE_READY)
  {
    MenuManager_StepDrainModeSpeed_InternalState = MENUMANAGER_STEPDRAINMODESPEED_INTERNALSTATE_RUNNING;
  }
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_StepDrainModeSpeed_StopBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_StepDrainModeSpeed_InternalState != MENUMANAGER_STEPDRAINMODESPEED_INTERNALSTATE_READY)
  {
    return FSM_GUARD_FALSE;
  }
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_StepDrainModeSpeed_UpBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_StepDrainModeSpeed_InternalState == MENUMANAGER_STEPDRAINMODESPEED_INTERNALSTATE_READY)
  {
    if (MenuManager_StepDrainModeSpeed_Value < MenuManager_StepDrainModeSpeed_ValueMax)
    {
      MenuManager_StepDrainModeSpeed_Value += (uint32_t)1U;
    }

    MenuManager_StepDrainModeSpeed_LcdShowAdjust();
  }

  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_StepDrainModeSpeed_DownBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_StepDrainModeSpeed_InternalState == MENUMANAGER_STEPDRAINMODESPEED_INTERNALSTATE_READY)
  {
    if (MenuManager_StepDrainModeSpeed_Value > MenuManager_StepDrainModeSpeed_ValueMin)
    {
      MenuManager_StepDrainModeSpeed_Value -= (uint32_t)1U;
    }

    MenuManager_StepDrainModeSpeed_LcdShowAdjust();
  }
  
  return FSM_GUARD_TRUE;
}



/*=============================================================================================*/
static void MenuManager_StepDrainModeSpeed_SubMainFunction(void)
{
  MenuManager_Common_ButEventMapHandler(&MenuManager_StepDrainModeSpeed_ButEventMapConf);

  switch (MenuManager_StepDrainModeSpeed_InternalState)
  {
    case MENUMANAGER_STEPDRAINMODESPEED_INTERNALSTATE_READY:
      break;
    case MENUMANAGER_STEPDRAINMODESPEED_INTERNALSTATE_RUNNING:
    {
      ProgramManager_MotorSpeedType tempDrainSpeed;

      tempDrainSpeed = (ProgramManager_MotorSpeedType)MenuManager_StepDrainModeSpeed_Value;

      ProgramManager_NormStepConfig_DrainStepConfig_DrainSpeed_SetData \
      ( \
        (uint8_t)MenuManager_StepDrainModeSpeed_SeqIdx,   \
        (uint8_t)MenuManager_StepDrainModeSpeed_StepIdx,  \
        (uint8_t)MenuManager_StepDrainModeSpeed_DrainIdx, \
        &tempDrainSpeed \
      );

      if ((uint8_t)MenuManager_StepDrainModeSpeed_SeqIdx == ProgramManager_gAutoSeqConfig.sequenceIndex)
      {
        ProgramManager_NormStepConfig_DrainStepConfig_DrainSpeed_GetData
        ( \
          (uint8_t)MenuManager_StepDrainModeSpeed_SeqIdx,   \
          (uint8_t)MenuManager_StepDrainModeSpeed_StepIdx,  \
          (uint8_t)MenuManager_StepDrainModeSpeed_DrainIdx, \
          &(ProgramManager_gAutoSeqConfig.normStep[MenuManager_StepDrainModeSpeed_StepIdx].drainStep[MenuManager_StepDrainModeSpeed_DrainIdx].drainSpeed) \
        );
      }

      MenuManager_StepDrainModeSpeed_InternalState = MENUMANAGER_STEPDRAINMODESPEED_INTERNALSTATE_DONE;
      
      break;
    }
    case MENUMANAGER_STEPDRAINMODESPEED_INTERNALSTATE_DONE:
    {
      MenuManager_StepDrainModeSpeed_LcdShowDone();

      MenuManager_StepDrainModeSpeed_InternalState = MENUMANAGER_STEPDRAINMODESPEED_INTERNALSTATE_EXITING;

      break;
    }
    case MENUMANAGER_STEPDRAINMODESPEED_INTERNALSTATE_EXITING:
    default:
      break;
  }
}

/*=============================================================================================*/
static void MenuManager_StepDrainModeSpeed_SubTickHandler(void)
{
  if (MenuManager_StepDrainModeSpeed_InternalState == MENUMANAGER_STEPDRAINMODESPEED_INTERNALSTATE_EXITING)
  {
    MenuManager_StepDrainModeSpeed_Counter += (uint32_t)1U;

    if (MenuManager_StepDrainModeSpeed_Counter >= MENUMANAGER_STEPDRAINMODESPEED_COUNTER_MAX)
    {
      MenuManager_StepDrainModeSpeed_Counter = (uint32_t)0U;
      
      Fsm_TriggerEvent(&MenuManager_FsmContext, (Fsm_EventType)MENUMANAGER_EVENT_PREV);
    }
  }
}



#ifdef  __cplusplus
}
#endif
