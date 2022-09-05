/* 
 * File:   _1273000000_menu_BalanceWithWaterTime.c
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
#define MENUMANAGER_BALANCEWITHWATERTIME_INTERNALDATALENGTH           (uint8_t)10U

#define MENUMANAGER_BALANCEWITHWATERTIME_INTERNALSTATE_IDX            0U
#define MENUMANAGER_BALANCEWITHWATERTIME_COUNTER_IDX                  1U
#define MENUMANAGER_BALANCEWITHWATERTIME_CURPOS_IDX                   2U
#define MENUMANAGER_BALANCEWITHWATERTIME_VALUE_MIN_IDX                3U
#define MENUMANAGER_BALANCEWITHWATERTIME_VALUE_MAX_IDX                4U
#define MENUMANAGER_BALANCEWITHWATERTIME_VALUE_IDX                    5U
#define MENUMANAGER_BALANCEWITHWATERTIME_UNITVAL_IDX                  6U

#define MenuManager_BalanceWithWaterTime_InternalState                MenuManager_GetInternalDataPtr(MENUMANAGER_BALANCEWITHWATERTIME_INTERNALSTATE_IDX)
#define MenuManager_BalanceWithWaterTime_Counter                      MenuManager_GetInternalDataPtr(MENUMANAGER_BALANCEWITHWATERTIME_COUNTER_IDX)
#define MenuManager_BalanceWithWaterTime_CurPos                       MenuManager_GetInternalDataPtr(MENUMANAGER_BALANCEWITHWATERTIME_CURPOS_IDX)
#define MenuManager_BalanceWithWaterTime_ValueMin                     MenuManager_GetInternalDataPtr(MENUMANAGER_BALANCEWITHWATERTIME_VALUE_MIN_IDX)
#define MenuManager_BalanceWithWaterTime_ValueMax                     MenuManager_GetInternalDataPtr(MENUMANAGER_BALANCEWITHWATERTIME_VALUE_MAX_IDX)
#define MenuManager_BalanceWithWaterTime_Value                        MenuManager_GetInternalDataPtr(MENUMANAGER_BALANCEWITHWATERTIME_VALUE_IDX)
#define MenuManager_BalanceWithWaterTime_UnitVal(idx)                 MenuManager_GetInternalDataPtr(MENUMANAGER_BALANCEWITHWATERTIME_UNITVAL_IDX + idx)



#define MENUMANAGER_BALANCEWITHWATERTIME_INTERNALSTATE_READY          (uint32_t)0U
#define MENUMANAGER_BALANCEWITHWATERTIME_INTERNALSTATE_RUNNING        (uint32_t)1U
#define MENUMANAGER_BALANCEWITHWATERTIME_INTERNALSTATE_DONE           (uint32_t)2U
#define MENUMANAGER_BALANCEWITHWATERTIME_INTERNALSTATE_EXITING        (uint32_t)3U

#define MENUMANAGER_BALANCEWITHWATERTIME_COUNTER_MAX                  (uint32_t)50U

#define MENUMANAGER_BALANCEWITHWATERTIME_CURPOS_XPOS                  (uint32_t)5U
#define MENUMANAGER_BALANCEWITHWATERTIME_CURPOS_YPOS                  (uint32_t)5U

#define MENUMANAGER_BALANCEWITHWATERTIME_UNIT_XPOS                    (uint32_t)9U
#define MENUMANAGER_BALANCEWITHWATERTIME_UNIT_YPOS                    (uint32_t)5U

#define MENUMANAGER_BALANCEWITHWATERTIME_UNITVAL_LENGTH               (uint32_t)3U



/** Menu manager main titles and child menu titles */
static const uint8_t MenuManager_BalanceWithWaterTime_MainTitleStr[] = "BAL WATER TIME";



/** Menu manager button event mapping array */
static MenuManager_ButEventMapStruct MenuManager_BalanceWithWaterTime_ButEventMap[6] =
{
  { IOMANAGER_BUTTON_STATE_START,                                     MENUMANAGER_EVENT_START_BUT             },
  { IOMANAGER_BUTTON_STATE_STOP,                                      MENUMANAGER_EVENT_STOP_BUT              },
  { IOMANAGER_BUTTON_STATE_UP,                                        MENUMANAGER_EVENT_UP_BUT                },
  { IOMANAGER_BUTTON_STATE_DOWN,                                      MENUMANAGER_EVENT_DOWN_BUT              },
  { IOMANAGER_BUTTON_STATE_ADD,                                       MENUMANAGER_EVENT_ADD_BUT               },
  { IOMANAGER_BUTTON_STATE_SUB,                                       MENUMANAGER_EVENT_SUB_BUT               }
};

/** Menu manager button event mapping configuration */
static MenuManager_ButEventMapConfStruct MenuManager_BalanceWithWaterTime_ButEventMapConf =
{
  .butEventMapNum             = (uint8_t)6U,
  .butEventMapCfg             = &MenuManager_BalanceWithWaterTime_ButEventMap
};



/** Menu manager event handlers */
static tFsmGuard MenuManager_BalanceWithWaterTime_Entry               (tFsmContextPtr const pFsmContext, tFsmEvent event);
static tFsmGuard MenuManager_BalanceWithWaterTime_Exit                (tFsmContextPtr const pFsmContext, tFsmEvent event);
static tFsmGuard MenuManager_BalanceWithWaterTime_Submenu1            (tFsmContextPtr const pFsmContext, tFsmEvent event);
static tFsmGuard MenuManager_BalanceWithWaterTime_StartBut            (tFsmContextPtr const pFsmContext, tFsmEvent event);
static tFsmGuard MenuManager_BalanceWithWaterTime_StopBut             (tFsmContextPtr const pFsmContext, tFsmEvent event);
static tFsmGuard MenuManager_BalanceWithWaterTime_UpBut               (tFsmContextPtr const pFsmContext, tFsmEvent event);
static tFsmGuard MenuManager_BalanceWithWaterTime_DownBut             (tFsmContextPtr const pFsmContext, tFsmEvent event);
static tFsmGuard MenuManager_BalanceWithWaterTime_AddBut              (tFsmContextPtr const pFsmContext, tFsmEvent event);
static tFsmGuard MenuManager_BalanceWithWaterTime_SubBut              (tFsmContextPtr const pFsmContext, tFsmEvent event);

/** Menu manager state machine */
tFsmEventEntry MenuManager_BalanceWithWaterTime_StateMachine[9] =
{
  FSM_TRIGGER_ENTRY             (                                     MenuManager_BalanceWithWaterTime_Entry                                          ),
  FSM_TRIGGER_EXIT              (                                     MenuManager_BalanceWithWaterTime_Exit                                           ),
  FSM_TRIGGER_TRANSITION_ACTION ( MENUMANAGER_EVENT_SUBMENU_1,        MenuManager_BalanceWithWaterTime_Submenu1, MENUMANAGER_STATE_EXTRACT_SETUP      ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_START_BUT,        MenuManager_BalanceWithWaterTime_StartBut                                       ),
  FSM_TRIGGER_TRANSITION_ACTION ( MENUMANAGER_EVENT_STOP_BUT,         MenuManager_BalanceWithWaterTime_StopBut,  MENUMANAGER_STATE_EXTRACT_SETUP      ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_UP_BUT,           MenuManager_BalanceWithWaterTime_UpBut                                          ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_DOWN_BUT,         MenuManager_BalanceWithWaterTime_DownBut                                        ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_ADD_BUT,          MenuManager_BalanceWithWaterTime_AddBut                                         ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_SUB_BUT,          MenuManager_BalanceWithWaterTime_SubBut                                         )
};



/*===============================================================================================
*                                   LOCAL FUNCTION PROTOTYPES
===============================================================================================*/

static void MenuManager_BalanceWithWaterTime_LcdShowMainTitle(void);
static void MenuManager_BalanceWithWaterTime_LcdShowAdjust(void);
static void MenuManager_BalanceWithWaterTime_LcdShowDone(void);

static void MenuManager_BalanceWithWaterTime_SubMainFunction(void);
static void MenuManager_BalanceWithWaterTime_SubTickHandler(void);



/*===============================================================================================
*                                       LOCAL FUNCTIONS
===============================================================================================*/

static void MenuManager_BalanceWithWaterTime_LcdShowMainTitle(void)
{
  /* Print main title */
  MenuManager_Common_LcdClearAdjustMenuStatic();
  MenuManager_Common_LcdShowMainTitle(MenuManager_BalanceWithWaterTime_MainTitleStr);
  MenuManager_Common_LcdShowOkExitChoice();
  MenuManager_Common_LcdUpdateAdjustMenuStatic();
}

/*=============================================================================================*/
static void MenuManager_BalanceWithWaterTime_LcdShowAdjust(void)
{
  MenuManager_Common_LcdClearAdjustMenuDynamic();

  MenuManager_Common_LcdShowAdjustUnitVal
  (
    MENUMANAGER_BALANCEWITHWATERTIME_CURPOS_XPOS,
    MENUMANAGER_BALANCEWITHWATERTIME_CURPOS_YPOS,
    &(MenuManager_BalanceWithWaterTime_UnitVal(0)),
    MENUMANAGER_BALANCEWITHWATERTIME_UNITVAL_LENGTH
  );
  
  MenuManager_Common_LcdShowAdjustArrow
  (
    MENUMANAGER_BALANCEWITHWATERTIME_CURPOS_XPOS,
    MENUMANAGER_BALANCEWITHWATERTIME_CURPOS_YPOS,
    MenuManager_BalanceWithWaterTime_CurPos
  );

  LCD_SetCursorPos(MENUMANAGER_BALANCEWITHWATERTIME_UNIT_XPOS, MENUMANAGER_BALANCEWITHWATERTIME_UNIT_YPOS, LCD_CURSOR_BY_FONT);
  LCD_PutString((uint8_t *)MenuManager_Common_SecondStr);

  MenuManager_Common_LcdUpdateAdjustMenuDynamic();
}

/*=============================================================================================*/
static void MenuManager_BalanceWithWaterTime_LcdShowDone(void)
{
  MenuManager_Common_LcdClearAll();

  MenuManager_Common_LcdShowAdjustUnitVal
  (
    MENUMANAGER_BALANCEWITHWATERTIME_CURPOS_XPOS,
    MENUMANAGER_BALANCEWITHWATERTIME_CURPOS_YPOS,
    &(MenuManager_BalanceWithWaterTime_UnitVal(0)),
    MENUMANAGER_BALANCEWITHWATERTIME_UNITVAL_LENGTH
  );

  LCD_SetCursorPos(MENUMANAGER_BALANCEWITHWATERTIME_UNIT_XPOS, MENUMANAGER_BALANCEWITHWATERTIME_UNIT_YPOS, LCD_CURSOR_BY_FONT);
  LCD_PutString((uint8_t *)MenuManager_Common_SecondStr);

  MenuManager_Common_LcdUpdateAll();
}



/*=============================================================================================*/
static tFsmGuard MenuManager_BalanceWithWaterTime_Entry(tFsmContextPtr const pFsmContext, tFsmEvent event)
{
  MenuManager_SubMainFunction = MenuManager_BalanceWithWaterTime_SubMainFunction;
  MenuManager_SubTickHandler = MenuManager_BalanceWithWaterTime_SubTickHandler;

  /* Check if previous state data hierachy is not empty */
  if (pFsmContext->dataHierachy != NULL)
  {
    if (pFsmContext->dataHierachy->dataId == MENUMANAGER_STATE_EXTRACT_SETUP)
    {
      /* Release previous state data hierachy */
      MenuManager_free(pFsmContext->dataHierachy);
      pFsmContext->dataHierachy = NULL;

      MenuManager_InternalDataPush(MENUMANAGER_BALANCEWITHWATERTIME_INTERNALDATALENGTH);

      MenuManager_BalanceWithWaterTime_InternalState = MENUMANAGER_BALANCEWITHWATERTIME_INTERNALSTATE_READY;
      MenuManager_BalanceWithWaterTime_Counter = (uint32_t)0U;
      MenuManager_BalanceWithWaterTime_CurPos = (uint32_t)0U;

      MenuManager_BalanceWithWaterTime_ValueMin = (uint32_t)PROGRAMMANAGER_EXTRACTSETUP_COMMON_TIME_MIN;
      MenuManager_BalanceWithWaterTime_ValueMax = (uint32_t)PROGRAMMANAGER_EXTRACTSETUP_COMMON_TIME_MAX;
      MenuManager_BalanceWithWaterTime_Value = (uint32_t)(ProgramManager_gParamConfig.extractCfg.balanceWithWaterTime);

      MenuManager_Common_DecToBcdConv
      (
        &(MenuManager_BalanceWithWaterTime_Value),
        &(MenuManager_BalanceWithWaterTime_UnitVal(0)),
        (uint8_t)MENUMANAGER_BALANCEWITHWATERTIME_UNITVAL_LENGTH
      );
    }
    else
    {
      return kFsmGuard_False;
    }

    MenuManager_BalanceWithWaterTime_LcdShowMainTitle();
    MenuManager_BalanceWithWaterTime_LcdShowAdjust();

    return kFsmGuard_True;
  }

  return kFsmGuard_False;
}

/*=============================================================================================*/
static tFsmGuard MenuManager_BalanceWithWaterTime_Exit(tFsmContextPtr const pFsmContext, tFsmEvent event)
{
  MenuManager_SubMainFunction = NULL;
  MenuManager_SubTickHandler = NULL;

  tFsmDataHierachy* dataHierachy;

  dataHierachy = (tFsmDataHierachy *)MenuManager_malloc(sizeof(tFsmDataHierachy));
  dataHierachy->dataId = MENUMANAGER_STATE_BALANCE_WITH_WATER_TIME;

  pFsmContext->dataHierachy = dataHierachy;

  /* Free internal data */
  MenuManager_InternalDataPop();
  
  return kFsmGuard_True;
}

/*=============================================================================================*/
static tFsmGuard MenuManager_BalanceWithWaterTime_Submenu1(tFsmContextPtr const pFsmContext, tFsmEvent event)
{
  
  
  return kFsmGuard_True;
}

/*=============================================================================================*/
static tFsmGuard MenuManager_BalanceWithWaterTime_StartBut(tFsmContextPtr const pFsmContext, tFsmEvent event)
{
  if (MenuManager_BalanceWithWaterTime_InternalState == MENUMANAGER_BALANCEWITHWATERTIME_INTERNALSTATE_READY)
  {
    MenuManager_BalanceWithWaterTime_InternalState = MENUMANAGER_BALANCEWITHWATERTIME_INTERNALSTATE_RUNNING;
  }
  
  return kFsmGuard_True;
}

/*=============================================================================================*/
static tFsmGuard MenuManager_BalanceWithWaterTime_StopBut(tFsmContextPtr const pFsmContext, tFsmEvent event)
{
  if (MenuManager_BalanceWithWaterTime_InternalState != MENUMANAGER_BALANCEWITHWATERTIME_INTERNALSTATE_READY)
  {
    return kFsmGuard_False;
  }
  
  return kFsmGuard_True;
}

/*=============================================================================================*/
static tFsmGuard MenuManager_BalanceWithWaterTime_UpBut(tFsmContextPtr const pFsmContext, tFsmEvent event)
{
  uint32_t tempUnitVal;
  uint32_t tempVal;

  if (MenuManager_BalanceWithWaterTime_InternalState == MENUMANAGER_BALANCEWITHWATERTIME_INTERNALSTATE_READY)
  {
    tempUnitVal = MenuManager_BalanceWithWaterTime_UnitVal(MenuManager_BalanceWithWaterTime_CurPos);

    MenuManager_BalanceWithWaterTime_UnitVal(MenuManager_BalanceWithWaterTime_CurPos) += (uint32_t)1U;

    if (MenuManager_BalanceWithWaterTime_UnitVal(MenuManager_BalanceWithWaterTime_CurPos) > (uint32_t)9U)
    {
      MenuManager_BalanceWithWaterTime_UnitVal(MenuManager_BalanceWithWaterTime_CurPos) = (uint32_t)0U;
    }

    MenuManager_Common_BcdToDecConv(&tempVal, &(MenuManager_BalanceWithWaterTime_UnitVal(0)), (uint8_t)MENUMANAGER_BALANCEWITHWATERTIME_UNITVAL_LENGTH);

    if ((tempVal <= MenuManager_BalanceWithWaterTime_ValueMin) || (tempVal >= MenuManager_BalanceWithWaterTime_ValueMax))
    {
      MenuManager_BalanceWithWaterTime_UnitVal(MenuManager_BalanceWithWaterTime_CurPos) = tempUnitVal;
    }

    MenuManager_BalanceWithWaterTime_LcdShowAdjust();
  }

  return kFsmGuard_True;
}

/*=============================================================================================*/
static tFsmGuard MenuManager_BalanceWithWaterTime_DownBut(tFsmContextPtr const pFsmContext, tFsmEvent event)
{
  uint32_t tempUnitVal;
  uint32_t tempVal;

  if (MenuManager_BalanceWithWaterTime_InternalState == MENUMANAGER_BALANCEWITHWATERTIME_INTERNALSTATE_READY)
  {
    tempUnitVal = MenuManager_BalanceWithWaterTime_UnitVal(MenuManager_BalanceWithWaterTime_CurPos);

    MenuManager_BalanceWithWaterTime_UnitVal(MenuManager_BalanceWithWaterTime_CurPos) -= (uint32_t)1U;

    if (MenuManager_BalanceWithWaterTime_UnitVal(MenuManager_BalanceWithWaterTime_CurPos) > (uint32_t)9U)
    {
      MenuManager_BalanceWithWaterTime_UnitVal(MenuManager_BalanceWithWaterTime_CurPos) = (uint32_t)9U;
    }

    MenuManager_Common_BcdToDecConv(&tempVal, &(MenuManager_BalanceWithWaterTime_UnitVal(0)), (uint8_t)MENUMANAGER_BALANCEWITHWATERTIME_UNITVAL_LENGTH);

    if ((tempVal <= MenuManager_BalanceWithWaterTime_ValueMin) || (tempVal >= MenuManager_BalanceWithWaterTime_ValueMax))
    {
      MenuManager_BalanceWithWaterTime_UnitVal(MenuManager_BalanceWithWaterTime_CurPos) = tempUnitVal;
    }

    MenuManager_BalanceWithWaterTime_LcdShowAdjust();
  }
  
  return kFsmGuard_True;
}

/*=============================================================================================*/
static tFsmGuard MenuManager_BalanceWithWaterTime_AddBut(tFsmContextPtr const pFsmContext, tFsmEvent event)
{
  if (MenuManager_BalanceWithWaterTime_InternalState == MENUMANAGER_BALANCEWITHWATERTIME_INTERNALSTATE_READY)
  {
    if (MenuManager_BalanceWithWaterTime_CurPos > (uint32_t)0U)
    {
      MenuManager_BalanceWithWaterTime_CurPos -= (uint32_t)1U;
    }

    MenuManager_BalanceWithWaterTime_LcdShowAdjust();
  }
  
  return kFsmGuard_True;
}

/*=============================================================================================*/
static tFsmGuard MenuManager_BalanceWithWaterTime_SubBut(tFsmContextPtr const pFsmContext, tFsmEvent event)
{
  if (MenuManager_BalanceWithWaterTime_InternalState == MENUMANAGER_BALANCEWITHWATERTIME_INTERNALSTATE_READY)
  {
    if (MenuManager_BalanceWithWaterTime_CurPos < (MENUMANAGER_BALANCEWITHWATERTIME_UNITVAL_LENGTH - (uint32_t)1U))
    {
      MenuManager_BalanceWithWaterTime_CurPos += (uint32_t)1U;
    }
    
    MenuManager_BalanceWithWaterTime_LcdShowAdjust();
  }
  
  return kFsmGuard_True;
}



/*=============================================================================================*/
static void MenuManager_BalanceWithWaterTime_SubMainFunction(void)
{
  MenuManager_Common_ButEventMapHandler(&MenuManager_BalanceWithWaterTime_ButEventMapConf);

  switch (MenuManager_BalanceWithWaterTime_InternalState)
  {
    case MENUMANAGER_BALANCEWITHWATERTIME_INTERNALSTATE_READY:
      break;
    case MENUMANAGER_BALANCEWITHWATERTIME_INTERNALSTATE_RUNNING:
    {
      uint16_t tempBalanceWithWaterTime;

      MenuManager_Common_BcdToDecConv(&(MenuManager_BalanceWithWaterTime_Value), &(MenuManager_BalanceWithWaterTime_UnitVal(0)), (uint8_t)MENUMANAGER_BALANCEWITHWATERTIME_UNITVAL_LENGTH);

      tempBalanceWithWaterTime = (uint16_t)MenuManager_BalanceWithWaterTime_Value;

      ProgramManager_ExtractSetup_BalanceWithWaterTime_SetData(&tempBalanceWithWaterTime);

      ProgramManager_ExtractSetup_BalanceWithWaterTime_GetData(&(ProgramManager_gParamConfig.extractCfg.balanceWithWaterTime));

      MenuManager_BalanceWithWaterTime_InternalState = MENUMANAGER_BALANCEWITHWATERTIME_INTERNALSTATE_DONE;
      
      break;
    }
    case MENUMANAGER_BALANCEWITHWATERTIME_INTERNALSTATE_DONE:
    {
      MenuManager_BalanceWithWaterTime_LcdShowDone();

      MenuManager_BalanceWithWaterTime_InternalState = MENUMANAGER_BALANCEWITHWATERTIME_INTERNALSTATE_EXITING;

      break;
    }
    case MENUMANAGER_BALANCEWITHWATERTIME_INTERNALSTATE_EXITING:
    default:
      break;
  }
}

/*=============================================================================================*/
static void MenuManager_BalanceWithWaterTime_SubTickHandler(void)
{
  if (MenuManager_BalanceWithWaterTime_InternalState == MENUMANAGER_BALANCEWITHWATERTIME_INTERNALSTATE_EXITING)
  {
    MenuManager_BalanceWithWaterTime_Counter += (uint32_t)1U;

    if (MenuManager_BalanceWithWaterTime_Counter >= MENUMANAGER_BALANCEWITHWATERTIME_COUNTER_MAX)
    {
      MenuManager_BalanceWithWaterTime_Counter = (uint32_t)0U;
      
      Fsm_TriggerEvent(&MenuManager_FsmContext, (tFsmEvent)MENUMANAGER_EVENT_SUBMENU_1);
    }
  }
}



#ifdef  __cplusplus
}
#endif
