/* 
 * File:   _127E000000_menu_ReextractTime.c
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
#define MENUMANAGER_REEXTRACTTIME_INTERNALDATALENGTH                  (uint8_t)5U

#define MENUMANAGER_REEXTRACTTIME_INTERNALSTATE_IDX                   0U
#define MENUMANAGER_REEXTRACTTIME_COUNTER_IDX                         1U
#define MENUMANAGER_REEXTRACTTIME_VALUE_MIN_IDX                       2U
#define MENUMANAGER_REEXTRACTTIME_VALUE_MAX_IDX                       3U
#define MENUMANAGER_REEXTRACTTIME_VALUE_IDX                           4U

#define MenuManager_ReextractTime_InternalState                       MenuManager_GetInternalDataPtr(MENUMANAGER_REEXTRACTTIME_INTERNALSTATE_IDX)
#define MenuManager_ReextractTime_Counter                             MenuManager_GetInternalDataPtr(MENUMANAGER_REEXTRACTTIME_COUNTER_IDX)
#define MenuManager_ReextractTime_ValueMin                            MenuManager_GetInternalDataPtr(MENUMANAGER_REEXTRACTTIME_VALUE_MIN_IDX)
#define MenuManager_ReextractTime_ValueMax                            MenuManager_GetInternalDataPtr(MENUMANAGER_REEXTRACTTIME_VALUE_MAX_IDX)
#define MenuManager_ReextractTime_Value                               MenuManager_GetInternalDataPtr(MENUMANAGER_REEXTRACTTIME_VALUE_IDX)



#define MENUMANAGER_REEXTRACTTIME_INTERNALSTATE_READY                 (uint32_t)0U
#define MENUMANAGER_REEXTRACTTIME_INTERNALSTATE_RUNNING               (uint32_t)1U
#define MENUMANAGER_REEXTRACTTIME_INTERNALSTATE_DONE                  (uint32_t)2U
#define MENUMANAGER_REEXTRACTTIME_INTERNALSTATE_EXITING               (uint32_t)3U

#define MENUMANAGER_REEXTRACTTIME_COUNTER_MAX                         (uint32_t)50U

#define MENUMANAGER_REEXTRACTTIME_VALUE_XPOS                          (uint32_t)10U
#define MENUMANAGER_REEXTRACTTIME_VALUE_YPOS                          (uint32_t)5U

#define MENUMANAGER_REEXTRACTTIME_VALUE_LENGTH                        (uint32_t)1U

#define MENUMANAGER_REEXTRACTTIME_UNIT_XPOS                           (uint32_t)1U
#define MENUMANAGER_REEXTRACTTIME_UNIT_YPOS                           (uint32_t)3U



/** Menu manager main titles and child menu titles */
static const uint8_t MenuManager_ReextractTime_MainTitleStr[] =       "REEXTRACT TIME";



/** Menu manager button event mapping array */
static MenuManager_ButEventMapStruct MenuManager_ReextractTime_ButEventMap[4] =
{
  { IOMANAGER_BUTTON_STATE_START,                                     MENUMANAGER_EVENT_START_BUT             },
  { IOMANAGER_BUTTON_STATE_STOP,                                      MENUMANAGER_EVENT_STOP_BUT              },
  { IOMANAGER_BUTTON_STATE_UP,                                        MENUMANAGER_EVENT_UP_BUT                },
  { IOMANAGER_BUTTON_STATE_DOWN,                                      MENUMANAGER_EVENT_DOWN_BUT              }
};

/** Menu manager button event mapping configuration */
static MenuManager_ButEventMapConfStruct MenuManager_ReextractTime_ButEventMapConf =
{
  .butEventMapNum             = (uint8_t)4U,
  .butEventMapCfg             = &MenuManager_ReextractTime_ButEventMap
};



/** Menu manager event handlers */
static Fsm_GuardType MenuManager_ReextractTime_Entry                  (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_ReextractTime_Exit                   (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_ReextractTime_StartBut               (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_ReextractTime_StopBut                (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_ReextractTime_UpBut                  (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_ReextractTime_DownBut                (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);

/** Menu manager state machine */
Fsm_EventEntryStruct MenuManager_ReextractTime_StateMachine[7] =
{
  FSM_TRIGGER_ENTRY             (                                     MenuManager_ReextractTime_Entry                                                 ),
  FSM_TRIGGER_EXIT              (                                     MenuManager_ReextractTime_Exit                                                  ),
  FSM_TRIGGER_TRANSITION        ( MENUMANAGER_EVENT_PREV,                                                     MENUMANAGER_STATE_EXTRACT_SETUP         ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_START_BUT,        MenuManager_ReextractTime_StartBut                                              ),
  FSM_TRIGGER_TRANSITION_ACTION ( MENUMANAGER_EVENT_STOP_BUT,         MenuManager_ReextractTime_StopBut,      MENUMANAGER_STATE_EXTRACT_SETUP         ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_UP_BUT,           MenuManager_ReextractTime_UpBut                                                 ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_DOWN_BUT,         MenuManager_ReextractTime_DownBut                                               )
};



/*===============================================================================================
*                                   LOCAL FUNCTION PROTOTYPES
===============================================================================================*/

static void MenuManager_ReextractTime_LcdShowMainTitle(void);
static void MenuManager_ReextractTime_LcdShowAdjust(void);
static void MenuManager_ReextractTime_LcdShowDone(void);

static void MenuManager_ReextractTime_SubMainFunction(void);
static void MenuManager_ReextractTime_SubTickHandler(void);



/*===============================================================================================
*                                       LOCAL FUNCTIONS
===============================================================================================*/

static void MenuManager_ReextractTime_LcdShowMainTitle(void)
{
  /* Print main title */
  MenuManager_Common_LcdClearAdjustMenuStatic();
  MenuManager_Common_LcdShowMainTitle(MenuManager_ReextractTime_MainTitleStr);
  MenuManager_Common_LcdShowOkExitChoice();
  MenuManager_Common_LcdUpdateAdjustMenuStatic();
}

/*=============================================================================================*/
static void MenuManager_ReextractTime_LcdShowAdjust(void)
{
  MenuManager_Common_LcdClearAdjustMenuDynamic();

  MenuManager_Common_LcdShowAdjustUnitVal
  (
    MENUMANAGER_REEXTRACTTIME_VALUE_XPOS,
    MENUMANAGER_REEXTRACTTIME_VALUE_YPOS,
    &(MenuManager_ReextractTime_Value),
    MENUMANAGER_REEXTRACTTIME_VALUE_LENGTH
  );
  
  MenuManager_Common_LcdShowAdjustArrow
  (
    MENUMANAGER_REEXTRACTTIME_VALUE_XPOS,
    MENUMANAGER_REEXTRACTTIME_VALUE_YPOS,
    (uint32_t)0U
  );

  LCD_SetCursorPos(MENUMANAGER_REEXTRACTTIME_UNIT_XPOS, MENUMANAGER_REEXTRACTTIME_UNIT_YPOS, LCD_CURSOR_BY_FONT);
  LCD_PutString((uint8_t *)MenuManager_Common_TimeStr);

  MenuManager_Common_LcdUpdateAdjustMenuDynamic();
}

/*=============================================================================================*/
static void MenuManager_ReextractTime_LcdShowDone(void)
{
  MenuManager_Common_LcdClearAll();

  MenuManager_Common_LcdShowAdjustUnitVal
  (
    MENUMANAGER_REEXTRACTTIME_VALUE_XPOS,
    MENUMANAGER_REEXTRACTTIME_VALUE_YPOS,
    &(MenuManager_ReextractTime_Value),
    MENUMANAGER_REEXTRACTTIME_VALUE_LENGTH
  );

  LCD_SetCursorPos(MENUMANAGER_REEXTRACTTIME_UNIT_XPOS, MENUMANAGER_REEXTRACTTIME_UNIT_YPOS, LCD_CURSOR_BY_FONT);
  LCD_PutString((uint8_t *)MenuManager_Common_TimeStr);

  MenuManager_Common_LcdUpdateAll();
}



/*=============================================================================================*/
static Fsm_GuardType MenuManager_ReextractTime_Entry(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  HAL_StatusTypeDef retVal = HAL_OK;

  /* Check if previous state data hierachy is not empty */
  if (pFsmContext->dataHierachy != NULL)
  {
    if (pFsmContext->dataHierachy->dataId == MENUMANAGER_STATE_EXTRACT_SETUP)
    {
      /* Release previous state data hierachy */
      MenuManager_free(pFsmContext->dataHierachy);
      pFsmContext->dataHierachy = NULL;

      MenuManager_InternalDataPush(MENUMANAGER_REEXTRACTTIME_INTERNALDATALENGTH);

      MenuManager_ReextractTime_InternalState = MENUMANAGER_REEXTRACTTIME_INTERNALSTATE_READY;
      MenuManager_ReextractTime_Counter = (uint32_t)0U;

      MenuManager_ReextractTime_ValueMin = (uint32_t)PROGRAMMANAGER_EXTRACTSETUP_REEXTRACTTIME_MIN;
      MenuManager_ReextractTime_ValueMax = (uint32_t)PROGRAMMANAGER_EXTRACTSETUP_REEXTRACTTIME_MAX;
      MenuManager_ReextractTime_Value = (uint32_t)(ProgramManager_gParamConfig.extractCfg.reextractTime);
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
    MenuManager_ReextractTime_LcdShowMainTitle();
    MenuManager_ReextractTime_LcdShowAdjust();

    MenuManager_SubMainFunction = MenuManager_ReextractTime_SubMainFunction;
    MenuManager_SubTickHandler = MenuManager_ReextractTime_SubTickHandler;

    return FSM_GUARD_TRUE;
  }

  return FSM_GUARD_FALSE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_ReextractTime_Exit(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  Fsm_DataHierachyStruct* dataHierachy;

  MenuManager_SubMainFunction = NULL;
  MenuManager_SubTickHandler = NULL;

  dataHierachy = (Fsm_DataHierachyStruct *)MenuManager_malloc(sizeof(Fsm_DataHierachyStruct));
  dataHierachy->dataId = MENUMANAGER_STATE_REEXTRACT_TIME;

  pFsmContext->dataHierachy = dataHierachy;

  /* Free internal data */
  MenuManager_InternalDataPop();
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_ReextractTime_StartBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_ReextractTime_InternalState == MENUMANAGER_REEXTRACTTIME_INTERNALSTATE_READY)
  {
    MenuManager_ReextractTime_InternalState = MENUMANAGER_REEXTRACTTIME_INTERNALSTATE_RUNNING;
  }
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_ReextractTime_StopBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_ReextractTime_InternalState != MENUMANAGER_REEXTRACTTIME_INTERNALSTATE_READY)
  {
    return FSM_GUARD_FALSE;
  }
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_ReextractTime_UpBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_ReextractTime_InternalState == MENUMANAGER_REEXTRACTTIME_INTERNALSTATE_READY)
  {
    if (MenuManager_ReextractTime_Value < MenuManager_ReextractTime_ValueMax)
    {
      MenuManager_ReextractTime_Value += (uint32_t)1U;
    }

    MenuManager_ReextractTime_LcdShowAdjust();
  }

  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_ReextractTime_DownBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_ReextractTime_InternalState == MENUMANAGER_REEXTRACTTIME_INTERNALSTATE_READY)
  {
    if (MenuManager_ReextractTime_Value > MenuManager_ReextractTime_ValueMin)
    {
      MenuManager_ReextractTime_Value -= (uint32_t)1U;
    }

    MenuManager_ReextractTime_LcdShowAdjust();
  }
  
  return FSM_GUARD_TRUE;
}



/*=============================================================================================*/
static void MenuManager_ReextractTime_SubMainFunction(void)
{
  MenuManager_Common_ButEventMapHandler(&MenuManager_ReextractTime_ButEventMapConf);

  switch (MenuManager_ReextractTime_InternalState)
  {
    case MENUMANAGER_REEXTRACTTIME_INTERNALSTATE_READY:
      break;
    case MENUMANAGER_REEXTRACTTIME_INTERNALSTATE_RUNNING:
    {
      uint16_t tempReextractTime;

      tempReextractTime = (uint16_t)MenuManager_ReextractTime_Value;

      ProgramManager_ExtractSetup_ReextractTime_SetData(&tempReextractTime);

      ProgramManager_ExtractSetup_ReextractTime_GetData(&(ProgramManager_gParamConfig.extractCfg.reextractTime));

      MenuManager_ReextractTime_InternalState = MENUMANAGER_REEXTRACTTIME_INTERNALSTATE_DONE;
      
      break;
    }
    case MENUMANAGER_REEXTRACTTIME_INTERNALSTATE_DONE:
    {
      MenuManager_ReextractTime_LcdShowDone();

      MenuManager_ReextractTime_InternalState = MENUMANAGER_REEXTRACTTIME_INTERNALSTATE_EXITING;

      break;
    }
    case MENUMANAGER_REEXTRACTTIME_INTERNALSTATE_EXITING:
    default:
      break;
  }
}

/*=============================================================================================*/
static void MenuManager_ReextractTime_SubTickHandler(void)
{
  if (MenuManager_ReextractTime_InternalState == MENUMANAGER_REEXTRACTTIME_INTERNALSTATE_EXITING)
  {
    MenuManager_ReextractTime_Counter += (uint32_t)1U;

    if (MenuManager_ReextractTime_Counter >= MENUMANAGER_REEXTRACTTIME_COUNTER_MAX)
    {
      MenuManager_ReextractTime_Counter = (uint32_t)0U;
      
      Fsm_TriggerEvent(&MenuManager_FsmContext, (Fsm_EventType)MENUMANAGER_EVENT_PREV);
    }
  }
}



#ifdef  __cplusplus
}
#endif
