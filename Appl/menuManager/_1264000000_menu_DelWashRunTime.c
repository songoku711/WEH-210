/* 
 * File:   _1264000000_menu_DelWashRunTime.c
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
#define MENUMANAGER_DELWASHRUNTIME_INTERNALDATALENGTH                 (uint8_t)10U

#define MENUMANAGER_DELWASHRUNTIME_INTERNALSTATE_IDX                  0U
#define MENUMANAGER_DELWASHRUNTIME_COUNTER_IDX                        1U
#define MENUMANAGER_DELWASHRUNTIME_CURPOS_IDX                         2U
#define MENUMANAGER_DELWASHRUNTIME_VALUE_MIN_IDX                      3U
#define MENUMANAGER_DELWASHRUNTIME_VALUE_MAX_IDX                      4U
#define MENUMANAGER_DELWASHRUNTIME_VALUE_IDX                          5U
#define MENUMANAGER_DELWASHRUNTIME_UNITVAL_IDX                        6U

#define MenuManager_DelWashRunTime_InternalState                      MenuManager_GetInternalDataPtr(MENUMANAGER_DELWASHRUNTIME_INTERNALSTATE_IDX)
#define MenuManager_DelWashRunTime_Counter                            MenuManager_GetInternalDataPtr(MENUMANAGER_DELWASHRUNTIME_COUNTER_IDX)
#define MenuManager_DelWashRunTime_CurPos                             MenuManager_GetInternalDataPtr(MENUMANAGER_DELWASHRUNTIME_CURPOS_IDX)
#define MenuManager_DelWashRunTime_ValueMin                           MenuManager_GetInternalDataPtr(MENUMANAGER_DELWASHRUNTIME_VALUE_MIN_IDX)
#define MenuManager_DelWashRunTime_ValueMax                           MenuManager_GetInternalDataPtr(MENUMANAGER_DELWASHRUNTIME_VALUE_MAX_IDX)
#define MenuManager_DelWashRunTime_Value                              MenuManager_GetInternalDataPtr(MENUMANAGER_DELWASHRUNTIME_VALUE_IDX)
#define MenuManager_DelWashRunTime_UnitVal(idx)                       MenuManager_GetInternalDataPtr(MENUMANAGER_DELWASHRUNTIME_UNITVAL_IDX + idx)



#define MENUMANAGER_DELWASHRUNTIME_INTERNALSTATE_READY                (uint32_t)0U
#define MENUMANAGER_DELWASHRUNTIME_INTERNALSTATE_RUNNING              (uint32_t)1U
#define MENUMANAGER_DELWASHRUNTIME_INTERNALSTATE_DONE                 (uint32_t)2U
#define MENUMANAGER_DELWASHRUNTIME_INTERNALSTATE_EXITING              (uint32_t)3U

#define MENUMANAGER_DELWASHRUNTIME_COUNTER_MAX                        (uint32_t)50U

#define MENUMANAGER_DELWASHRUNTIME_CURPOS_XPOS                        (uint32_t)5U
#define MENUMANAGER_DELWASHRUNTIME_CURPOS_YPOS                        (uint32_t)5U

#define MENUMANAGER_DELWASHRUNTIME_UNIT_XPOS                          (uint32_t)9U
#define MENUMANAGER_DELWASHRUNTIME_UNIT_YPOS                          (uint32_t)5U

#define MENUMANAGER_DELWASHRUNTIME_UNITVAL_LENGTH                     (uint32_t)3U



/** Menu manager main titles and child menu titles */
static const uint8_t MenuManager_DelWashRunTime_MainTitleStr[] =      "DEL WASH RUN T";



/** Menu manager button event mapping array */
static MenuManager_ButEventMapStruct MenuManager_DelWashRunTime_ButEventMap[6] =
{
  { IOMANAGER_BUTTON_STATE_START,                                     MENUMANAGER_EVENT_START_BUT             },
  { IOMANAGER_BUTTON_STATE_STOP,                                      MENUMANAGER_EVENT_STOP_BUT              },
  { IOMANAGER_BUTTON_STATE_UP,                                        MENUMANAGER_EVENT_UP_BUT                },
  { IOMANAGER_BUTTON_STATE_DOWN,                                      MENUMANAGER_EVENT_DOWN_BUT              },
  { IOMANAGER_BUTTON_STATE_ADD,                                       MENUMANAGER_EVENT_ADD_BUT               },
  { IOMANAGER_BUTTON_STATE_SUB,                                       MENUMANAGER_EVENT_SUB_BUT               }
};

/** Menu manager button event mapping configuration */
static MenuManager_ButEventMapConfStruct MenuManager_DelWashRunTime_ButEventMapConf =
{
  .butEventMapNum             = (uint8_t)6U,
  .butEventMapCfg             = &MenuManager_DelWashRunTime_ButEventMap
};



/** Menu manager event handlers */
static Fsm_GuardType MenuManager_DelWashRunTime_Entry                     (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_DelWashRunTime_Exit                      (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_DelWashRunTime_Submenu1                  (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_DelWashRunTime_StartBut                  (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_DelWashRunTime_StopBut                   (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_DelWashRunTime_UpBut                     (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_DelWashRunTime_DownBut                   (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_DelWashRunTime_AddBut                    (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_DelWashRunTime_SubBut                    (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);

/** Menu manager state machine */
Fsm_EventEntryStruct MenuManager_DelWashRunTime_StateMachine[9] =
{
  FSM_TRIGGER_ENTRY             (                                     MenuManager_DelWashRunTime_Entry                                                ),
  FSM_TRIGGER_EXIT              (                                     MenuManager_DelWashRunTime_Exit                                                 ),
  FSM_TRIGGER_TRANSITION_ACTION ( MENUMANAGER_EVENT_SUBMENU_1,        MenuManager_DelWashRunTime_Submenu1,    MENUMANAGER_STATE_WASH_SETUP            ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_START_BUT,        MenuManager_DelWashRunTime_StartBut                                             ),
  FSM_TRIGGER_TRANSITION_ACTION ( MENUMANAGER_EVENT_STOP_BUT,         MenuManager_DelWashRunTime_StopBut,     MENUMANAGER_STATE_WASH_SETUP            ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_UP_BUT,           MenuManager_DelWashRunTime_UpBut                                                ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_DOWN_BUT,         MenuManager_DelWashRunTime_DownBut                                              ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_ADD_BUT,          MenuManager_DelWashRunTime_AddBut                                               ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_SUB_BUT,          MenuManager_DelWashRunTime_SubBut                                               )
};



/*===============================================================================================
*                                   LOCAL FUNCTION PROTOTYPES
===============================================================================================*/

static void MenuManager_DelWashRunTime_LcdShowMainTitle(void);
static void MenuManager_DelWashRunTime_LcdShowAdjust(void);
static void MenuManager_DelWashRunTime_LcdShowDone(void);

static void MenuManager_DelWashRunTime_SubMainFunction(void);
static void MenuManager_DelWashRunTime_SubTickHandler(void);



/*===============================================================================================
*                                       LOCAL FUNCTIONS
===============================================================================================*/

static void MenuManager_DelWashRunTime_LcdShowMainTitle(void)
{
  /* Print main title */
  MenuManager_Common_LcdClearAdjustMenuStatic();
  MenuManager_Common_LcdShowMainTitle(MenuManager_DelWashRunTime_MainTitleStr);
  MenuManager_Common_LcdShowOkExitChoice();
  MenuManager_Common_LcdUpdateAdjustMenuStatic();
}

/*=============================================================================================*/
static void MenuManager_DelWashRunTime_LcdShowAdjust(void)
{
  MenuManager_Common_LcdClearAdjustMenuDynamic();

  MenuManager_Common_LcdShowAdjustUnitVal
  (
    MENUMANAGER_DELWASHRUNTIME_CURPOS_XPOS,
    MENUMANAGER_DELWASHRUNTIME_CURPOS_YPOS,
    &(MenuManager_DelWashRunTime_UnitVal(0)),
    MENUMANAGER_DELWASHRUNTIME_UNITVAL_LENGTH
  );
  
  MenuManager_Common_LcdShowAdjustArrow
  (
    MENUMANAGER_DELWASHRUNTIME_CURPOS_XPOS,
    MENUMANAGER_DELWASHRUNTIME_CURPOS_YPOS,
    MenuManager_DelWashRunTime_CurPos
  );

  LCD_SetCursorPos(MENUMANAGER_DELWASHRUNTIME_UNIT_XPOS, MENUMANAGER_DELWASHRUNTIME_UNIT_YPOS, LCD_CURSOR_BY_FONT);
  LCD_PutString((uint8_t *)MenuManager_Common_SecondStr);

  MenuManager_Common_LcdUpdateAdjustMenuDynamic();
}

/*=============================================================================================*/
static void MenuManager_DelWashRunTime_LcdShowDone(void)
{
  MenuManager_Common_LcdClearAll();

  MenuManager_Common_LcdShowAdjustUnitVal
  (
    MENUMANAGER_DELWASHRUNTIME_CURPOS_XPOS,
    MENUMANAGER_DELWASHRUNTIME_CURPOS_YPOS,
    &(MenuManager_DelWashRunTime_UnitVal(0)),
    MENUMANAGER_DELWASHRUNTIME_UNITVAL_LENGTH
  );

  LCD_SetCursorPos(MENUMANAGER_DELWASHRUNTIME_UNIT_XPOS, MENUMANAGER_DELWASHRUNTIME_UNIT_YPOS, LCD_CURSOR_BY_FONT);
  LCD_PutString((uint8_t *)MenuManager_Common_SecondStr);

  MenuManager_Common_LcdUpdateAll();
}



/*=============================================================================================*/
static Fsm_GuardType MenuManager_DelWashRunTime_Entry(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  MenuManager_SubMainFunction = MenuManager_DelWashRunTime_SubMainFunction;
  MenuManager_SubTickHandler = MenuManager_DelWashRunTime_SubTickHandler;

  /* Check if previous state data hierachy is not empty */
  if (pFsmContext->dataHierachy != NULL)
  {
    if (pFsmContext->dataHierachy->dataId == MENUMANAGER_STATE_WASH_SETUP)
    {
      /* Release previous state data hierachy */
      MenuManager_free(pFsmContext->dataHierachy);
      pFsmContext->dataHierachy = NULL;

      MenuManager_InternalDataPush(MENUMANAGER_DELWASHRUNTIME_INTERNALDATALENGTH);

      MenuManager_DelWashRunTime_InternalState = MENUMANAGER_DELWASHRUNTIME_INTERNALSTATE_READY;
      MenuManager_DelWashRunTime_Counter = (uint32_t)0U;
      MenuManager_DelWashRunTime_CurPos = (uint32_t)0U;

      MenuManager_DelWashRunTime_ValueMin = (uint32_t)PROGRAMMANAGER_WASHSETUP_COMMON_TIME_MIN;
      MenuManager_DelWashRunTime_ValueMax = (uint32_t)PROGRAMMANAGER_WASHSETUP_COMMON_TIME_MAX;
      MenuManager_DelWashRunTime_Value = (uint32_t)(ProgramManager_gParamConfig.washCfg.delWashRunTime);

      MenuManager_Common_DecToBcdConv
      (
        &(MenuManager_DelWashRunTime_Value),
        &(MenuManager_DelWashRunTime_UnitVal(0)),
        (uint8_t)MENUMANAGER_DELWASHRUNTIME_UNITVAL_LENGTH
      );
    }
    else
    {
      return FSM_GUARD_FALSE;
    }

    MenuManager_DelWashRunTime_LcdShowMainTitle();
    MenuManager_DelWashRunTime_LcdShowAdjust();

    return FSM_GUARD_TRUE;
  }

  return FSM_GUARD_FALSE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_DelWashRunTime_Exit(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  MenuManager_SubMainFunction = NULL;
  MenuManager_SubTickHandler = NULL;

  Fsm_DataHierachyStruct* dataHierachy;

  dataHierachy = (Fsm_DataHierachyStruct *)MenuManager_malloc(sizeof(Fsm_DataHierachyStruct));
  dataHierachy->dataId = MENUMANAGER_STATE_DEL_WASH_RUN_TIME;

  pFsmContext->dataHierachy = dataHierachy;

  /* Free internal data */
  MenuManager_InternalDataPop();
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_DelWashRunTime_Submenu1(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_DelWashRunTime_StartBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_DelWashRunTime_InternalState == MENUMANAGER_DELWASHRUNTIME_INTERNALSTATE_READY)
  {
    MenuManager_DelWashRunTime_InternalState = MENUMANAGER_DELWASHRUNTIME_INTERNALSTATE_RUNNING;
  }
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_DelWashRunTime_StopBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_DelWashRunTime_InternalState != MENUMANAGER_DELWASHRUNTIME_INTERNALSTATE_READY)
  {
    return FSM_GUARD_FALSE;
  }
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_DelWashRunTime_UpBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  uint32_t tempUnitVal;
  uint32_t tempVal;

  if (MenuManager_DelWashRunTime_InternalState == MENUMANAGER_DELWASHRUNTIME_INTERNALSTATE_READY)
  {
    tempUnitVal = MenuManager_DelWashRunTime_UnitVal(MenuManager_DelWashRunTime_CurPos);

    MenuManager_DelWashRunTime_UnitVal(MenuManager_DelWashRunTime_CurPos) += (uint32_t)1U;

    if (MenuManager_DelWashRunTime_UnitVal(MenuManager_DelWashRunTime_CurPos) > (uint32_t)9U)
    {
      MenuManager_DelWashRunTime_UnitVal(MenuManager_DelWashRunTime_CurPos) = (uint32_t)0U;
    }

    MenuManager_Common_BcdToDecConv(&tempVal, &(MenuManager_DelWashRunTime_UnitVal(0)), (uint8_t)MENUMANAGER_DELWASHRUNTIME_UNITVAL_LENGTH);

    if ((tempVal <= MenuManager_DelWashRunTime_ValueMin) || (tempVal >= MenuManager_DelWashRunTime_ValueMax))
    {
      MenuManager_DelWashRunTime_UnitVal(MenuManager_DelWashRunTime_CurPos) = tempUnitVal;
    }

    MenuManager_DelWashRunTime_LcdShowAdjust();
  }

  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_DelWashRunTime_DownBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  uint32_t tempUnitVal;
  uint32_t tempVal;

  if (MenuManager_DelWashRunTime_InternalState == MENUMANAGER_DELWASHRUNTIME_INTERNALSTATE_READY)
  {
    tempUnitVal = MenuManager_DelWashRunTime_UnitVal(MenuManager_DelWashRunTime_CurPos);

    MenuManager_DelWashRunTime_UnitVal(MenuManager_DelWashRunTime_CurPos) -= (uint32_t)1U;

    if (MenuManager_DelWashRunTime_UnitVal(MenuManager_DelWashRunTime_CurPos) > (uint32_t)9U)
    {
      MenuManager_DelWashRunTime_UnitVal(MenuManager_DelWashRunTime_CurPos) = (uint32_t)9U;
    }

    MenuManager_Common_BcdToDecConv(&tempVal, &(MenuManager_DelWashRunTime_UnitVal(0)), (uint8_t)MENUMANAGER_DELWASHRUNTIME_UNITVAL_LENGTH);

    if ((tempVal <= MenuManager_DelWashRunTime_ValueMin) || (tempVal >= MenuManager_DelWashRunTime_ValueMax))
    {
      MenuManager_DelWashRunTime_UnitVal(MenuManager_DelWashRunTime_CurPos) = tempUnitVal;
    }

    MenuManager_DelWashRunTime_LcdShowAdjust();
  }
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_DelWashRunTime_AddBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_DelWashRunTime_InternalState == MENUMANAGER_DELWASHRUNTIME_INTERNALSTATE_READY)
  {
    if (MenuManager_DelWashRunTime_CurPos > (uint32_t)0U)
    {
      MenuManager_DelWashRunTime_CurPos -= (uint32_t)1U;
    }

    MenuManager_DelWashRunTime_LcdShowAdjust();
  }
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_DelWashRunTime_SubBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_DelWashRunTime_InternalState == MENUMANAGER_DELWASHRUNTIME_INTERNALSTATE_READY)
  {
    if (MenuManager_DelWashRunTime_CurPos < (MENUMANAGER_DELWASHRUNTIME_UNITVAL_LENGTH - (uint32_t)1U))
    {
      MenuManager_DelWashRunTime_CurPos += (uint32_t)1U;
    }
    
    MenuManager_DelWashRunTime_LcdShowAdjust();
  }
  
  return FSM_GUARD_TRUE;
}



/*=============================================================================================*/
static void MenuManager_DelWashRunTime_SubMainFunction(void)
{
  MenuManager_Common_ButEventMapHandler(&MenuManager_DelWashRunTime_ButEventMapConf);

  switch (MenuManager_DelWashRunTime_InternalState)
  {
    case MENUMANAGER_DELWASHRUNTIME_INTERNALSTATE_READY:
      break;
    case MENUMANAGER_DELWASHRUNTIME_INTERNALSTATE_RUNNING:
    {
      uint16_t tempDelWashRunTime;

      MenuManager_Common_BcdToDecConv(&(MenuManager_DelWashRunTime_Value), &(MenuManager_DelWashRunTime_UnitVal(0)), (uint8_t)MENUMANAGER_DELWASHRUNTIME_UNITVAL_LENGTH);

      tempDelWashRunTime = (uint16_t)MenuManager_DelWashRunTime_Value;

      ProgramManager_WashSetup_DelWashRunTime_SetData(&tempDelWashRunTime);

      ProgramManager_WashSetup_DelWashRunTime_GetData(&(ProgramManager_gParamConfig.washCfg.delWashRunTime));

      MenuManager_DelWashRunTime_InternalState = MENUMANAGER_DELWASHRUNTIME_INTERNALSTATE_DONE;
      
      break;
    }
    case MENUMANAGER_DELWASHRUNTIME_INTERNALSTATE_DONE:
    {
      MenuManager_DelWashRunTime_LcdShowDone();

      MenuManager_DelWashRunTime_InternalState = MENUMANAGER_DELWASHRUNTIME_INTERNALSTATE_EXITING;

      break;
    }
    case MENUMANAGER_DELWASHRUNTIME_INTERNALSTATE_EXITING:
    default:
      break;
  }
}

/*=============================================================================================*/
static void MenuManager_DelWashRunTime_SubTickHandler(void)
{
  if (MenuManager_DelWashRunTime_InternalState == MENUMANAGER_DELWASHRUNTIME_INTERNALSTATE_EXITING)
  {
    MenuManager_DelWashRunTime_Counter += (uint32_t)1U;

    if (MenuManager_DelWashRunTime_Counter >= MENUMANAGER_DELWASHRUNTIME_COUNTER_MAX)
    {
      MenuManager_DelWashRunTime_Counter = (uint32_t)0U;
      
      Fsm_TriggerEvent(&MenuManager_FsmContext, (Fsm_EventType)MENUMANAGER_EVENT_SUBMENU_1);
    }
  }
}



#ifdef  __cplusplus
}
#endif
