/* 
 * File:   _127K000000_menu_HighExtractSpeed2.c
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
#define MENUMANAGER_HIGHEXTRACTSPEED2_INTERNALDATALENGTH              (uint8_t)5U

#define MENUMANAGER_HIGHEXTRACTSPEED2_INTERNALSTATE_IDX               0U
#define MENUMANAGER_HIGHEXTRACTSPEED2_COUNTER_IDX                     1U
#define MENUMANAGER_HIGHEXTRACTSPEED2_VALUE_MIN_IDX                   2U
#define MENUMANAGER_HIGHEXTRACTSPEED2_VALUE_MAX_IDX                   3U
#define MENUMANAGER_HIGHEXTRACTSPEED2_VALUE_IDX                       4U

#define MenuManager_HighExtractSpeed2_InternalState                   MenuManager_GetInternalDataPtr(MENUMANAGER_HIGHEXTRACTSPEED2_INTERNALSTATE_IDX)
#define MenuManager_HighExtractSpeed2_Counter                         MenuManager_GetInternalDataPtr(MENUMANAGER_HIGHEXTRACTSPEED2_COUNTER_IDX)
#define MenuManager_HighExtractSpeed2_ValueMin                        MenuManager_GetInternalDataPtr(MENUMANAGER_HIGHEXTRACTSPEED2_VALUE_MIN_IDX)
#define MenuManager_HighExtractSpeed2_ValueMax                        MenuManager_GetInternalDataPtr(MENUMANAGER_HIGHEXTRACTSPEED2_VALUE_MAX_IDX)
#define MenuManager_HighExtractSpeed2_Value                           MenuManager_GetInternalDataPtr(MENUMANAGER_HIGHEXTRACTSPEED2_VALUE_IDX)



#define MENUMANAGER_HIGHEXTRACTSPEED2_INTERNALSTATE_READY             (uint32_t)0U
#define MENUMANAGER_HIGHEXTRACTSPEED2_INTERNALSTATE_RUNNING           (uint32_t)1U
#define MENUMANAGER_HIGHEXTRACTSPEED2_INTERNALSTATE_DONE              (uint32_t)2U
#define MENUMANAGER_HIGHEXTRACTSPEED2_INTERNALSTATE_EXITING           (uint32_t)3U

#define MENUMANAGER_HIGHEXTRACTSPEED2_COUNTER_MAX                     (uint32_t)50U

#define MENUMANAGER_HIGHEXTRACTSPEED2_VALUE_XPOS                      (uint32_t)10U
#define MENUMANAGER_HIGHEXTRACTSPEED2_VALUE_YPOS                      (uint32_t)5U

#define MENUMANAGER_HIGHEXTRACTSPEED2_VALUE_LENGTH                    (uint32_t)1U

#define MENUMANAGER_HIGHEXTRACTSPEED2_UNIT_XPOS                       (uint32_t)1U
#define MENUMANAGER_HIGHEXTRACTSPEED2_UNIT_YPOS                       (uint32_t)3U



/** Menu manager main titles and child menu titles */
static const uint8_t MenuManager_HighExtractSpeed2_MainTitleStr[] =   "HIGH EX SPEED 2";



/** Menu manager button event mapping array */
static MenuManager_ButEventMapStruct MenuManager_HighExtractSpeed2_ButEventMap[4] =
{
  { IOMANAGER_BUTTON_STATE_START,                                     MENUMANAGER_EVENT_START_BUT             },
  { IOMANAGER_BUTTON_STATE_STOP,                                      MENUMANAGER_EVENT_STOP_BUT              },
  { IOMANAGER_BUTTON_STATE_UP,                                        MENUMANAGER_EVENT_UP_BUT                },
  { IOMANAGER_BUTTON_STATE_DOWN,                                      MENUMANAGER_EVENT_DOWN_BUT              }
};

/** Menu manager button event mapping configuration */
static MenuManager_ButEventMapConfStruct MenuManager_HighExtractSpeed2_ButEventMapConf =
{
  .butEventMapNum             = (uint8_t)4U,
  .butEventMapCfg             = &MenuManager_HighExtractSpeed2_ButEventMap
};



/** Menu manager event handlers */
static tFsmGuard MenuManager_HighExtractSpeed2_Entry                  (tFsmContextPtr const pFsmContext, tFsmEvent event);
static tFsmGuard MenuManager_HighExtractSpeed2_Exit                   (tFsmContextPtr const pFsmContext, tFsmEvent event);
static tFsmGuard MenuManager_HighExtractSpeed2_Submenu1               (tFsmContextPtr const pFsmContext, tFsmEvent event);
static tFsmGuard MenuManager_HighExtractSpeed2_StartBut               (tFsmContextPtr const pFsmContext, tFsmEvent event);
static tFsmGuard MenuManager_HighExtractSpeed2_StopBut                (tFsmContextPtr const pFsmContext, tFsmEvent event);
static tFsmGuard MenuManager_HighExtractSpeed2_UpBut                  (tFsmContextPtr const pFsmContext, tFsmEvent event);
static tFsmGuard MenuManager_HighExtractSpeed2_DownBut                (tFsmContextPtr const pFsmContext, tFsmEvent event);

/** Menu manager state machine */
tFsmEventEntry MenuManager_HighExtractSpeed2_StateMachine[7] =
{
  FSM_TRIGGER_ENTRY             (                                     MenuManager_HighExtractSpeed2_Entry                                             ),
  FSM_TRIGGER_EXIT              (                                     MenuManager_HighExtractSpeed2_Exit                                              ),
  FSM_TRIGGER_TRANSITION_ACTION ( MENUMANAGER_EVENT_SUBMENU_1,        MenuManager_HighExtractSpeed2_Submenu1, MENUMANAGER_STATE_EXTRACT_SETUP         ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_START_BUT,        MenuManager_HighExtractSpeed2_StartBut                                          ),
  FSM_TRIGGER_TRANSITION_ACTION ( MENUMANAGER_EVENT_STOP_BUT,         MenuManager_HighExtractSpeed2_StopBut,  MENUMANAGER_STATE_EXTRACT_SETUP         ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_UP_BUT,           MenuManager_HighExtractSpeed2_UpBut                                             ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_DOWN_BUT,         MenuManager_HighExtractSpeed2_DownBut                                           )
};



/*===============================================================================================
*                                   LOCAL FUNCTION PROTOTYPES
===============================================================================================*/

static void MenuManager_HighExtractSpeed2_LcdShowMainTitle(void);
static void MenuManager_HighExtractSpeed2_LcdShowAdjust(void);
static void MenuManager_HighExtractSpeed2_LcdShowDone(void);

static void MenuManager_HighExtractSpeed2_SubMainFunction(void);
static void MenuManager_HighExtractSpeed2_SubTickHandler(void);



/*===============================================================================================
*                                       LOCAL FUNCTIONS
===============================================================================================*/

static void MenuManager_HighExtractSpeed2_LcdShowMainTitle(void)
{
  /* Print main title */
  MenuManager_Common_LcdClearAdjustMenuStatic();
  MenuManager_Common_LcdShowMainTitle(MenuManager_HighExtractSpeed2_MainTitleStr);
  MenuManager_Common_LcdShowOkExitChoice();
  MenuManager_Common_LcdUpdateAdjustMenuStatic();
}

/*=============================================================================================*/
static void MenuManager_HighExtractSpeed2_LcdShowAdjust(void)
{
  MenuManager_Common_LcdClearAdjustMenuDynamic();

  MenuManager_Common_LcdShowAdjustUnitVal
  (
    MENUMANAGER_HIGHEXTRACTSPEED2_VALUE_XPOS,
    MENUMANAGER_HIGHEXTRACTSPEED2_VALUE_YPOS,
    &(MenuManager_HighExtractSpeed2_Value),
    MENUMANAGER_HIGHEXTRACTSPEED2_VALUE_LENGTH
  );
  
  MenuManager_Common_LcdShowAdjustArrow
  (
    MENUMANAGER_HIGHEXTRACTSPEED2_VALUE_XPOS,
    MENUMANAGER_HIGHEXTRACTSPEED2_VALUE_YPOS,
    (uint32_t)0U
  );

  LCD_SetCursorPos(MENUMANAGER_HIGHEXTRACTSPEED2_UNIT_XPOS, MENUMANAGER_HIGHEXTRACTSPEED2_UNIT_YPOS, LCD_CURSOR_BY_FONT);
  LCD_PutString((uint8_t *)MenuManager_Common_LevelStr);

  MenuManager_Common_LcdUpdateAdjustMenuDynamic();
}

/*=============================================================================================*/
static void MenuManager_HighExtractSpeed2_LcdShowDone(void)
{
  MenuManager_Common_LcdClearAll();

  MenuManager_Common_LcdShowAdjustUnitVal
  (
    MENUMANAGER_HIGHEXTRACTSPEED2_VALUE_XPOS,
    MENUMANAGER_HIGHEXTRACTSPEED2_VALUE_YPOS,
    &(MenuManager_HighExtractSpeed2_Value),
    MENUMANAGER_HIGHEXTRACTSPEED2_VALUE_LENGTH
  );

  LCD_SetCursorPos(MENUMANAGER_HIGHEXTRACTSPEED2_UNIT_XPOS, MENUMANAGER_HIGHEXTRACTSPEED2_UNIT_YPOS, LCD_CURSOR_BY_FONT);
  LCD_PutString((uint8_t *)MenuManager_Common_LevelStr);

  MenuManager_Common_LcdUpdateAll();
}



/*=============================================================================================*/
static tFsmGuard MenuManager_HighExtractSpeed2_Entry(tFsmContextPtr const pFsmContext, tFsmEvent event)
{
  MenuManager_SubMainFunction = MenuManager_HighExtractSpeed2_SubMainFunction;
  MenuManager_SubTickHandler = MenuManager_HighExtractSpeed2_SubTickHandler;

  /* Check if previous state data hierachy is not empty */
  if (pFsmContext->dataHierachy != NULL)
  {
    if (pFsmContext->dataHierachy->dataId == MENUMANAGER_STATE_EXTRACT_SETUP)
    {
      /* Release previous state data hierachy */
      MenuManager_free(pFsmContext->dataHierachy);
      pFsmContext->dataHierachy = NULL;

      MenuManager_InternalDataPush(MENUMANAGER_HIGHEXTRACTSPEED2_INTERNALDATALENGTH);

      MenuManager_HighExtractSpeed2_InternalState = MENUMANAGER_HIGHEXTRACTSPEED2_INTERNALSTATE_READY;
      MenuManager_HighExtractSpeed2_Counter = (uint32_t)0U;

      MenuManager_HighExtractSpeed2_ValueMin = (uint32_t)(ProgramManager_gParamConfig.extractCfg.highExtractSpeed1);
      MenuManager_HighExtractSpeed2_ValueMax = (uint32_t)(ProgramManager_gParamConfig.extractCfg.highExtractSpeed3);
      MenuManager_HighExtractSpeed2_Value = (uint32_t)(ProgramManager_gParamConfig.extractCfg.highExtractSpeed2);
    }
    else
    {
      return kFsmGuard_False;
    }

    MenuManager_HighExtractSpeed2_LcdShowMainTitle();
    MenuManager_HighExtractSpeed2_LcdShowAdjust();

    return kFsmGuard_True;
  }

  return kFsmGuard_False;
}

/*=============================================================================================*/
static tFsmGuard MenuManager_HighExtractSpeed2_Exit(tFsmContextPtr const pFsmContext, tFsmEvent event)
{
  MenuManager_SubMainFunction = NULL;
  MenuManager_SubTickHandler = NULL;

  tFsmDataHierachy* dataHierachy;

  dataHierachy = (tFsmDataHierachy *)MenuManager_malloc(sizeof(tFsmDataHierachy));
  dataHierachy->dataId = MENUMANAGER_STATE_HIGH_EXTRACT_SPEED_2;

  pFsmContext->dataHierachy = dataHierachy;

  /* Free internal data */
  MenuManager_InternalDataPop();
  
  return kFsmGuard_True;
}

/*=============================================================================================*/
static tFsmGuard MenuManager_HighExtractSpeed2_Submenu1(tFsmContextPtr const pFsmContext, tFsmEvent event)
{
  
  
  return kFsmGuard_True;
}

/*=============================================================================================*/
static tFsmGuard MenuManager_HighExtractSpeed2_StartBut(tFsmContextPtr const pFsmContext, tFsmEvent event)
{
  if (MenuManager_HighExtractSpeed2_InternalState == MENUMANAGER_HIGHEXTRACTSPEED2_INTERNALSTATE_READY)
  {
    MenuManager_HighExtractSpeed2_InternalState = MENUMANAGER_HIGHEXTRACTSPEED2_INTERNALSTATE_RUNNING;
  }
  
  return kFsmGuard_True;
}

/*=============================================================================================*/
static tFsmGuard MenuManager_HighExtractSpeed2_StopBut(tFsmContextPtr const pFsmContext, tFsmEvent event)
{
  if (MenuManager_HighExtractSpeed2_InternalState != MENUMANAGER_HIGHEXTRACTSPEED2_INTERNALSTATE_READY)
  {
    return kFsmGuard_False;
  }
  
  return kFsmGuard_True;
}

/*=============================================================================================*/
static tFsmGuard MenuManager_HighExtractSpeed2_UpBut(tFsmContextPtr const pFsmContext, tFsmEvent event)
{
  if (MenuManager_HighExtractSpeed2_InternalState == MENUMANAGER_HIGHEXTRACTSPEED2_INTERNALSTATE_READY)
  {
    if (MenuManager_HighExtractSpeed2_Value < MenuManager_HighExtractSpeed2_ValueMax)
    {
      MenuManager_HighExtractSpeed2_Value += (uint32_t)1U;
    }

    MenuManager_HighExtractSpeed2_LcdShowAdjust();
  }

  return kFsmGuard_True;
}

/*=============================================================================================*/
static tFsmGuard MenuManager_HighExtractSpeed2_DownBut(tFsmContextPtr const pFsmContext, tFsmEvent event)
{
  if (MenuManager_HighExtractSpeed2_InternalState == MENUMANAGER_HIGHEXTRACTSPEED2_INTERNALSTATE_READY)
  {
    if (MenuManager_HighExtractSpeed2_Value > MenuManager_HighExtractSpeed2_ValueMin)
    {
      MenuManager_HighExtractSpeed2_Value -= (uint32_t)1U;
    }

    MenuManager_HighExtractSpeed2_LcdShowAdjust();
  }
  
  return kFsmGuard_True;
}



/*=============================================================================================*/
static void MenuManager_HighExtractSpeed2_SubMainFunction(void)
{
  MenuManager_Common_ButEventMapHandler(&MenuManager_HighExtractSpeed2_ButEventMapConf);

  switch (MenuManager_HighExtractSpeed2_InternalState)
  {
    case MENUMANAGER_HIGHEXTRACTSPEED2_INTERNALSTATE_READY:
      break;
    case MENUMANAGER_HIGHEXTRACTSPEED2_INTERNALSTATE_RUNNING:
    {
      ProgramManager_MotorSpeedType tempHighExtractSpeed2;

      tempHighExtractSpeed2 = (ProgramManager_MotorSpeedType)MenuManager_HighExtractSpeed2_Value;

      ProgramManager_ExtractSetup_HighExtractSpeed2_SetData(&tempHighExtractSpeed2);

      ProgramManager_ExtractSetup_HighExtractSpeed2_GetData(&(ProgramManager_gParamConfig.extractCfg.highExtractSpeed2));

      MenuManager_HighExtractSpeed2_InternalState = MENUMANAGER_HIGHEXTRACTSPEED2_INTERNALSTATE_DONE;
      
      break;
    }
    case MENUMANAGER_HIGHEXTRACTSPEED2_INTERNALSTATE_DONE:
    {
      MenuManager_HighExtractSpeed2_LcdShowDone();

      MenuManager_HighExtractSpeed2_InternalState = MENUMANAGER_HIGHEXTRACTSPEED2_INTERNALSTATE_EXITING;

      break;
    }
    case MENUMANAGER_HIGHEXTRACTSPEED2_INTERNALSTATE_EXITING:
    default:
      break;
  }
}

/*=============================================================================================*/
static void MenuManager_HighExtractSpeed2_SubTickHandler(void)
{
  if (MenuManager_HighExtractSpeed2_InternalState == MENUMANAGER_HIGHEXTRACTSPEED2_INTERNALSTATE_EXITING)
  {
    MenuManager_HighExtractSpeed2_Counter += (uint32_t)1U;

    if (MenuManager_HighExtractSpeed2_Counter >= MENUMANAGER_HIGHEXTRACTSPEED2_COUNTER_MAX)
    {
      MenuManager_HighExtractSpeed2_Counter = (uint32_t)0U;
      
      Fsm_TriggerEvent(&MenuManager_FsmContext, (tFsmEvent)MENUMANAGER_EVENT_SUBMENU_1);
    }
  }
}



#ifdef  __cplusplus
}
#endif
