/* 
 * File:   _1250000000_menu_SoapSetup.c
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
#define MENUMANAGER_SOAPSETUP_INTERNALDATALENGTH                      (uint8_t)2U

#define MENUMANAGER_SOAPSETUP_LISTINDEX_IDX                           0U
#define MENUMANAGER_SOAPSETUP_CURPOS_IDX                              1U

#define MenuManager_SoapSetup_ListIndex                               MenuManager_GetInternalDataPtr(MENUMANAGER_SOAPSETUP_LISTINDEX_IDX)
#define MenuManager_SoapSetup_CurPos                                  MenuManager_GetInternalDataPtr(MENUMANAGER_SOAPSETUP_CURPOS_IDX)



/** Menu manager main titles and child menu titles */
static const uint8_t MenuManager_StopFillWhenSoap_ChildTitleStr[] =   "STOP FILL W SOAP";
static const uint8_t MenuManager_TimeSoap1_ChildTitleStr[] =          "SET TIME SOAP 1";
static const uint8_t MenuManager_TimeSoap2_ChildTitleStr[] =          "SET TIME SOAP 2";
static const uint8_t MenuManager_TimeSoap3_ChildTitleStr[] =          "SET TIME SOAP 3";

static const uint8_t MenuManager_SoapSetup_MainTitleStr[] =           "SOAP SETUP";

/** Menu manager child menu array */
static MenuManager_ChildMenuStruct MenuManager_SoapSetup_ChildMenu[4] =
{
  { &MenuManager_StopFillWhenSoap_ChildTitleStr,                      MENUMANAGER_EVENT_SUBMENU_1             },
  { &MenuManager_TimeSoap1_ChildTitleStr,                             MENUMANAGER_EVENT_SUBMENU_2             },
  { &MenuManager_TimeSoap2_ChildTitleStr,                             MENUMANAGER_EVENT_SUBMENU_3             },
  { &MenuManager_TimeSoap3_ChildTitleStr,                             MENUMANAGER_EVENT_SUBMENU_4             }
};

/** Menu manager child menu configuration */
static MenuManager_ChildMenuConfStruct MenuManager_SoapSetup_ChildMenuConf =
{
  .childMenuNum               = (uint8_t)4U,
  .childMenuCfg               = &MenuManager_SoapSetup_ChildMenu
};



/** Menu manager button event mapping array */
static MenuManager_ButEventMapStruct MenuManager_SoapSetup_ButEventMap[4] =
{
  { IOMANAGER_BUTTON_STATE_START,                                     MENUMANAGER_EVENT_START_BUT             },
  { IOMANAGER_BUTTON_STATE_STOP,                                      MENUMANAGER_EVENT_STOP_BUT              },
  { IOMANAGER_BUTTON_STATE_UP,                                        MENUMANAGER_EVENT_UP_BUT                },
  { IOMANAGER_BUTTON_STATE_DOWN,                                      MENUMANAGER_EVENT_DOWN_BUT              }
};

/** Menu manager button event mapping configuration */
static MenuManager_ButEventMapConfStruct MenuManager_SoapSetup_ButEventMapConf =
{
  .butEventMapNum             = (uint8_t)4U,
  .butEventMapCfg             = &MenuManager_SoapSetup_ButEventMap
};



/** Menu manager event handlers */
static tFsmGuard MenuManager_SoapSetup_Entry                          (tFsmContextPtr const pFsmContext, tFsmEvent event);
static tFsmGuard MenuManager_SoapSetup_StartBut                       (tFsmContextPtr const pFsmContext, tFsmEvent event);
static tFsmGuard MenuManager_SoapSetup_StopBut                        (tFsmContextPtr const pFsmContext, tFsmEvent event);
static tFsmGuard MenuManager_SoapSetup_UpBut                          (tFsmContextPtr const pFsmContext, tFsmEvent event);
static tFsmGuard MenuManager_SoapSetup_DownBut                        (tFsmContextPtr const pFsmContext, tFsmEvent event);

/** Menu manager state machine */
tFsmEventEntry MenuManager_SoapSetup_StateMachine[9] =
{
  FSM_TRIGGER_ENTRY             (                                     MenuManager_SoapSetup_Entry                                                     ),
  FSM_TRIGGER_TRANSITION        ( MENUMANAGER_EVENT_SUBMENU_1,                                                MENUMANAGER_STATE_STOP_FILL_WHEN_SOAP   ),
  FSM_TRIGGER_TRANSITION        ( MENUMANAGER_EVENT_SUBMENU_2,                                                MENUMANAGER_STATE_TIME_SOAP_1           ),
  FSM_TRIGGER_TRANSITION        ( MENUMANAGER_EVENT_SUBMENU_3,                                                MENUMANAGER_STATE_TIME_SOAP_2           ),
  FSM_TRIGGER_TRANSITION        ( MENUMANAGER_EVENT_SUBMENU_4,                                                MENUMANAGER_STATE_TIME_SOAP_3           ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_UP_BUT,           MenuManager_SoapSetup_UpBut                                                     ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_DOWN_BUT,         MenuManager_SoapSetup_DownBut                                                   ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_START_BUT,        MenuManager_SoapSetup_StartBut                                                  ),
  FSM_TRIGGER_TRANSITION_ACTION ( MENUMANAGER_EVENT_STOP_BUT,         MenuManager_SoapSetup_StopBut,          MENUMANAGER_STATE_MACHINE_SETUP         )
};



/*===============================================================================================
*                                   LOCAL FUNCTION PROTOTYPES
===============================================================================================*/

static void MenuManager_SoapSetup_LcdShowMainTitle(void);
static void MenuManager_SoapSetup_LcdShowList(void);

static void MenuManager_SoapSetup_SubMainFunction(void);
static void MenuManager_SoapSetup_SubTickHandler(void);



/*===============================================================================================
*                                       LOCAL FUNCTIONS
===============================================================================================*/

static void MenuManager_SoapSetup_LcdShowMainTitle(void)
{
  /* Print main title */
  MenuManager_Common_LcdClearElementMenuStatic();
  MenuManager_Common_LcdShowMainTitle(MenuManager_SoapSetup_MainTitleStr);
  MenuManager_Common_LcdUpdateElementMenuStatic();
}

/*=============================================================================================*/
static void MenuManager_SoapSetup_LcdShowList(void)
{
  uint32_t currentListIndex;
  uint32_t currentCursorPos;
  uint32_t listLength;
  
  currentListIndex = MenuManager_Common_LcdGetStartListIndex(MenuManager_SoapSetup_ListIndex, MenuManager_SoapSetup_CurPos);
  currentCursorPos = (uint32_t)MENUMANAGER_COMMON_LCD_CURSOR_MIN;

  listLength = MenuManager_Common_LcdGetListLength((uint32_t)(MenuManager_SoapSetup_ChildMenuConf.childMenuNum));
  
  MenuManager_Common_LcdClearElementMenuDynamic();
  
  for (; currentCursorPos < (listLength + (uint32_t)MENUMANAGER_COMMON_LCD_CURSOR_MIN); currentCursorPos++, currentListIndex++)
  {
    MenuManager_Common_LcdShowListElementName
    (
      (uint8_t *)*((*(MenuManager_SoapSetup_ChildMenuConf.childMenuCfg))[currentListIndex].childMenuTitleStr),
      MENUMANAGER_COMMON_LCD_ELEMENT_MENU_LENGTH,
      MENUMANAGER_COMMON_LCD_ELEMENT_MENU_XPOS,
      currentCursorPos
    );
  }
  
  MenuManager_Common_LcdShowChosenMark(MenuManager_SoapSetup_CurPos);
  
  MenuManager_Common_LcdShowScrollBar(MenuManager_SoapSetup_ListIndex, MenuManager_SoapSetup_CurPos, MenuManager_SoapSetup_ChildMenuConf.childMenuNum);
  
  MenuManager_Common_LcdUpdateElementMenuDynamic();
}



/*=============================================================================================*/
static tFsmGuard MenuManager_SoapSetup_Entry(tFsmContextPtr const pFsmContext, tFsmEvent event)
{
  MenuManager_SubMainFunction = MenuManager_SoapSetup_SubMainFunction;
  MenuManager_SubTickHandler = MenuManager_SoapSetup_SubTickHandler;

  /* Check if previous state data hierachy is not empty */
  if (pFsmContext->dataHierachy != NULL)
  {
    if ((pFsmContext->dataHierachy->dataId == MENUMANAGER_STATE_STOP_FILL_WHEN_SOAP)  || \
        (pFsmContext->dataHierachy->dataId == MENUMANAGER_STATE_TIME_SOAP_1)          || \
        (pFsmContext->dataHierachy->dataId == MENUMANAGER_STATE_TIME_SOAP_2)          || \
        (pFsmContext->dataHierachy->dataId == MENUMANAGER_STATE_TIME_SOAP_3))
    {
      /* Release previous state data hierachy */
      MenuManager_free(pFsmContext->dataHierachy);
      pFsmContext->dataHierachy = NULL;
    }
    else if (pFsmContext->dataHierachy->dataId == MENUMANAGER_STATE_MACHINE_SETUP)
    {
      /* Release previous state data hierachy */
      MenuManager_free(pFsmContext->dataHierachy);
      pFsmContext->dataHierachy = NULL;

      MenuManager_InternalDataPush(MENUMANAGER_SOAPSETUP_INTERNALDATALENGTH);

      MenuManager_SoapSetup_ListIndex = (uint32_t)0U;
      MenuManager_SoapSetup_CurPos = MENUMANAGER_COMMON_LCD_CURSOR_MIN;
    }
    else
    {
      return kFsmGuard_False;
    }

    MenuManager_SoapSetup_LcdShowMainTitle();
    MenuManager_SoapSetup_LcdShowList();

    return kFsmGuard_True;
  }

  return kFsmGuard_False;
}

/*=============================================================================================*/
static tFsmGuard MenuManager_SoapSetup_StartBut(tFsmContextPtr const pFsmContext, tFsmEvent event)
{
  tFsmDataHierachy* dataHierachy;

  dataHierachy = (tFsmDataHierachy *)MenuManager_malloc(sizeof(tFsmDataHierachy));
  dataHierachy->dataId = MENUMANAGER_STATE_SOAP_SETUP;

  pFsmContext->dataHierachy = dataHierachy;

  Fsm_TriggerEvent( &MenuManager_FsmContext, \
                    (tFsmEvent)((*(MenuManager_SoapSetup_ChildMenuConf.childMenuCfg))[MenuManager_SoapSetup_ListIndex].childMenuEvent));
  
  return kFsmGuard_True;
}

/*=============================================================================================*/
static tFsmGuard MenuManager_SoapSetup_StopBut(tFsmContextPtr const pFsmContext, tFsmEvent event)
{
  MenuManager_SubMainFunction = NULL;
  MenuManager_SubTickHandler = NULL;

  tFsmDataHierachy* dataHierachy;

  dataHierachy = (tFsmDataHierachy *)MenuManager_malloc(sizeof(tFsmDataHierachy));
  dataHierachy->dataId = MENUMANAGER_STATE_SOAP_SETUP;

  pFsmContext->dataHierachy = dataHierachy;

  /* Free internal data */
  MenuManager_InternalDataPop();
  
  return kFsmGuard_True;
}

/*=============================================================================================*/
static tFsmGuard MenuManager_SoapSetup_UpBut(tFsmContextPtr const pFsmContext, tFsmEvent event)
{
  if (MenuManager_SoapSetup_ListIndex > (uint32_t)0U)
  {
    MenuManager_SoapSetup_ListIndex -= (uint32_t)1U;
  }

  if (MenuManager_SoapSetup_CurPos > MENUMANAGER_COMMON_LCD_CURSOR_MIN)
  {
    MenuManager_SoapSetup_CurPos -= (uint32_t)1U;
  }

  MenuManager_SoapSetup_LcdShowList();
  
  return kFsmGuard_True;
}

/*=============================================================================================*/
static tFsmGuard MenuManager_SoapSetup_DownBut(tFsmContextPtr const pFsmContext, tFsmEvent event)
{
  if (MenuManager_SoapSetup_ListIndex < ((uint32_t)(MenuManager_SoapSetup_ChildMenuConf.childMenuNum) - (uint32_t)1U))
  {
    MenuManager_SoapSetup_ListIndex += (uint32_t)1U;
  }
  
  if (MenuManager_SoapSetup_CurPos < MenuManager_Common_LcdMaxCurPos(MenuManager_SoapSetup_ChildMenuConf.childMenuNum))
  {
    MenuManager_SoapSetup_CurPos += (uint32_t)1U;
  }
  
  MenuManager_SoapSetup_LcdShowList();
  
  return kFsmGuard_True;
}



/*=============================================================================================*/
static void MenuManager_SoapSetup_SubMainFunction(void)
{
  MenuManager_Common_ButEventMapHandler(&MenuManager_SoapSetup_ButEventMapConf);
}

/*=============================================================================================*/
static void MenuManager_SoapSetup_SubTickHandler(void)
{
  
}



#ifdef  __cplusplus
}
#endif
