/* 
 * File:   _1268000000_menu_StepWashSpeed.c
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
#define MENUMANAGER_STEPWASHSPEED_INTERNALDATALENGTH                  (uint8_t)7U

#define MENUMANAGER_STEPWASHSPEED_INTERNALSTATE_IDX                   0U
#define MENUMANAGER_STEPWASHSPEED_COUNTER_IDX                         1U
#define MENUMANAGER_STEPWASHSPEED_SEQIDX_IDX                          2U
#define MENUMANAGER_STEPWASHSPEED_STEPIDX_IDX                         3U
#define MENUMANAGER_STEPWASHSPEED_VALUE_MIN_IDX                       4U
#define MENUMANAGER_STEPWASHSPEED_VALUE_MAX_IDX                       5U
#define MENUMANAGER_STEPWASHSPEED_VALUE_IDX                           6U

#define MenuManager_StepWashSpeed_InternalState                       MenuManager_GetInternalDataPtr(MENUMANAGER_STEPWASHSPEED_INTERNALSTATE_IDX)
#define MenuManager_StepWashSpeed_Counter                             MenuManager_GetInternalDataPtr(MENUMANAGER_STEPWASHSPEED_COUNTER_IDX)
#define MenuManager_StepWashSpeed_SeqIdx                              MenuManager_GetInternalDataPtr(MENUMANAGER_STEPWASHSPEED_SEQIDX_IDX)
#define MenuManager_StepWashSpeed_StepIdx                             MenuManager_GetInternalDataPtr(MENUMANAGER_STEPWASHSPEED_STEPIDX_IDX)
#define MenuManager_StepWashSpeed_ValueMin                            MenuManager_GetInternalDataPtr(MENUMANAGER_STEPWASHSPEED_VALUE_MIN_IDX)
#define MenuManager_StepWashSpeed_ValueMax                            MenuManager_GetInternalDataPtr(MENUMANAGER_STEPWASHSPEED_VALUE_MAX_IDX)
#define MenuManager_StepWashSpeed_Value                               MenuManager_GetInternalDataPtr(MENUMANAGER_STEPWASHSPEED_VALUE_IDX)



#define MENUMANAGER_STEPWASHSPEED_INTERNALSTATE_READY                 (uint32_t)0U
#define MENUMANAGER_STEPWASHSPEED_INTERNALSTATE_RUNNING               (uint32_t)1U
#define MENUMANAGER_STEPWASHSPEED_INTERNALSTATE_DONE                  (uint32_t)2U
#define MENUMANAGER_STEPWASHSPEED_INTERNALSTATE_EXITING               (uint32_t)3U

#define MENUMANAGER_STEPWASHSPEED_COUNTER_MAX                         (uint32_t)50U

#define MENUMANAGER_STEPWASHSPEED_VALUE_XPOS                          (uint32_t)10U
#define MENUMANAGER_STEPWASHSPEED_VALUE_YPOS                          (uint32_t)5U

#define MENUMANAGER_STEPWASHSPEED_VALUE_LENGTH                        (uint32_t)1U

#define MENUMANAGER_STEPWASHSPEED_UNIT_XPOS                           (uint32_t)1U
#define MENUMANAGER_STEPWASHSPEED_UNIT_YPOS                           (uint32_t)3U



/** Menu manager main titles and child menu titles */
static const uint8_t MenuManager_StepWashSpeed_MainTitleStr[] =       "USER WASH SPEED";



/** Menu manager button event mapping array */
static MenuManager_ButEventMapStruct MenuManager_StepWashSpeed_ButEventMap[4] =
{
  { IOMANAGER_BUTTON_STATE_START,                                     MENUMANAGER_EVENT_START_BUT             },
  { IOMANAGER_BUTTON_STATE_STOP,                                      MENUMANAGER_EVENT_STOP_BUT              },
  { IOMANAGER_BUTTON_STATE_UP,                                        MENUMANAGER_EVENT_UP_BUT                },
  { IOMANAGER_BUTTON_STATE_DOWN,                                      MENUMANAGER_EVENT_DOWN_BUT              }
};

/** Menu manager button event mapping configuration */
static MenuManager_ButEventMapConfStruct MenuManager_StepWashSpeed_ButEventMapConf =
{
  .butEventMapNum             = (uint8_t)4U,
  .butEventMapCfg             = &MenuManager_StepWashSpeed_ButEventMap
};



/** Menu manager event handlers */
static Fsm_GuardType MenuManager_StepWashSpeed_Entry                  (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_StepWashSpeed_Exit                   (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_StepWashSpeed_StartBut               (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_StepWashSpeed_StopBut                (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_StepWashSpeed_UpBut                  (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_StepWashSpeed_DownBut                (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);

/** Menu manager state machine */
Fsm_EventEntryStruct MenuManager_StepWashSpeed_StateMachine[7] =
{
  FSM_TRIGGER_ENTRY             (                                     MenuManager_StepWashSpeed_Entry                                                 ),
  FSM_TRIGGER_EXIT              (                                     MenuManager_StepWashSpeed_Exit                                                  ),
  FSM_TRIGGER_TRANSITION        ( MENUMANAGER_EVENT_PREV,                                                     MENUMANAGER_STATE_STEP_WASH_MODE_CUSTOM ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_START_BUT,        MenuManager_StepWashSpeed_StartBut                                              ),
  FSM_TRIGGER_TRANSITION_ACTION ( MENUMANAGER_EVENT_STOP_BUT,         MenuManager_StepWashSpeed_StopBut,      MENUMANAGER_STATE_STEP_WASH_MODE_CUSTOM ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_UP_BUT,           MenuManager_StepWashSpeed_UpBut                                                 ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_DOWN_BUT,         MenuManager_StepWashSpeed_DownBut                                               )
};



/*===============================================================================================
*                                   LOCAL FUNCTION PROTOTYPES
===============================================================================================*/

static void MenuManager_StepWashSpeed_LcdShowMainTitle(void);
static void MenuManager_StepWashSpeed_LcdShowAdjust(void);
static void MenuManager_StepWashSpeed_LcdShowDone(void);

static void MenuManager_StepWashSpeed_SubMainFunction(void);
static void MenuManager_StepWashSpeed_SubTickHandler(void);



/*===============================================================================================
*                                       LOCAL FUNCTIONS
===============================================================================================*/

static void MenuManager_StepWashSpeed_LcdShowMainTitle(void)
{
  /* Print main title */
  MenuManager_Common_LcdClearAdjustMenuStatic();
  MenuManager_Common_LcdShowMainTitle(MenuManager_StepWashSpeed_MainTitleStr);
  MenuManager_Common_LcdShowOkExitChoice();
  MenuManager_Common_LcdUpdateAdjustMenuStatic();
}

/*=============================================================================================*/
static void MenuManager_StepWashSpeed_LcdShowAdjust(void)
{
  MenuManager_Common_LcdClearAdjustMenuDynamic();

  MenuManager_Common_LcdShowAdjustUnitVal
  (
    MENUMANAGER_STEPWASHSPEED_VALUE_XPOS,
    MENUMANAGER_STEPWASHSPEED_VALUE_YPOS,
    &(MenuManager_StepWashSpeed_Value),
    MENUMANAGER_STEPWASHSPEED_VALUE_LENGTH
  );
  
  MenuManager_Common_LcdShowAdjustArrow
  (
    MENUMANAGER_STEPWASHSPEED_VALUE_XPOS,
    MENUMANAGER_STEPWASHSPEED_VALUE_YPOS,
    (uint32_t)0U
  );

  LCD_SetCursorPos(MENUMANAGER_STEPWASHSPEED_UNIT_XPOS, MENUMANAGER_STEPWASHSPEED_UNIT_YPOS, LCD_CURSOR_BY_FONT);
  LCD_PutString((uint8_t *)MenuManager_Common_LevelStr);

  MenuManager_Common_LcdUpdateAdjustMenuDynamic();
}

/*=============================================================================================*/
static void MenuManager_StepWashSpeed_LcdShowDone(void)
{
  MenuManager_Common_LcdClearAll();

  MenuManager_Common_LcdShowAdjustUnitVal
  (
    MENUMANAGER_STEPWASHSPEED_VALUE_XPOS,
    MENUMANAGER_STEPWASHSPEED_VALUE_YPOS,
    &(MenuManager_StepWashSpeed_Value),
    MENUMANAGER_STEPWASHSPEED_VALUE_LENGTH
  );

  LCD_SetCursorPos(MENUMANAGER_STEPWASHSPEED_UNIT_XPOS, MENUMANAGER_STEPWASHSPEED_UNIT_YPOS, LCD_CURSOR_BY_FONT);
  LCD_PutString((uint8_t *)MenuManager_Common_LevelStr);

  MenuManager_Common_LcdUpdateAll();
}



/*=============================================================================================*/
static Fsm_GuardType MenuManager_StepWashSpeed_Entry(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  MenuManager_Common_ProgramSetupStruct* enterDataHierachy;
  ProgramManager_MotorSpeedType tempWashSpeed;
  HAL_StatusTypeDef retVal = HAL_OK;

  /* Check if previous state data hierachy is not empty */
  if (pFsmContext->dataHierachy != NULL)
  {
    if (pFsmContext->dataHierachy->dataId == MENUMANAGER_STATE_STEP_WASH_MODE_CUSTOM)
    {
      enterDataHierachy = (MenuManager_Common_ProgramSetupStruct *)(pFsmContext->dataHierachy);

      MenuManager_InternalDataPush(MENUMANAGER_STEPWASHSPEED_INTERNALDATALENGTH);

      MenuManager_StepWashSpeed_InternalState = MENUMANAGER_STEPWASHSPEED_INTERNALSTATE_READY;
      MenuManager_StepWashSpeed_Counter = (uint32_t)0U;

      MenuManager_StepWashSpeed_SeqIdx = enterDataHierachy->seqIdx;
      MenuManager_StepWashSpeed_StepIdx = enterDataHierachy->stepIdx;

      ProgramManager_NormStepConfig_WashSpeed_GetData((uint8_t)MenuManager_StepWashSpeed_SeqIdx, (uint8_t)MenuManager_StepWashSpeed_StepIdx, &tempWashSpeed);

      MenuManager_StepWashSpeed_ValueMin = (uint32_t)PROGRAMMANAGER_MOTOR_SPEED_LEVEL_0;
      MenuManager_StepWashSpeed_ValueMax = (uint32_t)PROGRAMMANAGER_MOTOR_SPEED_LEVEL_7;
      MenuManager_StepWashSpeed_Value = (uint32_t)(tempWashSpeed);

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
    MenuManager_StepWashSpeed_LcdShowMainTitle();
    MenuManager_StepWashSpeed_LcdShowAdjust();

    MenuManager_SubMainFunction = MenuManager_StepWashSpeed_SubMainFunction;
    MenuManager_SubTickHandler = MenuManager_StepWashSpeed_SubTickHandler;

    return FSM_GUARD_TRUE;
  }

  return FSM_GUARD_FALSE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_StepWashSpeed_Exit(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  Fsm_DataHierachyStruct* dataHierachy;

  MenuManager_SubMainFunction = NULL;
  MenuManager_SubTickHandler = NULL;

  dataHierachy = (Fsm_DataHierachyStruct *)MenuManager_malloc(sizeof(Fsm_DataHierachyStruct));
  dataHierachy->dataId = MENUMANAGER_STATE_STEP_WASH_SPEED;

  pFsmContext->dataHierachy = dataHierachy;

  /* Free internal data */
  MenuManager_InternalDataPop();
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_StepWashSpeed_StartBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_StepWashSpeed_InternalState == MENUMANAGER_STEPWASHSPEED_INTERNALSTATE_READY)
  {
    MenuManager_StepWashSpeed_InternalState = MENUMANAGER_STEPWASHSPEED_INTERNALSTATE_RUNNING;
  }
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_StepWashSpeed_StopBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_StepWashSpeed_InternalState != MENUMANAGER_STEPWASHSPEED_INTERNALSTATE_READY)
  {
    return FSM_GUARD_FALSE;
  }
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_StepWashSpeed_UpBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_StepWashSpeed_InternalState == MENUMANAGER_STEPWASHSPEED_INTERNALSTATE_READY)
  {
    if (MenuManager_StepWashSpeed_Value < MenuManager_StepWashSpeed_ValueMax)
    {
      MenuManager_StepWashSpeed_Value += (uint32_t)1U;
    }

    MenuManager_StepWashSpeed_LcdShowAdjust();
  }

  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_StepWashSpeed_DownBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_StepWashSpeed_InternalState == MENUMANAGER_STEPWASHSPEED_INTERNALSTATE_READY)
  {
    if (MenuManager_StepWashSpeed_Value > MenuManager_StepWashSpeed_ValueMin)
    {
      MenuManager_StepWashSpeed_Value -= (uint32_t)1U;
    }

    MenuManager_StepWashSpeed_LcdShowAdjust();
  }
  
  return FSM_GUARD_TRUE;
}



/*=============================================================================================*/
static void MenuManager_StepWashSpeed_SubMainFunction(void)
{
  MenuManager_Common_ButEventMapHandler(&MenuManager_StepWashSpeed_ButEventMapConf);

  switch (MenuManager_StepWashSpeed_InternalState)
  {
    case MENUMANAGER_STEPWASHSPEED_INTERNALSTATE_READY:
      break;
    case MENUMANAGER_STEPWASHSPEED_INTERNALSTATE_RUNNING:
    {
      ProgramManager_MotorSpeedType tempWashSpeed;

      tempWashSpeed = (ProgramManager_MotorSpeedType)MenuManager_StepWashSpeed_Value;

      ProgramManager_NormStepConfig_WashSpeed_SetData \
      ( \
        (uint8_t)MenuManager_StepWashSpeed_SeqIdx, \
        (uint8_t)MenuManager_StepWashSpeed_StepIdx, \
        &tempWashSpeed \
      );

      if ((uint8_t)MenuManager_StepWashSpeed_SeqIdx == ProgramManager_gAutoSeqConfig.sequenceIndex)
      {
        ProgramManager_NormStepConfig_WashSpeed_GetData \
        ( \
          (uint8_t)MenuManager_StepWashSpeed_SeqIdx, \
          (uint8_t)MenuManager_StepWashSpeed_StepIdx, \
          &((ProgramManager_gAutoSeqConfig.normStep)[MenuManager_StepWashSpeed_StepIdx].washSpeed) \
        );
      }

      MenuManager_StepWashSpeed_InternalState = MENUMANAGER_STEPWASHSPEED_INTERNALSTATE_DONE;
      
      break;
    }
    case MENUMANAGER_STEPWASHSPEED_INTERNALSTATE_DONE:
    {
      MenuManager_StepWashSpeed_LcdShowDone();

      MenuManager_StepWashSpeed_InternalState = MENUMANAGER_STEPWASHSPEED_INTERNALSTATE_EXITING;

      break;
    }
    case MENUMANAGER_STEPWASHSPEED_INTERNALSTATE_EXITING:
    default:
      break;
  }
}

/*=============================================================================================*/
static void MenuManager_StepWashSpeed_SubTickHandler(void)
{
  if (MenuManager_StepWashSpeed_InternalState == MENUMANAGER_STEPWASHSPEED_INTERNALSTATE_EXITING)
  {
    MenuManager_StepWashSpeed_Counter += (uint32_t)1U;

    if (MenuManager_StepWashSpeed_Counter >= MENUMANAGER_STEPWASHSPEED_COUNTER_MAX)
    {
      MenuManager_StepWashSpeed_Counter = (uint32_t)0U;
      
      Fsm_TriggerEvent(&MenuManager_FsmContext, (Fsm_EventType)MENUMANAGER_EVENT_PREV);
    }
  }
}



#ifdef  __cplusplus
}
#endif
