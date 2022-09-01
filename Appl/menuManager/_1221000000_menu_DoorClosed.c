/* 
 * File:   _1221000000_menu_DoorClosed.c
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
#define MENUMANAGER_DOORCLOSED_INTERNALDATALENGTH                     (uint8_t)4U

#define MENUMANAGER_DOORCLOSED_INTERNALSTATE_IDX                      0U
#define MENUMANAGER_DOORCLOSED_COUNTER_IDX                            1U
#define MENUMANAGER_DOORCLOSED_LISTINDEX_IDX                          2U
#define MENUMANAGER_DOORCLOSED_CURPOS_IDX                             3U

#define MenuManager_DoorClosed_InternalState                          MenuManager_GetInternalDataPtr(MENUMANAGER_DOORCLOSED_INTERNALSTATE_IDX)
#define MenuManager_DoorClosed_Counter                                MenuManager_GetInternalDataPtr(MENUMANAGER_DOORCLOSED_COUNTER_IDX)
#define MenuManager_DoorClosed_ListIndex                              MenuManager_GetInternalDataPtr(MENUMANAGER_DOORCLOSED_LISTINDEX_IDX)
#define MenuManager_DoorClosed_CurPos                                 MenuManager_GetInternalDataPtr(MENUMANAGER_DOORCLOSED_CURPOS_IDX)



#define MENUMANAGER_DOORCLOSED_INTERNALSTATE_READY                    (uint32_t)0U
#define MENUMANAGER_DOORCLOSED_INTERNALSTATE_RUNNING                  (uint32_t)1U
#define MENUMANAGER_DOORCLOSED_INTERNALSTATE_DONE                     (uint32_t)2U
#define MENUMANAGER_DOORCLOSED_INTERNALSTATE_EXITING                  (uint32_t)3U

#define MENUMANAGER_DOORCLOSED_COUNTER_MAX                            (uint32_t)50U

#define MENUMANAGER_DOORCLOSED_LISTINDEX_LOW                          (uint32_t)0U
#define MENUMANAGER_DOORCLOSED_LISTINDEX_HIGH                         (uint32_t)1U



/* Internal value mapping structure */
typedef struct
{
  uint32_t                    listIndex;
  ProgramManager_InputActiveStatusType mapValue;
} MenuManager_DoorClosed_IntValMapStruct;

static MenuManager_DoorClosed_IntValMapStruct MenuManager_DoorClosed_IntValMapConf[2] =
{
  { MENUMANAGER_DOORCLOSED_LISTINDEX_LOW,                             PROGRAMMANAGER_INPUT_ACTIVE_STAT_LOW    },
  { MENUMANAGER_DOORCLOSED_LISTINDEX_HIGH,                            PROGRAMMANAGER_INPUT_ACTIVE_STAT_HIGH   }
};



/** Menu manager main titles and child menu titles */
static const uint8_t MenuManager_DoorClosed_MainTitleStr[] =          "DOOR CLOSED";

/** Menu manager child menu array */
static MenuManager_ChildMenuStruct MenuManager_DoorClosed_ChildMenu[2] =
{
  { &MenuManager_Common_ActiveLowStr,                                 MENUMANAGER_EVENT_SUBMENU_1             },
  { &MenuManager_Common_ActiveHighStr,                                MENUMANAGER_EVENT_SUBMENU_1             }
};

/** Menu manager child menu configuration */
static MenuManager_ChildMenuConfStruct MenuManager_DoorClosed_ChildMenuConf =
{
  .childMenuNum               = (uint8_t)2U,
  .childMenuCfg               = &MenuManager_DoorClosed_ChildMenu
};



/** Menu manager button event mapping array */
static MenuManager_ButEventMapStruct MenuManager_DoorClosed_ButEventMap[4] =
{
  { IOMANAGER_BUTTON_STATE_START,                                     MENUMANAGER_EVENT_START_BUT             },
  { IOMANAGER_BUTTON_STATE_STOP,                                      MENUMANAGER_EVENT_STOP_BUT              },
  { IOMANAGER_BUTTON_STATE_UP,                                        MENUMANAGER_EVENT_UP_BUT                },
  { IOMANAGER_BUTTON_STATE_DOWN,                                      MENUMANAGER_EVENT_DOWN_BUT              }
};

/** Menu manager button event mapping configuration */
static MenuManager_ButEventMapConfStruct MenuManager_DoorClosed_ButEventMapConf =
{
  .butEventMapNum             = (uint8_t)4U,
  .butEventMapCfg             = &MenuManager_DoorClosed_ButEventMap
};



/** Menu manager event handlers */
static tFsmGuard MenuManager_DoorClosed_Entry                         (tFsmContextPtr const pFsmContext, tFsmEvent event);
static tFsmGuard MenuManager_DoorClosed_Exit                          (tFsmContextPtr const pFsmContext, tFsmEvent event);
static tFsmGuard MenuManager_DoorClosed_Submenu1                      (tFsmContextPtr const pFsmContext, tFsmEvent event);
static tFsmGuard MenuManager_DoorClosed_StartBut                      (tFsmContextPtr const pFsmContext, tFsmEvent event);
static tFsmGuard MenuManager_DoorClosed_StopBut                       (tFsmContextPtr const pFsmContext, tFsmEvent event);
static tFsmGuard MenuManager_DoorClosed_UpBut                         (tFsmContextPtr const pFsmContext, tFsmEvent event);
static tFsmGuard MenuManager_DoorClosed_DownBut                       (tFsmContextPtr const pFsmContext, tFsmEvent event);

/** Menu manager state machine */
tFsmEventEntry MenuManager_DoorClosed_StateMachine[7] =
{
  FSM_TRIGGER_ENTRY             (                                     MenuManager_DoorClosed_Entry                                              ),
  FSM_TRIGGER_EXIT              (                                     MenuManager_DoorClosed_Exit                                               ),
  FSM_TRIGGER_TRANSITION_ACTION ( MENUMANAGER_EVENT_SUBMENU_1,        MenuManager_DoorClosed_Submenu1,  MENUMANAGER_STATE_INPUT_STATUS_SETUP    ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_START_BUT,        MenuManager_DoorClosed_StartBut                                           ),
  FSM_TRIGGER_TRANSITION_ACTION ( MENUMANAGER_EVENT_STOP_BUT,         MenuManager_DoorClosed_StopBut,   MENUMANAGER_STATE_INPUT_STATUS_SETUP    ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_UP_BUT,           MenuManager_DoorClosed_UpBut                                              ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_DOWN_BUT,         MenuManager_DoorClosed_DownBut                                            )
};



/*===============================================================================================
*                                   LOCAL FUNCTION PROTOTYPES
===============================================================================================*/

static void MenuManager_DoorClosed_LcdShowMainTitle(void);
static void MenuManager_DoorClosed_LcdShowList(void);
static void MenuManager_DoorClosed_LcdShowDone(void);

static void MenuManager_DoorClosed_SubMainFunction(void);
static void MenuManager_DoorClosed_SubTickHandler(void);



/*===============================================================================================
*                                       LOCAL FUNCTIONS
===============================================================================================*/

static void MenuManager_DoorClosed_LcdShowMainTitle(void)
{
  /* Print main title */
  MenuManager_Common_LcdClearElementMenuStatic();
  MenuManager_Common_LcdShowMainTitle(MenuManager_DoorClosed_MainTitleStr);
  MenuManager_Common_LcdUpdateElementMenuStatic();
}

/*=============================================================================================*/
static void MenuManager_DoorClosed_LcdShowList(void)
{
  uint32_t currentListIndex;
  uint32_t currentCursorPos;
  uint32_t listLength;
  
  currentListIndex = MenuManager_Common_LcdGetStartListIndex(MenuManager_DoorClosed_ListIndex, MenuManager_DoorClosed_CurPos);
  currentCursorPos = (uint32_t)MENUMANAGER_COMMON_LCD_CURSOR_MIN;

  listLength = MenuManager_Common_LcdGetListLength((uint32_t)(MenuManager_DoorClosed_ChildMenuConf.childMenuNum));
  
  MenuManager_Common_LcdClearElementMenuDynamic();
  
  for (; currentCursorPos < (listLength + (uint32_t)MENUMANAGER_COMMON_LCD_CURSOR_MIN); currentCursorPos++, currentListIndex++)
  {
    MenuManager_Common_LcdShowListElementName
    (
      (uint8_t *)*((*(MenuManager_DoorClosed_ChildMenuConf.childMenuCfg))[currentListIndex].childMenuTitleStr),
      MENUMANAGER_COMMON_LCD_ELEMENT_MENU_LENGTH,
      MENUMANAGER_COMMON_LCD_ELEMENT_MENU_XPOS,
      currentCursorPos
    );
  }
  
  MenuManager_Common_LcdShowChosenMark(MenuManager_DoorClosed_CurPos);
  
  MenuManager_Common_LcdShowScrollBar(MenuManager_DoorClosed_ListIndex, MenuManager_DoorClosed_CurPos, MenuManager_DoorClosed_ChildMenuConf.childMenuNum);
  
  MenuManager_Common_LcdUpdateElementMenuDynamic();
}

/*=============================================================================================*/
static void MenuManager_DoorClosed_LcdShowDone(void)
{
  MenuManager_Common_LcdClearAll();

  MenuManager_Common_LcdShowListElementName
  (
    (uint8_t *)*((*(MenuManager_DoorClosed_ChildMenuConf.childMenuCfg))[MenuManager_DoorClosed_ListIndex].childMenuTitleStr),
    MENUMANAGER_COMMON_LCD_ELEMENT_MENU_LENGTH,
    MENUMANAGER_COMMON_LCD_ELEMENT_MENU_XPOS,
    (uint8_t)MenuManager_DoorClosed_CurPos
  );

  MenuManager_Common_LcdShowChosenMark(MenuManager_DoorClosed_CurPos);

  MenuManager_Common_LcdUpdateAll();
}



/*=============================================================================================*/
static tFsmGuard MenuManager_DoorClosed_Entry(tFsmContextPtr const pFsmContext, tFsmEvent event)
{
  MenuManager_SubMainFunction = MenuManager_DoorClosed_SubMainFunction;
  MenuManager_SubTickHandler = MenuManager_DoorClosed_SubTickHandler;

  /* Check if previous state data hierachy is not empty */
  if (pFsmContext->dataHierachy != NULL)
  {
    if (pFsmContext->dataHierachy->dataId == MENUMANAGER_STATE_INPUT_STATUS_SETUP)
    {
      /* Release previous state data hierachy */
      MenuManager_free(pFsmContext->dataHierachy);
      pFsmContext->dataHierachy = NULL;

      MenuManager_InternalDataPush(MENUMANAGER_DOORCLOSED_INTERNALDATALENGTH);

      MenuManager_DoorClosed_InternalState = MENUMANAGER_DOORCLOSED_INTERNALSTATE_READY;
      MenuManager_DoorClosed_Counter = (uint32_t)0U;
      
      for ( MenuManager_DoorClosed_ListIndex = (uint32_t)0U; \
            MenuManager_DoorClosed_ListIndex < MenuManager_DoorClosed_ChildMenuConf.childMenuNum; \
            MenuManager_DoorClosed_ListIndex += (uint32_t)1U)
      {
        if (MenuManager_DoorClosed_IntValMapConf[MenuManager_DoorClosed_ListIndex].mapValue == ProgramManager_gParamConfig.inputStatusCfg.doorClosed)
        {
          break;
        }
      }
      
      MenuManager_DoorClosed_CurPos = MenuManager_Common_LcdGetCurPosFirstTime(MenuManager_DoorClosed_ListIndex, MenuManager_DoorClosed_ChildMenuConf.childMenuNum);
    }
    else
    {
      return kFsmGuard_False;
    }

    MenuManager_DoorClosed_LcdShowMainTitle();
    MenuManager_DoorClosed_LcdShowList();

    return kFsmGuard_True;
  }

  return kFsmGuard_False;
}

/*=============================================================================================*/
static tFsmGuard MenuManager_DoorClosed_Exit(tFsmContextPtr const pFsmContext, tFsmEvent event)
{
  MenuManager_SubMainFunction = NULL;
  MenuManager_SubTickHandler = NULL;

  tFsmDataHierachy* dataHierachy;

  dataHierachy = (tFsmDataHierachy *)MenuManager_malloc(sizeof(tFsmDataHierachy));
  dataHierachy->dataId = MENUMANAGER_STATE_DOOR_CLOSED;

  pFsmContext->dataHierachy = dataHierachy;

  /* Free internal data */
  MenuManager_InternalDataPop();
  
  return kFsmGuard_True;
}

/*=============================================================================================*/
static tFsmGuard MenuManager_DoorClosed_Submenu1(tFsmContextPtr const pFsmContext, tFsmEvent event)
{
  
  
  return kFsmGuard_True;
}

/*=============================================================================================*/
static tFsmGuard MenuManager_DoorClosed_StartBut(tFsmContextPtr const pFsmContext, tFsmEvent event)
{
  if (MenuManager_DoorClosed_InternalState == MENUMANAGER_DOORCLOSED_INTERNALSTATE_READY)
  {
    MenuManager_DoorClosed_InternalState = MENUMANAGER_DOORCLOSED_INTERNALSTATE_RUNNING;
  }
  
  return kFsmGuard_True;
}

/*=============================================================================================*/
static tFsmGuard MenuManager_DoorClosed_StopBut(tFsmContextPtr const pFsmContext, tFsmEvent event)
{
  if (MenuManager_DoorClosed_InternalState != MENUMANAGER_DOORCLOSED_INTERNALSTATE_READY)
  {
    return kFsmGuard_False;
  }
  
  return kFsmGuard_True;
}

/*=============================================================================================*/
static tFsmGuard MenuManager_DoorClosed_UpBut(tFsmContextPtr const pFsmContext, tFsmEvent event)
{
  if (MenuManager_DoorClosed_ListIndex > (uint32_t)0U)
  {
    MenuManager_DoorClosed_ListIndex -= (uint32_t)1U;
  }

  if (MenuManager_DoorClosed_CurPos > MENUMANAGER_COMMON_LCD_CURSOR_MIN)
  {
    MenuManager_DoorClosed_CurPos -= (uint32_t)1U;
  }

  MenuManager_DoorClosed_LcdShowList();
  
  return kFsmGuard_True;
}

/*=============================================================================================*/
static tFsmGuard MenuManager_DoorClosed_DownBut(tFsmContextPtr const pFsmContext, tFsmEvent event)
{
  if (MenuManager_DoorClosed_ListIndex < ((uint32_t)(MenuManager_DoorClosed_ChildMenuConf.childMenuNum) - (uint32_t)1U))
  {
    MenuManager_DoorClosed_ListIndex += (uint32_t)1U;
  }
  
  if (MenuManager_DoorClosed_CurPos < MenuManager_Common_LcdMaxCurPos(MenuManager_DoorClosed_ChildMenuConf.childMenuNum))
  {
    MenuManager_DoorClosed_CurPos += (uint32_t)1U;
  }
  
  MenuManager_DoorClosed_LcdShowList();
  
  return kFsmGuard_True;
}



/*=============================================================================================*/
static void MenuManager_DoorClosed_SubMainFunction(void)
{
  MenuManager_Common_ButEventMapHandler(&MenuManager_DoorClosed_ButEventMapConf);

  switch (MenuManager_DoorClosed_InternalState)
  {
    case MENUMANAGER_DOORCLOSED_INTERNALSTATE_READY:
      break;
    case MENUMANAGER_DOORCLOSED_INTERNALSTATE_RUNNING:
    {
      ProgramManager_InputStatusSetup_DoorClosed_SetData(&(MenuManager_DoorClosed_IntValMapConf[MenuManager_DoorClosed_ListIndex].mapValue));

      ProgramManager_InputStatusSetup_DoorClosed_GetData(&(ProgramManager_gParamConfig.inputStatusCfg.doorClosed));

      MenuManager_DoorClosed_InternalState = MENUMANAGER_DOORCLOSED_INTERNALSTATE_DONE;
      
      break;
    }
    case MENUMANAGER_DOORCLOSED_INTERNALSTATE_DONE:
    {
      MenuManager_DoorClosed_LcdShowDone();

      MenuManager_DoorClosed_InternalState = MENUMANAGER_DOORCLOSED_INTERNALSTATE_EXITING;

      break;
    }
    case MENUMANAGER_DOORCLOSED_INTERNALSTATE_EXITING:
    default:
      break;
  }
}

/*=============================================================================================*/
static void MenuManager_DoorClosed_SubTickHandler(void)
{
  if (MenuManager_DoorClosed_InternalState == MENUMANAGER_DOORCLOSED_INTERNALSTATE_EXITING)
  {
    MenuManager_DoorClosed_Counter += (uint32_t)1U;

    if (MenuManager_DoorClosed_Counter >= MENUMANAGER_DOORCLOSED_COUNTER_MAX)
    {
      MenuManager_DoorClosed_Counter = (uint32_t)0U;
      
      Fsm_TriggerEvent(&MenuManager_FsmContext, (tFsmEvent)MENUMANAGER_EVENT_SUBMENU_1);
    }
  }
}



#ifdef  __cplusplus
}
#endif
