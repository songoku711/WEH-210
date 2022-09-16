/* 
 * File:   _1234000000_menu_MidLevel.c
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
#define MENUMANAGER_MIDLEVEL_INTERNALDATALENGTH                       (uint8_t)10U

#define MENUMANAGER_MIDLEVEL_INTERNALSTATE_IDX                        0U
#define MENUMANAGER_MIDLEVEL_COUNTER_IDX                              1U
#define MENUMANAGER_MIDLEVEL_CURPOS_IDX                               2U
#define MENUMANAGER_MIDLEVEL_VALUE_MIN_IDX                            3U
#define MENUMANAGER_MIDLEVEL_VALUE_MAX_IDX                            4U
#define MENUMANAGER_MIDLEVEL_VALUE_IDX                                5U
#define MENUMANAGER_MIDLEVEL_UNITVAL_IDX                              6U

#define MenuManager_MidLevel_InternalState                            MenuManager_GetInternalDataPtr(MENUMANAGER_MIDLEVEL_INTERNALSTATE_IDX)
#define MenuManager_MidLevel_Counter                                  MenuManager_GetInternalDataPtr(MENUMANAGER_MIDLEVEL_COUNTER_IDX)
#define MenuManager_MidLevel_CurPos                                   MenuManager_GetInternalDataPtr(MENUMANAGER_MIDLEVEL_CURPOS_IDX)
#define MenuManager_MidLevel_ValueMin                                 MenuManager_GetInternalDataPtr(MENUMANAGER_MIDLEVEL_VALUE_MIN_IDX)
#define MenuManager_MidLevel_ValueMax                                 MenuManager_GetInternalDataPtr(MENUMANAGER_MIDLEVEL_VALUE_MAX_IDX)
#define MenuManager_MidLevel_Value                                    MenuManager_GetInternalDataPtr(MENUMANAGER_MIDLEVEL_VALUE_IDX)
#define MenuManager_MidLevel_UnitVal(idx)                             MenuManager_GetInternalDataPtr(MENUMANAGER_MIDLEVEL_UNITVAL_IDX + idx)



#define MENUMANAGER_MIDLEVEL_INTERNALSTATE_READY                      (uint32_t)0U
#define MENUMANAGER_MIDLEVEL_INTERNALSTATE_RUNNING                    (uint32_t)1U
#define MENUMANAGER_MIDLEVEL_INTERNALSTATE_DONE                       (uint32_t)2U
#define MENUMANAGER_MIDLEVEL_INTERNALSTATE_EXITING                    (uint32_t)3U

#define MENUMANAGER_MIDLEVEL_COUNTER_MAX                              (uint32_t)50U

#define MENUMANAGER_MIDLEVEL_CURPOS_XPOS                              (uint32_t)8U
#define MENUMANAGER_MIDLEVEL_CURPOS_YPOS                              (uint32_t)5U

#define MENUMANAGER_MIDLEVEL_UNITVAL_LENGTH                           (uint32_t)4U



/** Menu manager main titles and child menu titles */
static const uint8_t MenuManager_MidLevel_MainTitleStr[] =            "MIDDLE LEVEL";



/** Menu manager button event mapping array */
static MenuManager_ButEventMapStruct MenuManager_MidLevel_ButEventMap[6] =
{
  { IOMANAGER_BUTTON_STATE_START,                                     MENUMANAGER_EVENT_START_BUT             },
  { IOMANAGER_BUTTON_STATE_STOP,                                      MENUMANAGER_EVENT_STOP_BUT              },
  { IOMANAGER_BUTTON_STATE_UP,                                        MENUMANAGER_EVENT_UP_BUT                },
  { IOMANAGER_BUTTON_STATE_DOWN,                                      MENUMANAGER_EVENT_DOWN_BUT              },
  { IOMANAGER_BUTTON_STATE_ADD,                                       MENUMANAGER_EVENT_ADD_BUT               },
  { IOMANAGER_BUTTON_STATE_SUB,                                       MENUMANAGER_EVENT_SUB_BUT               }
};

/** Menu manager button event mapping configuration */
static MenuManager_ButEventMapConfStruct MenuManager_MidLevel_ButEventMapConf =
{
  .butEventMapNum             = (uint8_t)6U,
  .butEventMapCfg             = &MenuManager_MidLevel_ButEventMap
};



/** Menu manager event handlers */
static Fsm_GuardType MenuManager_MidLevel_Entry                       (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_MidLevel_Exit                        (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_MidLevel_StartBut                    (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_MidLevel_StopBut                     (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_MidLevel_UpBut                       (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_MidLevel_DownBut                     (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_MidLevel_AddBut                      (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_MidLevel_SubBut                      (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);

/** Menu manager state machine */
Fsm_EventEntryStruct MenuManager_MidLevel_StateMachine[9] =
{
  FSM_TRIGGER_ENTRY             (                                     MenuManager_MidLevel_Entry                                                      ),
  FSM_TRIGGER_EXIT              (                                     MenuManager_MidLevel_Exit                                                       ),
  FSM_TRIGGER_TRANSITION        ( MENUMANAGER_EVENT_PREV,                                                     MENUMANAGER_STATE_FILL_LEVEL_SETUP      ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_START_BUT,        MenuManager_MidLevel_StartBut                                                   ),
  FSM_TRIGGER_TRANSITION_ACTION ( MENUMANAGER_EVENT_STOP_BUT,         MenuManager_MidLevel_StopBut,           MENUMANAGER_STATE_FILL_LEVEL_SETUP      ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_UP_BUT,           MenuManager_MidLevel_UpBut                                                      ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_DOWN_BUT,         MenuManager_MidLevel_DownBut                                                    ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_ADD_BUT,          MenuManager_MidLevel_AddBut                                                     ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_SUB_BUT,          MenuManager_MidLevel_SubBut                                                     )
};



/*===============================================================================================
*                                   LOCAL FUNCTION PROTOTYPES
===============================================================================================*/

static void MenuManager_MidLevel_LcdShowMainTitle(void);
static void MenuManager_MidLevel_LcdShowAdjust(void);
static void MenuManager_MidLevel_LcdShowDone(void);

static void MenuManager_MidLevel_SubMainFunction(void);
static void MenuManager_MidLevel_SubTickHandler(void);



/*===============================================================================================
*                                       LOCAL FUNCTIONS
===============================================================================================*/

static void MenuManager_MidLevel_LcdShowMainTitle(void)
{
  /* Print main title */
  MenuManager_Common_LcdClearAdjustMenuStatic();
  MenuManager_Common_LcdShowMainTitle(MenuManager_MidLevel_MainTitleStr);
  MenuManager_Common_LcdShowOkExitChoice();
  MenuManager_Common_LcdUpdateAdjustMenuStatic();
}

/*=============================================================================================*/
static void MenuManager_MidLevel_LcdShowAdjust(void)
{
  MenuManager_Common_LcdClearAdjustMenuDynamic();

  MenuManager_Common_LcdShowAdjustUnitVal
  (
    MENUMANAGER_MIDLEVEL_CURPOS_XPOS,
    MENUMANAGER_MIDLEVEL_CURPOS_YPOS,
    &(MenuManager_MidLevel_UnitVal(0)),
    MENUMANAGER_MIDLEVEL_UNITVAL_LENGTH
  );
  
  MenuManager_Common_LcdShowAdjustArrow
  (
    MENUMANAGER_MIDLEVEL_CURPOS_XPOS,
    MENUMANAGER_MIDLEVEL_CURPOS_YPOS,
    MenuManager_MidLevel_CurPos
  );

  MenuManager_Common_LcdUpdateAdjustMenuDynamic();
}

/*=============================================================================================*/
static void MenuManager_MidLevel_LcdShowDone(void)
{
  MenuManager_Common_LcdClearAll();

  MenuManager_Common_LcdShowAdjustUnitVal
  (
    MENUMANAGER_MIDLEVEL_CURPOS_XPOS,
    MENUMANAGER_MIDLEVEL_CURPOS_YPOS,
    &(MenuManager_MidLevel_UnitVal(0)),
    MENUMANAGER_MIDLEVEL_UNITVAL_LENGTH
  );

  MenuManager_Common_LcdUpdateAll();
}



/*=============================================================================================*/
static Fsm_GuardType MenuManager_MidLevel_Entry(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  HAL_StatusTypeDef retVal = HAL_OK;

  /* Check if previous state data hierachy is not empty */
  if (pFsmContext->dataHierachy != NULL)
  {
    if (pFsmContext->dataHierachy->dataId == MENUMANAGER_STATE_FILL_LEVEL_SETUP)
    {
      /* Release previous state data hierachy */
      MenuManager_free(pFsmContext->dataHierachy);
      pFsmContext->dataHierachy = NULL;

      MenuManager_InternalDataPush(MENUMANAGER_MIDLEVEL_INTERNALDATALENGTH);

      MenuManager_MidLevel_InternalState = MENUMANAGER_MIDLEVEL_INTERNALSTATE_READY;
      MenuManager_MidLevel_Counter = (uint32_t)0U;
      MenuManager_MidLevel_CurPos = (uint32_t)0U;

      MenuManager_MidLevel_ValueMin = (uint32_t)(ProgramManager_gParamConfig.fillLevelCfg.lowLevel);
      MenuManager_MidLevel_ValueMax = (uint32_t)(ProgramManager_gParamConfig.fillLevelCfg.highLevel);

      MenuManager_MidLevel_Value = (uint32_t)(ProgramManager_gParamConfig.fillLevelCfg.midLevel);

      MenuManager_Common_DecToBcdConv
      (
        &(MenuManager_MidLevel_Value),
        &(MenuManager_MidLevel_UnitVal(0)),
        (uint8_t)MENUMANAGER_MIDLEVEL_UNITVAL_LENGTH
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
    MenuManager_MidLevel_LcdShowMainTitle();
    MenuManager_MidLevel_LcdShowAdjust();

    MenuManager_SubMainFunction = MenuManager_MidLevel_SubMainFunction;
    MenuManager_SubTickHandler = MenuManager_MidLevel_SubTickHandler;

    return FSM_GUARD_TRUE;
  }

  return FSM_GUARD_FALSE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_MidLevel_Exit(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  Fsm_DataHierachyStruct* dataHierachy;

  MenuManager_SubMainFunction = NULL;
  MenuManager_SubTickHandler = NULL;

  dataHierachy = (Fsm_DataHierachyStruct *)MenuManager_malloc(sizeof(Fsm_DataHierachyStruct));
  dataHierachy->dataId = MENUMANAGER_STATE_MID_LEVEL;

  pFsmContext->dataHierachy = dataHierachy;

  /* Free internal data */
  MenuManager_InternalDataPop();
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_MidLevel_StartBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_MidLevel_InternalState == MENUMANAGER_MIDLEVEL_INTERNALSTATE_READY)
  {
    MenuManager_MidLevel_InternalState = MENUMANAGER_MIDLEVEL_INTERNALSTATE_RUNNING;
  }
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_MidLevel_StopBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_MidLevel_InternalState != MENUMANAGER_MIDLEVEL_INTERNALSTATE_READY)
  {
    return FSM_GUARD_FALSE;
  }
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_MidLevel_UpBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  uint32_t tempUnitVal;
  uint32_t tempVal;

  if (MenuManager_MidLevel_InternalState == MENUMANAGER_MIDLEVEL_INTERNALSTATE_READY)
  {
    tempUnitVal = MenuManager_MidLevel_UnitVal(MenuManager_MidLevel_CurPos);

    MenuManager_MidLevel_UnitVal(MenuManager_MidLevel_CurPos) += (uint32_t)1U;

    if (MenuManager_MidLevel_UnitVal(MenuManager_MidLevel_CurPos) > (uint32_t)9U)
    {
      MenuManager_MidLevel_UnitVal(MenuManager_MidLevel_CurPos) = (uint32_t)0U;
    }

    MenuManager_Common_BcdToDecConv(&tempVal, &(MenuManager_MidLevel_UnitVal(0)), (uint8_t)MENUMANAGER_MIDLEVEL_UNITVAL_LENGTH);

    if ((tempVal <= MenuManager_MidLevel_ValueMin) || (tempVal >= MenuManager_MidLevel_ValueMax))
    {
      MenuManager_MidLevel_UnitVal(MenuManager_MidLevel_CurPos) = tempUnitVal;
    }

    MenuManager_MidLevel_LcdShowAdjust();
  }

  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_MidLevel_DownBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  uint32_t tempUnitVal;
  uint32_t tempVal;

  if (MenuManager_MidLevel_InternalState == MENUMANAGER_MIDLEVEL_INTERNALSTATE_READY)
  {
    tempUnitVal = MenuManager_MidLevel_UnitVal(MenuManager_MidLevel_CurPos);

    MenuManager_MidLevel_UnitVal(MenuManager_MidLevel_CurPos) -= (uint32_t)1U;

    if (MenuManager_MidLevel_UnitVal(MenuManager_MidLevel_CurPos) > (uint32_t)9U)
    {
      MenuManager_MidLevel_UnitVal(MenuManager_MidLevel_CurPos) = (uint32_t)9U;
    }

    MenuManager_Common_BcdToDecConv(&tempVal, &(MenuManager_MidLevel_UnitVal(0)), (uint8_t)MENUMANAGER_MIDLEVEL_UNITVAL_LENGTH);

    if ((tempVal <= MenuManager_MidLevel_ValueMin) || (tempVal >= MenuManager_MidLevel_ValueMax))
    {
      MenuManager_MidLevel_UnitVal(MenuManager_MidLevel_CurPos) = tempUnitVal;
    }

    MenuManager_MidLevel_LcdShowAdjust();
  }
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_MidLevel_AddBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_MidLevel_InternalState == MENUMANAGER_MIDLEVEL_INTERNALSTATE_READY)
  {
    if (MenuManager_MidLevel_CurPos > (uint32_t)0U)
    {
      MenuManager_MidLevel_CurPos -= (uint32_t)1U;
    }

    MenuManager_MidLevel_LcdShowAdjust();
  }
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_MidLevel_SubBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_MidLevel_InternalState == MENUMANAGER_MIDLEVEL_INTERNALSTATE_READY)
  {
    if (MenuManager_MidLevel_CurPos < (MENUMANAGER_MIDLEVEL_UNITVAL_LENGTH - (uint32_t)1U))
    {
      MenuManager_MidLevel_CurPos += (uint32_t)1U;
    }
    
    MenuManager_MidLevel_LcdShowAdjust();
  }
  
  return FSM_GUARD_TRUE;
}



/*=============================================================================================*/
static void MenuManager_MidLevel_SubMainFunction(void)
{
  MenuManager_Common_ButEventMapHandler(&MenuManager_MidLevel_ButEventMapConf);

  switch (MenuManager_MidLevel_InternalState)
  {
    case MENUMANAGER_MIDLEVEL_INTERNALSTATE_READY:
      break;
    case MENUMANAGER_MIDLEVEL_INTERNALSTATE_RUNNING:
    {
      uint16_t tempMidLevel;

      MenuManager_Common_BcdToDecConv(&(MenuManager_MidLevel_Value), &(MenuManager_MidLevel_UnitVal(0)), (uint8_t)MENUMANAGER_MIDLEVEL_UNITVAL_LENGTH);

      tempMidLevel = (uint16_t)MenuManager_MidLevel_Value;

      ProgramManager_FillLevelSetup_MidLevel_SetData(&tempMidLevel);

      ProgramManager_FillLevelSetup_MidLevel_GetData(&(ProgramManager_gParamConfig.fillLevelCfg.midLevel));

      MenuManager_MidLevel_InternalState = MENUMANAGER_MIDLEVEL_INTERNALSTATE_DONE;
      
      break;
    }
    case MENUMANAGER_MIDLEVEL_INTERNALSTATE_DONE:
    {
      MenuManager_MidLevel_LcdShowDone();

      MenuManager_MidLevel_InternalState = MENUMANAGER_MIDLEVEL_INTERNALSTATE_EXITING;

      break;
    }
    case MENUMANAGER_MIDLEVEL_INTERNALSTATE_EXITING:
    default:
      break;
  }
}

/*=============================================================================================*/
static void MenuManager_MidLevel_SubTickHandler(void)
{
  if (MenuManager_MidLevel_InternalState == MENUMANAGER_MIDLEVEL_INTERNALSTATE_EXITING)
  {
    MenuManager_MidLevel_Counter += (uint32_t)1U;

    if (MenuManager_MidLevel_Counter >= MENUMANAGER_MIDLEVEL_COUNTER_MAX)
    {
      MenuManager_MidLevel_Counter = (uint32_t)0U;
      
      Fsm_TriggerEvent(&MenuManager_FsmContext, (Fsm_EventType)MENUMANAGER_EVENT_PREV);
    }
  }
}



#ifdef  __cplusplus
}
#endif
