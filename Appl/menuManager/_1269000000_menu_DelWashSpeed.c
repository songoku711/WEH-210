/* 
 * File:   _1269000000_menu_DelWashSpeed.c
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
#define MENUMANAGER_DELWASHSPEED_INTERNALDATALENGTH                   (uint8_t)5U

#define MENUMANAGER_DELWASHSPEED_INTERNALSTATE_IDX                    0U
#define MENUMANAGER_DELWASHSPEED_COUNTER_IDX                          1U
#define MENUMANAGER_DELWASHSPEED_VALUE_MIN_IDX                        2U
#define MENUMANAGER_DELWASHSPEED_VALUE_MAX_IDX                        3U
#define MENUMANAGER_DELWASHSPEED_VALUE_IDX                            4U

#define MenuManager_DelWashSpeed_InternalState                        MenuManager_GetInternalDataPtr(MENUMANAGER_DELWASHSPEED_INTERNALSTATE_IDX)
#define MenuManager_DelWashSpeed_Counter                              MenuManager_GetInternalDataPtr(MENUMANAGER_DELWASHSPEED_COUNTER_IDX)
#define MenuManager_DelWashSpeed_ValueMin                             MenuManager_GetInternalDataPtr(MENUMANAGER_DELWASHSPEED_VALUE_MIN_IDX)
#define MenuManager_DelWashSpeed_ValueMax                             MenuManager_GetInternalDataPtr(MENUMANAGER_DELWASHSPEED_VALUE_MAX_IDX)
#define MenuManager_DelWashSpeed_Value                                MenuManager_GetInternalDataPtr(MENUMANAGER_DELWASHSPEED_VALUE_IDX)



#define MENUMANAGER_DELWASHSPEED_INTERNALSTATE_READY                  (uint32_t)0U
#define MENUMANAGER_DELWASHSPEED_INTERNALSTATE_RUNNING                (uint32_t)1U
#define MENUMANAGER_DELWASHSPEED_INTERNALSTATE_DONE                   (uint32_t)2U
#define MENUMANAGER_DELWASHSPEED_INTERNALSTATE_EXITING                (uint32_t)3U

#define MENUMANAGER_DELWASHSPEED_COUNTER_MAX                          (uint32_t)50U

#define MENUMANAGER_DELWASHSPEED_VALUE_XPOS                           (uint32_t)10U
#define MENUMANAGER_DELWASHSPEED_VALUE_YPOS                           (uint32_t)5U

#define MENUMANAGER_DELWASHSPEED_VALUE_LENGTH                         (uint32_t)1U

#define MENUMANAGER_DELWASHSPEED_UNIT_XPOS                            (uint32_t)1U
#define MENUMANAGER_DELWASHSPEED_UNIT_YPOS                            (uint32_t)3U



/** Menu manager main titles and child menu titles */
static const uint8_t MenuManager_DelWashSpeed_MainTitleStr[] =        "DEL WASH SPEED";



/** Menu manager button event mapping array */
static MenuManager_ButEventMapStruct MenuManager_DelWashSpeed_ButEventMap[4] =
{
  { IOMANAGER_BUTTON_STATE_START,                                     MENUMANAGER_EVENT_START_BUT             },
  { IOMANAGER_BUTTON_STATE_STOP,                                      MENUMANAGER_EVENT_STOP_BUT              },
  { IOMANAGER_BUTTON_STATE_UP,                                        MENUMANAGER_EVENT_UP_BUT                },
  { IOMANAGER_BUTTON_STATE_DOWN,                                      MENUMANAGER_EVENT_DOWN_BUT              }
};

/** Menu manager button event mapping configuration */
static MenuManager_ButEventMapConfStruct MenuManager_DelWashSpeed_ButEventMapConf =
{
  .butEventMapNum             = (uint8_t)4U,
  .butEventMapCfg             = &MenuManager_DelWashSpeed_ButEventMap
};



/** Menu manager event handlers */
static Fsm_GuardType MenuManager_DelWashSpeed_Entry                       (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_DelWashSpeed_Exit                        (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_DelWashSpeed_Submenu1                    (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_DelWashSpeed_StartBut                    (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_DelWashSpeed_StopBut                     (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_DelWashSpeed_UpBut                       (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_DelWashSpeed_DownBut                     (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);

/** Menu manager state machine */
Fsm_EventEntryStruct MenuManager_DelWashSpeed_StateMachine[7] =
{
  FSM_TRIGGER_ENTRY             (                                     MenuManager_DelWashSpeed_Entry                                                  ),
  FSM_TRIGGER_EXIT              (                                     MenuManager_DelWashSpeed_Exit                                                   ),
  FSM_TRIGGER_TRANSITION_ACTION ( MENUMANAGER_EVENT_SUBMENU_1,        MenuManager_DelWashSpeed_Submenu1,      MENUMANAGER_STATE_WASH_SETUP            ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_START_BUT,        MenuManager_DelWashSpeed_StartBut                                               ),
  FSM_TRIGGER_TRANSITION_ACTION ( MENUMANAGER_EVENT_STOP_BUT,         MenuManager_DelWashSpeed_StopBut,       MENUMANAGER_STATE_WASH_SETUP            ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_UP_BUT,           MenuManager_DelWashSpeed_UpBut                                                  ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_DOWN_BUT,         MenuManager_DelWashSpeed_DownBut                                                )
};



/*===============================================================================================
*                                   LOCAL FUNCTION PROTOTYPES
===============================================================================================*/

static void MenuManager_DelWashSpeed_LcdShowMainTitle(void);
static void MenuManager_DelWashSpeed_LcdShowAdjust(void);
static void MenuManager_DelWashSpeed_LcdShowDone(void);

static void MenuManager_DelWashSpeed_SubMainFunction(void);
static void MenuManager_DelWashSpeed_SubTickHandler(void);



/*===============================================================================================
*                                       LOCAL FUNCTIONS
===============================================================================================*/

static void MenuManager_DelWashSpeed_LcdShowMainTitle(void)
{
  /* Print main title */
  MenuManager_Common_LcdClearAdjustMenuStatic();
  MenuManager_Common_LcdShowMainTitle(MenuManager_DelWashSpeed_MainTitleStr);
  MenuManager_Common_LcdShowOkExitChoice();
  MenuManager_Common_LcdUpdateAdjustMenuStatic();
}

/*=============================================================================================*/
static void MenuManager_DelWashSpeed_LcdShowAdjust(void)
{
  MenuManager_Common_LcdClearAdjustMenuDynamic();

  MenuManager_Common_LcdShowAdjustUnitVal
  (
    MENUMANAGER_DELWASHSPEED_VALUE_XPOS,
    MENUMANAGER_DELWASHSPEED_VALUE_YPOS,
    &(MenuManager_DelWashSpeed_Value),
    MENUMANAGER_DELWASHSPEED_VALUE_LENGTH
  );
  
  MenuManager_Common_LcdShowAdjustArrow
  (
    MENUMANAGER_DELWASHSPEED_VALUE_XPOS,
    MENUMANAGER_DELWASHSPEED_VALUE_YPOS,
    (uint32_t)0U
  );

  LCD_SetCursorPos(MENUMANAGER_DELWASHSPEED_UNIT_XPOS, MENUMANAGER_DELWASHSPEED_UNIT_YPOS, LCD_CURSOR_BY_FONT);
  LCD_PutString((uint8_t *)MenuManager_Common_LevelStr);

  MenuManager_Common_LcdUpdateAdjustMenuDynamic();
}

/*=============================================================================================*/
static void MenuManager_DelWashSpeed_LcdShowDone(void)
{
  MenuManager_Common_LcdClearAll();

  MenuManager_Common_LcdShowAdjustUnitVal
  (
    MENUMANAGER_DELWASHSPEED_VALUE_XPOS,
    MENUMANAGER_DELWASHSPEED_VALUE_YPOS,
    &(MenuManager_DelWashSpeed_Value),
    MENUMANAGER_DELWASHSPEED_VALUE_LENGTH
  );

  LCD_SetCursorPos(MENUMANAGER_DELWASHSPEED_UNIT_XPOS, MENUMANAGER_DELWASHSPEED_UNIT_YPOS, LCD_CURSOR_BY_FONT);
  LCD_PutString((uint8_t *)MenuManager_Common_LevelStr);

  MenuManager_Common_LcdUpdateAll();
}



/*=============================================================================================*/
static Fsm_GuardType MenuManager_DelWashSpeed_Entry(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  MenuManager_SubMainFunction = MenuManager_DelWashSpeed_SubMainFunction;
  MenuManager_SubTickHandler = MenuManager_DelWashSpeed_SubTickHandler;

  /* Check if previous state data hierachy is not empty */
  if (pFsmContext->dataHierachy != NULL)
  {
    if (pFsmContext->dataHierachy->dataId == MENUMANAGER_STATE_WASH_SETUP)
    {
      /* Release previous state data hierachy */
      MenuManager_free(pFsmContext->dataHierachy);
      pFsmContext->dataHierachy = NULL;

      MenuManager_InternalDataPush(MENUMANAGER_DELWASHSPEED_INTERNALDATALENGTH);

      MenuManager_DelWashSpeed_InternalState = MENUMANAGER_DELWASHSPEED_INTERNALSTATE_READY;
      MenuManager_DelWashSpeed_Counter = (uint32_t)0U;

      MenuManager_DelWashSpeed_ValueMin = (uint32_t)PROGRAMMANAGER_MOTOR_SPEED_LEVEL_0;
      MenuManager_DelWashSpeed_ValueMax = (uint32_t)(ProgramManager_gParamConfig.washCfg.maxWashSpeed);
      MenuManager_DelWashSpeed_Value = (uint32_t)(ProgramManager_gParamConfig.washCfg.delWashSpeed);
    }
    else
    {
      return FSM_GUARD_FALSE;
    }

    MenuManager_DelWashSpeed_LcdShowMainTitle();
    MenuManager_DelWashSpeed_LcdShowAdjust();

    return FSM_GUARD_TRUE;
  }

  return FSM_GUARD_FALSE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_DelWashSpeed_Exit(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  MenuManager_SubMainFunction = NULL;
  MenuManager_SubTickHandler = NULL;

  Fsm_DataHierachyStruct* dataHierachy;

  dataHierachy = (Fsm_DataHierachyStruct *)MenuManager_malloc(sizeof(Fsm_DataHierachyStruct));
  dataHierachy->dataId = MENUMANAGER_STATE_DEL_WASH_SPEED;

  pFsmContext->dataHierachy = dataHierachy;

  /* Free internal data */
  MenuManager_InternalDataPop();
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_DelWashSpeed_Submenu1(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_DelWashSpeed_StartBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_DelWashSpeed_InternalState == MENUMANAGER_DELWASHSPEED_INTERNALSTATE_READY)
  {
    MenuManager_DelWashSpeed_InternalState = MENUMANAGER_DELWASHSPEED_INTERNALSTATE_RUNNING;
  }
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_DelWashSpeed_StopBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_DelWashSpeed_InternalState != MENUMANAGER_DELWASHSPEED_INTERNALSTATE_READY)
  {
    return FSM_GUARD_FALSE;
  }
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_DelWashSpeed_UpBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_DelWashSpeed_InternalState == MENUMANAGER_DELWASHSPEED_INTERNALSTATE_READY)
  {
    if (MenuManager_DelWashSpeed_Value < MenuManager_DelWashSpeed_ValueMax)
    {
      MenuManager_DelWashSpeed_Value += (uint32_t)1U;
    }

    MenuManager_DelWashSpeed_LcdShowAdjust();
  }

  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_DelWashSpeed_DownBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_DelWashSpeed_InternalState == MENUMANAGER_DELWASHSPEED_INTERNALSTATE_READY)
  {
    if (MenuManager_DelWashSpeed_Value > MenuManager_DelWashSpeed_ValueMin)
    {
      MenuManager_DelWashSpeed_Value -= (uint32_t)1U;
    }

    MenuManager_DelWashSpeed_LcdShowAdjust();
  }
  
  return FSM_GUARD_TRUE;
}



/*=============================================================================================*/
static void MenuManager_DelWashSpeed_SubMainFunction(void)
{
  MenuManager_Common_ButEventMapHandler(&MenuManager_DelWashSpeed_ButEventMapConf);

  switch (MenuManager_DelWashSpeed_InternalState)
  {
    case MENUMANAGER_DELWASHSPEED_INTERNALSTATE_READY:
      break;
    case MENUMANAGER_DELWASHSPEED_INTERNALSTATE_RUNNING:
    {
      ProgramManager_MotorSpeedType tempDelWashSpeed;

      tempDelWashSpeed = (ProgramManager_MotorSpeedType)MenuManager_DelWashSpeed_Value;

      ProgramManager_WashSetup_DelWashSpeed_SetData(&tempDelWashSpeed);

      ProgramManager_WashSetup_DelWashSpeed_GetData(&(ProgramManager_gParamConfig.washCfg.delWashSpeed));

      MenuManager_DelWashSpeed_InternalState = MENUMANAGER_DELWASHSPEED_INTERNALSTATE_DONE;
      
      break;
    }
    case MENUMANAGER_DELWASHSPEED_INTERNALSTATE_DONE:
    {
      MenuManager_DelWashSpeed_LcdShowDone();

      MenuManager_DelWashSpeed_InternalState = MENUMANAGER_DELWASHSPEED_INTERNALSTATE_EXITING;

      break;
    }
    case MENUMANAGER_DELWASHSPEED_INTERNALSTATE_EXITING:
    default:
      break;
  }
}

/*=============================================================================================*/
static void MenuManager_DelWashSpeed_SubTickHandler(void)
{
  if (MenuManager_DelWashSpeed_InternalState == MENUMANAGER_DELWASHSPEED_INTERNALSTATE_EXITING)
  {
    MenuManager_DelWashSpeed_Counter += (uint32_t)1U;

    if (MenuManager_DelWashSpeed_Counter >= MENUMANAGER_DELWASHSPEED_COUNTER_MAX)
    {
      MenuManager_DelWashSpeed_Counter = (uint32_t)0U;
      
      Fsm_TriggerEvent(&MenuManager_FsmContext, (Fsm_EventType)MENUMANAGER_EVENT_SUBMENU_1);
    }
  }
}



#ifdef  __cplusplus
}
#endif
