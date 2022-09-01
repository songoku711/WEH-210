/* 
 * File:   _1212000000_menu_HeatUseTimeout.c
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
#define MENUMANAGER_HEATUSETIMEOUT_INTERNALDATALENGTH                 (uint8_t)4U

#define MENUMANAGER_HEATUSETIMEOUT_INTERNALSTATE_IDX                  0U
#define MENUMANAGER_HEATUSETIMEOUT_COUNTER_IDX                        1U
#define MENUMANAGER_HEATUSETIMEOUT_LISTINDEX_IDX                      2U
#define MENUMANAGER_HEATUSETIMEOUT_CURPOS_IDX                         3U

#define MenuManager_HeatUseTimeout_InternalState                      MenuManager_GetInternalDataPtr(MENUMANAGER_HEATUSETIMEOUT_INTERNALSTATE_IDX)
#define MenuManager_HeatUseTimeout_Counter                            MenuManager_GetInternalDataPtr(MENUMANAGER_HEATUSETIMEOUT_COUNTER_IDX)
#define MenuManager_HeatUseTimeout_ListIndex                          MenuManager_GetInternalDataPtr(MENUMANAGER_HEATUSETIMEOUT_LISTINDEX_IDX)
#define MenuManager_HeatUseTimeout_CurPos                             MenuManager_GetInternalDataPtr(MENUMANAGER_HEATUSETIMEOUT_CURPOS_IDX)



#define MENUMANAGER_HEATUSETIMEOUT_INTERNALSTATE_READY                (uint32_t)0U
#define MENUMANAGER_HEATUSETIMEOUT_INTERNALSTATE_RUNNING              (uint32_t)1U
#define MENUMANAGER_HEATUSETIMEOUT_INTERNALSTATE_DONE                 (uint32_t)2U
#define MENUMANAGER_HEATUSETIMEOUT_INTERNALSTATE_EXITING              (uint32_t)3U

#define MENUMANAGER_HEATUSETIMEOUT_COUNTER_MAX                        (uint32_t)50U

#define MENUMANAGER_HEATUSETIMEOUT_LISTINDEX_DISABLE                  (uint32_t)0U
#define MENUMANAGER_HEATUSETIMEOUT_LISTINDEX_ENABLE                   (uint32_t)1U



/* Internal value mapping structure */
typedef struct
{
  uint32_t                    listIndex;
  bool                        mapValue;
} MenuManager_HeatUseTimeout_IntValMapStruct;

static MenuManager_HeatUseTimeout_IntValMapStruct MenuManager_HeatUseTimeout_IntValMapConf[2] =
{
  { MENUMANAGER_HEATUSETIMEOUT_LISTINDEX_DISABLE,                     false                                   },
  { MENUMANAGER_HEATUSETIMEOUT_LISTINDEX_ENABLE,                      true                                    }
};



/** Menu manager main titles and child menu titles */
static const uint8_t MenuManager_HeatUseTimeout_MainTitleStr[] =      "HEAT TIMEOUT";

/** Menu manager child menu array */
static MenuManager_ChildMenuStruct MenuManager_HeatUseTimeout_ChildMenu[2] =
{
  { &MenuManager_Common_DisableStr,                                   MENUMANAGER_EVENT_SUBMENU_1             },
  { &MenuManager_Common_EnableStr,                                    MENUMANAGER_EVENT_SUBMENU_1             }
};

/** Menu manager child menu configuration */
static MenuManager_ChildMenuConfStruct MenuManager_HeatUseTimeout_ChildMenuConf =
{
  .childMenuNum               = (uint8_t)2U,
  .childMenuCfg               = &MenuManager_HeatUseTimeout_ChildMenu
};



/** Menu manager button event mapping array */
static MenuManager_ButEventMapStruct MenuManager_HeatUseTimeout_ButEventMap[4] =
{
  { IOMANAGER_BUTTON_STATE_START,                                     MENUMANAGER_EVENT_START_BUT             },
  { IOMANAGER_BUTTON_STATE_STOP,                                      MENUMANAGER_EVENT_STOP_BUT              },
  { IOMANAGER_BUTTON_STATE_UP,                                        MENUMANAGER_EVENT_UP_BUT                },
  { IOMANAGER_BUTTON_STATE_DOWN,                                      MENUMANAGER_EVENT_DOWN_BUT              }
};

/** Menu manager button event mapping configuration */
static MenuManager_ButEventMapConfStruct MenuManager_HeatUseTimeout_ButEventMapConf =
{
  .butEventMapNum             = (uint8_t)4U,
  .butEventMapCfg             = &MenuManager_HeatUseTimeout_ButEventMap
};



/** Menu manager event handlers */
static tFsmGuard MenuManager_HeatUseTimeout_Entry                     (tFsmContextPtr const pFsmContext, tFsmEvent event);
static tFsmGuard MenuManager_HeatUseTimeout_Exit                      (tFsmContextPtr const pFsmContext, tFsmEvent event);
static tFsmGuard MenuManager_HeatUseTimeout_Submenu1                  (tFsmContextPtr const pFsmContext, tFsmEvent event);
static tFsmGuard MenuManager_HeatUseTimeout_StartBut                  (tFsmContextPtr const pFsmContext, tFsmEvent event);
static tFsmGuard MenuManager_HeatUseTimeout_StopBut                   (tFsmContextPtr const pFsmContext, tFsmEvent event);
static tFsmGuard MenuManager_HeatUseTimeout_UpBut                     (tFsmContextPtr const pFsmContext, tFsmEvent event);
static tFsmGuard MenuManager_HeatUseTimeout_DownBut                   (tFsmContextPtr const pFsmContext, tFsmEvent event);

/** Menu manager state machine */
tFsmEventEntry MenuManager_HeatUseTimeout_StateMachine[7] =
{
  FSM_TRIGGER_ENTRY             (                                     MenuManager_HeatUseTimeout_Entry                                                ),
  FSM_TRIGGER_EXIT              (                                     MenuManager_HeatUseTimeout_Exit                                                 ),
  FSM_TRIGGER_TRANSITION_ACTION ( MENUMANAGER_EVENT_SUBMENU_1,        MenuManager_HeatUseTimeout_Submenu1,    MENUMANAGER_STATE_MACHINE_FUNC_SETUP    ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_START_BUT,        MenuManager_HeatUseTimeout_StartBut                                             ),
  FSM_TRIGGER_TRANSITION_ACTION ( MENUMANAGER_EVENT_STOP_BUT,         MenuManager_HeatUseTimeout_StopBut,     MENUMANAGER_STATE_MACHINE_FUNC_SETUP    ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_UP_BUT,           MenuManager_HeatUseTimeout_UpBut                                                ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_DOWN_BUT,         MenuManager_HeatUseTimeout_DownBut                                              )
};



/*===============================================================================================
*                                   LOCAL FUNCTION PROTOTYPES
===============================================================================================*/

static void MenuManager_HeatUseTimeout_LcdShowMainTitle(void);
static void MenuManager_HeatUseTimeout_LcdShowList(void);
static void MenuManager_HeatUseTimeout_LcdShowDone(void);

static void MenuManager_HeatUseTimeout_SubMainFunction(void);
static void MenuManager_HeatUseTimeout_SubTickHandler(void);



/*===============================================================================================
*                                       LOCAL FUNCTIONS
===============================================================================================*/

static void MenuManager_HeatUseTimeout_LcdShowMainTitle(void)
{
  /* Print main title */
  MenuManager_Common_LcdClearElementMenuStatic();
  MenuManager_Common_LcdShowMainTitle(MenuManager_HeatUseTimeout_MainTitleStr);
  MenuManager_Common_LcdUpdateElementMenuStatic();
}

/*=============================================================================================*/
static void MenuManager_HeatUseTimeout_LcdShowList(void)
{
  uint32_t currentListIndex;
  uint32_t currentCursorPos;
  uint32_t listLength;
  
  currentListIndex = MenuManager_Common_LcdGetStartListIndex(MenuManager_HeatUseTimeout_ListIndex, MenuManager_HeatUseTimeout_CurPos);
  currentCursorPos = (uint32_t)MENUMANAGER_COMMON_LCD_CURSOR_MIN;

  listLength = MenuManager_Common_LcdGetListLength((uint32_t)(MenuManager_HeatUseTimeout_ChildMenuConf.childMenuNum));
  
  MenuManager_Common_LcdClearElementMenuDynamic();
  
  for (; currentCursorPos < (listLength + (uint32_t)MENUMANAGER_COMMON_LCD_CURSOR_MIN); currentCursorPos++, currentListIndex++)
  {
    MenuManager_Common_LcdShowListElementName
    (
      (uint8_t *)*((*(MenuManager_HeatUseTimeout_ChildMenuConf.childMenuCfg))[currentListIndex].childMenuTitleStr),
      MENUMANAGER_COMMON_LCD_ELEMENT_MENU_LENGTH,
      MENUMANAGER_COMMON_LCD_ELEMENT_MENU_XPOS,
      currentCursorPos
    );
  }
  
  MenuManager_Common_LcdShowChosenMark(MenuManager_HeatUseTimeout_CurPos);
  
  MenuManager_Common_LcdShowScrollBar(MenuManager_HeatUseTimeout_ListIndex, MenuManager_HeatUseTimeout_CurPos, MenuManager_HeatUseTimeout_ChildMenuConf.childMenuNum);
  
  MenuManager_Common_LcdUpdateElementMenuDynamic();
}

/*=============================================================================================*/
static void MenuManager_HeatUseTimeout_LcdShowDone(void)
{
  MenuManager_Common_LcdClearAll();

  MenuManager_Common_LcdShowListElementName
  (
    (uint8_t *)*((*(MenuManager_HeatUseTimeout_ChildMenuConf.childMenuCfg))[MenuManager_HeatUseTimeout_ListIndex].childMenuTitleStr),
    MENUMANAGER_COMMON_LCD_ELEMENT_MENU_LENGTH,
    MENUMANAGER_COMMON_LCD_ELEMENT_MENU_XPOS,
    (uint8_t)MenuManager_HeatUseTimeout_CurPos
  );

  MenuManager_Common_LcdShowChosenMark(MenuManager_HeatUseTimeout_CurPos);

  MenuManager_Common_LcdUpdateAll();
}



/*=============================================================================================*/
static tFsmGuard MenuManager_HeatUseTimeout_Entry(tFsmContextPtr const pFsmContext, tFsmEvent event)
{
  MenuManager_SubMainFunction = MenuManager_HeatUseTimeout_SubMainFunction;
  MenuManager_SubTickHandler = MenuManager_HeatUseTimeout_SubTickHandler;

  /* Check if previous state data hierachy is not empty */
  if (pFsmContext->dataHierachy != NULL)
  {
    if (pFsmContext->dataHierachy->dataId == MENUMANAGER_STATE_MACHINE_FUNC_SETUP)
    {
      /* Release previous state data hierachy */
      MenuManager_free(pFsmContext->dataHierachy);
      pFsmContext->dataHierachy = NULL;

      MenuManager_InternalDataPush(MENUMANAGER_HEATUSETIMEOUT_INTERNALDATALENGTH);

      MenuManager_HeatUseTimeout_InternalState = MENUMANAGER_HEATUSETIMEOUT_INTERNALSTATE_READY;
      MenuManager_HeatUseTimeout_Counter = (uint32_t)0U;
      
      for ( MenuManager_HeatUseTimeout_ListIndex = (uint32_t)0U; \
            MenuManager_HeatUseTimeout_ListIndex < MenuManager_HeatUseTimeout_ChildMenuConf.childMenuNum; \
            MenuManager_HeatUseTimeout_ListIndex += (uint32_t)1U)
      {
        if (MenuManager_HeatUseTimeout_IntValMapConf[MenuManager_HeatUseTimeout_ListIndex].mapValue == ProgramManager_gParamConfig.machineFuncCfg.heatUseTimeout)
        {
          break;
        }
      }
      
      MenuManager_HeatUseTimeout_CurPos = MenuManager_Common_LcdGetCurPosFirstTime(MenuManager_HeatUseTimeout_ListIndex, MenuManager_HeatUseTimeout_ChildMenuConf.childMenuNum);
    }
    else
    {
      return kFsmGuard_False;
    }

    MenuManager_HeatUseTimeout_LcdShowMainTitle();
    MenuManager_HeatUseTimeout_LcdShowList();

    return kFsmGuard_True;
  }

  return kFsmGuard_False;
}

/*=============================================================================================*/
static tFsmGuard MenuManager_HeatUseTimeout_Exit(tFsmContextPtr const pFsmContext, tFsmEvent event)
{
  MenuManager_SubMainFunction = NULL;
  MenuManager_SubTickHandler = NULL;

  tFsmDataHierachy* dataHierachy;

  dataHierachy = (tFsmDataHierachy *)MenuManager_malloc(sizeof(tFsmDataHierachy));
  dataHierachy->dataId = MENUMANAGER_STATE_HEAT_USE_TIMEOUT;

  pFsmContext->dataHierachy = dataHierachy;

  /* Free internal data */
  MenuManager_InternalDataPop();
  
  return kFsmGuard_True;
}

/*=============================================================================================*/
static tFsmGuard MenuManager_HeatUseTimeout_Submenu1(tFsmContextPtr const pFsmContext, tFsmEvent event)
{
  
  
  return kFsmGuard_True;
}

/*=============================================================================================*/
static tFsmGuard MenuManager_HeatUseTimeout_StartBut(tFsmContextPtr const pFsmContext, tFsmEvent event)
{
  if (MenuManager_HeatUseTimeout_InternalState == MENUMANAGER_HEATUSETIMEOUT_INTERNALSTATE_READY)
  {
    MenuManager_HeatUseTimeout_InternalState = MENUMANAGER_HEATUSETIMEOUT_INTERNALSTATE_RUNNING;
  }
  
  return kFsmGuard_True;
}

/*=============================================================================================*/
static tFsmGuard MenuManager_HeatUseTimeout_StopBut(tFsmContextPtr const pFsmContext, tFsmEvent event)
{
  if (MenuManager_HeatUseTimeout_InternalState != MENUMANAGER_HEATUSETIMEOUT_INTERNALSTATE_READY)
  {
    return kFsmGuard_False;
  }
  
  return kFsmGuard_True;
}

/*=============================================================================================*/
static tFsmGuard MenuManager_HeatUseTimeout_UpBut(tFsmContextPtr const pFsmContext, tFsmEvent event)
{
  if (MenuManager_HeatUseTimeout_ListIndex > (uint32_t)0U)
  {
    MenuManager_HeatUseTimeout_ListIndex -= (uint32_t)1U;
  }

  if (MenuManager_HeatUseTimeout_CurPos > MENUMANAGER_COMMON_LCD_CURSOR_MIN)
  {
    MenuManager_HeatUseTimeout_CurPos -= (uint32_t)1U;
  }

  MenuManager_HeatUseTimeout_LcdShowList();
  
  return kFsmGuard_True;
}

/*=============================================================================================*/
static tFsmGuard MenuManager_HeatUseTimeout_DownBut(tFsmContextPtr const pFsmContext, tFsmEvent event)
{
  if (MenuManager_HeatUseTimeout_ListIndex < ((uint32_t)(MenuManager_HeatUseTimeout_ChildMenuConf.childMenuNum) - (uint32_t)1U))
  {
    MenuManager_HeatUseTimeout_ListIndex += (uint32_t)1U;
  }
  
  if (MenuManager_HeatUseTimeout_CurPos < MenuManager_Common_LcdMaxCurPos(MenuManager_HeatUseTimeout_ChildMenuConf.childMenuNum))
  {
    MenuManager_HeatUseTimeout_CurPos += (uint32_t)1U;
  }
  
  MenuManager_HeatUseTimeout_LcdShowList();
  
  return kFsmGuard_True;
}



/*=============================================================================================*/
static void MenuManager_HeatUseTimeout_SubMainFunction(void)
{
  MenuManager_Common_ButEventMapHandler(&MenuManager_HeatUseTimeout_ButEventMapConf);

  switch (MenuManager_HeatUseTimeout_InternalState)
  {
    case MENUMANAGER_HEATUSETIMEOUT_INTERNALSTATE_READY:
      break;
    case MENUMANAGER_HEATUSETIMEOUT_INTERNALSTATE_RUNNING:
    {
      ProgramManager_MachineFuncSetup_HeatUseTimeout_SetData(&(MenuManager_HeatUseTimeout_IntValMapConf[MenuManager_HeatUseTimeout_ListIndex].mapValue));

      ProgramManager_MachineFuncSetup_HeatUseTimeout_GetData(&(ProgramManager_gParamConfig.machineFuncCfg.heatUseTimeout));

      MenuManager_HeatUseTimeout_InternalState = MENUMANAGER_HEATUSETIMEOUT_INTERNALSTATE_DONE;
      
      break;
    }
    case MENUMANAGER_HEATUSETIMEOUT_INTERNALSTATE_DONE:
    {
      MenuManager_HeatUseTimeout_LcdShowDone();

      MenuManager_HeatUseTimeout_InternalState = MENUMANAGER_HEATUSETIMEOUT_INTERNALSTATE_EXITING;

      break;
    }
    case MENUMANAGER_HEATUSETIMEOUT_INTERNALSTATE_EXITING:
    default:
      break;
  }
}

/*=============================================================================================*/
static void MenuManager_HeatUseTimeout_SubTickHandler(void)
{
  if (MenuManager_HeatUseTimeout_InternalState == MENUMANAGER_HEATUSETIMEOUT_INTERNALSTATE_EXITING)
  {
    MenuManager_HeatUseTimeout_Counter += (uint32_t)1U;

    if (MenuManager_HeatUseTimeout_Counter >= MENUMANAGER_HEATUSETIMEOUT_COUNTER_MAX)
    {
      MenuManager_HeatUseTimeout_Counter = (uint32_t)0U;
      
      Fsm_TriggerEvent(&MenuManager_FsmContext, (tFsmEvent)MENUMANAGER_EVENT_SUBMENU_1);
    }
  }
}



#ifdef  __cplusplus
}
#endif
