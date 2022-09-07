/* 
 * File:   _1238000000_menu_MaxTimeFill.c
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
#define MENUMANAGER_MAXTIMEFILL_INTERNALDATALENGTH                    (uint8_t)9U

#define MENUMANAGER_MAXTIMEFILL_INTERNALSTATE_IDX                     0U
#define MENUMANAGER_MAXTIMEFILL_COUNTER_IDX                           1U
#define MENUMANAGER_MAXTIMEFILL_CURPOS_IDX                            2U
#define MENUMANAGER_MAXTIMEFILL_VALUE_MIN_IDX                         3U
#define MENUMANAGER_MAXTIMEFILL_VALUE_MAX_IDX                         4U
#define MENUMANAGER_MAXTIMEFILL_VALUE_IDX                             5U
#define MENUMANAGER_MAXTIMEFILL_UNITVAL_IDX                           6U

#define MenuManager_MaxTimeFill_InternalState                         MenuManager_GetInternalDataPtr(MENUMANAGER_MAXTIMEFILL_INTERNALSTATE_IDX)
#define MenuManager_MaxTimeFill_Counter                               MenuManager_GetInternalDataPtr(MENUMANAGER_MAXTIMEFILL_COUNTER_IDX)
#define MenuManager_MaxTimeFill_CurPos                                MenuManager_GetInternalDataPtr(MENUMANAGER_MAXTIMEFILL_CURPOS_IDX)
#define MenuManager_MaxTimeFill_ValueMin                              MenuManager_GetInternalDataPtr(MENUMANAGER_MAXTIMEFILL_VALUE_MIN_IDX)
#define MenuManager_MaxTimeFill_ValueMax                              MenuManager_GetInternalDataPtr(MENUMANAGER_MAXTIMEFILL_VALUE_MAX_IDX)
#define MenuManager_MaxTimeFill_Value                                 MenuManager_GetInternalDataPtr(MENUMANAGER_MAXTIMEFILL_VALUE_IDX)
#define MenuManager_MaxTimeFill_UnitVal(idx)                          MenuManager_GetInternalDataPtr(MENUMANAGER_MAXTIMEFILL_UNITVAL_IDX + idx)



#define MENUMANAGER_MAXTIMEFILL_INTERNALSTATE_READY                   (uint32_t)0U
#define MENUMANAGER_MAXTIMEFILL_INTERNALSTATE_RUNNING                 (uint32_t)1U
#define MENUMANAGER_MAXTIMEFILL_INTERNALSTATE_DONE                    (uint32_t)2U
#define MENUMANAGER_MAXTIMEFILL_INTERNALSTATE_EXITING                 (uint32_t)3U

#define MENUMANAGER_MAXTIMEFILL_COUNTER_MAX                           (uint32_t)50U

#define MENUMANAGER_MAXTIMEFILL_CURPOS_XPOS                           (uint32_t)5U
#define MENUMANAGER_MAXTIMEFILL_CURPOS_YPOS                           (uint32_t)5U

#define MENUMANAGER_MAXTIMEFILL_UNIT_XPOS                             (uint32_t)9U
#define MENUMANAGER_MAXTIMEFILL_UNIT_YPOS                             (uint32_t)5U

#define MENUMANAGER_MAXTIMEFILL_UNITVAL_LENGTH                        (uint32_t)3U



/** Menu manager main titles and child menu titles */
static const uint8_t MenuManager_MaxTimeFill_MainTitleStr[] =         "MAX TIME FILL";



/** Menu manager button event mapping array */
static MenuManager_ButEventMapStruct MenuManager_MaxTimeFill_ButEventMap[6] =
{
  { IOMANAGER_BUTTON_STATE_START,                                     MENUMANAGER_EVENT_START_BUT             },
  { IOMANAGER_BUTTON_STATE_STOP,                                      MENUMANAGER_EVENT_STOP_BUT              },
  { IOMANAGER_BUTTON_STATE_UP,                                        MENUMANAGER_EVENT_UP_BUT                },
  { IOMANAGER_BUTTON_STATE_DOWN,                                      MENUMANAGER_EVENT_DOWN_BUT              },
  { IOMANAGER_BUTTON_STATE_ADD,                                       MENUMANAGER_EVENT_ADD_BUT               },
  { IOMANAGER_BUTTON_STATE_SUB,                                       MENUMANAGER_EVENT_SUB_BUT               }
};

/** Menu manager button event mapping configuration */
static MenuManager_ButEventMapConfStruct MenuManager_MaxTimeFill_ButEventMapConf =
{
  .butEventMapNum             = (uint8_t)6U,
  .butEventMapCfg             = &MenuManager_MaxTimeFill_ButEventMap
};



/** Menu manager event handlers */
static Fsm_GuardType MenuManager_MaxTimeFill_Entry                        (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_MaxTimeFill_Exit                         (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_MaxTimeFill_Submenu1                     (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_MaxTimeFill_StartBut                     (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_MaxTimeFill_StopBut                      (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_MaxTimeFill_UpBut                        (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_MaxTimeFill_DownBut                      (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_MaxTimeFill_AddBut                       (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_MaxTimeFill_SubBut                       (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);

/** Menu manager state machine */
Fsm_EventEntryStruct MenuManager_MaxTimeFill_StateMachine[9] =
{
  FSM_TRIGGER_ENTRY             (                                     MenuManager_MaxTimeFill_Entry                                                   ),
  FSM_TRIGGER_EXIT              (                                     MenuManager_MaxTimeFill_Exit                                                    ),
  FSM_TRIGGER_TRANSITION_ACTION ( MENUMANAGER_EVENT_SUBMENU_1,        MenuManager_MaxTimeFill_Submenu1,       MENUMANAGER_STATE_FILL_LEVEL_SETUP      ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_START_BUT,        MenuManager_MaxTimeFill_StartBut                                                ),
  FSM_TRIGGER_TRANSITION_ACTION ( MENUMANAGER_EVENT_STOP_BUT,         MenuManager_MaxTimeFill_StopBut,        MENUMANAGER_STATE_FILL_LEVEL_SETUP      ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_UP_BUT,           MenuManager_MaxTimeFill_UpBut                                                   ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_DOWN_BUT,         MenuManager_MaxTimeFill_DownBut                                                 ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_ADD_BUT,          MenuManager_MaxTimeFill_AddBut                                                  ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_SUB_BUT,          MenuManager_MaxTimeFill_SubBut                                                  )
};



/*===============================================================================================
*                                   LOCAL FUNCTION PROTOTYPES
===============================================================================================*/

static void MenuManager_MaxTimeFill_LcdShowMainTitle(void);
static void MenuManager_MaxTimeFill_LcdShowAdjust(void);
static void MenuManager_MaxTimeFill_LcdShowDone(void);

static void MenuManager_MaxTimeFill_SubMainFunction(void);
static void MenuManager_MaxTimeFill_SubTickHandler(void);



/*===============================================================================================
*                                       LOCAL FUNCTIONS
===============================================================================================*/

static void MenuManager_MaxTimeFill_LcdShowMainTitle(void)
{
  /* Print main title */
  MenuManager_Common_LcdClearAdjustMenuStatic();
  MenuManager_Common_LcdShowMainTitle(MenuManager_MaxTimeFill_MainTitleStr);
  MenuManager_Common_LcdShowOkExitChoice();
  MenuManager_Common_LcdUpdateAdjustMenuStatic();
}

/*=============================================================================================*/
static void MenuManager_MaxTimeFill_LcdShowAdjust(void)
{
  MenuManager_Common_LcdClearAdjustMenuDynamic();

  MenuManager_Common_LcdShowAdjustUnitVal
  (
    MENUMANAGER_MAXTIMEFILL_CURPOS_XPOS,
    MENUMANAGER_MAXTIMEFILL_CURPOS_YPOS,
    &(MenuManager_MaxTimeFill_UnitVal(0)),
    MENUMANAGER_MAXTIMEFILL_UNITVAL_LENGTH
  );
  
  MenuManager_Common_LcdShowAdjustArrow
  (
    MENUMANAGER_MAXTIMEFILL_CURPOS_XPOS,
    MENUMANAGER_MAXTIMEFILL_CURPOS_YPOS,
    MenuManager_MaxTimeFill_CurPos
  );

  LCD_SetCursorPos(MENUMANAGER_MAXTIMEFILL_UNIT_XPOS, MENUMANAGER_MAXTIMEFILL_UNIT_YPOS, LCD_CURSOR_BY_FONT);
  LCD_PutString((uint8_t *)MenuManager_Common_MinuteStr);

  MenuManager_Common_LcdUpdateAdjustMenuDynamic();
}

/*=============================================================================================*/
static void MenuManager_MaxTimeFill_LcdShowDone(void)
{
  MenuManager_Common_LcdClearAll();

  MenuManager_Common_LcdShowAdjustUnitVal
  (
    MENUMANAGER_MAXTIMEFILL_CURPOS_XPOS,
    MENUMANAGER_MAXTIMEFILL_CURPOS_YPOS,
    &(MenuManager_MaxTimeFill_UnitVal(0)),
    MENUMANAGER_MAXTIMEFILL_UNITVAL_LENGTH
  );

  LCD_SetCursorPos(MENUMANAGER_MAXTIMEFILL_UNIT_XPOS, MENUMANAGER_MAXTIMEFILL_UNIT_YPOS, LCD_CURSOR_BY_FONT);
  LCD_PutString((uint8_t *)MenuManager_Common_MinuteStr);

  MenuManager_Common_LcdUpdateAll();
}



/*=============================================================================================*/
static Fsm_GuardType MenuManager_MaxTimeFill_Entry(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  MenuManager_SubMainFunction = MenuManager_MaxTimeFill_SubMainFunction;
  MenuManager_SubTickHandler = MenuManager_MaxTimeFill_SubTickHandler;

  /* Check if previous state data hierachy is not empty */
  if (pFsmContext->dataHierachy != NULL)
  {
    if (pFsmContext->dataHierachy->dataId == MENUMANAGER_STATE_FILL_LEVEL_SETUP)
    {
      /* Release previous state data hierachy */
      MenuManager_free(pFsmContext->dataHierachy);
      pFsmContext->dataHierachy = NULL;

      MenuManager_InternalDataPush(MENUMANAGER_MAXTIMEFILL_INTERNALDATALENGTH);

      MenuManager_MaxTimeFill_InternalState = MENUMANAGER_MAXTIMEFILL_INTERNALSTATE_READY;
      MenuManager_MaxTimeFill_Counter = (uint32_t)0U;
      MenuManager_MaxTimeFill_CurPos = (uint32_t)0U;

      MenuManager_MaxTimeFill_ValueMin = (uint32_t)PROGRAMMANAGER_FILLLEVELSETUP_MAXTIMEFILL_MIN;
      MenuManager_MaxTimeFill_ValueMax = (uint32_t)PROGRAMMANAGER_FILLLEVELSETUP_MAXTIMEFILL_MAX;

      MenuManager_MaxTimeFill_Value = (uint32_t)(ProgramManager_gParamConfig.fillLevelCfg.maxTimeFill);

      MenuManager_Common_DecToBcdConv
      (
        &(MenuManager_MaxTimeFill_Value),
        &(MenuManager_MaxTimeFill_UnitVal(0)),
        (uint8_t)MENUMANAGER_MAXTIMEFILL_UNITVAL_LENGTH
      );
    }
    else
    {
      return FSM_GUARD_FALSE;
    }

    MenuManager_MaxTimeFill_LcdShowMainTitle();
    MenuManager_MaxTimeFill_LcdShowAdjust();

    return FSM_GUARD_TRUE;
  }

  return FSM_GUARD_FALSE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_MaxTimeFill_Exit(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  MenuManager_SubMainFunction = NULL;
  MenuManager_SubTickHandler = NULL;

  Fsm_DataHierachyStruct* dataHierachy;

  dataHierachy = (Fsm_DataHierachyStruct *)MenuManager_malloc(sizeof(Fsm_DataHierachyStruct));
  dataHierachy->dataId = MENUMANAGER_STATE_MAX_TIME_REFILL;

  pFsmContext->dataHierachy = dataHierachy;

  /* Free internal data */
  MenuManager_InternalDataPop();
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_MaxTimeFill_Submenu1(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_MaxTimeFill_StartBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_MaxTimeFill_InternalState == MENUMANAGER_MAXTIMEFILL_INTERNALSTATE_READY)
  {
    MenuManager_MaxTimeFill_InternalState = MENUMANAGER_MAXTIMEFILL_INTERNALSTATE_RUNNING;
  }
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_MaxTimeFill_StopBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_MaxTimeFill_InternalState != MENUMANAGER_MAXTIMEFILL_INTERNALSTATE_READY)
  {
    return FSM_GUARD_FALSE;
  }
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_MaxTimeFill_UpBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  uint32_t tempUnitVal;
  uint32_t tempVal;

  if (MenuManager_MaxTimeFill_InternalState == MENUMANAGER_MAXTIMEFILL_INTERNALSTATE_READY)
  {
    tempUnitVal = MenuManager_MaxTimeFill_UnitVal(MenuManager_MaxTimeFill_CurPos);

    MenuManager_MaxTimeFill_UnitVal(MenuManager_MaxTimeFill_CurPos) += (uint32_t)1U;

    if (MenuManager_MaxTimeFill_UnitVal(MenuManager_MaxTimeFill_CurPos) > (uint32_t)9U)
    {
      MenuManager_MaxTimeFill_UnitVal(MenuManager_MaxTimeFill_CurPos) = (uint32_t)0U;
    }

    MenuManager_Common_BcdToDecConv(&tempVal, &(MenuManager_MaxTimeFill_UnitVal(0)), (uint8_t)MENUMANAGER_MAXTIMEFILL_UNITVAL_LENGTH);

    if ((tempVal <= MenuManager_MaxTimeFill_ValueMin) || (tempVal >= MenuManager_MaxTimeFill_ValueMax))
    {
      MenuManager_MaxTimeFill_UnitVal(MenuManager_MaxTimeFill_CurPos) = tempUnitVal;
    }

    MenuManager_MaxTimeFill_LcdShowAdjust();
  }

  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_MaxTimeFill_DownBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  uint32_t tempUnitVal;
  uint32_t tempVal;

  if (MenuManager_MaxTimeFill_InternalState == MENUMANAGER_MAXTIMEFILL_INTERNALSTATE_READY)
  {
    tempUnitVal = MenuManager_MaxTimeFill_UnitVal(MenuManager_MaxTimeFill_CurPos);

    MenuManager_MaxTimeFill_UnitVal(MenuManager_MaxTimeFill_CurPos) -= (uint32_t)1U;

    if (MenuManager_MaxTimeFill_UnitVal(MenuManager_MaxTimeFill_CurPos) > (uint32_t)9U)
    {
      MenuManager_MaxTimeFill_UnitVal(MenuManager_MaxTimeFill_CurPos) = (uint32_t)9U;
    }

    MenuManager_Common_BcdToDecConv(&tempVal, &(MenuManager_MaxTimeFill_UnitVal(0)), (uint8_t)MENUMANAGER_MAXTIMEFILL_UNITVAL_LENGTH);

    if ((tempVal <= MenuManager_MaxTimeFill_ValueMin) || (tempVal >= MenuManager_MaxTimeFill_ValueMax))
    {
      MenuManager_MaxTimeFill_UnitVal(MenuManager_MaxTimeFill_CurPos) = tempUnitVal;
    }

    MenuManager_MaxTimeFill_LcdShowAdjust();
  }
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_MaxTimeFill_AddBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_MaxTimeFill_InternalState == MENUMANAGER_MAXTIMEFILL_INTERNALSTATE_READY)
  {
    if (MenuManager_MaxTimeFill_CurPos > (uint32_t)0U)
    {
      MenuManager_MaxTimeFill_CurPos -= (uint32_t)1U;
    }

    MenuManager_MaxTimeFill_LcdShowAdjust();
  }
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_MaxTimeFill_SubBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_MaxTimeFill_InternalState == MENUMANAGER_MAXTIMEFILL_INTERNALSTATE_READY)
  {
    if (MenuManager_MaxTimeFill_CurPos < (MENUMANAGER_MAXTIMEFILL_UNITVAL_LENGTH - (uint32_t)1U))
    {
      MenuManager_MaxTimeFill_CurPos += (uint32_t)1U;
    }
    
    MenuManager_MaxTimeFill_LcdShowAdjust();
  }
  
  return FSM_GUARD_TRUE;
}



/*=============================================================================================*/
static void MenuManager_MaxTimeFill_SubMainFunction(void)
{
  MenuManager_Common_ButEventMapHandler(&MenuManager_MaxTimeFill_ButEventMapConf);

  switch (MenuManager_MaxTimeFill_InternalState)
  {
    case MENUMANAGER_MAXTIMEFILL_INTERNALSTATE_READY:
      break;
    case MENUMANAGER_MAXTIMEFILL_INTERNALSTATE_RUNNING:
    {
      uint16_t tempMaxTimeFill;

      MenuManager_Common_BcdToDecConv(&(MenuManager_MaxTimeFill_Value), &(MenuManager_MaxTimeFill_UnitVal(0)), (uint8_t)MENUMANAGER_MAXTIMEFILL_UNITVAL_LENGTH);

      tempMaxTimeFill = (uint16_t)MenuManager_MaxTimeFill_Value;

      ProgramManager_FillLevelSetup_MaxTimeFill_SetData(&tempMaxTimeFill);

      ProgramManager_FillLevelSetup_MaxTimeFill_GetData(&(ProgramManager_gParamConfig.fillLevelCfg.maxTimeFill));

      MenuManager_MaxTimeFill_InternalState = MENUMANAGER_MAXTIMEFILL_INTERNALSTATE_DONE;
      
      break;
    }
    case MENUMANAGER_MAXTIMEFILL_INTERNALSTATE_DONE:
    {
      MenuManager_MaxTimeFill_LcdShowDone();

      MenuManager_MaxTimeFill_InternalState = MENUMANAGER_MAXTIMEFILL_INTERNALSTATE_EXITING;

      break;
    }
    case MENUMANAGER_MAXTIMEFILL_INTERNALSTATE_EXITING:
    default:
      break;
  }
}

/*=============================================================================================*/
static void MenuManager_MaxTimeFill_SubTickHandler(void)
{
  if (MenuManager_MaxTimeFill_InternalState == MENUMANAGER_MAXTIMEFILL_INTERNALSTATE_EXITING)
  {
    MenuManager_MaxTimeFill_Counter += (uint32_t)1U;

    if (MenuManager_MaxTimeFill_Counter >= MENUMANAGER_MAXTIMEFILL_COUNTER_MAX)
    {
      MenuManager_MaxTimeFill_Counter = (uint32_t)0U;
      
      Fsm_TriggerEvent(&MenuManager_FsmContext, (Fsm_EventType)MENUMANAGER_EVENT_SUBMENU_1);
    }
  }
}



#ifdef  __cplusplus
}
#endif
