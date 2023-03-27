/* 
 * File:   _1273000000_menu_MaxDrainExtrTime.c
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
#define MENUMANAGER_MAXDRAINEXTRTIME_INTERNALDATALENGTH               (uint8_t)10U

#define MENUMANAGER_MAXDRAINEXTRTIME_INTERNALSTATE_IDX                0U
#define MENUMANAGER_MAXDRAINEXTRTIME_COUNTER_IDX                      1U
#define MENUMANAGER_MAXDRAINEXTRTIME_CURPOS_IDX                       2U
#define MENUMANAGER_MAXDRAINEXTRTIME_VALUE_MIN_IDX                    3U
#define MENUMANAGER_MAXDRAINEXTRTIME_VALUE_MAX_IDX                    4U
#define MENUMANAGER_MAXDRAINEXTRTIME_VALUE_IDX                        5U
#define MENUMANAGER_MAXDRAINEXTRTIME_UNITVAL_IDX                      6U

#define MenuManager_MaxDrainExtrTime_InternalState                    MenuManager_GetInternalDataPtr(MENUMANAGER_MAXDRAINEXTRTIME_INTERNALSTATE_IDX)
#define MenuManager_MaxDrainExtrTime_Counter                          MenuManager_GetInternalDataPtr(MENUMANAGER_MAXDRAINEXTRTIME_COUNTER_IDX)
#define MenuManager_MaxDrainExtrTime_CurPos                           MenuManager_GetInternalDataPtr(MENUMANAGER_MAXDRAINEXTRTIME_CURPOS_IDX)
#define MenuManager_MaxDrainExtrTime_ValueMin                         MenuManager_GetInternalDataPtr(MENUMANAGER_MAXDRAINEXTRTIME_VALUE_MIN_IDX)
#define MenuManager_MaxDrainExtrTime_ValueMax                         MenuManager_GetInternalDataPtr(MENUMANAGER_MAXDRAINEXTRTIME_VALUE_MAX_IDX)
#define MenuManager_MaxDrainExtrTime_Value                            MenuManager_GetInternalDataPtr(MENUMANAGER_MAXDRAINEXTRTIME_VALUE_IDX)
#define MenuManager_MaxDrainExtrTime_UnitVal(idx)                     MenuManager_GetInternalDataPtr(MENUMANAGER_MAXDRAINEXTRTIME_UNITVAL_IDX + idx)



#define MENUMANAGER_MAXDRAINEXTRTIME_INTERNALSTATE_READY              (uint32_t)0U
#define MENUMANAGER_MAXDRAINEXTRTIME_INTERNALSTATE_RUNNING            (uint32_t)1U
#define MENUMANAGER_MAXDRAINEXTRTIME_INTERNALSTATE_DONE               (uint32_t)2U
#define MENUMANAGER_MAXDRAINEXTRTIME_INTERNALSTATE_EXITING            (uint32_t)3U

#define MENUMANAGER_MAXDRAINEXTRTIME_COUNTER_MAX                      (uint32_t)50U

#define MENUMANAGER_MAXDRAINEXTRTIME_CURPOS_XPOS                      (uint32_t)5U
#define MENUMANAGER_MAXDRAINEXTRTIME_CURPOS_YPOS                      (uint32_t)5U

#define MENUMANAGER_MAXDRAINEXTRTIME_UNIT_XPOS                        (uint32_t)10U
#define MENUMANAGER_MAXDRAINEXTRTIME_UNIT_YPOS                        (uint32_t)5U

#define MENUMANAGER_MAXDRAINEXTRTIME_UNITVAL_LENGTH                   (uint32_t)4U



/** Menu manager main titles and child menu titles */
static const uint8_t MenuManager_MaxDrainExtrTime_MainTitleStr[] =    "MAX DRAIN TIME";



/** Menu manager button event mapping array */
static MenuManager_ButEventMapStruct MenuManager_MaxDrainExtrTime_ButEventMap[6] =
{
  { IOMANAGER_BUTTON_STATE_START,                                     MENUMANAGER_EVENT_START_BUT             },
  { IOMANAGER_BUTTON_STATE_STOP,                                      MENUMANAGER_EVENT_STOP_BUT              },
  { IOMANAGER_BUTTON_STATE_UP,                                        MENUMANAGER_EVENT_UP_BUT                },
  { IOMANAGER_BUTTON_STATE_DOWN,                                      MENUMANAGER_EVENT_DOWN_BUT              },
  { IOMANAGER_BUTTON_STATE_ADD,                                       MENUMANAGER_EVENT_ADD_BUT               },
  { IOMANAGER_BUTTON_STATE_SUB,                                       MENUMANAGER_EVENT_SUB_BUT               }
};

/** Menu manager button event mapping configuration */
static MenuManager_ButEventMapConfStruct MenuManager_MaxDrainExtrTime_ButEventMapConf =
{
  .butEventMapNum             = (uint8_t)6U,
  .butEventMapCfg             = &MenuManager_MaxDrainExtrTime_ButEventMap
};



/** Menu manager event handlers */
static Fsm_GuardType MenuManager_MaxDrainExtrTime_Entry               (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_MaxDrainExtrTime_Exit                (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_MaxDrainExtrTime_StartBut            (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_MaxDrainExtrTime_StopBut             (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_MaxDrainExtrTime_UpBut               (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_MaxDrainExtrTime_DownBut             (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_MaxDrainExtrTime_AddBut              (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_MaxDrainExtrTime_SubBut              (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);

/** Menu manager state machine */
Fsm_EventEntryStruct MenuManager_MaxDrainExtrTime_StateMachine[9] =
{
  FSM_TRIGGER_ENTRY             (                                     MenuManager_MaxDrainExtrTime_Entry                                            ),
  FSM_TRIGGER_EXIT              (                                     MenuManager_MaxDrainExtrTime_Exit                                             ),
  FSM_TRIGGER_TRANSITION        ( MENUMANAGER_EVENT_PREV,                                                     MENUMANAGER_STATE_DRAIN_SETUP         ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_START_BUT,        MenuManager_MaxDrainExtrTime_StartBut                                         ),
  FSM_TRIGGER_TRANSITION_ACTION ( MENUMANAGER_EVENT_STOP_BUT,         MenuManager_MaxDrainExtrTime_StopBut,   MENUMANAGER_STATE_DRAIN_SETUP         ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_UP_BUT,           MenuManager_MaxDrainExtrTime_UpBut                                            ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_DOWN_BUT,         MenuManager_MaxDrainExtrTime_DownBut                                          ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_ADD_BUT,          MenuManager_MaxDrainExtrTime_AddBut                                           ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_SUB_BUT,          MenuManager_MaxDrainExtrTime_SubBut                                           )
};



/*===============================================================================================
*                                   LOCAL FUNCTION PROTOTYPES
===============================================================================================*/

static void MenuManager_MaxDrainExtrTime_LcdShowMainTitle(void);
static void MenuManager_MaxDrainExtrTime_LcdShowAdjust(void);
static void MenuManager_MaxDrainExtrTime_LcdShowDone(void);

static void MenuManager_MaxDrainExtrTime_SubMainFunction(void);
static void MenuManager_MaxDrainExtrTime_SubTickHandler(void);



/*===============================================================================================
*                                       LOCAL FUNCTIONS
===============================================================================================*/

static void MenuManager_MaxDrainExtrTime_LcdShowMainTitle(void)
{
  /* Print main title */
  MenuManager_Common_LcdClearAdjustMenuStatic();
  MenuManager_Common_LcdShowMainTitle(MenuManager_MaxDrainExtrTime_MainTitleStr);
  MenuManager_Common_LcdShowOkExitChoice();
  MenuManager_Common_LcdUpdateAdjustMenuStatic();
}

/*=============================================================================================*/
static void MenuManager_MaxDrainExtrTime_LcdShowAdjust(void)
{
  MenuManager_Common_LcdClearAdjustMenuDynamic();

  MenuManager_Common_LcdShowAdjustUnitVal
  (
    MENUMANAGER_MAXDRAINEXTRTIME_CURPOS_XPOS,
    MENUMANAGER_MAXDRAINEXTRTIME_CURPOS_YPOS,
    &(MenuManager_MaxDrainExtrTime_UnitVal(0)),
    MENUMANAGER_MAXDRAINEXTRTIME_UNITVAL_LENGTH
  );
  
  MenuManager_Common_LcdShowAdjustArrow
  (
    MENUMANAGER_MAXDRAINEXTRTIME_CURPOS_XPOS,
    MENUMANAGER_MAXDRAINEXTRTIME_CURPOS_YPOS,
    MenuManager_MaxDrainExtrTime_CurPos
  );

  LCD_SetCursorPos(MENUMANAGER_MAXDRAINEXTRTIME_UNIT_XPOS, MENUMANAGER_MAXDRAINEXTRTIME_UNIT_YPOS, LCD_CURSOR_BY_FONT);
  LCD_PutString((uint8_t *)MenuManager_Common_SecondStr);

  MenuManager_Common_LcdUpdateAdjustMenuDynamic();
}

/*=============================================================================================*/
static void MenuManager_MaxDrainExtrTime_LcdShowDone(void)
{
  MenuManager_Common_LcdClearAll();

  MenuManager_Common_LcdShowAdjustUnitVal
  (
    MENUMANAGER_MAXDRAINEXTRTIME_CURPOS_XPOS,
    MENUMANAGER_MAXDRAINEXTRTIME_CURPOS_YPOS,
    &(MenuManager_MaxDrainExtrTime_UnitVal(0)),
    MENUMANAGER_MAXDRAINEXTRTIME_UNITVAL_LENGTH
  );

  LCD_SetCursorPos(MENUMANAGER_MAXDRAINEXTRTIME_UNIT_XPOS, MENUMANAGER_MAXDRAINEXTRTIME_UNIT_YPOS, LCD_CURSOR_BY_FONT);
  LCD_PutString((uint8_t *)MenuManager_Common_SecondStr);

  MenuManager_Common_LcdUpdateAll();
}



/*=============================================================================================*/
static Fsm_GuardType MenuManager_MaxDrainExtrTime_Entry(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
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

      MenuManager_InternalDataPush(MENUMANAGER_MAXDRAINEXTRTIME_INTERNALDATALENGTH);

      MenuManager_MaxDrainExtrTime_InternalState = MENUMANAGER_MAXDRAINEXTRTIME_INTERNALSTATE_READY;
      MenuManager_MaxDrainExtrTime_Counter = (uint32_t)0U;
      MenuManager_MaxDrainExtrTime_CurPos = (uint32_t)0U;

      MenuManager_MaxDrainExtrTime_ValueMin = (uint32_t)PROGRAMMANAGER_DRAINSETUP_COMMON_TIME_MIN;
      MenuManager_MaxDrainExtrTime_ValueMax = (uint32_t)PROGRAMMANAGER_DRAINSETUP_COMMON_TIME_MAX;
      MenuManager_MaxDrainExtrTime_Value = (uint32_t)(ProgramManager_gParamConfig.drainCfg.maxDrainExtrTime);

      MenuManager_Common_DecToBcdConv
      (
        &(MenuManager_MaxDrainExtrTime_Value),
        &(MenuManager_MaxDrainExtrTime_UnitVal(0)),
        (uint8_t)MENUMANAGER_MAXDRAINEXTRTIME_UNITVAL_LENGTH
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
    MenuManager_MaxDrainExtrTime_LcdShowMainTitle();
    MenuManager_MaxDrainExtrTime_LcdShowAdjust();

    MenuManager_SubMainFunction = MenuManager_MaxDrainExtrTime_SubMainFunction;
    MenuManager_SubTickHandler = MenuManager_MaxDrainExtrTime_SubTickHandler;

    return FSM_GUARD_TRUE;
  }

  return FSM_GUARD_FALSE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_MaxDrainExtrTime_Exit(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  Fsm_DataHierachyStruct* dataHierachy;

  MenuManager_SubMainFunction = NULL;
  MenuManager_SubTickHandler = NULL;

  dataHierachy = (Fsm_DataHierachyStruct *)MenuManager_malloc(sizeof(Fsm_DataHierachyStruct));
  dataHierachy->dataId = MENUMANAGER_STATE_MAX_DRAIN_EXTRACT_TIME;

  pFsmContext->dataHierachy = dataHierachy;

  /* Free internal data */
  MenuManager_InternalDataPop();
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_MaxDrainExtrTime_StartBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_MaxDrainExtrTime_InternalState == MENUMANAGER_MAXDRAINEXTRTIME_INTERNALSTATE_READY)
  {
    MenuManager_MaxDrainExtrTime_InternalState = MENUMANAGER_MAXDRAINEXTRTIME_INTERNALSTATE_RUNNING;
  }
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_MaxDrainExtrTime_StopBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_MaxDrainExtrTime_InternalState != MENUMANAGER_MAXDRAINEXTRTIME_INTERNALSTATE_READY)
  {
    return FSM_GUARD_FALSE;
  }
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_MaxDrainExtrTime_UpBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  uint32_t tempUnitVal;
  uint32_t tempVal;

  if (MenuManager_MaxDrainExtrTime_InternalState == MENUMANAGER_MAXDRAINEXTRTIME_INTERNALSTATE_READY)
  {
    tempUnitVal = MenuManager_MaxDrainExtrTime_UnitVal(MenuManager_MaxDrainExtrTime_CurPos);

    MenuManager_MaxDrainExtrTime_UnitVal(MenuManager_MaxDrainExtrTime_CurPos) += (uint32_t)1U;

    if (MenuManager_MaxDrainExtrTime_UnitVal(MenuManager_MaxDrainExtrTime_CurPos) > (uint32_t)9U)
    {
      MenuManager_MaxDrainExtrTime_UnitVal(MenuManager_MaxDrainExtrTime_CurPos) = (uint32_t)0U;
    }

    MenuManager_Common_BcdToDecConv(&tempVal, &(MenuManager_MaxDrainExtrTime_UnitVal(0)), (uint8_t)MENUMANAGER_MAXDRAINEXTRTIME_UNITVAL_LENGTH);

    if ((tempVal <= MenuManager_MaxDrainExtrTime_ValueMin) || (tempVal >= MenuManager_MaxDrainExtrTime_ValueMax))
    {
      MenuManager_MaxDrainExtrTime_UnitVal(MenuManager_MaxDrainExtrTime_CurPos) = tempUnitVal;
    }

    MenuManager_MaxDrainExtrTime_LcdShowAdjust();
  }

  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_MaxDrainExtrTime_DownBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  uint32_t tempUnitVal;
  uint32_t tempVal;

  if (MenuManager_MaxDrainExtrTime_InternalState == MENUMANAGER_MAXDRAINEXTRTIME_INTERNALSTATE_READY)
  {
    tempUnitVal = MenuManager_MaxDrainExtrTime_UnitVal(MenuManager_MaxDrainExtrTime_CurPos);

    MenuManager_MaxDrainExtrTime_UnitVal(MenuManager_MaxDrainExtrTime_CurPos) -= (uint32_t)1U;

    if (MenuManager_MaxDrainExtrTime_UnitVal(MenuManager_MaxDrainExtrTime_CurPos) > (uint32_t)9U)
    {
      MenuManager_MaxDrainExtrTime_UnitVal(MenuManager_MaxDrainExtrTime_CurPos) = (uint32_t)9U;
    }

    MenuManager_Common_BcdToDecConv(&tempVal, &(MenuManager_MaxDrainExtrTime_UnitVal(0)), (uint8_t)MENUMANAGER_MAXDRAINEXTRTIME_UNITVAL_LENGTH);

    if ((tempVal <= MenuManager_MaxDrainExtrTime_ValueMin) || (tempVal >= MenuManager_MaxDrainExtrTime_ValueMax))
    {
      MenuManager_MaxDrainExtrTime_UnitVal(MenuManager_MaxDrainExtrTime_CurPos) = tempUnitVal;
    }

    MenuManager_MaxDrainExtrTime_LcdShowAdjust();
  }
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_MaxDrainExtrTime_AddBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_MaxDrainExtrTime_InternalState == MENUMANAGER_MAXDRAINEXTRTIME_INTERNALSTATE_READY)
  {
    if (MenuManager_MaxDrainExtrTime_CurPos > (uint32_t)0U)
    {
      MenuManager_MaxDrainExtrTime_CurPos -= (uint32_t)1U;
    }

    MenuManager_MaxDrainExtrTime_LcdShowAdjust();
  }
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_MaxDrainExtrTime_SubBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_MaxDrainExtrTime_InternalState == MENUMANAGER_MAXDRAINEXTRTIME_INTERNALSTATE_READY)
  {
    if (MenuManager_MaxDrainExtrTime_CurPos < (MENUMANAGER_MAXDRAINEXTRTIME_UNITVAL_LENGTH - (uint32_t)1U))
    {
      MenuManager_MaxDrainExtrTime_CurPos += (uint32_t)1U;
    }
    
    MenuManager_MaxDrainExtrTime_LcdShowAdjust();
  }
  
  return FSM_GUARD_TRUE;
}



/*=============================================================================================*/
static void MenuManager_MaxDrainExtrTime_SubMainFunction(void)
{
  MenuManager_Common_ButEventMapHandler(&MenuManager_MaxDrainExtrTime_ButEventMapConf);

  switch (MenuManager_MaxDrainExtrTime_InternalState)
  {
    case MENUMANAGER_MAXDRAINEXTRTIME_INTERNALSTATE_READY:
      break;
    case MENUMANAGER_MAXDRAINEXTRTIME_INTERNALSTATE_RUNNING:
    {
      uint16_t tempMaxDrainExtrTime;

      MenuManager_Common_BcdToDecConv(&(MenuManager_MaxDrainExtrTime_Value), &(MenuManager_MaxDrainExtrTime_UnitVal(0)), (uint8_t)MENUMANAGER_MAXDRAINEXTRTIME_UNITVAL_LENGTH);

      tempMaxDrainExtrTime = (uint16_t)MenuManager_MaxDrainExtrTime_Value;

      ProgramManager_DrainSetup_MaxDrainExtrTime_SetData(&tempMaxDrainExtrTime);

      ProgramManager_DrainSetup_MaxDrainExtrTime_GetData(&(ProgramManager_gParamConfig.drainCfg.maxDrainExtrTime));

      MenuManager_MaxDrainExtrTime_InternalState = MENUMANAGER_MAXDRAINEXTRTIME_INTERNALSTATE_DONE;
      
      break;
    }
    case MENUMANAGER_MAXDRAINEXTRTIME_INTERNALSTATE_DONE:
    {
      MenuManager_MaxDrainExtrTime_LcdShowDone();

      MenuManager_MaxDrainExtrTime_InternalState = MENUMANAGER_MAXDRAINEXTRTIME_INTERNALSTATE_EXITING;

      break;
    }
    case MENUMANAGER_MAXDRAINEXTRTIME_INTERNALSTATE_EXITING:
    default:
      break;
  }
}

/*=============================================================================================*/
static void MenuManager_MaxDrainExtrTime_SubTickHandler(void)
{
  if (MenuManager_MaxDrainExtrTime_InternalState == MENUMANAGER_MAXDRAINEXTRTIME_INTERNALSTATE_EXITING)
  {
    MenuManager_MaxDrainExtrTime_Counter += (uint32_t)1U;

    if (MenuManager_MaxDrainExtrTime_Counter >= MENUMANAGER_MAXDRAINEXTRTIME_COUNTER_MAX)
    {
      MenuManager_MaxDrainExtrTime_Counter = (uint32_t)0U;
      
      Fsm_TriggerEvent(&MenuManager_FsmContext, (Fsm_EventType)MENUMANAGER_EVENT_PREV);
    }
  }
}



#ifdef  __cplusplus
}
#endif
