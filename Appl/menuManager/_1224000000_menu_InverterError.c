/* 
 * File:   _1224000000_menu_InverterError.c
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
#define MENUMANAGER_INVERTERERROR_INTERNALDATALENGTH                  (uint8_t)4U

#define MENUMANAGER_INVERTERERROR_INTERNALSTATE_IDX                   0U
#define MENUMANAGER_INVERTERERROR_COUNTER_IDX                         1U
#define MENUMANAGER_INVERTERERROR_LISTINDEX_IDX                       2U
#define MENUMANAGER_INVERTERERROR_CURPOS_IDX                          3U

#define MenuManager_InverterError_InternalState                       MenuManager_GetInternalDataPtr(MENUMANAGER_INVERTERERROR_INTERNALSTATE_IDX)
#define MenuManager_InverterError_Counter                             MenuManager_GetInternalDataPtr(MENUMANAGER_INVERTERERROR_COUNTER_IDX)
#define MenuManager_InverterError_ListIndex                           MenuManager_GetInternalDataPtr(MENUMANAGER_INVERTERERROR_LISTINDEX_IDX)
#define MenuManager_InverterError_CurPos                              MenuManager_GetInternalDataPtr(MENUMANAGER_INVERTERERROR_CURPOS_IDX)



#define MENUMANAGER_INVERTERERROR_INTERNALSTATE_READY                 (uint32_t)0U
#define MENUMANAGER_INVERTERERROR_INTERNALSTATE_RUNNING               (uint32_t)1U
#define MENUMANAGER_INVERTERERROR_INTERNALSTATE_DONE                  (uint32_t)2U
#define MENUMANAGER_INVERTERERROR_INTERNALSTATE_EXITING               (uint32_t)3U

#define MENUMANAGER_INVERTERERROR_COUNTER_MAX                         (uint32_t)50U

#define MENUMANAGER_INVERTERERROR_LISTINDEX_LOW                       (uint32_t)0U
#define MENUMANAGER_INVERTERERROR_LISTINDEX_HIGH                      (uint32_t)1U



/* Internal value mapping structure */
typedef struct
{
  uint32_t                    listIndex;
  ProgramManager_InputActiveStatusType mapValue;
} MenuManager_InverterError_IntValMapStruct;

static MenuManager_InverterError_IntValMapStruct MenuManager_InverterError_IntValMapConf[2] =
{
  { MENUMANAGER_INVERTERERROR_LISTINDEX_LOW,                          PROGRAMMANAGER_INPUT_ACTIVE_STAT_LOW    },
  { MENUMANAGER_INVERTERERROR_LISTINDEX_HIGH,                         PROGRAMMANAGER_INPUT_ACTIVE_STAT_HIGH   }
};



/** Menu manager main titles and child menu titles */
static const uint8_t MenuManager_InverterError_MainTitleStr[] =       "INVERTER ERROR";

/** Menu manager child menu array */
static MenuManager_ChildMenuStruct MenuManager_InverterError_ChildMenu[2] =
{
  { &MenuManager_Common_ActiveLowStr,                                 MENUMANAGER_EVENT_SUBMENU_1             },
  { &MenuManager_Common_ActiveHighStr,                                MENUMANAGER_EVENT_SUBMENU_1             }
};

/** Menu manager child menu configuration */
static MenuManager_ChildMenuConfStruct MenuManager_InverterError_ChildMenuConf =
{
  .childMenuNum               = (uint8_t)2U,
  .childMenuCfg               = &MenuManager_InverterError_ChildMenu
};



/** Menu manager button event mapping array */
static MenuManager_ButEventMapStruct MenuManager_InverterError_ButEventMap[4] =
{
  { IOMANAGER_BUTTON_STATE_START,                                     MENUMANAGER_EVENT_START_BUT             },
  { IOMANAGER_BUTTON_STATE_STOP,                                      MENUMANAGER_EVENT_STOP_BUT              },
  { IOMANAGER_BUTTON_STATE_UP,                                        MENUMANAGER_EVENT_UP_BUT                },
  { IOMANAGER_BUTTON_STATE_DOWN,                                      MENUMANAGER_EVENT_DOWN_BUT              }
};

/** Menu manager button event mapping configuration */
static MenuManager_ButEventMapConfStruct MenuManager_InverterError_ButEventMapConf =
{
  .butEventMapNum             = (uint8_t)4U,
  .butEventMapCfg             = &MenuManager_InverterError_ButEventMap
};



/** Menu manager event handlers */
static Fsm_GuardType MenuManager_InverterError_Entry                      (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_InverterError_Exit                       (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_InverterError_Submenu1                   (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_InverterError_StartBut                   (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_InverterError_StopBut                    (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_InverterError_UpBut                      (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_InverterError_DownBut                    (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);

/** Menu manager state machine */
Fsm_EventEntryStruct MenuManager_InverterError_StateMachine[7] =
{
  FSM_TRIGGER_ENTRY             (                                     MenuManager_InverterError_Entry                                                 ),
  FSM_TRIGGER_EXIT              (                                     MenuManager_InverterError_Exit                                                  ),
  FSM_TRIGGER_TRANSITION_ACTION ( MENUMANAGER_EVENT_SUBMENU_1,        MenuManager_InverterError_Submenu1,     MENUMANAGER_STATE_INPUT_STATUS_SETUP    ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_START_BUT,        MenuManager_InverterError_StartBut                                              ),
  FSM_TRIGGER_TRANSITION_ACTION ( MENUMANAGER_EVENT_STOP_BUT,         MenuManager_InverterError_StopBut,      MENUMANAGER_STATE_INPUT_STATUS_SETUP    ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_UP_BUT,           MenuManager_InverterError_UpBut                                                 ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_DOWN_BUT,         MenuManager_InverterError_DownBut                                               )
};



/*===============================================================================================
*                                   LOCAL FUNCTION PROTOTYPES
===============================================================================================*/

static void MenuManager_InverterError_LcdShowMainTitle(void);
static void MenuManager_InverterError_LcdShowList(void);
static void MenuManager_InverterError_LcdShowDone(void);

static void MenuManager_InverterError_SubMainFunction(void);
static void MenuManager_InverterError_SubTickHandler(void);



/*===============================================================================================
*                                       LOCAL FUNCTIONS
===============================================================================================*/

static void MenuManager_InverterError_LcdShowMainTitle(void)
{
  /* Print main title */
  MenuManager_Common_LcdClearElementMenuStatic();
  MenuManager_Common_LcdShowMainTitle(MenuManager_InverterError_MainTitleStr);
  MenuManager_Common_LcdUpdateElementMenuStatic();
}

/*=============================================================================================*/
static void MenuManager_InverterError_LcdShowList(void)
{
  uint32_t currentListIndex;
  uint32_t currentCursorPos;
  uint32_t listLength;
  
  currentListIndex = MenuManager_Common_LcdGetStartListIndex(MenuManager_InverterError_ListIndex, MenuManager_InverterError_CurPos);
  currentCursorPos = (uint32_t)MENUMANAGER_COMMON_LCD_CURSOR_MIN;

  listLength = MenuManager_Common_LcdGetListLength((uint32_t)(MenuManager_InverterError_ChildMenuConf.childMenuNum));
  
  MenuManager_Common_LcdClearElementMenuDynamic();
  
  for (; currentCursorPos < (listLength + (uint32_t)MENUMANAGER_COMMON_LCD_CURSOR_MIN); currentCursorPos++, currentListIndex++)
  {
    MenuManager_Common_LcdShowListElementName
    (
      (uint8_t *)*((*(MenuManager_InverterError_ChildMenuConf.childMenuCfg))[currentListIndex].childMenuTitleStr),
      MENUMANAGER_COMMON_LCD_ELEMENT_MENU_LENGTH,
      MENUMANAGER_COMMON_LCD_ELEMENT_MENU_XPOS,
      currentCursorPos
    );
  }
  
  MenuManager_Common_LcdShowChosenMark(MenuManager_InverterError_CurPos);
  
  MenuManager_Common_LcdShowScrollBar(MenuManager_InverterError_ListIndex, MenuManager_InverterError_CurPos, MenuManager_InverterError_ChildMenuConf.childMenuNum);
  
  MenuManager_Common_LcdUpdateElementMenuDynamic();
}

/*=============================================================================================*/
static void MenuManager_InverterError_LcdShowDone(void)
{
  MenuManager_Common_LcdClearAll();

  MenuManager_Common_LcdShowListElementName
  (
    (uint8_t *)*((*(MenuManager_InverterError_ChildMenuConf.childMenuCfg))[MenuManager_InverterError_ListIndex].childMenuTitleStr),
    MENUMANAGER_COMMON_LCD_ELEMENT_MENU_LENGTH,
    MENUMANAGER_COMMON_LCD_ELEMENT_MENU_XPOS,
    (uint8_t)MenuManager_InverterError_CurPos
  );

  MenuManager_Common_LcdShowChosenMark(MenuManager_InverterError_CurPos);

  MenuManager_Common_LcdUpdateAll();
}



/*=============================================================================================*/
static Fsm_GuardType MenuManager_InverterError_Entry(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  MenuManager_SubMainFunction = MenuManager_InverterError_SubMainFunction;
  MenuManager_SubTickHandler = MenuManager_InverterError_SubTickHandler;

  /* Check if previous state data hierachy is not empty */
  if (pFsmContext->dataHierachy != NULL)
  {
    if (pFsmContext->dataHierachy->dataId == MENUMANAGER_STATE_INPUT_STATUS_SETUP)
    {
      /* Release previous state data hierachy */
      MenuManager_free(pFsmContext->dataHierachy);
      pFsmContext->dataHierachy = NULL;

      MenuManager_InternalDataPush(MENUMANAGER_INVERTERERROR_INTERNALDATALENGTH);

      MenuManager_InverterError_InternalState = MENUMANAGER_INVERTERERROR_INTERNALSTATE_READY;
      MenuManager_InverterError_Counter = (uint32_t)0U;
      
      for ( MenuManager_InverterError_ListIndex = (uint32_t)0U; \
            MenuManager_InverterError_ListIndex < MenuManager_InverterError_ChildMenuConf.childMenuNum; \
            MenuManager_InverterError_ListIndex += (uint32_t)1U)
      {
        if (MenuManager_InverterError_IntValMapConf[MenuManager_InverterError_ListIndex].mapValue == ProgramManager_gParamConfig.inputStatusCfg.inverterError)
        {
          break;
        }
      }
      
      MenuManager_InverterError_CurPos = MenuManager_Common_LcdGetCurPosFirstTime(MenuManager_InverterError_ListIndex, MenuManager_InverterError_ChildMenuConf.childMenuNum);
    }
    else
    {
      return FSM_GUARD_FALSE;
    }

    MenuManager_InverterError_LcdShowMainTitle();
    MenuManager_InverterError_LcdShowList();

    return FSM_GUARD_TRUE;
  }

  return FSM_GUARD_FALSE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_InverterError_Exit(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  MenuManager_SubMainFunction = NULL;
  MenuManager_SubTickHandler = NULL;

  Fsm_DataHierachyStruct* dataHierachy;

  dataHierachy = (Fsm_DataHierachyStruct *)MenuManager_malloc(sizeof(Fsm_DataHierachyStruct));
  dataHierachy->dataId = MENUMANAGER_STATE_INVERTER_ERROR;

  pFsmContext->dataHierachy = dataHierachy;

  /* Free internal data */
  MenuManager_InternalDataPop();
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_InverterError_Submenu1(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_InverterError_StartBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_InverterError_InternalState == MENUMANAGER_INVERTERERROR_INTERNALSTATE_READY)
  {
    MenuManager_InverterError_InternalState = MENUMANAGER_INVERTERERROR_INTERNALSTATE_RUNNING;
  }
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_InverterError_StopBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_InverterError_InternalState != MENUMANAGER_INVERTERERROR_INTERNALSTATE_READY)
  {
    return FSM_GUARD_FALSE;
  }
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_InverterError_UpBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_InverterError_ListIndex > (uint32_t)0U)
  {
    MenuManager_InverterError_ListIndex -= (uint32_t)1U;
  }

  if (MenuManager_InverterError_CurPos > MENUMANAGER_COMMON_LCD_CURSOR_MIN)
  {
    MenuManager_InverterError_CurPos -= (uint32_t)1U;
  }

  MenuManager_InverterError_LcdShowList();
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_InverterError_DownBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_InverterError_ListIndex < ((uint32_t)(MenuManager_InverterError_ChildMenuConf.childMenuNum) - (uint32_t)1U))
  {
    MenuManager_InverterError_ListIndex += (uint32_t)1U;
  }
  
  if (MenuManager_InverterError_CurPos < MenuManager_Common_LcdMaxCurPos(MenuManager_InverterError_ChildMenuConf.childMenuNum))
  {
    MenuManager_InverterError_CurPos += (uint32_t)1U;
  }
  
  MenuManager_InverterError_LcdShowList();
  
  return FSM_GUARD_TRUE;
}



/*=============================================================================================*/
static void MenuManager_InverterError_SubMainFunction(void)
{
  MenuManager_Common_ButEventMapHandler(&MenuManager_InverterError_ButEventMapConf);

  switch (MenuManager_InverterError_InternalState)
  {
    case MENUMANAGER_INVERTERERROR_INTERNALSTATE_READY:
      break;
    case MENUMANAGER_INVERTERERROR_INTERNALSTATE_RUNNING:
    {
      ProgramManager_InputStatusSetup_InverterError_SetData(&(MenuManager_InverterError_IntValMapConf[MenuManager_InverterError_ListIndex].mapValue));

      ProgramManager_InputStatusSetup_InverterError_GetData(&(ProgramManager_gParamConfig.inputStatusCfg.inverterError));

      MenuManager_InverterError_InternalState = MENUMANAGER_INVERTERERROR_INTERNALSTATE_DONE;
      
      break;
    }
    case MENUMANAGER_INVERTERERROR_INTERNALSTATE_DONE:
    {
      MenuManager_InverterError_LcdShowDone();

      MenuManager_InverterError_InternalState = MENUMANAGER_INVERTERERROR_INTERNALSTATE_EXITING;

      break;
    }
    case MENUMANAGER_INVERTERERROR_INTERNALSTATE_EXITING:
    default:
      break;
  }
}

/*=============================================================================================*/
static void MenuManager_InverterError_SubTickHandler(void)
{
  if (MenuManager_InverterError_InternalState == MENUMANAGER_INVERTERERROR_INTERNALSTATE_EXITING)
  {
    MenuManager_InverterError_Counter += (uint32_t)1U;

    if (MenuManager_InverterError_Counter >= MENUMANAGER_INVERTERERROR_COUNTER_MAX)
    {
      MenuManager_InverterError_Counter = (uint32_t)0U;
      
      Fsm_TriggerEvent(&MenuManager_FsmContext, (Fsm_EventType)MENUMANAGER_EVENT_SUBMENU_1);
    }
  }
}



#ifdef  __cplusplus
}
#endif
