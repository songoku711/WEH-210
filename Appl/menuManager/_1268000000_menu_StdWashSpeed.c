/* 
 * File:   _1268000000_menu_StdWashSpeed.c
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
#define MENUMANAGER_STDWASHSPEED_INTERNALDATALENGTH                   (uint8_t)5U

#define MENUMANAGER_STDWASHSPEED_INTERNALSTATE_IDX                    0U
#define MENUMANAGER_STDWASHSPEED_COUNTER_IDX                          1U
#define MENUMANAGER_STDWASHSPEED_VALUE_MIN_IDX                        2U
#define MENUMANAGER_STDWASHSPEED_VALUE_MAX_IDX                        3U
#define MENUMANAGER_STDWASHSPEED_VALUE_IDX                            4U

#define MenuManager_StdWashSpeed_InternalState                        MenuManager_GetInternalDataPtr(MENUMANAGER_STDWASHSPEED_INTERNALSTATE_IDX)
#define MenuManager_StdWashSpeed_Counter                              MenuManager_GetInternalDataPtr(MENUMANAGER_STDWASHSPEED_COUNTER_IDX)
#define MenuManager_StdWashSpeed_ValueMin                             MenuManager_GetInternalDataPtr(MENUMANAGER_STDWASHSPEED_VALUE_MIN_IDX)
#define MenuManager_StdWashSpeed_ValueMax                             MenuManager_GetInternalDataPtr(MENUMANAGER_STDWASHSPEED_VALUE_MAX_IDX)
#define MenuManager_StdWashSpeed_Value                                MenuManager_GetInternalDataPtr(MENUMANAGER_STDWASHSPEED_VALUE_IDX)



#define MENUMANAGER_STDWASHSPEED_INTERNALSTATE_READY                  (uint32_t)0U
#define MENUMANAGER_STDWASHSPEED_INTERNALSTATE_RUNNING                (uint32_t)1U
#define MENUMANAGER_STDWASHSPEED_INTERNALSTATE_DONE                   (uint32_t)2U
#define MENUMANAGER_STDWASHSPEED_INTERNALSTATE_EXITING                (uint32_t)3U

#define MENUMANAGER_STDWASHSPEED_COUNTER_MAX                          (uint32_t)50U

#define MENUMANAGER_STDWASHSPEED_VALUE_XPOS                           (uint32_t)10U
#define MENUMANAGER_STDWASHSPEED_VALUE_YPOS                           (uint32_t)5U

#define MENUMANAGER_STDWASHSPEED_VALUE_LENGTH                         (uint32_t)1U

#define MENUMANAGER_STDWASHSPEED_UNIT_XPOS                            (uint32_t)1U
#define MENUMANAGER_STDWASHSPEED_UNIT_YPOS                            (uint32_t)3U



/** Menu manager main titles and child menu titles */
static const uint8_t MenuManager_StdWashSpeed_MainTitleStr[] =        "STD WASH SPEED";



/** Menu manager button event mapping array */
static MenuManager_ButEventMapStruct MenuManager_StdWashSpeed_ButEventMap[4] =
{
  { IOMANAGER_BUTTON_STATE_START,                                     MENUMANAGER_EVENT_START_BUT             },
  { IOMANAGER_BUTTON_STATE_STOP,                                      MENUMANAGER_EVENT_STOP_BUT              },
  { IOMANAGER_BUTTON_STATE_UP,                                        MENUMANAGER_EVENT_UP_BUT                },
  { IOMANAGER_BUTTON_STATE_DOWN,                                      MENUMANAGER_EVENT_DOWN_BUT              }
};

/** Menu manager button event mapping configuration */
static MenuManager_ButEventMapConfStruct MenuManager_StdWashSpeed_ButEventMapConf =
{
  .butEventMapNum             = (uint8_t)4U,
  .butEventMapCfg             = &MenuManager_StdWashSpeed_ButEventMap
};



/** Menu manager event handlers */
static tFsmGuard MenuManager_StdWashSpeed_Entry                       (tFsmContextPtr const pFsmContext, tFsmEvent event);
static tFsmGuard MenuManager_StdWashSpeed_Exit                        (tFsmContextPtr const pFsmContext, tFsmEvent event);
static tFsmGuard MenuManager_StdWashSpeed_Submenu1                    (tFsmContextPtr const pFsmContext, tFsmEvent event);
static tFsmGuard MenuManager_StdWashSpeed_StartBut                    (tFsmContextPtr const pFsmContext, tFsmEvent event);
static tFsmGuard MenuManager_StdWashSpeed_StopBut                     (tFsmContextPtr const pFsmContext, tFsmEvent event);
static tFsmGuard MenuManager_StdWashSpeed_UpBut                       (tFsmContextPtr const pFsmContext, tFsmEvent event);
static tFsmGuard MenuManager_StdWashSpeed_DownBut                     (tFsmContextPtr const pFsmContext, tFsmEvent event);

/** Menu manager state machine */
tFsmEventEntry MenuManager_StdWashSpeed_StateMachine[7] =
{
  FSM_TRIGGER_ENTRY             (                                     MenuManager_StdWashSpeed_Entry                                                  ),
  FSM_TRIGGER_EXIT              (                                     MenuManager_StdWashSpeed_Exit                                                   ),
  FSM_TRIGGER_TRANSITION_ACTION ( MENUMANAGER_EVENT_SUBMENU_1,        MenuManager_StdWashSpeed_Submenu1,      MENUMANAGER_STATE_WASH_SETUP            ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_START_BUT,        MenuManager_StdWashSpeed_StartBut                                               ),
  FSM_TRIGGER_TRANSITION_ACTION ( MENUMANAGER_EVENT_STOP_BUT,         MenuManager_StdWashSpeed_StopBut,       MENUMANAGER_STATE_WASH_SETUP            ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_UP_BUT,           MenuManager_StdWashSpeed_UpBut                                                  ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_DOWN_BUT,         MenuManager_StdWashSpeed_DownBut                                                )
};



/*===============================================================================================
*                                   LOCAL FUNCTION PROTOTYPES
===============================================================================================*/

static void MenuManager_StdWashSpeed_LcdShowMainTitle(void);
static void MenuManager_StdWashSpeed_LcdShowAdjust(void);
static void MenuManager_StdWashSpeed_LcdShowDone(void);

static void MenuManager_StdWashSpeed_SubMainFunction(void);
static void MenuManager_StdWashSpeed_SubTickHandler(void);



/*===============================================================================================
*                                       LOCAL FUNCTIONS
===============================================================================================*/

static void MenuManager_StdWashSpeed_LcdShowMainTitle(void)
{
  /* Print main title */
  MenuManager_Common_LcdClearAdjustMenuStatic();
  MenuManager_Common_LcdShowMainTitle(MenuManager_StdWashSpeed_MainTitleStr);
  MenuManager_Common_LcdShowOkExitChoice();
  MenuManager_Common_LcdUpdateAdjustMenuStatic();
}

/*=============================================================================================*/
static void MenuManager_StdWashSpeed_LcdShowAdjust(void)
{
  MenuManager_Common_LcdClearAdjustMenuDynamic();

  MenuManager_Common_LcdShowAdjustUnitVal
  (
    MENUMANAGER_STDWASHSPEED_VALUE_XPOS,
    MENUMANAGER_STDWASHSPEED_VALUE_YPOS,
    &(MenuManager_StdWashSpeed_Value),
    MENUMANAGER_STDWASHSPEED_VALUE_LENGTH
  );
  
  MenuManager_Common_LcdShowAdjustArrow
  (
    MENUMANAGER_STDWASHSPEED_VALUE_XPOS,
    MENUMANAGER_STDWASHSPEED_VALUE_YPOS,
    (uint32_t)0U
  );

  LCD_SetCursorPos(MENUMANAGER_STDWASHSPEED_UNIT_XPOS, MENUMANAGER_STDWASHSPEED_UNIT_YPOS, LCD_CURSOR_BY_FONT);
  LCD_PutString((uint8_t *)MenuManager_Common_LevelStr);

  MenuManager_Common_LcdUpdateAdjustMenuDynamic();
}

/*=============================================================================================*/
static void MenuManager_StdWashSpeed_LcdShowDone(void)
{
  MenuManager_Common_LcdClearAll();

  MenuManager_Common_LcdShowAdjustUnitVal
  (
    MENUMANAGER_STDWASHSPEED_VALUE_XPOS,
    MENUMANAGER_STDWASHSPEED_VALUE_YPOS,
    &(MenuManager_StdWashSpeed_Value),
    MENUMANAGER_STDWASHSPEED_VALUE_LENGTH
  );

  LCD_SetCursorPos(MENUMANAGER_STDWASHSPEED_UNIT_XPOS, MENUMANAGER_STDWASHSPEED_UNIT_YPOS, LCD_CURSOR_BY_FONT);
  LCD_PutString((uint8_t *)MenuManager_Common_LevelStr);

  MenuManager_Common_LcdUpdateAll();
}



/*=============================================================================================*/
static tFsmGuard MenuManager_StdWashSpeed_Entry(tFsmContextPtr const pFsmContext, tFsmEvent event)
{
  MenuManager_SubMainFunction = MenuManager_StdWashSpeed_SubMainFunction;
  MenuManager_SubTickHandler = MenuManager_StdWashSpeed_SubTickHandler;

  /* Check if previous state data hierachy is not empty */
  if (pFsmContext->dataHierachy != NULL)
  {
    if (pFsmContext->dataHierachy->dataId == MENUMANAGER_STATE_WASH_SETUP)
    {
      /* Release previous state data hierachy */
      MenuManager_free(pFsmContext->dataHierachy);
      pFsmContext->dataHierachy = NULL;

      MenuManager_InternalDataPush(MENUMANAGER_STDWASHSPEED_INTERNALDATALENGTH);

      MenuManager_StdWashSpeed_InternalState = MENUMANAGER_STDWASHSPEED_INTERNALSTATE_READY;
      MenuManager_StdWashSpeed_Counter = (uint32_t)0U;

      MenuManager_StdWashSpeed_ValueMin = (uint32_t)PROGRAMMANAGER_MOTOR_SPEED_LEVEL_0;
      MenuManager_StdWashSpeed_ValueMax = (uint32_t)(ProgramManager_gParamConfig.washCfg.maxWashSpeed);
      MenuManager_StdWashSpeed_Value = (uint32_t)(ProgramManager_gParamConfig.washCfg.stdWashSpeed);
    }
    else
    {
      return kFsmGuard_False;
    }

    MenuManager_StdWashSpeed_LcdShowMainTitle();
    MenuManager_StdWashSpeed_LcdShowAdjust();

    return kFsmGuard_True;
  }

  return kFsmGuard_False;
}

/*=============================================================================================*/
static tFsmGuard MenuManager_StdWashSpeed_Exit(tFsmContextPtr const pFsmContext, tFsmEvent event)
{
  MenuManager_SubMainFunction = NULL;
  MenuManager_SubTickHandler = NULL;

  tFsmDataHierachy* dataHierachy;

  dataHierachy = (tFsmDataHierachy *)MenuManager_malloc(sizeof(tFsmDataHierachy));
  dataHierachy->dataId = MENUMANAGER_STATE_STD_WASH_SPEED;

  pFsmContext->dataHierachy = dataHierachy;

  /* Free internal data */
  MenuManager_InternalDataPop();
  
  return kFsmGuard_True;
}

/*=============================================================================================*/
static tFsmGuard MenuManager_StdWashSpeed_Submenu1(tFsmContextPtr const pFsmContext, tFsmEvent event)
{
  
  
  return kFsmGuard_True;
}

/*=============================================================================================*/
static tFsmGuard MenuManager_StdWashSpeed_StartBut(tFsmContextPtr const pFsmContext, tFsmEvent event)
{
  if (MenuManager_StdWashSpeed_InternalState == MENUMANAGER_STDWASHSPEED_INTERNALSTATE_READY)
  {
    MenuManager_StdWashSpeed_InternalState = MENUMANAGER_STDWASHSPEED_INTERNALSTATE_RUNNING;
  }
  
  return kFsmGuard_True;
}

/*=============================================================================================*/
static tFsmGuard MenuManager_StdWashSpeed_StopBut(tFsmContextPtr const pFsmContext, tFsmEvent event)
{
  if (MenuManager_StdWashSpeed_InternalState != MENUMANAGER_STDWASHSPEED_INTERNALSTATE_READY)
  {
    return kFsmGuard_False;
  }
  
  return kFsmGuard_True;
}

/*=============================================================================================*/
static tFsmGuard MenuManager_StdWashSpeed_UpBut(tFsmContextPtr const pFsmContext, tFsmEvent event)
{
  if (MenuManager_StdWashSpeed_InternalState == MENUMANAGER_STDWASHSPEED_INTERNALSTATE_READY)
  {
    if (MenuManager_StdWashSpeed_Value < MenuManager_StdWashSpeed_ValueMax)
    {
      MenuManager_StdWashSpeed_Value += (uint32_t)1U;
    }

    MenuManager_StdWashSpeed_LcdShowAdjust();
  }

  return kFsmGuard_True;
}

/*=============================================================================================*/
static tFsmGuard MenuManager_StdWashSpeed_DownBut(tFsmContextPtr const pFsmContext, tFsmEvent event)
{
  if (MenuManager_StdWashSpeed_InternalState == MENUMANAGER_STDWASHSPEED_INTERNALSTATE_READY)
  {
    if (MenuManager_StdWashSpeed_Value > MenuManager_StdWashSpeed_ValueMin)
    {
      MenuManager_StdWashSpeed_Value -= (uint32_t)1U;
    }

    MenuManager_StdWashSpeed_LcdShowAdjust();
  }
  
  return kFsmGuard_True;
}



/*=============================================================================================*/
static void MenuManager_StdWashSpeed_SubMainFunction(void)
{
  MenuManager_Common_ButEventMapHandler(&MenuManager_StdWashSpeed_ButEventMapConf);

  switch (MenuManager_StdWashSpeed_InternalState)
  {
    case MENUMANAGER_STDWASHSPEED_INTERNALSTATE_READY:
      break;
    case MENUMANAGER_STDWASHSPEED_INTERNALSTATE_RUNNING:
    {
      ProgramManager_MotorSpeedType tempStdWashSpeed;

      tempStdWashSpeed = (ProgramManager_MotorSpeedType)MenuManager_StdWashSpeed_Value;

      ProgramManager_WashSetup_StdWashSpeed_SetData(&tempStdWashSpeed);

      ProgramManager_WashSetup_StdWashSpeed_GetData(&(ProgramManager_gParamConfig.washCfg.stdWashSpeed));

      MenuManager_StdWashSpeed_InternalState = MENUMANAGER_STDWASHSPEED_INTERNALSTATE_DONE;
      
      break;
    }
    case MENUMANAGER_STDWASHSPEED_INTERNALSTATE_DONE:
    {
      MenuManager_StdWashSpeed_LcdShowDone();

      MenuManager_StdWashSpeed_InternalState = MENUMANAGER_STDWASHSPEED_INTERNALSTATE_EXITING;

      break;
    }
    case MENUMANAGER_STDWASHSPEED_INTERNALSTATE_EXITING:
    default:
      break;
  }
}

/*=============================================================================================*/
static void MenuManager_StdWashSpeed_SubTickHandler(void)
{
  if (MenuManager_StdWashSpeed_InternalState == MENUMANAGER_STDWASHSPEED_INTERNALSTATE_EXITING)
  {
    MenuManager_StdWashSpeed_Counter += (uint32_t)1U;

    if (MenuManager_StdWashSpeed_Counter >= MENUMANAGER_STDWASHSPEED_COUNTER_MAX)
    {
      MenuManager_StdWashSpeed_Counter = (uint32_t)0U;
      
      Fsm_TriggerEvent(&MenuManager_FsmContext, (tFsmEvent)MENUMANAGER_EVENT_SUBMENU_1);
    }
  }
}



#ifdef  __cplusplus
}
#endif
