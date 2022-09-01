/* 
 * File:   _1232000000_menu_ZeroLevel.c
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
#define MENUMANAGER_ZEROLEVEL_INTERNALDATALENGTH                      (uint8_t)10U

#define MENUMANAGER_ZEROLEVEL_INTERNALSTATE_IDX                       0U
#define MENUMANAGER_ZEROLEVEL_COUNTER_IDX                             1U
#define MENUMANAGER_ZEROLEVEL_CURPOS_IDX                              2U
#define MENUMANAGER_ZEROLEVEL_VALUE_MIN_IDX                           3U
#define MENUMANAGER_ZEROLEVEL_VALUE_MAX_IDX                           4U
#define MENUMANAGER_ZEROLEVEL_VALUE_IDX                               5U
#define MENUMANAGER_ZEROLEVEL_UNITVAL_IDX                             6U

#define MenuManager_ZeroLevel_InternalState                           MenuManager_GetInternalDataPtr(MENUMANAGER_ZEROLEVEL_INTERNALSTATE_IDX)
#define MenuManager_ZeroLevel_Counter                                 MenuManager_GetInternalDataPtr(MENUMANAGER_ZEROLEVEL_COUNTER_IDX)
#define MenuManager_ZeroLevel_CurPos                                  MenuManager_GetInternalDataPtr(MENUMANAGER_ZEROLEVEL_CURPOS_IDX)
#define MenuManager_ZeroLevel_ValueMin                                MenuManager_GetInternalDataPtr(MENUMANAGER_ZEROLEVEL_VALUE_MIN_IDX)
#define MenuManager_ZeroLevel_ValueMax                                MenuManager_GetInternalDataPtr(MENUMANAGER_ZEROLEVEL_VALUE_MAX_IDX)
#define MenuManager_ZeroLevel_Value                                   MenuManager_GetInternalDataPtr(MENUMANAGER_ZEROLEVEL_VALUE_IDX)
#define MenuManager_ZeroLevel_UnitVal(idx)                            MenuManager_GetInternalDataPtr(MENUMANAGER_ZEROLEVEL_UNITVAL_IDX + idx)



#define MENUMANAGER_ZEROLEVEL_INTERNALSTATE_READY                     (uint32_t)0U
#define MENUMANAGER_ZEROLEVEL_INTERNALSTATE_RUNNING                   (uint32_t)1U
#define MENUMANAGER_ZEROLEVEL_INTERNALSTATE_DONE                      (uint32_t)2U
#define MENUMANAGER_ZEROLEVEL_INTERNALSTATE_EXITING                   (uint32_t)3U

#define MENUMANAGER_ZEROLEVEL_COUNTER_MAX                             (uint32_t)50U

#define MENUMANAGER_ZEROLEVEL_CURPOS_XPOS                             (uint32_t)8U
#define MENUMANAGER_ZEROLEVEL_CURPOS_YPOS                             (uint32_t)5U

#define MENUMANAGER_ZEROLEVEL_UNITVAL_LENGTH                          (uint32_t)4U



/** Menu manager main titles and child menu titles */
static const uint8_t MenuManager_ZeroLevel_MainTitleStr[] =           "ZERO LEVEL";



/** Menu manager button event mapping array */
static MenuManager_ButEventMapStruct MenuManager_ZeroLevel_ButEventMap[6] =
{
  { IOMANAGER_BUTTON_STATE_START,                                     MENUMANAGER_EVENT_START_BUT             },
  { IOMANAGER_BUTTON_STATE_STOP,                                      MENUMANAGER_EVENT_STOP_BUT              },
  { IOMANAGER_BUTTON_STATE_UP,                                        MENUMANAGER_EVENT_UP_BUT                },
  { IOMANAGER_BUTTON_STATE_DOWN,                                      MENUMANAGER_EVENT_DOWN_BUT              },
  { IOMANAGER_BUTTON_STATE_ADD,                                       MENUMANAGER_EVENT_ADD_BUT               },
  { IOMANAGER_BUTTON_STATE_SUB,                                       MENUMANAGER_EVENT_SUB_BUT               }
};

/** Menu manager button event mapping configuration */
static MenuManager_ButEventMapConfStruct MenuManager_ZeroLevel_ButEventMapConf =
{
  .butEventMapNum             = (uint8_t)6U,
  .butEventMapCfg             = &MenuManager_ZeroLevel_ButEventMap
};



/** Menu manager event handlers */
static tFsmGuard MenuManager_ZeroLevel_Entry                          (tFsmContextPtr const pFsmContext, tFsmEvent event);
static tFsmGuard MenuManager_ZeroLevel_Exit                           (tFsmContextPtr const pFsmContext, tFsmEvent event);
static tFsmGuard MenuManager_ZeroLevel_Submenu1                       (tFsmContextPtr const pFsmContext, tFsmEvent event);
static tFsmGuard MenuManager_ZeroLevel_StartBut                       (tFsmContextPtr const pFsmContext, tFsmEvent event);
static tFsmGuard MenuManager_ZeroLevel_StopBut                        (tFsmContextPtr const pFsmContext, tFsmEvent event);
static tFsmGuard MenuManager_ZeroLevel_UpBut                          (tFsmContextPtr const pFsmContext, tFsmEvent event);
static tFsmGuard MenuManager_ZeroLevel_DownBut                        (tFsmContextPtr const pFsmContext, tFsmEvent event);
static tFsmGuard MenuManager_ZeroLevel_AddBut                         (tFsmContextPtr const pFsmContext, tFsmEvent event);
static tFsmGuard MenuManager_ZeroLevel_SubBut                         (tFsmContextPtr const pFsmContext, tFsmEvent event);

/** Menu manager state machine */
tFsmEventEntry MenuManager_ZeroLevel_StateMachine[9] =
{
  FSM_TRIGGER_ENTRY             (                                     MenuManager_ZeroLevel_Entry                                                     ),
  FSM_TRIGGER_EXIT              (                                     MenuManager_ZeroLevel_Exit                                                      ),
  FSM_TRIGGER_TRANSITION_ACTION ( MENUMANAGER_EVENT_SUBMENU_1,        MenuManager_ZeroLevel_Submenu1,         MENUMANAGER_STATE_FILL_LEVEL_SETUP      ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_START_BUT,        MenuManager_ZeroLevel_StartBut                                                  ),
  FSM_TRIGGER_TRANSITION_ACTION ( MENUMANAGER_EVENT_STOP_BUT,         MenuManager_ZeroLevel_StopBut,          MENUMANAGER_STATE_FILL_LEVEL_SETUP      ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_UP_BUT,           MenuManager_ZeroLevel_UpBut                                                     ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_DOWN_BUT,         MenuManager_ZeroLevel_DownBut                                                   ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_ADD_BUT,          MenuManager_ZeroLevel_AddBut                                                    ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_SUB_BUT,          MenuManager_ZeroLevel_SubBut                                                    )
};



/*===============================================================================================
*                                   LOCAL FUNCTION PROTOTYPES
===============================================================================================*/

static void MenuManager_ZeroLevel_LcdShowMainTitle(void);
static void MenuManager_ZeroLevel_LcdShowAdjust(void);
static void MenuManager_ZeroLevel_LcdShowDone(void);

static void MenuManager_ZeroLevel_SubMainFunction(void);
static void MenuManager_ZeroLevel_SubTickHandler(void);



/*===============================================================================================
*                                       LOCAL FUNCTIONS
===============================================================================================*/

static void MenuManager_ZeroLevel_LcdShowMainTitle(void)
{
  /* Print main title */
  MenuManager_Common_LcdClearAdjustMenuStatic();
  MenuManager_Common_LcdShowMainTitle(MenuManager_ZeroLevel_MainTitleStr);
  MenuManager_Common_LcdShowOkExitChoice();
  MenuManager_Common_LcdUpdateAdjustMenuStatic();
}

/*=============================================================================================*/
static void MenuManager_ZeroLevel_LcdShowAdjust(void)
{
  MenuManager_Common_LcdClearAdjustMenuDynamic();

  MenuManager_Common_LcdShowAdjustUnitVal
  (
    MENUMANAGER_ZEROLEVEL_CURPOS_XPOS,
    MENUMANAGER_ZEROLEVEL_CURPOS_YPOS,
    &(MenuManager_ZeroLevel_UnitVal(0)),
    MENUMANAGER_ZEROLEVEL_UNITVAL_LENGTH
  );
  
  MenuManager_Common_LcdShowAdjustArrow
  (
    MENUMANAGER_ZEROLEVEL_CURPOS_XPOS,
    MENUMANAGER_ZEROLEVEL_CURPOS_YPOS,
    MenuManager_ZeroLevel_CurPos
  );

  MenuManager_Common_LcdUpdateAdjustMenuDynamic();
}

/*=============================================================================================*/
static void MenuManager_ZeroLevel_LcdShowDone(void)
{
  MenuManager_Common_LcdClearAll();

  MenuManager_Common_LcdShowAdjustUnitVal
  (
    MENUMANAGER_ZEROLEVEL_CURPOS_XPOS,
    MENUMANAGER_ZEROLEVEL_CURPOS_YPOS,
    &(MenuManager_ZeroLevel_UnitVal(0)),
    MENUMANAGER_ZEROLEVEL_UNITVAL_LENGTH
  );

  MenuManager_Common_LcdUpdateAll();
}



/*=============================================================================================*/
static tFsmGuard MenuManager_ZeroLevel_Entry(tFsmContextPtr const pFsmContext, tFsmEvent event)
{
  MenuManager_SubMainFunction = MenuManager_ZeroLevel_SubMainFunction;
  MenuManager_SubTickHandler = MenuManager_ZeroLevel_SubTickHandler;

  /* Check if previous state data hierachy is not empty */
  if (pFsmContext->dataHierachy != NULL)
  {
    if (pFsmContext->dataHierachy->dataId == MENUMANAGER_STATE_FILL_LEVEL_SETUP)
    {
      /* Release previous state data hierachy */
      MenuManager_free(pFsmContext->dataHierachy);
      pFsmContext->dataHierachy = NULL;

      MenuManager_InternalDataPush(MENUMANAGER_ZEROLEVEL_INTERNALDATALENGTH);

      MenuManager_ZeroLevel_InternalState = MENUMANAGER_ZEROLEVEL_INTERNALSTATE_READY;
      MenuManager_ZeroLevel_Counter = (uint32_t)0U;
      MenuManager_ZeroLevel_CurPos = (uint32_t)0U;

      MenuManager_ZeroLevel_ValueMin = (uint32_t)0U;
      MenuManager_ZeroLevel_ValueMax = (uint32_t)(ProgramManager_gParamConfig.fillLevelCfg.lowLevel);

      MenuManager_ZeroLevel_Value = (uint32_t)(ProgramManager_gParamConfig.fillLevelCfg.zeroLevel);

      MenuManager_Common_DecToBcdConv
      (
        &(MenuManager_ZeroLevel_Value),
        &(MenuManager_ZeroLevel_UnitVal(0)),
        (uint8_t)MENUMANAGER_ZEROLEVEL_UNITVAL_LENGTH
      );
    }
    else
    {
      return kFsmGuard_False;
    }

    MenuManager_ZeroLevel_LcdShowMainTitle();
    MenuManager_ZeroLevel_LcdShowAdjust();

    return kFsmGuard_True;
  }

  return kFsmGuard_False;
}

/*=============================================================================================*/
static tFsmGuard MenuManager_ZeroLevel_Exit(tFsmContextPtr const pFsmContext, tFsmEvent event)
{
  MenuManager_SubMainFunction = NULL;
  MenuManager_SubTickHandler = NULL;

  tFsmDataHierachy* dataHierachy;

  dataHierachy = (tFsmDataHierachy *)MenuManager_malloc(sizeof(tFsmDataHierachy));
  dataHierachy->dataId = MENUMANAGER_STATE_ZERO_LEVEL;

  pFsmContext->dataHierachy = dataHierachy;

  /* Free internal data */
  MenuManager_InternalDataPop();
  
  return kFsmGuard_True;
}

/*=============================================================================================*/
static tFsmGuard MenuManager_ZeroLevel_Submenu1(tFsmContextPtr const pFsmContext, tFsmEvent event)
{
  
  
  return kFsmGuard_True;
}

/*=============================================================================================*/
static tFsmGuard MenuManager_ZeroLevel_StartBut(tFsmContextPtr const pFsmContext, tFsmEvent event)
{
  if (MenuManager_ZeroLevel_InternalState == MENUMANAGER_ZEROLEVEL_INTERNALSTATE_READY)
  {
    MenuManager_ZeroLevel_InternalState = MENUMANAGER_ZEROLEVEL_INTERNALSTATE_RUNNING;
  }
  
  return kFsmGuard_True;
}

/*=============================================================================================*/
static tFsmGuard MenuManager_ZeroLevel_StopBut(tFsmContextPtr const pFsmContext, tFsmEvent event)
{
  if (MenuManager_ZeroLevel_InternalState != MENUMANAGER_ZEROLEVEL_INTERNALSTATE_READY)
  {
    return kFsmGuard_False;
  }
  
  return kFsmGuard_True;
}

/*=============================================================================================*/
static tFsmGuard MenuManager_ZeroLevel_UpBut(tFsmContextPtr const pFsmContext, tFsmEvent event)
{
  uint32_t tempUnitVal;
  uint32_t tempVal;

  if (MenuManager_ZeroLevel_InternalState == MENUMANAGER_ZEROLEVEL_INTERNALSTATE_READY)
  {
    tempUnitVal = MenuManager_ZeroLevel_UnitVal(MenuManager_ZeroLevel_CurPos);

    MenuManager_ZeroLevel_UnitVal(MenuManager_ZeroLevel_CurPos) += (uint32_t)1U;

    if (MenuManager_ZeroLevel_UnitVal(MenuManager_ZeroLevel_CurPos) > (uint32_t)9U)
    {
      MenuManager_ZeroLevel_UnitVal(MenuManager_ZeroLevel_CurPos) = (uint32_t)0U;
    }

    MenuManager_Common_BcdToDecConv(&tempVal, &(MenuManager_ZeroLevel_UnitVal(0)), (uint8_t)MENUMANAGER_ZEROLEVEL_UNITVAL_LENGTH);

    if ((tempVal <= MenuManager_ZeroLevel_ValueMin) || (tempVal >= MenuManager_ZeroLevel_ValueMax))
    {
      MenuManager_ZeroLevel_UnitVal(MenuManager_ZeroLevel_CurPos) = tempUnitVal;
    }

    MenuManager_ZeroLevel_LcdShowAdjust();
  }

  return kFsmGuard_True;
}

/*=============================================================================================*/
static tFsmGuard MenuManager_ZeroLevel_DownBut(tFsmContextPtr const pFsmContext, tFsmEvent event)
{
  uint32_t tempUnitVal;
  uint32_t tempVal;

  if (MenuManager_ZeroLevel_InternalState == MENUMANAGER_ZEROLEVEL_INTERNALSTATE_READY)
  {
    tempUnitVal = MenuManager_ZeroLevel_UnitVal(MenuManager_ZeroLevel_CurPos);

    MenuManager_ZeroLevel_UnitVal(MenuManager_ZeroLevel_CurPos) -= (uint32_t)1U;

    if (MenuManager_ZeroLevel_UnitVal(MenuManager_ZeroLevel_CurPos) > (uint32_t)9U)
    {
      MenuManager_ZeroLevel_UnitVal(MenuManager_ZeroLevel_CurPos) = (uint32_t)9U;
    }

    MenuManager_Common_BcdToDecConv(&tempVal, &(MenuManager_ZeroLevel_UnitVal(0)), (uint8_t)MENUMANAGER_ZEROLEVEL_UNITVAL_LENGTH);

    if ((tempVal <= MenuManager_ZeroLevel_ValueMin) || (tempVal >= MenuManager_ZeroLevel_ValueMax))
    {
      MenuManager_ZeroLevel_UnitVal(MenuManager_ZeroLevel_CurPos) = tempUnitVal;
    }

    MenuManager_ZeroLevel_LcdShowAdjust();
  }
  
  return kFsmGuard_True;
}

/*=============================================================================================*/
static tFsmGuard MenuManager_ZeroLevel_AddBut(tFsmContextPtr const pFsmContext, tFsmEvent event)
{
  if (MenuManager_ZeroLevel_InternalState == MENUMANAGER_ZEROLEVEL_INTERNALSTATE_READY)
  {
    if (MenuManager_ZeroLevel_CurPos > (uint32_t)0U)
    {
      MenuManager_ZeroLevel_CurPos -= (uint32_t)1U;
    }

    MenuManager_ZeroLevel_LcdShowAdjust();
  }
  
  return kFsmGuard_True;
}

/*=============================================================================================*/
static tFsmGuard MenuManager_ZeroLevel_SubBut(tFsmContextPtr const pFsmContext, tFsmEvent event)
{
  if (MenuManager_ZeroLevel_InternalState == MENUMANAGER_ZEROLEVEL_INTERNALSTATE_READY)
  {
    if (MenuManager_ZeroLevel_CurPos < (MENUMANAGER_ZEROLEVEL_UNITVAL_LENGTH - (uint32_t)1U))
    {
      MenuManager_ZeroLevel_CurPos += (uint32_t)1U;
    }
    
    MenuManager_ZeroLevel_LcdShowAdjust();
  }
  
  return kFsmGuard_True;
}



/*=============================================================================================*/
static void MenuManager_ZeroLevel_SubMainFunction(void)
{
  MenuManager_Common_ButEventMapHandler(&MenuManager_ZeroLevel_ButEventMapConf);

  switch (MenuManager_ZeroLevel_InternalState)
  {
    case MENUMANAGER_ZEROLEVEL_INTERNALSTATE_READY:
      break;
    case MENUMANAGER_ZEROLEVEL_INTERNALSTATE_RUNNING:
    {
      uint16_t tempZeroLevel;

      MenuManager_Common_BcdToDecConv(&(MenuManager_ZeroLevel_Value), &(MenuManager_ZeroLevel_UnitVal(0)), (uint8_t)MENUMANAGER_ZEROLEVEL_UNITVAL_LENGTH);

      tempZeroLevel = (uint16_t)MenuManager_ZeroLevel_Value;

      ProgramManager_FillLevelSetup_ZeroLevel_SetData(&tempZeroLevel);

      ProgramManager_FillLevelSetup_ZeroLevel_GetData(&(ProgramManager_gParamConfig.fillLevelCfg.zeroLevel));

      MenuManager_ZeroLevel_InternalState = MENUMANAGER_ZEROLEVEL_INTERNALSTATE_DONE;
      
      break;
    }
    case MENUMANAGER_ZEROLEVEL_INTERNALSTATE_DONE:
    {
      MenuManager_ZeroLevel_LcdShowDone();

      MenuManager_ZeroLevel_InternalState = MENUMANAGER_ZEROLEVEL_INTERNALSTATE_EXITING;

      break;
    }
    case MENUMANAGER_ZEROLEVEL_INTERNALSTATE_EXITING:
    default:
      break;
  }
}

/*=============================================================================================*/
static void MenuManager_ZeroLevel_SubTickHandler(void)
{
  if (MenuManager_ZeroLevel_InternalState == MENUMANAGER_ZEROLEVEL_INTERNALSTATE_EXITING)
  {
    MenuManager_ZeroLevel_Counter += (uint32_t)1U;

    if (MenuManager_ZeroLevel_Counter >= MENUMANAGER_ZEROLEVEL_COUNTER_MAX)
    {
      MenuManager_ZeroLevel_Counter = (uint32_t)0U;
      
      Fsm_TriggerEvent(&MenuManager_FsmContext, (tFsmEvent)MENUMANAGER_EVENT_SUBMENU_1);
    }
  }
}



#ifdef  __cplusplus
}
#endif
