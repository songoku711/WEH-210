/* 
 * File:   _1272000000_menu_BalanceDrainLevel.c
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
#define MENUMANAGER_BALANCEDRAINLEVEL_INTERNALDATALENGTH              (uint8_t)10U

#define MENUMANAGER_BALANCEDRAINLEVEL_INTERNALSTATE_IDX               0U
#define MENUMANAGER_BALANCEDRAINLEVEL_COUNTER_IDX                     1U
#define MENUMANAGER_BALANCEDRAINLEVEL_CURPOS_IDX                      2U
#define MENUMANAGER_BALANCEDRAINLEVEL_VALUE_MIN_IDX                   3U
#define MENUMANAGER_BALANCEDRAINLEVEL_VALUE_MAX_IDX                   4U
#define MENUMANAGER_BALANCEDRAINLEVEL_VALUE_IDX                       5U
#define MENUMANAGER_BALANCEDRAINLEVEL_UNITVAL_IDX                     6U

#define MenuManager_BalanceDrainLevel_InternalState                   MenuManager_GetInternalDataPtr(MENUMANAGER_BALANCEDRAINLEVEL_INTERNALSTATE_IDX)
#define MenuManager_BalanceDrainLevel_Counter                         MenuManager_GetInternalDataPtr(MENUMANAGER_BALANCEDRAINLEVEL_COUNTER_IDX)
#define MenuManager_BalanceDrainLevel_CurPos                          MenuManager_GetInternalDataPtr(MENUMANAGER_BALANCEDRAINLEVEL_CURPOS_IDX)
#define MenuManager_BalanceDrainLevel_ValueMin                        MenuManager_GetInternalDataPtr(MENUMANAGER_BALANCEDRAINLEVEL_VALUE_MIN_IDX)
#define MenuManager_BalanceDrainLevel_ValueMax                        MenuManager_GetInternalDataPtr(MENUMANAGER_BALANCEDRAINLEVEL_VALUE_MAX_IDX)
#define MenuManager_BalanceDrainLevel_Value                           MenuManager_GetInternalDataPtr(MENUMANAGER_BALANCEDRAINLEVEL_VALUE_IDX)
#define MenuManager_BalanceDrainLevel_UnitVal(idx)                    MenuManager_GetInternalDataPtr(MENUMANAGER_BALANCEDRAINLEVEL_UNITVAL_IDX + idx)



#define MENUMANAGER_BALANCEDRAINLEVEL_INTERNALSTATE_READY             (uint32_t)0U
#define MENUMANAGER_BALANCEDRAINLEVEL_INTERNALSTATE_RUNNING           (uint32_t)1U
#define MENUMANAGER_BALANCEDRAINLEVEL_INTERNALSTATE_DONE              (uint32_t)2U
#define MENUMANAGER_BALANCEDRAINLEVEL_INTERNALSTATE_EXITING           (uint32_t)3U

#define MENUMANAGER_BALANCEDRAINLEVEL_COUNTER_MAX                     (uint32_t)50U

#define MENUMANAGER_BALANCEDRAINLEVEL_CURPOS_XPOS                     (uint32_t)9U
#define MENUMANAGER_BALANCEDRAINLEVEL_CURPOS_YPOS                     (uint32_t)5U

#define MENUMANAGER_BALANCEDRAINLEVEL_UNITVAL_LENGTH                  (uint32_t)3U



/** Menu manager main titles and child menu titles */
static const uint8_t MenuManager_BalanceDrainLevel_MainTitleStr[] =   "BAL DRAIN LEVEL";



/** Menu manager button event mapping array */
static MenuManager_ButEventMapStruct MenuManager_BalanceDrainLevel_ButEventMap[6] =
{
  { IOMANAGER_BUTTON_STATE_START,                                     MENUMANAGER_EVENT_START_BUT             },
  { IOMANAGER_BUTTON_STATE_STOP,                                      MENUMANAGER_EVENT_STOP_BUT              },
  { IOMANAGER_BUTTON_STATE_UP,                                        MENUMANAGER_EVENT_UP_BUT                },
  { IOMANAGER_BUTTON_STATE_DOWN,                                      MENUMANAGER_EVENT_DOWN_BUT              },
  { IOMANAGER_BUTTON_STATE_ADD,                                       MENUMANAGER_EVENT_ADD_BUT               },
  { IOMANAGER_BUTTON_STATE_SUB,                                       MENUMANAGER_EVENT_SUB_BUT               }
};

/** Menu manager button event mapping configuration */
static MenuManager_ButEventMapConfStruct MenuManager_BalanceDrainLevel_ButEventMapConf =
{
  .butEventMapNum             = (uint8_t)6U,
  .butEventMapCfg             = &MenuManager_BalanceDrainLevel_ButEventMap
};



/** Menu manager event handlers */
static Fsm_GuardType MenuManager_BalanceDrainLevel_Entry              (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_BalanceDrainLevel_Exit               (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_BalanceDrainLevel_StartBut           (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_BalanceDrainLevel_StopBut            (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_BalanceDrainLevel_UpBut              (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_BalanceDrainLevel_DownBut            (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_BalanceDrainLevel_AddBut             (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_BalanceDrainLevel_SubBut             (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);

/** Menu manager state machine */
Fsm_EventEntryStruct MenuManager_BalanceDrainLevel_StateMachine[9] =
{
  FSM_TRIGGER_ENTRY             (                                     MenuManager_BalanceDrainLevel_Entry                                             ),
  FSM_TRIGGER_EXIT              (                                     MenuManager_BalanceDrainLevel_Exit                                              ),
  FSM_TRIGGER_TRANSITION        ( MENUMANAGER_EVENT_PREV,                                                     MENUMANAGER_STATE_EXTRACT_SETUP         ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_START_BUT,        MenuManager_BalanceDrainLevel_StartBut                                          ),
  FSM_TRIGGER_TRANSITION_ACTION ( MENUMANAGER_EVENT_STOP_BUT,         MenuManager_BalanceDrainLevel_StopBut,  MENUMANAGER_STATE_EXTRACT_SETUP         ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_UP_BUT,           MenuManager_BalanceDrainLevel_UpBut                                             ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_DOWN_BUT,         MenuManager_BalanceDrainLevel_DownBut                                           ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_ADD_BUT,          MenuManager_BalanceDrainLevel_AddBut                                            ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_SUB_BUT,          MenuManager_BalanceDrainLevel_SubBut                                            )
};



/*===============================================================================================
*                                   LOCAL FUNCTION PROTOTYPES
===============================================================================================*/

static void MenuManager_BalanceDrainLevel_LcdShowMainTitle(void);
static void MenuManager_BalanceDrainLevel_LcdShowAdjust(void);
static void MenuManager_BalanceDrainLevel_LcdShowDone(void);

static void MenuManager_BalanceDrainLevel_SubMainFunction(void);
static void MenuManager_BalanceDrainLevel_SubTickHandler(void);



/*===============================================================================================
*                                       LOCAL FUNCTIONS
===============================================================================================*/

static void MenuManager_BalanceDrainLevel_LcdShowMainTitle(void)
{
  /* Print main title */
  MenuManager_Common_LcdClearAdjustMenuStatic();
  MenuManager_Common_LcdShowMainTitle(MenuManager_BalanceDrainLevel_MainTitleStr);
  MenuManager_Common_LcdShowOkExitChoice();
  MenuManager_Common_LcdUpdateAdjustMenuStatic();
}

/*=============================================================================================*/
static void MenuManager_BalanceDrainLevel_LcdShowAdjust(void)
{
  MenuManager_Common_LcdClearAdjustMenuDynamic();

  MenuManager_Common_LcdShowAdjustUnitVal
  (
    MENUMANAGER_BALANCEDRAINLEVEL_CURPOS_XPOS,
    MENUMANAGER_BALANCEDRAINLEVEL_CURPOS_YPOS,
    &(MenuManager_BalanceDrainLevel_UnitVal(0)),
    MENUMANAGER_BALANCEDRAINLEVEL_UNITVAL_LENGTH
  );
  
  MenuManager_Common_LcdShowAdjustArrow
  (
    MENUMANAGER_BALANCEDRAINLEVEL_CURPOS_XPOS,
    MENUMANAGER_BALANCEDRAINLEVEL_CURPOS_YPOS,
    MenuManager_BalanceDrainLevel_CurPos
  );

  MenuManager_Common_LcdUpdateAdjustMenuDynamic();
}

/*=============================================================================================*/
static void MenuManager_BalanceDrainLevel_LcdShowDone(void)
{
  MenuManager_Common_LcdClearAll();

  MenuManager_Common_LcdShowAdjustUnitVal
  (
    MENUMANAGER_BALANCEDRAINLEVEL_CURPOS_XPOS,
    MENUMANAGER_BALANCEDRAINLEVEL_CURPOS_YPOS,
    &(MenuManager_BalanceDrainLevel_UnitVal(0)),
    MENUMANAGER_BALANCEDRAINLEVEL_UNITVAL_LENGTH
  );

  MenuManager_Common_LcdUpdateAll();
}



/*=============================================================================================*/
static Fsm_GuardType MenuManager_BalanceDrainLevel_Entry(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
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

      MenuManager_InternalDataPush(MENUMANAGER_BALANCEDRAINLEVEL_INTERNALDATALENGTH);

      MenuManager_BalanceDrainLevel_InternalState = MENUMANAGER_BALANCEDRAINLEVEL_INTERNALSTATE_READY;
      MenuManager_BalanceDrainLevel_Counter = (uint32_t)0U;
      MenuManager_BalanceDrainLevel_CurPos = (uint32_t)0U;

      MenuManager_BalanceDrainLevel_ValueMin = (uint32_t)PROGRAMMANAGER_EXTRACTSETUP_COMMON_LEVEL_MIN;
      MenuManager_BalanceDrainLevel_ValueMax = (uint32_t)PROGRAMMANAGER_EXTRACTSETUP_COMMON_LEVEL_MAX;

      MenuManager_BalanceDrainLevel_Value = (uint32_t)(ProgramManager_gParamConfig.extractCfg.balanceDrainLevel);

      MenuManager_Common_DecToBcdConv
      (
        &(MenuManager_BalanceDrainLevel_Value),
        &(MenuManager_BalanceDrainLevel_UnitVal(0)),
        (uint8_t)MENUMANAGER_BALANCEDRAINLEVEL_UNITVAL_LENGTH
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
    MenuManager_BalanceDrainLevel_LcdShowMainTitle();
    MenuManager_BalanceDrainLevel_LcdShowAdjust();

    MenuManager_SubMainFunction = MenuManager_BalanceDrainLevel_SubMainFunction;
    MenuManager_SubTickHandler = MenuManager_BalanceDrainLevel_SubTickHandler;

    return FSM_GUARD_TRUE;
  }

  return FSM_GUARD_FALSE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_BalanceDrainLevel_Exit(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  Fsm_DataHierachyStruct* dataHierachy;

  MenuManager_SubMainFunction = NULL;
  MenuManager_SubTickHandler = NULL;

  dataHierachy = (Fsm_DataHierachyStruct *)MenuManager_malloc(sizeof(Fsm_DataHierachyStruct));
  dataHierachy->dataId = MENUMANAGER_STATE_BALANCE_DRAIN_LEVEL;

  pFsmContext->dataHierachy = dataHierachy;

  /* Free internal data */
  MenuManager_InternalDataPop();
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_BalanceDrainLevel_StartBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_BalanceDrainLevel_InternalState == MENUMANAGER_BALANCEDRAINLEVEL_INTERNALSTATE_READY)
  {
    MenuManager_BalanceDrainLevel_InternalState = MENUMANAGER_BALANCEDRAINLEVEL_INTERNALSTATE_RUNNING;
  }
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_BalanceDrainLevel_StopBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_BalanceDrainLevel_InternalState != MENUMANAGER_BALANCEDRAINLEVEL_INTERNALSTATE_READY)
  {
    return FSM_GUARD_FALSE;
  }
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_BalanceDrainLevel_UpBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  uint32_t tempUnitVal;
  uint32_t tempVal;

  if (MenuManager_BalanceDrainLevel_InternalState == MENUMANAGER_BALANCEDRAINLEVEL_INTERNALSTATE_READY)
  {
    tempUnitVal = MenuManager_BalanceDrainLevel_UnitVal(MenuManager_BalanceDrainLevel_CurPos);

    MenuManager_BalanceDrainLevel_UnitVal(MenuManager_BalanceDrainLevel_CurPos) += (uint32_t)1U;

    if (MenuManager_BalanceDrainLevel_UnitVal(MenuManager_BalanceDrainLevel_CurPos) > (uint32_t)9U)
    {
      MenuManager_BalanceDrainLevel_UnitVal(MenuManager_BalanceDrainLevel_CurPos) = (uint32_t)0U;
    }

    MenuManager_Common_BcdToDecConv(&tempVal, &(MenuManager_BalanceDrainLevel_UnitVal(0)), (uint8_t)MENUMANAGER_BALANCEDRAINLEVEL_UNITVAL_LENGTH);

    if ((tempVal < MenuManager_BalanceDrainLevel_ValueMin) || (tempVal >= MenuManager_BalanceDrainLevel_ValueMax))
    {
      MenuManager_BalanceDrainLevel_UnitVal(MenuManager_BalanceDrainLevel_CurPos) = tempUnitVal;
    }

    MenuManager_BalanceDrainLevel_LcdShowAdjust();
  }

  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_BalanceDrainLevel_DownBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  uint32_t tempUnitVal;
  uint32_t tempVal;

  if (MenuManager_BalanceDrainLevel_InternalState == MENUMANAGER_BALANCEDRAINLEVEL_INTERNALSTATE_READY)
  {
    tempUnitVal = MenuManager_BalanceDrainLevel_UnitVal(MenuManager_BalanceDrainLevel_CurPos);

    MenuManager_BalanceDrainLevel_UnitVal(MenuManager_BalanceDrainLevel_CurPos) -= (uint32_t)1U;

    if (MenuManager_BalanceDrainLevel_UnitVal(MenuManager_BalanceDrainLevel_CurPos) > (uint32_t)9U)
    {
      MenuManager_BalanceDrainLevel_UnitVal(MenuManager_BalanceDrainLevel_CurPos) = (uint32_t)9U;
    }

    MenuManager_Common_BcdToDecConv(&tempVal, &(MenuManager_BalanceDrainLevel_UnitVal(0)), (uint8_t)MENUMANAGER_BALANCEDRAINLEVEL_UNITVAL_LENGTH);

    if ((tempVal < MenuManager_BalanceDrainLevel_ValueMin) || (tempVal >= MenuManager_BalanceDrainLevel_ValueMax))
    {
      MenuManager_BalanceDrainLevel_UnitVal(MenuManager_BalanceDrainLevel_CurPos) = tempUnitVal;
    }

    MenuManager_BalanceDrainLevel_LcdShowAdjust();
  }
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_BalanceDrainLevel_AddBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_BalanceDrainLevel_InternalState == MENUMANAGER_BALANCEDRAINLEVEL_INTERNALSTATE_READY)
  {
    if (MenuManager_BalanceDrainLevel_CurPos > (uint32_t)0U)
    {
      MenuManager_BalanceDrainLevel_CurPos -= (uint32_t)1U;
    }

    MenuManager_BalanceDrainLevel_LcdShowAdjust();
  }
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_BalanceDrainLevel_SubBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_BalanceDrainLevel_InternalState == MENUMANAGER_BALANCEDRAINLEVEL_INTERNALSTATE_READY)
  {
    if (MenuManager_BalanceDrainLevel_CurPos < (MENUMANAGER_BALANCEDRAINLEVEL_UNITVAL_LENGTH - (uint32_t)1U))
    {
      MenuManager_BalanceDrainLevel_CurPos += (uint32_t)1U;
    }
    
    MenuManager_BalanceDrainLevel_LcdShowAdjust();
  }
  
  return FSM_GUARD_TRUE;
}



/*=============================================================================================*/
static void MenuManager_BalanceDrainLevel_SubMainFunction(void)
{
  MenuManager_Common_ButEventMapHandler(&MenuManager_BalanceDrainLevel_ButEventMapConf);

  switch (MenuManager_BalanceDrainLevel_InternalState)
  {
    case MENUMANAGER_BALANCEDRAINLEVEL_INTERNALSTATE_READY:
      break;
    case MENUMANAGER_BALANCEDRAINLEVEL_INTERNALSTATE_RUNNING:
    {
      uint16_t tempBalanceDrainLevel;

      MenuManager_Common_BcdToDecConv(&(MenuManager_BalanceDrainLevel_Value), &(MenuManager_BalanceDrainLevel_UnitVal(0)), (uint8_t)MENUMANAGER_BALANCEDRAINLEVEL_UNITVAL_LENGTH);

      tempBalanceDrainLevel = (uint16_t)MenuManager_BalanceDrainLevel_Value;

      ProgramManager_ExtractSetup_BalanceDrainLevel_SetData(&tempBalanceDrainLevel);

      ProgramManager_ExtractSetup_BalanceDrainLevel_GetData(&(ProgramManager_gParamConfig.extractCfg.balanceDrainLevel));

      MenuManager_BalanceDrainLevel_InternalState = MENUMANAGER_BALANCEDRAINLEVEL_INTERNALSTATE_DONE;
      
      break;
    }
    case MENUMANAGER_BALANCEDRAINLEVEL_INTERNALSTATE_DONE:
    {
      MenuManager_BalanceDrainLevel_LcdShowDone();

      MenuManager_BalanceDrainLevel_InternalState = MENUMANAGER_BALANCEDRAINLEVEL_INTERNALSTATE_EXITING;

      break;
    }
    case MENUMANAGER_BALANCEDRAINLEVEL_INTERNALSTATE_EXITING:
    default:
      break;
  }
}

/*=============================================================================================*/
static void MenuManager_BalanceDrainLevel_SubTickHandler(void)
{
  if (MenuManager_BalanceDrainLevel_InternalState == MENUMANAGER_BALANCEDRAINLEVEL_INTERNALSTATE_EXITING)
  {
    MenuManager_BalanceDrainLevel_Counter += (uint32_t)1U;

    if (MenuManager_BalanceDrainLevel_Counter >= MENUMANAGER_BALANCEDRAINLEVEL_COUNTER_MAX)
    {
      MenuManager_BalanceDrainLevel_Counter = (uint32_t)0U;
      
      Fsm_TriggerEvent(&MenuManager_FsmContext, (Fsm_EventType)MENUMANAGER_EVENT_PREV);
    }
  }
}



#ifdef  __cplusplus
}
#endif
