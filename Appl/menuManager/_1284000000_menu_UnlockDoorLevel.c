/* 
 * File:   _1284000000_menu_UnlockDoorLevel.c
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
#define MENUMANAGER_UNLOCKDOORLEVEL_INTERNALDATALENGTH                (uint8_t)10U

#define MENUMANAGER_UNLOCKDOORLEVEL_INTERNALSTATE_IDX                 0U
#define MENUMANAGER_UNLOCKDOORLEVEL_COUNTER_IDX                       1U
#define MENUMANAGER_UNLOCKDOORLEVEL_CURPOS_IDX                        2U
#define MENUMANAGER_UNLOCKDOORLEVEL_VALUE_MIN_IDX                     3U
#define MENUMANAGER_UNLOCKDOORLEVEL_VALUE_MAX_IDX                     4U
#define MENUMANAGER_UNLOCKDOORLEVEL_VALUE_IDX                         5U
#define MENUMANAGER_UNLOCKDOORLEVEL_UNITVAL_IDX                       6U

#define MenuManager_UnlockDoorLevel_InternalState                     MenuManager_GetInternalDataPtr(MENUMANAGER_UNLOCKDOORLEVEL_INTERNALSTATE_IDX)
#define MenuManager_UnlockDoorLevel_Counter                           MenuManager_GetInternalDataPtr(MENUMANAGER_UNLOCKDOORLEVEL_COUNTER_IDX)
#define MenuManager_UnlockDoorLevel_CurPos                            MenuManager_GetInternalDataPtr(MENUMANAGER_UNLOCKDOORLEVEL_CURPOS_IDX)
#define MenuManager_UnlockDoorLevel_ValueMin                          MenuManager_GetInternalDataPtr(MENUMANAGER_UNLOCKDOORLEVEL_VALUE_MIN_IDX)
#define MenuManager_UnlockDoorLevel_ValueMax                          MenuManager_GetInternalDataPtr(MENUMANAGER_UNLOCKDOORLEVEL_VALUE_MAX_IDX)
#define MenuManager_UnlockDoorLevel_Value                             MenuManager_GetInternalDataPtr(MENUMANAGER_UNLOCKDOORLEVEL_VALUE_IDX)
#define MenuManager_UnlockDoorLevel_UnitVal(idx)                      MenuManager_GetInternalDataPtr(MENUMANAGER_UNLOCKDOORLEVEL_UNITVAL_IDX + idx)



#define MENUMANAGER_UNLOCKDOORLEVEL_INTERNALSTATE_READY               (uint32_t)0U
#define MENUMANAGER_UNLOCKDOORLEVEL_INTERNALSTATE_RUNNING             (uint32_t)1U
#define MENUMANAGER_UNLOCKDOORLEVEL_INTERNALSTATE_DONE                (uint32_t)2U
#define MENUMANAGER_UNLOCKDOORLEVEL_INTERNALSTATE_EXITING             (uint32_t)3U

#define MENUMANAGER_UNLOCKDOORLEVEL_COUNTER_MAX                       (uint32_t)50U

#define MENUMANAGER_UNLOCKDOORLEVEL_CURPOS_XPOS                       (uint32_t)8U
#define MENUMANAGER_UNLOCKDOORLEVEL_CURPOS_YPOS                       (uint32_t)5U

#define MENUMANAGER_UNLOCKDOORLEVEL_UNITVAL_LENGTH                    (uint32_t)4U



/** Menu manager main titles and child menu titles */
static const uint8_t MenuManager_UnlockDoorLevel_MainTitleStr[] =     "UNLOCK DOOR LVL";



/** Menu manager button event mapping array */
static MenuManager_ButEventMapStruct MenuManager_UnlockDoorLevel_ButEventMap[6] =
{
  { IOMANAGER_BUTTON_STATE_START,                                     MENUMANAGER_EVENT_START_BUT             },
  { IOMANAGER_BUTTON_STATE_STOP,                                      MENUMANAGER_EVENT_STOP_BUT              },
  { IOMANAGER_BUTTON_STATE_UP,                                        MENUMANAGER_EVENT_UP_BUT                },
  { IOMANAGER_BUTTON_STATE_DOWN,                                      MENUMANAGER_EVENT_DOWN_BUT              },
  { IOMANAGER_BUTTON_STATE_ADD,                                       MENUMANAGER_EVENT_ADD_BUT               },
  { IOMANAGER_BUTTON_STATE_SUB,                                       MENUMANAGER_EVENT_SUB_BUT               }
};

/** Menu manager button event mapping configuration */
static MenuManager_ButEventMapConfStruct MenuManager_UnlockDoorLevel_ButEventMapConf =
{
  .butEventMapNum             = (uint8_t)6U,
  .butEventMapCfg             = &MenuManager_UnlockDoorLevel_ButEventMap
};



/** Menu manager event handlers */
static Fsm_GuardType MenuManager_UnlockDoorLevel_Entry                (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_UnlockDoorLevel_Exit                 (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_UnlockDoorLevel_StartBut             (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_UnlockDoorLevel_StopBut              (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_UnlockDoorLevel_UpBut                (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_UnlockDoorLevel_DownBut              (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_UnlockDoorLevel_AddBut               (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_UnlockDoorLevel_SubBut               (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);

/** Menu manager state machine */
Fsm_EventEntryStruct MenuManager_UnlockDoorLevel_StateMachine[9] =
{
  FSM_TRIGGER_ENTRY             (                                     MenuManager_UnlockDoorLevel_Entry                                               ),
  FSM_TRIGGER_EXIT              (                                     MenuManager_UnlockDoorLevel_Exit                                                ),
  FSM_TRIGGER_TRANSITION        ( MENUMANAGER_EVENT_PREV,                                                     MENUMANAGER_STATE_DOOR_LOCK_SETUP       ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_START_BUT,        MenuManager_UnlockDoorLevel_StartBut                                            ),
  FSM_TRIGGER_TRANSITION_ACTION ( MENUMANAGER_EVENT_STOP_BUT,         MenuManager_UnlockDoorLevel_StopBut,    MENUMANAGER_STATE_DOOR_LOCK_SETUP       ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_UP_BUT,           MenuManager_UnlockDoorLevel_UpBut                                               ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_DOWN_BUT,         MenuManager_UnlockDoorLevel_DownBut                                             ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_ADD_BUT,          MenuManager_UnlockDoorLevel_AddBut                                              ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_SUB_BUT,          MenuManager_UnlockDoorLevel_SubBut                                              )
};



/*===============================================================================================
*                                   LOCAL FUNCTION PROTOTYPES
===============================================================================================*/

static void MenuManager_UnlockDoorLevel_LcdShowMainTitle(void);
static void MenuManager_UnlockDoorLevel_LcdShowAdjust(void);
static void MenuManager_UnlockDoorLevel_LcdShowDone(void);

static void MenuManager_UnlockDoorLevel_SubMainFunction(void);
static void MenuManager_UnlockDoorLevel_SubTickHandler(void);



/*===============================================================================================
*                                       LOCAL FUNCTIONS
===============================================================================================*/

static void MenuManager_UnlockDoorLevel_LcdShowMainTitle(void)
{
  /* Print main title */
  MenuManager_Common_LcdClearAdjustMenuStatic();
  MenuManager_Common_LcdShowMainTitle(MenuManager_UnlockDoorLevel_MainTitleStr);
  MenuManager_Common_LcdShowOkExitChoice();
  MenuManager_Common_LcdUpdateAdjustMenuStatic();
}

/*=============================================================================================*/
static void MenuManager_UnlockDoorLevel_LcdShowAdjust(void)
{
  MenuManager_Common_LcdClearAdjustMenuDynamic();

  MenuManager_Common_LcdShowAdjustUnitVal
  (
    MENUMANAGER_UNLOCKDOORLEVEL_CURPOS_XPOS,
    MENUMANAGER_UNLOCKDOORLEVEL_CURPOS_YPOS,
    &(MenuManager_UnlockDoorLevel_UnitVal(0)),
    MENUMANAGER_UNLOCKDOORLEVEL_UNITVAL_LENGTH
  );
  
  MenuManager_Common_LcdShowAdjustArrow
  (
    MENUMANAGER_UNLOCKDOORLEVEL_CURPOS_XPOS,
    MENUMANAGER_UNLOCKDOORLEVEL_CURPOS_YPOS,
    MenuManager_UnlockDoorLevel_CurPos
  );

  MenuManager_Common_LcdUpdateAdjustMenuDynamic();
}

/*=============================================================================================*/
static void MenuManager_UnlockDoorLevel_LcdShowDone(void)
{
  MenuManager_Common_LcdClearAll();

  MenuManager_Common_LcdShowAdjustUnitVal
  (
    MENUMANAGER_UNLOCKDOORLEVEL_CURPOS_XPOS,
    MENUMANAGER_UNLOCKDOORLEVEL_CURPOS_YPOS,
    &(MenuManager_UnlockDoorLevel_UnitVal(0)),
    MENUMANAGER_UNLOCKDOORLEVEL_UNITVAL_LENGTH
  );

  MenuManager_Common_LcdUpdateAll();
}



/*=============================================================================================*/
static Fsm_GuardType MenuManager_UnlockDoorLevel_Entry(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  HAL_StatusTypeDef retVal = HAL_OK;

  /* Check if previous state data hierachy is not empty */
  if (pFsmContext->dataHierachy != NULL)
  {
    if (pFsmContext->dataHierachy->dataId == MENUMANAGER_STATE_DOOR_LOCK_SETUP)
    {
      /* Release previous state data hierachy */
      MenuManager_free(pFsmContext->dataHierachy);
      pFsmContext->dataHierachy = NULL;

      MenuManager_InternalDataPush(MENUMANAGER_UNLOCKDOORLEVEL_INTERNALDATALENGTH);

      MenuManager_UnlockDoorLevel_InternalState = MENUMANAGER_UNLOCKDOORLEVEL_INTERNALSTATE_READY;
      MenuManager_UnlockDoorLevel_Counter = (uint32_t)0U;
      MenuManager_UnlockDoorLevel_CurPos = (uint32_t)0U;

      MenuManager_UnlockDoorLevel_ValueMin = (uint32_t)PROGRAMMANAGER_FILLLEVELSETUP_WATER_LEVEL_MIN;
      MenuManager_UnlockDoorLevel_ValueMax = (uint32_t)PROGRAMMANAGER_FILLLEVELSETUP_WATER_LEVEL_MAX;

      MenuManager_UnlockDoorLevel_Value = (uint32_t)(ProgramManager_gParamConfig.doorLockCfg.unlockDoorLevel);

      MenuManager_Common_DecToBcdConv
      (
        &(MenuManager_UnlockDoorLevel_Value),
        &(MenuManager_UnlockDoorLevel_UnitVal(0)),
        (uint8_t)MENUMANAGER_UNLOCKDOORLEVEL_UNITVAL_LENGTH
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
    MenuManager_UnlockDoorLevel_LcdShowMainTitle();
    MenuManager_UnlockDoorLevel_LcdShowAdjust();

    MenuManager_SubMainFunction = MenuManager_UnlockDoorLevel_SubMainFunction;
    MenuManager_SubTickHandler = MenuManager_UnlockDoorLevel_SubTickHandler;

    return FSM_GUARD_TRUE;
  }

  return FSM_GUARD_FALSE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_UnlockDoorLevel_Exit(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  Fsm_DataHierachyStruct* dataHierachy;

  MenuManager_SubMainFunction = NULL;
  MenuManager_SubTickHandler = NULL;

  dataHierachy = (Fsm_DataHierachyStruct *)MenuManager_malloc(sizeof(Fsm_DataHierachyStruct));
  dataHierachy->dataId = MENUMANAGER_STATE_UNLOCK_DOOR_LEVEL;

  pFsmContext->dataHierachy = dataHierachy;

  /* Free internal data */
  MenuManager_InternalDataPop();
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_UnlockDoorLevel_StartBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_UnlockDoorLevel_InternalState == MENUMANAGER_UNLOCKDOORLEVEL_INTERNALSTATE_READY)
  {
    MenuManager_UnlockDoorLevel_InternalState = MENUMANAGER_UNLOCKDOORLEVEL_INTERNALSTATE_RUNNING;
  }
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_UnlockDoorLevel_StopBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_UnlockDoorLevel_InternalState != MENUMANAGER_UNLOCKDOORLEVEL_INTERNALSTATE_READY)
  {
    return FSM_GUARD_FALSE;
  }
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_UnlockDoorLevel_UpBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  uint32_t tempUnitVal;
  uint32_t tempVal;

  if (MenuManager_UnlockDoorLevel_InternalState == MENUMANAGER_UNLOCKDOORLEVEL_INTERNALSTATE_READY)
  {
    tempUnitVal = MenuManager_UnlockDoorLevel_UnitVal(MenuManager_UnlockDoorLevel_CurPos);

    MenuManager_UnlockDoorLevel_UnitVal(MenuManager_UnlockDoorLevel_CurPos) += (uint32_t)1U;

    if (MenuManager_UnlockDoorLevel_UnitVal(MenuManager_UnlockDoorLevel_CurPos) > (uint32_t)9U)
    {
      MenuManager_UnlockDoorLevel_UnitVal(MenuManager_UnlockDoorLevel_CurPos) = (uint32_t)0U;
    }

    MenuManager_Common_BcdToDecConv(&tempVal, &(MenuManager_UnlockDoorLevel_UnitVal(0)), (uint8_t)MENUMANAGER_UNLOCKDOORLEVEL_UNITVAL_LENGTH);

    if ((tempVal < MenuManager_UnlockDoorLevel_ValueMin) || (tempVal >= MenuManager_UnlockDoorLevel_ValueMax))
    {
      MenuManager_UnlockDoorLevel_UnitVal(MenuManager_UnlockDoorLevel_CurPos) = tempUnitVal;
    }

    MenuManager_UnlockDoorLevel_LcdShowAdjust();
  }

  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_UnlockDoorLevel_DownBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  uint32_t tempUnitVal;
  uint32_t tempVal;

  if (MenuManager_UnlockDoorLevel_InternalState == MENUMANAGER_UNLOCKDOORLEVEL_INTERNALSTATE_READY)
  {
    tempUnitVal = MenuManager_UnlockDoorLevel_UnitVal(MenuManager_UnlockDoorLevel_CurPos);

    MenuManager_UnlockDoorLevel_UnitVal(MenuManager_UnlockDoorLevel_CurPos) -= (uint32_t)1U;

    if (MenuManager_UnlockDoorLevel_UnitVal(MenuManager_UnlockDoorLevel_CurPos) > (uint32_t)9U)
    {
      MenuManager_UnlockDoorLevel_UnitVal(MenuManager_UnlockDoorLevel_CurPos) = (uint32_t)9U;
    }

    MenuManager_Common_BcdToDecConv(&tempVal, &(MenuManager_UnlockDoorLevel_UnitVal(0)), (uint8_t)MENUMANAGER_UNLOCKDOORLEVEL_UNITVAL_LENGTH);

    if ((tempVal < MenuManager_UnlockDoorLevel_ValueMin) || (tempVal >= MenuManager_UnlockDoorLevel_ValueMax))
    {
      MenuManager_UnlockDoorLevel_UnitVal(MenuManager_UnlockDoorLevel_CurPos) = tempUnitVal;
    }

    MenuManager_UnlockDoorLevel_LcdShowAdjust();
  }
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_UnlockDoorLevel_AddBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_UnlockDoorLevel_InternalState == MENUMANAGER_UNLOCKDOORLEVEL_INTERNALSTATE_READY)
  {
    if (MenuManager_UnlockDoorLevel_CurPos > (uint32_t)0U)
    {
      MenuManager_UnlockDoorLevel_CurPos -= (uint32_t)1U;
    }

    MenuManager_UnlockDoorLevel_LcdShowAdjust();
  }
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_UnlockDoorLevel_SubBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_UnlockDoorLevel_InternalState == MENUMANAGER_UNLOCKDOORLEVEL_INTERNALSTATE_READY)
  {
    if (MenuManager_UnlockDoorLevel_CurPos < (MENUMANAGER_UNLOCKDOORLEVEL_UNITVAL_LENGTH - (uint32_t)1U))
    {
      MenuManager_UnlockDoorLevel_CurPos += (uint32_t)1U;
    }
    
    MenuManager_UnlockDoorLevel_LcdShowAdjust();
  }
  
  return FSM_GUARD_TRUE;
}



/*=============================================================================================*/
static void MenuManager_UnlockDoorLevel_SubMainFunction(void)
{
  MenuManager_Common_ButEventMapHandler(&MenuManager_UnlockDoorLevel_ButEventMapConf);

  switch (MenuManager_UnlockDoorLevel_InternalState)
  {
    case MENUMANAGER_UNLOCKDOORLEVEL_INTERNALSTATE_READY:
      break;
    case MENUMANAGER_UNLOCKDOORLEVEL_INTERNALSTATE_RUNNING:
    {
      uint16_t tempUnlockDoorLevel;

      MenuManager_Common_BcdToDecConv(&(MenuManager_UnlockDoorLevel_Value), &(MenuManager_UnlockDoorLevel_UnitVal(0)), (uint8_t)MENUMANAGER_UNLOCKDOORLEVEL_UNITVAL_LENGTH);

      tempUnlockDoorLevel = (uint16_t)MenuManager_UnlockDoorLevel_Value;

      ProgramManager_DoorLockSetup_UnlockDoorLevel_SetData(&tempUnlockDoorLevel);

      ProgramManager_DoorLockSetup_UnlockDoorLevel_GetData(&(ProgramManager_gParamConfig.doorLockCfg.unlockDoorLevel));

      MenuManager_UnlockDoorLevel_InternalState = MENUMANAGER_UNLOCKDOORLEVEL_INTERNALSTATE_DONE;
      
      break;
    }
    case MENUMANAGER_UNLOCKDOORLEVEL_INTERNALSTATE_DONE:
    {
      MenuManager_UnlockDoorLevel_LcdShowDone();

      MenuManager_UnlockDoorLevel_InternalState = MENUMANAGER_UNLOCKDOORLEVEL_INTERNALSTATE_EXITING;

      break;
    }
    case MENUMANAGER_UNLOCKDOORLEVEL_INTERNALSTATE_EXITING:
    default:
      break;
  }
}

/*=============================================================================================*/
static void MenuManager_UnlockDoorLevel_SubTickHandler(void)
{
  if (MenuManager_UnlockDoorLevel_InternalState == MENUMANAGER_UNLOCKDOORLEVEL_INTERNALSTATE_EXITING)
  {
    MenuManager_UnlockDoorLevel_Counter += (uint32_t)1U;

    if (MenuManager_UnlockDoorLevel_Counter >= MENUMANAGER_UNLOCKDOORLEVEL_COUNTER_MAX)
    {
      MenuManager_UnlockDoorLevel_Counter = (uint32_t)0U;
      
      Fsm_TriggerEvent(&MenuManager_FsmContext, (Fsm_EventType)MENUMANAGER_EVENT_PREV);
    }
  }
}



#ifdef  __cplusplus
}
#endif
