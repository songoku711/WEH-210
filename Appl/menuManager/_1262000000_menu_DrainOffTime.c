/* 
 * File:   _1272000000_menu_DrainOffTime.c
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
#define MENUMANAGER_DRAINOFFTIME_INTERNALDATALENGTH                   (uint8_t)10U

#define MENUMANAGER_DRAINOFFTIME_INTERNALSTATE_IDX                    0U
#define MENUMANAGER_DRAINOFFTIME_COUNTER_IDX                          1U
#define MENUMANAGER_DRAINOFFTIME_CURPOS_IDX                           2U
#define MENUMANAGER_DRAINOFFTIME_VALUE_MIN_IDX                        3U
#define MENUMANAGER_DRAINOFFTIME_VALUE_MAX_IDX                        4U
#define MENUMANAGER_DRAINOFFTIME_VALUE_IDX                            5U
#define MENUMANAGER_DRAINOFFTIME_UNITVAL_IDX                          6U

#define MenuManager_DrainOffTime_InternalState                        MenuManager_GetInternalDataPtr(MENUMANAGER_DRAINOFFTIME_INTERNALSTATE_IDX)
#define MenuManager_DrainOffTime_Counter                              MenuManager_GetInternalDataPtr(MENUMANAGER_DRAINOFFTIME_COUNTER_IDX)
#define MenuManager_DrainOffTime_CurPos                               MenuManager_GetInternalDataPtr(MENUMANAGER_DRAINOFFTIME_CURPOS_IDX)
#define MenuManager_DrainOffTime_ValueMin                             MenuManager_GetInternalDataPtr(MENUMANAGER_DRAINOFFTIME_VALUE_MIN_IDX)
#define MenuManager_DrainOffTime_ValueMax                             MenuManager_GetInternalDataPtr(MENUMANAGER_DRAINOFFTIME_VALUE_MAX_IDX)
#define MenuManager_DrainOffTime_Value                                MenuManager_GetInternalDataPtr(MENUMANAGER_DRAINOFFTIME_VALUE_IDX)
#define MenuManager_DrainOffTime_UnitVal(idx)                         MenuManager_GetInternalDataPtr(MENUMANAGER_DRAINOFFTIME_UNITVAL_IDX + idx)



#define MENUMANAGER_DRAINOFFTIME_INTERNALSTATE_READY                  (uint32_t)0U
#define MENUMANAGER_DRAINOFFTIME_INTERNALSTATE_RUNNING                (uint32_t)1U
#define MENUMANAGER_DRAINOFFTIME_INTERNALSTATE_DONE                   (uint32_t)2U
#define MENUMANAGER_DRAINOFFTIME_INTERNALSTATE_EXITING                (uint32_t)3U

#define MENUMANAGER_DRAINOFFTIME_COUNTER_MAX                          (uint32_t)50U

#define MENUMANAGER_DRAINOFFTIME_CURPOS_XPOS                          (uint32_t)5U
#define MENUMANAGER_DRAINOFFTIME_CURPOS_YPOS                          (uint32_t)5U

#define MENUMANAGER_DRAINOFFTIME_UNIT_XPOS                            (uint32_t)9U
#define MENUMANAGER_DRAINOFFTIME_UNIT_YPOS                            (uint32_t)5U

#define MENUMANAGER_DRAINOFFTIME_UNITVAL_LENGTH                       (uint32_t)3U



/** Menu manager main titles and child menu titles */
static const uint8_t MenuManager_DrainOffTime_MainTitleStr[] =        "DRAIN OFF TIME";



/** Menu manager button event mapping array */
static MenuManager_ButEventMapStruct MenuManager_DrainOffTime_ButEventMap[6] =
{
  { IOMANAGER_BUTTON_STATE_START,                                     MENUMANAGER_EVENT_START_BUT             },
  { IOMANAGER_BUTTON_STATE_STOP,                                      MENUMANAGER_EVENT_STOP_BUT              },
  { IOMANAGER_BUTTON_STATE_UP,                                        MENUMANAGER_EVENT_UP_BUT                },
  { IOMANAGER_BUTTON_STATE_DOWN,                                      MENUMANAGER_EVENT_DOWN_BUT              },
  { IOMANAGER_BUTTON_STATE_ADD,                                       MENUMANAGER_EVENT_ADD_BUT               },
  { IOMANAGER_BUTTON_STATE_SUB,                                       MENUMANAGER_EVENT_SUB_BUT               }
};

/** Menu manager button event mapping configuration */
static MenuManager_ButEventMapConfStruct MenuManager_DrainOffTime_ButEventMapConf =
{
  .butEventMapNum             = (uint8_t)6U,
  .butEventMapCfg             = &MenuManager_DrainOffTime_ButEventMap
};



/** Menu manager event handlers */
static Fsm_GuardType MenuManager_DrainOffTime_Entry                   (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_DrainOffTime_Exit                    (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_DrainOffTime_StartBut                (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_DrainOffTime_StopBut                 (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_DrainOffTime_UpBut                   (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_DrainOffTime_DownBut                 (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_DrainOffTime_AddBut                  (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_DrainOffTime_SubBut                  (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);

/** Menu manager state machine */
Fsm_EventEntryStruct MenuManager_DrainOffTime_StateMachine[9] =
{
  FSM_TRIGGER_ENTRY             (                                     MenuManager_DrainOffTime_Entry                                                ),
  FSM_TRIGGER_EXIT              (                                     MenuManager_DrainOffTime_Exit                                                 ),
  FSM_TRIGGER_TRANSITION        ( MENUMANAGER_EVENT_PREV,                                                     MENUMANAGER_STATE_DRAIN_SETUP         ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_START_BUT,        MenuManager_DrainOffTime_StartBut                                             ),
  FSM_TRIGGER_TRANSITION_ACTION ( MENUMANAGER_EVENT_STOP_BUT,         MenuManager_DrainOffTime_StopBut,       MENUMANAGER_STATE_DRAIN_SETUP         ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_UP_BUT,           MenuManager_DrainOffTime_UpBut                                                ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_DOWN_BUT,         MenuManager_DrainOffTime_DownBut                                              ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_ADD_BUT,          MenuManager_DrainOffTime_AddBut                                               ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_SUB_BUT,          MenuManager_DrainOffTime_SubBut                                               )
};



/*===============================================================================================
*                                   LOCAL FUNCTION PROTOTYPES
===============================================================================================*/

static void MenuManager_DrainOffTime_LcdShowMainTitle(void);
static void MenuManager_DrainOffTime_LcdShowAdjust(void);
static void MenuManager_DrainOffTime_LcdShowDone(void);

static void MenuManager_DrainOffTime_SubMainFunction(void);
static void MenuManager_DrainOffTime_SubTickHandler(void);



/*===============================================================================================
*                                       LOCAL FUNCTIONS
===============================================================================================*/

static void MenuManager_DrainOffTime_LcdShowMainTitle(void)
{
  /* Print main title */
  MenuManager_Common_LcdClearAdjustMenuStatic();
  MenuManager_Common_LcdShowMainTitle(MenuManager_DrainOffTime_MainTitleStr);
  MenuManager_Common_LcdShowOkExitChoice();
  MenuManager_Common_LcdUpdateAdjustMenuStatic();
}

/*=============================================================================================*/
static void MenuManager_DrainOffTime_LcdShowAdjust(void)
{
  MenuManager_Common_LcdClearAdjustMenuDynamic();

  MenuManager_Common_LcdShowAdjustUnitVal
  (
    MENUMANAGER_DRAINOFFTIME_CURPOS_XPOS,
    MENUMANAGER_DRAINOFFTIME_CURPOS_YPOS,
    &(MenuManager_DrainOffTime_UnitVal(0)),
    MENUMANAGER_DRAINOFFTIME_UNITVAL_LENGTH
  );
  
  MenuManager_Common_LcdShowAdjustArrow
  (
    MENUMANAGER_DRAINOFFTIME_CURPOS_XPOS,
    MENUMANAGER_DRAINOFFTIME_CURPOS_YPOS,
    MenuManager_DrainOffTime_CurPos
  );

  LCD_SetCursorPos(MENUMANAGER_DRAINOFFTIME_UNIT_XPOS, MENUMANAGER_DRAINOFFTIME_UNIT_YPOS, LCD_CURSOR_BY_FONT);
  LCD_PutString((uint8_t *)MenuManager_Common_SecondStr);

  MenuManager_Common_LcdUpdateAdjustMenuDynamic();
}

/*=============================================================================================*/
static void MenuManager_DrainOffTime_LcdShowDone(void)
{
  MenuManager_Common_LcdClearAll();

  MenuManager_Common_LcdShowAdjustUnitVal
  (
    MENUMANAGER_DRAINOFFTIME_CURPOS_XPOS,
    MENUMANAGER_DRAINOFFTIME_CURPOS_YPOS,
    &(MenuManager_DrainOffTime_UnitVal(0)),
    MENUMANAGER_DRAINOFFTIME_UNITVAL_LENGTH
  );

  LCD_SetCursorPos(MENUMANAGER_DRAINOFFTIME_UNIT_XPOS, MENUMANAGER_DRAINOFFTIME_UNIT_YPOS, LCD_CURSOR_BY_FONT);
  LCD_PutString((uint8_t *)MenuManager_Common_SecondStr);

  MenuManager_Common_LcdUpdateAll();
}



/*=============================================================================================*/
static Fsm_GuardType MenuManager_DrainOffTime_Entry(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
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

      MenuManager_InternalDataPush(MENUMANAGER_DRAINOFFTIME_INTERNALDATALENGTH);

      MenuManager_DrainOffTime_InternalState = MENUMANAGER_DRAINOFFTIME_INTERNALSTATE_READY;
      MenuManager_DrainOffTime_Counter = (uint32_t)0U;
      MenuManager_DrainOffTime_CurPos = (uint32_t)0U;

      MenuManager_DrainOffTime_ValueMin = (uint32_t)PROGRAMMANAGER_DRAINSETUP_DRAINOFFTIME_MIN;
      MenuManager_DrainOffTime_ValueMax = (uint32_t)PROGRAMMANAGER_DRAINSETUP_DRAINOFFTIME_MAX;
      MenuManager_DrainOffTime_Value = (uint32_t)(ProgramManager_gParamConfig.drainCfg.drainOffTime);

      MenuManager_Common_DecToBcdConv
      (
        &(MenuManager_DrainOffTime_Value),
        &(MenuManager_DrainOffTime_UnitVal(0)),
        (uint8_t)MENUMANAGER_DRAINOFFTIME_UNITVAL_LENGTH
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
    MenuManager_DrainOffTime_LcdShowMainTitle();
    MenuManager_DrainOffTime_LcdShowAdjust();

    MenuManager_SubMainFunction = MenuManager_DrainOffTime_SubMainFunction;
    MenuManager_SubTickHandler = MenuManager_DrainOffTime_SubTickHandler;

    return FSM_GUARD_TRUE;
  }

  return FSM_GUARD_FALSE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_DrainOffTime_Exit(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  Fsm_DataHierachyStruct* dataHierachy;

  MenuManager_SubMainFunction = NULL;
  MenuManager_SubTickHandler = NULL;

  dataHierachy = (Fsm_DataHierachyStruct *)MenuManager_malloc(sizeof(Fsm_DataHierachyStruct));
  dataHierachy->dataId = MENUMANAGER_STATE_DRAIN_OFF_TIME;

  pFsmContext->dataHierachy = dataHierachy;

  /* Free internal data */
  MenuManager_InternalDataPop();
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_DrainOffTime_StartBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_DrainOffTime_InternalState == MENUMANAGER_DRAINOFFTIME_INTERNALSTATE_READY)
  {
    MenuManager_DrainOffTime_InternalState = MENUMANAGER_DRAINOFFTIME_INTERNALSTATE_RUNNING;
  }
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_DrainOffTime_StopBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_DrainOffTime_InternalState != MENUMANAGER_DRAINOFFTIME_INTERNALSTATE_READY)
  {
    return FSM_GUARD_FALSE;
  }
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_DrainOffTime_UpBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  uint32_t tempUnitVal;
  uint32_t tempVal;

  if (MenuManager_DrainOffTime_InternalState == MENUMANAGER_DRAINOFFTIME_INTERNALSTATE_READY)
  {
    tempUnitVal = MenuManager_DrainOffTime_UnitVal(MenuManager_DrainOffTime_CurPos);

    MenuManager_DrainOffTime_UnitVal(MenuManager_DrainOffTime_CurPos) += (uint32_t)1U;

    if (MenuManager_DrainOffTime_UnitVal(MenuManager_DrainOffTime_CurPos) > (uint32_t)9U)
    {
      MenuManager_DrainOffTime_UnitVal(MenuManager_DrainOffTime_CurPos) = (uint32_t)0U;
    }

    MenuManager_Common_BcdToDecConv(&tempVal, &(MenuManager_DrainOffTime_UnitVal(0)), (uint8_t)MENUMANAGER_DRAINOFFTIME_UNITVAL_LENGTH);

    if ((tempVal <= MenuManager_DrainOffTime_ValueMin) || (tempVal >= MenuManager_DrainOffTime_ValueMax))
    {
      MenuManager_DrainOffTime_UnitVal(MenuManager_DrainOffTime_CurPos) = tempUnitVal;
    }

    MenuManager_DrainOffTime_LcdShowAdjust();
  }

  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_DrainOffTime_DownBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  uint32_t tempUnitVal;
  uint32_t tempVal;

  if (MenuManager_DrainOffTime_InternalState == MENUMANAGER_DRAINOFFTIME_INTERNALSTATE_READY)
  {
    tempUnitVal = MenuManager_DrainOffTime_UnitVal(MenuManager_DrainOffTime_CurPos);

    MenuManager_DrainOffTime_UnitVal(MenuManager_DrainOffTime_CurPos) -= (uint32_t)1U;

    if (MenuManager_DrainOffTime_UnitVal(MenuManager_DrainOffTime_CurPos) > (uint32_t)9U)
    {
      MenuManager_DrainOffTime_UnitVal(MenuManager_DrainOffTime_CurPos) = (uint32_t)9U;
    }

    MenuManager_Common_BcdToDecConv(&tempVal, &(MenuManager_DrainOffTime_UnitVal(0)), (uint8_t)MENUMANAGER_DRAINOFFTIME_UNITVAL_LENGTH);

    if ((tempVal <= MenuManager_DrainOffTime_ValueMin) || (tempVal >= MenuManager_DrainOffTime_ValueMax))
    {
      MenuManager_DrainOffTime_UnitVal(MenuManager_DrainOffTime_CurPos) = tempUnitVal;
    }

    MenuManager_DrainOffTime_LcdShowAdjust();
  }
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_DrainOffTime_AddBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_DrainOffTime_InternalState == MENUMANAGER_DRAINOFFTIME_INTERNALSTATE_READY)
  {
    if (MenuManager_DrainOffTime_CurPos > (uint32_t)0U)
    {
      MenuManager_DrainOffTime_CurPos -= (uint32_t)1U;
    }

    MenuManager_DrainOffTime_LcdShowAdjust();
  }
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_DrainOffTime_SubBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_DrainOffTime_InternalState == MENUMANAGER_DRAINOFFTIME_INTERNALSTATE_READY)
  {
    if (MenuManager_DrainOffTime_CurPos < (MENUMANAGER_DRAINOFFTIME_UNITVAL_LENGTH - (uint32_t)1U))
    {
      MenuManager_DrainOffTime_CurPos += (uint32_t)1U;
    }
    
    MenuManager_DrainOffTime_LcdShowAdjust();
  }
  
  return FSM_GUARD_TRUE;
}



/*=============================================================================================*/
static void MenuManager_DrainOffTime_SubMainFunction(void)
{
  MenuManager_Common_ButEventMapHandler(&MenuManager_DrainOffTime_ButEventMapConf);

  switch (MenuManager_DrainOffTime_InternalState)
  {
    case MENUMANAGER_DRAINOFFTIME_INTERNALSTATE_READY:
      break;
    case MENUMANAGER_DRAINOFFTIME_INTERNALSTATE_RUNNING:
    {
      uint16_t tempDrainOffTime;

      MenuManager_Common_BcdToDecConv(&(MenuManager_DrainOffTime_Value), &(MenuManager_DrainOffTime_UnitVal(0)), (uint8_t)MENUMANAGER_DRAINOFFTIME_UNITVAL_LENGTH);

      tempDrainOffTime = (uint16_t)MenuManager_DrainOffTime_Value;

      ProgramManager_DrainSetup_DrainOffTime_SetData(&tempDrainOffTime);

      ProgramManager_DrainSetup_DrainOffTime_GetData(&(ProgramManager_gParamConfig.drainCfg.drainOffTime));

      MenuManager_DrainOffTime_InternalState = MENUMANAGER_DRAINOFFTIME_INTERNALSTATE_DONE;
      
      break;
    }
    case MENUMANAGER_DRAINOFFTIME_INTERNALSTATE_DONE:
    {
      MenuManager_DrainOffTime_LcdShowDone();

      MenuManager_DrainOffTime_InternalState = MENUMANAGER_DRAINOFFTIME_INTERNALSTATE_EXITING;

      break;
    }
    case MENUMANAGER_DRAINOFFTIME_INTERNALSTATE_EXITING:
    default:
      break;
  }
}

/*=============================================================================================*/
static void MenuManager_DrainOffTime_SubTickHandler(void)
{
  if (MenuManager_DrainOffTime_InternalState == MENUMANAGER_DRAINOFFTIME_INTERNALSTATE_EXITING)
  {
    MenuManager_DrainOffTime_Counter += (uint32_t)1U;

    if (MenuManager_DrainOffTime_Counter >= MENUMANAGER_DRAINOFFTIME_COUNTER_MAX)
    {
      MenuManager_DrainOffTime_Counter = (uint32_t)0U;
      
      Fsm_TriggerEvent(&MenuManager_FsmContext, (Fsm_EventType)MENUMANAGER_EVENT_PREV);
    }
  }
}



#ifdef  __cplusplus
}
#endif
