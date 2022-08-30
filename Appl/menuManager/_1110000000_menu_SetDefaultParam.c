/* 
 * File:   _1110000000_menu_SetDefaultParam.c
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
#define MENUMANAGER_SETDEFAULTPARAM_INTERNALDATALENGTH                (uint8_t)2U

#define MENUMANAGER_SETDEFAULTPARAM_INTERNALSTATE_IDX                 0U
#define MENUMANAGER_SETDEFAULTPARAM_COUNTER_IDX                       1U

#define MenuManager_SetDefaultParam_InternalState                     MenuManager_GetInternalDataPtr(MENUMANAGER_SETDEFAULTPARAM_INTERNALSTATE_IDX)
#define MenuManager_SetDefaultParam_Counter                           MenuManager_GetInternalDataPtr(MENUMANAGER_SETDEFAULTPARAM_COUNTER_IDX)



#define MENUMANAGER_SETDEFAULTPARAM_INTERNALSTATE_READY               (uint32_t)0U
#define MENUMANAGER_SETDEFAULTPARAM_INTERNALSTATE_RESETTING           (uint32_t)1U
#define MENUMANAGER_SETDEFAULTPARAM_INTERNALSTATE_DONE                (uint32_t)2U
#define MENUMANAGER_SETDEFAULTPARAM_INTERNALSTATE_EXITING             (uint32_t)3U

#define MENUMANAGER_SETDEFAULTPARAM_COUNTER_MAX                       (uint32_t)50U



/** Menu manager main titles and child menu titles */
const uint8_t MenuManager_SetDefaultParam_MainTitleStr[] =            "PARAM DEFAULT";

const uint8_t MenuManager_SetDefaultParam_Notify1Str[] =              "RESET PARAMS BACK";
const uint8_t MenuManager_SetDefaultParam_Notify2Str[] =              "TO FACTORY DEFAULT";



/** Menu manager button event mapping array */
static MenuManager_ButEventMapStruct MenuManager_SetDefaultParam_ButEventMap[2] =
{
  { IOMANAGER_BUTTON_STATE_START,                                     MENUMANAGER_EVENT_START_BUT             },
  { IOMANAGER_BUTTON_STATE_STOP,                                      MENUMANAGER_EVENT_STOP_BUT              }
};

/** Menu manager button event mapping configuration */
static MenuManager_ButEventMapConfStruct MenuManager_SetDefaultParam_ButEventMapConf =
{
  .butEventMapNum             = (uint8_t)2U,
  .butEventMapCfg             = &MenuManager_SetDefaultParam_ButEventMap
};



/** Menu manager event handlers */
static tFsmGuard MenuManager_SetDefaultParam_Entry                    (tFsmContextPtr const pFsmContext, tFsmEvent event);
static tFsmGuard MenuManager_SetDefaultParam_Exit                     (tFsmContextPtr const pFsmContext, tFsmEvent event);
static tFsmGuard MenuManager_SetDefaultParam_Submenu1                 (tFsmContextPtr const pFsmContext, tFsmEvent event);
static tFsmGuard MenuManager_SetDefaultParam_StartBut                 (tFsmContextPtr const pFsmContext, tFsmEvent event);
static tFsmGuard MenuManager_SetDefaultParam_StopBut                  (tFsmContextPtr const pFsmContext, tFsmEvent event);

/** Menu manager state machine */
tFsmEventEntry MenuManager_SetDefaultParam_StateMachine[5] =
{
  FSM_TRIGGER_ENTRY             (                                     MenuManager_SetDefaultParam_Entry                                               ),
  FSM_TRIGGER_EXIT              (                                     MenuManager_SetDefaultParam_Exit                                                ),
  FSM_TRIGGER_TRANSITION_ACTION ( MENUMANAGER_EVENT_SUBMENU_1,        MenuManager_SetDefaultParam_Submenu1,   MENUMANAGER_STATE_SET_TO_DEFAULT        ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_START_BUT,        MenuManager_SetDefaultParam_StartBut                                            ),
  FSM_TRIGGER_TRANSITION_ACTION ( MENUMANAGER_EVENT_STOP_BUT,         MenuManager_SetDefaultParam_StopBut,    MENUMANAGER_STATE_SET_TO_DEFAULT        )
};



/*===============================================================================================
*                                   LOCAL FUNCTION PROTOTYPES
===============================================================================================*/

void MenuManager_SetDefaultParam_LcdShowMainTitle(void);
void MenuManager_SetDefaultParam_LcdShowNotify(void);
void MenuManager_SetDefaultParam_LcdShowDone(void);

static void MenuManager_SetDefaultParam_SubMainFunction(void);
static void MenuManager_SetDefaultParam_SubTickHandler(void);



/*===============================================================================================
*                                       LOCAL FUNCTIONS
===============================================================================================*/

void MenuManager_SetDefaultParam_LcdShowMainTitle(void)
{
  /* Print main title */
  MenuManager_Common_LcdClearElementMenuStatic();
  MenuManager_Common_LcdShowMainTitle(MenuManager_SetDefaultParam_MainTitleStr);
  MenuManager_Common_LcdUpdateElementMenuStatic();
}

/*=============================================================================================*/
void MenuManager_SetDefaultParam_LcdShowNotify(void)
{
  MenuManager_Common_LcdClearElementMenuDynamic();

  LCD_SetFont(LCD_FONT_SMALL);
  LCD_SetCursorPos(1, 3, LCD_CURSOR_BY_FONT);
  LCD_PutString((uint8_t *)MenuManager_SetDefaultParam_Notify1Str);
  LCD_SetCursorPos(1, 4, LCD_CURSOR_BY_FONT);
  LCD_PutString((uint8_t *)MenuManager_SetDefaultParam_Notify2Str);

  MenuManager_Common_LcdShowOkExitChoice();

  MenuManager_Common_LcdUpdateElementMenuDynamic();
}

/*=============================================================================================*/
void MenuManager_SetDefaultParam_LcdShowDone(void)
{
  MenuManager_Common_LcdClearElementMenuDynamic();

  LCD_SetFont(LCD_FONT_SMALL);
  LCD_SetCursorPos(1, 3, LCD_CURSOR_BY_FONT);
  LCD_PutString((uint8_t *)MenuManager_Common_DoneStr);

  MenuManager_Common_LcdUpdateElementMenuDynamic();
}



/*=============================================================================================*/
static tFsmGuard MenuManager_SetDefaultParam_Entry(tFsmContextPtr const pFsmContext, tFsmEvent event)
{
  MenuManager_SubMainFunction = MenuManager_SetDefaultParam_SubMainFunction;
  MenuManager_SubTickHandler = MenuManager_SetDefaultParam_SubTickHandler;

  /* Check if previous state data hierachy is not empty */
  if (pFsmContext->dataHierachy != NULL)
  {
    if (pFsmContext->dataHierachy->dataId == MENUMANAGER_STATE_SET_TO_DEFAULT)
    {
      /* Release previous state data hierachy */
      MenuManager_free(pFsmContext->dataHierachy);
      pFsmContext->dataHierachy = NULL;

      MenuManager_InternalDataPush(MENUMANAGER_SETDEFAULTPARAM_INTERNALDATALENGTH);

      MenuManager_SetDefaultParam_InternalState = MENUMANAGER_SETDEFAULTPARAM_INTERNALSTATE_READY;
      MenuManager_SetDefaultParam_Counter = (uint32_t)0U;
    }
    else
    {
      return kFsmGuard_False;
    }

    MenuManager_SetDefaultParam_LcdShowMainTitle();
    MenuManager_SetDefaultParam_LcdShowNotify();

    return kFsmGuard_True;
  }

  return kFsmGuard_False;
}

/*=============================================================================================*/
static tFsmGuard MenuManager_SetDefaultParam_Exit(tFsmContextPtr const pFsmContext, tFsmEvent event)
{
  MenuManager_SubMainFunction = NULL;
  MenuManager_SubTickHandler = NULL;

  tFsmDataHierachy* dataHierachy;

  dataHierachy = (tFsmDataHierachy *)MenuManager_malloc(sizeof(tFsmDataHierachy));
  dataHierachy->dataId = MENUMANAGER_STATE_SET_DEFAULT_PARAM;

  pFsmContext->dataHierachy = dataHierachy;

  /* Free internal data */
  MenuManager_InternalDataPop();

  return kFsmGuard_True;
}

/*=============================================================================================*/
static tFsmGuard MenuManager_SetDefaultParam_Submenu1(tFsmContextPtr const pFsmContext, tFsmEvent event)
{

  
  return kFsmGuard_True;
}

/*=============================================================================================*/
static tFsmGuard MenuManager_SetDefaultParam_StartBut(tFsmContextPtr const pFsmContext, tFsmEvent event)
{
  if (MenuManager_SetDefaultParam_InternalState == MENUMANAGER_SETDEFAULTPARAM_INTERNALSTATE_READY)
  {
    MenuManager_SetDefaultParam_InternalState = MENUMANAGER_SETDEFAULTPARAM_INTERNALSTATE_RESETTING;
  }
  
  return kFsmGuard_True;
}

/*=============================================================================================*/
static tFsmGuard MenuManager_SetDefaultParam_StopBut(tFsmContextPtr const pFsmContext, tFsmEvent event)
{
  if (MenuManager_SetDefaultParam_InternalState != MENUMANAGER_SETDEFAULTPARAM_INTERNALSTATE_READY)
  {
    return kFsmGuard_False;
  }
  
  return kFsmGuard_True;
}



/*=============================================================================================*/
static void MenuManager_SetDefaultParam_SubMainFunction(void)
{
  MenuManager_Common_ButEventMapHandler(&MenuManager_SetDefaultParam_ButEventMapConf);

  switch (MenuManager_SetDefaultParam_InternalState)
  {
    case MENUMANAGER_SETDEFAULTPARAM_INTERNALSTATE_READY:
      break;
    case MENUMANAGER_SETDEFAULTPARAM_INTERNALSTATE_RESETTING:
    {
      /* Initialize program database by setting default factory value */
      ProgramManager_ParamConfigSetup_SetData((ProgramManager_ParamConfigSetupStruct *)&ProgramManager_gParamDefConfig);

      /* Read program parameters and current sequence */
      ProgramManager_ParamConfigSetup_GetData(&ProgramManager_gParamConfig);

      MenuManager_SetDefaultParam_InternalState = MENUMANAGER_SETDEFAULTPARAM_INTERNALSTATE_DONE;
      
      break;
    }
    case MENUMANAGER_SETDEFAULTPARAM_INTERNALSTATE_DONE:
    {
      MenuManager_SetDefaultParam_LcdShowDone();

      MenuManager_SetDefaultParam_InternalState = MENUMANAGER_SETDEFAULTPARAM_INTERNALSTATE_EXITING;

      break;
    }
    case MENUMANAGER_SETDEFAULTPARAM_INTERNALSTATE_EXITING:
    default:
      break;
  }
}

/*=============================================================================================*/
static void MenuManager_SetDefaultParam_SubTickHandler(void)
{
  if (MenuManager_SetDefaultParam_InternalState == MENUMANAGER_SETDEFAULTPARAM_INTERNALSTATE_EXITING)
  {
    MenuManager_SetDefaultParam_Counter += (uint32_t)1U;

    if (MenuManager_SetDefaultParam_Counter >= MENUMANAGER_SETDEFAULTPARAM_COUNTER_MAX)
    {
      MenuManager_SetDefaultParam_Counter = (uint32_t)0U;
      
      Fsm_TriggerEvent(&MenuManager_FsmContext, (tFsmEvent)MENUMANAGER_EVENT_SUBMENU_1);
    }
  }
}



#ifdef  __cplusplus
}
#endif
