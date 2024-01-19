/* 
 * File:   _1271100000_menu_DrainSetupTime.c
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
#define MENUMANAGER_DRAINSETUPTIME_INTERNALDATALENGTH                 (uint8_t)11U

#define MENUMANAGER_DRAINSETUPTIME_DRAINSTEPIDX_IDX                   0U
#define MENUMANAGER_DRAINSETUPTIME_INTERNALSTATE_IDX                  1U
#define MENUMANAGER_DRAINSETUPTIME_COUNTER_IDX                        2U
#define MENUMANAGER_DRAINSETUPTIME_CURPOS_IDX                         3U
#define MENUMANAGER_DRAINSETUPTIME_VALUE_MIN_IDX                      4U
#define MENUMANAGER_DRAINSETUPTIME_VALUE_MAX_IDX                      5U
#define MENUMANAGER_DRAINSETUPTIME_VALUE_IDX                          6U
#define MENUMANAGER_DRAINSETUPTIME_UNITVAL_IDX                        7U

#define MenuManager_DrainSetupTime_DrainStepIdx                       MenuManager_GetInternalDataPtr(MENUMANAGER_DRAINSETUPTIME_DRAINSTEPIDX_IDX)
#define MenuManager_DrainSetupTime_InternalState                      MenuManager_GetInternalDataPtr(MENUMANAGER_DRAINSETUPTIME_INTERNALSTATE_IDX)
#define MenuManager_DrainSetupTime_Counter                            MenuManager_GetInternalDataPtr(MENUMANAGER_DRAINSETUPTIME_COUNTER_IDX)
#define MenuManager_DrainSetupTime_CurPos                             MenuManager_GetInternalDataPtr(MENUMANAGER_DRAINSETUPTIME_CURPOS_IDX)
#define MenuManager_DrainSetupTime_ValueMin                           MenuManager_GetInternalDataPtr(MENUMANAGER_DRAINSETUPTIME_VALUE_MIN_IDX)
#define MenuManager_DrainSetupTime_ValueMax                           MenuManager_GetInternalDataPtr(MENUMANAGER_DRAINSETUPTIME_VALUE_MAX_IDX)
#define MenuManager_DrainSetupTime_Value                              MenuManager_GetInternalDataPtr(MENUMANAGER_DRAINSETUPTIME_VALUE_IDX)
#define MenuManager_DrainSetupTime_UnitVal(idx)                       MenuManager_GetInternalDataPtr(MENUMANAGER_DRAINSETUPTIME_UNITVAL_IDX + idx)



#define MENUMANAGER_DRAINSETUPTIME_INTERNALSTATE_READY                (uint32_t)0U
#define MENUMANAGER_DRAINSETUPTIME_INTERNALSTATE_RUNNING              (uint32_t)1U
#define MENUMANAGER_DRAINSETUPTIME_INTERNALSTATE_DONE                 (uint32_t)2U
#define MENUMANAGER_DRAINSETUPTIME_INTERNALSTATE_EXITING              (uint32_t)3U

#define MENUMANAGER_DRAINSETUPTIME_COUNTER_MAX                        (uint32_t)50U

#define MENUMANAGER_DRAINSETUPTIME_CURPOS_XPOS                        (uint32_t)5U
#define MENUMANAGER_DRAINSETUPTIME_CURPOS_YPOS                        (uint32_t)5U

#define MENUMANAGER_DRAINSETUPTIME_UNIT_XPOS                          (uint32_t)10U
#define MENUMANAGER_DRAINSETUPTIME_UNIT_YPOS                          (uint32_t)5U

#define MENUMANAGER_DRAINSETUPTIME_UNITVAL_LENGTH                     (uint32_t)4U



/** Menu manager main titles and child menu titles */
static const uint8_t MenuManager_DrainSetupTime_MainTitleStr[] =      "DRAIN TIME";



/** Menu manager button event mapping array */
static MenuManager_ButEventMapStruct MenuManager_DrainSetupTime_ButEventMap[6] =
{
  { IOMANAGER_BUTTON_STATE_START,                                     MENUMANAGER_EVENT_START_BUT             },
  { IOMANAGER_BUTTON_STATE_STOP,                                      MENUMANAGER_EVENT_STOP_BUT              },
  { IOMANAGER_BUTTON_STATE_UP,                                        MENUMANAGER_EVENT_UP_BUT                },
  { IOMANAGER_BUTTON_STATE_DOWN,                                      MENUMANAGER_EVENT_DOWN_BUT              },
  { IOMANAGER_BUTTON_STATE_ADD,                                       MENUMANAGER_EVENT_ADD_BUT               },
  { IOMANAGER_BUTTON_STATE_SUB,                                       MENUMANAGER_EVENT_SUB_BUT               }
};

/** Menu manager button event mapping configuration */
static MenuManager_ButEventMapConfStruct MenuManager_DrainSetupTime_ButEventMapConf =
{
  .butEventMapNum             = (uint8_t)6U,
  .butEventMapCfg             = &MenuManager_DrainSetupTime_ButEventMap
};



/** Menu manager event handlers */
static Fsm_GuardType MenuManager_DrainSetupTime_Entry                 (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_DrainSetupTime_Exit                  (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_DrainSetupTime_StartBut              (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_DrainSetupTime_StopBut               (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_DrainSetupTime_UpBut                 (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_DrainSetupTime_DownBut               (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_DrainSetupTime_AddBut                (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_DrainSetupTime_SubBut                (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);

/** Menu manager state machine */
Fsm_EventEntryStruct MenuManager_DrainSetupTime_StateMachine[9] =
{
  FSM_TRIGGER_ENTRY             (                                     MenuManager_DrainSetupTime_Entry                                                ),
  FSM_TRIGGER_EXIT              (                                     MenuManager_DrainSetupTime_Exit                                                 ),
  FSM_TRIGGER_TRANSITION        ( MENUMANAGER_EVENT_PREV,                                                     MENUMANAGER_STATE_DRAIN_SETUP           ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_START_BUT,        MenuManager_DrainSetupTime_StartBut                                             ),
  FSM_TRIGGER_TRANSITION_ACTION ( MENUMANAGER_EVENT_STOP_BUT,         MenuManager_DrainSetupTime_StopBut,     MENUMANAGER_STATE_DRAIN_SETUP           ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_UP_BUT,           MenuManager_DrainSetupTime_UpBut                                                ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_DOWN_BUT,         MenuManager_DrainSetupTime_DownBut                                              ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_ADD_BUT,          MenuManager_DrainSetupTime_AddBut                                               ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_SUB_BUT,          MenuManager_DrainSetupTime_SubBut                                               )
};



/*===============================================================================================
*                                   LOCAL FUNCTION PROTOTYPES
===============================================================================================*/

static void MenuManager_DrainSetupTime_LcdShowMainTitle(void);
static void MenuManager_DrainSetupTime_LcdShowAdjust(void);
static void MenuManager_DrainSetupTime_LcdShowDone(void);

static void MenuManager_DrainSetupTime_SubMainFunction(void);
static void MenuManager_DrainSetupTime_SubTickHandler(void);



/*===============================================================================================
*                                       LOCAL FUNCTIONS
===============================================================================================*/

static void MenuManager_DrainSetupTime_LcdShowMainTitle(void)
{
  /* Print main title */
  MenuManager_Common_LcdClearAdjustMenuStatic();
  MenuManager_Common_LcdShowMainTitle(MenuManager_DrainSetupTime_MainTitleStr);
  MenuManager_Common_LcdShowOkExitChoice();
  MenuManager_Common_LcdUpdateAdjustMenuStatic();
}

/*=============================================================================================*/
static void MenuManager_DrainSetupTime_LcdShowAdjust(void)
{
  MenuManager_Common_LcdClearAdjustMenuDynamic();

  MenuManager_Common_LcdShowAdjustUnitVal
  (
    MENUMANAGER_DRAINSETUPTIME_CURPOS_XPOS,
    MENUMANAGER_DRAINSETUPTIME_CURPOS_YPOS,
    &(MenuManager_DrainSetupTime_UnitVal(0)),
    MENUMANAGER_DRAINSETUPTIME_UNITVAL_LENGTH
  );
  
  MenuManager_Common_LcdShowAdjustArrow
  (
    MENUMANAGER_DRAINSETUPTIME_CURPOS_XPOS,
    MENUMANAGER_DRAINSETUPTIME_CURPOS_YPOS,
    MenuManager_DrainSetupTime_CurPos
  );

  LCD_SetCursorPos(MENUMANAGER_DRAINSETUPTIME_UNIT_XPOS, MENUMANAGER_DRAINSETUPTIME_UNIT_YPOS, LCD_CURSOR_BY_FONT);
  LCD_PutString((uint8_t *)MenuManager_Common_SecondStr);

  MenuManager_Common_LcdUpdateAdjustMenuDynamic();
}

/*=============================================================================================*/
static void MenuManager_DrainSetupTime_LcdShowDone(void)
{
  MenuManager_Common_LcdClearAll();

  MenuManager_Common_LcdShowAdjustUnitVal
  (
    MENUMANAGER_DRAINSETUPTIME_CURPOS_XPOS,
    MENUMANAGER_DRAINSETUPTIME_CURPOS_YPOS,
    &(MenuManager_DrainSetupTime_UnitVal(0)),
    MENUMANAGER_DRAINSETUPTIME_UNITVAL_LENGTH
  );

  LCD_SetCursorPos(MENUMANAGER_DRAINSETUPTIME_UNIT_XPOS, MENUMANAGER_DRAINSETUPTIME_UNIT_YPOS, LCD_CURSOR_BY_FONT);
  LCD_PutString((uint8_t *)MenuManager_Common_SecondStr);

  MenuManager_Common_LcdUpdateAll();
}



/*=============================================================================================*/
static Fsm_GuardType MenuManager_DrainSetupTime_Entry(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  MenuManager_Common_DrainSetupStruct* enterDataHierachy;
  HAL_StatusTypeDef retVal = HAL_OK;

  /* Check if previous state data hierachy is not empty */
  if (pFsmContext->dataHierachy != NULL)
  {
    if (pFsmContext->dataHierachy->dataId == MENUMANAGER_STATE_DRAIN_SETUP)
    {
      enterDataHierachy = (MenuManager_Common_DrainSetupStruct *)(pFsmContext->dataHierachy);

      MenuManager_InternalDataPush(MENUMANAGER_DRAINSETUPTIME_INTERNALDATALENGTH);

      MenuManager_DrainSetupTime_DrainStepIdx = enterDataHierachy->drainStepIdx;

      MenuManager_DrainSetupTime_InternalState = MENUMANAGER_DRAINSETUPTIME_INTERNALSTATE_READY;
      MenuManager_DrainSetupTime_Counter = (uint32_t)0U;
      MenuManager_DrainSetupTime_CurPos = (uint32_t)0U;

      MenuManager_DrainSetupTime_ValueMin = (uint32_t)(ProgramManager_DrainStep_DrainTimeMin[MenuManager_DrainSetupTime_DrainStepIdx]);
      MenuManager_DrainSetupTime_ValueMax = (uint32_t)(ProgramManager_DrainStep_DrainTimeMax[MenuManager_DrainSetupTime_DrainStepIdx]);
      MenuManager_DrainSetupTime_Value = (uint32_t)(ProgramManager_gParamConfig.drainCfg.drainTime[MenuManager_DrainSetupTime_DrainStepIdx]);

      MenuManager_Common_DecToBcdConv
      (
        &(MenuManager_DrainSetupTime_Value),
        &(MenuManager_DrainSetupTime_UnitVal(0)),
        (uint8_t)MENUMANAGER_DRAINSETUPTIME_UNITVAL_LENGTH
      );

      /* Release previous state data hierachy */
      MenuManager_free(pFsmContext->dataHierachy);
      pFsmContext->dataHierachy = NULL;
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
    MenuManager_DrainSetupTime_LcdShowMainTitle();
    MenuManager_DrainSetupTime_LcdShowAdjust();

    MenuManager_SubMainFunction = MenuManager_DrainSetupTime_SubMainFunction;
    MenuManager_SubTickHandler = MenuManager_DrainSetupTime_SubTickHandler;

    return FSM_GUARD_TRUE;
  }

  return FSM_GUARD_FALSE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_DrainSetupTime_Exit(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  Fsm_DataHierachyStruct* dataHierachy;

  MenuManager_SubMainFunction = NULL;
  MenuManager_SubTickHandler = NULL;

  dataHierachy = (Fsm_DataHierachyStruct *)MenuManager_malloc(sizeof(Fsm_DataHierachyStruct));
  dataHierachy->dataId = MENUMANAGER_STATE_DRAIN_SETUP_TIME;

  pFsmContext->dataHierachy = dataHierachy;

  /* Free internal data */
  MenuManager_InternalDataPop();
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_DrainSetupTime_StartBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_DrainSetupTime_InternalState == MENUMANAGER_DRAINSETUPTIME_INTERNALSTATE_READY)
  {
    MenuManager_DrainSetupTime_InternalState = MENUMANAGER_DRAINSETUPTIME_INTERNALSTATE_RUNNING;
  }
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_DrainSetupTime_StopBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_DrainSetupTime_InternalState != MENUMANAGER_DRAINSETUPTIME_INTERNALSTATE_READY)
  {
    return FSM_GUARD_FALSE;
  }
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_DrainSetupTime_UpBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  uint32_t tempUnitVal;
  uint32_t tempVal;

  if (MenuManager_DrainSetupTime_InternalState == MENUMANAGER_DRAINSETUPTIME_INTERNALSTATE_READY)
  {
    tempUnitVal = MenuManager_DrainSetupTime_UnitVal(MenuManager_DrainSetupTime_CurPos);

    MenuManager_DrainSetupTime_UnitVal(MenuManager_DrainSetupTime_CurPos) += (uint32_t)1U;

    if (MenuManager_DrainSetupTime_UnitVal(MenuManager_DrainSetupTime_CurPos) > (uint32_t)9U)
    {
      MenuManager_DrainSetupTime_UnitVal(MenuManager_DrainSetupTime_CurPos) = (uint32_t)0U;
    }

    MenuManager_Common_BcdToDecConv(&tempVal, &(MenuManager_DrainSetupTime_UnitVal(0)), (uint8_t)MENUMANAGER_DRAINSETUPTIME_UNITVAL_LENGTH);

    if ((tempVal < MenuManager_DrainSetupTime_ValueMin) || (tempVal > MenuManager_DrainSetupTime_ValueMax))
    {
      MenuManager_DrainSetupTime_UnitVal(MenuManager_DrainSetupTime_CurPos) = tempUnitVal;
    }

    MenuManager_DrainSetupTime_LcdShowAdjust();
  }

  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_DrainSetupTime_DownBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  uint32_t tempUnitVal;
  uint32_t tempVal;

  if (MenuManager_DrainSetupTime_InternalState == MENUMANAGER_DRAINSETUPTIME_INTERNALSTATE_READY)
  {
    tempUnitVal = MenuManager_DrainSetupTime_UnitVal(MenuManager_DrainSetupTime_CurPos);

    MenuManager_DrainSetupTime_UnitVal(MenuManager_DrainSetupTime_CurPos) -= (uint32_t)1U;

    if (MenuManager_DrainSetupTime_UnitVal(MenuManager_DrainSetupTime_CurPos) > (uint32_t)9U)
    {
      MenuManager_DrainSetupTime_UnitVal(MenuManager_DrainSetupTime_CurPos) = (uint32_t)9U;
    }

    MenuManager_Common_BcdToDecConv(&tempVal, &(MenuManager_DrainSetupTime_UnitVal(0)), (uint8_t)MENUMANAGER_DRAINSETUPTIME_UNITVAL_LENGTH);

    if ((tempVal < MenuManager_DrainSetupTime_ValueMin) || (tempVal > MenuManager_DrainSetupTime_ValueMax))
    {
      MenuManager_DrainSetupTime_UnitVal(MenuManager_DrainSetupTime_CurPos) = tempUnitVal;
    }

    MenuManager_DrainSetupTime_LcdShowAdjust();
  }
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_DrainSetupTime_AddBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_DrainSetupTime_InternalState == MENUMANAGER_DRAINSETUPTIME_INTERNALSTATE_READY)
  {
    if (MenuManager_DrainSetupTime_CurPos > (uint32_t)0U)
    {
      MenuManager_DrainSetupTime_CurPos -= (uint32_t)1U;
    }

    MenuManager_DrainSetupTime_LcdShowAdjust();
  }
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_DrainSetupTime_SubBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_DrainSetupTime_InternalState == MENUMANAGER_DRAINSETUPTIME_INTERNALSTATE_READY)
  {
    if (MenuManager_DrainSetupTime_CurPos < (MENUMANAGER_DRAINSETUPTIME_UNITVAL_LENGTH - (uint32_t)1U))
    {
      MenuManager_DrainSetupTime_CurPos += (uint32_t)1U;
    }
    
    MenuManager_DrainSetupTime_LcdShowAdjust();
  }
  
  return FSM_GUARD_TRUE;
}



/*=============================================================================================*/
static void MenuManager_DrainSetupTime_SubMainFunction(void)
{
  MenuManager_Common_ButEventMapHandler(&MenuManager_DrainSetupTime_ButEventMapConf);

  switch (MenuManager_DrainSetupTime_InternalState)
  {
    case MENUMANAGER_DRAINSETUPTIME_INTERNALSTATE_READY:
      break;
    case MENUMANAGER_DRAINSETUPTIME_INTERNALSTATE_RUNNING:
    {
      uint16_t tempDrainSetupTime;

      MenuManager_Common_BcdToDecConv(&(MenuManager_DrainSetupTime_Value), &(MenuManager_DrainSetupTime_UnitVal(0)), (uint8_t)MENUMANAGER_DRAINSETUPTIME_UNITVAL_LENGTH);

      tempDrainSetupTime = (uint16_t)MenuManager_DrainSetupTime_Value;

      ProgramManager_DrainSetup_DrainTime_SetData((uint8_t)MenuManager_DrainSetupTime_DrainStepIdx, &tempDrainSetupTime);

      ProgramManager_DrainSetup_DrainTime_GetData((uint8_t)MenuManager_DrainSetupTime_DrainStepIdx, &(ProgramManager_gParamConfig.drainCfg.drainTime[MenuManager_DrainSetupTime_DrainStepIdx]));

      MenuManager_DrainSetupTime_InternalState = MENUMANAGER_DRAINSETUPTIME_INTERNALSTATE_DONE;
      
      break;
    }
    case MENUMANAGER_DRAINSETUPTIME_INTERNALSTATE_DONE:
    {
      MenuManager_DrainSetupTime_LcdShowDone();

      MenuManager_DrainSetupTime_InternalState = MENUMANAGER_DRAINSETUPTIME_INTERNALSTATE_EXITING;

      break;
    }
    case MENUMANAGER_DRAINSETUPTIME_INTERNALSTATE_EXITING:
    default:
      break;
  }
}

/*=============================================================================================*/
static void MenuManager_DrainSetupTime_SubTickHandler(void)
{
  if (MenuManager_DrainSetupTime_InternalState == MENUMANAGER_DRAINSETUPTIME_INTERNALSTATE_EXITING)
  {
    MenuManager_DrainSetupTime_Counter += (uint32_t)1U;

    if (MenuManager_DrainSetupTime_Counter >= MENUMANAGER_DRAINSETUPTIME_COUNTER_MAX)
    {
      MenuManager_DrainSetupTime_Counter = (uint32_t)0U;
      
      Fsm_TriggerEvent(&MenuManager_FsmContext, (Fsm_EventType)MENUMANAGER_EVENT_PREV);
    }
  }
}



#ifdef  __cplusplus
}
#endif
