/* 
 * File:   _1245000000_menu_TempDiffReheat.c
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
#define MENUMANAGER_TEMPDIFFREHEAT_INTERNALDATALENGTH                 (uint8_t)10U

#define MENUMANAGER_TEMPDIFFREHEAT_INTERNALSTATE_IDX                  0U
#define MENUMANAGER_TEMPDIFFREHEAT_COUNTER_IDX                        1U
#define MENUMANAGER_TEMPDIFFREHEAT_CURPOS_IDX                         2U
#define MENUMANAGER_TEMPDIFFREHEAT_VALUE_MIN_IDX                      3U
#define MENUMANAGER_TEMPDIFFREHEAT_VALUE_MAX_IDX                      4U
#define MENUMANAGER_TEMPDIFFREHEAT_VALUE_IDX                          5U
#define MENUMANAGER_TEMPDIFFREHEAT_UNITVAL_IDX                        6U

#define MenuManager_TempDiffReheat_InternalState                      MenuManager_GetInternalDataPtr(MENUMANAGER_TEMPDIFFREHEAT_INTERNALSTATE_IDX)
#define MenuManager_TempDiffReheat_Counter                            MenuManager_GetInternalDataPtr(MENUMANAGER_TEMPDIFFREHEAT_COUNTER_IDX)
#define MenuManager_TempDiffReheat_CurPos                             MenuManager_GetInternalDataPtr(MENUMANAGER_TEMPDIFFREHEAT_CURPOS_IDX)
#define MenuManager_TempDiffReheat_ValueMin                           MenuManager_GetInternalDataPtr(MENUMANAGER_TEMPDIFFREHEAT_VALUE_MIN_IDX)
#define MenuManager_TempDiffReheat_ValueMax                           MenuManager_GetInternalDataPtr(MENUMANAGER_TEMPDIFFREHEAT_VALUE_MAX_IDX)
#define MenuManager_TempDiffReheat_Value                              MenuManager_GetInternalDataPtr(MENUMANAGER_TEMPDIFFREHEAT_VALUE_IDX)
#define MenuManager_TempDiffReheat_UnitVal(idx)                       MenuManager_GetInternalDataPtr(MENUMANAGER_TEMPDIFFREHEAT_UNITVAL_IDX + idx)



#define MENUMANAGER_TEMPDIFFREHEAT_INTERNALSTATE_READY                (uint32_t)0U
#define MENUMANAGER_TEMPDIFFREHEAT_INTERNALSTATE_RUNNING              (uint32_t)1U
#define MENUMANAGER_TEMPDIFFREHEAT_INTERNALSTATE_DONE                 (uint32_t)2U
#define MENUMANAGER_TEMPDIFFREHEAT_INTERNALSTATE_EXITING              (uint32_t)3U

#define MENUMANAGER_TEMPDIFFREHEAT_COUNTER_MAX                        (uint32_t)50U

#define MENUMANAGER_TEMPDIFFREHEAT_CURPOS_XPOS                        (uint32_t)8U
#define MENUMANAGER_TEMPDIFFREHEAT_CURPOS_YPOS                        (uint32_t)5U

#define MENUMANAGER_TEMPDIFFREHEAT_UNIT_XPOS                          (uint32_t)12U
#define MENUMANAGER_TEMPDIFFREHEAT_UNIT_YPOS                          (uint32_t)5U

#define MENUMANAGER_TEMPDIFFREHEAT_UNITVAL_LENGTH                     (uint32_t)3U



/** Menu manager main titles and child menu titles */
static const uint8_t MenuManager_TempDiffReheat_MainTitleStr[] =      "TEMP DIFF";



/** Menu manager button event mapping array */
static MenuManager_ButEventMapStruct MenuManager_TempDiffReheat_ButEventMap[6] =
{
  { IOMANAGER_BUTTON_STATE_START,                                     MENUMANAGER_EVENT_START_BUT             },
  { IOMANAGER_BUTTON_STATE_STOP,                                      MENUMANAGER_EVENT_STOP_BUT              },
  { IOMANAGER_BUTTON_STATE_UP,                                        MENUMANAGER_EVENT_UP_BUT                },
  { IOMANAGER_BUTTON_STATE_DOWN,                                      MENUMANAGER_EVENT_DOWN_BUT              },
  { IOMANAGER_BUTTON_STATE_ADD,                                       MENUMANAGER_EVENT_ADD_BUT               },
  { IOMANAGER_BUTTON_STATE_SUB,                                       MENUMANAGER_EVENT_SUB_BUT               }
};

/** Menu manager button event mapping configuration */
static MenuManager_ButEventMapConfStruct MenuManager_TempDiffReheat_ButEventMapConf =
{
  .butEventMapNum             = (uint8_t)6U,
  .butEventMapCfg             = &MenuManager_TempDiffReheat_ButEventMap
};



/** Menu manager event handlers */
static Fsm_GuardType MenuManager_TempDiffReheat_Entry                     (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_TempDiffReheat_Exit                      (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_TempDiffReheat_Submenu1                  (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_TempDiffReheat_StartBut                  (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_TempDiffReheat_StopBut                   (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_TempDiffReheat_UpBut                     (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_TempDiffReheat_DownBut                   (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_TempDiffReheat_AddBut                    (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_TempDiffReheat_SubBut                    (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);

/** Menu manager state machine */
Fsm_EventEntryStruct MenuManager_TempDiffReheat_StateMachine[9] =
{
  FSM_TRIGGER_ENTRY             (                                     MenuManager_TempDiffReheat_Entry                                                ),
  FSM_TRIGGER_EXIT              (                                     MenuManager_TempDiffReheat_Exit                                                 ),
  FSM_TRIGGER_TRANSITION_ACTION ( MENUMANAGER_EVENT_SUBMENU_1,        MenuManager_TempDiffReheat_Submenu1,    MENUMANAGER_STATE_HEAT_TEMP_SETUP       ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_START_BUT,        MenuManager_TempDiffReheat_StartBut                                             ),
  FSM_TRIGGER_TRANSITION_ACTION ( MENUMANAGER_EVENT_STOP_BUT,         MenuManager_TempDiffReheat_StopBut,     MENUMANAGER_STATE_HEAT_TEMP_SETUP       ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_UP_BUT,           MenuManager_TempDiffReheat_UpBut                                                ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_DOWN_BUT,         MenuManager_TempDiffReheat_DownBut                                              ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_ADD_BUT,          MenuManager_TempDiffReheat_AddBut                                               ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_SUB_BUT,          MenuManager_TempDiffReheat_SubBut                                               )
};



/*===============================================================================================
*                                   LOCAL FUNCTION PROTOTYPES
===============================================================================================*/

static void MenuManager_TempDiffReheat_LcdShowMainTitle(void);
static void MenuManager_TempDiffReheat_LcdShowAdjust(void);
static void MenuManager_TempDiffReheat_LcdShowDone(void);

static void MenuManager_TempDiffReheat_SubMainFunction(void);
static void MenuManager_TempDiffReheat_SubTickHandler(void);



/*===============================================================================================
*                                       LOCAL FUNCTIONS
===============================================================================================*/

static void MenuManager_TempDiffReheat_LcdShowMainTitle(void)
{
  /* Print main title */
  MenuManager_Common_LcdClearAdjustMenuStatic();
  MenuManager_Common_LcdShowMainTitle(MenuManager_TempDiffReheat_MainTitleStr);
  MenuManager_Common_LcdShowOkExitChoice();
  MenuManager_Common_LcdUpdateAdjustMenuStatic();
}

/*=============================================================================================*/
static void MenuManager_TempDiffReheat_LcdShowAdjust(void)
{
  MenuManager_Common_LcdClearAdjustMenuDynamic();

  MenuManager_Common_LcdShowAdjustUnitVal
  (
    MENUMANAGER_TEMPDIFFREHEAT_CURPOS_XPOS,
    MENUMANAGER_TEMPDIFFREHEAT_CURPOS_YPOS,
    &(MenuManager_TempDiffReheat_UnitVal(0)),
    MENUMANAGER_TEMPDIFFREHEAT_UNITVAL_LENGTH
  );
  
  MenuManager_Common_LcdShowAdjustArrow
  (
    MENUMANAGER_TEMPDIFFREHEAT_CURPOS_XPOS,
    MENUMANAGER_TEMPDIFFREHEAT_CURPOS_YPOS,
    MenuManager_TempDiffReheat_CurPos
  );

  LCD_SetCursorPos(MENUMANAGER_TEMPDIFFREHEAT_UNIT_XPOS, MENUMANAGER_TEMPDIFFREHEAT_UNIT_YPOS, LCD_CURSOR_BY_FONT);
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
static void MenuManager_TempDiffReheat_LcdShowDone(void)
{
  MenuManager_Common_LcdClearAll();

  MenuManager_Common_LcdShowAdjustUnitVal
  (
    MENUMANAGER_TEMPDIFFREHEAT_CURPOS_XPOS,
    MENUMANAGER_TEMPDIFFREHEAT_CURPOS_YPOS,
    &(MenuManager_TempDiffReheat_UnitVal(0)),
    MENUMANAGER_TEMPDIFFREHEAT_UNITVAL_LENGTH
  );

  LCD_SetCursorPos(MENUMANAGER_TEMPDIFFREHEAT_UNIT_XPOS, MENUMANAGER_TEMPDIFFREHEAT_UNIT_YPOS, LCD_CURSOR_BY_FONT);
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
static Fsm_GuardType MenuManager_TempDiffReheat_Entry(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  MenuManager_SubMainFunction = MenuManager_TempDiffReheat_SubMainFunction;
  MenuManager_SubTickHandler = MenuManager_TempDiffReheat_SubTickHandler;

  /* Check if previous state data hierachy is not empty */
  if (pFsmContext->dataHierachy != NULL)
  {
    if (pFsmContext->dataHierachy->dataId == MENUMANAGER_STATE_HEAT_TEMP_SETUP)
    {
      /* Release previous state data hierachy */
      MenuManager_free(pFsmContext->dataHierachy);
      pFsmContext->dataHierachy = NULL;

      MenuManager_InternalDataPush(MENUMANAGER_TEMPDIFFREHEAT_INTERNALDATALENGTH);

      MenuManager_TempDiffReheat_InternalState = MENUMANAGER_TEMPDIFFREHEAT_INTERNALSTATE_READY;
      MenuManager_TempDiffReheat_Counter = (uint32_t)0U;
      MenuManager_TempDiffReheat_CurPos = (uint32_t)0U;

      if (ProgramManager_gParamConfig.machineFuncCfg.tempUnit == PROGRAMMANAGER_TEMP_UNIT_CELSIUS)
      {
        MenuManager_TempDiffReheat_ValueMin = (uint32_t)PROGRAMMANAGER_HEATTEMPSETUP_WATER_TEMP_C_MIN;
        MenuManager_TempDiffReheat_ValueMax = (uint32_t)PROGRAMMANAGER_HEATTEMPSETUP_WATER_TEMP_C_MAX;
      }
      else
      {
        MenuManager_TempDiffReheat_ValueMin = (uint32_t)PROGRAMMANAGER_HEATTEMPSETUP_WATER_TEMP_F_MIN;
        MenuManager_TempDiffReheat_ValueMax = (uint32_t)PROGRAMMANAGER_HEATTEMPSETUP_WATER_TEMP_F_MAX;
      }

      MenuManager_TempDiffReheat_Value = (uint32_t)(ProgramManager_gParamConfig.heatTempCfg.tempDiffReheat);

      MenuManager_Common_DecToBcdConv
      (
        &(MenuManager_TempDiffReheat_Value),
        &(MenuManager_TempDiffReheat_UnitVal(0)),
        (uint8_t)MENUMANAGER_TEMPDIFFREHEAT_UNITVAL_LENGTH
      );
    }
    else
    {
      return FSM_GUARD_FALSE;
    }

    MenuManager_TempDiffReheat_LcdShowMainTitle();
    MenuManager_TempDiffReheat_LcdShowAdjust();

    return FSM_GUARD_TRUE;
  }

  return FSM_GUARD_FALSE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_TempDiffReheat_Exit(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  MenuManager_SubMainFunction = NULL;
  MenuManager_SubTickHandler = NULL;

  Fsm_DataHierachyStruct* dataHierachy;

  dataHierachy = (Fsm_DataHierachyStruct *)MenuManager_malloc(sizeof(Fsm_DataHierachyStruct));
  dataHierachy->dataId = MENUMANAGER_STATE_MAX_WATER_TEMP;

  pFsmContext->dataHierachy = dataHierachy;

  /* Free internal data */
  MenuManager_InternalDataPop();
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_TempDiffReheat_Submenu1(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_TempDiffReheat_StartBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_TempDiffReheat_InternalState == MENUMANAGER_TEMPDIFFREHEAT_INTERNALSTATE_READY)
  {
    MenuManager_TempDiffReheat_InternalState = MENUMANAGER_TEMPDIFFREHEAT_INTERNALSTATE_RUNNING;
  }
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_TempDiffReheat_StopBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_TempDiffReheat_InternalState != MENUMANAGER_TEMPDIFFREHEAT_INTERNALSTATE_READY)
  {
    return FSM_GUARD_FALSE;
  }
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_TempDiffReheat_UpBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  uint32_t tempUnitVal;
  uint32_t tempVal;

  if (MenuManager_TempDiffReheat_InternalState == MENUMANAGER_TEMPDIFFREHEAT_INTERNALSTATE_READY)
  {
    tempUnitVal = MenuManager_TempDiffReheat_UnitVal(MenuManager_TempDiffReheat_CurPos);

    MenuManager_TempDiffReheat_UnitVal(MenuManager_TempDiffReheat_CurPos) += (uint32_t)1U;

    if (MenuManager_TempDiffReheat_UnitVal(MenuManager_TempDiffReheat_CurPos) > (uint32_t)9U)
    {
      MenuManager_TempDiffReheat_UnitVal(MenuManager_TempDiffReheat_CurPos) = (uint32_t)0U;
    }

    MenuManager_Common_BcdToDecConv(&tempVal, &(MenuManager_TempDiffReheat_UnitVal(0)), (uint8_t)MENUMANAGER_TEMPDIFFREHEAT_UNITVAL_LENGTH);

    if ((tempVal <= MenuManager_TempDiffReheat_ValueMin) || (tempVal >= MenuManager_TempDiffReheat_ValueMax))
    {
      MenuManager_TempDiffReheat_UnitVal(MenuManager_TempDiffReheat_CurPos) = tempUnitVal;
    }

    MenuManager_TempDiffReheat_LcdShowAdjust();
  }

  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_TempDiffReheat_DownBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  uint32_t tempUnitVal;
  uint32_t tempVal;

  if (MenuManager_TempDiffReheat_InternalState == MENUMANAGER_TEMPDIFFREHEAT_INTERNALSTATE_READY)
  {
    tempUnitVal = MenuManager_TempDiffReheat_UnitVal(MenuManager_TempDiffReheat_CurPos);

    MenuManager_TempDiffReheat_UnitVal(MenuManager_TempDiffReheat_CurPos) -= (uint32_t)1U;

    if (MenuManager_TempDiffReheat_UnitVal(MenuManager_TempDiffReheat_CurPos) > (uint32_t)9U)
    {
      MenuManager_TempDiffReheat_UnitVal(MenuManager_TempDiffReheat_CurPos) = (uint32_t)9U;
    }

    MenuManager_Common_BcdToDecConv(&tempVal, &(MenuManager_TempDiffReheat_UnitVal(0)), (uint8_t)MENUMANAGER_TEMPDIFFREHEAT_UNITVAL_LENGTH);

    if ((tempVal <= MenuManager_TempDiffReheat_ValueMin) || (tempVal >= MenuManager_TempDiffReheat_ValueMax))
    {
      MenuManager_TempDiffReheat_UnitVal(MenuManager_TempDiffReheat_CurPos) = tempUnitVal;
    }

    MenuManager_TempDiffReheat_LcdShowAdjust();
  }
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_TempDiffReheat_AddBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_TempDiffReheat_InternalState == MENUMANAGER_TEMPDIFFREHEAT_INTERNALSTATE_READY)
  {
    if (MenuManager_TempDiffReheat_CurPos > (uint32_t)0U)
    {
      MenuManager_TempDiffReheat_CurPos -= (uint32_t)1U;
    }

    MenuManager_TempDiffReheat_LcdShowAdjust();
  }
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_TempDiffReheat_SubBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_TempDiffReheat_InternalState == MENUMANAGER_TEMPDIFFREHEAT_INTERNALSTATE_READY)
  {
    if (MenuManager_TempDiffReheat_CurPos < (MENUMANAGER_TEMPDIFFREHEAT_UNITVAL_LENGTH - (uint32_t)1U))
    {
      MenuManager_TempDiffReheat_CurPos += (uint32_t)1U;
    }
    
    MenuManager_TempDiffReheat_LcdShowAdjust();
  }
  
  return FSM_GUARD_TRUE;
}



/*=============================================================================================*/
static void MenuManager_TempDiffReheat_SubMainFunction(void)
{
  MenuManager_Common_ButEventMapHandler(&MenuManager_TempDiffReheat_ButEventMapConf);

  switch (MenuManager_TempDiffReheat_InternalState)
  {
    case MENUMANAGER_TEMPDIFFREHEAT_INTERNALSTATE_READY:
      break;
    case MENUMANAGER_TEMPDIFFREHEAT_INTERNALSTATE_RUNNING:
    {
      uint8_t tempTempDiffReheat;

      MenuManager_Common_BcdToDecConv(&(MenuManager_TempDiffReheat_Value), &(MenuManager_TempDiffReheat_UnitVal(0)), (uint8_t)MENUMANAGER_TEMPDIFFREHEAT_UNITVAL_LENGTH);

      tempTempDiffReheat = (uint8_t)MenuManager_TempDiffReheat_Value;

      ProgramManager_HeatTempSetup_TempDiffReheat_SetData(&tempTempDiffReheat, ProgramManager_gParamConfig.machineFuncCfg.tempUnit);

      ProgramManager_HeatTempSetup_TempDiffReheat_GetData(&(ProgramManager_gParamConfig.heatTempCfg.tempDiffReheat), ProgramManager_gParamConfig.machineFuncCfg.tempUnit);

      MenuManager_TempDiffReheat_InternalState = MENUMANAGER_TEMPDIFFREHEAT_INTERNALSTATE_DONE;
      
      break;
    }
    case MENUMANAGER_TEMPDIFFREHEAT_INTERNALSTATE_DONE:
    {
      MenuManager_TempDiffReheat_LcdShowDone();

      MenuManager_TempDiffReheat_InternalState = MENUMANAGER_TEMPDIFFREHEAT_INTERNALSTATE_EXITING;

      break;
    }
    case MENUMANAGER_TEMPDIFFREHEAT_INTERNALSTATE_EXITING:
    default:
      break;
  }
}

/*=============================================================================================*/
static void MenuManager_TempDiffReheat_SubTickHandler(void)
{
  if (MenuManager_TempDiffReheat_InternalState == MENUMANAGER_TEMPDIFFREHEAT_INTERNALSTATE_EXITING)
  {
    MenuManager_TempDiffReheat_Counter += (uint32_t)1U;

    if (MenuManager_TempDiffReheat_Counter >= MENUMANAGER_TEMPDIFFREHEAT_COUNTER_MAX)
    {
      MenuManager_TempDiffReheat_Counter = (uint32_t)0U;
      
      Fsm_TriggerEvent(&MenuManager_FsmContext, (Fsm_EventType)MENUMANAGER_EVENT_SUBMENU_1);
    }
  }
}



#ifdef  __cplusplus
}
#endif
