/* 
 * File:   _1271200000_menu_DrainSetupSpeed.c
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
#define MENUMANAGER_DRAINSETUPSPEED_INTERNALDATALENGTH                (uint8_t)6U

#define MENUMANAGER_DRAINSETUPSPEED_DRAINSTEPIDX_IDX                  0U
#define MENUMANAGER_DRAINSETUPSPEED_INTERNALSTATE_IDX                 1U
#define MENUMANAGER_DRAINSETUPSPEED_COUNTER_IDX                       2U
#define MENUMANAGER_DRAINSETUPSPEED_VALUE_MIN_IDX                     3U
#define MENUMANAGER_DRAINSETUPSPEED_VALUE_MAX_IDX                     4U
#define MENUMANAGER_DRAINSETUPSPEED_VALUE_IDX                         5U

#define MenuManager_DrainSetupSpeed_DrainStepIdx                      MenuManager_GetInternalDataPtr(MENUMANAGER_DRAINSETUPSPEED_DRAINSTEPIDX_IDX)
#define MenuManager_DrainSetupSpeed_InternalState                     MenuManager_GetInternalDataPtr(MENUMANAGER_DRAINSETUPSPEED_INTERNALSTATE_IDX)
#define MenuManager_DrainSetupSpeed_Counter                           MenuManager_GetInternalDataPtr(MENUMANAGER_DRAINSETUPSPEED_COUNTER_IDX)
#define MenuManager_DrainSetupSpeed_ValueMin                          MenuManager_GetInternalDataPtr(MENUMANAGER_DRAINSETUPSPEED_VALUE_MIN_IDX)
#define MenuManager_DrainSetupSpeed_ValueMax                          MenuManager_GetInternalDataPtr(MENUMANAGER_DRAINSETUPSPEED_VALUE_MAX_IDX)
#define MenuManager_DrainSetupSpeed_Value                             MenuManager_GetInternalDataPtr(MENUMANAGER_DRAINSETUPSPEED_VALUE_IDX)



#define MENUMANAGER_DRAINSETUPSPEED_INTERNALSTATE_READY               (uint32_t)0U
#define MENUMANAGER_DRAINSETUPSPEED_INTERNALSTATE_RUNNING             (uint32_t)1U
#define MENUMANAGER_DRAINSETUPSPEED_INTERNALSTATE_DONE                (uint32_t)2U
#define MENUMANAGER_DRAINSETUPSPEED_INTERNALSTATE_EXITING             (uint32_t)3U

#define MENUMANAGER_DRAINSETUPSPEED_COUNTER_MAX                       (uint32_t)50U

#define MENUMANAGER_DRAINSETUPSPEED_VALUE_XPOS                        (uint32_t)10U
#define MENUMANAGER_DRAINSETUPSPEED_VALUE_YPOS                        (uint32_t)5U

#define MENUMANAGER_DRAINSETUPSPEED_VALUE_LENGTH                      (uint32_t)1U

#define MENUMANAGER_DRAINSETUPSPEED_UNIT_XPOS                         (uint32_t)1U
#define MENUMANAGER_DRAINSETUPSPEED_UNIT_YPOS                         (uint32_t)3U



/** Menu manager main titles and child menu titles */
static const uint8_t MenuManager_DrainSetupSpeed_MainTitleStr[] =     "DRAIN SPEED";



/** Menu manager button event mapping array */
static MenuManager_ButEventMapStruct MenuManager_DrainSetupSpeed_ButEventMap[4] =
{
  { IOMANAGER_BUTTON_STATE_START,                                     MENUMANAGER_EVENT_START_BUT             },
  { IOMANAGER_BUTTON_STATE_STOP,                                      MENUMANAGER_EVENT_STOP_BUT              },
  { IOMANAGER_BUTTON_STATE_UP,                                        MENUMANAGER_EVENT_UP_BUT                },
  { IOMANAGER_BUTTON_STATE_DOWN,                                      MENUMANAGER_EVENT_DOWN_BUT              }
};

/** Menu manager button event mapping configuration */
static MenuManager_ButEventMapConfStruct MenuManager_DrainSetupSpeed_ButEventMapConf =
{
  .butEventMapNum             = (uint8_t)4U,
  .butEventMapCfg             = &MenuManager_DrainSetupSpeed_ButEventMap
};



/** Menu manager event handlers */
static Fsm_GuardType MenuManager_DrainSetupSpeed_Entry                (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_DrainSetupSpeed_Exit                 (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_DrainSetupSpeed_StartBut             (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_DrainSetupSpeed_StopBut              (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_DrainSetupSpeed_UpBut                (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_DrainSetupSpeed_DownBut              (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);

/** Menu manager state machine */
Fsm_EventEntryStruct MenuManager_DrainSetupSpeed_StateMachine[7] =
{
  FSM_TRIGGER_ENTRY             (                                     MenuManager_DrainSetupSpeed_Entry                                               ),
  FSM_TRIGGER_EXIT              (                                     MenuManager_DrainSetupSpeed_Exit                                                ),
  FSM_TRIGGER_TRANSITION        ( MENUMANAGER_EVENT_PREV,                                                     MENUMANAGER_STATE_DRAIN_SETUP_PARAM     ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_START_BUT,        MenuManager_DrainSetupSpeed_StartBut                                            ),
  FSM_TRIGGER_TRANSITION_ACTION ( MENUMANAGER_EVENT_STOP_BUT,         MenuManager_DrainSetupSpeed_StopBut,    MENUMANAGER_STATE_DRAIN_SETUP_PARAM     ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_UP_BUT,           MenuManager_DrainSetupSpeed_UpBut                                               ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_DOWN_BUT,         MenuManager_DrainSetupSpeed_DownBut                                             )
};



/*===============================================================================================
*                                   LOCAL FUNCTION PROTOTYPES
===============================================================================================*/

static void MenuManager_DrainSetupSpeed_LcdShowMainTitle(void);
static void MenuManager_DrainSetupSpeed_LcdShowAdjust(void);
static void MenuManager_DrainSetupSpeed_LcdShowDone(void);

static void MenuManager_DrainSetupSpeed_SubMainFunction(void);
static void MenuManager_DrainSetupSpeed_SubTickHandler(void);



/*===============================================================================================
*                                       LOCAL FUNCTIONS
===============================================================================================*/

static void MenuManager_DrainSetupSpeed_LcdShowMainTitle(void)
{
  /* Print main title */
  MenuManager_Common_LcdClearAdjustMenuStatic();
  MenuManager_Common_LcdShowMainTitle(MenuManager_DrainSetupSpeed_MainTitleStr);
  MenuManager_Common_LcdShowOkExitChoice();
  MenuManager_Common_LcdUpdateAdjustMenuStatic();
}

/*=============================================================================================*/
static void MenuManager_DrainSetupSpeed_LcdShowAdjust(void)
{
  MenuManager_Common_LcdClearAdjustMenuDynamic();

  MenuManager_Common_LcdShowAdjustUnitVal
  (
    MENUMANAGER_DRAINSETUPSPEED_VALUE_XPOS,
    MENUMANAGER_DRAINSETUPSPEED_VALUE_YPOS,
    &(MenuManager_DrainSetupSpeed_Value),
    MENUMANAGER_DRAINSETUPSPEED_VALUE_LENGTH
  );
  
  MenuManager_Common_LcdShowAdjustArrow
  (
    MENUMANAGER_DRAINSETUPSPEED_VALUE_XPOS,
    MENUMANAGER_DRAINSETUPSPEED_VALUE_YPOS,
    (uint32_t)0U
  );

  LCD_SetCursorPos(MENUMANAGER_DRAINSETUPSPEED_UNIT_XPOS, MENUMANAGER_DRAINSETUPSPEED_UNIT_YPOS, LCD_CURSOR_BY_FONT);
  LCD_PutString((uint8_t *)MenuManager_Common_LevelStr);

  MenuManager_Common_LcdUpdateAdjustMenuDynamic();
}

/*=============================================================================================*/
static void MenuManager_DrainSetupSpeed_LcdShowDone(void)
{
  MenuManager_Common_LcdClearAll();

  MenuManager_Common_LcdShowAdjustUnitVal
  (
    MENUMANAGER_DRAINSETUPSPEED_VALUE_XPOS,
    MENUMANAGER_DRAINSETUPSPEED_VALUE_YPOS,
    &(MenuManager_DrainSetupSpeed_Value),
    MENUMANAGER_DRAINSETUPSPEED_VALUE_LENGTH
  );

  LCD_SetCursorPos(MENUMANAGER_DRAINSETUPSPEED_UNIT_XPOS, MENUMANAGER_DRAINSETUPSPEED_UNIT_YPOS, LCD_CURSOR_BY_FONT);
  LCD_PutString((uint8_t *)MenuManager_Common_LevelStr);

  MenuManager_Common_LcdUpdateAll();
}



/*=============================================================================================*/
static Fsm_GuardType MenuManager_DrainSetupSpeed_Entry(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  MenuManager_Common_DrainSetupStruct* enterDataHierachy;
  HAL_StatusTypeDef retVal = HAL_OK;

  /* Check if previous state data hierachy is not empty */
  if (pFsmContext->dataHierachy != NULL)
  {
    if (pFsmContext->dataHierachy->dataId == MENUMANAGER_STATE_DRAIN_SETUP)
    {
      enterDataHierachy = (MenuManager_Common_DrainSetupStruct *)(pFsmContext->dataHierachy);

      MenuManager_InternalDataPush(MENUMANAGER_DRAINSETUPSPEED_INTERNALDATALENGTH);

      MenuManager_DrainSetupSpeed_DrainStepIdx = enterDataHierachy->drainStepIdx;

      MenuManager_DrainSetupSpeed_InternalState = MENUMANAGER_DRAINSETUPSPEED_INTERNALSTATE_READY;
      MenuManager_DrainSetupSpeed_Counter = (uint32_t)0U;

      MenuManager_DrainSetupSpeed_ValueMin = (uint32_t)PROGRAMMANAGER_MOTOR_SPEED_LEVEL_0;
      MenuManager_DrainSetupSpeed_ValueMax = (uint32_t)(ProgramManager_gParamConfig.drainCfg.maxDrainExtrSpeed);
      MenuManager_DrainSetupSpeed_Value = (uint32_t)(ProgramManager_gParamConfig.drainCfg.drainStepCfg[MenuManager_DrainSetupSpeed_DrainStepIdx].drainSpeed);

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
    MenuManager_DrainSetupSpeed_LcdShowMainTitle();
    MenuManager_DrainSetupSpeed_LcdShowAdjust();

    MenuManager_SubMainFunction = MenuManager_DrainSetupSpeed_SubMainFunction;
    MenuManager_SubTickHandler = MenuManager_DrainSetupSpeed_SubTickHandler;

    return FSM_GUARD_TRUE;
  }

  return FSM_GUARD_FALSE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_DrainSetupSpeed_Exit(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  Fsm_DataHierachyStruct* dataHierachy;

  MenuManager_SubMainFunction = NULL;
  MenuManager_SubTickHandler = NULL;

  dataHierachy = (Fsm_DataHierachyStruct *)MenuManager_malloc(sizeof(Fsm_DataHierachyStruct));
  dataHierachy->dataId = MENUMANAGER_STATE_DRAIN_SETUP_SPEED;

  pFsmContext->dataHierachy = dataHierachy;

  /* Free internal data */
  MenuManager_InternalDataPop();
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_DrainSetupSpeed_StartBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_DrainSetupSpeed_InternalState == MENUMANAGER_DRAINSETUPSPEED_INTERNALSTATE_READY)
  {
    MenuManager_DrainSetupSpeed_InternalState = MENUMANAGER_DRAINSETUPSPEED_INTERNALSTATE_RUNNING;
  }
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_DrainSetupSpeed_StopBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_DrainSetupSpeed_InternalState != MENUMANAGER_DRAINSETUPSPEED_INTERNALSTATE_READY)
  {
    return FSM_GUARD_FALSE;
  }
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_DrainSetupSpeed_UpBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_DrainSetupSpeed_InternalState == MENUMANAGER_DRAINSETUPSPEED_INTERNALSTATE_READY)
  {
    if (MenuManager_DrainSetupSpeed_Value < MenuManager_DrainSetupSpeed_ValueMax)
    {
      MenuManager_DrainSetupSpeed_Value += (uint32_t)1U;
    }

    MenuManager_DrainSetupSpeed_LcdShowAdjust();
  }

  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_DrainSetupSpeed_DownBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_DrainSetupSpeed_InternalState == MENUMANAGER_DRAINSETUPSPEED_INTERNALSTATE_READY)
  {
    if (MenuManager_DrainSetupSpeed_Value > MenuManager_DrainSetupSpeed_ValueMin)
    {
      MenuManager_DrainSetupSpeed_Value -= (uint32_t)1U;
    }

    MenuManager_DrainSetupSpeed_LcdShowAdjust();
  }
  
  return FSM_GUARD_TRUE;
}



/*=============================================================================================*/
static void MenuManager_DrainSetupSpeed_SubMainFunction(void)
{
  MenuManager_Common_ButEventMapHandler(&MenuManager_DrainSetupSpeed_ButEventMapConf);

  switch (MenuManager_DrainSetupSpeed_InternalState)
  {
    case MENUMANAGER_DRAINSETUPSPEED_INTERNALSTATE_READY:
      break;
    case MENUMANAGER_DRAINSETUPSPEED_INTERNALSTATE_RUNNING:
    {
      ProgramManager_MotorSpeedType tempDrainSetupSpeed;

      tempDrainSetupSpeed = (ProgramManager_MotorSpeedType)MenuManager_DrainSetupSpeed_Value;

      ProgramManager_DrainSetup_DrainSpeed_SetData((uint8_t)MenuManager_DrainSetupSpeed_DrainStepIdx, &tempDrainSetupSpeed);

      ProgramManager_DrainSetup_DrainSpeed_GetData((uint8_t)MenuManager_DrainSetupSpeed_DrainStepIdx, \
                                                   &(ProgramManager_gParamConfig.drainCfg.drainStepCfg[MenuManager_DrainSetupSpeed_DrainStepIdx].drainSpeed));
      
      MenuManager_DrainSetupSpeed_InternalState = MENUMANAGER_DRAINSETUPSPEED_INTERNALSTATE_DONE;
      
      break;
    }
    case MENUMANAGER_DRAINSETUPSPEED_INTERNALSTATE_DONE:
    {
      MenuManager_DrainSetupSpeed_LcdShowDone();

      MenuManager_DrainSetupSpeed_InternalState = MENUMANAGER_DRAINSETUPSPEED_INTERNALSTATE_EXITING;

      break;
    }
    case MENUMANAGER_DRAINSETUPSPEED_INTERNALSTATE_EXITING:
    default:
      break;
  }
}

/*=============================================================================================*/
static void MenuManager_DrainSetupSpeed_SubTickHandler(void)
{
  if (MenuManager_DrainSetupSpeed_InternalState == MENUMANAGER_DRAINSETUPSPEED_INTERNALSTATE_EXITING)
  {
    MenuManager_DrainSetupSpeed_Counter += (uint32_t)1U;

    if (MenuManager_DrainSetupSpeed_Counter >= MENUMANAGER_DRAINSETUPSPEED_COUNTER_MAX)
    {
      MenuManager_DrainSetupSpeed_Counter = (uint32_t)0U;
      
      Fsm_TriggerEvent(&MenuManager_FsmContext, (Fsm_EventType)MENUMANAGER_EVENT_PREV);
    }
  }
}



#ifdef  __cplusplus
}
#endif
