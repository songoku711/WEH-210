/* 
 * File:   _127D000000_menu_MaxExtractTime.c
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
#define MENUMANAGER_MAXEXTRACTTIME_INTERNALDATALENGTH                 (uint8_t)10U

#define MENUMANAGER_MAXEXTRACTTIME_INTERNALSTATE_IDX                  0U
#define MENUMANAGER_MAXEXTRACTTIME_COUNTER_IDX                        1U
#define MENUMANAGER_MAXEXTRACTTIME_CURPOS_IDX                         2U
#define MENUMANAGER_MAXEXTRACTTIME_VALUE_MIN_IDX                      3U
#define MENUMANAGER_MAXEXTRACTTIME_VALUE_MAX_IDX                      4U
#define MENUMANAGER_MAXEXTRACTTIME_VALUE_IDX                          5U
#define MENUMANAGER_MAXEXTRACTTIME_UNITVAL_IDX                        6U

#define MenuManager_MaxExtractTime_InternalState                      MenuManager_GetInternalDataPtr(MENUMANAGER_MAXEXTRACTTIME_INTERNALSTATE_IDX)
#define MenuManager_MaxExtractTime_Counter                            MenuManager_GetInternalDataPtr(MENUMANAGER_MAXEXTRACTTIME_COUNTER_IDX)
#define MenuManager_MaxExtractTime_CurPos                             MenuManager_GetInternalDataPtr(MENUMANAGER_MAXEXTRACTTIME_CURPOS_IDX)
#define MenuManager_MaxExtractTime_ValueMin                           MenuManager_GetInternalDataPtr(MENUMANAGER_MAXEXTRACTTIME_VALUE_MIN_IDX)
#define MenuManager_MaxExtractTime_ValueMax                           MenuManager_GetInternalDataPtr(MENUMANAGER_MAXEXTRACTTIME_VALUE_MAX_IDX)
#define MenuManager_MaxExtractTime_Value                              MenuManager_GetInternalDataPtr(MENUMANAGER_MAXEXTRACTTIME_VALUE_IDX)
#define MenuManager_MaxExtractTime_UnitVal(idx)                       MenuManager_GetInternalDataPtr(MENUMANAGER_MAXEXTRACTTIME_UNITVAL_IDX + idx)



#define MENUMANAGER_MAXEXTRACTTIME_INTERNALSTATE_READY                (uint32_t)0U
#define MENUMANAGER_MAXEXTRACTTIME_INTERNALSTATE_RUNNING              (uint32_t)1U
#define MENUMANAGER_MAXEXTRACTTIME_INTERNALSTATE_DONE                 (uint32_t)2U
#define MENUMANAGER_MAXEXTRACTTIME_INTERNALSTATE_EXITING              (uint32_t)3U

#define MENUMANAGER_MAXEXTRACTTIME_COUNTER_MAX                        (uint32_t)50U

#define MENUMANAGER_MAXEXTRACTTIME_CURPOS_XPOS                        (uint32_t)5U
#define MENUMANAGER_MAXEXTRACTTIME_CURPOS_YPOS                        (uint32_t)5U

#define MENUMANAGER_MAXEXTRACTTIME_UNIT_XPOS                          (uint32_t)9U
#define MENUMANAGER_MAXEXTRACTTIME_UNIT_YPOS                          (uint32_t)5U

#define MENUMANAGER_MAXEXTRACTTIME_UNITVAL_LENGTH                     (uint32_t)3U



/** Menu manager main titles and child menu titles */
static const uint8_t MenuManager_MaxExtractTime_MainTitleStr[] =      "MAX EXT TIME";



/** Menu manager button event mapping array */
static MenuManager_ButEventMapStruct MenuManager_MaxExtractTime_ButEventMap[6] =
{
  { IOMANAGER_BUTTON_STATE_START,                                     MENUMANAGER_EVENT_START_BUT             },
  { IOMANAGER_BUTTON_STATE_STOP,                                      MENUMANAGER_EVENT_STOP_BUT              },
  { IOMANAGER_BUTTON_STATE_UP,                                        MENUMANAGER_EVENT_UP_BUT                },
  { IOMANAGER_BUTTON_STATE_DOWN,                                      MENUMANAGER_EVENT_DOWN_BUT              },
  { IOMANAGER_BUTTON_STATE_ADD,                                       MENUMANAGER_EVENT_ADD_BUT               },
  { IOMANAGER_BUTTON_STATE_SUB,                                       MENUMANAGER_EVENT_SUB_BUT               }
};

/** Menu manager button event mapping configuration */
static MenuManager_ButEventMapConfStruct MenuManager_MaxExtractTime_ButEventMapConf =
{
  .butEventMapNum             = (uint8_t)6U,
  .butEventMapCfg             = &MenuManager_MaxExtractTime_ButEventMap
};



/** Menu manager event handlers */
static tFsmGuard MenuManager_MaxExtractTime_Entry                     (tFsmContextPtr const pFsmContext, tFsmEvent event);
static tFsmGuard MenuManager_MaxExtractTime_Exit                      (tFsmContextPtr const pFsmContext, tFsmEvent event);
static tFsmGuard MenuManager_MaxExtractTime_Submenu1                  (tFsmContextPtr const pFsmContext, tFsmEvent event);
static tFsmGuard MenuManager_MaxExtractTime_StartBut                  (tFsmContextPtr const pFsmContext, tFsmEvent event);
static tFsmGuard MenuManager_MaxExtractTime_StopBut                   (tFsmContextPtr const pFsmContext, tFsmEvent event);
static tFsmGuard MenuManager_MaxExtractTime_UpBut                     (tFsmContextPtr const pFsmContext, tFsmEvent event);
static tFsmGuard MenuManager_MaxExtractTime_DownBut                   (tFsmContextPtr const pFsmContext, tFsmEvent event);
static tFsmGuard MenuManager_MaxExtractTime_AddBut                    (tFsmContextPtr const pFsmContext, tFsmEvent event);
static tFsmGuard MenuManager_MaxExtractTime_SubBut                    (tFsmContextPtr const pFsmContext, tFsmEvent event);

/** Menu manager state machine */
tFsmEventEntry MenuManager_MaxExtractTime_StateMachine[9] =
{
  FSM_TRIGGER_ENTRY             (                                     MenuManager_MaxExtractTime_Entry                                                ),
  FSM_TRIGGER_EXIT              (                                     MenuManager_MaxExtractTime_Exit                                                 ),
  FSM_TRIGGER_TRANSITION_ACTION ( MENUMANAGER_EVENT_SUBMENU_1,        MenuManager_MaxExtractTime_Submenu1,    MENUMANAGER_STATE_EXTRACT_SETUP         ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_START_BUT,        MenuManager_MaxExtractTime_StartBut                                             ),
  FSM_TRIGGER_TRANSITION_ACTION ( MENUMANAGER_EVENT_STOP_BUT,         MenuManager_MaxExtractTime_StopBut,     MENUMANAGER_STATE_EXTRACT_SETUP         ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_UP_BUT,           MenuManager_MaxExtractTime_UpBut                                                ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_DOWN_BUT,         MenuManager_MaxExtractTime_DownBut                                              ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_ADD_BUT,          MenuManager_MaxExtractTime_AddBut                                               ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_SUB_BUT,          MenuManager_MaxExtractTime_SubBut                                               )
};



/*===============================================================================================
*                                   LOCAL FUNCTION PROTOTYPES
===============================================================================================*/

static void MenuManager_MaxExtractTime_LcdShowMainTitle(void);
static void MenuManager_MaxExtractTime_LcdShowAdjust(void);
static void MenuManager_MaxExtractTime_LcdShowDone(void);

static void MenuManager_MaxExtractTime_SubMainFunction(void);
static void MenuManager_MaxExtractTime_SubTickHandler(void);



/*===============================================================================================
*                                       LOCAL FUNCTIONS
===============================================================================================*/

static void MenuManager_MaxExtractTime_LcdShowMainTitle(void)
{
  /* Print main title */
  MenuManager_Common_LcdClearAdjustMenuStatic();
  MenuManager_Common_LcdShowMainTitle(MenuManager_MaxExtractTime_MainTitleStr);
  MenuManager_Common_LcdShowOkExitChoice();
  MenuManager_Common_LcdUpdateAdjustMenuStatic();
}

/*=============================================================================================*/
static void MenuManager_MaxExtractTime_LcdShowAdjust(void)
{
  MenuManager_Common_LcdClearAdjustMenuDynamic();

  MenuManager_Common_LcdShowAdjustUnitVal
  (
    MENUMANAGER_MAXEXTRACTTIME_CURPOS_XPOS,
    MENUMANAGER_MAXEXTRACTTIME_CURPOS_YPOS,
    &(MenuManager_MaxExtractTime_UnitVal(0)),
    MENUMANAGER_MAXEXTRACTTIME_UNITVAL_LENGTH
  );
  
  MenuManager_Common_LcdShowAdjustArrow
  (
    MENUMANAGER_MAXEXTRACTTIME_CURPOS_XPOS,
    MENUMANAGER_MAXEXTRACTTIME_CURPOS_YPOS,
    MenuManager_MaxExtractTime_CurPos
  );

  LCD_SetCursorPos(MENUMANAGER_MAXEXTRACTTIME_UNIT_XPOS, MENUMANAGER_MAXEXTRACTTIME_UNIT_YPOS, LCD_CURSOR_BY_FONT);
  LCD_PutString((uint8_t *)MenuManager_Common_MinuteStr);

  MenuManager_Common_LcdUpdateAdjustMenuDynamic();
}

/*=============================================================================================*/
static void MenuManager_MaxExtractTime_LcdShowDone(void)
{
  MenuManager_Common_LcdClearAll();

  MenuManager_Common_LcdShowAdjustUnitVal
  (
    MENUMANAGER_MAXEXTRACTTIME_CURPOS_XPOS,
    MENUMANAGER_MAXEXTRACTTIME_CURPOS_YPOS,
    &(MenuManager_MaxExtractTime_UnitVal(0)),
    MENUMANAGER_MAXEXTRACTTIME_UNITVAL_LENGTH
  );

  LCD_SetCursorPos(MENUMANAGER_MAXEXTRACTTIME_UNIT_XPOS, MENUMANAGER_MAXEXTRACTTIME_UNIT_YPOS, LCD_CURSOR_BY_FONT);
  LCD_PutString((uint8_t *)MenuManager_Common_MinuteStr);

  MenuManager_Common_LcdUpdateAll();
}



/*=============================================================================================*/
static tFsmGuard MenuManager_MaxExtractTime_Entry(tFsmContextPtr const pFsmContext, tFsmEvent event)
{
  MenuManager_SubMainFunction = MenuManager_MaxExtractTime_SubMainFunction;
  MenuManager_SubTickHandler = MenuManager_MaxExtractTime_SubTickHandler;

  /* Check if previous state data hierachy is not empty */
  if (pFsmContext->dataHierachy != NULL)
  {
    if (pFsmContext->dataHierachy->dataId == MENUMANAGER_STATE_EXTRACT_SETUP)
    {
      /* Release previous state data hierachy */
      MenuManager_free(pFsmContext->dataHierachy);
      pFsmContext->dataHierachy = NULL;

      MenuManager_InternalDataPush(MENUMANAGER_MAXEXTRACTTIME_INTERNALDATALENGTH);

      MenuManager_MaxExtractTime_InternalState = MENUMANAGER_MAXEXTRACTTIME_INTERNALSTATE_READY;
      MenuManager_MaxExtractTime_Counter = (uint32_t)0U;
      MenuManager_MaxExtractTime_CurPos = (uint32_t)0U;

      MenuManager_MaxExtractTime_ValueMin = (uint32_t)PROGRAMMANAGER_EXTRACTSETUP_MAXEXTRACTTIME_MIN;
      MenuManager_MaxExtractTime_ValueMax = (uint32_t)PROGRAMMANAGER_EXTRACTSETUP_MAXEXTRACTTIME_MAX;
      MenuManager_MaxExtractTime_Value = (uint32_t)(ProgramManager_gParamConfig.extractCfg.maxExtractTime);

      MenuManager_Common_DecToBcdConv
      (
        &(MenuManager_MaxExtractTime_Value),
        &(MenuManager_MaxExtractTime_UnitVal(0)),
        (uint8_t)MENUMANAGER_MAXEXTRACTTIME_UNITVAL_LENGTH
      );
    }
    else
    {
      return kFsmGuard_False;
    }

    MenuManager_MaxExtractTime_LcdShowMainTitle();
    MenuManager_MaxExtractTime_LcdShowAdjust();

    return kFsmGuard_True;
  }

  return kFsmGuard_False;
}

/*=============================================================================================*/
static tFsmGuard MenuManager_MaxExtractTime_Exit(tFsmContextPtr const pFsmContext, tFsmEvent event)
{
  MenuManager_SubMainFunction = NULL;
  MenuManager_SubTickHandler = NULL;

  tFsmDataHierachy* dataHierachy;

  dataHierachy = (tFsmDataHierachy *)MenuManager_malloc(sizeof(tFsmDataHierachy));
  dataHierachy->dataId = MENUMANAGER_STATE_MAX_EXTRACT_TIME;

  pFsmContext->dataHierachy = dataHierachy;

  /* Free internal data */
  MenuManager_InternalDataPop();
  
  return kFsmGuard_True;
}

/*=============================================================================================*/
static tFsmGuard MenuManager_MaxExtractTime_Submenu1(tFsmContextPtr const pFsmContext, tFsmEvent event)
{
  
  
  return kFsmGuard_True;
}

/*=============================================================================================*/
static tFsmGuard MenuManager_MaxExtractTime_StartBut(tFsmContextPtr const pFsmContext, tFsmEvent event)
{
  if (MenuManager_MaxExtractTime_InternalState == MENUMANAGER_MAXEXTRACTTIME_INTERNALSTATE_READY)
  {
    MenuManager_MaxExtractTime_InternalState = MENUMANAGER_MAXEXTRACTTIME_INTERNALSTATE_RUNNING;
  }
  
  return kFsmGuard_True;
}

/*=============================================================================================*/
static tFsmGuard MenuManager_MaxExtractTime_StopBut(tFsmContextPtr const pFsmContext, tFsmEvent event)
{
  if (MenuManager_MaxExtractTime_InternalState != MENUMANAGER_MAXEXTRACTTIME_INTERNALSTATE_READY)
  {
    return kFsmGuard_False;
  }
  
  return kFsmGuard_True;
}

/*=============================================================================================*/
static tFsmGuard MenuManager_MaxExtractTime_UpBut(tFsmContextPtr const pFsmContext, tFsmEvent event)
{
  uint32_t tempUnitVal;
  uint32_t tempVal;

  if (MenuManager_MaxExtractTime_InternalState == MENUMANAGER_MAXEXTRACTTIME_INTERNALSTATE_READY)
  {
    tempUnitVal = MenuManager_MaxExtractTime_UnitVal(MenuManager_MaxExtractTime_CurPos);

    MenuManager_MaxExtractTime_UnitVal(MenuManager_MaxExtractTime_CurPos) += (uint32_t)1U;

    if (MenuManager_MaxExtractTime_UnitVal(MenuManager_MaxExtractTime_CurPos) > (uint32_t)9U)
    {
      MenuManager_MaxExtractTime_UnitVal(MenuManager_MaxExtractTime_CurPos) = (uint32_t)0U;
    }

    MenuManager_Common_BcdToDecConv(&tempVal, &(MenuManager_MaxExtractTime_UnitVal(0)), (uint8_t)MENUMANAGER_MAXEXTRACTTIME_UNITVAL_LENGTH);

    if ((tempVal <= MenuManager_MaxExtractTime_ValueMin) || (tempVal >= MenuManager_MaxExtractTime_ValueMax))
    {
      MenuManager_MaxExtractTime_UnitVal(MenuManager_MaxExtractTime_CurPos) = tempUnitVal;
    }

    MenuManager_MaxExtractTime_LcdShowAdjust();
  }

  return kFsmGuard_True;
}

/*=============================================================================================*/
static tFsmGuard MenuManager_MaxExtractTime_DownBut(tFsmContextPtr const pFsmContext, tFsmEvent event)
{
  uint32_t tempUnitVal;
  uint32_t tempVal;

  if (MenuManager_MaxExtractTime_InternalState == MENUMANAGER_MAXEXTRACTTIME_INTERNALSTATE_READY)
  {
    tempUnitVal = MenuManager_MaxExtractTime_UnitVal(MenuManager_MaxExtractTime_CurPos);

    MenuManager_MaxExtractTime_UnitVal(MenuManager_MaxExtractTime_CurPos) -= (uint32_t)1U;

    if (MenuManager_MaxExtractTime_UnitVal(MenuManager_MaxExtractTime_CurPos) > (uint32_t)9U)
    {
      MenuManager_MaxExtractTime_UnitVal(MenuManager_MaxExtractTime_CurPos) = (uint32_t)9U;
    }

    MenuManager_Common_BcdToDecConv(&tempVal, &(MenuManager_MaxExtractTime_UnitVal(0)), (uint8_t)MENUMANAGER_MAXEXTRACTTIME_UNITVAL_LENGTH);

    if ((tempVal <= MenuManager_MaxExtractTime_ValueMin) || (tempVal >= MenuManager_MaxExtractTime_ValueMax))
    {
      MenuManager_MaxExtractTime_UnitVal(MenuManager_MaxExtractTime_CurPos) = tempUnitVal;
    }

    MenuManager_MaxExtractTime_LcdShowAdjust();
  }
  
  return kFsmGuard_True;
}

/*=============================================================================================*/
static tFsmGuard MenuManager_MaxExtractTime_AddBut(tFsmContextPtr const pFsmContext, tFsmEvent event)
{
  if (MenuManager_MaxExtractTime_InternalState == MENUMANAGER_MAXEXTRACTTIME_INTERNALSTATE_READY)
  {
    if (MenuManager_MaxExtractTime_CurPos > (uint32_t)0U)
    {
      MenuManager_MaxExtractTime_CurPos -= (uint32_t)1U;
    }

    MenuManager_MaxExtractTime_LcdShowAdjust();
  }
  
  return kFsmGuard_True;
}

/*=============================================================================================*/
static tFsmGuard MenuManager_MaxExtractTime_SubBut(tFsmContextPtr const pFsmContext, tFsmEvent event)
{
  if (MenuManager_MaxExtractTime_InternalState == MENUMANAGER_MAXEXTRACTTIME_INTERNALSTATE_READY)
  {
    if (MenuManager_MaxExtractTime_CurPos < (MENUMANAGER_MAXEXTRACTTIME_UNITVAL_LENGTH - (uint32_t)1U))
    {
      MenuManager_MaxExtractTime_CurPos += (uint32_t)1U;
    }
    
    MenuManager_MaxExtractTime_LcdShowAdjust();
  }
  
  return kFsmGuard_True;
}



/*=============================================================================================*/
static void MenuManager_MaxExtractTime_SubMainFunction(void)
{
  MenuManager_Common_ButEventMapHandler(&MenuManager_MaxExtractTime_ButEventMapConf);

  switch (MenuManager_MaxExtractTime_InternalState)
  {
    case MENUMANAGER_MAXEXTRACTTIME_INTERNALSTATE_READY:
      break;
    case MENUMANAGER_MAXEXTRACTTIME_INTERNALSTATE_RUNNING:
    {
      uint16_t tempMaxExtractTime;

      MenuManager_Common_BcdToDecConv(&(MenuManager_MaxExtractTime_Value), &(MenuManager_MaxExtractTime_UnitVal(0)), (uint8_t)MENUMANAGER_MAXEXTRACTTIME_UNITVAL_LENGTH);

      tempMaxExtractTime = (uint16_t)MenuManager_MaxExtractTime_Value;

      ProgramManager_ExtractSetup_MaxExtractTime_SetData(&tempMaxExtractTime);

      ProgramManager_ExtractSetup_MaxExtractTime_GetData(&(ProgramManager_gParamConfig.extractCfg.maxExtractTime));

      MenuManager_MaxExtractTime_InternalState = MENUMANAGER_MAXEXTRACTTIME_INTERNALSTATE_DONE;
      
      break;
    }
    case MENUMANAGER_MAXEXTRACTTIME_INTERNALSTATE_DONE:
    {
      MenuManager_MaxExtractTime_LcdShowDone();

      MenuManager_MaxExtractTime_InternalState = MENUMANAGER_MAXEXTRACTTIME_INTERNALSTATE_EXITING;

      break;
    }
    case MENUMANAGER_MAXEXTRACTTIME_INTERNALSTATE_EXITING:
    default:
      break;
  }
}

/*=============================================================================================*/
static void MenuManager_MaxExtractTime_SubTickHandler(void)
{
  if (MenuManager_MaxExtractTime_InternalState == MENUMANAGER_MAXEXTRACTTIME_INTERNALSTATE_EXITING)
  {
    MenuManager_MaxExtractTime_Counter += (uint32_t)1U;

    if (MenuManager_MaxExtractTime_Counter >= MENUMANAGER_MAXEXTRACTTIME_COUNTER_MAX)
    {
      MenuManager_MaxExtractTime_Counter = (uint32_t)0U;
      
      Fsm_TriggerEvent(&MenuManager_FsmContext, (tFsmEvent)MENUMANAGER_EVENT_SUBMENU_1);
    }
  }
}



#ifdef  __cplusplus
}
#endif
