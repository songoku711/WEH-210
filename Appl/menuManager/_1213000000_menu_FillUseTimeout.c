/* 
 * File:   _1213000000_menu_FillUseTimeout.c
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
#define MENUMANAGER_FILLUSETIMEOUT_INTERNALDATALENGTH                 (uint8_t)4U

#define MENUMANAGER_FILLUSETIMEOUT_INTERNALSTATE_IDX                  0U
#define MENUMANAGER_FILLUSETIMEOUT_COUNTER_IDX                        1U
#define MENUMANAGER_FILLUSETIMEOUT_LISTINDEX_IDX                      2U
#define MENUMANAGER_FILLUSETIMEOUT_CURPOS_IDX                         3U

#define MenuManager_FillUseTimeout_InternalState                      MenuManager_GetInternalDataPtr(MENUMANAGER_FILLUSETIMEOUT_INTERNALSTATE_IDX)
#define MenuManager_FillUseTimeout_Counter                            MenuManager_GetInternalDataPtr(MENUMANAGER_FILLUSETIMEOUT_COUNTER_IDX)
#define MenuManager_FillUseTimeout_ListIndex                          MenuManager_GetInternalDataPtr(MENUMANAGER_FILLUSETIMEOUT_LISTINDEX_IDX)
#define MenuManager_FillUseTimeout_CurPos                             MenuManager_GetInternalDataPtr(MENUMANAGER_FILLUSETIMEOUT_CURPOS_IDX)



#define MENUMANAGER_FILLUSETIMEOUT_INTERNALSTATE_READY                (uint32_t)0U
#define MENUMANAGER_FILLUSETIMEOUT_INTERNALSTATE_RUNNING              (uint32_t)1U
#define MENUMANAGER_FILLUSETIMEOUT_INTERNALSTATE_DONE                 (uint32_t)2U
#define MENUMANAGER_FILLUSETIMEOUT_INTERNALSTATE_EXITING              (uint32_t)3U

#define MENUMANAGER_FILLUSETIMEOUT_COUNTER_MAX                        (uint32_t)50U

#define MENUMANAGER_FILLUSETIMEOUT_LISTINDEX_DISABLE                  (uint32_t)0U
#define MENUMANAGER_FILLUSETIMEOUT_LISTINDEX_ENABLE                   (uint32_t)1U



/* Internal value mapping structure */
typedef struct
{
  uint32_t                    listIndex;
  bool                        mapValue;
} MenuManager_FillUseTimeout_IntValMapStruct;

static MenuManager_FillUseTimeout_IntValMapStruct MenuManager_FillUseTimeout_IntValMapConf[2] =
{
  { MENUMANAGER_FILLUSETIMEOUT_LISTINDEX_DISABLE,                     false                                   },
  { MENUMANAGER_FILLUSETIMEOUT_LISTINDEX_ENABLE,                      true                                    }
};



/** Menu manager main titles and child menu titles */
static const uint8_t MenuManager_FillUseTimeout_MainTitleStr[] =      "FILL TIMEOUT";

/** Menu manager child menu array */
static MenuManager_ChildMenuStruct MenuManager_FillUseTimeout_ChildMenu[2] =
{
  { &MenuManager_Common_DisableStr,                                   MENUMANAGER_EVENT_PREV                  },
  { &MenuManager_Common_EnableStr,                                    MENUMANAGER_EVENT_PREV                  }
};

/** Menu manager child menu configuration */
static MenuManager_ChildMenuConfStruct MenuManager_FillUseTimeout_ChildMenuConf =
{
  .childMenuNum               = (uint8_t)2U,
  .childMenuCfg               = &MenuManager_FillUseTimeout_ChildMenu
};



/** Menu manager button event mapping array */
static MenuManager_ButEventMapStruct MenuManager_FillUseTimeout_ButEventMap[4] =
{
  { IOMANAGER_BUTTON_STATE_START,                                     MENUMANAGER_EVENT_START_BUT             },
  { IOMANAGER_BUTTON_STATE_STOP,                                      MENUMANAGER_EVENT_STOP_BUT              },
  { IOMANAGER_BUTTON_STATE_UP,                                        MENUMANAGER_EVENT_UP_BUT                },
  { IOMANAGER_BUTTON_STATE_DOWN,                                      MENUMANAGER_EVENT_DOWN_BUT              }
};

/** Menu manager button event mapping configuration */
static MenuManager_ButEventMapConfStruct MenuManager_FillUseTimeout_ButEventMapConf =
{
  .butEventMapNum             = (uint8_t)4U,
  .butEventMapCfg             = &MenuManager_FillUseTimeout_ButEventMap
};



/** Menu manager event handlers */
static Fsm_GuardType MenuManager_FillUseTimeout_Entry                 (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_FillUseTimeout_Exit                  (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_FillUseTimeout_StartBut              (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_FillUseTimeout_StopBut               (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_FillUseTimeout_UpBut                 (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_FillUseTimeout_DownBut               (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);

/** Menu manager state machine */
Fsm_EventEntryStruct MenuManager_FillUseTimeout_StateMachine[7] =
{
  FSM_TRIGGER_ENTRY             (                                     MenuManager_FillUseTimeout_Entry                                                ),
  FSM_TRIGGER_EXIT              (                                     MenuManager_FillUseTimeout_Exit                                                 ),
  FSM_TRIGGER_TRANSITION        ( MENUMANAGER_EVENT_PREV,                                                     MENUMANAGER_STATE_MACHINE_FUNC_SETUP    ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_START_BUT,        MenuManager_FillUseTimeout_StartBut                                             ),
  FSM_TRIGGER_TRANSITION_ACTION ( MENUMANAGER_EVENT_STOP_BUT,         MenuManager_FillUseTimeout_StopBut,     MENUMANAGER_STATE_MACHINE_FUNC_SETUP    ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_UP_BUT,           MenuManager_FillUseTimeout_UpBut                                                ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_DOWN_BUT,         MenuManager_FillUseTimeout_DownBut                                              )
};



/*===============================================================================================
*                                   LOCAL FUNCTION PROTOTYPES
===============================================================================================*/

static void MenuManager_FillUseTimeout_LcdShowMainTitle(void);
static void MenuManager_FillUseTimeout_LcdShowList(void);
static void MenuManager_FillUseTimeout_LcdShowDone(void);

static void MenuManager_FillUseTimeout_SubMainFunction(void);
static void MenuManager_FillUseTimeout_SubTickHandler(void);



/*===============================================================================================
*                                       LOCAL FUNCTIONS
===============================================================================================*/

static void MenuManager_FillUseTimeout_LcdShowMainTitle(void)
{
  /* Print main title */
  MenuManager_Common_LcdClearElementMenuStatic();
  MenuManager_Common_LcdShowMainTitle(MenuManager_FillUseTimeout_MainTitleStr);
  MenuManager_Common_LcdUpdateElementMenuStatic();
}

/*=============================================================================================*/
static void MenuManager_FillUseTimeout_LcdShowList(void)
{
  uint32_t currentListIndex;
  uint32_t currentCursorPos;
  uint32_t listLength;
  
  currentListIndex = MenuManager_Common_LcdGetStartListIndex(MenuManager_FillUseTimeout_ListIndex, MenuManager_FillUseTimeout_CurPos);
  currentCursorPos = (uint32_t)MENUMANAGER_COMMON_LCD_CURSOR_MIN;

  listLength = MenuManager_Common_LcdGetListLength((uint32_t)(MenuManager_FillUseTimeout_ChildMenuConf.childMenuNum));
  
  MenuManager_Common_LcdClearElementMenuDynamic();
  
  for (; currentCursorPos < (listLength + (uint32_t)MENUMANAGER_COMMON_LCD_CURSOR_MIN); currentCursorPos++, currentListIndex++)
  {
    MenuManager_Common_LcdShowListElementName
    (
      (uint8_t *)*((*(MenuManager_FillUseTimeout_ChildMenuConf.childMenuCfg))[currentListIndex].childMenuTitleStr),
      MENUMANAGER_COMMON_LCD_ELEMENT_MENU_LENGTH,
      MENUMANAGER_COMMON_LCD_ELEMENT_MENU_XPOS,
      currentCursorPos
    );
  }
  
  MenuManager_Common_LcdShowChosenMark(MenuManager_FillUseTimeout_CurPos);
  
  MenuManager_Common_LcdShowScrollBar(MenuManager_FillUseTimeout_ListIndex, MenuManager_FillUseTimeout_CurPos, MenuManager_FillUseTimeout_ChildMenuConf.childMenuNum);
  
  MenuManager_Common_LcdUpdateElementMenuDynamic();
}

/*=============================================================================================*/
static void MenuManager_FillUseTimeout_LcdShowDone(void)
{
  MenuManager_Common_LcdClearAll();

  MenuManager_Common_LcdShowListElementName
  (
    (uint8_t *)*((*(MenuManager_FillUseTimeout_ChildMenuConf.childMenuCfg))[MenuManager_FillUseTimeout_ListIndex].childMenuTitleStr),
    MENUMANAGER_COMMON_LCD_ELEMENT_MENU_LENGTH,
    MENUMANAGER_COMMON_LCD_ELEMENT_MENU_XPOS,
    (uint8_t)MenuManager_FillUseTimeout_CurPos
  );

  MenuManager_Common_LcdShowChosenMark(MenuManager_FillUseTimeout_CurPos);

  MenuManager_Common_LcdUpdateAll();
}



/*=============================================================================================*/
static Fsm_GuardType MenuManager_FillUseTimeout_Entry(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  HAL_StatusTypeDef retVal = HAL_OK;

  /* Check if previous state data hierachy is not empty */
  if (pFsmContext->dataHierachy != NULL)
  {
    if (pFsmContext->dataHierachy->dataId == MENUMANAGER_STATE_MACHINE_FUNC_SETUP)
    {
      /* Release previous state data hierachy */
      MenuManager_free(pFsmContext->dataHierachy);
      pFsmContext->dataHierachy = NULL;

      MenuManager_InternalDataPush(MENUMANAGER_FILLUSETIMEOUT_INTERNALDATALENGTH);

      MenuManager_FillUseTimeout_InternalState = MENUMANAGER_FILLUSETIMEOUT_INTERNALSTATE_READY;
      MenuManager_FillUseTimeout_Counter = (uint32_t)0U;
      
      for ( MenuManager_FillUseTimeout_ListIndex = (uint32_t)0U; \
            MenuManager_FillUseTimeout_ListIndex < MenuManager_FillUseTimeout_ChildMenuConf.childMenuNum; \
            MenuManager_FillUseTimeout_ListIndex += (uint32_t)1U)
      {
        if (MenuManager_FillUseTimeout_IntValMapConf[MenuManager_FillUseTimeout_ListIndex].mapValue == ProgramManager_gParamConfig.machineFuncCfg.fillUseTimeout)
        {
          break;
        }
      }
      
      MenuManager_FillUseTimeout_CurPos = MenuManager_Common_LcdGetCurPosFirstTime(MenuManager_FillUseTimeout_ListIndex, MenuManager_FillUseTimeout_ChildMenuConf.childMenuNum);
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
    MenuManager_FillUseTimeout_LcdShowMainTitle();
    MenuManager_FillUseTimeout_LcdShowList();

    MenuManager_SubMainFunction = MenuManager_FillUseTimeout_SubMainFunction;
    MenuManager_SubTickHandler = MenuManager_FillUseTimeout_SubTickHandler;

    return FSM_GUARD_TRUE;
  }

  return FSM_GUARD_FALSE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_FillUseTimeout_Exit(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  Fsm_DataHierachyStruct* dataHierachy;

  MenuManager_SubMainFunction = NULL;
  MenuManager_SubTickHandler = NULL;

  dataHierachy = (Fsm_DataHierachyStruct *)MenuManager_malloc(sizeof(Fsm_DataHierachyStruct));
  dataHierachy->dataId = MENUMANAGER_STATE_FILL_USE_TIMEOUT;

  pFsmContext->dataHierachy = dataHierachy;

  /* Free internal data */
  MenuManager_InternalDataPop();
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_FillUseTimeout_StartBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_FillUseTimeout_InternalState == MENUMANAGER_FILLUSETIMEOUT_INTERNALSTATE_READY)
  {
    MenuManager_FillUseTimeout_InternalState = MENUMANAGER_FILLUSETIMEOUT_INTERNALSTATE_RUNNING;
  }
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_FillUseTimeout_StopBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_FillUseTimeout_InternalState != MENUMANAGER_FILLUSETIMEOUT_INTERNALSTATE_READY)
  {
    return FSM_GUARD_FALSE;
  }
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_FillUseTimeout_UpBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_FillUseTimeout_ListIndex > (uint32_t)0U)
  {
    MenuManager_FillUseTimeout_ListIndex -= (uint32_t)1U;
  }

  if (MenuManager_FillUseTimeout_CurPos > MENUMANAGER_COMMON_LCD_CURSOR_MIN)
  {
    MenuManager_FillUseTimeout_CurPos -= (uint32_t)1U;
  }

  MenuManager_FillUseTimeout_LcdShowList();
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_FillUseTimeout_DownBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_FillUseTimeout_ListIndex < ((uint32_t)(MenuManager_FillUseTimeout_ChildMenuConf.childMenuNum) - (uint32_t)1U))
  {
    MenuManager_FillUseTimeout_ListIndex += (uint32_t)1U;
  }
  
  if (MenuManager_FillUseTimeout_CurPos < MenuManager_Common_LcdMaxCurPos(MenuManager_FillUseTimeout_ChildMenuConf.childMenuNum))
  {
    MenuManager_FillUseTimeout_CurPos += (uint32_t)1U;
  }
  
  MenuManager_FillUseTimeout_LcdShowList();
  
  return FSM_GUARD_TRUE;
}



/*=============================================================================================*/
static void MenuManager_FillUseTimeout_SubMainFunction(void)
{
  MenuManager_Common_ButEventMapHandler(&MenuManager_FillUseTimeout_ButEventMapConf);

  switch (MenuManager_FillUseTimeout_InternalState)
  {
    case MENUMANAGER_FILLUSETIMEOUT_INTERNALSTATE_READY:
      break;
    case MENUMANAGER_FILLUSETIMEOUT_INTERNALSTATE_RUNNING:
    {
      ProgramManager_MachineFuncSetup_FillUseTimeout_SetData(&(MenuManager_FillUseTimeout_IntValMapConf[MenuManager_FillUseTimeout_ListIndex].mapValue));

      ProgramManager_MachineFuncSetup_FillUseTimeout_GetData(&(ProgramManager_gParamConfig.machineFuncCfg.fillUseTimeout));

      MenuManager_FillUseTimeout_InternalState = MENUMANAGER_FILLUSETIMEOUT_INTERNALSTATE_DONE;
      
      break;
    }
    case MENUMANAGER_FILLUSETIMEOUT_INTERNALSTATE_DONE:
    {
      MenuManager_FillUseTimeout_LcdShowDone();

      MenuManager_FillUseTimeout_InternalState = MENUMANAGER_FILLUSETIMEOUT_INTERNALSTATE_EXITING;

      break;
    }
    case MENUMANAGER_FILLUSETIMEOUT_INTERNALSTATE_EXITING:
    default:
      break;
  }
}

/*=============================================================================================*/
static void MenuManager_FillUseTimeout_SubTickHandler(void)
{
  if (MenuManager_FillUseTimeout_InternalState == MENUMANAGER_FILLUSETIMEOUT_INTERNALSTATE_EXITING)
  {
    MenuManager_FillUseTimeout_Counter += (uint32_t)1U;

    if (MenuManager_FillUseTimeout_Counter >= MENUMANAGER_FILLUSETIMEOUT_COUNTER_MAX)
    {
      MenuManager_FillUseTimeout_Counter = (uint32_t)0U;
      
      Fsm_TriggerEvent( &MenuManager_FsmContext, \
                        (Fsm_EventType)((*(MenuManager_FillUseTimeout_ChildMenuConf.childMenuCfg))[MenuManager_FillUseTimeout_ListIndex].childMenuEvent));
    }
  }
}



#ifdef  __cplusplus
}
#endif
