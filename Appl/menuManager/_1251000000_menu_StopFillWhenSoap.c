/* 
 * File:   _1251000000_menu_StopFillWhenSoap.c
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
#define MENUMANAGER_STOPFILLWHENSOAP_INTERNALDATALENGTH               (uint8_t)4U

#define MENUMANAGER_STOPFILLWHENSOAP_INTERNALSTATE_IDX                0U
#define MENUMANAGER_STOPFILLWHENSOAP_COUNTER_IDX                      1U
#define MENUMANAGER_STOPFILLWHENSOAP_LISTINDEX_IDX                    2U
#define MENUMANAGER_STOPFILLWHENSOAP_CURPOS_IDX                       3U

#define MenuManager_StopFillWhenSoap_InternalState                    MenuManager_GetInternalDataPtr(MENUMANAGER_STOPFILLWHENSOAP_INTERNALSTATE_IDX)
#define MenuManager_StopFillWhenSoap_Counter                          MenuManager_GetInternalDataPtr(MENUMANAGER_STOPFILLWHENSOAP_COUNTER_IDX)
#define MenuManager_StopFillWhenSoap_ListIndex                        MenuManager_GetInternalDataPtr(MENUMANAGER_STOPFILLWHENSOAP_LISTINDEX_IDX)
#define MenuManager_StopFillWhenSoap_CurPos                           MenuManager_GetInternalDataPtr(MENUMANAGER_STOPFILLWHENSOAP_CURPOS_IDX)



#define MENUMANAGER_STOPFILLWHENSOAP_INTERNALSTATE_READY              (uint32_t)0U
#define MENUMANAGER_STOPFILLWHENSOAP_INTERNALSTATE_RUNNING            (uint32_t)1U
#define MENUMANAGER_STOPFILLWHENSOAP_INTERNALSTATE_DONE               (uint32_t)2U
#define MENUMANAGER_STOPFILLWHENSOAP_INTERNALSTATE_EXITING            (uint32_t)3U

#define MENUMANAGER_STOPFILLWHENSOAP_COUNTER_MAX                      (uint32_t)50U

#define MENUMANAGER_STOPFILLWHENSOAP_LISTINDEX_DISABLE                (uint32_t)0U
#define MENUMANAGER_STOPFILLWHENSOAP_LISTINDEX_ENABLE                 (uint32_t)1U



/* Internal value mapping structure */
typedef struct
{
  uint32_t                    listIndex;
  bool                        mapValue;
} MenuManager_StopFillWhenSoap_IntValMapStruct;

static MenuManager_StopFillWhenSoap_IntValMapStruct MenuManager_StopFillWhenSoap_IntValMapConf[2] =
{
  { MENUMANAGER_STOPFILLWHENSOAP_LISTINDEX_DISABLE,                   false                                   },
  { MENUMANAGER_STOPFILLWHENSOAP_LISTINDEX_ENABLE,                    true                                    }
};




/** Menu manager main titles and child menu titles */
static const uint8_t MenuManager_StopFillWhenSoap_MainTitleStr[] =    "STOP FILL SOAP";

/** Menu manager child menu array */
static MenuManager_ChildMenuStruct MenuManager_StopFillWhenSoap_ChildMenu[2] =
{
  { &MenuManager_Common_DisableStr,                                   MENUMANAGER_EVENT_SUBMENU_1             },
  { &MenuManager_Common_EnableStr,                                    MENUMANAGER_EVENT_SUBMENU_1             }
};

/** Menu manager child menu configuration */
static MenuManager_ChildMenuConfStruct MenuManager_StopFillWhenSoap_ChildMenuConf =
{
  .childMenuNum               = (uint8_t)2U,
  .childMenuCfg               = &MenuManager_StopFillWhenSoap_ChildMenu
};



/** Menu manager button event mapping array */
static MenuManager_ButEventMapStruct MenuManager_StopFillWhenSoap_ButEventMap[4] =
{
  { IOMANAGER_BUTTON_STATE_START,                                     MENUMANAGER_EVENT_START_BUT             },
  { IOMANAGER_BUTTON_STATE_STOP,                                      MENUMANAGER_EVENT_STOP_BUT              },
  { IOMANAGER_BUTTON_STATE_UP,                                        MENUMANAGER_EVENT_UP_BUT                },
  { IOMANAGER_BUTTON_STATE_DOWN,                                      MENUMANAGER_EVENT_DOWN_BUT              }
};

/** Menu manager button event mapping configuration */
static MenuManager_ButEventMapConfStruct MenuManager_StopFillWhenSoap_ButEventMapConf =
{
  .butEventMapNum             = (uint8_t)4U,
  .butEventMapCfg             = &MenuManager_StopFillWhenSoap_ButEventMap
};



/** Menu manager event handlers */
static tFsmGuard MenuManager_StopFillWhenSoap_Entry                   (tFsmContextPtr const pFsmContext, tFsmEvent event);
static tFsmGuard MenuManager_StopFillWhenSoap_Exit                    (tFsmContextPtr const pFsmContext, tFsmEvent event);
static tFsmGuard MenuManager_StopFillWhenSoap_Submenu1                (tFsmContextPtr const pFsmContext, tFsmEvent event);
static tFsmGuard MenuManager_StopFillWhenSoap_StartBut                (tFsmContextPtr const pFsmContext, tFsmEvent event);
static tFsmGuard MenuManager_StopFillWhenSoap_StopBut                 (tFsmContextPtr const pFsmContext, tFsmEvent event);
static tFsmGuard MenuManager_StopFillWhenSoap_UpBut                   (tFsmContextPtr const pFsmContext, tFsmEvent event);
static tFsmGuard MenuManager_StopFillWhenSoap_DownBut                 (tFsmContextPtr const pFsmContext, tFsmEvent event);

/** Menu manager state machine */
tFsmEventEntry MenuManager_StopFillWhenSoap_StateMachine[7] =
{
  FSM_TRIGGER_ENTRY             (                                     MenuManager_StopFillWhenSoap_Entry                                              ),
  FSM_TRIGGER_EXIT              (                                     MenuManager_StopFillWhenSoap_Exit                                               ),
  FSM_TRIGGER_TRANSITION_ACTION ( MENUMANAGER_EVENT_SUBMENU_1,        MenuManager_StopFillWhenSoap_Submenu1,  MENUMANAGER_STATE_SOAP_SETUP            ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_START_BUT,        MenuManager_StopFillWhenSoap_StartBut                                           ),
  FSM_TRIGGER_TRANSITION_ACTION ( MENUMANAGER_EVENT_STOP_BUT,         MenuManager_StopFillWhenSoap_StopBut,   MENUMANAGER_STATE_SOAP_SETUP            ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_UP_BUT,           MenuManager_StopFillWhenSoap_UpBut                                              ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_DOWN_BUT,         MenuManager_StopFillWhenSoap_DownBut                                            )
};



/*===============================================================================================
*                                   LOCAL FUNCTION PROTOTYPES
===============================================================================================*/

static void MenuManager_StopFillWhenSoap_LcdShowMainTitle(void);
static void MenuManager_StopFillWhenSoap_LcdShowList(void);
static void MenuManager_StopFillWhenSoap_LcdShowDone(void);

static void MenuManager_StopFillWhenSoap_SubMainFunction(void);
static void MenuManager_StopFillWhenSoap_SubTickHandler(void);



/*===============================================================================================
*                                       LOCAL FUNCTIONS
===============================================================================================*/

static void MenuManager_StopFillWhenSoap_LcdShowMainTitle(void)
{
  /* Print main title */
  MenuManager_Common_LcdClearElementMenuStatic();
  MenuManager_Common_LcdShowMainTitle(MenuManager_StopFillWhenSoap_MainTitleStr);
  MenuManager_Common_LcdUpdateElementMenuStatic();
}

/*=============================================================================================*/
static void MenuManager_StopFillWhenSoap_LcdShowList(void)
{
  uint32_t currentListIndex;
  uint32_t currentCursorPos;
  uint32_t listLength;
  
  currentListIndex = MenuManager_Common_LcdGetStartListIndex(MenuManager_StopFillWhenSoap_ListIndex, MenuManager_StopFillWhenSoap_CurPos);
  currentCursorPos = (uint32_t)MENUMANAGER_COMMON_LCD_CURSOR_MIN;

  listLength = MenuManager_Common_LcdGetListLength((uint32_t)(MenuManager_StopFillWhenSoap_ChildMenuConf.childMenuNum));
  
  MenuManager_Common_LcdClearElementMenuDynamic();
  
  for (; currentCursorPos < (listLength + (uint32_t)MENUMANAGER_COMMON_LCD_CURSOR_MIN); currentCursorPos++, currentListIndex++)
  {
    MenuManager_Common_LcdShowListElementName
    (
      (uint8_t *)*((*(MenuManager_StopFillWhenSoap_ChildMenuConf.childMenuCfg))[currentListIndex].childMenuTitleStr),
      MENUMANAGER_COMMON_LCD_ELEMENT_MENU_LENGTH,
      MENUMANAGER_COMMON_LCD_ELEMENT_MENU_XPOS,
      currentCursorPos
    );
  }
  
  MenuManager_Common_LcdShowChosenMark(MenuManager_StopFillWhenSoap_CurPos);
  
  MenuManager_Common_LcdShowScrollBar(MenuManager_StopFillWhenSoap_ListIndex, MenuManager_StopFillWhenSoap_CurPos, MenuManager_StopFillWhenSoap_ChildMenuConf.childMenuNum);
  
  MenuManager_Common_LcdUpdateElementMenuDynamic();
}

/*=============================================================================================*/
static void MenuManager_StopFillWhenSoap_LcdShowDone(void)
{
  MenuManager_Common_LcdClearAll();

  MenuManager_Common_LcdShowListElementName
  (
    (uint8_t *)*((*(MenuManager_StopFillWhenSoap_ChildMenuConf.childMenuCfg))[MenuManager_StopFillWhenSoap_ListIndex].childMenuTitleStr),
    MENUMANAGER_COMMON_LCD_ELEMENT_MENU_LENGTH,
    MENUMANAGER_COMMON_LCD_ELEMENT_MENU_XPOS,
    (uint8_t)MenuManager_StopFillWhenSoap_CurPos
  );

  MenuManager_Common_LcdShowChosenMark(MenuManager_StopFillWhenSoap_CurPos);

  MenuManager_Common_LcdUpdateAll();
}



/*=============================================================================================*/
static tFsmGuard MenuManager_StopFillWhenSoap_Entry(tFsmContextPtr const pFsmContext, tFsmEvent event)
{
  MenuManager_SubMainFunction = MenuManager_StopFillWhenSoap_SubMainFunction;
  MenuManager_SubTickHandler = MenuManager_StopFillWhenSoap_SubTickHandler;

  /* Check if previous state data hierachy is not empty */
  if (pFsmContext->dataHierachy != NULL)
  {
    if (pFsmContext->dataHierachy->dataId == MENUMANAGER_STATE_SOAP_SETUP)
    {
      /* Release previous state data hierachy */
      MenuManager_free(pFsmContext->dataHierachy);
      pFsmContext->dataHierachy = NULL;

      MenuManager_InternalDataPush(MENUMANAGER_STOPFILLWHENSOAP_INTERNALDATALENGTH);

      MenuManager_StopFillWhenSoap_InternalState = MENUMANAGER_STOPFILLWHENSOAP_INTERNALSTATE_READY;
      MenuManager_StopFillWhenSoap_Counter = (uint32_t)0U;
      
      for ( MenuManager_StopFillWhenSoap_ListIndex = (uint32_t)0U; \
            MenuManager_StopFillWhenSoap_ListIndex < MenuManager_StopFillWhenSoap_ChildMenuConf.childMenuNum; \
            MenuManager_StopFillWhenSoap_ListIndex += (uint32_t)1U)
      {
        if (MenuManager_StopFillWhenSoap_IntValMapConf[MenuManager_StopFillWhenSoap_ListIndex].mapValue == ProgramManager_gParamConfig.soapCfg.stopFillWhenSoap)
        {
          break;
        }
      }
      
      MenuManager_StopFillWhenSoap_CurPos = MenuManager_Common_LcdGetCurPosFirstTime(MenuManager_StopFillWhenSoap_ListIndex, MenuManager_StopFillWhenSoap_ChildMenuConf.childMenuNum);
    }
    else
    {
      return kFsmGuard_False;
    }

    MenuManager_StopFillWhenSoap_LcdShowMainTitle();
    MenuManager_StopFillWhenSoap_LcdShowList();

    return kFsmGuard_True;
  }

  return kFsmGuard_False;
}

/*=============================================================================================*/
static tFsmGuard MenuManager_StopFillWhenSoap_Exit(tFsmContextPtr const pFsmContext, tFsmEvent event)
{
  MenuManager_SubMainFunction = NULL;
  MenuManager_SubTickHandler = NULL;

  tFsmDataHierachy* dataHierachy;

  dataHierachy = (tFsmDataHierachy *)MenuManager_malloc(sizeof(tFsmDataHierachy));
  dataHierachy->dataId = MENUMANAGER_STATE_STOP_FILL_WHEN_SOAP;

  pFsmContext->dataHierachy = dataHierachy;

  /* Free internal data */
  MenuManager_InternalDataPop();
  
  return kFsmGuard_True;
}

/*=============================================================================================*/
static tFsmGuard MenuManager_StopFillWhenSoap_Submenu1(tFsmContextPtr const pFsmContext, tFsmEvent event)
{
  
  
  return kFsmGuard_True;
}

/*=============================================================================================*/
static tFsmGuard MenuManager_StopFillWhenSoap_StartBut(tFsmContextPtr const pFsmContext, tFsmEvent event)
{
  if (MenuManager_StopFillWhenSoap_InternalState == MENUMANAGER_STOPFILLWHENSOAP_INTERNALSTATE_READY)
  {
    MenuManager_StopFillWhenSoap_InternalState = MENUMANAGER_STOPFILLWHENSOAP_INTERNALSTATE_RUNNING;
  }
  
  return kFsmGuard_True;
}

/*=============================================================================================*/
static tFsmGuard MenuManager_StopFillWhenSoap_StopBut(tFsmContextPtr const pFsmContext, tFsmEvent event)
{
  if (MenuManager_StopFillWhenSoap_InternalState != MENUMANAGER_STOPFILLWHENSOAP_INTERNALSTATE_READY)
  {
    return kFsmGuard_False;
  }
  
  return kFsmGuard_True;
}

/*=============================================================================================*/
static tFsmGuard MenuManager_StopFillWhenSoap_UpBut(tFsmContextPtr const pFsmContext, tFsmEvent event)
{
  if (MenuManager_StopFillWhenSoap_ListIndex > (uint32_t)0U)
  {
    MenuManager_StopFillWhenSoap_ListIndex -= (uint32_t)1U;
  }

  if (MenuManager_StopFillWhenSoap_CurPos > MENUMANAGER_COMMON_LCD_CURSOR_MIN)
  {
    MenuManager_StopFillWhenSoap_CurPos -= (uint32_t)1U;
  }

  MenuManager_StopFillWhenSoap_LcdShowList();
  
  return kFsmGuard_True;
}

/*=============================================================================================*/
static tFsmGuard MenuManager_StopFillWhenSoap_DownBut(tFsmContextPtr const pFsmContext, tFsmEvent event)
{
  if (MenuManager_StopFillWhenSoap_ListIndex < ((uint32_t)(MenuManager_StopFillWhenSoap_ChildMenuConf.childMenuNum) - (uint32_t)1U))
  {
    MenuManager_StopFillWhenSoap_ListIndex += (uint32_t)1U;
  }
  
  if (MenuManager_StopFillWhenSoap_CurPos < MenuManager_Common_LcdMaxCurPos(MenuManager_StopFillWhenSoap_ChildMenuConf.childMenuNum))
  {
    MenuManager_StopFillWhenSoap_CurPos += (uint32_t)1U;
  }
  
  MenuManager_StopFillWhenSoap_LcdShowList();
  
  return kFsmGuard_True;
}



/*=============================================================================================*/
static void MenuManager_StopFillWhenSoap_SubMainFunction(void)
{
  MenuManager_Common_ButEventMapHandler(&MenuManager_StopFillWhenSoap_ButEventMapConf);

  switch (MenuManager_StopFillWhenSoap_InternalState)
  {
    case MENUMANAGER_STOPFILLWHENSOAP_INTERNALSTATE_READY:
      break;
    case MENUMANAGER_STOPFILLWHENSOAP_INTERNALSTATE_RUNNING:
    {
      ProgramManager_SoapSetup_StopFillWhenSoap_SetData(&(MenuManager_StopFillWhenSoap_IntValMapConf[MenuManager_StopFillWhenSoap_ListIndex].mapValue));

      ProgramManager_SoapSetup_StopFillWhenSoap_GetData(&(ProgramManager_gParamConfig.soapCfg.stopFillWhenSoap));

      MenuManager_StopFillWhenSoap_InternalState = MENUMANAGER_STOPFILLWHENSOAP_INTERNALSTATE_DONE;
      
      break;
    }
    case MENUMANAGER_STOPFILLWHENSOAP_INTERNALSTATE_DONE:
    {
      MenuManager_StopFillWhenSoap_LcdShowDone();

      MenuManager_StopFillWhenSoap_InternalState = MENUMANAGER_STOPFILLWHENSOAP_INTERNALSTATE_EXITING;

      break;
    }
    case MENUMANAGER_STOPFILLWHENSOAP_INTERNALSTATE_EXITING:
    default:
      break;
  }
}

/*=============================================================================================*/
static void MenuManager_StopFillWhenSoap_SubTickHandler(void)
{
  if (MenuManager_StopFillWhenSoap_InternalState == MENUMANAGER_STOPFILLWHENSOAP_INTERNALSTATE_EXITING)
  {
    MenuManager_StopFillWhenSoap_Counter += (uint32_t)1U;

    if (MenuManager_StopFillWhenSoap_Counter >= MENUMANAGER_STOPFILLWHENSOAP_COUNTER_MAX)
    {
      MenuManager_StopFillWhenSoap_Counter = (uint32_t)0U;
      
      Fsm_TriggerEvent(&MenuManager_FsmContext, (tFsmEvent)MENUMANAGER_EVENT_SUBMENU_1);
    }
  }
}



#ifdef  __cplusplus
}
#endif
