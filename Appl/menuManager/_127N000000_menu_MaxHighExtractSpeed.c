/* 
 * File:   _127N000000_menu_MaxHighExtractSpeed.c
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
#define MENUMANAGER_MAXHIGHEXTRACTSPEED_INTERNALDATALENGTH            (uint8_t)5U

#define MENUMANAGER_MAXHIGHEXTRACTSPEED_INTERNALSTATE_IDX             0U
#define MENUMANAGER_MAXHIGHEXTRACTSPEED_COUNTER_IDX                   1U
#define MENUMANAGER_MAXHIGHEXTRACTSPEED_VALUE_MIN_IDX                 2U
#define MENUMANAGER_MAXHIGHEXTRACTSPEED_VALUE_MAX_IDX                 3U
#define MENUMANAGER_MAXHIGHEXTRACTSPEED_VALUE_IDX                     4U

#define MenuManager_MaxHighExtractSpeed_InternalState                 MenuManager_GetInternalDataPtr(MENUMANAGER_MAXHIGHEXTRACTSPEED_INTERNALSTATE_IDX)
#define MenuManager_MaxHighExtractSpeed_Counter                       MenuManager_GetInternalDataPtr(MENUMANAGER_MAXHIGHEXTRACTSPEED_COUNTER_IDX)
#define MenuManager_MaxHighExtractSpeed_ValueMin                      MenuManager_GetInternalDataPtr(MENUMANAGER_MAXHIGHEXTRACTSPEED_VALUE_MIN_IDX)
#define MenuManager_MaxHighExtractSpeed_ValueMax                      MenuManager_GetInternalDataPtr(MENUMANAGER_MAXHIGHEXTRACTSPEED_VALUE_MAX_IDX)
#define MenuManager_MaxHighExtractSpeed_Value                         MenuManager_GetInternalDataPtr(MENUMANAGER_MAXHIGHEXTRACTSPEED_VALUE_IDX)



#define MENUMANAGER_MAXHIGHEXTRACTSPEED_INTERNALSTATE_READY           (uint32_t)0U
#define MENUMANAGER_MAXHIGHEXTRACTSPEED_INTERNALSTATE_RUNNING         (uint32_t)1U
#define MENUMANAGER_MAXHIGHEXTRACTSPEED_INTERNALSTATE_DONE            (uint32_t)2U
#define MENUMANAGER_MAXHIGHEXTRACTSPEED_INTERNALSTATE_EXITING         (uint32_t)3U

#define MENUMANAGER_MAXHIGHEXTRACTSPEED_COUNTER_MAX                   (uint32_t)50U

#define MENUMANAGER_MAXHIGHEXTRACTSPEED_VALUE_XPOS                    (uint32_t)10U
#define MENUMANAGER_MAXHIGHEXTRACTSPEED_VALUE_YPOS                    (uint32_t)5U

#define MENUMANAGER_MAXHIGHEXTRACTSPEED_VALUE_LENGTH                  (uint32_t)1U

#define MENUMANAGER_MAXHIGHEXTRACTSPEED_UNIT_XPOS                     (uint32_t)1U
#define MENUMANAGER_MAXHIGHEXTRACTSPEED_UNIT_YPOS                     (uint32_t)3U



/** Menu manager main titles and child menu titles */
static const uint8_t MenuManager_MaxHighExtractSpeed_MainTitleStr[] = "MAX HIGH E SPEED";



/** Menu manager button event mapping array */
static MenuManager_ButEventMapStruct MenuManager_MaxHighExtractSpeed_ButEventMap[4] =
{
  { IOMANAGER_BUTTON_STATE_START,                                     MENUMANAGER_EVENT_START_BUT             },
  { IOMANAGER_BUTTON_STATE_STOP,                                      MENUMANAGER_EVENT_STOP_BUT              },
  { IOMANAGER_BUTTON_STATE_UP,                                        MENUMANAGER_EVENT_UP_BUT                },
  { IOMANAGER_BUTTON_STATE_DOWN,                                      MENUMANAGER_EVENT_DOWN_BUT              }
};

/** Menu manager button event mapping configuration */
static MenuManager_ButEventMapConfStruct MenuManager_MaxHighExtractSpeed_ButEventMapConf =
{
  .butEventMapNum             = (uint8_t)4U,
  .butEventMapCfg             = &MenuManager_MaxHighExtractSpeed_ButEventMap
};



/** Menu manager event handlers */
static Fsm_GuardType MenuManager_MaxHighExtractSpeed_Entry                (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_MaxHighExtractSpeed_Exit                 (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_MaxHighExtractSpeed_Submenu1             (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_MaxHighExtractSpeed_StartBut             (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_MaxHighExtractSpeed_StopBut              (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_MaxHighExtractSpeed_UpBut                (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_MaxHighExtractSpeed_DownBut              (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);

/** Menu manager state machine */
Fsm_EventEntryStruct MenuManager_MaxHighExtractSpeed_StateMachine[7] =
{
  FSM_TRIGGER_ENTRY             (                                     MenuManager_MaxHighExtractSpeed_Entry                                           ),
  FSM_TRIGGER_EXIT              (                                     MenuManager_MaxHighExtractSpeed_Exit                                            ),
  FSM_TRIGGER_TRANSITION_ACTION ( MENUMANAGER_EVENT_SUBMENU_1,        MenuManager_MaxHighExtractSpeed_Submenu1, MENUMANAGER_STATE_EXTRACT_SETUP       ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_START_BUT,        MenuManager_MaxHighExtractSpeed_StartBut                                        ),
  FSM_TRIGGER_TRANSITION_ACTION ( MENUMANAGER_EVENT_STOP_BUT,         MenuManager_MaxHighExtractSpeed_StopBut,  MENUMANAGER_STATE_EXTRACT_SETUP       ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_UP_BUT,           MenuManager_MaxHighExtractSpeed_UpBut                                           ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_DOWN_BUT,         MenuManager_MaxHighExtractSpeed_DownBut                                         )
};



/*===============================================================================================
*                                   LOCAL FUNCTION PROTOTYPES
===============================================================================================*/

static void MenuManager_MaxHighExtractSpeed_LcdShowMainTitle(void);
static void MenuManager_MaxHighExtractSpeed_LcdShowAdjust(void);
static void MenuManager_MaxHighExtractSpeed_LcdShowDone(void);

static void MenuManager_MaxHighExtractSpeed_SubMainFunction(void);
static void MenuManager_MaxHighExtractSpeed_SubTickHandler(void);



/*===============================================================================================
*                                       LOCAL FUNCTIONS
===============================================================================================*/

static void MenuManager_MaxHighExtractSpeed_LcdShowMainTitle(void)
{
  /* Print main title */
  MenuManager_Common_LcdClearAdjustMenuStatic();
  MenuManager_Common_LcdShowMainTitle(MenuManager_MaxHighExtractSpeed_MainTitleStr);
  MenuManager_Common_LcdShowOkExitChoice();
  MenuManager_Common_LcdUpdateAdjustMenuStatic();
}

/*=============================================================================================*/
static void MenuManager_MaxHighExtractSpeed_LcdShowAdjust(void)
{
  MenuManager_Common_LcdClearAdjustMenuDynamic();

  MenuManager_Common_LcdShowAdjustUnitVal
  (
    MENUMANAGER_MAXHIGHEXTRACTSPEED_VALUE_XPOS,
    MENUMANAGER_MAXHIGHEXTRACTSPEED_VALUE_YPOS,
    &(MenuManager_MaxHighExtractSpeed_Value),
    MENUMANAGER_MAXHIGHEXTRACTSPEED_VALUE_LENGTH
  );
  
  MenuManager_Common_LcdShowAdjustArrow
  (
    MENUMANAGER_MAXHIGHEXTRACTSPEED_VALUE_XPOS,
    MENUMANAGER_MAXHIGHEXTRACTSPEED_VALUE_YPOS,
    (uint32_t)0U
  );

  LCD_SetCursorPos(MENUMANAGER_MAXHIGHEXTRACTSPEED_UNIT_XPOS, MENUMANAGER_MAXHIGHEXTRACTSPEED_UNIT_YPOS, LCD_CURSOR_BY_FONT);
  LCD_PutString((uint8_t *)MenuManager_Common_LevelStr);

  MenuManager_Common_LcdUpdateAdjustMenuDynamic();
}

/*=============================================================================================*/
static void MenuManager_MaxHighExtractSpeed_LcdShowDone(void)
{
  MenuManager_Common_LcdClearAll();

  MenuManager_Common_LcdShowAdjustUnitVal
  (
    MENUMANAGER_MAXHIGHEXTRACTSPEED_VALUE_XPOS,
    MENUMANAGER_MAXHIGHEXTRACTSPEED_VALUE_YPOS,
    &(MenuManager_MaxHighExtractSpeed_Value),
    MENUMANAGER_MAXHIGHEXTRACTSPEED_VALUE_LENGTH
  );

  LCD_SetCursorPos(MENUMANAGER_MAXHIGHEXTRACTSPEED_UNIT_XPOS, MENUMANAGER_MAXHIGHEXTRACTSPEED_UNIT_YPOS, LCD_CURSOR_BY_FONT);
  LCD_PutString((uint8_t *)MenuManager_Common_LevelStr);

  MenuManager_Common_LcdUpdateAll();
}



/*=============================================================================================*/
static Fsm_GuardType MenuManager_MaxHighExtractSpeed_Entry(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  MenuManager_SubMainFunction = MenuManager_MaxHighExtractSpeed_SubMainFunction;
  MenuManager_SubTickHandler = MenuManager_MaxHighExtractSpeed_SubTickHandler;

  /* Check if previous state data hierachy is not empty */
  if (pFsmContext->dataHierachy != NULL)
  {
    if (pFsmContext->dataHierachy->dataId == MENUMANAGER_STATE_EXTRACT_SETUP)
    {
      /* Release previous state data hierachy */
      MenuManager_free(pFsmContext->dataHierachy);
      pFsmContext->dataHierachy = NULL;

      MenuManager_InternalDataPush(MENUMANAGER_MAXHIGHEXTRACTSPEED_INTERNALDATALENGTH);

      MenuManager_MaxHighExtractSpeed_InternalState = MENUMANAGER_MAXHIGHEXTRACTSPEED_INTERNALSTATE_READY;
      MenuManager_MaxHighExtractSpeed_Counter = (uint32_t)0U;

      MenuManager_MaxHighExtractSpeed_ValueMin = (uint32_t)(ProgramManager_gParamConfig.extractCfg.highExtractSpeed1);
      MenuManager_MaxHighExtractSpeed_ValueMax = (uint32_t)PROGRAMMANAGER_MOTOR_SPEED_LEVEL_7;
      MenuManager_MaxHighExtractSpeed_Value = (uint32_t)(ProgramManager_gParamConfig.extractCfg.maxHighExtractSpeed);
    }
    else
    {
      return FSM_GUARD_FALSE;
    }

    MenuManager_MaxHighExtractSpeed_LcdShowMainTitle();
    MenuManager_MaxHighExtractSpeed_LcdShowAdjust();

    return FSM_GUARD_TRUE;
  }

  return FSM_GUARD_FALSE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_MaxHighExtractSpeed_Exit(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  MenuManager_SubMainFunction = NULL;
  MenuManager_SubTickHandler = NULL;

  Fsm_DataHierachyStruct* dataHierachy;

  dataHierachy = (Fsm_DataHierachyStruct *)MenuManager_malloc(sizeof(Fsm_DataHierachyStruct));
  dataHierachy->dataId = MENUMANAGER_STATE_MAX_HIGH_EXTRACT_SPEED;

  pFsmContext->dataHierachy = dataHierachy;

  /* Free internal data */
  MenuManager_InternalDataPop();
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_MaxHighExtractSpeed_Submenu1(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_MaxHighExtractSpeed_StartBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_MaxHighExtractSpeed_InternalState == MENUMANAGER_MAXHIGHEXTRACTSPEED_INTERNALSTATE_READY)
  {
    MenuManager_MaxHighExtractSpeed_InternalState = MENUMANAGER_MAXHIGHEXTRACTSPEED_INTERNALSTATE_RUNNING;
  }
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_MaxHighExtractSpeed_StopBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_MaxHighExtractSpeed_InternalState != MENUMANAGER_MAXHIGHEXTRACTSPEED_INTERNALSTATE_READY)
  {
    return FSM_GUARD_FALSE;
  }
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_MaxHighExtractSpeed_UpBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_MaxHighExtractSpeed_InternalState == MENUMANAGER_MAXHIGHEXTRACTSPEED_INTERNALSTATE_READY)
  {
    if (MenuManager_MaxHighExtractSpeed_Value < MenuManager_MaxHighExtractSpeed_ValueMax)
    {
      MenuManager_MaxHighExtractSpeed_Value += (uint32_t)1U;
    }

    MenuManager_MaxHighExtractSpeed_LcdShowAdjust();
  }

  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_MaxHighExtractSpeed_DownBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_MaxHighExtractSpeed_InternalState == MENUMANAGER_MAXHIGHEXTRACTSPEED_INTERNALSTATE_READY)
  {
    if (MenuManager_MaxHighExtractSpeed_Value > MenuManager_MaxHighExtractSpeed_ValueMin)
    {
      MenuManager_MaxHighExtractSpeed_Value -= (uint32_t)1U;
    }

    MenuManager_MaxHighExtractSpeed_LcdShowAdjust();
  }
  
  return FSM_GUARD_TRUE;
}



/*=============================================================================================*/
static void MenuManager_MaxHighExtractSpeed_SubMainFunction(void)
{
  MenuManager_Common_ButEventMapHandler(&MenuManager_MaxHighExtractSpeed_ButEventMapConf);

  switch (MenuManager_MaxHighExtractSpeed_InternalState)
  {
    case MENUMANAGER_MAXHIGHEXTRACTSPEED_INTERNALSTATE_READY:
      break;
    case MENUMANAGER_MAXHIGHEXTRACTSPEED_INTERNALSTATE_RUNNING:
    {
      ProgramManager_MotorSpeedType tempMaxHighExtractSpeed;

      tempMaxHighExtractSpeed = (ProgramManager_MotorSpeedType)MenuManager_MaxHighExtractSpeed_Value;

      ProgramManager_ExtractSetup_MaxHighExtractSpeed_SetData(&tempMaxHighExtractSpeed);

      ProgramManager_ExtractSetup_MaxHighExtractSpeed_GetData(&(ProgramManager_gParamConfig.extractCfg.maxHighExtractSpeed));

      MenuManager_MaxHighExtractSpeed_InternalState = MENUMANAGER_MAXHIGHEXTRACTSPEED_INTERNALSTATE_DONE;
      
      break;
    }
    case MENUMANAGER_MAXHIGHEXTRACTSPEED_INTERNALSTATE_DONE:
    {
      MenuManager_MaxHighExtractSpeed_LcdShowDone();

      MenuManager_MaxHighExtractSpeed_InternalState = MENUMANAGER_MAXHIGHEXTRACTSPEED_INTERNALSTATE_EXITING;

      break;
    }
    case MENUMANAGER_MAXHIGHEXTRACTSPEED_INTERNALSTATE_EXITING:
    default:
      break;
  }
}

/*=============================================================================================*/
static void MenuManager_MaxHighExtractSpeed_SubTickHandler(void)
{
  if (MenuManager_MaxHighExtractSpeed_InternalState == MENUMANAGER_MAXHIGHEXTRACTSPEED_INTERNALSTATE_EXITING)
  {
    MenuManager_MaxHighExtractSpeed_Counter += (uint32_t)1U;

    if (MenuManager_MaxHighExtractSpeed_Counter >= MENUMANAGER_MAXHIGHEXTRACTSPEED_COUNTER_MAX)
    {
      MenuManager_MaxHighExtractSpeed_Counter = (uint32_t)0U;
      
      Fsm_TriggerEvent(&MenuManager_FsmContext, (Fsm_EventType)MENUMANAGER_EVENT_SUBMENU_1);
    }
  }
}



#ifdef  __cplusplus
}
#endif
