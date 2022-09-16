/* 
 * File:   _126A000000_menu_HvyWashSpeed.c
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
#define MENUMANAGER_HVYWASHSPEED_INTERNALDATALENGTH                   (uint8_t)5U

#define MENUMANAGER_HVYWASHSPEED_INTERNALSTATE_IDX                    0U
#define MENUMANAGER_HVYWASHSPEED_COUNTER_IDX                          1U
#define MENUMANAGER_HVYWASHSPEED_VALUE_MIN_IDX                        2U
#define MENUMANAGER_HVYWASHSPEED_VALUE_MAX_IDX                        3U
#define MENUMANAGER_HVYWASHSPEED_VALUE_IDX                            4U

#define MenuManager_HvyWashSpeed_InternalState                        MenuManager_GetInternalDataPtr(MENUMANAGER_HVYWASHSPEED_INTERNALSTATE_IDX)
#define MenuManager_HvyWashSpeed_Counter                              MenuManager_GetInternalDataPtr(MENUMANAGER_HVYWASHSPEED_COUNTER_IDX)
#define MenuManager_HvyWashSpeed_ValueMin                             MenuManager_GetInternalDataPtr(MENUMANAGER_HVYWASHSPEED_VALUE_MIN_IDX)
#define MenuManager_HvyWashSpeed_ValueMax                             MenuManager_GetInternalDataPtr(MENUMANAGER_HVYWASHSPEED_VALUE_MAX_IDX)
#define MenuManager_HvyWashSpeed_Value                                MenuManager_GetInternalDataPtr(MENUMANAGER_HVYWASHSPEED_VALUE_IDX)



#define MENUMANAGER_HVYWASHSPEED_INTERNALSTATE_READY                  (uint32_t)0U
#define MENUMANAGER_HVYWASHSPEED_INTERNALSTATE_RUNNING                (uint32_t)1U
#define MENUMANAGER_HVYWASHSPEED_INTERNALSTATE_DONE                   (uint32_t)2U
#define MENUMANAGER_HVYWASHSPEED_INTERNALSTATE_EXITING                (uint32_t)3U

#define MENUMANAGER_HVYWASHSPEED_COUNTER_MAX                          (uint32_t)50U

#define MENUMANAGER_HVYWASHSPEED_VALUE_XPOS                           (uint32_t)10U
#define MENUMANAGER_HVYWASHSPEED_VALUE_YPOS                           (uint32_t)5U

#define MENUMANAGER_HVYWASHSPEED_VALUE_LENGTH                         (uint32_t)1U

#define MENUMANAGER_HVYWASHSPEED_UNIT_XPOS                            (uint32_t)1U
#define MENUMANAGER_HVYWASHSPEED_UNIT_YPOS                            (uint32_t)3U



/** Menu manager main titles and child menu titles */
static const uint8_t MenuManager_HvyWashSpeed_MainTitleStr[] =        "HVY WASH SPEED";



/** Menu manager button event mapping array */
static MenuManager_ButEventMapStruct MenuManager_HvyWashSpeed_ButEventMap[4] =
{
  { IOMANAGER_BUTTON_STATE_START,                                     MENUMANAGER_EVENT_START_BUT             },
  { IOMANAGER_BUTTON_STATE_STOP,                                      MENUMANAGER_EVENT_STOP_BUT              },
  { IOMANAGER_BUTTON_STATE_UP,                                        MENUMANAGER_EVENT_UP_BUT                },
  { IOMANAGER_BUTTON_STATE_DOWN,                                      MENUMANAGER_EVENT_DOWN_BUT              }
};

/** Menu manager button event mapping configuration */
static MenuManager_ButEventMapConfStruct MenuManager_HvyWashSpeed_ButEventMapConf =
{
  .butEventMapNum             = (uint8_t)4U,
  .butEventMapCfg             = &MenuManager_HvyWashSpeed_ButEventMap
};



/** Menu manager event handlers */
static Fsm_GuardType MenuManager_HvyWashSpeed_Entry                   (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_HvyWashSpeed_Exit                    (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_HvyWashSpeed_StartBut                (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_HvyWashSpeed_StopBut                 (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_HvyWashSpeed_UpBut                   (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_HvyWashSpeed_DownBut                 (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);

/** Menu manager state machine */
Fsm_EventEntryStruct MenuManager_HvyWashSpeed_StateMachine[7] =
{
  FSM_TRIGGER_ENTRY             (                                     MenuManager_HvyWashSpeed_Entry                                                  ),
  FSM_TRIGGER_EXIT              (                                     MenuManager_HvyWashSpeed_Exit                                                   ),
  FSM_TRIGGER_TRANSITION        ( MENUMANAGER_EVENT_PREV,                                                     MENUMANAGER_STATE_WASH_SETUP            ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_START_BUT,        MenuManager_HvyWashSpeed_StartBut                                               ),
  FSM_TRIGGER_TRANSITION_ACTION ( MENUMANAGER_EVENT_STOP_BUT,         MenuManager_HvyWashSpeed_StopBut,       MENUMANAGER_STATE_WASH_SETUP            ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_UP_BUT,           MenuManager_HvyWashSpeed_UpBut                                                  ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_DOWN_BUT,         MenuManager_HvyWashSpeed_DownBut                                                )
};



/*===============================================================================================
*                                   LOCAL FUNCTION PROTOTYPES
===============================================================================================*/

static void MenuManager_HvyWashSpeed_LcdShowMainTitle(void);
static void MenuManager_HvyWashSpeed_LcdShowAdjust(void);
static void MenuManager_HvyWashSpeed_LcdShowDone(void);

static void MenuManager_HvyWashSpeed_SubMainFunction(void);
static void MenuManager_HvyWashSpeed_SubTickHandler(void);



/*===============================================================================================
*                                       LOCAL FUNCTIONS
===============================================================================================*/

static void MenuManager_HvyWashSpeed_LcdShowMainTitle(void)
{
  /* Print main title */
  MenuManager_Common_LcdClearAdjustMenuStatic();
  MenuManager_Common_LcdShowMainTitle(MenuManager_HvyWashSpeed_MainTitleStr);
  MenuManager_Common_LcdShowOkExitChoice();
  MenuManager_Common_LcdUpdateAdjustMenuStatic();
}

/*=============================================================================================*/
static void MenuManager_HvyWashSpeed_LcdShowAdjust(void)
{
  MenuManager_Common_LcdClearAdjustMenuDynamic();

  MenuManager_Common_LcdShowAdjustUnitVal
  (
    MENUMANAGER_HVYWASHSPEED_VALUE_XPOS,
    MENUMANAGER_HVYWASHSPEED_VALUE_YPOS,
    &(MenuManager_HvyWashSpeed_Value),
    MENUMANAGER_HVYWASHSPEED_VALUE_LENGTH
  );
  
  MenuManager_Common_LcdShowAdjustArrow
  (
    MENUMANAGER_HVYWASHSPEED_VALUE_XPOS,
    MENUMANAGER_HVYWASHSPEED_VALUE_YPOS,
    (uint32_t)0U
  );

  LCD_SetCursorPos(MENUMANAGER_HVYWASHSPEED_UNIT_XPOS, MENUMANAGER_HVYWASHSPEED_UNIT_YPOS, LCD_CURSOR_BY_FONT);
  LCD_PutString((uint8_t *)MenuManager_Common_LevelStr);

  MenuManager_Common_LcdUpdateAdjustMenuDynamic();
}

/*=============================================================================================*/
static void MenuManager_HvyWashSpeed_LcdShowDone(void)
{
  MenuManager_Common_LcdClearAll();

  MenuManager_Common_LcdShowAdjustUnitVal
  (
    MENUMANAGER_HVYWASHSPEED_VALUE_XPOS,
    MENUMANAGER_HVYWASHSPEED_VALUE_YPOS,
    &(MenuManager_HvyWashSpeed_Value),
    MENUMANAGER_HVYWASHSPEED_VALUE_LENGTH
  );

  LCD_SetCursorPos(MENUMANAGER_HVYWASHSPEED_UNIT_XPOS, MENUMANAGER_HVYWASHSPEED_UNIT_YPOS, LCD_CURSOR_BY_FONT);
  LCD_PutString((uint8_t *)MenuManager_Common_LevelStr);

  MenuManager_Common_LcdUpdateAll();
}



/*=============================================================================================*/
static Fsm_GuardType MenuManager_HvyWashSpeed_Entry(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  HAL_StatusTypeDef retVal = HAL_OK;

  /* Check if previous state data hierachy is not empty */
  if (pFsmContext->dataHierachy != NULL)
  {
    if (pFsmContext->dataHierachy->dataId == MENUMANAGER_STATE_WASH_SETUP)
    {
      /* Release previous state data hierachy */
      MenuManager_free(pFsmContext->dataHierachy);
      pFsmContext->dataHierachy = NULL;

      MenuManager_InternalDataPush(MENUMANAGER_HVYWASHSPEED_INTERNALDATALENGTH);

      MenuManager_HvyWashSpeed_InternalState = MENUMANAGER_HVYWASHSPEED_INTERNALSTATE_READY;
      MenuManager_HvyWashSpeed_Counter = (uint32_t)0U;

      MenuManager_HvyWashSpeed_ValueMin = (uint32_t)PROGRAMMANAGER_MOTOR_SPEED_LEVEL_0;
      MenuManager_HvyWashSpeed_ValueMax = (uint32_t)(ProgramManager_gParamConfig.washCfg.maxWashSpeed);
      MenuManager_HvyWashSpeed_Value = (uint32_t)(ProgramManager_gParamConfig.washCfg.hvyWashSpeed);
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
    MenuManager_HvyWashSpeed_LcdShowMainTitle();
    MenuManager_HvyWashSpeed_LcdShowAdjust();

    MenuManager_SubMainFunction = MenuManager_HvyWashSpeed_SubMainFunction;
    MenuManager_SubTickHandler = MenuManager_HvyWashSpeed_SubTickHandler;

    return FSM_GUARD_TRUE;
  }

  return FSM_GUARD_FALSE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_HvyWashSpeed_Exit(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  Fsm_DataHierachyStruct* dataHierachy;

  MenuManager_SubMainFunction = NULL;
  MenuManager_SubTickHandler = NULL;

  dataHierachy = (Fsm_DataHierachyStruct *)MenuManager_malloc(sizeof(Fsm_DataHierachyStruct));
  dataHierachy->dataId = MENUMANAGER_STATE_HVY_WASH_SPEED;

  pFsmContext->dataHierachy = dataHierachy;

  /* Free internal data */
  MenuManager_InternalDataPop();
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_HvyWashSpeed_StartBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_HvyWashSpeed_InternalState == MENUMANAGER_HVYWASHSPEED_INTERNALSTATE_READY)
  {
    MenuManager_HvyWashSpeed_InternalState = MENUMANAGER_HVYWASHSPEED_INTERNALSTATE_RUNNING;
  }
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_HvyWashSpeed_StopBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_HvyWashSpeed_InternalState != MENUMANAGER_HVYWASHSPEED_INTERNALSTATE_READY)
  {
    return FSM_GUARD_FALSE;
  }
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_HvyWashSpeed_UpBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_HvyWashSpeed_InternalState == MENUMANAGER_HVYWASHSPEED_INTERNALSTATE_READY)
  {
    if (MenuManager_HvyWashSpeed_Value < MenuManager_HvyWashSpeed_ValueMax)
    {
      MenuManager_HvyWashSpeed_Value += (uint32_t)1U;
    }

    MenuManager_HvyWashSpeed_LcdShowAdjust();
  }

  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_HvyWashSpeed_DownBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_HvyWashSpeed_InternalState == MENUMANAGER_HVYWASHSPEED_INTERNALSTATE_READY)
  {
    if (MenuManager_HvyWashSpeed_Value > MenuManager_HvyWashSpeed_ValueMin)
    {
      MenuManager_HvyWashSpeed_Value -= (uint32_t)1U;
    }

    MenuManager_HvyWashSpeed_LcdShowAdjust();
  }
  
  return FSM_GUARD_TRUE;
}



/*=============================================================================================*/
static void MenuManager_HvyWashSpeed_SubMainFunction(void)
{
  MenuManager_Common_ButEventMapHandler(&MenuManager_HvyWashSpeed_ButEventMapConf);

  switch (MenuManager_HvyWashSpeed_InternalState)
  {
    case MENUMANAGER_HVYWASHSPEED_INTERNALSTATE_READY:
      break;
    case MENUMANAGER_HVYWASHSPEED_INTERNALSTATE_RUNNING:
    {
      ProgramManager_MotorSpeedType tempHvyWashSpeed;

      tempHvyWashSpeed = (ProgramManager_MotorSpeedType)MenuManager_HvyWashSpeed_Value;

      ProgramManager_WashSetup_HvyWashSpeed_SetData(&tempHvyWashSpeed);

      ProgramManager_WashSetup_HvyWashSpeed_GetData(&(ProgramManager_gParamConfig.washCfg.hvyWashSpeed));

      MenuManager_HvyWashSpeed_InternalState = MENUMANAGER_HVYWASHSPEED_INTERNALSTATE_DONE;
      
      break;
    }
    case MENUMANAGER_HVYWASHSPEED_INTERNALSTATE_DONE:
    {
      MenuManager_HvyWashSpeed_LcdShowDone();

      MenuManager_HvyWashSpeed_InternalState = MENUMANAGER_HVYWASHSPEED_INTERNALSTATE_EXITING;

      break;
    }
    case MENUMANAGER_HVYWASHSPEED_INTERNALSTATE_EXITING:
    default:
      break;
  }
}

/*=============================================================================================*/
static void MenuManager_HvyWashSpeed_SubTickHandler(void)
{
  if (MenuManager_HvyWashSpeed_InternalState == MENUMANAGER_HVYWASHSPEED_INTERNALSTATE_EXITING)
  {
    MenuManager_HvyWashSpeed_Counter += (uint32_t)1U;

    if (MenuManager_HvyWashSpeed_Counter >= MENUMANAGER_HVYWASHSPEED_COUNTER_MAX)
    {
      MenuManager_HvyWashSpeed_Counter = (uint32_t)0U;
      
      Fsm_TriggerEvent(&MenuManager_FsmContext, (Fsm_EventType)MENUMANAGER_EVENT_PREV);
    }
  }
}



#ifdef  __cplusplus
}
#endif
