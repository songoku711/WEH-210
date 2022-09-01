/* 
 * File:   _1235000000_menu_HighLevel.c
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
#define MENUMANAGER_HIGHLEVEL_INTERNALDATALENGTH                      (uint8_t)10U

#define MENUMANAGER_HIGHLEVEL_INTERNALSTATE_IDX                       0U
#define MENUMANAGER_HIGHLEVEL_COUNTER_IDX                             1U
#define MENUMANAGER_HIGHLEVEL_CURPOS_IDX                              2U
#define MENUMANAGER_HIGHLEVEL_VALUE_MIN_IDX                           3U
#define MENUMANAGER_HIGHLEVEL_VALUE_MAX_IDX                           4U
#define MENUMANAGER_HIGHLEVEL_VALUE_IDX                               5U
#define MENUMANAGER_HIGHLEVEL_UNITVAL_IDX                             6U

#define MenuManager_HighLevel_InternalState                           MenuManager_GetInternalDataPtr(MENUMANAGER_HIGHLEVEL_INTERNALSTATE_IDX)
#define MenuManager_HighLevel_Counter                                 MenuManager_GetInternalDataPtr(MENUMANAGER_HIGHLEVEL_COUNTER_IDX)
#define MenuManager_HighLevel_CurPos                                  MenuManager_GetInternalDataPtr(MENUMANAGER_HIGHLEVEL_CURPOS_IDX)
#define MenuManager_HighLevel_ValueMin                                MenuManager_GetInternalDataPtr(MENUMANAGER_HIGHLEVEL_VALUE_MIN_IDX)
#define MenuManager_HighLevel_ValueMax                                MenuManager_GetInternalDataPtr(MENUMANAGER_HIGHLEVEL_VALUE_MAX_IDX)
#define MenuManager_HighLevel_Value                                   MenuManager_GetInternalDataPtr(MENUMANAGER_HIGHLEVEL_VALUE_IDX)
#define MenuManager_HighLevel_UnitVal(idx)                            MenuManager_GetInternalDataPtr(MENUMANAGER_HIGHLEVEL_UNITVAL_IDX + idx)



#define MENUMANAGER_HIGHLEVEL_INTERNALSTATE_READY                     (uint32_t)0U
#define MENUMANAGER_HIGHLEVEL_INTERNALSTATE_RUNNING                   (uint32_t)1U
#define MENUMANAGER_HIGHLEVEL_INTERNALSTATE_DONE                      (uint32_t)2U
#define MENUMANAGER_HIGHLEVEL_INTERNALSTATE_EXITING                   (uint32_t)3U

#define MENUMANAGER_HIGHLEVEL_COUNTER_MAX                             (uint32_t)50U

#define MENUMANAGER_HIGHLEVEL_CURPOS_XPOS                             (uint32_t)8U
#define MENUMANAGER_HIGHLEVEL_CURPOS_YPOS                             (uint32_t)5U

#define MENUMANAGER_HIGHLEVEL_UNITVAL_LENGTH                          (uint32_t)4U



/** Menu manager main titles and child menu titles */
static const uint8_t MenuManager_HighLevel_MainTitleStr[] =           "ZERO LEVEL";



/** Menu manager button event mapping array */
static MenuManager_ButEventMapStruct MenuManager_HighLevel_ButEventMap[6] =
{
  { IOMANAGER_BUTTON_STATE_START,                                     MENUMANAGER_EVENT_START_BUT             },
  { IOMANAGER_BUTTON_STATE_STOP,                                      MENUMANAGER_EVENT_STOP_BUT              },
  { IOMANAGER_BUTTON_STATE_UP,                                        MENUMANAGER_EVENT_UP_BUT                },
  { IOMANAGER_BUTTON_STATE_DOWN,                                      MENUMANAGER_EVENT_DOWN_BUT              },
  { IOMANAGER_BUTTON_STATE_ADD,                                       MENUMANAGER_EVENT_ADD_BUT               },
  { IOMANAGER_BUTTON_STATE_SUB,                                       MENUMANAGER_EVENT_SUB_BUT               }
};

/** Menu manager button event mapping configuration */
static MenuManager_ButEventMapConfStruct MenuManager_HighLevel_ButEventMapConf =
{
  .butEventMapNum             = (uint8_t)6U,
  .butEventMapCfg             = &MenuManager_HighLevel_ButEventMap
};



/** Menu manager event handlers */
static tFsmGuard MenuManager_HighLevel_Entry                          (tFsmContextPtr const pFsmContext, tFsmEvent event);
static tFsmGuard MenuManager_HighLevel_Exit                           (tFsmContextPtr const pFsmContext, tFsmEvent event);
static tFsmGuard MenuManager_HighLevel_Submenu1                       (tFsmContextPtr const pFsmContext, tFsmEvent event);
static tFsmGuard MenuManager_HighLevel_StartBut                       (tFsmContextPtr const pFsmContext, tFsmEvent event);
static tFsmGuard MenuManager_HighLevel_StopBut                        (tFsmContextPtr const pFsmContext, tFsmEvent event);
static tFsmGuard MenuManager_HighLevel_UpBut                          (tFsmContextPtr const pFsmContext, tFsmEvent event);
static tFsmGuard MenuManager_HighLevel_DownBut                        (tFsmContextPtr const pFsmContext, tFsmEvent event);
static tFsmGuard MenuManager_HighLevel_AddBut                         (tFsmContextPtr const pFsmContext, tFsmEvent event);
static tFsmGuard MenuManager_HighLevel_SubBut                         (tFsmContextPtr const pFsmContext, tFsmEvent event);

/** Menu manager state machine */
tFsmEventEntry MenuManager_HighLevel_StateMachine[9] =
{
  FSM_TRIGGER_ENTRY             (                                     MenuManager_HighLevel_Entry                                                     ),
  FSM_TRIGGER_EXIT              (                                     MenuManager_HighLevel_Exit                                                      ),
  FSM_TRIGGER_TRANSITION_ACTION ( MENUMANAGER_EVENT_SUBMENU_1,        MenuManager_HighLevel_Submenu1,         MENUMANAGER_STATE_FILL_LEVEL_SETUP      ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_START_BUT,        MenuManager_HighLevel_StartBut                                                  ),
  FSM_TRIGGER_TRANSITION_ACTION ( MENUMANAGER_EVENT_STOP_BUT,         MenuManager_HighLevel_StopBut,          MENUMANAGER_STATE_FILL_LEVEL_SETUP      ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_UP_BUT,           MenuManager_HighLevel_UpBut                                                     ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_DOWN_BUT,         MenuManager_HighLevel_DownBut                                                   ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_ADD_BUT,          MenuManager_HighLevel_AddBut                                                    ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_SUB_BUT,          MenuManager_HighLevel_SubBut                                                    )
};



/*===============================================================================================
*                                   LOCAL FUNCTION PROTOTYPES
===============================================================================================*/

static void MenuManager_HighLevel_LcdShowMainTitle(void);
static void MenuManager_HighLevel_LcdShowAdjust(void);
static void MenuManager_HighLevel_LcdShowDone(void);

static void MenuManager_HighLevel_SubMainFunction(void);
static void MenuManager_HighLevel_SubTickHandler(void);



/*===============================================================================================
*                                       LOCAL FUNCTIONS
===============================================================================================*/

static void MenuManager_HighLevel_LcdShowMainTitle(void)
{
  /* Print main title */
  MenuManager_Common_LcdClearAdjustMenuStatic();
  MenuManager_Common_LcdShowMainTitle(MenuManager_HighLevel_MainTitleStr);
  MenuManager_Common_LcdShowOkExitChoice();
  MenuManager_Common_LcdUpdateAdjustMenuStatic();
}

/*=============================================================================================*/
static void MenuManager_HighLevel_LcdShowAdjust(void)
{
  MenuManager_Common_LcdClearAdjustMenuDynamic();

  MenuManager_Common_LcdShowAdjustUnitVal
  (
    MENUMANAGER_HIGHLEVEL_CURPOS_XPOS,
    MENUMANAGER_HIGHLEVEL_CURPOS_YPOS,
    &(MenuManager_HighLevel_UnitVal(0)),
    MENUMANAGER_HIGHLEVEL_UNITVAL_LENGTH
  );
  
  MenuManager_Common_LcdShowAdjustArrow
  (
    MENUMANAGER_HIGHLEVEL_CURPOS_XPOS,
    MENUMANAGER_HIGHLEVEL_CURPOS_YPOS,
    MenuManager_HighLevel_CurPos
  );

  MenuManager_Common_LcdUpdateAdjustMenuDynamic();
}

/*=============================================================================================*/
static void MenuManager_HighLevel_LcdShowDone(void)
{
  MenuManager_Common_LcdClearAll();

  MenuManager_Common_LcdShowAdjustUnitVal
  (
    MENUMANAGER_HIGHLEVEL_CURPOS_XPOS,
    MENUMANAGER_HIGHLEVEL_CURPOS_YPOS,
    &(MenuManager_HighLevel_UnitVal(0)),
    MENUMANAGER_HIGHLEVEL_UNITVAL_LENGTH
  );

  MenuManager_Common_LcdUpdateAll();
}



/*=============================================================================================*/
static tFsmGuard MenuManager_HighLevel_Entry(tFsmContextPtr const pFsmContext, tFsmEvent event)
{
  MenuManager_SubMainFunction = MenuManager_HighLevel_SubMainFunction;
  MenuManager_SubTickHandler = MenuManager_HighLevel_SubTickHandler;

  /* Check if previous state data hierachy is not empty */
  if (pFsmContext->dataHierachy != NULL)
  {
    if (pFsmContext->dataHierachy->dataId == MENUMANAGER_STATE_FILL_LEVEL_SETUP)
    {
      /* Release previous state data hierachy */
      MenuManager_free(pFsmContext->dataHierachy);
      pFsmContext->dataHierachy = NULL;

      MenuManager_InternalDataPush(MENUMANAGER_HIGHLEVEL_INTERNALDATALENGTH);

      MenuManager_HighLevel_InternalState = MENUMANAGER_HIGHLEVEL_INTERNALSTATE_READY;
      MenuManager_HighLevel_Counter = (uint32_t)0U;
      MenuManager_HighLevel_CurPos = (uint32_t)0U;

      MenuManager_HighLevel_ValueMin = (uint32_t)(ProgramManager_gParamConfig.fillLevelCfg.midLevel);
      MenuManager_HighLevel_ValueMax = (uint32_t)(ProgramManager_gParamConfig.fillLevelCfg.overbrimLevel);

      MenuManager_HighLevel_Value = (uint32_t)(ProgramManager_gParamConfig.fillLevelCfg.highLevel);

      MenuManager_Common_DecToBcdConv
      (
        &(MenuManager_HighLevel_Value),
        &(MenuManager_HighLevel_UnitVal(0)),
        (uint8_t)MENUMANAGER_HIGHLEVEL_UNITVAL_LENGTH
      );
    }
    else
    {
      return kFsmGuard_False;
    }

    MenuManager_HighLevel_LcdShowMainTitle();
    MenuManager_HighLevel_LcdShowAdjust();

    return kFsmGuard_True;
  }

  return kFsmGuard_False;
}

/*=============================================================================================*/
static tFsmGuard MenuManager_HighLevel_Exit(tFsmContextPtr const pFsmContext, tFsmEvent event)
{
  MenuManager_SubMainFunction = NULL;
  MenuManager_SubTickHandler = NULL;

  tFsmDataHierachy* dataHierachy;

  dataHierachy = (tFsmDataHierachy *)MenuManager_malloc(sizeof(tFsmDataHierachy));
  dataHierachy->dataId = MENUMANAGER_STATE_HIGH_LEVEL;

  pFsmContext->dataHierachy = dataHierachy;

  /* Free internal data */
  MenuManager_InternalDataPop();
  
  return kFsmGuard_True;
}

/*=============================================================================================*/
static tFsmGuard MenuManager_HighLevel_Submenu1(tFsmContextPtr const pFsmContext, tFsmEvent event)
{
  
  
  return kFsmGuard_True;
}

/*=============================================================================================*/
static tFsmGuard MenuManager_HighLevel_StartBut(tFsmContextPtr const pFsmContext, tFsmEvent event)
{
  if (MenuManager_HighLevel_InternalState == MENUMANAGER_HIGHLEVEL_INTERNALSTATE_READY)
  {
    MenuManager_HighLevel_InternalState = MENUMANAGER_HIGHLEVEL_INTERNALSTATE_RUNNING;
  }
  
  return kFsmGuard_True;
}

/*=============================================================================================*/
static tFsmGuard MenuManager_HighLevel_StopBut(tFsmContextPtr const pFsmContext, tFsmEvent event)
{
  if (MenuManager_HighLevel_InternalState != MENUMANAGER_HIGHLEVEL_INTERNALSTATE_READY)
  {
    return kFsmGuard_False;
  }
  
  return kFsmGuard_True;
}

/*=============================================================================================*/
static tFsmGuard MenuManager_HighLevel_UpBut(tFsmContextPtr const pFsmContext, tFsmEvent event)
{
  uint32_t tempUnitVal;
  uint32_t tempVal;

  if (MenuManager_HighLevel_InternalState == MENUMANAGER_HIGHLEVEL_INTERNALSTATE_READY)
  {
    tempUnitVal = MenuManager_HighLevel_UnitVal(MenuManager_HighLevel_CurPos);

    MenuManager_HighLevel_UnitVal(MenuManager_HighLevel_CurPos) += (uint32_t)1U;

    if (MenuManager_HighLevel_UnitVal(MenuManager_HighLevel_CurPos) > (uint32_t)9U)
    {
      MenuManager_HighLevel_UnitVal(MenuManager_HighLevel_CurPos) = (uint32_t)0U;
    }

    MenuManager_Common_BcdToDecConv(&tempVal, &(MenuManager_HighLevel_UnitVal(0)), (uint8_t)MENUMANAGER_HIGHLEVEL_UNITVAL_LENGTH);

    if ((tempVal <= MenuManager_HighLevel_ValueMin) || (tempVal >= MenuManager_HighLevel_ValueMax))
    {
      MenuManager_HighLevel_UnitVal(MenuManager_HighLevel_CurPos) = tempUnitVal;
    }

    MenuManager_HighLevel_LcdShowAdjust();
  }

  return kFsmGuard_True;
}

/*=============================================================================================*/
static tFsmGuard MenuManager_HighLevel_DownBut(tFsmContextPtr const pFsmContext, tFsmEvent event)
{
  uint32_t tempUnitVal;
  uint32_t tempVal;

  if (MenuManager_HighLevel_InternalState == MENUMANAGER_HIGHLEVEL_INTERNALSTATE_READY)
  {
    tempUnitVal = MenuManager_HighLevel_UnitVal(MenuManager_HighLevel_CurPos);

    MenuManager_HighLevel_UnitVal(MenuManager_HighLevel_CurPos) -= (uint32_t)1U;

    if (MenuManager_HighLevel_UnitVal(MenuManager_HighLevel_CurPos) > (uint32_t)9U)
    {
      MenuManager_HighLevel_UnitVal(MenuManager_HighLevel_CurPos) = (uint32_t)9U;
    }

    MenuManager_Common_BcdToDecConv(&tempVal, &(MenuManager_HighLevel_UnitVal(0)), (uint8_t)MENUMANAGER_HIGHLEVEL_UNITVAL_LENGTH);

    if ((tempVal <= MenuManager_HighLevel_ValueMin) || (tempVal >= MenuManager_HighLevel_ValueMax))
    {
      MenuManager_HighLevel_UnitVal(MenuManager_HighLevel_CurPos) = tempUnitVal;
    }

    MenuManager_HighLevel_LcdShowAdjust();
  }
  
  return kFsmGuard_True;
}

/*=============================================================================================*/
static tFsmGuard MenuManager_HighLevel_AddBut(tFsmContextPtr const pFsmContext, tFsmEvent event)
{
  if (MenuManager_HighLevel_InternalState == MENUMANAGER_HIGHLEVEL_INTERNALSTATE_READY)
  {
    if (MenuManager_HighLevel_CurPos > (uint32_t)0U)
    {
      MenuManager_HighLevel_CurPos -= (uint32_t)1U;
    }

    MenuManager_HighLevel_LcdShowAdjust();
  }
  
  return kFsmGuard_True;
}

/*=============================================================================================*/
static tFsmGuard MenuManager_HighLevel_SubBut(tFsmContextPtr const pFsmContext, tFsmEvent event)
{
  if (MenuManager_HighLevel_InternalState == MENUMANAGER_HIGHLEVEL_INTERNALSTATE_READY)
  {
    if (MenuManager_HighLevel_CurPos < (MENUMANAGER_HIGHLEVEL_UNITVAL_LENGTH - (uint32_t)1U))
    {
      MenuManager_HighLevel_CurPos += (uint32_t)1U;
    }
    
    MenuManager_HighLevel_LcdShowAdjust();
  }
  
  return kFsmGuard_True;
}



/*=============================================================================================*/
static void MenuManager_HighLevel_SubMainFunction(void)
{
  MenuManager_Common_ButEventMapHandler(&MenuManager_HighLevel_ButEventMapConf);

  switch (MenuManager_HighLevel_InternalState)
  {
    case MENUMANAGER_HIGHLEVEL_INTERNALSTATE_READY:
      break;
    case MENUMANAGER_HIGHLEVEL_INTERNALSTATE_RUNNING:
    {
      uint16_t tempHighLevel;

      MenuManager_Common_BcdToDecConv(&(MenuManager_HighLevel_Value), &(MenuManager_HighLevel_UnitVal(0)), (uint8_t)MENUMANAGER_HIGHLEVEL_UNITVAL_LENGTH);

      tempHighLevel = (uint16_t)MenuManager_HighLevel_Value;

      ProgramManager_FillLevelSetup_HighLevel_SetData(&tempHighLevel);

      ProgramManager_FillLevelSetup_HighLevel_GetData(&(ProgramManager_gParamConfig.fillLevelCfg.highLevel));

      MenuManager_HighLevel_InternalState = MENUMANAGER_HIGHLEVEL_INTERNALSTATE_DONE;
      
      break;
    }
    case MENUMANAGER_HIGHLEVEL_INTERNALSTATE_DONE:
    {
      MenuManager_HighLevel_LcdShowDone();

      MenuManager_HighLevel_InternalState = MENUMANAGER_HIGHLEVEL_INTERNALSTATE_EXITING;

      break;
    }
    case MENUMANAGER_HIGHLEVEL_INTERNALSTATE_EXITING:
    default:
      break;
  }
}

/*=============================================================================================*/
static void MenuManager_HighLevel_SubTickHandler(void)
{
  if (MenuManager_HighLevel_InternalState == MENUMANAGER_HIGHLEVEL_INTERNALSTATE_EXITING)
  {
    MenuManager_HighLevel_Counter += (uint32_t)1U;

    if (MenuManager_HighLevel_Counter >= MENUMANAGER_HIGHLEVEL_COUNTER_MAX)
    {
      MenuManager_HighLevel_Counter = (uint32_t)0U;
      
      Fsm_TriggerEvent(&MenuManager_FsmContext, (tFsmEvent)MENUMANAGER_EVENT_SUBMENU_1);
    }
  }
}



#ifdef  __cplusplus
}
#endif
