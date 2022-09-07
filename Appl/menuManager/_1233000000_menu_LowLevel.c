/* 
 * File:   _1233000000_menu_LowLevel.c
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
#define MENUMANAGER_LOWLEVEL_INTERNALDATALENGTH                       (uint8_t)10U

#define MENUMANAGER_LOWLEVEL_INTERNALSTATE_IDX                        0U
#define MENUMANAGER_LOWLEVEL_COUNTER_IDX                              1U
#define MENUMANAGER_LOWLEVEL_CURPOS_IDX                               2U
#define MENUMANAGER_LOWLEVEL_VALUE_MIN_IDX                            3U
#define MENUMANAGER_LOWLEVEL_VALUE_MAX_IDX                            4U
#define MENUMANAGER_LOWLEVEL_VALUE_IDX                                5U
#define MENUMANAGER_LOWLEVEL_UNITVAL_IDX                              6U

#define MenuManager_LowLevel_InternalState                            MenuManager_GetInternalDataPtr(MENUMANAGER_LOWLEVEL_INTERNALSTATE_IDX)
#define MenuManager_LowLevel_Counter                                  MenuManager_GetInternalDataPtr(MENUMANAGER_LOWLEVEL_COUNTER_IDX)
#define MenuManager_LowLevel_CurPos                                   MenuManager_GetInternalDataPtr(MENUMANAGER_LOWLEVEL_CURPOS_IDX)
#define MenuManager_LowLevel_ValueMin                                 MenuManager_GetInternalDataPtr(MENUMANAGER_LOWLEVEL_VALUE_MIN_IDX)
#define MenuManager_LowLevel_ValueMax                                 MenuManager_GetInternalDataPtr(MENUMANAGER_LOWLEVEL_VALUE_MAX_IDX)
#define MenuManager_LowLevel_Value                                    MenuManager_GetInternalDataPtr(MENUMANAGER_LOWLEVEL_VALUE_IDX)
#define MenuManager_LowLevel_UnitVal(idx)                             MenuManager_GetInternalDataPtr(MENUMANAGER_LOWLEVEL_UNITVAL_IDX + idx)



#define MENUMANAGER_LOWLEVEL_INTERNALSTATE_READY                      (uint32_t)0U
#define MENUMANAGER_LOWLEVEL_INTERNALSTATE_RUNNING                    (uint32_t)1U
#define MENUMANAGER_LOWLEVEL_INTERNALSTATE_DONE                       (uint32_t)2U
#define MENUMANAGER_LOWLEVEL_INTERNALSTATE_EXITING                    (uint32_t)3U

#define MENUMANAGER_LOWLEVEL_COUNTER_MAX                              (uint32_t)50U

#define MENUMANAGER_LOWLEVEL_CURPOS_XPOS                              (uint32_t)8U
#define MENUMANAGER_LOWLEVEL_CURPOS_YPOS                              (uint32_t)5U

#define MENUMANAGER_LOWLEVEL_UNITVAL_LENGTH                           (uint32_t)4U



/** Menu manager main titles and child menu titles */
static const uint8_t MenuManager_LowLevel_MainTitleStr[] =            "LOW LEVEL";



/** Menu manager button event mapping array */
static MenuManager_ButEventMapStruct MenuManager_LowLevel_ButEventMap[6] =
{
  { IOMANAGER_BUTTON_STATE_START,                                     MENUMANAGER_EVENT_START_BUT             },
  { IOMANAGER_BUTTON_STATE_STOP,                                      MENUMANAGER_EVENT_STOP_BUT              },
  { IOMANAGER_BUTTON_STATE_UP,                                        MENUMANAGER_EVENT_UP_BUT                },
  { IOMANAGER_BUTTON_STATE_DOWN,                                      MENUMANAGER_EVENT_DOWN_BUT              },
  { IOMANAGER_BUTTON_STATE_ADD,                                       MENUMANAGER_EVENT_ADD_BUT               },
  { IOMANAGER_BUTTON_STATE_SUB,                                       MENUMANAGER_EVENT_SUB_BUT               }
};

/** Menu manager button event mapping configuration */
static MenuManager_ButEventMapConfStruct MenuManager_LowLevel_ButEventMapConf =
{
  .butEventMapNum             = (uint8_t)6U,
  .butEventMapCfg             = &MenuManager_LowLevel_ButEventMap
};



/** Menu manager event handlers */
static Fsm_GuardType MenuManager_LowLevel_Entry                           (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_LowLevel_Exit                            (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_LowLevel_Submenu1                        (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_LowLevel_StartBut                        (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_LowLevel_StopBut                         (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_LowLevel_UpBut                           (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_LowLevel_DownBut                         (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_LowLevel_AddBut                          (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_LowLevel_SubBut                          (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);

/** Menu manager state machine */
Fsm_EventEntryStruct MenuManager_LowLevel_StateMachine[9] =
{
  FSM_TRIGGER_ENTRY             (                                     MenuManager_LowLevel_Entry                                                      ),
  FSM_TRIGGER_EXIT              (                                     MenuManager_LowLevel_Exit                                                       ),
  FSM_TRIGGER_TRANSITION_ACTION ( MENUMANAGER_EVENT_SUBMENU_1,        MenuManager_LowLevel_Submenu1,          MENUMANAGER_STATE_FILL_LEVEL_SETUP      ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_START_BUT,        MenuManager_LowLevel_StartBut                                                   ),
  FSM_TRIGGER_TRANSITION_ACTION ( MENUMANAGER_EVENT_STOP_BUT,         MenuManager_LowLevel_StopBut,           MENUMANAGER_STATE_FILL_LEVEL_SETUP      ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_UP_BUT,           MenuManager_LowLevel_UpBut                                                      ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_DOWN_BUT,         MenuManager_LowLevel_DownBut                                                    ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_ADD_BUT,          MenuManager_LowLevel_AddBut                                                     ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_SUB_BUT,          MenuManager_LowLevel_SubBut                                                     )
};



/*===============================================================================================
*                                   LOCAL FUNCTION PROTOTYPES
===============================================================================================*/

static void MenuManager_LowLevel_LcdShowMainTitle(void);
static void MenuManager_LowLevel_LcdShowAdjust(void);
static void MenuManager_LowLevel_LcdShowDone(void);

static void MenuManager_LowLevel_SubMainFunction(void);
static void MenuManager_LowLevel_SubTickHandler(void);



/*===============================================================================================
*                                       LOCAL FUNCTIONS
===============================================================================================*/

static void MenuManager_LowLevel_LcdShowMainTitle(void)
{
  /* Print main title */
  MenuManager_Common_LcdClearAdjustMenuStatic();
  MenuManager_Common_LcdShowMainTitle(MenuManager_LowLevel_MainTitleStr);
  MenuManager_Common_LcdShowOkExitChoice();
  MenuManager_Common_LcdUpdateAdjustMenuStatic();
}

/*=============================================================================================*/
static void MenuManager_LowLevel_LcdShowAdjust(void)
{
  MenuManager_Common_LcdClearAdjustMenuDynamic();

  MenuManager_Common_LcdShowAdjustUnitVal
  (
    MENUMANAGER_LOWLEVEL_CURPOS_XPOS,
    MENUMANAGER_LOWLEVEL_CURPOS_YPOS,
    &(MenuManager_LowLevel_UnitVal(0)),
    MENUMANAGER_LOWLEVEL_UNITVAL_LENGTH
  );
  
  MenuManager_Common_LcdShowAdjustArrow
  (
    MENUMANAGER_LOWLEVEL_CURPOS_XPOS,
    MENUMANAGER_LOWLEVEL_CURPOS_YPOS,
    MenuManager_LowLevel_CurPos
  );

  MenuManager_Common_LcdUpdateAdjustMenuDynamic();
}

/*=============================================================================================*/
static void MenuManager_LowLevel_LcdShowDone(void)
{
  MenuManager_Common_LcdClearAll();

  MenuManager_Common_LcdShowAdjustUnitVal
  (
    MENUMANAGER_LOWLEVEL_CURPOS_XPOS,
    MENUMANAGER_LOWLEVEL_CURPOS_YPOS,
    &(MenuManager_LowLevel_UnitVal(0)),
    MENUMANAGER_LOWLEVEL_UNITVAL_LENGTH
  );

  MenuManager_Common_LcdUpdateAll();
}



/*=============================================================================================*/
static Fsm_GuardType MenuManager_LowLevel_Entry(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  MenuManager_SubMainFunction = MenuManager_LowLevel_SubMainFunction;
  MenuManager_SubTickHandler = MenuManager_LowLevel_SubTickHandler;

  /* Check if previous state data hierachy is not empty */
  if (pFsmContext->dataHierachy != NULL)
  {
    if (pFsmContext->dataHierachy->dataId == MENUMANAGER_STATE_FILL_LEVEL_SETUP)
    {
      /* Release previous state data hierachy */
      MenuManager_free(pFsmContext->dataHierachy);
      pFsmContext->dataHierachy = NULL;

      MenuManager_InternalDataPush(MENUMANAGER_LOWLEVEL_INTERNALDATALENGTH);

      MenuManager_LowLevel_InternalState = MENUMANAGER_LOWLEVEL_INTERNALSTATE_READY;
      MenuManager_LowLevel_Counter = (uint32_t)0U;
      MenuManager_LowLevel_CurPos = (uint32_t)0U;

      MenuManager_LowLevel_ValueMin = (uint32_t)PROGRAMMANAGER_FILLLEVELSETUP_WATER_LEVEL_MIN;
      MenuManager_LowLevel_ValueMax = (uint32_t)(ProgramManager_gParamConfig.fillLevelCfg.midLevel);

      MenuManager_LowLevel_Value = (uint32_t)(ProgramManager_gParamConfig.fillLevelCfg.lowLevel);

      MenuManager_Common_DecToBcdConv
      (
        &(MenuManager_LowLevel_Value),
        &(MenuManager_LowLevel_UnitVal(0)),
        (uint8_t)MENUMANAGER_LOWLEVEL_UNITVAL_LENGTH
      );
    }
    else
    {
      return FSM_GUARD_FALSE;
    }

    MenuManager_LowLevel_LcdShowMainTitle();
    MenuManager_LowLevel_LcdShowAdjust();

    return FSM_GUARD_TRUE;
  }

  return FSM_GUARD_FALSE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_LowLevel_Exit(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  MenuManager_SubMainFunction = NULL;
  MenuManager_SubTickHandler = NULL;

  Fsm_DataHierachyStruct* dataHierachy;

  dataHierachy = (Fsm_DataHierachyStruct *)MenuManager_malloc(sizeof(Fsm_DataHierachyStruct));
  dataHierachy->dataId = MENUMANAGER_STATE_LOW_LEVEL;

  pFsmContext->dataHierachy = dataHierachy;

  /* Free internal data */
  MenuManager_InternalDataPop();
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_LowLevel_Submenu1(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_LowLevel_StartBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_LowLevel_InternalState == MENUMANAGER_LOWLEVEL_INTERNALSTATE_READY)
  {
    MenuManager_LowLevel_InternalState = MENUMANAGER_LOWLEVEL_INTERNALSTATE_RUNNING;
  }
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_LowLevel_StopBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_LowLevel_InternalState != MENUMANAGER_LOWLEVEL_INTERNALSTATE_READY)
  {
    return FSM_GUARD_FALSE;
  }
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_LowLevel_UpBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  uint32_t tempUnitVal;
  uint32_t tempVal;

  if (MenuManager_LowLevel_InternalState == MENUMANAGER_LOWLEVEL_INTERNALSTATE_READY)
  {
    tempUnitVal = MenuManager_LowLevel_UnitVal(MenuManager_LowLevel_CurPos);

    MenuManager_LowLevel_UnitVal(MenuManager_LowLevel_CurPos) += (uint32_t)1U;

    if (MenuManager_LowLevel_UnitVal(MenuManager_LowLevel_CurPos) > (uint32_t)9U)
    {
      MenuManager_LowLevel_UnitVal(MenuManager_LowLevel_CurPos) = (uint32_t)0U;
    }

    MenuManager_Common_BcdToDecConv(&tempVal, &(MenuManager_LowLevel_UnitVal(0)), (uint8_t)MENUMANAGER_LOWLEVEL_UNITVAL_LENGTH);

    if ((tempVal <= MenuManager_LowLevel_ValueMin) || (tempVal >= MenuManager_LowLevel_ValueMax))
    {
      MenuManager_LowLevel_UnitVal(MenuManager_LowLevel_CurPos) = tempUnitVal;
    }

    MenuManager_LowLevel_LcdShowAdjust();
  }

  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_LowLevel_DownBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  uint32_t tempUnitVal;
  uint32_t tempVal;

  if (MenuManager_LowLevel_InternalState == MENUMANAGER_LOWLEVEL_INTERNALSTATE_READY)
  {
    tempUnitVal = MenuManager_LowLevel_UnitVal(MenuManager_LowLevel_CurPos);

    MenuManager_LowLevel_UnitVal(MenuManager_LowLevel_CurPos) -= (uint32_t)1U;

    if (MenuManager_LowLevel_UnitVal(MenuManager_LowLevel_CurPos) > (uint32_t)9U)
    {
      MenuManager_LowLevel_UnitVal(MenuManager_LowLevel_CurPos) = (uint32_t)9U;
    }

    MenuManager_Common_BcdToDecConv(&tempVal, &(MenuManager_LowLevel_UnitVal(0)), (uint8_t)MENUMANAGER_LOWLEVEL_UNITVAL_LENGTH);

    if ((tempVal <= MenuManager_LowLevel_ValueMin) || (tempVal >= MenuManager_LowLevel_ValueMax))
    {
      MenuManager_LowLevel_UnitVal(MenuManager_LowLevel_CurPos) = tempUnitVal;
    }

    MenuManager_LowLevel_LcdShowAdjust();
  }
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_LowLevel_AddBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_LowLevel_InternalState == MENUMANAGER_LOWLEVEL_INTERNALSTATE_READY)
  {
    if (MenuManager_LowLevel_CurPos > (uint32_t)0U)
    {
      MenuManager_LowLevel_CurPos -= (uint32_t)1U;
    }

    MenuManager_LowLevel_LcdShowAdjust();
  }
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_LowLevel_SubBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_LowLevel_InternalState == MENUMANAGER_LOWLEVEL_INTERNALSTATE_READY)
  {
    if (MenuManager_LowLevel_CurPos < (MENUMANAGER_LOWLEVEL_UNITVAL_LENGTH - (uint32_t)1U))
    {
      MenuManager_LowLevel_CurPos += (uint32_t)1U;
    }
    
    MenuManager_LowLevel_LcdShowAdjust();
  }
  
  return FSM_GUARD_TRUE;
}



/*=============================================================================================*/
static void MenuManager_LowLevel_SubMainFunction(void)
{
  MenuManager_Common_ButEventMapHandler(&MenuManager_LowLevel_ButEventMapConf);

  switch (MenuManager_LowLevel_InternalState)
  {
    case MENUMANAGER_LOWLEVEL_INTERNALSTATE_READY:
      break;
    case MENUMANAGER_LOWLEVEL_INTERNALSTATE_RUNNING:
    {
      uint16_t tempLowLevel;

      MenuManager_Common_BcdToDecConv(&(MenuManager_LowLevel_Value), &(MenuManager_LowLevel_UnitVal(0)), (uint8_t)MENUMANAGER_LOWLEVEL_UNITVAL_LENGTH);

      tempLowLevel = (uint16_t)MenuManager_LowLevel_Value;

      ProgramManager_FillLevelSetup_LowLevel_SetData(&tempLowLevel);

      ProgramManager_FillLevelSetup_LowLevel_GetData(&(ProgramManager_gParamConfig.fillLevelCfg.lowLevel));

      MenuManager_LowLevel_InternalState = MENUMANAGER_LOWLEVEL_INTERNALSTATE_DONE;
      
      break;
    }
    case MENUMANAGER_LOWLEVEL_INTERNALSTATE_DONE:
    {
      MenuManager_LowLevel_LcdShowDone();

      MenuManager_LowLevel_InternalState = MENUMANAGER_LOWLEVEL_INTERNALSTATE_EXITING;

      break;
    }
    case MENUMANAGER_LOWLEVEL_INTERNALSTATE_EXITING:
    default:
      break;
  }
}

/*=============================================================================================*/
static void MenuManager_LowLevel_SubTickHandler(void)
{
  if (MenuManager_LowLevel_InternalState == MENUMANAGER_LOWLEVEL_INTERNALSTATE_EXITING)
  {
    MenuManager_LowLevel_Counter += (uint32_t)1U;

    if (MenuManager_LowLevel_Counter >= MENUMANAGER_LOWLEVEL_COUNTER_MAX)
    {
      MenuManager_LowLevel_Counter = (uint32_t)0U;
      
      Fsm_TriggerEvent(&MenuManager_FsmContext, (Fsm_EventType)MENUMANAGER_EVENT_SUBMENU_1);
    }
  }
}



#ifdef  __cplusplus
}
#endif
