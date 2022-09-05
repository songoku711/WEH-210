/* 
 * File:   _126B000000_menu_MaxWashSpeed.c
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
#define MENUMANAGER_MAXWASHSPEED_INTERNALDATALENGTH                   (uint8_t)5U

#define MENUMANAGER_MAXWASHSPEED_INTERNALSTATE_IDX                    0U
#define MENUMANAGER_MAXWASHSPEED_COUNTER_IDX                          1U
#define MENUMANAGER_MAXWASHSPEED_VALUE_MIN_IDX                        2U
#define MENUMANAGER_MAXWASHSPEED_VALUE_MAX_IDX                        3U
#define MENUMANAGER_MAXWASHSPEED_VALUE_IDX                            4U

#define MenuManager_MaxWashSpeed_InternalState                        MenuManager_GetInternalDataPtr(MENUMANAGER_MAXWASHSPEED_INTERNALSTATE_IDX)
#define MenuManager_MaxWashSpeed_Counter                              MenuManager_GetInternalDataPtr(MENUMANAGER_MAXWASHSPEED_COUNTER_IDX)
#define MenuManager_MaxWashSpeed_ValueMin                             MenuManager_GetInternalDataPtr(MENUMANAGER_MAXWASHSPEED_VALUE_MIN_IDX)
#define MenuManager_MaxWashSpeed_ValueMax                             MenuManager_GetInternalDataPtr(MENUMANAGER_MAXWASHSPEED_VALUE_MAX_IDX)
#define MenuManager_MaxWashSpeed_Value                                MenuManager_GetInternalDataPtr(MENUMANAGER_MAXWASHSPEED_VALUE_IDX)



#define MENUMANAGER_MAXWASHSPEED_INTERNALSTATE_READY                  (uint32_t)0U
#define MENUMANAGER_MAXWASHSPEED_INTERNALSTATE_RUNNING                (uint32_t)1U
#define MENUMANAGER_MAXWASHSPEED_INTERNALSTATE_DONE                   (uint32_t)2U
#define MENUMANAGER_MAXWASHSPEED_INTERNALSTATE_EXITING                (uint32_t)3U

#define MENUMANAGER_MAXWASHSPEED_COUNTER_MAX                          (uint32_t)50U

#define MENUMANAGER_MAXWASHSPEED_VALUE_XPOS                           (uint32_t)10U
#define MENUMANAGER_MAXWASHSPEED_VALUE_YPOS                           (uint32_t)5U

#define MENUMANAGER_MAXWASHSPEED_VALUE_LENGTH                         (uint32_t)1U

#define MENUMANAGER_MAXWASHSPEED_UNIT_XPOS                            (uint32_t)1U
#define MENUMANAGER_MAXWASHSPEED_UNIT_YPOS                            (uint32_t)3U



/** Menu manager main titles and child menu titles */
static const uint8_t MenuManager_MaxWashSpeed_MainTitleStr[] =        "MAX WASH SPEED";



/** Menu manager button event mapping array */
static MenuManager_ButEventMapStruct MenuManager_MaxWashSpeed_ButEventMap[4] =
{
  { IOMANAGER_BUTTON_STATE_START,                                     MENUMANAGER_EVENT_START_BUT             },
  { IOMANAGER_BUTTON_STATE_STOP,                                      MENUMANAGER_EVENT_STOP_BUT              },
  { IOMANAGER_BUTTON_STATE_UP,                                        MENUMANAGER_EVENT_UP_BUT                },
  { IOMANAGER_BUTTON_STATE_DOWN,                                      MENUMANAGER_EVENT_DOWN_BUT              }
};

/** Menu manager button event mapping configuration */
static MenuManager_ButEventMapConfStruct MenuManager_MaxWashSpeed_ButEventMapConf =
{
  .butEventMapNum             = (uint8_t)4U,
  .butEventMapCfg             = &MenuManager_MaxWashSpeed_ButEventMap
};



/** Menu manager event handlers */
static tFsmGuard MenuManager_MaxWashSpeed_Entry                       (tFsmContextPtr const pFsmContext, tFsmEvent event);
static tFsmGuard MenuManager_MaxWashSpeed_Exit                        (tFsmContextPtr const pFsmContext, tFsmEvent event);
static tFsmGuard MenuManager_MaxWashSpeed_Submenu1                    (tFsmContextPtr const pFsmContext, tFsmEvent event);
static tFsmGuard MenuManager_MaxWashSpeed_StartBut                    (tFsmContextPtr const pFsmContext, tFsmEvent event);
static tFsmGuard MenuManager_MaxWashSpeed_StopBut                     (tFsmContextPtr const pFsmContext, tFsmEvent event);
static tFsmGuard MenuManager_MaxWashSpeed_UpBut                       (tFsmContextPtr const pFsmContext, tFsmEvent event);
static tFsmGuard MenuManager_MaxWashSpeed_DownBut                     (tFsmContextPtr const pFsmContext, tFsmEvent event);

/** Menu manager state machine */
tFsmEventEntry MenuManager_MaxWashSpeed_StateMachine[7] =
{
  FSM_TRIGGER_ENTRY             (                                     MenuManager_MaxWashSpeed_Entry                                                  ),
  FSM_TRIGGER_EXIT              (                                     MenuManager_MaxWashSpeed_Exit                                                   ),
  FSM_TRIGGER_TRANSITION_ACTION ( MENUMANAGER_EVENT_SUBMENU_1,        MenuManager_MaxWashSpeed_Submenu1,      MENUMANAGER_STATE_WASH_SETUP            ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_START_BUT,        MenuManager_MaxWashSpeed_StartBut                                               ),
  FSM_TRIGGER_TRANSITION_ACTION ( MENUMANAGER_EVENT_STOP_BUT,         MenuManager_MaxWashSpeed_StopBut,       MENUMANAGER_STATE_WASH_SETUP            ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_UP_BUT,           MenuManager_MaxWashSpeed_UpBut                                                  ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_DOWN_BUT,         MenuManager_MaxWashSpeed_DownBut                                                )
};



/*===============================================================================================
*                                   LOCAL FUNCTION PROTOTYPES
===============================================================================================*/

static void MenuManager_MaxWashSpeed_LcdShowMainTitle(void);
static void MenuManager_MaxWashSpeed_LcdShowAdjust(void);
static void MenuManager_MaxWashSpeed_LcdShowDone(void);

static void MenuManager_MaxWashSpeed_SubMainFunction(void);
static void MenuManager_MaxWashSpeed_SubTickHandler(void);



/*===============================================================================================
*                                       LOCAL FUNCTIONS
===============================================================================================*/

static void MenuManager_MaxWashSpeed_LcdShowMainTitle(void)
{
  /* Print main title */
  MenuManager_Common_LcdClearAdjustMenuStatic();
  MenuManager_Common_LcdShowMainTitle(MenuManager_MaxWashSpeed_MainTitleStr);
  MenuManager_Common_LcdShowOkExitChoice();
  MenuManager_Common_LcdUpdateAdjustMenuStatic();
}

/*=============================================================================================*/
static void MenuManager_MaxWashSpeed_LcdShowAdjust(void)
{
  MenuManager_Common_LcdClearAdjustMenuDynamic();

  MenuManager_Common_LcdShowAdjustUnitVal
  (
    MENUMANAGER_MAXWASHSPEED_VALUE_XPOS,
    MENUMANAGER_MAXWASHSPEED_VALUE_YPOS,
    &(MenuManager_MaxWashSpeed_Value),
    MENUMANAGER_MAXWASHSPEED_VALUE_LENGTH
  );
  
  MenuManager_Common_LcdShowAdjustArrow
  (
    MENUMANAGER_MAXWASHSPEED_VALUE_XPOS,
    MENUMANAGER_MAXWASHSPEED_VALUE_YPOS,
    (uint32_t)0U
  );

  LCD_SetCursorPos(MENUMANAGER_MAXWASHSPEED_UNIT_XPOS, MENUMANAGER_MAXWASHSPEED_UNIT_YPOS, LCD_CURSOR_BY_FONT);
  LCD_PutString((uint8_t *)MenuManager_Common_LevelStr);

  MenuManager_Common_LcdUpdateAdjustMenuDynamic();
}

/*=============================================================================================*/
static void MenuManager_MaxWashSpeed_LcdShowDone(void)
{
  MenuManager_Common_LcdClearAll();

  MenuManager_Common_LcdShowAdjustUnitVal
  (
    MENUMANAGER_MAXWASHSPEED_VALUE_XPOS,
    MENUMANAGER_MAXWASHSPEED_VALUE_YPOS,
    &(MenuManager_MaxWashSpeed_Value),
    MENUMANAGER_MAXWASHSPEED_VALUE_LENGTH
  );

  LCD_SetCursorPos(MENUMANAGER_MAXWASHSPEED_UNIT_XPOS, MENUMANAGER_MAXWASHSPEED_UNIT_YPOS, LCD_CURSOR_BY_FONT);
  LCD_PutString((uint8_t *)MenuManager_Common_LevelStr);

  MenuManager_Common_LcdUpdateAll();
}



/*=============================================================================================*/
static tFsmGuard MenuManager_MaxWashSpeed_Entry(tFsmContextPtr const pFsmContext, tFsmEvent event)
{
  MenuManager_SubMainFunction = MenuManager_MaxWashSpeed_SubMainFunction;
  MenuManager_SubTickHandler = MenuManager_MaxWashSpeed_SubTickHandler;

  /* Check if previous state data hierachy is not empty */
  if (pFsmContext->dataHierachy != NULL)
  {
    if (pFsmContext->dataHierachy->dataId == MENUMANAGER_STATE_WASH_SETUP)
    {
      /* Release previous state data hierachy */
      MenuManager_free(pFsmContext->dataHierachy);
      pFsmContext->dataHierachy = NULL;

      MenuManager_InternalDataPush(MENUMANAGER_MAXWASHSPEED_INTERNALDATALENGTH);

      MenuManager_MaxWashSpeed_InternalState = MENUMANAGER_MAXWASHSPEED_INTERNALSTATE_READY;
      MenuManager_MaxWashSpeed_Counter = (uint32_t)0U;

      MenuManager_MaxWashSpeed_ValueMin = (uint32_t)PROGRAMMANAGER_MOTOR_SPEED_LEVEL_0;
      MenuManager_MaxWashSpeed_ValueMax = (uint32_t)PROGRAMMANAGER_MOTOR_SPEED_LEVEL_7;
      MenuManager_MaxWashSpeed_Value = (uint32_t)(ProgramManager_gParamConfig.washCfg.maxWashSpeed);
    }
    else
    {
      return kFsmGuard_False;
    }

    MenuManager_MaxWashSpeed_LcdShowMainTitle();
    MenuManager_MaxWashSpeed_LcdShowAdjust();

    return kFsmGuard_True;
  }

  return kFsmGuard_False;
}

/*=============================================================================================*/
static tFsmGuard MenuManager_MaxWashSpeed_Exit(tFsmContextPtr const pFsmContext, tFsmEvent event)
{
  MenuManager_SubMainFunction = NULL;
  MenuManager_SubTickHandler = NULL;

  tFsmDataHierachy* dataHierachy;

  dataHierachy = (tFsmDataHierachy *)MenuManager_malloc(sizeof(tFsmDataHierachy));
  dataHierachy->dataId = MENUMANAGER_STATE_MAX_WASH_SPEED;

  pFsmContext->dataHierachy = dataHierachy;

  /* Free internal data */
  MenuManager_InternalDataPop();
  
  return kFsmGuard_True;
}

/*=============================================================================================*/
static tFsmGuard MenuManager_MaxWashSpeed_Submenu1(tFsmContextPtr const pFsmContext, tFsmEvent event)
{
  
  
  return kFsmGuard_True;
}

/*=============================================================================================*/
static tFsmGuard MenuManager_MaxWashSpeed_StartBut(tFsmContextPtr const pFsmContext, tFsmEvent event)
{
  if (MenuManager_MaxWashSpeed_InternalState == MENUMANAGER_MAXWASHSPEED_INTERNALSTATE_READY)
  {
    MenuManager_MaxWashSpeed_InternalState = MENUMANAGER_MAXWASHSPEED_INTERNALSTATE_RUNNING;
  }
  
  return kFsmGuard_True;
}

/*=============================================================================================*/
static tFsmGuard MenuManager_MaxWashSpeed_StopBut(tFsmContextPtr const pFsmContext, tFsmEvent event)
{
  if (MenuManager_MaxWashSpeed_InternalState != MENUMANAGER_MAXWASHSPEED_INTERNALSTATE_READY)
  {
    return kFsmGuard_False;
  }
  
  return kFsmGuard_True;
}

/*=============================================================================================*/
static tFsmGuard MenuManager_MaxWashSpeed_UpBut(tFsmContextPtr const pFsmContext, tFsmEvent event)
{
  if (MenuManager_MaxWashSpeed_InternalState == MENUMANAGER_MAXWASHSPEED_INTERNALSTATE_READY)
  {
    if (MenuManager_MaxWashSpeed_Value < MenuManager_MaxWashSpeed_ValueMax)
    {
      MenuManager_MaxWashSpeed_Value += (uint32_t)1U;
    }

    MenuManager_MaxWashSpeed_LcdShowAdjust();
  }

  return kFsmGuard_True;
}

/*=============================================================================================*/
static tFsmGuard MenuManager_MaxWashSpeed_DownBut(tFsmContextPtr const pFsmContext, tFsmEvent event)
{
  if (MenuManager_MaxWashSpeed_InternalState == MENUMANAGER_MAXWASHSPEED_INTERNALSTATE_READY)
  {
    if (MenuManager_MaxWashSpeed_Value > MenuManager_MaxWashSpeed_ValueMin)
    {
      MenuManager_MaxWashSpeed_Value -= (uint32_t)1U;
    }

    MenuManager_MaxWashSpeed_LcdShowAdjust();
  }
  
  return kFsmGuard_True;
}



/*=============================================================================================*/
static void MenuManager_MaxWashSpeed_SubMainFunction(void)
{
  MenuManager_Common_ButEventMapHandler(&MenuManager_MaxWashSpeed_ButEventMapConf);

  switch (MenuManager_MaxWashSpeed_InternalState)
  {
    case MENUMANAGER_MAXWASHSPEED_INTERNALSTATE_READY:
      break;
    case MENUMANAGER_MAXWASHSPEED_INTERNALSTATE_RUNNING:
    {
      ProgramManager_MotorSpeedType tempMaxWashSpeed;

      tempMaxWashSpeed = (ProgramManager_MotorSpeedType)MenuManager_MaxWashSpeed_Value;

      ProgramManager_WashSetup_MaxWashSpeed_SetData(&tempMaxWashSpeed);
      ProgramManager_WashSetup_MaxWashSpeed_GetData(&(ProgramManager_gParamConfig.washCfg.maxWashSpeed));

      /* Check if other wash speeds is greater than current max speed then correct them */
      if (ProgramManager_gParamConfig.washCfg.stdWashSpeed > ProgramManager_gParamConfig.washCfg.maxWashSpeed)
      {
        ProgramManager_WashSetup_StdWashSpeed_SetData(&tempMaxWashSpeed);
        ProgramManager_WashSetup_StdWashSpeed_GetData(&(ProgramManager_gParamConfig.washCfg.stdWashSpeed));
      }

      if (ProgramManager_gParamConfig.washCfg.delWashSpeed > ProgramManager_gParamConfig.washCfg.maxWashSpeed)
      {
        ProgramManager_WashSetup_DelWashSpeed_SetData(&tempMaxWashSpeed);
        ProgramManager_WashSetup_DelWashSpeed_GetData(&(ProgramManager_gParamConfig.washCfg.delWashSpeed));
      }

      if (ProgramManager_gParamConfig.washCfg.hvyWashSpeed > ProgramManager_gParamConfig.washCfg.maxWashSpeed)
      {
        ProgramManager_WashSetup_HvyWashSpeed_SetData(&tempMaxWashSpeed);
        ProgramManager_WashSetup_HvyWashSpeed_GetData(&(ProgramManager_gParamConfig.washCfg.hvyWashSpeed));
      }

      MenuManager_MaxWashSpeed_InternalState = MENUMANAGER_MAXWASHSPEED_INTERNALSTATE_DONE;
      
      break;
    }
    case MENUMANAGER_MAXWASHSPEED_INTERNALSTATE_DONE:
    {
      MenuManager_MaxWashSpeed_LcdShowDone();

      MenuManager_MaxWashSpeed_InternalState = MENUMANAGER_MAXWASHSPEED_INTERNALSTATE_EXITING;

      break;
    }
    case MENUMANAGER_MAXWASHSPEED_INTERNALSTATE_EXITING:
    default:
      break;
  }
}

/*=============================================================================================*/
static void MenuManager_MaxWashSpeed_SubTickHandler(void)
{
  if (MenuManager_MaxWashSpeed_InternalState == MENUMANAGER_MAXWASHSPEED_INTERNALSTATE_EXITING)
  {
    MenuManager_MaxWashSpeed_Counter += (uint32_t)1U;

    if (MenuManager_MaxWashSpeed_Counter >= MENUMANAGER_MAXWASHSPEED_COUNTER_MAX)
    {
      MenuManager_MaxWashSpeed_Counter = (uint32_t)0U;
      
      Fsm_TriggerEvent(&MenuManager_FsmContext, (tFsmEvent)MENUMANAGER_EVENT_SUBMENU_1);
    }
  }
}



#ifdef  __cplusplus
}
#endif
