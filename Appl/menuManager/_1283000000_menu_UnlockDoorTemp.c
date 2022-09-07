/* 
 * File:   _1283000000_menu_UnlockDoorTemp.c
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
#define MENUMANAGER_UNLOCKDOORTEMP_INTERNALDATALENGTH                 (uint8_t)10U

#define MENUMANAGER_UNLOCKDOORTEMP_INTERNALSTATE_IDX                  0U
#define MENUMANAGER_UNLOCKDOORTEMP_COUNTER_IDX                        1U
#define MENUMANAGER_UNLOCKDOORTEMP_CURPOS_IDX                         2U
#define MENUMANAGER_UNLOCKDOORTEMP_VALUE_MIN_IDX                      3U
#define MENUMANAGER_UNLOCKDOORTEMP_VALUE_MAX_IDX                      4U
#define MENUMANAGER_UNLOCKDOORTEMP_VALUE_IDX                          5U
#define MENUMANAGER_UNLOCKDOORTEMP_UNITVAL_IDX                        6U

#define MenuManager_UnlockDoorTemp_InternalState                      MenuManager_GetInternalDataPtr(MENUMANAGER_UNLOCKDOORTEMP_INTERNALSTATE_IDX)
#define MenuManager_UnlockDoorTemp_Counter                            MenuManager_GetInternalDataPtr(MENUMANAGER_UNLOCKDOORTEMP_COUNTER_IDX)
#define MenuManager_UnlockDoorTemp_CurPos                             MenuManager_GetInternalDataPtr(MENUMANAGER_UNLOCKDOORTEMP_CURPOS_IDX)
#define MenuManager_UnlockDoorTemp_ValueMin                           MenuManager_GetInternalDataPtr(MENUMANAGER_UNLOCKDOORTEMP_VALUE_MIN_IDX)
#define MenuManager_UnlockDoorTemp_ValueMax                           MenuManager_GetInternalDataPtr(MENUMANAGER_UNLOCKDOORTEMP_VALUE_MAX_IDX)
#define MenuManager_UnlockDoorTemp_Value                              MenuManager_GetInternalDataPtr(MENUMANAGER_UNLOCKDOORTEMP_VALUE_IDX)
#define MenuManager_UnlockDoorTemp_UnitVal(idx)                       MenuManager_GetInternalDataPtr(MENUMANAGER_UNLOCKDOORTEMP_UNITVAL_IDX + idx)



#define MENUMANAGER_UNLOCKDOORTEMP_INTERNALSTATE_READY                (uint32_t)0U
#define MENUMANAGER_UNLOCKDOORTEMP_INTERNALSTATE_RUNNING              (uint32_t)1U
#define MENUMANAGER_UNLOCKDOORTEMP_INTERNALSTATE_DONE                 (uint32_t)2U
#define MENUMANAGER_UNLOCKDOORTEMP_INTERNALSTATE_EXITING              (uint32_t)3U

#define MENUMANAGER_UNLOCKDOORTEMP_COUNTER_MAX                        (uint32_t)50U

#define MENUMANAGER_UNLOCKDOORTEMP_CURPOS_XPOS                        (uint32_t)8U
#define MENUMANAGER_UNLOCKDOORTEMP_CURPOS_YPOS                        (uint32_t)5U

#define MENUMANAGER_UNLOCKDOORTEMP_UNIT_XPOS                          (uint32_t)12U
#define MENUMANAGER_UNLOCKDOORTEMP_UNIT_YPOS                          (uint32_t)5U

#define MENUMANAGER_UNLOCKDOORTEMP_UNITVAL_LENGTH                     (uint32_t)3U



/** Menu manager main titles and child menu titles */
static const uint8_t MenuManager_UnlockDoorTemp_MainTitleStr[] =      "UNLOCK DOOR TEMP";



/** Menu manager button event mapping array */
static MenuManager_ButEventMapStruct MenuManager_UnlockDoorTemp_ButEventMap[6] =
{
  { IOMANAGER_BUTTON_STATE_START,                                     MENUMANAGER_EVENT_START_BUT             },
  { IOMANAGER_BUTTON_STATE_STOP,                                      MENUMANAGER_EVENT_STOP_BUT              },
  { IOMANAGER_BUTTON_STATE_UP,                                        MENUMANAGER_EVENT_UP_BUT                },
  { IOMANAGER_BUTTON_STATE_DOWN,                                      MENUMANAGER_EVENT_DOWN_BUT              },
  { IOMANAGER_BUTTON_STATE_ADD,                                       MENUMANAGER_EVENT_ADD_BUT               },
  { IOMANAGER_BUTTON_STATE_SUB,                                       MENUMANAGER_EVENT_SUB_BUT               }
};

/** Menu manager button event mapping configuration */
static MenuManager_ButEventMapConfStruct MenuManager_UnlockDoorTemp_ButEventMapConf =
{
  .butEventMapNum             = (uint8_t)6U,
  .butEventMapCfg             = &MenuManager_UnlockDoorTemp_ButEventMap
};



/** Menu manager event handlers */
static Fsm_GuardType MenuManager_UnlockDoorTemp_Entry                     (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_UnlockDoorTemp_Exit                      (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_UnlockDoorTemp_Submenu1                  (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_UnlockDoorTemp_StartBut                  (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_UnlockDoorTemp_StopBut                   (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_UnlockDoorTemp_UpBut                     (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_UnlockDoorTemp_DownBut                   (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_UnlockDoorTemp_AddBut                    (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_UnlockDoorTemp_SubBut                    (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);

/** Menu manager state machine */
Fsm_EventEntryStruct MenuManager_UnlockDoorTemp_StateMachine[9] =
{
  FSM_TRIGGER_ENTRY             (                                     MenuManager_UnlockDoorTemp_Entry                                                  ),
  FSM_TRIGGER_EXIT              (                                     MenuManager_UnlockDoorTemp_Exit                                                   ),
  FSM_TRIGGER_TRANSITION_ACTION ( MENUMANAGER_EVENT_SUBMENU_1,        MenuManager_UnlockDoorTemp_Submenu1,      MENUMANAGER_STATE_DOOR_LOCK_SETUP       ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_START_BUT,        MenuManager_UnlockDoorTemp_StartBut                                               ),
  FSM_TRIGGER_TRANSITION_ACTION ( MENUMANAGER_EVENT_STOP_BUT,         MenuManager_UnlockDoorTemp_StopBut,       MENUMANAGER_STATE_DOOR_LOCK_SETUP       ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_UP_BUT,           MenuManager_UnlockDoorTemp_UpBut                                                  ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_DOWN_BUT,         MenuManager_UnlockDoorTemp_DownBut                                                ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_ADD_BUT,          MenuManager_UnlockDoorTemp_AddBut                                                 ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_SUB_BUT,          MenuManager_UnlockDoorTemp_SubBut                                                 )
};



/*===============================================================================================
*                                   LOCAL FUNCTION PROTOTYPES
===============================================================================================*/

static void MenuManager_UnlockDoorTemp_LcdShowMainTitle(void);
static void MenuManager_UnlockDoorTemp_LcdShowAdjust(void);
static void MenuManager_UnlockDoorTemp_LcdShowDone(void);

static void MenuManager_UnlockDoorTemp_SubMainFunction(void);
static void MenuManager_UnlockDoorTemp_SubTickHandler(void);



/*===============================================================================================
*                                       LOCAL FUNCTIONS
===============================================================================================*/

static void MenuManager_UnlockDoorTemp_LcdShowMainTitle(void)
{
  /* Print main title */
  MenuManager_Common_LcdClearAdjustMenuStatic();
  MenuManager_Common_LcdShowMainTitle(MenuManager_UnlockDoorTemp_MainTitleStr);
  MenuManager_Common_LcdShowOkExitChoice();
  MenuManager_Common_LcdUpdateAdjustMenuStatic();
}

/*=============================================================================================*/
static void MenuManager_UnlockDoorTemp_LcdShowAdjust(void)
{
  MenuManager_Common_LcdClearAdjustMenuDynamic();

  MenuManager_Common_LcdShowAdjustUnitVal
  (
    MENUMANAGER_UNLOCKDOORTEMP_CURPOS_XPOS,
    MENUMANAGER_UNLOCKDOORTEMP_CURPOS_YPOS,
    &(MenuManager_UnlockDoorTemp_UnitVal(0)),
    MENUMANAGER_UNLOCKDOORTEMP_UNITVAL_LENGTH
  );
  
  MenuManager_Common_LcdShowAdjustArrow
  (
    MENUMANAGER_UNLOCKDOORTEMP_CURPOS_XPOS,
    MENUMANAGER_UNLOCKDOORTEMP_CURPOS_YPOS,
    MenuManager_UnlockDoorTemp_CurPos
  );

  LCD_SetCursorPos(MENUMANAGER_UNLOCKDOORTEMP_UNIT_XPOS, MENUMANAGER_UNLOCKDOORTEMP_UNIT_YPOS, LCD_CURSOR_BY_FONT);
  if (ProgramManager_gParamConfig.machineFuncCfg.tempUnit == PROGRAMMANAGER_TEMP_UNIT_CELSIUS)
  {
    LCD_PutString((uint8_t *)MenuManager_Common_CelsiusDegStr);
  }
  else
  {
    LCD_PutString((uint8_t *)MenuManager_Common_FahrenheitDegStr);
  }

  MenuManager_Common_LcdUpdateAdjustMenuDynamic();
}

/*=============================================================================================*/
static void MenuManager_UnlockDoorTemp_LcdShowDone(void)
{
  MenuManager_Common_LcdClearAll();

  MenuManager_Common_LcdShowAdjustUnitVal
  (
    MENUMANAGER_UNLOCKDOORTEMP_CURPOS_XPOS,
    MENUMANAGER_UNLOCKDOORTEMP_CURPOS_YPOS,
    &(MenuManager_UnlockDoorTemp_UnitVal(0)),
    MENUMANAGER_UNLOCKDOORTEMP_UNITVAL_LENGTH
  );

  LCD_SetCursorPos(MENUMANAGER_UNLOCKDOORTEMP_UNIT_XPOS, MENUMANAGER_UNLOCKDOORTEMP_UNIT_YPOS, LCD_CURSOR_BY_FONT);
  if (ProgramManager_gParamConfig.machineFuncCfg.tempUnit == PROGRAMMANAGER_TEMP_UNIT_CELSIUS)
  {
    LCD_PutString((uint8_t *)MenuManager_Common_CelsiusDegStr);
  }
  else
  {
    LCD_PutString((uint8_t *)MenuManager_Common_FahrenheitDegStr);
  }

  MenuManager_Common_LcdUpdateAll();
}



/*=============================================================================================*/
static Fsm_GuardType MenuManager_UnlockDoorTemp_Entry(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  MenuManager_SubMainFunction = MenuManager_UnlockDoorTemp_SubMainFunction;
  MenuManager_SubTickHandler = MenuManager_UnlockDoorTemp_SubTickHandler;

  /* Check if previous state data hierachy is not empty */
  if (pFsmContext->dataHierachy != NULL)
  {
    if (pFsmContext->dataHierachy->dataId == MENUMANAGER_STATE_DOOR_LOCK_SETUP)
    {
      /* Release previous state data hierachy */
      MenuManager_free(pFsmContext->dataHierachy);
      pFsmContext->dataHierachy = NULL;

      MenuManager_InternalDataPush(MENUMANAGER_UNLOCKDOORTEMP_INTERNALDATALENGTH);

      MenuManager_UnlockDoorTemp_InternalState = MENUMANAGER_UNLOCKDOORTEMP_INTERNALSTATE_READY;
      MenuManager_UnlockDoorTemp_Counter = (uint32_t)0U;
      MenuManager_UnlockDoorTemp_CurPos = (uint32_t)0U;

      if (ProgramManager_gParamConfig.machineFuncCfg.tempUnit == PROGRAMMANAGER_TEMP_UNIT_CELSIUS)
      {
        MenuManager_UnlockDoorTemp_ValueMin = (uint32_t)PROGRAMMANAGER_HEATTEMPSETUP_WATER_TEMP_C_MIN;
        MenuManager_UnlockDoorTemp_ValueMax = (uint32_t)PROGRAMMANAGER_HEATTEMPSETUP_WATER_TEMP_C_MAX;
      }
      else
      {
        MenuManager_UnlockDoorTemp_ValueMin = (uint32_t)PROGRAMMANAGER_HEATTEMPSETUP_WATER_TEMP_F_MIN;
        MenuManager_UnlockDoorTemp_ValueMax = (uint32_t)PROGRAMMANAGER_HEATTEMPSETUP_WATER_TEMP_F_MAX;
      }

      MenuManager_UnlockDoorTemp_Value = (uint32_t)(ProgramManager_gParamConfig.doorLockCfg.unlockDoorTemp);

      MenuManager_Common_DecToBcdConv
      (
        &(MenuManager_UnlockDoorTemp_Value),
        &(MenuManager_UnlockDoorTemp_UnitVal(0)),
        (uint8_t)MENUMANAGER_UNLOCKDOORTEMP_UNITVAL_LENGTH
      );
    }
    else
    {
      return FSM_GUARD_FALSE;
    }

    MenuManager_UnlockDoorTemp_LcdShowMainTitle();
    MenuManager_UnlockDoorTemp_LcdShowAdjust();

    return FSM_GUARD_TRUE;
  }

  return FSM_GUARD_FALSE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_UnlockDoorTemp_Exit(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  MenuManager_SubMainFunction = NULL;
  MenuManager_SubTickHandler = NULL;

  Fsm_DataHierachyStruct* dataHierachy;

  dataHierachy = (Fsm_DataHierachyStruct *)MenuManager_malloc(sizeof(Fsm_DataHierachyStruct));
  dataHierachy->dataId = MENUMANAGER_STATE_UNLOCK_DOOR_TEMP;

  pFsmContext->dataHierachy = dataHierachy;

  /* Free internal data */
  MenuManager_InternalDataPop();
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_UnlockDoorTemp_Submenu1(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_UnlockDoorTemp_StartBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_UnlockDoorTemp_InternalState == MENUMANAGER_UNLOCKDOORTEMP_INTERNALSTATE_READY)
  {
    MenuManager_UnlockDoorTemp_InternalState = MENUMANAGER_UNLOCKDOORTEMP_INTERNALSTATE_RUNNING;
  }
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_UnlockDoorTemp_StopBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_UnlockDoorTemp_InternalState != MENUMANAGER_UNLOCKDOORTEMP_INTERNALSTATE_READY)
  {
    return FSM_GUARD_FALSE;
  }
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_UnlockDoorTemp_UpBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  uint32_t tempUnitVal;
  uint32_t tempVal;

  if (MenuManager_UnlockDoorTemp_InternalState == MENUMANAGER_UNLOCKDOORTEMP_INTERNALSTATE_READY)
  {
    tempUnitVal = MenuManager_UnlockDoorTemp_UnitVal(MenuManager_UnlockDoorTemp_CurPos);

    MenuManager_UnlockDoorTemp_UnitVal(MenuManager_UnlockDoorTemp_CurPos) += (uint32_t)1U;

    if (MenuManager_UnlockDoorTemp_UnitVal(MenuManager_UnlockDoorTemp_CurPos) > (uint32_t)9U)
    {
      MenuManager_UnlockDoorTemp_UnitVal(MenuManager_UnlockDoorTemp_CurPos) = (uint32_t)0U;
    }

    MenuManager_Common_BcdToDecConv(&tempVal, &(MenuManager_UnlockDoorTemp_UnitVal(0)), (uint8_t)MENUMANAGER_UNLOCKDOORTEMP_UNITVAL_LENGTH);

    if ((tempVal < MenuManager_UnlockDoorTemp_ValueMin) || (tempVal >= MenuManager_UnlockDoorTemp_ValueMax))
    {
      MenuManager_UnlockDoorTemp_UnitVal(MenuManager_UnlockDoorTemp_CurPos) = tempUnitVal;
    }

    MenuManager_UnlockDoorTemp_LcdShowAdjust();
  }

  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_UnlockDoorTemp_DownBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  uint32_t tempUnitVal;
  uint32_t tempVal;

  if (MenuManager_UnlockDoorTemp_InternalState == MENUMANAGER_UNLOCKDOORTEMP_INTERNALSTATE_READY)
  {
    tempUnitVal = MenuManager_UnlockDoorTemp_UnitVal(MenuManager_UnlockDoorTemp_CurPos);

    MenuManager_UnlockDoorTemp_UnitVal(MenuManager_UnlockDoorTemp_CurPos) -= (uint32_t)1U;

    if (MenuManager_UnlockDoorTemp_UnitVal(MenuManager_UnlockDoorTemp_CurPos) > (uint32_t)9U)
    {
      MenuManager_UnlockDoorTemp_UnitVal(MenuManager_UnlockDoorTemp_CurPos) = (uint32_t)9U;
    }

    MenuManager_Common_BcdToDecConv(&tempVal, &(MenuManager_UnlockDoorTemp_UnitVal(0)), (uint8_t)MENUMANAGER_UNLOCKDOORTEMP_UNITVAL_LENGTH);

    if ((tempVal < MenuManager_UnlockDoorTemp_ValueMin) || (tempVal >= MenuManager_UnlockDoorTemp_ValueMax))
    {
      MenuManager_UnlockDoorTemp_UnitVal(MenuManager_UnlockDoorTemp_CurPos) = tempUnitVal;
    }

    MenuManager_UnlockDoorTemp_LcdShowAdjust();
  }
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_UnlockDoorTemp_AddBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_UnlockDoorTemp_InternalState == MENUMANAGER_UNLOCKDOORTEMP_INTERNALSTATE_READY)
  {
    if (MenuManager_UnlockDoorTemp_CurPos > (uint32_t)0U)
    {
      MenuManager_UnlockDoorTemp_CurPos -= (uint32_t)1U;
    }

    MenuManager_UnlockDoorTemp_LcdShowAdjust();
  }
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_UnlockDoorTemp_SubBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_UnlockDoorTemp_InternalState == MENUMANAGER_UNLOCKDOORTEMP_INTERNALSTATE_READY)
  {
    if (MenuManager_UnlockDoorTemp_CurPos < (MENUMANAGER_UNLOCKDOORTEMP_UNITVAL_LENGTH - (uint32_t)1U))
    {
      MenuManager_UnlockDoorTemp_CurPos += (uint32_t)1U;
    }
    
    MenuManager_UnlockDoorTemp_LcdShowAdjust();
  }
  
  return FSM_GUARD_TRUE;
}



/*=============================================================================================*/
static void MenuManager_UnlockDoorTemp_SubMainFunction(void)
{
  MenuManager_Common_ButEventMapHandler(&MenuManager_UnlockDoorTemp_ButEventMapConf);

  switch (MenuManager_UnlockDoorTemp_InternalState)
  {
    case MENUMANAGER_UNLOCKDOORTEMP_INTERNALSTATE_READY:
      break;
    case MENUMANAGER_UNLOCKDOORTEMP_INTERNALSTATE_RUNNING:
    {
      uint8_t tempUnlockDoorTemp;

      MenuManager_Common_BcdToDecConv(&(MenuManager_UnlockDoorTemp_Value), &(MenuManager_UnlockDoorTemp_UnitVal(0)), (uint8_t)MENUMANAGER_UNLOCKDOORTEMP_UNITVAL_LENGTH);

      tempUnlockDoorTemp = (uint8_t)MenuManager_UnlockDoorTemp_Value;

      ProgramManager_DoorLockSetup_UnlockDoorTemp_SetData(&tempUnlockDoorTemp, ProgramManager_gParamConfig.machineFuncCfg.tempUnit);

      ProgramManager_DoorLockSetup_UnlockDoorTemp_GetData(&(ProgramManager_gParamConfig.doorLockCfg.unlockDoorTemp), ProgramManager_gParamConfig.machineFuncCfg.tempUnit);

      MenuManager_UnlockDoorTemp_InternalState = MENUMANAGER_UNLOCKDOORTEMP_INTERNALSTATE_DONE;
      
      break;
    }
    case MENUMANAGER_UNLOCKDOORTEMP_INTERNALSTATE_DONE:
    {
      MenuManager_UnlockDoorTemp_LcdShowDone();

      MenuManager_UnlockDoorTemp_InternalState = MENUMANAGER_UNLOCKDOORTEMP_INTERNALSTATE_EXITING;

      break;
    }
    case MENUMANAGER_UNLOCKDOORTEMP_INTERNALSTATE_EXITING:
    default:
      break;
  }
}

/*=============================================================================================*/
static void MenuManager_UnlockDoorTemp_SubTickHandler(void)
{
  if (MenuManager_UnlockDoorTemp_InternalState == MENUMANAGER_UNLOCKDOORTEMP_INTERNALSTATE_EXITING)
  {
    MenuManager_UnlockDoorTemp_Counter += (uint32_t)1U;

    if (MenuManager_UnlockDoorTemp_Counter >= MENUMANAGER_UNLOCKDOORTEMP_COUNTER_MAX)
    {
      MenuManager_UnlockDoorTemp_Counter = (uint32_t)0U;
      
      Fsm_TriggerEvent(&MenuManager_FsmContext, (Fsm_EventType)MENUMANAGER_EVENT_SUBMENU_1);
    }
  }
}



#ifdef  __cplusplus
}
#endif
