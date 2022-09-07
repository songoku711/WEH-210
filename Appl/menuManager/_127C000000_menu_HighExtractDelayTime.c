/* 
 * File:   _127C000000_menu_HighExtractDelayTime.c
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
#define MENUMANAGER_HIGHEXTRACTDELAYTIME_INTERNALDATALENGTH           (uint8_t)10U

#define MENUMANAGER_HIGHEXTRACTDELAYTIME_INTERNALSTATE_IDX            0U
#define MENUMANAGER_HIGHEXTRACTDELAYTIME_COUNTER_IDX                  1U
#define MENUMANAGER_HIGHEXTRACTDELAYTIME_CURPOS_IDX                   2U
#define MENUMANAGER_HIGHEXTRACTDELAYTIME_VALUE_MIN_IDX                3U
#define MENUMANAGER_HIGHEXTRACTDELAYTIME_VALUE_MAX_IDX                4U
#define MENUMANAGER_HIGHEXTRACTDELAYTIME_VALUE_IDX                    5U
#define MENUMANAGER_HIGHEXTRACTDELAYTIME_UNITVAL_IDX                  6U

#define MenuManager_HighExtractDelayTime_InternalState                MenuManager_GetInternalDataPtr(MENUMANAGER_HIGHEXTRACTDELAYTIME_INTERNALSTATE_IDX)
#define MenuManager_HighExtractDelayTime_Counter                      MenuManager_GetInternalDataPtr(MENUMANAGER_HIGHEXTRACTDELAYTIME_COUNTER_IDX)
#define MenuManager_HighExtractDelayTime_CurPos                       MenuManager_GetInternalDataPtr(MENUMANAGER_HIGHEXTRACTDELAYTIME_CURPOS_IDX)
#define MenuManager_HighExtractDelayTime_ValueMin                     MenuManager_GetInternalDataPtr(MENUMANAGER_HIGHEXTRACTDELAYTIME_VALUE_MIN_IDX)
#define MenuManager_HighExtractDelayTime_ValueMax                     MenuManager_GetInternalDataPtr(MENUMANAGER_HIGHEXTRACTDELAYTIME_VALUE_MAX_IDX)
#define MenuManager_HighExtractDelayTime_Value                        MenuManager_GetInternalDataPtr(MENUMANAGER_HIGHEXTRACTDELAYTIME_VALUE_IDX)
#define MenuManager_HighExtractDelayTime_UnitVal(idx)                 MenuManager_GetInternalDataPtr(MENUMANAGER_HIGHEXTRACTDELAYTIME_UNITVAL_IDX + idx)



#define MENUMANAGER_HIGHEXTRACTDELAYTIME_INTERNALSTATE_READY          (uint32_t)0U
#define MENUMANAGER_HIGHEXTRACTDELAYTIME_INTERNALSTATE_RUNNING        (uint32_t)1U
#define MENUMANAGER_HIGHEXTRACTDELAYTIME_INTERNALSTATE_DONE           (uint32_t)2U
#define MENUMANAGER_HIGHEXTRACTDELAYTIME_INTERNALSTATE_EXITING        (uint32_t)3U

#define MENUMANAGER_HIGHEXTRACTDELAYTIME_COUNTER_MAX                  (uint32_t)50U

#define MENUMANAGER_HIGHEXTRACTDELAYTIME_CURPOS_XPOS                  (uint32_t)5U
#define MENUMANAGER_HIGHEXTRACTDELAYTIME_CURPOS_YPOS                  (uint32_t)5U

#define MENUMANAGER_HIGHEXTRACTDELAYTIME_UNIT_XPOS                    (uint32_t)9U
#define MENUMANAGER_HIGHEXTRACTDELAYTIME_UNIT_YPOS                    (uint32_t)5U

#define MENUMANAGER_HIGHEXTRACTDELAYTIME_UNITVAL_LENGTH               (uint32_t)3U



/** Menu manager main titles and child menu titles */
static const uint8_t MenuManager_HighExtractDelayTime_MainTitleStr[] = "HIGH EXT DELAY";



/** Menu manager button event mapping array */
static MenuManager_ButEventMapStruct MenuManager_HighExtractDelayTime_ButEventMap[6] =
{
  { IOMANAGER_BUTTON_STATE_START,                                     MENUMANAGER_EVENT_START_BUT             },
  { IOMANAGER_BUTTON_STATE_STOP,                                      MENUMANAGER_EVENT_STOP_BUT              },
  { IOMANAGER_BUTTON_STATE_UP,                                        MENUMANAGER_EVENT_UP_BUT                },
  { IOMANAGER_BUTTON_STATE_DOWN,                                      MENUMANAGER_EVENT_DOWN_BUT              },
  { IOMANAGER_BUTTON_STATE_ADD,                                       MENUMANAGER_EVENT_ADD_BUT               },
  { IOMANAGER_BUTTON_STATE_SUB,                                       MENUMANAGER_EVENT_SUB_BUT               }
};

/** Menu manager button event mapping configuration */
static MenuManager_ButEventMapConfStruct MenuManager_HighExtractDelayTime_ButEventMapConf =
{
  .butEventMapNum             = (uint8_t)6U,
  .butEventMapCfg             = &MenuManager_HighExtractDelayTime_ButEventMap
};



/** Menu manager event handlers */
static Fsm_GuardType MenuManager_HighExtractDelayTime_Entry               (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_HighExtractDelayTime_Exit                (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_HighExtractDelayTime_Submenu1            (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_HighExtractDelayTime_StartBut            (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_HighExtractDelayTime_StopBut             (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_HighExtractDelayTime_UpBut               (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_HighExtractDelayTime_DownBut             (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_HighExtractDelayTime_AddBut              (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_HighExtractDelayTime_SubBut              (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);

/** Menu manager state machine */
Fsm_EventEntryStruct MenuManager_HighExtractDelayTime_StateMachine[9] =
{
  FSM_TRIGGER_ENTRY             (                                     MenuManager_HighExtractDelayTime_Entry                                          ),
  FSM_TRIGGER_EXIT              (                                     MenuManager_HighExtractDelayTime_Exit                                           ),
  FSM_TRIGGER_TRANSITION_ACTION ( MENUMANAGER_EVENT_SUBMENU_1,        MenuManager_HighExtractDelayTime_Submenu1, MENUMANAGER_STATE_EXTRACT_SETUP      ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_START_BUT,        MenuManager_HighExtractDelayTime_StartBut                                       ),
  FSM_TRIGGER_TRANSITION_ACTION ( MENUMANAGER_EVENT_STOP_BUT,         MenuManager_HighExtractDelayTime_StopBut,  MENUMANAGER_STATE_EXTRACT_SETUP      ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_UP_BUT,           MenuManager_HighExtractDelayTime_UpBut                                          ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_DOWN_BUT,         MenuManager_HighExtractDelayTime_DownBut                                        ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_ADD_BUT,          MenuManager_HighExtractDelayTime_AddBut                                         ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_SUB_BUT,          MenuManager_HighExtractDelayTime_SubBut                                         )
};



/*===============================================================================================
*                                   LOCAL FUNCTION PROTOTYPES
===============================================================================================*/

static void MenuManager_HighExtractDelayTime_LcdShowMainTitle(void);
static void MenuManager_HighExtractDelayTime_LcdShowAdjust(void);
static void MenuManager_HighExtractDelayTime_LcdShowDone(void);

static void MenuManager_HighExtractDelayTime_SubMainFunction(void);
static void MenuManager_HighExtractDelayTime_SubTickHandler(void);



/*===============================================================================================
*                                       LOCAL FUNCTIONS
===============================================================================================*/

static void MenuManager_HighExtractDelayTime_LcdShowMainTitle(void)
{
  /* Print main title */
  MenuManager_Common_LcdClearAdjustMenuStatic();
  MenuManager_Common_LcdShowMainTitle(MenuManager_HighExtractDelayTime_MainTitleStr);
  MenuManager_Common_LcdShowOkExitChoice();
  MenuManager_Common_LcdUpdateAdjustMenuStatic();
}

/*=============================================================================================*/
static void MenuManager_HighExtractDelayTime_LcdShowAdjust(void)
{
  MenuManager_Common_LcdClearAdjustMenuDynamic();

  MenuManager_Common_LcdShowAdjustUnitVal
  (
    MENUMANAGER_HIGHEXTRACTDELAYTIME_CURPOS_XPOS,
    MENUMANAGER_HIGHEXTRACTDELAYTIME_CURPOS_YPOS,
    &(MenuManager_HighExtractDelayTime_UnitVal(0)),
    MENUMANAGER_HIGHEXTRACTDELAYTIME_UNITVAL_LENGTH
  );
  
  MenuManager_Common_LcdShowAdjustArrow
  (
    MENUMANAGER_HIGHEXTRACTDELAYTIME_CURPOS_XPOS,
    MENUMANAGER_HIGHEXTRACTDELAYTIME_CURPOS_YPOS,
    MenuManager_HighExtractDelayTime_CurPos
  );

  LCD_SetCursorPos(MENUMANAGER_HIGHEXTRACTDELAYTIME_UNIT_XPOS, MENUMANAGER_HIGHEXTRACTDELAYTIME_UNIT_YPOS, LCD_CURSOR_BY_FONT);
  LCD_PutString((uint8_t *)MenuManager_Common_SecondStr);

  MenuManager_Common_LcdUpdateAdjustMenuDynamic();
}

/*=============================================================================================*/
static void MenuManager_HighExtractDelayTime_LcdShowDone(void)
{
  MenuManager_Common_LcdClearAll();

  MenuManager_Common_LcdShowAdjustUnitVal
  (
    MENUMANAGER_HIGHEXTRACTDELAYTIME_CURPOS_XPOS,
    MENUMANAGER_HIGHEXTRACTDELAYTIME_CURPOS_YPOS,
    &(MenuManager_HighExtractDelayTime_UnitVal(0)),
    MENUMANAGER_HIGHEXTRACTDELAYTIME_UNITVAL_LENGTH
  );

  LCD_SetCursorPos(MENUMANAGER_HIGHEXTRACTDELAYTIME_UNIT_XPOS, MENUMANAGER_HIGHEXTRACTDELAYTIME_UNIT_YPOS, LCD_CURSOR_BY_FONT);
  LCD_PutString((uint8_t *)MenuManager_Common_SecondStr);

  MenuManager_Common_LcdUpdateAll();
}



/*=============================================================================================*/
static Fsm_GuardType MenuManager_HighExtractDelayTime_Entry(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  MenuManager_SubMainFunction = MenuManager_HighExtractDelayTime_SubMainFunction;
  MenuManager_SubTickHandler = MenuManager_HighExtractDelayTime_SubTickHandler;

  /* Check if previous state data hierachy is not empty */
  if (pFsmContext->dataHierachy != NULL)
  {
    if (pFsmContext->dataHierachy->dataId == MENUMANAGER_STATE_EXTRACT_SETUP)
    {
      /* Release previous state data hierachy */
      MenuManager_free(pFsmContext->dataHierachy);
      pFsmContext->dataHierachy = NULL;

      MenuManager_InternalDataPush(MENUMANAGER_HIGHEXTRACTDELAYTIME_INTERNALDATALENGTH);

      MenuManager_HighExtractDelayTime_InternalState = MENUMANAGER_HIGHEXTRACTDELAYTIME_INTERNALSTATE_READY;
      MenuManager_HighExtractDelayTime_Counter = (uint32_t)0U;
      MenuManager_HighExtractDelayTime_CurPos = (uint32_t)0U;

      MenuManager_HighExtractDelayTime_ValueMin = (uint32_t)PROGRAMMANAGER_EXTRACTSETUP_HIGHEXTRACTDELAYTIME_MIN;
      MenuManager_HighExtractDelayTime_ValueMax = (uint32_t)PROGRAMMANAGER_EXTRACTSETUP_HIGHEXTRACTDELAYTIME_MAX;
      MenuManager_HighExtractDelayTime_Value = (uint32_t)(ProgramManager_gParamConfig.extractCfg.highExtractDelayTime);

      MenuManager_Common_DecToBcdConv
      (
        &(MenuManager_HighExtractDelayTime_Value),
        &(MenuManager_HighExtractDelayTime_UnitVal(0)),
        (uint8_t)MENUMANAGER_HIGHEXTRACTDELAYTIME_UNITVAL_LENGTH
      );
    }
    else
    {
      return FSM_GUARD_FALSE;
    }

    MenuManager_HighExtractDelayTime_LcdShowMainTitle();
    MenuManager_HighExtractDelayTime_LcdShowAdjust();

    return FSM_GUARD_TRUE;
  }

  return FSM_GUARD_FALSE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_HighExtractDelayTime_Exit(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  MenuManager_SubMainFunction = NULL;
  MenuManager_SubTickHandler = NULL;

  Fsm_DataHierachyStruct* dataHierachy;

  dataHierachy = (Fsm_DataHierachyStruct *)MenuManager_malloc(sizeof(Fsm_DataHierachyStruct));
  dataHierachy->dataId = MENUMANAGER_STATE_HIGH_EXTRACT_DELAY_TIME;

  pFsmContext->dataHierachy = dataHierachy;

  /* Free internal data */
  MenuManager_InternalDataPop();
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_HighExtractDelayTime_Submenu1(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_HighExtractDelayTime_StartBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_HighExtractDelayTime_InternalState == MENUMANAGER_HIGHEXTRACTDELAYTIME_INTERNALSTATE_READY)
  {
    MenuManager_HighExtractDelayTime_InternalState = MENUMANAGER_HIGHEXTRACTDELAYTIME_INTERNALSTATE_RUNNING;
  }
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_HighExtractDelayTime_StopBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_HighExtractDelayTime_InternalState != MENUMANAGER_HIGHEXTRACTDELAYTIME_INTERNALSTATE_READY)
  {
    return FSM_GUARD_FALSE;
  }
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_HighExtractDelayTime_UpBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  uint32_t tempUnitVal;
  uint32_t tempVal;

  if (MenuManager_HighExtractDelayTime_InternalState == MENUMANAGER_HIGHEXTRACTDELAYTIME_INTERNALSTATE_READY)
  {
    tempUnitVal = MenuManager_HighExtractDelayTime_UnitVal(MenuManager_HighExtractDelayTime_CurPos);

    MenuManager_HighExtractDelayTime_UnitVal(MenuManager_HighExtractDelayTime_CurPos) += (uint32_t)1U;

    if (MenuManager_HighExtractDelayTime_UnitVal(MenuManager_HighExtractDelayTime_CurPos) > (uint32_t)9U)
    {
      MenuManager_HighExtractDelayTime_UnitVal(MenuManager_HighExtractDelayTime_CurPos) = (uint32_t)0U;
    }

    MenuManager_Common_BcdToDecConv(&tempVal, &(MenuManager_HighExtractDelayTime_UnitVal(0)), (uint8_t)MENUMANAGER_HIGHEXTRACTDELAYTIME_UNITVAL_LENGTH);

    if ((tempVal < MenuManager_HighExtractDelayTime_ValueMin) || (tempVal > MenuManager_HighExtractDelayTime_ValueMax))
    {
      MenuManager_HighExtractDelayTime_UnitVal(MenuManager_HighExtractDelayTime_CurPos) = tempUnitVal;
    }

    MenuManager_HighExtractDelayTime_LcdShowAdjust();
  }

  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_HighExtractDelayTime_DownBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  uint32_t tempUnitVal;
  uint32_t tempVal;

  if (MenuManager_HighExtractDelayTime_InternalState == MENUMANAGER_HIGHEXTRACTDELAYTIME_INTERNALSTATE_READY)
  {
    tempUnitVal = MenuManager_HighExtractDelayTime_UnitVal(MenuManager_HighExtractDelayTime_CurPos);

    MenuManager_HighExtractDelayTime_UnitVal(MenuManager_HighExtractDelayTime_CurPos) -= (uint32_t)1U;

    if (MenuManager_HighExtractDelayTime_UnitVal(MenuManager_HighExtractDelayTime_CurPos) > (uint32_t)9U)
    {
      MenuManager_HighExtractDelayTime_UnitVal(MenuManager_HighExtractDelayTime_CurPos) = (uint32_t)9U;
    }

    MenuManager_Common_BcdToDecConv(&tempVal, &(MenuManager_HighExtractDelayTime_UnitVal(0)), (uint8_t)MENUMANAGER_HIGHEXTRACTDELAYTIME_UNITVAL_LENGTH);

    if ((tempVal < MenuManager_HighExtractDelayTime_ValueMin) || (tempVal > MenuManager_HighExtractDelayTime_ValueMax))
    {
      MenuManager_HighExtractDelayTime_UnitVal(MenuManager_HighExtractDelayTime_CurPos) = tempUnitVal;
    }

    MenuManager_HighExtractDelayTime_LcdShowAdjust();
  }
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_HighExtractDelayTime_AddBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_HighExtractDelayTime_InternalState == MENUMANAGER_HIGHEXTRACTDELAYTIME_INTERNALSTATE_READY)
  {
    if (MenuManager_HighExtractDelayTime_CurPos > (uint32_t)0U)
    {
      MenuManager_HighExtractDelayTime_CurPos -= (uint32_t)1U;
    }

    MenuManager_HighExtractDelayTime_LcdShowAdjust();
  }
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_HighExtractDelayTime_SubBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_HighExtractDelayTime_InternalState == MENUMANAGER_HIGHEXTRACTDELAYTIME_INTERNALSTATE_READY)
  {
    if (MenuManager_HighExtractDelayTime_CurPos < (MENUMANAGER_HIGHEXTRACTDELAYTIME_UNITVAL_LENGTH - (uint32_t)1U))
    {
      MenuManager_HighExtractDelayTime_CurPos += (uint32_t)1U;
    }
    
    MenuManager_HighExtractDelayTime_LcdShowAdjust();
  }
  
  return FSM_GUARD_TRUE;
}



/*=============================================================================================*/
static void MenuManager_HighExtractDelayTime_SubMainFunction(void)
{
  MenuManager_Common_ButEventMapHandler(&MenuManager_HighExtractDelayTime_ButEventMapConf);

  switch (MenuManager_HighExtractDelayTime_InternalState)
  {
    case MENUMANAGER_HIGHEXTRACTDELAYTIME_INTERNALSTATE_READY:
      break;
    case MENUMANAGER_HIGHEXTRACTDELAYTIME_INTERNALSTATE_RUNNING:
    {
      uint16_t tempHighExtractDelayTime;

      MenuManager_Common_BcdToDecConv(&(MenuManager_HighExtractDelayTime_Value), &(MenuManager_HighExtractDelayTime_UnitVal(0)), (uint8_t)MENUMANAGER_HIGHEXTRACTDELAYTIME_UNITVAL_LENGTH);

      tempHighExtractDelayTime = (uint16_t)MenuManager_HighExtractDelayTime_Value;

      ProgramManager_ExtractSetup_HighExtractDelayTime_SetData(&tempHighExtractDelayTime);

      ProgramManager_ExtractSetup_HighExtractDelayTime_GetData(&(ProgramManager_gParamConfig.extractCfg.highExtractDelayTime));

      MenuManager_HighExtractDelayTime_InternalState = MENUMANAGER_HIGHEXTRACTDELAYTIME_INTERNALSTATE_DONE;
      
      break;
    }
    case MENUMANAGER_HIGHEXTRACTDELAYTIME_INTERNALSTATE_DONE:
    {
      MenuManager_HighExtractDelayTime_LcdShowDone();

      MenuManager_HighExtractDelayTime_InternalState = MENUMANAGER_HIGHEXTRACTDELAYTIME_INTERNALSTATE_EXITING;

      break;
    }
    case MENUMANAGER_HIGHEXTRACTDELAYTIME_INTERNALSTATE_EXITING:
    default:
      break;
  }
}

/*=============================================================================================*/
static void MenuManager_HighExtractDelayTime_SubTickHandler(void)
{
  if (MenuManager_HighExtractDelayTime_InternalState == MENUMANAGER_HIGHEXTRACTDELAYTIME_INTERNALSTATE_EXITING)
  {
    MenuManager_HighExtractDelayTime_Counter += (uint32_t)1U;

    if (MenuManager_HighExtractDelayTime_Counter >= MENUMANAGER_HIGHEXTRACTDELAYTIME_COUNTER_MAX)
    {
      MenuManager_HighExtractDelayTime_Counter = (uint32_t)0U;
      
      Fsm_TriggerEvent(&MenuManager_FsmContext, (Fsm_EventType)MENUMANAGER_EVENT_SUBMENU_1);
    }
  }
}



#ifdef  __cplusplus
}
#endif
