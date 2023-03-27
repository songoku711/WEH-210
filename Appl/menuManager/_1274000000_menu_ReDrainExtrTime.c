/* 
 * File:   _1274000000_menu_ReDrainExtrTime.c
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
#define MENUMANAGER_REDRAINEXTRTIME_INTERNALDATALENGTH                (uint8_t)5U

#define MENUMANAGER_REDRAINEXTRTIME_INTERNALSTATE_IDX                 0U
#define MENUMANAGER_REDRAINEXTRTIME_COUNTER_IDX                       1U
#define MENUMANAGER_REDRAINEXTRTIME_VALUE_MIN_IDX                     2U
#define MENUMANAGER_REDRAINEXTRTIME_VALUE_MAX_IDX                     3U
#define MENUMANAGER_REDRAINEXTRTIME_VALUE_IDX                         4U

#define MenuManager_ReDrainExtrTime_InternalState                     MenuManager_GetInternalDataPtr(MENUMANAGER_REDRAINEXTRTIME_INTERNALSTATE_IDX)
#define MenuManager_ReDrainExtrTime_Counter                           MenuManager_GetInternalDataPtr(MENUMANAGER_REDRAINEXTRTIME_COUNTER_IDX)
#define MenuManager_ReDrainExtrTime_ValueMin                          MenuManager_GetInternalDataPtr(MENUMANAGER_REDRAINEXTRTIME_VALUE_MIN_IDX)
#define MenuManager_ReDrainExtrTime_ValueMax                          MenuManager_GetInternalDataPtr(MENUMANAGER_REDRAINEXTRTIME_VALUE_MAX_IDX)
#define MenuManager_ReDrainExtrTime_Value                             MenuManager_GetInternalDataPtr(MENUMANAGER_REDRAINEXTRTIME_VALUE_IDX)



#define MENUMANAGER_REDRAINEXTRTIME_INTERNALSTATE_READY               (uint32_t)0U
#define MENUMANAGER_REDRAINEXTRTIME_INTERNALSTATE_RUNNING             (uint32_t)1U
#define MENUMANAGER_REDRAINEXTRTIME_INTERNALSTATE_DONE                (uint32_t)2U
#define MENUMANAGER_REDRAINEXTRTIME_INTERNALSTATE_EXITING             (uint32_t)3U

#define MENUMANAGER_REDRAINEXTRTIME_COUNTER_MAX                       (uint32_t)50U

#define MENUMANAGER_REDRAINEXTRTIME_VALUE_XPOS                        (uint32_t)10U
#define MENUMANAGER_REDRAINEXTRTIME_VALUE_YPOS                        (uint32_t)5U

#define MENUMANAGER_REDRAINEXTRTIME_VALUE_LENGTH                      (uint32_t)1U

#define MENUMANAGER_REDRAINEXTRTIME_UNIT_XPOS                         (uint32_t)1U
#define MENUMANAGER_REDRAINEXTRTIME_UNIT_YPOS                         (uint32_t)3U



/** Menu manager main titles and child menu titles */
static const uint8_t MenuManager_ReDrainExtrTime_MainTitleStr[] =     "REDRAIN TIME";



/** Menu manager button event mapping array */
static MenuManager_ButEventMapStruct MenuManager_ReDrainExtrTime_ButEventMap[4] =
{
  { IOMANAGER_BUTTON_STATE_START,                                     MENUMANAGER_EVENT_START_BUT             },
  { IOMANAGER_BUTTON_STATE_STOP,                                      MENUMANAGER_EVENT_STOP_BUT              },
  { IOMANAGER_BUTTON_STATE_UP,                                        MENUMANAGER_EVENT_UP_BUT                },
  { IOMANAGER_BUTTON_STATE_DOWN,                                      MENUMANAGER_EVENT_DOWN_BUT              }
};

/** Menu manager button event mapping configuration */
static MenuManager_ButEventMapConfStruct MenuManager_ReDrainExtrTime_ButEventMapConf =
{
  .butEventMapNum             = (uint8_t)4U,
  .butEventMapCfg             = &MenuManager_ReDrainExtrTime_ButEventMap
};



/** Menu manager event handlers */
static Fsm_GuardType MenuManager_ReDrainExtrTime_Entry                (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_ReDrainExtrTime_Exit                 (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_ReDrainExtrTime_StartBut             (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_ReDrainExtrTime_StopBut              (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_ReDrainExtrTime_UpBut                (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_ReDrainExtrTime_DownBut              (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);

/** Menu manager state machine */
Fsm_EventEntryStruct MenuManager_ReDrainExtrTime_StateMachine[7] =
{
  FSM_TRIGGER_ENTRY             (                                     MenuManager_ReDrainExtrTime_Entry                                               ),
  FSM_TRIGGER_EXIT              (                                     MenuManager_ReDrainExtrTime_Exit                                                ),
  FSM_TRIGGER_TRANSITION        ( MENUMANAGER_EVENT_PREV,                                                     MENUMANAGER_STATE_DRAIN_SETUP           ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_START_BUT,        MenuManager_ReDrainExtrTime_StartBut                                            ),
  FSM_TRIGGER_TRANSITION_ACTION ( MENUMANAGER_EVENT_STOP_BUT,         MenuManager_ReDrainExtrTime_StopBut,    MENUMANAGER_STATE_DRAIN_SETUP           ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_UP_BUT,           MenuManager_ReDrainExtrTime_UpBut                                               ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_DOWN_BUT,         MenuManager_ReDrainExtrTime_DownBut                                             )
};



/*===============================================================================================
*                                   LOCAL FUNCTION PROTOTYPES
===============================================================================================*/

static void MenuManager_ReDrainExtrTime_LcdShowMainTitle(void);
static void MenuManager_ReDrainExtrTime_LcdShowAdjust(void);
static void MenuManager_ReDrainExtrTime_LcdShowDone(void);

static void MenuManager_ReDrainExtrTime_SubMainFunction(void);
static void MenuManager_ReDrainExtrTime_SubTickHandler(void);



/*===============================================================================================
*                                       LOCAL FUNCTIONS
===============================================================================================*/

static void MenuManager_ReDrainExtrTime_LcdShowMainTitle(void)
{
  /* Print main title */
  MenuManager_Common_LcdClearAdjustMenuStatic();
  MenuManager_Common_LcdShowMainTitle(MenuManager_ReDrainExtrTime_MainTitleStr);
  MenuManager_Common_LcdShowOkExitChoice();
  MenuManager_Common_LcdUpdateAdjustMenuStatic();
}

/*=============================================================================================*/
static void MenuManager_ReDrainExtrTime_LcdShowAdjust(void)
{
  MenuManager_Common_LcdClearAdjustMenuDynamic();

  MenuManager_Common_LcdShowAdjustUnitVal
  (
    MENUMANAGER_REDRAINEXTRTIME_VALUE_XPOS,
    MENUMANAGER_REDRAINEXTRTIME_VALUE_YPOS,
    &(MenuManager_ReDrainExtrTime_Value),
    MENUMANAGER_REDRAINEXTRTIME_VALUE_LENGTH
  );
  
  MenuManager_Common_LcdShowAdjustArrow
  (
    MENUMANAGER_REDRAINEXTRTIME_VALUE_XPOS,
    MENUMANAGER_REDRAINEXTRTIME_VALUE_YPOS,
    (uint32_t)0U
  );

  LCD_SetCursorPos(MENUMANAGER_REDRAINEXTRTIME_UNIT_XPOS, MENUMANAGER_REDRAINEXTRTIME_UNIT_YPOS, LCD_CURSOR_BY_FONT);
  LCD_PutString((uint8_t *)MenuManager_Common_TimeStr);

  MenuManager_Common_LcdUpdateAdjustMenuDynamic();
}

/*=============================================================================================*/
static void MenuManager_ReDrainExtrTime_LcdShowDone(void)
{
  MenuManager_Common_LcdClearAll();

  MenuManager_Common_LcdShowAdjustUnitVal
  (
    MENUMANAGER_REDRAINEXTRTIME_VALUE_XPOS,
    MENUMANAGER_REDRAINEXTRTIME_VALUE_YPOS,
    &(MenuManager_ReDrainExtrTime_Value),
    MENUMANAGER_REDRAINEXTRTIME_VALUE_LENGTH
  );

  LCD_SetCursorPos(MENUMANAGER_REDRAINEXTRTIME_UNIT_XPOS, MENUMANAGER_REDRAINEXTRTIME_UNIT_YPOS, LCD_CURSOR_BY_FONT);
  LCD_PutString((uint8_t *)MenuManager_Common_TimeStr);

  MenuManager_Common_LcdUpdateAll();
}



/*=============================================================================================*/
static Fsm_GuardType MenuManager_ReDrainExtrTime_Entry(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  HAL_StatusTypeDef retVal = HAL_OK;

  /* Check if previous state data hierachy is not empty */
  if (pFsmContext->dataHierachy != NULL)
  {
    if (pFsmContext->dataHierachy->dataId == MENUMANAGER_STATE_DRAIN_SETUP)
    {
      /* Release previous state data hierachy */
      MenuManager_free(pFsmContext->dataHierachy);
      pFsmContext->dataHierachy = NULL;

      MenuManager_InternalDataPush(MENUMANAGER_REDRAINEXTRTIME_INTERNALDATALENGTH);

      MenuManager_ReDrainExtrTime_InternalState = MENUMANAGER_REDRAINEXTRTIME_INTERNALSTATE_READY;
      MenuManager_ReDrainExtrTime_Counter = (uint32_t)0U;

      MenuManager_ReDrainExtrTime_ValueMin = (uint32_t)PROGRAMMANAGER_DRAINSETUP_REDRAINEXTRTIME_MIN;
      MenuManager_ReDrainExtrTime_ValueMax = (uint32_t)PROGRAMMANAGER_DRAINSETUP_REDRAINEXTRTIME_MAX;
      MenuManager_ReDrainExtrTime_Value = (uint32_t)(ProgramManager_gParamConfig.drainCfg.reDrainExtrTime);
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
    MenuManager_ReDrainExtrTime_LcdShowMainTitle();
    MenuManager_ReDrainExtrTime_LcdShowAdjust();

    MenuManager_SubMainFunction = MenuManager_ReDrainExtrTime_SubMainFunction;
    MenuManager_SubTickHandler = MenuManager_ReDrainExtrTime_SubTickHandler;

    return FSM_GUARD_TRUE;
  }

  return FSM_GUARD_FALSE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_ReDrainExtrTime_Exit(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  Fsm_DataHierachyStruct* dataHierachy;

  MenuManager_SubMainFunction = NULL;
  MenuManager_SubTickHandler = NULL;

  dataHierachy = (Fsm_DataHierachyStruct *)MenuManager_malloc(sizeof(Fsm_DataHierachyStruct));
  dataHierachy->dataId = MENUMANAGER_STATE_REDRAIN_EXTRACT_TIME;

  pFsmContext->dataHierachy = dataHierachy;

  /* Free internal data */
  MenuManager_InternalDataPop();
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_ReDrainExtrTime_StartBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_ReDrainExtrTime_InternalState == MENUMANAGER_REDRAINEXTRTIME_INTERNALSTATE_READY)
  {
    MenuManager_ReDrainExtrTime_InternalState = MENUMANAGER_REDRAINEXTRTIME_INTERNALSTATE_RUNNING;
  }
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_ReDrainExtrTime_StopBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_ReDrainExtrTime_InternalState != MENUMANAGER_REDRAINEXTRTIME_INTERNALSTATE_READY)
  {
    return FSM_GUARD_FALSE;
  }
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_ReDrainExtrTime_UpBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_ReDrainExtrTime_InternalState == MENUMANAGER_REDRAINEXTRTIME_INTERNALSTATE_READY)
  {
    if (MenuManager_ReDrainExtrTime_Value < MenuManager_ReDrainExtrTime_ValueMax)
    {
      MenuManager_ReDrainExtrTime_Value += (uint32_t)1U;
    }

    MenuManager_ReDrainExtrTime_LcdShowAdjust();
  }

  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_ReDrainExtrTime_DownBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_ReDrainExtrTime_InternalState == MENUMANAGER_REDRAINEXTRTIME_INTERNALSTATE_READY)
  {
    if (MenuManager_ReDrainExtrTime_Value > MenuManager_ReDrainExtrTime_ValueMin)
    {
      MenuManager_ReDrainExtrTime_Value -= (uint32_t)1U;
    }

    MenuManager_ReDrainExtrTime_LcdShowAdjust();
  }
  
  return FSM_GUARD_TRUE;
}



/*=============================================================================================*/
static void MenuManager_ReDrainExtrTime_SubMainFunction(void)
{
  MenuManager_Common_ButEventMapHandler(&MenuManager_ReDrainExtrTime_ButEventMapConf);

  switch (MenuManager_ReDrainExtrTime_InternalState)
  {
    case MENUMANAGER_REDRAINEXTRTIME_INTERNALSTATE_READY:
      break;
    case MENUMANAGER_REDRAINEXTRTIME_INTERNALSTATE_RUNNING:
    {
      uint8_t tempReDrainExtrTime;

      tempReDrainExtrTime = (uint8_t)MenuManager_ReDrainExtrTime_Value;

      ProgramManager_DrainSetup_ReDrainExtrTime_SetData(&tempReDrainExtrTime);

      ProgramManager_DrainSetup_ReDrainExtrTime_GetData(&(ProgramManager_gParamConfig.drainCfg.reDrainExtrTime));

      MenuManager_ReDrainExtrTime_InternalState = MENUMANAGER_REDRAINEXTRTIME_INTERNALSTATE_DONE;
      
      break;
    }
    case MENUMANAGER_REDRAINEXTRTIME_INTERNALSTATE_DONE:
    {
      MenuManager_ReDrainExtrTime_LcdShowDone();

      MenuManager_ReDrainExtrTime_InternalState = MENUMANAGER_REDRAINEXTRTIME_INTERNALSTATE_EXITING;

      break;
    }
    case MENUMANAGER_REDRAINEXTRTIME_INTERNALSTATE_EXITING:
    default:
      break;
  }
}

/*=============================================================================================*/
static void MenuManager_ReDrainExtrTime_SubTickHandler(void)
{
  if (MenuManager_ReDrainExtrTime_InternalState == MENUMANAGER_REDRAINEXTRTIME_INTERNALSTATE_EXITING)
  {
    MenuManager_ReDrainExtrTime_Counter += (uint32_t)1U;

    if (MenuManager_ReDrainExtrTime_Counter >= MENUMANAGER_REDRAINEXTRTIME_COUNTER_MAX)
    {
      MenuManager_ReDrainExtrTime_Counter = (uint32_t)0U;
      
      Fsm_TriggerEvent(&MenuManager_FsmContext, (Fsm_EventType)MENUMANAGER_EVENT_PREV);
    }
  }
}



#ifdef  __cplusplus
}
#endif
