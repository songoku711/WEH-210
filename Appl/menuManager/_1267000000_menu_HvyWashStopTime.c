/* 
 * File:   _1267000000_menu_HvyWashStopTime.c
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
#define MENUMANAGER_HVYWASHSTOPTIME_INTERNALDATALENGTH                (uint8_t)10U

#define MENUMANAGER_HVYWASHSTOPTIME_INTERNALSTATE_IDX                 0U
#define MENUMANAGER_HVYWASHSTOPTIME_COUNTER_IDX                       1U
#define MENUMANAGER_HVYWASHSTOPTIME_CURPOS_IDX                        2U
#define MENUMANAGER_HVYWASHSTOPTIME_VALUE_MIN_IDX                     3U
#define MENUMANAGER_HVYWASHSTOPTIME_VALUE_MAX_IDX                     4U
#define MENUMANAGER_HVYWASHSTOPTIME_VALUE_IDX                         5U
#define MENUMANAGER_HVYWASHSTOPTIME_UNITVAL_IDX                       6U

#define MenuManager_HvyWashStopTime_InternalState                     MenuManager_GetInternalDataPtr(MENUMANAGER_HVYWASHSTOPTIME_INTERNALSTATE_IDX)
#define MenuManager_HvyWashStopTime_Counter                           MenuManager_GetInternalDataPtr(MENUMANAGER_HVYWASHSTOPTIME_COUNTER_IDX)
#define MenuManager_HvyWashStopTime_CurPos                            MenuManager_GetInternalDataPtr(MENUMANAGER_HVYWASHSTOPTIME_CURPOS_IDX)
#define MenuManager_HvyWashStopTime_ValueMin                          MenuManager_GetInternalDataPtr(MENUMANAGER_HVYWASHSTOPTIME_VALUE_MIN_IDX)
#define MenuManager_HvyWashStopTime_ValueMax                          MenuManager_GetInternalDataPtr(MENUMANAGER_HVYWASHSTOPTIME_VALUE_MAX_IDX)
#define MenuManager_HvyWashStopTime_Value                             MenuManager_GetInternalDataPtr(MENUMANAGER_HVYWASHSTOPTIME_VALUE_IDX)
#define MenuManager_HvyWashStopTime_UnitVal(idx)                      MenuManager_GetInternalDataPtr(MENUMANAGER_HVYWASHSTOPTIME_UNITVAL_IDX + idx)



#define MENUMANAGER_HVYWASHSTOPTIME_INTERNALSTATE_READY               (uint32_t)0U
#define MENUMANAGER_HVYWASHSTOPTIME_INTERNALSTATE_RUNNING             (uint32_t)1U
#define MENUMANAGER_HVYWASHSTOPTIME_INTERNALSTATE_DONE                (uint32_t)2U
#define MENUMANAGER_HVYWASHSTOPTIME_INTERNALSTATE_EXITING             (uint32_t)3U

#define MENUMANAGER_HVYWASHSTOPTIME_COUNTER_MAX                       (uint32_t)50U

#define MENUMANAGER_HVYWASHSTOPTIME_CURPOS_XPOS                       (uint32_t)5U
#define MENUMANAGER_HVYWASHSTOPTIME_CURPOS_YPOS                       (uint32_t)5U

#define MENUMANAGER_HVYWASHSTOPTIME_UNIT_XPOS                         (uint32_t)9U
#define MENUMANAGER_HVYWASHSTOPTIME_UNIT_YPOS                         (uint32_t)5U

#define MENUMANAGER_HVYWASHSTOPTIME_UNITVAL_LENGTH                    (uint32_t)3U



/** Menu manager main titles and child menu titles */
static const uint8_t MenuManager_HvyWashStopTime_MainTitleStr[] =     "HVY WASH STOP T";



/** Menu manager button event mapping array */
static MenuManager_ButEventMapStruct MenuManager_HvyWashStopTime_ButEventMap[6] =
{
  { IOMANAGER_BUTTON_STATE_START,                                     MENUMANAGER_EVENT_START_BUT             },
  { IOMANAGER_BUTTON_STATE_STOP,                                      MENUMANAGER_EVENT_STOP_BUT              },
  { IOMANAGER_BUTTON_STATE_UP,                                        MENUMANAGER_EVENT_UP_BUT                },
  { IOMANAGER_BUTTON_STATE_DOWN,                                      MENUMANAGER_EVENT_DOWN_BUT              },
  { IOMANAGER_BUTTON_STATE_ADD,                                       MENUMANAGER_EVENT_ADD_BUT               },
  { IOMANAGER_BUTTON_STATE_SUB,                                       MENUMANAGER_EVENT_SUB_BUT               }
};

/** Menu manager button event mapping configuration */
static MenuManager_ButEventMapConfStruct MenuManager_HvyWashStopTime_ButEventMapConf =
{
  .butEventMapNum             = (uint8_t)6U,
  .butEventMapCfg             = &MenuManager_HvyWashStopTime_ButEventMap
};



/** Menu manager event handlers */
static Fsm_GuardType MenuManager_HvyWashStopTime_Entry                (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_HvyWashStopTime_Exit                 (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_HvyWashStopTime_StartBut             (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_HvyWashStopTime_StopBut              (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_HvyWashStopTime_UpBut                (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_HvyWashStopTime_DownBut              (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_HvyWashStopTime_AddBut               (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_HvyWashStopTime_SubBut               (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);

/** Menu manager state machine */
Fsm_EventEntryStruct MenuManager_HvyWashStopTime_StateMachine[9] =
{
  FSM_TRIGGER_ENTRY             (                                     MenuManager_HvyWashStopTime_Entry                                               ),
  FSM_TRIGGER_EXIT              (                                     MenuManager_HvyWashStopTime_Exit                                                ),
  FSM_TRIGGER_TRANSITION        ( MENUMANAGER_EVENT_PREV,                                                     MENUMANAGER_STATE_WASH_SETUP            ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_START_BUT,        MenuManager_HvyWashStopTime_StartBut                                            ),
  FSM_TRIGGER_TRANSITION_ACTION ( MENUMANAGER_EVENT_STOP_BUT,         MenuManager_HvyWashStopTime_StopBut,    MENUMANAGER_STATE_WASH_SETUP            ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_UP_BUT,           MenuManager_HvyWashStopTime_UpBut                                               ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_DOWN_BUT,         MenuManager_HvyWashStopTime_DownBut                                             ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_ADD_BUT,          MenuManager_HvyWashStopTime_AddBut                                              ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_SUB_BUT,          MenuManager_HvyWashStopTime_SubBut                                              )
};



/*===============================================================================================
*                                   LOCAL FUNCTION PROTOTYPES
===============================================================================================*/

static void MenuManager_HvyWashStopTime_LcdShowMainTitle(void);
static void MenuManager_HvyWashStopTime_LcdShowAdjust(void);
static void MenuManager_HvyWashStopTime_LcdShowDone(void);

static void MenuManager_HvyWashStopTime_SubMainFunction(void);
static void MenuManager_HvyWashStopTime_SubTickHandler(void);



/*===============================================================================================
*                                       LOCAL FUNCTIONS
===============================================================================================*/

static void MenuManager_HvyWashStopTime_LcdShowMainTitle(void)
{
  /* Print main title */
  MenuManager_Common_LcdClearAdjustMenuStatic();
  MenuManager_Common_LcdShowMainTitle(MenuManager_HvyWashStopTime_MainTitleStr);
  MenuManager_Common_LcdShowOkExitChoice();
  MenuManager_Common_LcdUpdateAdjustMenuStatic();
}

/*=============================================================================================*/
static void MenuManager_HvyWashStopTime_LcdShowAdjust(void)
{
  MenuManager_Common_LcdClearAdjustMenuDynamic();

  MenuManager_Common_LcdShowAdjustUnitVal
  (
    MENUMANAGER_HVYWASHSTOPTIME_CURPOS_XPOS,
    MENUMANAGER_HVYWASHSTOPTIME_CURPOS_YPOS,
    &(MenuManager_HvyWashStopTime_UnitVal(0)),
    MENUMANAGER_HVYWASHSTOPTIME_UNITVAL_LENGTH
  );
  
  MenuManager_Common_LcdShowAdjustArrow
  (
    MENUMANAGER_HVYWASHSTOPTIME_CURPOS_XPOS,
    MENUMANAGER_HVYWASHSTOPTIME_CURPOS_YPOS,
    MenuManager_HvyWashStopTime_CurPos
  );

  LCD_SetCursorPos(MENUMANAGER_HVYWASHSTOPTIME_UNIT_XPOS, MENUMANAGER_HVYWASHSTOPTIME_UNIT_YPOS, LCD_CURSOR_BY_FONT);
  LCD_PutString((uint8_t *)MenuManager_Common_SecondStr);

  MenuManager_Common_LcdUpdateAdjustMenuDynamic();
}

/*=============================================================================================*/
static void MenuManager_HvyWashStopTime_LcdShowDone(void)
{
  MenuManager_Common_LcdClearAll();

  MenuManager_Common_LcdShowAdjustUnitVal
  (
    MENUMANAGER_HVYWASHSTOPTIME_CURPOS_XPOS,
    MENUMANAGER_HVYWASHSTOPTIME_CURPOS_YPOS,
    &(MenuManager_HvyWashStopTime_UnitVal(0)),
    MENUMANAGER_HVYWASHSTOPTIME_UNITVAL_LENGTH
  );

  LCD_SetCursorPos(MENUMANAGER_HVYWASHSTOPTIME_UNIT_XPOS, MENUMANAGER_HVYWASHSTOPTIME_UNIT_YPOS, LCD_CURSOR_BY_FONT);
  LCD_PutString((uint8_t *)MenuManager_Common_SecondStr);

  MenuManager_Common_LcdUpdateAll();
}



/*=============================================================================================*/
static Fsm_GuardType MenuManager_HvyWashStopTime_Entry(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
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

      MenuManager_InternalDataPush(MENUMANAGER_HVYWASHSTOPTIME_INTERNALDATALENGTH);

      MenuManager_HvyWashStopTime_InternalState = MENUMANAGER_HVYWASHSTOPTIME_INTERNALSTATE_READY;
      MenuManager_HvyWashStopTime_Counter = (uint32_t)0U;
      MenuManager_HvyWashStopTime_CurPos = (uint32_t)0U;

      MenuManager_HvyWashStopTime_ValueMin = (uint32_t)PROGRAMMANAGER_WASHSETUP_COMMON_TIME_MIN;
      MenuManager_HvyWashStopTime_ValueMax = (uint32_t)PROGRAMMANAGER_WASHSETUP_COMMON_TIME_MAX;
      MenuManager_HvyWashStopTime_Value = (uint32_t)(ProgramManager_gParamConfig.washCfg.hvyWashStopTime);

      MenuManager_Common_DecToBcdConv
      (
        &(MenuManager_HvyWashStopTime_Value),
        &(MenuManager_HvyWashStopTime_UnitVal(0)),
        (uint8_t)MENUMANAGER_HVYWASHSTOPTIME_UNITVAL_LENGTH
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
    MenuManager_HvyWashStopTime_LcdShowMainTitle();
    MenuManager_HvyWashStopTime_LcdShowAdjust();

    MenuManager_SubMainFunction = MenuManager_HvyWashStopTime_SubMainFunction;
    MenuManager_SubTickHandler = MenuManager_HvyWashStopTime_SubTickHandler;

    return FSM_GUARD_TRUE;
  }

  return FSM_GUARD_FALSE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_HvyWashStopTime_Exit(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  Fsm_DataHierachyStruct* dataHierachy;

  MenuManager_SubMainFunction = NULL;
  MenuManager_SubTickHandler = NULL;

  dataHierachy = (Fsm_DataHierachyStruct *)MenuManager_malloc(sizeof(Fsm_DataHierachyStruct));
  dataHierachy->dataId = MENUMANAGER_STATE_HVY_WASH_STOP_TIME;

  pFsmContext->dataHierachy = dataHierachy;

  /* Free internal data */
  MenuManager_InternalDataPop();
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_HvyWashStopTime_StartBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_HvyWashStopTime_InternalState == MENUMANAGER_HVYWASHSTOPTIME_INTERNALSTATE_READY)
  {
    MenuManager_HvyWashStopTime_InternalState = MENUMANAGER_HVYWASHSTOPTIME_INTERNALSTATE_RUNNING;
  }
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_HvyWashStopTime_StopBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_HvyWashStopTime_InternalState != MENUMANAGER_HVYWASHSTOPTIME_INTERNALSTATE_READY)
  {
    return FSM_GUARD_FALSE;
  }
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_HvyWashStopTime_UpBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  uint32_t tempUnitVal;
  uint32_t tempVal;

  if (MenuManager_HvyWashStopTime_InternalState == MENUMANAGER_HVYWASHSTOPTIME_INTERNALSTATE_READY)
  {
    tempUnitVal = MenuManager_HvyWashStopTime_UnitVal(MenuManager_HvyWashStopTime_CurPos);

    MenuManager_HvyWashStopTime_UnitVal(MenuManager_HvyWashStopTime_CurPos) += (uint32_t)1U;

    if (MenuManager_HvyWashStopTime_UnitVal(MenuManager_HvyWashStopTime_CurPos) > (uint32_t)9U)
    {
      MenuManager_HvyWashStopTime_UnitVal(MenuManager_HvyWashStopTime_CurPos) = (uint32_t)0U;
    }

    MenuManager_Common_BcdToDecConv(&tempVal, &(MenuManager_HvyWashStopTime_UnitVal(0)), (uint8_t)MENUMANAGER_HVYWASHSTOPTIME_UNITVAL_LENGTH);

    if ((tempVal <= MenuManager_HvyWashStopTime_ValueMin) || (tempVal >= MenuManager_HvyWashStopTime_ValueMax))
    {
      MenuManager_HvyWashStopTime_UnitVal(MenuManager_HvyWashStopTime_CurPos) = tempUnitVal;
    }

    MenuManager_HvyWashStopTime_LcdShowAdjust();
  }

  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_HvyWashStopTime_DownBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  uint32_t tempUnitVal;
  uint32_t tempVal;

  if (MenuManager_HvyWashStopTime_InternalState == MENUMANAGER_HVYWASHSTOPTIME_INTERNALSTATE_READY)
  {
    tempUnitVal = MenuManager_HvyWashStopTime_UnitVal(MenuManager_HvyWashStopTime_CurPos);

    MenuManager_HvyWashStopTime_UnitVal(MenuManager_HvyWashStopTime_CurPos) -= (uint32_t)1U;

    if (MenuManager_HvyWashStopTime_UnitVal(MenuManager_HvyWashStopTime_CurPos) > (uint32_t)9U)
    {
      MenuManager_HvyWashStopTime_UnitVal(MenuManager_HvyWashStopTime_CurPos) = (uint32_t)9U;
    }

    MenuManager_Common_BcdToDecConv(&tempVal, &(MenuManager_HvyWashStopTime_UnitVal(0)), (uint8_t)MENUMANAGER_HVYWASHSTOPTIME_UNITVAL_LENGTH);

    if ((tempVal <= MenuManager_HvyWashStopTime_ValueMin) || (tempVal >= MenuManager_HvyWashStopTime_ValueMax))
    {
      MenuManager_HvyWashStopTime_UnitVal(MenuManager_HvyWashStopTime_CurPos) = tempUnitVal;
    }

    MenuManager_HvyWashStopTime_LcdShowAdjust();
  }
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_HvyWashStopTime_AddBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_HvyWashStopTime_InternalState == MENUMANAGER_HVYWASHSTOPTIME_INTERNALSTATE_READY)
  {
    if (MenuManager_HvyWashStopTime_CurPos > (uint32_t)0U)
    {
      MenuManager_HvyWashStopTime_CurPos -= (uint32_t)1U;
    }

    MenuManager_HvyWashStopTime_LcdShowAdjust();
  }
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_HvyWashStopTime_SubBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_HvyWashStopTime_InternalState == MENUMANAGER_HVYWASHSTOPTIME_INTERNALSTATE_READY)
  {
    if (MenuManager_HvyWashStopTime_CurPos < (MENUMANAGER_HVYWASHSTOPTIME_UNITVAL_LENGTH - (uint32_t)1U))
    {
      MenuManager_HvyWashStopTime_CurPos += (uint32_t)1U;
    }
    
    MenuManager_HvyWashStopTime_LcdShowAdjust();
  }
  
  return FSM_GUARD_TRUE;
}



/*=============================================================================================*/
static void MenuManager_HvyWashStopTime_SubMainFunction(void)
{
  MenuManager_Common_ButEventMapHandler(&MenuManager_HvyWashStopTime_ButEventMapConf);

  switch (MenuManager_HvyWashStopTime_InternalState)
  {
    case MENUMANAGER_HVYWASHSTOPTIME_INTERNALSTATE_READY:
      break;
    case MENUMANAGER_HVYWASHSTOPTIME_INTERNALSTATE_RUNNING:
    {
      uint16_t tempHvyWashStopTime;

      MenuManager_Common_BcdToDecConv(&(MenuManager_HvyWashStopTime_Value), &(MenuManager_HvyWashStopTime_UnitVal(0)), (uint8_t)MENUMANAGER_HVYWASHSTOPTIME_UNITVAL_LENGTH);

      tempHvyWashStopTime = (uint16_t)MenuManager_HvyWashStopTime_Value;

      ProgramManager_WashSetup_HvyWashStopTime_SetData(&tempHvyWashStopTime);

      ProgramManager_WashSetup_HvyWashStopTime_GetData(&(ProgramManager_gParamConfig.washCfg.hvyWashStopTime));

      MenuManager_HvyWashStopTime_InternalState = MENUMANAGER_HVYWASHSTOPTIME_INTERNALSTATE_DONE;
      
      break;
    }
    case MENUMANAGER_HVYWASHSTOPTIME_INTERNALSTATE_DONE:
    {
      MenuManager_HvyWashStopTime_LcdShowDone();

      MenuManager_HvyWashStopTime_InternalState = MENUMANAGER_HVYWASHSTOPTIME_INTERNALSTATE_EXITING;

      break;
    }
    case MENUMANAGER_HVYWASHSTOPTIME_INTERNALSTATE_EXITING:
    default:
      break;
  }
}

/*=============================================================================================*/
static void MenuManager_HvyWashStopTime_SubTickHandler(void)
{
  if (MenuManager_HvyWashStopTime_InternalState == MENUMANAGER_HVYWASHSTOPTIME_INTERNALSTATE_EXITING)
  {
    MenuManager_HvyWashStopTime_Counter += (uint32_t)1U;

    if (MenuManager_HvyWashStopTime_Counter >= MENUMANAGER_HVYWASHSTOPTIME_COUNTER_MAX)
    {
      MenuManager_HvyWashStopTime_Counter = (uint32_t)0U;
      
      Fsm_TriggerEvent(&MenuManager_FsmContext, (Fsm_EventType)MENUMANAGER_EVENT_PREV);
    }
  }
}



#ifdef  __cplusplus
}
#endif
