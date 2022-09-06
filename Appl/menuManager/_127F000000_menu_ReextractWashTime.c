/* 
 * File:   _127F000000_menu_ReextractWashTime.c
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
#define MENUMANAGER_REEXTRACTWASHTIME_INTERNALDATALENGTH              (uint8_t)10U

#define MENUMANAGER_REEXTRACTWASHTIME_INTERNALSTATE_IDX               0U
#define MENUMANAGER_REEXTRACTWASHTIME_COUNTER_IDX                     1U
#define MENUMANAGER_REEXTRACTWASHTIME_CURPOS_IDX                      2U
#define MENUMANAGER_REEXTRACTWASHTIME_VALUE_MIN_IDX                   3U
#define MENUMANAGER_REEXTRACTWASHTIME_VALUE_MAX_IDX                   4U
#define MENUMANAGER_REEXTRACTWASHTIME_VALUE_IDX                       5U
#define MENUMANAGER_REEXTRACTWASHTIME_UNITVAL_IDX                     6U

#define MenuManager_ReextractWashTime_InternalState                   MenuManager_GetInternalDataPtr(MENUMANAGER_REEXTRACTWASHTIME_INTERNALSTATE_IDX)
#define MenuManager_ReextractWashTime_Counter                         MenuManager_GetInternalDataPtr(MENUMANAGER_REEXTRACTWASHTIME_COUNTER_IDX)
#define MenuManager_ReextractWashTime_CurPos                          MenuManager_GetInternalDataPtr(MENUMANAGER_REEXTRACTWASHTIME_CURPOS_IDX)
#define MenuManager_ReextractWashTime_ValueMin                        MenuManager_GetInternalDataPtr(MENUMANAGER_REEXTRACTWASHTIME_VALUE_MIN_IDX)
#define MenuManager_ReextractWashTime_ValueMax                        MenuManager_GetInternalDataPtr(MENUMANAGER_REEXTRACTWASHTIME_VALUE_MAX_IDX)
#define MenuManager_ReextractWashTime_Value                           MenuManager_GetInternalDataPtr(MENUMANAGER_REEXTRACTWASHTIME_VALUE_IDX)
#define MenuManager_ReextractWashTime_UnitVal(idx)                    MenuManager_GetInternalDataPtr(MENUMANAGER_REEXTRACTWASHTIME_UNITVAL_IDX + idx)



#define MENUMANAGER_REEXTRACTWASHTIME_INTERNALSTATE_READY             (uint32_t)0U
#define MENUMANAGER_REEXTRACTWASHTIME_INTERNALSTATE_RUNNING           (uint32_t)1U
#define MENUMANAGER_REEXTRACTWASHTIME_INTERNALSTATE_DONE              (uint32_t)2U
#define MENUMANAGER_REEXTRACTWASHTIME_INTERNALSTATE_EXITING           (uint32_t)3U

#define MENUMANAGER_REEXTRACTWASHTIME_COUNTER_MAX                     (uint32_t)50U

#define MENUMANAGER_REEXTRACTWASHTIME_CURPOS_XPOS                     (uint32_t)5U
#define MENUMANAGER_REEXTRACTWASHTIME_CURPOS_YPOS                     (uint32_t)5U

#define MENUMANAGER_REEXTRACTWASHTIME_UNIT_XPOS                       (uint32_t)9U
#define MENUMANAGER_REEXTRACTWASHTIME_UNIT_YPOS                       (uint32_t)5U

#define MENUMANAGER_REEXTRACTWASHTIME_UNITVAL_LENGTH                  (uint32_t)3U



/** Menu manager main titles and child menu titles */
static const uint8_t MenuManager_ReextractWashTime_MainTitleStr[] =   "RE-EXT WASH TIME";



/** Menu manager button event mapping array */
static MenuManager_ButEventMapStruct MenuManager_ReextractWashTime_ButEventMap[6] =
{
  { IOMANAGER_BUTTON_STATE_START,                                     MENUMANAGER_EVENT_START_BUT             },
  { IOMANAGER_BUTTON_STATE_STOP,                                      MENUMANAGER_EVENT_STOP_BUT              },
  { IOMANAGER_BUTTON_STATE_UP,                                        MENUMANAGER_EVENT_UP_BUT                },
  { IOMANAGER_BUTTON_STATE_DOWN,                                      MENUMANAGER_EVENT_DOWN_BUT              },
  { IOMANAGER_BUTTON_STATE_ADD,                                       MENUMANAGER_EVENT_ADD_BUT               },
  { IOMANAGER_BUTTON_STATE_SUB,                                       MENUMANAGER_EVENT_SUB_BUT               }
};

/** Menu manager button event mapping configuration */
static MenuManager_ButEventMapConfStruct MenuManager_ReextractWashTime_ButEventMapConf =
{
  .butEventMapNum             = (uint8_t)6U,
  .butEventMapCfg             = &MenuManager_ReextractWashTime_ButEventMap
};



/** Menu manager event handlers */
static tFsmGuard MenuManager_ReextractWashTime_Entry                  (tFsmContextPtr const pFsmContext, tFsmEvent event);
static tFsmGuard MenuManager_ReextractWashTime_Exit                   (tFsmContextPtr const pFsmContext, tFsmEvent event);
static tFsmGuard MenuManager_ReextractWashTime_Submenu1               (tFsmContextPtr const pFsmContext, tFsmEvent event);
static tFsmGuard MenuManager_ReextractWashTime_StartBut               (tFsmContextPtr const pFsmContext, tFsmEvent event);
static tFsmGuard MenuManager_ReextractWashTime_StopBut                (tFsmContextPtr const pFsmContext, tFsmEvent event);
static tFsmGuard MenuManager_ReextractWashTime_UpBut                  (tFsmContextPtr const pFsmContext, tFsmEvent event);
static tFsmGuard MenuManager_ReextractWashTime_DownBut                (tFsmContextPtr const pFsmContext, tFsmEvent event);
static tFsmGuard MenuManager_ReextractWashTime_AddBut                 (tFsmContextPtr const pFsmContext, tFsmEvent event);
static tFsmGuard MenuManager_ReextractWashTime_SubBut                 (tFsmContextPtr const pFsmContext, tFsmEvent event);

/** Menu manager state machine */
tFsmEventEntry MenuManager_ReextractWashTime_StateMachine[9] =
{
  FSM_TRIGGER_ENTRY             (                                     MenuManager_ReextractWashTime_Entry                                             ),
  FSM_TRIGGER_EXIT              (                                     MenuManager_ReextractWashTime_Exit                                              ),
  FSM_TRIGGER_TRANSITION_ACTION ( MENUMANAGER_EVENT_SUBMENU_1,        MenuManager_ReextractWashTime_Submenu1, MENUMANAGER_STATE_EXTRACT_SETUP         ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_START_BUT,        MenuManager_ReextractWashTime_StartBut                                          ),
  FSM_TRIGGER_TRANSITION_ACTION ( MENUMANAGER_EVENT_STOP_BUT,         MenuManager_ReextractWashTime_StopBut,  MENUMANAGER_STATE_EXTRACT_SETUP         ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_UP_BUT,           MenuManager_ReextractWashTime_UpBut                                             ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_DOWN_BUT,         MenuManager_ReextractWashTime_DownBut                                           ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_ADD_BUT,          MenuManager_ReextractWashTime_AddBut                                            ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_SUB_BUT,          MenuManager_ReextractWashTime_SubBut                                            )
};



/*===============================================================================================
*                                   LOCAL FUNCTION PROTOTYPES
===============================================================================================*/

static void MenuManager_ReextractWashTime_LcdShowMainTitle(void);
static void MenuManager_ReextractWashTime_LcdShowAdjust(void);
static void MenuManager_ReextractWashTime_LcdShowDone(void);

static void MenuManager_ReextractWashTime_SubMainFunction(void);
static void MenuManager_ReextractWashTime_SubTickHandler(void);



/*===============================================================================================
*                                       LOCAL FUNCTIONS
===============================================================================================*/

static void MenuManager_ReextractWashTime_LcdShowMainTitle(void)
{
  /* Print main title */
  MenuManager_Common_LcdClearAdjustMenuStatic();
  MenuManager_Common_LcdShowMainTitle(MenuManager_ReextractWashTime_MainTitleStr);
  MenuManager_Common_LcdShowOkExitChoice();
  MenuManager_Common_LcdUpdateAdjustMenuStatic();
}

/*=============================================================================================*/
static void MenuManager_ReextractWashTime_LcdShowAdjust(void)
{
  MenuManager_Common_LcdClearAdjustMenuDynamic();

  MenuManager_Common_LcdShowAdjustUnitVal
  (
    MENUMANAGER_REEXTRACTWASHTIME_CURPOS_XPOS,
    MENUMANAGER_REEXTRACTWASHTIME_CURPOS_YPOS,
    &(MenuManager_ReextractWashTime_UnitVal(0)),
    MENUMANAGER_REEXTRACTWASHTIME_UNITVAL_LENGTH
  );
  
  MenuManager_Common_LcdShowAdjustArrow
  (
    MENUMANAGER_REEXTRACTWASHTIME_CURPOS_XPOS,
    MENUMANAGER_REEXTRACTWASHTIME_CURPOS_YPOS,
    MenuManager_ReextractWashTime_CurPos
  );

  LCD_SetCursorPos(MENUMANAGER_REEXTRACTWASHTIME_UNIT_XPOS, MENUMANAGER_REEXTRACTWASHTIME_UNIT_YPOS, LCD_CURSOR_BY_FONT);
  LCD_PutString((uint8_t *)MenuManager_Common_SecondStr);

  MenuManager_Common_LcdUpdateAdjustMenuDynamic();
}

/*=============================================================================================*/
static void MenuManager_ReextractWashTime_LcdShowDone(void)
{
  MenuManager_Common_LcdClearAll();

  MenuManager_Common_LcdShowAdjustUnitVal
  (
    MENUMANAGER_REEXTRACTWASHTIME_CURPOS_XPOS,
    MENUMANAGER_REEXTRACTWASHTIME_CURPOS_YPOS,
    &(MenuManager_ReextractWashTime_UnitVal(0)),
    MENUMANAGER_REEXTRACTWASHTIME_UNITVAL_LENGTH
  );

  LCD_SetCursorPos(MENUMANAGER_REEXTRACTWASHTIME_UNIT_XPOS, MENUMANAGER_REEXTRACTWASHTIME_UNIT_YPOS, LCD_CURSOR_BY_FONT);
  LCD_PutString((uint8_t *)MenuManager_Common_SecondStr);

  MenuManager_Common_LcdUpdateAll();
}



/*=============================================================================================*/
static tFsmGuard MenuManager_ReextractWashTime_Entry(tFsmContextPtr const pFsmContext, tFsmEvent event)
{
  MenuManager_SubMainFunction = MenuManager_ReextractWashTime_SubMainFunction;
  MenuManager_SubTickHandler = MenuManager_ReextractWashTime_SubTickHandler;

  /* Check if previous state data hierachy is not empty */
  if (pFsmContext->dataHierachy != NULL)
  {
    if (pFsmContext->dataHierachy->dataId == MENUMANAGER_STATE_EXTRACT_SETUP)
    {
      /* Release previous state data hierachy */
      MenuManager_free(pFsmContext->dataHierachy);
      pFsmContext->dataHierachy = NULL;

      MenuManager_InternalDataPush(MENUMANAGER_REEXTRACTWASHTIME_INTERNALDATALENGTH);

      MenuManager_ReextractWashTime_InternalState = MENUMANAGER_REEXTRACTWASHTIME_INTERNALSTATE_READY;
      MenuManager_ReextractWashTime_Counter = (uint32_t)0U;
      MenuManager_ReextractWashTime_CurPos = (uint32_t)0U;

      MenuManager_ReextractWashTime_ValueMin = (uint32_t)PROGRAMMANAGER_EXTRACTSETUP_COMMON_TIME_MIN;
      MenuManager_ReextractWashTime_ValueMax = (uint32_t)PROGRAMMANAGER_EXTRACTSETUP_COMMON_TIME_MAX;
      MenuManager_ReextractWashTime_Value = (uint32_t)(ProgramManager_gParamConfig.extractCfg.reextractTime);

      MenuManager_Common_DecToBcdConv
      (
        &(MenuManager_ReextractWashTime_Value),
        &(MenuManager_ReextractWashTime_UnitVal(0)),
        (uint8_t)MENUMANAGER_REEXTRACTWASHTIME_UNITVAL_LENGTH
      );
    }
    else
    {
      return kFsmGuard_False;
    }

    MenuManager_ReextractWashTime_LcdShowMainTitle();
    MenuManager_ReextractWashTime_LcdShowAdjust();

    return kFsmGuard_True;
  }

  return kFsmGuard_False;
}

/*=============================================================================================*/
static tFsmGuard MenuManager_ReextractWashTime_Exit(tFsmContextPtr const pFsmContext, tFsmEvent event)
{
  MenuManager_SubMainFunction = NULL;
  MenuManager_SubTickHandler = NULL;

  tFsmDataHierachy* dataHierachy;

  dataHierachy = (tFsmDataHierachy *)MenuManager_malloc(sizeof(tFsmDataHierachy));
  dataHierachy->dataId = MENUMANAGER_STATE_REEXTRACT_WASH_TIME;

  pFsmContext->dataHierachy = dataHierachy;

  /* Free internal data */
  MenuManager_InternalDataPop();
  
  return kFsmGuard_True;
}

/*=============================================================================================*/
static tFsmGuard MenuManager_ReextractWashTime_Submenu1(tFsmContextPtr const pFsmContext, tFsmEvent event)
{
  
  
  return kFsmGuard_True;
}

/*=============================================================================================*/
static tFsmGuard MenuManager_ReextractWashTime_StartBut(tFsmContextPtr const pFsmContext, tFsmEvent event)
{
  if (MenuManager_ReextractWashTime_InternalState == MENUMANAGER_REEXTRACTWASHTIME_INTERNALSTATE_READY)
  {
    MenuManager_ReextractWashTime_InternalState = MENUMANAGER_REEXTRACTWASHTIME_INTERNALSTATE_RUNNING;
  }
  
  return kFsmGuard_True;
}

/*=============================================================================================*/
static tFsmGuard MenuManager_ReextractWashTime_StopBut(tFsmContextPtr const pFsmContext, tFsmEvent event)
{
  if (MenuManager_ReextractWashTime_InternalState != MENUMANAGER_REEXTRACTWASHTIME_INTERNALSTATE_READY)
  {
    return kFsmGuard_False;
  }
  
  return kFsmGuard_True;
}

/*=============================================================================================*/
static tFsmGuard MenuManager_ReextractWashTime_UpBut(tFsmContextPtr const pFsmContext, tFsmEvent event)
{
  uint32_t tempUnitVal;
  uint32_t tempVal;

  if (MenuManager_ReextractWashTime_InternalState == MENUMANAGER_REEXTRACTWASHTIME_INTERNALSTATE_READY)
  {
    tempUnitVal = MenuManager_ReextractWashTime_UnitVal(MenuManager_ReextractWashTime_CurPos);

    MenuManager_ReextractWashTime_UnitVal(MenuManager_ReextractWashTime_CurPos) += (uint32_t)1U;

    if (MenuManager_ReextractWashTime_UnitVal(MenuManager_ReextractWashTime_CurPos) > (uint32_t)9U)
    {
      MenuManager_ReextractWashTime_UnitVal(MenuManager_ReextractWashTime_CurPos) = (uint32_t)0U;
    }

    MenuManager_Common_BcdToDecConv(&tempVal, &(MenuManager_ReextractWashTime_UnitVal(0)), (uint8_t)MENUMANAGER_REEXTRACTWASHTIME_UNITVAL_LENGTH);

    if ((tempVal < MenuManager_ReextractWashTime_ValueMin) || (tempVal >= MenuManager_ReextractWashTime_ValueMax))
    {
      MenuManager_ReextractWashTime_UnitVal(MenuManager_ReextractWashTime_CurPos) = tempUnitVal;
    }

    MenuManager_ReextractWashTime_LcdShowAdjust();
  }

  return kFsmGuard_True;
}

/*=============================================================================================*/
static tFsmGuard MenuManager_ReextractWashTime_DownBut(tFsmContextPtr const pFsmContext, tFsmEvent event)
{
  uint32_t tempUnitVal;
  uint32_t tempVal;

  if (MenuManager_ReextractWashTime_InternalState == MENUMANAGER_REEXTRACTWASHTIME_INTERNALSTATE_READY)
  {
    tempUnitVal = MenuManager_ReextractWashTime_UnitVal(MenuManager_ReextractWashTime_CurPos);

    MenuManager_ReextractWashTime_UnitVal(MenuManager_ReextractWashTime_CurPos) -= (uint32_t)1U;

    if (MenuManager_ReextractWashTime_UnitVal(MenuManager_ReextractWashTime_CurPos) > (uint32_t)9U)
    {
      MenuManager_ReextractWashTime_UnitVal(MenuManager_ReextractWashTime_CurPos) = (uint32_t)9U;
    }

    MenuManager_Common_BcdToDecConv(&tempVal, &(MenuManager_ReextractWashTime_UnitVal(0)), (uint8_t)MENUMANAGER_REEXTRACTWASHTIME_UNITVAL_LENGTH);

    if ((tempVal < MenuManager_ReextractWashTime_ValueMin) || (tempVal >= MenuManager_ReextractWashTime_ValueMax))
    {
      MenuManager_ReextractWashTime_UnitVal(MenuManager_ReextractWashTime_CurPos) = tempUnitVal;
    }

    MenuManager_ReextractWashTime_LcdShowAdjust();
  }
  
  return kFsmGuard_True;
}

/*=============================================================================================*/
static tFsmGuard MenuManager_ReextractWashTime_AddBut(tFsmContextPtr const pFsmContext, tFsmEvent event)
{
  if (MenuManager_ReextractWashTime_InternalState == MENUMANAGER_REEXTRACTWASHTIME_INTERNALSTATE_READY)
  {
    if (MenuManager_ReextractWashTime_CurPos > (uint32_t)0U)
    {
      MenuManager_ReextractWashTime_CurPos -= (uint32_t)1U;
    }

    MenuManager_ReextractWashTime_LcdShowAdjust();
  }
  
  return kFsmGuard_True;
}

/*=============================================================================================*/
static tFsmGuard MenuManager_ReextractWashTime_SubBut(tFsmContextPtr const pFsmContext, tFsmEvent event)
{
  if (MenuManager_ReextractWashTime_InternalState == MENUMANAGER_REEXTRACTWASHTIME_INTERNALSTATE_READY)
  {
    if (MenuManager_ReextractWashTime_CurPos < (MENUMANAGER_REEXTRACTWASHTIME_UNITVAL_LENGTH - (uint32_t)1U))
    {
      MenuManager_ReextractWashTime_CurPos += (uint32_t)1U;
    }
    
    MenuManager_ReextractWashTime_LcdShowAdjust();
  }
  
  return kFsmGuard_True;
}



/*=============================================================================================*/
static void MenuManager_ReextractWashTime_SubMainFunction(void)
{
  MenuManager_Common_ButEventMapHandler(&MenuManager_ReextractWashTime_ButEventMapConf);

  switch (MenuManager_ReextractWashTime_InternalState)
  {
    case MENUMANAGER_REEXTRACTWASHTIME_INTERNALSTATE_READY:
      break;
    case MENUMANAGER_REEXTRACTWASHTIME_INTERNALSTATE_RUNNING:
    {
      uint16_t tempReextractWashTime;

      MenuManager_Common_BcdToDecConv(&(MenuManager_ReextractWashTime_Value), &(MenuManager_ReextractWashTime_UnitVal(0)), (uint8_t)MENUMANAGER_REEXTRACTWASHTIME_UNITVAL_LENGTH);

      tempReextractWashTime = (uint16_t)MenuManager_ReextractWashTime_Value;

      ProgramManager_ExtractSetup_ReextractWashTime_SetData(&tempReextractWashTime);

      ProgramManager_ExtractSetup_ReextractWashTime_GetData(&(ProgramManager_gParamConfig.extractCfg.reextractWashTime));

      MenuManager_ReextractWashTime_InternalState = MENUMANAGER_REEXTRACTWASHTIME_INTERNALSTATE_DONE;
      
      break;
    }
    case MENUMANAGER_REEXTRACTWASHTIME_INTERNALSTATE_DONE:
    {
      MenuManager_ReextractWashTime_LcdShowDone();

      MenuManager_ReextractWashTime_InternalState = MENUMANAGER_REEXTRACTWASHTIME_INTERNALSTATE_EXITING;

      break;
    }
    case MENUMANAGER_REEXTRACTWASHTIME_INTERNALSTATE_EXITING:
    default:
      break;
  }
}

/*=============================================================================================*/
static void MenuManager_ReextractWashTime_SubTickHandler(void)
{
  if (MenuManager_ReextractWashTime_InternalState == MENUMANAGER_REEXTRACTWASHTIME_INTERNALSTATE_EXITING)
  {
    MenuManager_ReextractWashTime_Counter += (uint32_t)1U;

    if (MenuManager_ReextractWashTime_Counter >= MENUMANAGER_REEXTRACTWASHTIME_COUNTER_MAX)
    {
      MenuManager_ReextractWashTime_Counter = (uint32_t)0U;
      
      Fsm_TriggerEvent(&MenuManager_FsmContext, (tFsmEvent)MENUMANAGER_EVENT_SUBMENU_1);
    }
  }
}



#ifdef  __cplusplus
}
#endif
