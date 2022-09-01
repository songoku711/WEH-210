/* 
 * File:   _1236000000_menu_OverbrimLevel.c
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
#define MENUMANAGER_OVERBRIMLEVEL_INTERNALDATALENGTH                  (uint8_t)10U

#define MENUMANAGER_OVERBRIMLEVEL_INTERNALSTATE_IDX                   0U
#define MENUMANAGER_OVERBRIMLEVEL_COUNTER_IDX                         1U
#define MENUMANAGER_OVERBRIMLEVEL_CURPOS_IDX                          2U
#define MENUMANAGER_OVERBRIMLEVEL_VALUE_MIN_IDX                       3U
#define MENUMANAGER_OVERBRIMLEVEL_VALUE_MAX_IDX                       4U
#define MENUMANAGER_OVERBRIMLEVEL_VALUE_IDX                           5U
#define MENUMANAGER_OVERBRIMLEVEL_UNITVAL_IDX                         6U

#define MenuManager_OverbrimLevel_InternalState                       MenuManager_GetInternalDataPtr(MENUMANAGER_OVERBRIMLEVEL_INTERNALSTATE_IDX)
#define MenuManager_OverbrimLevel_Counter                             MenuManager_GetInternalDataPtr(MENUMANAGER_OVERBRIMLEVEL_COUNTER_IDX)
#define MenuManager_OverbrimLevel_CurPos                              MenuManager_GetInternalDataPtr(MENUMANAGER_OVERBRIMLEVEL_CURPOS_IDX)
#define MenuManager_OverbrimLevel_ValueMin                            MenuManager_GetInternalDataPtr(MENUMANAGER_OVERBRIMLEVEL_VALUE_MIN_IDX)
#define MenuManager_OverbrimLevel_ValueMax                            MenuManager_GetInternalDataPtr(MENUMANAGER_OVERBRIMLEVEL_VALUE_MAX_IDX)
#define MenuManager_OverbrimLevel_Value                               MenuManager_GetInternalDataPtr(MENUMANAGER_OVERBRIMLEVEL_VALUE_IDX)
#define MenuManager_OverbrimLevel_UnitVal(idx)                        MenuManager_GetInternalDataPtr(MENUMANAGER_OVERBRIMLEVEL_UNITVAL_IDX + idx)



#define MENUMANAGER_OVERBRIMLEVEL_INTERNALSTATE_READY                 (uint32_t)0U
#define MENUMANAGER_OVERBRIMLEVEL_INTERNALSTATE_RUNNING               (uint32_t)1U
#define MENUMANAGER_OVERBRIMLEVEL_INTERNALSTATE_DONE                  (uint32_t)2U
#define MENUMANAGER_OVERBRIMLEVEL_INTERNALSTATE_EXITING               (uint32_t)3U

#define MENUMANAGER_OVERBRIMLEVEL_COUNTER_MAX                         (uint32_t)50U

#define MENUMANAGER_OVERBRIMLEVEL_CURPOS_XPOS                         (uint32_t)8U
#define MENUMANAGER_OVERBRIMLEVEL_CURPOS_YPOS                         (uint32_t)5U

#define MENUMANAGER_OVERBRIMLEVEL_UNITVAL_LENGTH                      (uint32_t)4U



/** Menu manager main titles and child menu titles */
static const uint8_t MenuManager_OverbrimLevel_MainTitleStr[] =       "OVERBRIM LEVEL";



/** Menu manager button event mapping array */
static MenuManager_ButEventMapStruct MenuManager_OverbrimLevel_ButEventMap[6] =
{
  { IOMANAGER_BUTTON_STATE_START,                                     MENUMANAGER_EVENT_START_BUT             },
  { IOMANAGER_BUTTON_STATE_STOP,                                      MENUMANAGER_EVENT_STOP_BUT              },
  { IOMANAGER_BUTTON_STATE_UP,                                        MENUMANAGER_EVENT_UP_BUT                },
  { IOMANAGER_BUTTON_STATE_DOWN,                                      MENUMANAGER_EVENT_DOWN_BUT              },
  { IOMANAGER_BUTTON_STATE_ADD,                                       MENUMANAGER_EVENT_ADD_BUT               },
  { IOMANAGER_BUTTON_STATE_SUB,                                       MENUMANAGER_EVENT_SUB_BUT               }
};

/** Menu manager button event mapping configuration */
static MenuManager_ButEventMapConfStruct MenuManager_OverbrimLevel_ButEventMapConf =
{
  .butEventMapNum             = (uint8_t)6U,
  .butEventMapCfg             = &MenuManager_OverbrimLevel_ButEventMap
};



/** Menu manager event handlers */
static tFsmGuard MenuManager_OverbrimLevel_Entry                      (tFsmContextPtr const pFsmContext, tFsmEvent event);
static tFsmGuard MenuManager_OverbrimLevel_Exit                       (tFsmContextPtr const pFsmContext, tFsmEvent event);
static tFsmGuard MenuManager_OverbrimLevel_Submenu1                   (tFsmContextPtr const pFsmContext, tFsmEvent event);
static tFsmGuard MenuManager_OverbrimLevel_StartBut                   (tFsmContextPtr const pFsmContext, tFsmEvent event);
static tFsmGuard MenuManager_OverbrimLevel_StopBut                    (tFsmContextPtr const pFsmContext, tFsmEvent event);
static tFsmGuard MenuManager_OverbrimLevel_UpBut                      (tFsmContextPtr const pFsmContext, tFsmEvent event);
static tFsmGuard MenuManager_OverbrimLevel_DownBut                    (tFsmContextPtr const pFsmContext, tFsmEvent event);
static tFsmGuard MenuManager_OverbrimLevel_AddBut                     (tFsmContextPtr const pFsmContext, tFsmEvent event);
static tFsmGuard MenuManager_OverbrimLevel_SubBut                     (tFsmContextPtr const pFsmContext, tFsmEvent event);

/** Menu manager state machine */
tFsmEventEntry MenuManager_OverbrimLevel_StateMachine[9] =
{
  FSM_TRIGGER_ENTRY             (                                     MenuManager_OverbrimLevel_Entry                                                 ),
  FSM_TRIGGER_EXIT              (                                     MenuManager_OverbrimLevel_Exit                                                  ),
  FSM_TRIGGER_TRANSITION_ACTION ( MENUMANAGER_EVENT_SUBMENU_1,        MenuManager_OverbrimLevel_Submenu1,     MENUMANAGER_STATE_FILL_LEVEL_SETUP      ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_START_BUT,        MenuManager_OverbrimLevel_StartBut                                              ),
  FSM_TRIGGER_TRANSITION_ACTION ( MENUMANAGER_EVENT_STOP_BUT,         MenuManager_OverbrimLevel_StopBut,      MENUMANAGER_STATE_FILL_LEVEL_SETUP      ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_UP_BUT,           MenuManager_OverbrimLevel_UpBut                                                 ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_DOWN_BUT,         MenuManager_OverbrimLevel_DownBut                                               ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_ADD_BUT,          MenuManager_OverbrimLevel_AddBut                                                ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_SUB_BUT,          MenuManager_OverbrimLevel_SubBut                                                )
};



/*===============================================================================================
*                                   LOCAL FUNCTION PROTOTYPES
===============================================================================================*/

static void MenuManager_OverbrimLevel_LcdShowMainTitle(void);
static void MenuManager_OverbrimLevel_LcdShowAdjust(void);
static void MenuManager_OverbrimLevel_LcdShowDone(void);

static void MenuManager_OverbrimLevel_SubMainFunction(void);
static void MenuManager_OverbrimLevel_SubTickHandler(void);



/*===============================================================================================
*                                       LOCAL FUNCTIONS
===============================================================================================*/

static void MenuManager_OverbrimLevel_LcdShowMainTitle(void)
{
  /* Print main title */
  MenuManager_Common_LcdClearAdjustMenuStatic();
  MenuManager_Common_LcdShowMainTitle(MenuManager_OverbrimLevel_MainTitleStr);
  MenuManager_Common_LcdShowOkExitChoice();
  MenuManager_Common_LcdUpdateAdjustMenuStatic();
}

/*=============================================================================================*/
static void MenuManager_OverbrimLevel_LcdShowAdjust(void)
{
  MenuManager_Common_LcdClearAdjustMenuDynamic();

  MenuManager_Common_LcdShowAdjustUnitVal
  (
    MENUMANAGER_OVERBRIMLEVEL_CURPOS_XPOS,
    MENUMANAGER_OVERBRIMLEVEL_CURPOS_YPOS,
    &(MenuManager_OverbrimLevel_UnitVal(0)),
    MENUMANAGER_OVERBRIMLEVEL_UNITVAL_LENGTH
  );
  
  MenuManager_Common_LcdShowAdjustArrow
  (
    MENUMANAGER_OVERBRIMLEVEL_CURPOS_XPOS,
    MENUMANAGER_OVERBRIMLEVEL_CURPOS_YPOS,
    MenuManager_OverbrimLevel_CurPos
  );

  MenuManager_Common_LcdUpdateAdjustMenuDynamic();
}

/*=============================================================================================*/
static void MenuManager_OverbrimLevel_LcdShowDone(void)
{
  MenuManager_Common_LcdClearAll();

  MenuManager_Common_LcdShowAdjustUnitVal
  (
    MENUMANAGER_OVERBRIMLEVEL_CURPOS_XPOS,
    MENUMANAGER_OVERBRIMLEVEL_CURPOS_YPOS,
    &(MenuManager_OverbrimLevel_UnitVal(0)),
    MENUMANAGER_OVERBRIMLEVEL_UNITVAL_LENGTH
  );

  MenuManager_Common_LcdUpdateAll();
}



/*=============================================================================================*/
static tFsmGuard MenuManager_OverbrimLevel_Entry(tFsmContextPtr const pFsmContext, tFsmEvent event)
{
  MenuManager_SubMainFunction = MenuManager_OverbrimLevel_SubMainFunction;
  MenuManager_SubTickHandler = MenuManager_OverbrimLevel_SubTickHandler;

  /* Check if previous state data hierachy is not empty */
  if (pFsmContext->dataHierachy != NULL)
  {
    if (pFsmContext->dataHierachy->dataId == MENUMANAGER_STATE_FILL_LEVEL_SETUP)
    {
      /* Release previous state data hierachy */
      MenuManager_free(pFsmContext->dataHierachy);
      pFsmContext->dataHierachy = NULL;

      MenuManager_InternalDataPush(MENUMANAGER_OVERBRIMLEVEL_INTERNALDATALENGTH);

      MenuManager_OverbrimLevel_InternalState = MENUMANAGER_OVERBRIMLEVEL_INTERNALSTATE_READY;
      MenuManager_OverbrimLevel_Counter = (uint32_t)0U;
      MenuManager_OverbrimLevel_CurPos = (uint32_t)0U;

      MenuManager_OverbrimLevel_ValueMin = (uint32_t)(ProgramManager_gParamConfig.fillLevelCfg.highLevel);
      MenuManager_OverbrimLevel_ValueMax = (uint32_t)PROGRAMMANAGER_FILLLEVELSETUP_WATER_LEVEL_MAX;

      MenuManager_OverbrimLevel_Value = (uint32_t)(ProgramManager_gParamConfig.fillLevelCfg.overbrimLevel);

      MenuManager_Common_DecToBcdConv
      (
        &(MenuManager_OverbrimLevel_Value),
        &(MenuManager_OverbrimLevel_UnitVal(0)),
        (uint8_t)MENUMANAGER_OVERBRIMLEVEL_UNITVAL_LENGTH
      );
    }
    else
    {
      return kFsmGuard_False;
    }

    MenuManager_OverbrimLevel_LcdShowMainTitle();
    MenuManager_OverbrimLevel_LcdShowAdjust();

    return kFsmGuard_True;
  }

  return kFsmGuard_False;
}

/*=============================================================================================*/
static tFsmGuard MenuManager_OverbrimLevel_Exit(tFsmContextPtr const pFsmContext, tFsmEvent event)
{
  MenuManager_SubMainFunction = NULL;
  MenuManager_SubTickHandler = NULL;

  tFsmDataHierachy* dataHierachy;

  dataHierachy = (tFsmDataHierachy *)MenuManager_malloc(sizeof(tFsmDataHierachy));
  dataHierachy->dataId = MENUMANAGER_STATE_OVERBRIM_LEVEL;

  pFsmContext->dataHierachy = dataHierachy;

  /* Free internal data */
  MenuManager_InternalDataPop();
  
  return kFsmGuard_True;
}

/*=============================================================================================*/
static tFsmGuard MenuManager_OverbrimLevel_Submenu1(tFsmContextPtr const pFsmContext, tFsmEvent event)
{
  
  
  return kFsmGuard_True;
}

/*=============================================================================================*/
static tFsmGuard MenuManager_OverbrimLevel_StartBut(tFsmContextPtr const pFsmContext, tFsmEvent event)
{
  if (MenuManager_OverbrimLevel_InternalState == MENUMANAGER_OVERBRIMLEVEL_INTERNALSTATE_READY)
  {
    MenuManager_OverbrimLevel_InternalState = MENUMANAGER_OVERBRIMLEVEL_INTERNALSTATE_RUNNING;
  }
  
  return kFsmGuard_True;
}

/*=============================================================================================*/
static tFsmGuard MenuManager_OverbrimLevel_StopBut(tFsmContextPtr const pFsmContext, tFsmEvent event)
{
  if (MenuManager_OverbrimLevel_InternalState != MENUMANAGER_OVERBRIMLEVEL_INTERNALSTATE_READY)
  {
    return kFsmGuard_False;
  }
  
  return kFsmGuard_True;
}

/*=============================================================================================*/
static tFsmGuard MenuManager_OverbrimLevel_UpBut(tFsmContextPtr const pFsmContext, tFsmEvent event)
{
  uint32_t tempUnitVal;
  uint32_t tempVal;

  if (MenuManager_OverbrimLevel_InternalState == MENUMANAGER_OVERBRIMLEVEL_INTERNALSTATE_READY)
  {
    tempUnitVal = MenuManager_OverbrimLevel_UnitVal(MenuManager_OverbrimLevel_CurPos);

    MenuManager_OverbrimLevel_UnitVal(MenuManager_OverbrimLevel_CurPos) += (uint32_t)1U;

    if (MenuManager_OverbrimLevel_UnitVal(MenuManager_OverbrimLevel_CurPos) > (uint32_t)9U)
    {
      MenuManager_OverbrimLevel_UnitVal(MenuManager_OverbrimLevel_CurPos) = (uint32_t)0U;
    }

    MenuManager_Common_BcdToDecConv(&tempVal, &(MenuManager_OverbrimLevel_UnitVal(0)), (uint8_t)MENUMANAGER_OVERBRIMLEVEL_UNITVAL_LENGTH);

    if ((tempVal <= MenuManager_OverbrimLevel_ValueMin) || (tempVal >= MenuManager_OverbrimLevel_ValueMax))
    {
      MenuManager_OverbrimLevel_UnitVal(MenuManager_OverbrimLevel_CurPos) = tempUnitVal;
    }

    MenuManager_OverbrimLevel_LcdShowAdjust();
  }

  return kFsmGuard_True;
}

/*=============================================================================================*/
static tFsmGuard MenuManager_OverbrimLevel_DownBut(tFsmContextPtr const pFsmContext, tFsmEvent event)
{
  uint32_t tempUnitVal;
  uint32_t tempVal;

  if (MenuManager_OverbrimLevel_InternalState == MENUMANAGER_OVERBRIMLEVEL_INTERNALSTATE_READY)
  {
    tempUnitVal = MenuManager_OverbrimLevel_UnitVal(MenuManager_OverbrimLevel_CurPos);

    MenuManager_OverbrimLevel_UnitVal(MenuManager_OverbrimLevel_CurPos) -= (uint32_t)1U;

    if (MenuManager_OverbrimLevel_UnitVal(MenuManager_OverbrimLevel_CurPos) > (uint32_t)9U)
    {
      MenuManager_OverbrimLevel_UnitVal(MenuManager_OverbrimLevel_CurPos) = (uint32_t)9U;
    }

    MenuManager_Common_BcdToDecConv(&tempVal, &(MenuManager_OverbrimLevel_UnitVal(0)), (uint8_t)MENUMANAGER_OVERBRIMLEVEL_UNITVAL_LENGTH);

    if ((tempVal <= MenuManager_OverbrimLevel_ValueMin) || (tempVal >= MenuManager_OverbrimLevel_ValueMax))
    {
      MenuManager_OverbrimLevel_UnitVal(MenuManager_OverbrimLevel_CurPos) = tempUnitVal;
    }

    MenuManager_OverbrimLevel_LcdShowAdjust();
  }
  
  return kFsmGuard_True;
}

/*=============================================================================================*/
static tFsmGuard MenuManager_OverbrimLevel_AddBut(tFsmContextPtr const pFsmContext, tFsmEvent event)
{
  if (MenuManager_OverbrimLevel_InternalState == MENUMANAGER_OVERBRIMLEVEL_INTERNALSTATE_READY)
  {
    if (MenuManager_OverbrimLevel_CurPos > (uint32_t)0U)
    {
      MenuManager_OverbrimLevel_CurPos -= (uint32_t)1U;
    }

    MenuManager_OverbrimLevel_LcdShowAdjust();
  }
  
  return kFsmGuard_True;
}

/*=============================================================================================*/
static tFsmGuard MenuManager_OverbrimLevel_SubBut(tFsmContextPtr const pFsmContext, tFsmEvent event)
{
  if (MenuManager_OverbrimLevel_InternalState == MENUMANAGER_OVERBRIMLEVEL_INTERNALSTATE_READY)
  {
    if (MenuManager_OverbrimLevel_CurPos < (MENUMANAGER_OVERBRIMLEVEL_UNITVAL_LENGTH - (uint32_t)1U))
    {
      MenuManager_OverbrimLevel_CurPos += (uint32_t)1U;
    }
    
    MenuManager_OverbrimLevel_LcdShowAdjust();
  }
  
  return kFsmGuard_True;
}



/*=============================================================================================*/
static void MenuManager_OverbrimLevel_SubMainFunction(void)
{
  MenuManager_Common_ButEventMapHandler(&MenuManager_OverbrimLevel_ButEventMapConf);

  switch (MenuManager_OverbrimLevel_InternalState)
  {
    case MENUMANAGER_OVERBRIMLEVEL_INTERNALSTATE_READY:
      break;
    case MENUMANAGER_OVERBRIMLEVEL_INTERNALSTATE_RUNNING:
    {
      uint16_t tempOverbrimLevel;

      MenuManager_Common_BcdToDecConv(&(MenuManager_OverbrimLevel_Value), &(MenuManager_OverbrimLevel_UnitVal(0)), (uint8_t)MENUMANAGER_OVERBRIMLEVEL_UNITVAL_LENGTH);

      tempOverbrimLevel = (uint16_t)MenuManager_OverbrimLevel_Value;

      ProgramManager_FillLevelSetup_OverbrimLevel_SetData(&tempOverbrimLevel);

      ProgramManager_FillLevelSetup_OverbrimLevel_GetData(&(ProgramManager_gParamConfig.fillLevelCfg.overbrimLevel));

      MenuManager_OverbrimLevel_InternalState = MENUMANAGER_OVERBRIMLEVEL_INTERNALSTATE_DONE;
      
      break;
    }
    case MENUMANAGER_OVERBRIMLEVEL_INTERNALSTATE_DONE:
    {
      MenuManager_OverbrimLevel_LcdShowDone();

      MenuManager_OverbrimLevel_InternalState = MENUMANAGER_OVERBRIMLEVEL_INTERNALSTATE_EXITING;

      break;
    }
    case MENUMANAGER_OVERBRIMLEVEL_INTERNALSTATE_EXITING:
    default:
      break;
  }
}

/*=============================================================================================*/
static void MenuManager_OverbrimLevel_SubTickHandler(void)
{
  if (MenuManager_OverbrimLevel_InternalState == MENUMANAGER_OVERBRIMLEVEL_INTERNALSTATE_EXITING)
  {
    MenuManager_OverbrimLevel_Counter += (uint32_t)1U;

    if (MenuManager_OverbrimLevel_Counter >= MENUMANAGER_OVERBRIMLEVEL_COUNTER_MAX)
    {
      MenuManager_OverbrimLevel_Counter = (uint32_t)0U;
      
      Fsm_TriggerEvent(&MenuManager_FsmContext, (tFsmEvent)MENUMANAGER_EVENT_SUBMENU_1);
    }
  }
}



#ifdef  __cplusplus
}
#endif
