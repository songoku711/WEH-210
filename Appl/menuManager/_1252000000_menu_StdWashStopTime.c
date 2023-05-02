/* 
 * File:   _1263000000_menu_StdWashStopTime.c
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
#define MENUMANAGER_STDWASHSTOPTIME_INTERNALDATALENGTH                (uint8_t)10U

#define MENUMANAGER_STDWASHSTOPTIME_INTERNALSTATE_IDX                 0U
#define MENUMANAGER_STDWASHSTOPTIME_COUNTER_IDX                       1U
#define MENUMANAGER_STDWASHSTOPTIME_CURPOS_IDX                        2U
#define MENUMANAGER_STDWASHSTOPTIME_VALUE_MIN_IDX                     3U
#define MENUMANAGER_STDWASHSTOPTIME_VALUE_MAX_IDX                     4U
#define MENUMANAGER_STDWASHSTOPTIME_VALUE_IDX                         5U
#define MENUMANAGER_STDWASHSTOPTIME_UNITVAL_IDX                       6U

#define MenuManager_StdWashStopTime_InternalState                     MenuManager_GetInternalDataPtr(MENUMANAGER_STDWASHSTOPTIME_INTERNALSTATE_IDX)
#define MenuManager_StdWashStopTime_Counter                           MenuManager_GetInternalDataPtr(MENUMANAGER_STDWASHSTOPTIME_COUNTER_IDX)
#define MenuManager_StdWashStopTime_CurPos                            MenuManager_GetInternalDataPtr(MENUMANAGER_STDWASHSTOPTIME_CURPOS_IDX)
#define MenuManager_StdWashStopTime_ValueMin                          MenuManager_GetInternalDataPtr(MENUMANAGER_STDWASHSTOPTIME_VALUE_MIN_IDX)
#define MenuManager_StdWashStopTime_ValueMax                          MenuManager_GetInternalDataPtr(MENUMANAGER_STDWASHSTOPTIME_VALUE_MAX_IDX)
#define MenuManager_StdWashStopTime_Value                             MenuManager_GetInternalDataPtr(MENUMANAGER_STDWASHSTOPTIME_VALUE_IDX)
#define MenuManager_StdWashStopTime_UnitVal(idx)                      MenuManager_GetInternalDataPtr(MENUMANAGER_STDWASHSTOPTIME_UNITVAL_IDX + idx)



#define MENUMANAGER_STDWASHSTOPTIME_INTERNALSTATE_READY               (uint32_t)0U
#define MENUMANAGER_STDWASHSTOPTIME_INTERNALSTATE_RUNNING             (uint32_t)1U
#define MENUMANAGER_STDWASHSTOPTIME_INTERNALSTATE_DONE                (uint32_t)2U
#define MENUMANAGER_STDWASHSTOPTIME_INTERNALSTATE_EXITING             (uint32_t)3U

#define MENUMANAGER_STDWASHSTOPTIME_COUNTER_MAX                       (uint32_t)50U

#define MENUMANAGER_STDWASHSTOPTIME_CURPOS_XPOS                       (uint32_t)5U
#define MENUMANAGER_STDWASHSTOPTIME_CURPOS_YPOS                       (uint32_t)5U

#define MENUMANAGER_STDWASHSTOPTIME_UNIT_XPOS                         (uint32_t)9U
#define MENUMANAGER_STDWASHSTOPTIME_UNIT_YPOS                         (uint32_t)5U

#define MENUMANAGER_STDWASHSTOPTIME_UNITVAL_LENGTH                    (uint32_t)3U



/** Menu manager main titles and child menu titles */
static const uint8_t MenuManager_StdWashStopTime_MainTitleStr[] =     "STD WASH STOP T";



/** Menu manager button event mapping array */
static MenuManager_ButEventMapStruct MenuManager_StdWashStopTime_ButEventMap[6] =
{
  { IOMANAGER_BUTTON_STATE_START,                                     MENUMANAGER_EVENT_START_BUT             },
  { IOMANAGER_BUTTON_STATE_STOP,                                      MENUMANAGER_EVENT_STOP_BUT              },
  { IOMANAGER_BUTTON_STATE_UP,                                        MENUMANAGER_EVENT_UP_BUT                },
  { IOMANAGER_BUTTON_STATE_DOWN,                                      MENUMANAGER_EVENT_DOWN_BUT              },
  { IOMANAGER_BUTTON_STATE_ADD,                                       MENUMANAGER_EVENT_ADD_BUT               },
  { IOMANAGER_BUTTON_STATE_SUB,                                       MENUMANAGER_EVENT_SUB_BUT               }
};

/** Menu manager button event mapping configuration */
static MenuManager_ButEventMapConfStruct MenuManager_StdWashStopTime_ButEventMapConf =
{
  .butEventMapNum             = (uint8_t)6U,
  .butEventMapCfg             = &MenuManager_StdWashStopTime_ButEventMap
};



/** Menu manager event handlers */
static Fsm_GuardType MenuManager_StdWashStopTime_Entry                (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_StdWashStopTime_Exit                 (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_StdWashStopTime_StartBut             (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_StdWashStopTime_StopBut              (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_StdWashStopTime_UpBut                (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_StdWashStopTime_DownBut              (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_StdWashStopTime_AddBut               (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_StdWashStopTime_SubBut               (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);

/** Menu manager state machine */
Fsm_EventEntryStruct MenuManager_StdWashStopTime_StateMachine[9] =
{
  FSM_TRIGGER_ENTRY             (                                     MenuManager_StdWashStopTime_Entry                                               ),
  FSM_TRIGGER_EXIT              (                                     MenuManager_StdWashStopTime_Exit                                                ),
  FSM_TRIGGER_TRANSITION        ( MENUMANAGER_EVENT_PREV,                                                     MENUMANAGER_STATE_WASH_SETUP            ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_START_BUT,        MenuManager_StdWashStopTime_StartBut                                            ),
  FSM_TRIGGER_TRANSITION_ACTION ( MENUMANAGER_EVENT_STOP_BUT,         MenuManager_StdWashStopTime_StopBut,    MENUMANAGER_STATE_WASH_SETUP            ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_UP_BUT,           MenuManager_StdWashStopTime_UpBut                                               ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_DOWN_BUT,         MenuManager_StdWashStopTime_DownBut                                             ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_ADD_BUT,          MenuManager_StdWashStopTime_AddBut                                              ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_SUB_BUT,          MenuManager_StdWashStopTime_SubBut                                              )
};



/*===============================================================================================
*                                   LOCAL FUNCTION PROTOTYPES
===============================================================================================*/

static void MenuManager_StdWashStopTime_LcdShowMainTitle(void);
static void MenuManager_StdWashStopTime_LcdShowAdjust(void);
static void MenuManager_StdWashStopTime_LcdShowDone(void);

static void MenuManager_StdWashStopTime_SubMainFunction(void);
static void MenuManager_StdWashStopTime_SubTickHandler(void);



/*===============================================================================================
*                                       LOCAL FUNCTIONS
===============================================================================================*/

static void MenuManager_StdWashStopTime_LcdShowMainTitle(void)
{
  /* Print main title */
  MenuManager_Common_LcdClearAdjustMenuStatic();
  MenuManager_Common_LcdShowMainTitle(MenuManager_StdWashStopTime_MainTitleStr);
  MenuManager_Common_LcdShowOkExitChoice();
  MenuManager_Common_LcdUpdateAdjustMenuStatic();
}

/*=============================================================================================*/
static void MenuManager_StdWashStopTime_LcdShowAdjust(void)
{
  MenuManager_Common_LcdClearAdjustMenuDynamic();

  MenuManager_Common_LcdShowAdjustUnitVal
  (
    MENUMANAGER_STDWASHSTOPTIME_CURPOS_XPOS,
    MENUMANAGER_STDWASHSTOPTIME_CURPOS_YPOS,
    &(MenuManager_StdWashStopTime_UnitVal(0)),
    MENUMANAGER_STDWASHSTOPTIME_UNITVAL_LENGTH
  );
  
  MenuManager_Common_LcdShowAdjustArrow
  (
    MENUMANAGER_STDWASHSTOPTIME_CURPOS_XPOS,
    MENUMANAGER_STDWASHSTOPTIME_CURPOS_YPOS,
    MenuManager_StdWashStopTime_CurPos
  );

  LCD_SetCursorPos(MENUMANAGER_STDWASHSTOPTIME_UNIT_XPOS, MENUMANAGER_STDWASHSTOPTIME_UNIT_YPOS, LCD_CURSOR_BY_FONT);
  LCD_PutString((uint8_t *)MenuManager_Common_SecondStr);

  MenuManager_Common_LcdUpdateAdjustMenuDynamic();
}

/*=============================================================================================*/
static void MenuManager_StdWashStopTime_LcdShowDone(void)
{
  MenuManager_Common_LcdClearAll();

  MenuManager_Common_LcdShowAdjustUnitVal
  (
    MENUMANAGER_STDWASHSTOPTIME_CURPOS_XPOS,
    MENUMANAGER_STDWASHSTOPTIME_CURPOS_YPOS,
    &(MenuManager_StdWashStopTime_UnitVal(0)),
    MENUMANAGER_STDWASHSTOPTIME_UNITVAL_LENGTH
  );

  LCD_SetCursorPos(MENUMANAGER_STDWASHSTOPTIME_UNIT_XPOS, MENUMANAGER_STDWASHSTOPTIME_UNIT_YPOS, LCD_CURSOR_BY_FONT);
  LCD_PutString((uint8_t *)MenuManager_Common_SecondStr);

  MenuManager_Common_LcdUpdateAll();
}



/*=============================================================================================*/
static Fsm_GuardType MenuManager_StdWashStopTime_Entry(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
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

      MenuManager_InternalDataPush(MENUMANAGER_STDWASHSTOPTIME_INTERNALDATALENGTH);

      MenuManager_StdWashStopTime_InternalState = MENUMANAGER_STDWASHSTOPTIME_INTERNALSTATE_READY;
      MenuManager_StdWashStopTime_Counter = (uint32_t)0U;
      MenuManager_StdWashStopTime_CurPos = (uint32_t)0U;

      MenuManager_StdWashStopTime_ValueMin = (uint32_t)PROGRAMMANAGER_WASHSETUP_COMMON_TIME_MIN;
      MenuManager_StdWashStopTime_ValueMax = (uint32_t)PROGRAMMANAGER_WASHSETUP_COMMON_TIME_MAX;
      MenuManager_StdWashStopTime_Value = (uint32_t)(ProgramManager_gParamConfig.washCfg.stdWashStopTime);

      MenuManager_Common_DecToBcdConv
      (
        &(MenuManager_StdWashStopTime_Value),
        &(MenuManager_StdWashStopTime_UnitVal(0)),
        (uint8_t)MENUMANAGER_STDWASHSTOPTIME_UNITVAL_LENGTH
      );
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
    MenuManager_StdWashStopTime_LcdShowMainTitle();
    MenuManager_StdWashStopTime_LcdShowAdjust();

    MenuManager_SubMainFunction = MenuManager_StdWashStopTime_SubMainFunction;
    MenuManager_SubTickHandler = MenuManager_StdWashStopTime_SubTickHandler;

    return FSM_GUARD_TRUE;
  }

  return FSM_GUARD_FALSE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_StdWashStopTime_Exit(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  Fsm_DataHierachyStruct* dataHierachy;

  MenuManager_SubMainFunction = NULL;
  MenuManager_SubTickHandler = NULL;

  dataHierachy = (Fsm_DataHierachyStruct *)MenuManager_malloc(sizeof(Fsm_DataHierachyStruct));
  dataHierachy->dataId = MENUMANAGER_STATE_STD_WASH_STOP_TIME;

  pFsmContext->dataHierachy = dataHierachy;

  /* Free internal data */
  MenuManager_InternalDataPop();
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_StdWashStopTime_StartBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_StdWashStopTime_InternalState == MENUMANAGER_STDWASHSTOPTIME_INTERNALSTATE_READY)
  {
    MenuManager_StdWashStopTime_InternalState = MENUMANAGER_STDWASHSTOPTIME_INTERNALSTATE_RUNNING;
  }
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_StdWashStopTime_StopBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_StdWashStopTime_InternalState != MENUMANAGER_STDWASHSTOPTIME_INTERNALSTATE_READY)
  {
    return FSM_GUARD_FALSE;
  }
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_StdWashStopTime_UpBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  uint32_t tempUnitVal;
  uint32_t tempVal;

  if (MenuManager_StdWashStopTime_InternalState == MENUMANAGER_STDWASHSTOPTIME_INTERNALSTATE_READY)
  {
    tempUnitVal = MenuManager_StdWashStopTime_UnitVal(MenuManager_StdWashStopTime_CurPos);

    MenuManager_StdWashStopTime_UnitVal(MenuManager_StdWashStopTime_CurPos) += (uint32_t)1U;

    if (MenuManager_StdWashStopTime_UnitVal(MenuManager_StdWashStopTime_CurPos) > (uint32_t)9U)
    {
      MenuManager_StdWashStopTime_UnitVal(MenuManager_StdWashStopTime_CurPos) = (uint32_t)0U;
    }

    MenuManager_Common_BcdToDecConv(&tempVal, &(MenuManager_StdWashStopTime_UnitVal(0)), (uint8_t)MENUMANAGER_STDWASHSTOPTIME_UNITVAL_LENGTH);

    if ((tempVal <= MenuManager_StdWashStopTime_ValueMin) || (tempVal >= MenuManager_StdWashStopTime_ValueMax))
    {
      MenuManager_StdWashStopTime_UnitVal(MenuManager_StdWashStopTime_CurPos) = tempUnitVal;
    }

    MenuManager_StdWashStopTime_LcdShowAdjust();
  }

  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_StdWashStopTime_DownBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  uint32_t tempUnitVal;
  uint32_t tempVal;

  if (MenuManager_StdWashStopTime_InternalState == MENUMANAGER_STDWASHSTOPTIME_INTERNALSTATE_READY)
  {
    tempUnitVal = MenuManager_StdWashStopTime_UnitVal(MenuManager_StdWashStopTime_CurPos);

    MenuManager_StdWashStopTime_UnitVal(MenuManager_StdWashStopTime_CurPos) -= (uint32_t)1U;

    if (MenuManager_StdWashStopTime_UnitVal(MenuManager_StdWashStopTime_CurPos) > (uint32_t)9U)
    {
      MenuManager_StdWashStopTime_UnitVal(MenuManager_StdWashStopTime_CurPos) = (uint32_t)9U;
    }

    MenuManager_Common_BcdToDecConv(&tempVal, &(MenuManager_StdWashStopTime_UnitVal(0)), (uint8_t)MENUMANAGER_STDWASHSTOPTIME_UNITVAL_LENGTH);

    if ((tempVal <= MenuManager_StdWashStopTime_ValueMin) || (tempVal >= MenuManager_StdWashStopTime_ValueMax))
    {
      MenuManager_StdWashStopTime_UnitVal(MenuManager_StdWashStopTime_CurPos) = tempUnitVal;
    }

    MenuManager_StdWashStopTime_LcdShowAdjust();
  }
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_StdWashStopTime_AddBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_StdWashStopTime_InternalState == MENUMANAGER_STDWASHSTOPTIME_INTERNALSTATE_READY)
  {
    if (MenuManager_StdWashStopTime_CurPos > (uint32_t)0U)
    {
      MenuManager_StdWashStopTime_CurPos -= (uint32_t)1U;
    }

    MenuManager_StdWashStopTime_LcdShowAdjust();
  }
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_StdWashStopTime_SubBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_StdWashStopTime_InternalState == MENUMANAGER_STDWASHSTOPTIME_INTERNALSTATE_READY)
  {
    if (MenuManager_StdWashStopTime_CurPos < (MENUMANAGER_STDWASHSTOPTIME_UNITVAL_LENGTH - (uint32_t)1U))
    {
      MenuManager_StdWashStopTime_CurPos += (uint32_t)1U;
    }
    
    MenuManager_StdWashStopTime_LcdShowAdjust();
  }
  
  return FSM_GUARD_TRUE;
}



/*=============================================================================================*/
static void MenuManager_StdWashStopTime_SubMainFunction(void)
{
  MenuManager_Common_ButEventMapHandler(&MenuManager_StdWashStopTime_ButEventMapConf);

  switch (MenuManager_StdWashStopTime_InternalState)
  {
    case MENUMANAGER_STDWASHSTOPTIME_INTERNALSTATE_READY:
      break;
    case MENUMANAGER_STDWASHSTOPTIME_INTERNALSTATE_RUNNING:
    {
      uint16_t tempStdWashStopTime;

      MenuManager_Common_BcdToDecConv(&(MenuManager_StdWashStopTime_Value), &(MenuManager_StdWashStopTime_UnitVal(0)), (uint8_t)MENUMANAGER_STDWASHSTOPTIME_UNITVAL_LENGTH);

      tempStdWashStopTime = (uint16_t)MenuManager_StdWashStopTime_Value;

      ProgramManager_WashSetup_StdWashStopTime_SetData(&tempStdWashStopTime);

      ProgramManager_WashSetup_StdWashStopTime_GetData(&(ProgramManager_gParamConfig.washCfg.stdWashStopTime));

      MenuManager_StdWashStopTime_InternalState = MENUMANAGER_STDWASHSTOPTIME_INTERNALSTATE_DONE;
      
      break;
    }
    case MENUMANAGER_STDWASHSTOPTIME_INTERNALSTATE_DONE:
    {
      MenuManager_StdWashStopTime_LcdShowDone();

      MenuManager_StdWashStopTime_InternalState = MENUMANAGER_STDWASHSTOPTIME_INTERNALSTATE_EXITING;

      break;
    }
    case MENUMANAGER_STDWASHSTOPTIME_INTERNALSTATE_EXITING:
    default:
      break;
  }
}

/*=============================================================================================*/
static void MenuManager_StdWashStopTime_SubTickHandler(void)
{
  if (MenuManager_StdWashStopTime_InternalState == MENUMANAGER_STDWASHSTOPTIME_INTERNALSTATE_EXITING)
  {
    MenuManager_StdWashStopTime_Counter += (uint32_t)1U;

    if (MenuManager_StdWashStopTime_Counter >= MENUMANAGER_STDWASHSTOPTIME_COUNTER_MAX)
    {
      MenuManager_StdWashStopTime_Counter = (uint32_t)0U;
      
      Fsm_TriggerEvent(&MenuManager_FsmContext, (Fsm_EventType)MENUMANAGER_EVENT_PREV);
    }
  }
}



#ifdef  __cplusplus
}
#endif
