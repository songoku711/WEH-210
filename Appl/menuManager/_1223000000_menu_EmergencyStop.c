/* 
 * File:   _1223000000_menu_EmergencyStop.c
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
#define MENUMANAGER_EMERGENCYSTOP_INTERNALDATALENGTH                  (uint8_t)4U

#define MENUMANAGER_EMERGENCYSTOP_INTERNALSTATE_IDX                   0U
#define MENUMANAGER_EMERGENCYSTOP_COUNTER_IDX                         1U
#define MENUMANAGER_EMERGENCYSTOP_LISTINDEX_IDX                       2U
#define MENUMANAGER_EMERGENCYSTOP_CURPOS_IDX                          3U

#define MenuManager_EmergencyStop_InternalState                       MenuManager_GetInternalDataPtr(MENUMANAGER_EMERGENCYSTOP_INTERNALSTATE_IDX)
#define MenuManager_EmergencyStop_Counter                             MenuManager_GetInternalDataPtr(MENUMANAGER_EMERGENCYSTOP_COUNTER_IDX)
#define MenuManager_EmergencyStop_ListIndex                           MenuManager_GetInternalDataPtr(MENUMANAGER_EMERGENCYSTOP_LISTINDEX_IDX)
#define MenuManager_EmergencyStop_CurPos                              MenuManager_GetInternalDataPtr(MENUMANAGER_EMERGENCYSTOP_CURPOS_IDX)



#define MENUMANAGER_EMERGENCYSTOP_INTERNALSTATE_READY                 (uint32_t)0U
#define MENUMANAGER_EMERGENCYSTOP_INTERNALSTATE_RUNNING               (uint32_t)1U
#define MENUMANAGER_EMERGENCYSTOP_INTERNALSTATE_DONE                  (uint32_t)2U
#define MENUMANAGER_EMERGENCYSTOP_INTERNALSTATE_EXITING               (uint32_t)3U

#define MENUMANAGER_EMERGENCYSTOP_COUNTER_MAX                         (uint32_t)50U

#define MENUMANAGER_EMERGENCYSTOP_LISTINDEX_LOW                       (uint32_t)0U
#define MENUMANAGER_EMERGENCYSTOP_LISTINDEX_HIGH                      (uint32_t)1U



/* Internal value mapping structure */
typedef struct
{
  uint32_t                    listIndex;
  ProgramManager_InputActiveStatusType mapValue;
} MenuManager_EmergencyStop_IntValMapStruct;

static MenuManager_EmergencyStop_IntValMapStruct MenuManager_EmergencyStop_IntValMapConf[2] =
{
  { MENUMANAGER_EMERGENCYSTOP_LISTINDEX_LOW,                          PROGRAMMANAGER_INPUT_ACTIVE_STAT_LOW    },
  { MENUMANAGER_EMERGENCYSTOP_LISTINDEX_HIGH,                         PROGRAMMANAGER_INPUT_ACTIVE_STAT_HIGH   }
};



/** Menu manager main titles and child menu titles */
static const uint8_t MenuManager_EmergencyStop_MainTitleStr[] =       "EMERGENCY STOP";

/** Menu manager child menu array */
static MenuManager_ChildMenuStruct MenuManager_EmergencyStop_ChildMenu[2] =
{
  { &MenuManager_Common_ActiveLowStr,                                 MENUMANAGER_EVENT_PREV                  },
  { &MenuManager_Common_ActiveHighStr,                                MENUMANAGER_EVENT_PREV                  }
};

/** Menu manager child menu configuration */
static MenuManager_ChildMenuConfStruct MenuManager_EmergencyStop_ChildMenuConf =
{
  .childMenuNum               = (uint8_t)2U,
  .childMenuCfg               = &MenuManager_EmergencyStop_ChildMenu
};



/** Menu manager button event mapping array */
static MenuManager_ButEventMapStruct MenuManager_EmergencyStop_ButEventMap[4] =
{
  { IOMANAGER_BUTTON_STATE_START,                                     MENUMANAGER_EVENT_START_BUT             },
  { IOMANAGER_BUTTON_STATE_STOP,                                      MENUMANAGER_EVENT_STOP_BUT              },
  { IOMANAGER_BUTTON_STATE_UP,                                        MENUMANAGER_EVENT_UP_BUT                },
  { IOMANAGER_BUTTON_STATE_DOWN,                                      MENUMANAGER_EVENT_DOWN_BUT              }
};

/** Menu manager button event mapping configuration */
static MenuManager_ButEventMapConfStruct MenuManager_EmergencyStop_ButEventMapConf =
{
  .butEventMapNum             = (uint8_t)4U,
  .butEventMapCfg             = &MenuManager_EmergencyStop_ButEventMap
};



/** Menu manager event handlers */
static Fsm_GuardType MenuManager_EmergencyStop_Entry                  (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_EmergencyStop_Exit                   (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_EmergencyStop_StartBut               (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_EmergencyStop_StopBut                (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_EmergencyStop_UpBut                  (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_EmergencyStop_DownBut                (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);

/** Menu manager state machine */
Fsm_EventEntryStruct MenuManager_EmergencyStop_StateMachine[7] =
{
  FSM_TRIGGER_ENTRY             (                                     MenuManager_EmergencyStop_Entry                                                 ),
  FSM_TRIGGER_EXIT              (                                     MenuManager_EmergencyStop_Exit                                                  ),
  FSM_TRIGGER_TRANSITION        ( MENUMANAGER_EVENT_PREV,                                                     MENUMANAGER_STATE_INPUT_STATUS_SETUP    ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_START_BUT,        MenuManager_EmergencyStop_StartBut                                              ),
  FSM_TRIGGER_TRANSITION_ACTION ( MENUMANAGER_EVENT_STOP_BUT,         MenuManager_EmergencyStop_StopBut,      MENUMANAGER_STATE_INPUT_STATUS_SETUP    ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_UP_BUT,           MenuManager_EmergencyStop_UpBut                                                 ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_DOWN_BUT,         MenuManager_EmergencyStop_DownBut                                               )
};



/*===============================================================================================
*                                   LOCAL FUNCTION PROTOTYPES
===============================================================================================*/

static void MenuManager_EmergencyStop_LcdShowMainTitle(void);
static void MenuManager_EmergencyStop_LcdShowList(void);
static void MenuManager_EmergencyStop_LcdShowDone(void);

static void MenuManager_EmergencyStop_SubMainFunction(void);
static void MenuManager_EmergencyStop_SubTickHandler(void);



/*===============================================================================================
*                                       LOCAL FUNCTIONS
===============================================================================================*/

static void MenuManager_EmergencyStop_LcdShowMainTitle(void)
{
  /* Print main title */
  MenuManager_Common_LcdClearElementMenuStatic();
  MenuManager_Common_LcdShowMainTitle(MenuManager_EmergencyStop_MainTitleStr);
  MenuManager_Common_LcdUpdateElementMenuStatic();
}

/*=============================================================================================*/
static void MenuManager_EmergencyStop_LcdShowList(void)
{
  uint32_t currentListIndex;
  uint32_t currentCursorPos;
  uint32_t listLength;
  
  currentListIndex = MenuManager_Common_LcdGetStartListIndex(MenuManager_EmergencyStop_ListIndex, MenuManager_EmergencyStop_CurPos);
  currentCursorPos = (uint32_t)MENUMANAGER_COMMON_LCD_CURSOR_MIN;

  listLength = MenuManager_Common_LcdGetListLength((uint32_t)(MenuManager_EmergencyStop_ChildMenuConf.childMenuNum));
  
  MenuManager_Common_LcdClearElementMenuDynamic();
  
  for (; currentCursorPos < (listLength + (uint32_t)MENUMANAGER_COMMON_LCD_CURSOR_MIN); currentCursorPos++, currentListIndex++)
  {
    MenuManager_Common_LcdShowListElementName
    (
      (uint8_t *)*((*(MenuManager_EmergencyStop_ChildMenuConf.childMenuCfg))[currentListIndex].childMenuTitleStr),
      MENUMANAGER_COMMON_LCD_ELEMENT_MENU_LENGTH,
      MENUMANAGER_COMMON_LCD_ELEMENT_MENU_XPOS,
      currentCursorPos
    );
  }
  
  MenuManager_Common_LcdShowChosenMark(MenuManager_EmergencyStop_CurPos);
  
  MenuManager_Common_LcdShowScrollBar(MenuManager_EmergencyStop_ListIndex, MenuManager_EmergencyStop_CurPos, MenuManager_EmergencyStop_ChildMenuConf.childMenuNum);
  
  MenuManager_Common_LcdUpdateElementMenuDynamic();
}

/*=============================================================================================*/
static void MenuManager_EmergencyStop_LcdShowDone(void)
{
  MenuManager_Common_LcdClearAll();

  MenuManager_Common_LcdShowListElementName
  (
    (uint8_t *)*((*(MenuManager_EmergencyStop_ChildMenuConf.childMenuCfg))[MenuManager_EmergencyStop_ListIndex].childMenuTitleStr),
    MENUMANAGER_COMMON_LCD_ELEMENT_MENU_LENGTH,
    MENUMANAGER_COMMON_LCD_ELEMENT_MENU_XPOS,
    (uint8_t)MenuManager_EmergencyStop_CurPos
  );

  MenuManager_Common_LcdShowChosenMark(MenuManager_EmergencyStop_CurPos);

  MenuManager_Common_LcdUpdateAll();
}



/*=============================================================================================*/
static Fsm_GuardType MenuManager_EmergencyStop_Entry(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
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

      MenuManager_InternalDataPush(MENUMANAGER_EMERGENCYSTOP_INTERNALDATALENGTH);

      MenuManager_EmergencyStop_InternalState = MENUMANAGER_EMERGENCYSTOP_INTERNALSTATE_READY;
      MenuManager_EmergencyStop_Counter = (uint32_t)0U;
      
      for ( MenuManager_EmergencyStop_ListIndex = (uint32_t)0U; \
            MenuManager_EmergencyStop_ListIndex < MenuManager_EmergencyStop_ChildMenuConf.childMenuNum; \
            MenuManager_EmergencyStop_ListIndex += (uint32_t)1U)
      {
        if (MenuManager_EmergencyStop_IntValMapConf[MenuManager_EmergencyStop_ListIndex].mapValue == ProgramManager_gParamConfig.inputStatusCfg.emergencyStop)
        {
          break;
        }
      }
      
      MenuManager_EmergencyStop_CurPos = MenuManager_Common_LcdGetCurPosFirstTime(MenuManager_EmergencyStop_ListIndex, MenuManager_EmergencyStop_ChildMenuConf.childMenuNum);
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
    MenuManager_EmergencyStop_LcdShowMainTitle();
    MenuManager_EmergencyStop_LcdShowList();

    MenuManager_SubMainFunction = MenuManager_EmergencyStop_SubMainFunction;
    MenuManager_SubTickHandler = MenuManager_EmergencyStop_SubTickHandler;

    return FSM_GUARD_TRUE;
  }

  return FSM_GUARD_FALSE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_EmergencyStop_Exit(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  Fsm_DataHierachyStruct* dataHierachy;

  MenuManager_SubMainFunction = NULL;
  MenuManager_SubTickHandler = NULL;

  dataHierachy = (Fsm_DataHierachyStruct *)MenuManager_malloc(sizeof(Fsm_DataHierachyStruct));
  dataHierachy->dataId = MENUMANAGER_STATE_EMERGENCY_STOP;

  pFsmContext->dataHierachy = dataHierachy;

  /* Free internal data */
  MenuManager_InternalDataPop();
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_EmergencyStop_StartBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_EmergencyStop_InternalState == MENUMANAGER_EMERGENCYSTOP_INTERNALSTATE_READY)
  {
    MenuManager_EmergencyStop_InternalState = MENUMANAGER_EMERGENCYSTOP_INTERNALSTATE_RUNNING;
  }
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_EmergencyStop_StopBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_EmergencyStop_InternalState != MENUMANAGER_EMERGENCYSTOP_INTERNALSTATE_READY)
  {
    return FSM_GUARD_FALSE;
  }
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_EmergencyStop_UpBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_EmergencyStop_ListIndex > (uint32_t)0U)
  {
    MenuManager_EmergencyStop_ListIndex -= (uint32_t)1U;
  }

  if (MenuManager_EmergencyStop_CurPos > MENUMANAGER_COMMON_LCD_CURSOR_MIN)
  {
    MenuManager_EmergencyStop_CurPos -= (uint32_t)1U;
  }

  MenuManager_EmergencyStop_LcdShowList();
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_EmergencyStop_DownBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_EmergencyStop_ListIndex < ((uint32_t)(MenuManager_EmergencyStop_ChildMenuConf.childMenuNum) - (uint32_t)1U))
  {
    MenuManager_EmergencyStop_ListIndex += (uint32_t)1U;
  }
  
  if (MenuManager_EmergencyStop_CurPos < MenuManager_Common_LcdMaxCurPos(MenuManager_EmergencyStop_ChildMenuConf.childMenuNum))
  {
    MenuManager_EmergencyStop_CurPos += (uint32_t)1U;
  }
  
  MenuManager_EmergencyStop_LcdShowList();
  
  return FSM_GUARD_TRUE;
}



/*=============================================================================================*/
static void MenuManager_EmergencyStop_SubMainFunction(void)
{
  MenuManager_Common_ButEventMapHandler(&MenuManager_EmergencyStop_ButEventMapConf);

  switch (MenuManager_EmergencyStop_InternalState)
  {
    case MENUMANAGER_EMERGENCYSTOP_INTERNALSTATE_READY:
      break;
    case MENUMANAGER_EMERGENCYSTOP_INTERNALSTATE_RUNNING:
    {
      ProgramManager_InputStatusSetup_EmergencyStop_SetData(&(MenuManager_EmergencyStop_IntValMapConf[MenuManager_EmergencyStop_ListIndex].mapValue));

      ProgramManager_InputStatusSetup_EmergencyStop_GetData(&(ProgramManager_gParamConfig.inputStatusCfg.emergencyStop));

      MenuManager_EmergencyStop_InternalState = MENUMANAGER_EMERGENCYSTOP_INTERNALSTATE_DONE;
      
      break;
    }
    case MENUMANAGER_EMERGENCYSTOP_INTERNALSTATE_DONE:
    {
      MenuManager_EmergencyStop_LcdShowDone();

      MenuManager_EmergencyStop_InternalState = MENUMANAGER_EMERGENCYSTOP_INTERNALSTATE_EXITING;

      break;
    }
    case MENUMANAGER_EMERGENCYSTOP_INTERNALSTATE_EXITING:
    default:
      break;
  }
}

/*=============================================================================================*/
static void MenuManager_EmergencyStop_SubTickHandler(void)
{
  if (MenuManager_EmergencyStop_InternalState == MENUMANAGER_EMERGENCYSTOP_INTERNALSTATE_EXITING)
  {
    MenuManager_EmergencyStop_Counter += (uint32_t)1U;

    if (MenuManager_EmergencyStop_Counter >= MENUMANAGER_EMERGENCYSTOP_COUNTER_MAX)
    {
      MenuManager_EmergencyStop_Counter = (uint32_t)0U;
      
      Fsm_TriggerEvent( &MenuManager_FsmContext, \
                        (Fsm_EventType)((*(MenuManager_EmergencyStop_ChildMenuConf.childMenuCfg))[MenuManager_EmergencyStop_ListIndex].childMenuEvent));
    }
  }
}



#ifdef  __cplusplus
}
#endif
