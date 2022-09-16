/* 
 * File:   _1222000000_menu_ExtractShock.c
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
#define MENUMANAGER_EXTRACTSHOCK_INTERNALDATALENGTH                   (uint8_t)4U

#define MENUMANAGER_EXTRACTSHOCK_INTERNALSTATE_IDX                    0U
#define MENUMANAGER_EXTRACTSHOCK_COUNTER_IDX                          1U
#define MENUMANAGER_EXTRACTSHOCK_LISTINDEX_IDX                        2U
#define MENUMANAGER_EXTRACTSHOCK_CURPOS_IDX                           3U

#define MenuManager_ExtractShock_InternalState                        MenuManager_GetInternalDataPtr(MENUMANAGER_EXTRACTSHOCK_INTERNALSTATE_IDX)
#define MenuManager_ExtractShock_Counter                              MenuManager_GetInternalDataPtr(MENUMANAGER_EXTRACTSHOCK_COUNTER_IDX)
#define MenuManager_ExtractShock_ListIndex                            MenuManager_GetInternalDataPtr(MENUMANAGER_EXTRACTSHOCK_LISTINDEX_IDX)
#define MenuManager_ExtractShock_CurPos                               MenuManager_GetInternalDataPtr(MENUMANAGER_EXTRACTSHOCK_CURPOS_IDX)



#define MENUMANAGER_EXTRACTSHOCK_INTERNALSTATE_READY                  (uint32_t)0U
#define MENUMANAGER_EXTRACTSHOCK_INTERNALSTATE_RUNNING                (uint32_t)1U
#define MENUMANAGER_EXTRACTSHOCK_INTERNALSTATE_DONE                   (uint32_t)2U
#define MENUMANAGER_EXTRACTSHOCK_INTERNALSTATE_EXITING                (uint32_t)3U

#define MENUMANAGER_EXTRACTSHOCK_COUNTER_MAX                          (uint32_t)50U

#define MENUMANAGER_EXTRACTSHOCK_LISTINDEX_LOW                        (uint32_t)0U
#define MENUMANAGER_EXTRACTSHOCK_LISTINDEX_HIGH                       (uint32_t)1U



/* Internal value mapping structure */
typedef struct
{
  uint32_t                    listIndex;
  ProgramManager_InputActiveStatusType mapValue;
} MenuManager_ExtractShock_IntValMapStruct;

static MenuManager_ExtractShock_IntValMapStruct MenuManager_ExtractShock_IntValMapConf[2] =
{
  { MENUMANAGER_EXTRACTSHOCK_LISTINDEX_LOW,                           PROGRAMMANAGER_INPUT_ACTIVE_STAT_LOW    },
  { MENUMANAGER_EXTRACTSHOCK_LISTINDEX_HIGH,                          PROGRAMMANAGER_INPUT_ACTIVE_STAT_HIGH   }
};



/** Menu manager main titles and child menu titles */
static const uint8_t MenuManager_ExtractShock_MainTitleStr[] =        "EXTRACT SHOCK";

/** Menu manager child menu array */
static MenuManager_ChildMenuStruct MenuManager_ExtractShock_ChildMenu[2] =
{
  { &MenuManager_Common_ActiveLowStr,                                 MENUMANAGER_EVENT_PREV                  },
  { &MenuManager_Common_ActiveHighStr,                                MENUMANAGER_EVENT_PREV                  }
};

/** Menu manager child menu configuration */
static MenuManager_ChildMenuConfStruct MenuManager_ExtractShock_ChildMenuConf =
{
  .childMenuNum               = (uint8_t)2U,
  .childMenuCfg               = &MenuManager_ExtractShock_ChildMenu
};



/** Menu manager button event mapping array */
static MenuManager_ButEventMapStruct MenuManager_ExtractShock_ButEventMap[4] =
{
  { IOMANAGER_BUTTON_STATE_START,                                     MENUMANAGER_EVENT_START_BUT             },
  { IOMANAGER_BUTTON_STATE_STOP,                                      MENUMANAGER_EVENT_STOP_BUT              },
  { IOMANAGER_BUTTON_STATE_UP,                                        MENUMANAGER_EVENT_UP_BUT                },
  { IOMANAGER_BUTTON_STATE_DOWN,                                      MENUMANAGER_EVENT_DOWN_BUT              }
};

/** Menu manager button event mapping configuration */
static MenuManager_ButEventMapConfStruct MenuManager_ExtractShock_ButEventMapConf =
{
  .butEventMapNum             = (uint8_t)4U,
  .butEventMapCfg             = &MenuManager_ExtractShock_ButEventMap
};



/** Menu manager event handlers */
static Fsm_GuardType MenuManager_ExtractShock_Entry                   (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_ExtractShock_Exit                    (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_ExtractShock_StartBut                (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_ExtractShock_StopBut                 (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_ExtractShock_UpBut                   (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_ExtractShock_DownBut                 (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);

/** Menu manager state machine */
Fsm_EventEntryStruct MenuManager_ExtractShock_StateMachine[7] =
{
  FSM_TRIGGER_ENTRY             (                                     MenuManager_ExtractShock_Entry                                                  ),
  FSM_TRIGGER_EXIT              (                                     MenuManager_ExtractShock_Exit                                                   ),
  FSM_TRIGGER_TRANSITION        ( MENUMANAGER_EVENT_PREV,                                                     MENUMANAGER_STATE_INPUT_STATUS_SETUP    ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_START_BUT,        MenuManager_ExtractShock_StartBut                                               ),
  FSM_TRIGGER_TRANSITION_ACTION ( MENUMANAGER_EVENT_STOP_BUT,         MenuManager_ExtractShock_StopBut,       MENUMANAGER_STATE_INPUT_STATUS_SETUP    ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_UP_BUT,           MenuManager_ExtractShock_UpBut                                                  ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_DOWN_BUT,         MenuManager_ExtractShock_DownBut                                                )
};



/*===============================================================================================
*                                   LOCAL FUNCTION PROTOTYPES
===============================================================================================*/

static void MenuManager_ExtractShock_LcdShowMainTitle(void);
static void MenuManager_ExtractShock_LcdShowList(void);
static void MenuManager_ExtractShock_LcdShowDone(void);

static void MenuManager_ExtractShock_SubMainFunction(void);
static void MenuManager_ExtractShock_SubTickHandler(void);



/*===============================================================================================
*                                       LOCAL FUNCTIONS
===============================================================================================*/

static void MenuManager_ExtractShock_LcdShowMainTitle(void)
{
  /* Print main title */
  MenuManager_Common_LcdClearElementMenuStatic();
  MenuManager_Common_LcdShowMainTitle(MenuManager_ExtractShock_MainTitleStr);
  MenuManager_Common_LcdUpdateElementMenuStatic();
}

/*=============================================================================================*/
static void MenuManager_ExtractShock_LcdShowList(void)
{
  uint32_t currentListIndex;
  uint32_t currentCursorPos;
  uint32_t listLength;
  
  currentListIndex = MenuManager_Common_LcdGetStartListIndex(MenuManager_ExtractShock_ListIndex, MenuManager_ExtractShock_CurPos);
  currentCursorPos = (uint32_t)MENUMANAGER_COMMON_LCD_CURSOR_MIN;

  listLength = MenuManager_Common_LcdGetListLength((uint32_t)(MenuManager_ExtractShock_ChildMenuConf.childMenuNum));
  
  MenuManager_Common_LcdClearElementMenuDynamic();
  
  for (; currentCursorPos < (listLength + (uint32_t)MENUMANAGER_COMMON_LCD_CURSOR_MIN); currentCursorPos++, currentListIndex++)
  {
    MenuManager_Common_LcdShowListElementName
    (
      (uint8_t *)*((*(MenuManager_ExtractShock_ChildMenuConf.childMenuCfg))[currentListIndex].childMenuTitleStr),
      MENUMANAGER_COMMON_LCD_ELEMENT_MENU_LENGTH,
      MENUMANAGER_COMMON_LCD_ELEMENT_MENU_XPOS,
      currentCursorPos
    );
  }
  
  MenuManager_Common_LcdShowChosenMark(MenuManager_ExtractShock_CurPos);
  
  MenuManager_Common_LcdShowScrollBar(MenuManager_ExtractShock_ListIndex, MenuManager_ExtractShock_CurPos, MenuManager_ExtractShock_ChildMenuConf.childMenuNum);
  
  MenuManager_Common_LcdUpdateElementMenuDynamic();
}

/*=============================================================================================*/
static void MenuManager_ExtractShock_LcdShowDone(void)
{
  MenuManager_Common_LcdClearAll();

  MenuManager_Common_LcdShowListElementName
  (
    (uint8_t *)*((*(MenuManager_ExtractShock_ChildMenuConf.childMenuCfg))[MenuManager_ExtractShock_ListIndex].childMenuTitleStr),
    MENUMANAGER_COMMON_LCD_ELEMENT_MENU_LENGTH,
    MENUMANAGER_COMMON_LCD_ELEMENT_MENU_XPOS,
    (uint8_t)MenuManager_ExtractShock_CurPos
  );

  MenuManager_Common_LcdShowChosenMark(MenuManager_ExtractShock_CurPos);

  MenuManager_Common_LcdUpdateAll();
}



/*=============================================================================================*/
static Fsm_GuardType MenuManager_ExtractShock_Entry(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  HAL_StatusTypeDef retVal = HAL_OK;

  /* Check if previous state data hierachy is not empty */
  if (pFsmContext->dataHierachy != NULL)
  {
    if (pFsmContext->dataHierachy->dataId == MENUMANAGER_STATE_INPUT_STATUS_SETUP)
    {
      /* Release previous state data hierachy */
      MenuManager_free(pFsmContext->dataHierachy);
      pFsmContext->dataHierachy = NULL;

      MenuManager_InternalDataPush(MENUMANAGER_EXTRACTSHOCK_INTERNALDATALENGTH);

      MenuManager_ExtractShock_InternalState = MENUMANAGER_EXTRACTSHOCK_INTERNALSTATE_READY;
      MenuManager_ExtractShock_Counter = (uint32_t)0U;
      
      for ( MenuManager_ExtractShock_ListIndex = (uint32_t)0U; \
            MenuManager_ExtractShock_ListIndex < MenuManager_ExtractShock_ChildMenuConf.childMenuNum; \
            MenuManager_ExtractShock_ListIndex += (uint32_t)1U)
      {
        if (MenuManager_ExtractShock_IntValMapConf[MenuManager_ExtractShock_ListIndex].mapValue == ProgramManager_gParamConfig.inputStatusCfg.extractShock)
        {
          break;
        }
      }
      
      MenuManager_ExtractShock_CurPos = MenuManager_Common_LcdGetCurPosFirstTime(MenuManager_ExtractShock_ListIndex, MenuManager_ExtractShock_ChildMenuConf.childMenuNum);
    }
    else
    {
      retVal = HAL_ERROR;
    }
  }
  else
  {
    retVal = HAL_ERROR;
  }

  if (retVal == HAL_OK)
  {
    MenuManager_ExtractShock_LcdShowMainTitle();
    MenuManager_ExtractShock_LcdShowList();

    MenuManager_SubMainFunction = MenuManager_ExtractShock_SubMainFunction;
    MenuManager_SubTickHandler = MenuManager_ExtractShock_SubTickHandler;

    return FSM_GUARD_TRUE;
  }

  return FSM_GUARD_FALSE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_ExtractShock_Exit(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  Fsm_DataHierachyStruct* dataHierachy;

  MenuManager_SubMainFunction = NULL;
  MenuManager_SubTickHandler = NULL;

  dataHierachy = (Fsm_DataHierachyStruct *)MenuManager_malloc(sizeof(Fsm_DataHierachyStruct));
  dataHierachy->dataId = MENUMANAGER_STATE_EXTRACT_SHOCK;

  pFsmContext->dataHierachy = dataHierachy;

  /* Free internal data */
  MenuManager_InternalDataPop();
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_ExtractShock_StartBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_ExtractShock_InternalState == MENUMANAGER_EXTRACTSHOCK_INTERNALSTATE_READY)
  {
    MenuManager_ExtractShock_InternalState = MENUMANAGER_EXTRACTSHOCK_INTERNALSTATE_RUNNING;
  }
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_ExtractShock_StopBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_ExtractShock_InternalState != MENUMANAGER_EXTRACTSHOCK_INTERNALSTATE_READY)
  {
    return FSM_GUARD_FALSE;
  }
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_ExtractShock_UpBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_ExtractShock_ListIndex > (uint32_t)0U)
  {
    MenuManager_ExtractShock_ListIndex -= (uint32_t)1U;
  }

  if (MenuManager_ExtractShock_CurPos > MENUMANAGER_COMMON_LCD_CURSOR_MIN)
  {
    MenuManager_ExtractShock_CurPos -= (uint32_t)1U;
  }

  MenuManager_ExtractShock_LcdShowList();
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_ExtractShock_DownBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_ExtractShock_ListIndex < ((uint32_t)(MenuManager_ExtractShock_ChildMenuConf.childMenuNum) - (uint32_t)1U))
  {
    MenuManager_ExtractShock_ListIndex += (uint32_t)1U;
  }
  
  if (MenuManager_ExtractShock_CurPos < MenuManager_Common_LcdMaxCurPos(MenuManager_ExtractShock_ChildMenuConf.childMenuNum))
  {
    MenuManager_ExtractShock_CurPos += (uint32_t)1U;
  }
  
  MenuManager_ExtractShock_LcdShowList();
  
  return FSM_GUARD_TRUE;
}



/*=============================================================================================*/
static void MenuManager_ExtractShock_SubMainFunction(void)
{
  MenuManager_Common_ButEventMapHandler(&MenuManager_ExtractShock_ButEventMapConf);

  switch (MenuManager_ExtractShock_InternalState)
  {
    case MENUMANAGER_EXTRACTSHOCK_INTERNALSTATE_READY:
      break;
    case MENUMANAGER_EXTRACTSHOCK_INTERNALSTATE_RUNNING:
    {
      ProgramManager_InputStatusSetup_ExtractShock_SetData(&(MenuManager_ExtractShock_IntValMapConf[MenuManager_ExtractShock_ListIndex].mapValue));

      ProgramManager_InputStatusSetup_ExtractShock_GetData(&(ProgramManager_gParamConfig.inputStatusCfg.extractShock));

      MenuManager_ExtractShock_InternalState = MENUMANAGER_EXTRACTSHOCK_INTERNALSTATE_DONE;
      
      break;
    }
    case MENUMANAGER_EXTRACTSHOCK_INTERNALSTATE_DONE:
    {
      MenuManager_ExtractShock_LcdShowDone();

      MenuManager_ExtractShock_InternalState = MENUMANAGER_EXTRACTSHOCK_INTERNALSTATE_EXITING;

      break;
    }
    case MENUMANAGER_EXTRACTSHOCK_INTERNALSTATE_EXITING:
    default:
      break;
  }
}

/*=============================================================================================*/
static void MenuManager_ExtractShock_SubTickHandler(void)
{
  if (MenuManager_ExtractShock_InternalState == MENUMANAGER_EXTRACTSHOCK_INTERNALSTATE_EXITING)
  {
    MenuManager_ExtractShock_Counter += (uint32_t)1U;

    if (MenuManager_ExtractShock_Counter >= MENUMANAGER_EXTRACTSHOCK_COUNTER_MAX)
    {
      MenuManager_ExtractShock_Counter = (uint32_t)0U;
      
      Fsm_TriggerEvent( &MenuManager_FsmContext, \
                        (Fsm_EventType)((*(MenuManager_ExtractShock_ChildMenuConf.childMenuCfg))[MenuManager_ExtractShock_ListIndex].childMenuEvent));
    }
  }
}



#ifdef  __cplusplus
}
#endif
