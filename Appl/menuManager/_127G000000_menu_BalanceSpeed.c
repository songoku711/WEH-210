/* 
 * File:   _127G000000_menu_BalanceSpeed.c
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
#define MENUMANAGER_BALANCESPEED_INTERNALDATALENGTH                   (uint8_t)5U

#define MENUMANAGER_BALANCESPEED_INTERNALSTATE_IDX                    0U
#define MENUMANAGER_BALANCESPEED_COUNTER_IDX                          1U
#define MENUMANAGER_BALANCESPEED_VALUE_MIN_IDX                        2U
#define MENUMANAGER_BALANCESPEED_VALUE_MAX_IDX                        3U
#define MENUMANAGER_BALANCESPEED_VALUE_IDX                            4U

#define MenuManager_BalanceSpeed_InternalState                        MenuManager_GetInternalDataPtr(MENUMANAGER_BALANCESPEED_INTERNALSTATE_IDX)
#define MenuManager_BalanceSpeed_Counter                              MenuManager_GetInternalDataPtr(MENUMANAGER_BALANCESPEED_COUNTER_IDX)
#define MenuManager_BalanceSpeed_ValueMin                             MenuManager_GetInternalDataPtr(MENUMANAGER_BALANCESPEED_VALUE_MIN_IDX)
#define MenuManager_BalanceSpeed_ValueMax                             MenuManager_GetInternalDataPtr(MENUMANAGER_BALANCESPEED_VALUE_MAX_IDX)
#define MenuManager_BalanceSpeed_Value                                MenuManager_GetInternalDataPtr(MENUMANAGER_BALANCESPEED_VALUE_IDX)



#define MENUMANAGER_BALANCESPEED_INTERNALSTATE_READY                  (uint32_t)0U
#define MENUMANAGER_BALANCESPEED_INTERNALSTATE_RUNNING                (uint32_t)1U
#define MENUMANAGER_BALANCESPEED_INTERNALSTATE_DONE                   (uint32_t)2U
#define MENUMANAGER_BALANCESPEED_INTERNALSTATE_EXITING                (uint32_t)3U

#define MENUMANAGER_BALANCESPEED_COUNTER_MAX                          (uint32_t)50U

#define MENUMANAGER_BALANCESPEED_VALUE_XPOS                           (uint32_t)10U
#define MENUMANAGER_BALANCESPEED_VALUE_YPOS                           (uint32_t)5U

#define MENUMANAGER_BALANCESPEED_VALUE_LENGTH                         (uint32_t)1U

#define MENUMANAGER_BALANCESPEED_UNIT_XPOS                            (uint32_t)1U
#define MENUMANAGER_BALANCESPEED_UNIT_YPOS                            (uint32_t)3U



/** Menu manager main titles and child menu titles */
static const uint8_t MenuManager_BalanceSpeed_MainTitleStr[] =        "BALANCE SPEED";



/** Menu manager button event mapping array */
static MenuManager_ButEventMapStruct MenuManager_BalanceSpeed_ButEventMap[4] =
{
  { IOMANAGER_BUTTON_STATE_START,                                     MENUMANAGER_EVENT_START_BUT             },
  { IOMANAGER_BUTTON_STATE_STOP,                                      MENUMANAGER_EVENT_STOP_BUT              },
  { IOMANAGER_BUTTON_STATE_UP,                                        MENUMANAGER_EVENT_UP_BUT                },
  { IOMANAGER_BUTTON_STATE_DOWN,                                      MENUMANAGER_EVENT_DOWN_BUT              }
};

/** Menu manager button event mapping configuration */
static MenuManager_ButEventMapConfStruct MenuManager_BalanceSpeed_ButEventMapConf =
{
  .butEventMapNum             = (uint8_t)4U,
  .butEventMapCfg             = &MenuManager_BalanceSpeed_ButEventMap
};



/** Menu manager event handlers */
static Fsm_GuardType MenuManager_BalanceSpeed_Entry                       (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_BalanceSpeed_Exit                        (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_BalanceSpeed_Submenu1                    (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_BalanceSpeed_StartBut                    (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_BalanceSpeed_StopBut                     (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_BalanceSpeed_UpBut                       (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_BalanceSpeed_DownBut                     (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);

/** Menu manager state machine */
Fsm_EventEntryStruct MenuManager_BalanceSpeed_StateMachine[7] =
{
  FSM_TRIGGER_ENTRY             (                                     MenuManager_BalanceSpeed_Entry                                                  ),
  FSM_TRIGGER_EXIT              (                                     MenuManager_BalanceSpeed_Exit                                                   ),
  FSM_TRIGGER_TRANSITION_ACTION ( MENUMANAGER_EVENT_SUBMENU_1,        MenuManager_BalanceSpeed_Submenu1,      MENUMANAGER_STATE_EXTRACT_SETUP         ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_START_BUT,        MenuManager_BalanceSpeed_StartBut                                               ),
  FSM_TRIGGER_TRANSITION_ACTION ( MENUMANAGER_EVENT_STOP_BUT,         MenuManager_BalanceSpeed_StopBut,       MENUMANAGER_STATE_EXTRACT_SETUP         ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_UP_BUT,           MenuManager_BalanceSpeed_UpBut                                                  ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_DOWN_BUT,         MenuManager_BalanceSpeed_DownBut                                                )
};



/*===============================================================================================
*                                   LOCAL FUNCTION PROTOTYPES
===============================================================================================*/

static void MenuManager_BalanceSpeed_LcdShowMainTitle(void);
static void MenuManager_BalanceSpeed_LcdShowAdjust(void);
static void MenuManager_BalanceSpeed_LcdShowDone(void);

static void MenuManager_BalanceSpeed_SubMainFunction(void);
static void MenuManager_BalanceSpeed_SubTickHandler(void);



/*===============================================================================================
*                                       LOCAL FUNCTIONS
===============================================================================================*/

static void MenuManager_BalanceSpeed_LcdShowMainTitle(void)
{
  /* Print main title */
  MenuManager_Common_LcdClearAdjustMenuStatic();
  MenuManager_Common_LcdShowMainTitle(MenuManager_BalanceSpeed_MainTitleStr);
  MenuManager_Common_LcdShowOkExitChoice();
  MenuManager_Common_LcdUpdateAdjustMenuStatic();
}

/*=============================================================================================*/
static void MenuManager_BalanceSpeed_LcdShowAdjust(void)
{
  MenuManager_Common_LcdClearAdjustMenuDynamic();

  MenuManager_Common_LcdShowAdjustUnitVal
  (
    MENUMANAGER_BALANCESPEED_VALUE_XPOS,
    MENUMANAGER_BALANCESPEED_VALUE_YPOS,
    &(MenuManager_BalanceSpeed_Value),
    MENUMANAGER_BALANCESPEED_VALUE_LENGTH
  );
  
  MenuManager_Common_LcdShowAdjustArrow
  (
    MENUMANAGER_BALANCESPEED_VALUE_XPOS,
    MENUMANAGER_BALANCESPEED_VALUE_YPOS,
    (uint32_t)0U
  );

  LCD_SetCursorPos(MENUMANAGER_BALANCESPEED_UNIT_XPOS, MENUMANAGER_BALANCESPEED_UNIT_YPOS, LCD_CURSOR_BY_FONT);
  LCD_PutString((uint8_t *)MenuManager_Common_LevelStr);

  MenuManager_Common_LcdUpdateAdjustMenuDynamic();
}

/*=============================================================================================*/
static void MenuManager_BalanceSpeed_LcdShowDone(void)
{
  MenuManager_Common_LcdClearAll();

  MenuManager_Common_LcdShowAdjustUnitVal
  (
    MENUMANAGER_BALANCESPEED_VALUE_XPOS,
    MENUMANAGER_BALANCESPEED_VALUE_YPOS,
    &(MenuManager_BalanceSpeed_Value),
    MENUMANAGER_BALANCESPEED_VALUE_LENGTH
  );

  LCD_SetCursorPos(MENUMANAGER_BALANCESPEED_UNIT_XPOS, MENUMANAGER_BALANCESPEED_UNIT_YPOS, LCD_CURSOR_BY_FONT);
  LCD_PutString((uint8_t *)MenuManager_Common_LevelStr);

  MenuManager_Common_LcdUpdateAll();
}



/*=============================================================================================*/
static Fsm_GuardType MenuManager_BalanceSpeed_Entry(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  MenuManager_SubMainFunction = MenuManager_BalanceSpeed_SubMainFunction;
  MenuManager_SubTickHandler = MenuManager_BalanceSpeed_SubTickHandler;

  /* Check if previous state data hierachy is not empty */
  if (pFsmContext->dataHierachy != NULL)
  {
    if (pFsmContext->dataHierachy->dataId == MENUMANAGER_STATE_EXTRACT_SETUP)
    {
      /* Release previous state data hierachy */
      MenuManager_free(pFsmContext->dataHierachy);
      pFsmContext->dataHierachy = NULL;

      MenuManager_InternalDataPush(MENUMANAGER_BALANCESPEED_INTERNALDATALENGTH);

      MenuManager_BalanceSpeed_InternalState = MENUMANAGER_BALANCESPEED_INTERNALSTATE_READY;
      MenuManager_BalanceSpeed_Counter = (uint32_t)0U;

      MenuManager_BalanceSpeed_ValueMin = (uint32_t)(ProgramManager_gParamConfig.extractCfg.fwdRunSpeed);
      MenuManager_BalanceSpeed_ValueMax = (uint32_t)(ProgramManager_gParamConfig.extractCfg.midExtractSpeed);
      MenuManager_BalanceSpeed_Value = (uint32_t)(ProgramManager_gParamConfig.extractCfg.balanceSpeed);
    }
    else
    {
      return FSM_GUARD_FALSE;
    }

    MenuManager_BalanceSpeed_LcdShowMainTitle();
    MenuManager_BalanceSpeed_LcdShowAdjust();

    return FSM_GUARD_TRUE;
  }

  return FSM_GUARD_FALSE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_BalanceSpeed_Exit(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  MenuManager_SubMainFunction = NULL;
  MenuManager_SubTickHandler = NULL;

  Fsm_DataHierachyStruct* dataHierachy;

  dataHierachy = (Fsm_DataHierachyStruct *)MenuManager_malloc(sizeof(Fsm_DataHierachyStruct));
  dataHierachy->dataId = MENUMANAGER_STATE_BALANCE_SPEED;

  pFsmContext->dataHierachy = dataHierachy;

  /* Free internal data */
  MenuManager_InternalDataPop();
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_BalanceSpeed_Submenu1(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_BalanceSpeed_StartBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_BalanceSpeed_InternalState == MENUMANAGER_BALANCESPEED_INTERNALSTATE_READY)
  {
    MenuManager_BalanceSpeed_InternalState = MENUMANAGER_BALANCESPEED_INTERNALSTATE_RUNNING;
  }
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_BalanceSpeed_StopBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_BalanceSpeed_InternalState != MENUMANAGER_BALANCESPEED_INTERNALSTATE_READY)
  {
    return FSM_GUARD_FALSE;
  }
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_BalanceSpeed_UpBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_BalanceSpeed_InternalState == MENUMANAGER_BALANCESPEED_INTERNALSTATE_READY)
  {
    if (MenuManager_BalanceSpeed_Value < MenuManager_BalanceSpeed_ValueMax)
    {
      MenuManager_BalanceSpeed_Value += (uint32_t)1U;
    }

    MenuManager_BalanceSpeed_LcdShowAdjust();
  }

  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_BalanceSpeed_DownBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_BalanceSpeed_InternalState == MENUMANAGER_BALANCESPEED_INTERNALSTATE_READY)
  {
    if (MenuManager_BalanceSpeed_Value > MenuManager_BalanceSpeed_ValueMin)
    {
      MenuManager_BalanceSpeed_Value -= (uint32_t)1U;
    }

    MenuManager_BalanceSpeed_LcdShowAdjust();
  }
  
  return FSM_GUARD_TRUE;
}



/*=============================================================================================*/
static void MenuManager_BalanceSpeed_SubMainFunction(void)
{
  MenuManager_Common_ButEventMapHandler(&MenuManager_BalanceSpeed_ButEventMapConf);

  switch (MenuManager_BalanceSpeed_InternalState)
  {
    case MENUMANAGER_BALANCESPEED_INTERNALSTATE_READY:
      break;
    case MENUMANAGER_BALANCESPEED_INTERNALSTATE_RUNNING:
    {
      ProgramManager_MotorSpeedType tempBalanceSpeed;

      tempBalanceSpeed = (ProgramManager_MotorSpeedType)MenuManager_BalanceSpeed_Value;

      ProgramManager_ExtractSetup_BalanceSpeed_SetData(&tempBalanceSpeed);

      ProgramManager_ExtractSetup_BalanceSpeed_GetData(&(ProgramManager_gParamConfig.extractCfg.balanceSpeed));

      MenuManager_BalanceSpeed_InternalState = MENUMANAGER_BALANCESPEED_INTERNALSTATE_DONE;
      
      break;
    }
    case MENUMANAGER_BALANCESPEED_INTERNALSTATE_DONE:
    {
      MenuManager_BalanceSpeed_LcdShowDone();

      MenuManager_BalanceSpeed_InternalState = MENUMANAGER_BALANCESPEED_INTERNALSTATE_EXITING;

      break;
    }
    case MENUMANAGER_BALANCESPEED_INTERNALSTATE_EXITING:
    default:
      break;
  }
}

/*=============================================================================================*/
static void MenuManager_BalanceSpeed_SubTickHandler(void)
{
  if (MenuManager_BalanceSpeed_InternalState == MENUMANAGER_BALANCESPEED_INTERNALSTATE_EXITING)
  {
    MenuManager_BalanceSpeed_Counter += (uint32_t)1U;

    if (MenuManager_BalanceSpeed_Counter >= MENUMANAGER_BALANCESPEED_COUNTER_MAX)
    {
      MenuManager_BalanceSpeed_Counter = (uint32_t)0U;
      
      Fsm_TriggerEvent(&MenuManager_FsmContext, (Fsm_EventType)MENUMANAGER_EVENT_SUBMENU_1);
    }
  }
}



#ifdef  __cplusplus
}
#endif
