/* 
 * File:   _1254000000_menu_TimeSoap3.c
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
#define MENUMANAGER_TIMESOAP3_INTERNALDATALENGTH                      (uint8_t)10U

#define MENUMANAGER_TIMESOAP3_INTERNALSTATE_IDX                       0U
#define MENUMANAGER_TIMESOAP3_COUNTER_IDX                             1U
#define MENUMANAGER_TIMESOAP3_CURPOS_IDX                              2U
#define MENUMANAGER_TIMESOAP3_VALUE_MIN_IDX                           3U
#define MENUMANAGER_TIMESOAP3_VALUE_MAX_IDX                           4U
#define MENUMANAGER_TIMESOAP3_VALUE_IDX                               5U
#define MENUMANAGER_TIMESOAP3_UNITVAL_IDX                             6U

#define MenuManager_TimeSoap3_InternalState                           MenuManager_GetInternalDataPtr(MENUMANAGER_TIMESOAP3_INTERNALSTATE_IDX)
#define MenuManager_TimeSoap3_Counter                                 MenuManager_GetInternalDataPtr(MENUMANAGER_TIMESOAP3_COUNTER_IDX)
#define MenuManager_TimeSoap3_CurPos                                  MenuManager_GetInternalDataPtr(MENUMANAGER_TIMESOAP3_CURPOS_IDX)
#define MenuManager_TimeSoap3_ValueMin                                MenuManager_GetInternalDataPtr(MENUMANAGER_TIMESOAP3_VALUE_MIN_IDX)
#define MenuManager_TimeSoap3_ValueMax                                MenuManager_GetInternalDataPtr(MENUMANAGER_TIMESOAP3_VALUE_MAX_IDX)
#define MenuManager_TimeSoap3_Value                                   MenuManager_GetInternalDataPtr(MENUMANAGER_TIMESOAP3_VALUE_IDX)
#define MenuManager_TimeSoap3_UnitVal(idx)                            MenuManager_GetInternalDataPtr(MENUMANAGER_TIMESOAP3_UNITVAL_IDX + idx)



#define MENUMANAGER_TIMESOAP3_INTERNALSTATE_READY                     (uint32_t)0U
#define MENUMANAGER_TIMESOAP3_INTERNALSTATE_RUNNING                   (uint32_t)1U
#define MENUMANAGER_TIMESOAP3_INTERNALSTATE_DONE                      (uint32_t)2U
#define MENUMANAGER_TIMESOAP3_INTERNALSTATE_EXITING                   (uint32_t)3U

#define MENUMANAGER_TIMESOAP3_COUNTER_MAX                             (uint32_t)50U

#define MENUMANAGER_TIMESOAP3_CURPOS_XPOS                             (uint32_t)5U
#define MENUMANAGER_TIMESOAP3_CURPOS_YPOS                             (uint32_t)5U

#define MENUMANAGER_TIMESOAP3_UNIT_XPOS                               (uint32_t)9U
#define MENUMANAGER_TIMESOAP3_UNIT_YPOS                               (uint32_t)5U

#define MENUMANAGER_TIMESOAP3_UNITVAL_LENGTH                          (uint32_t)3U



/** Menu manager main titles and child menu titles */
static const uint8_t MenuManager_TimeSoap3_MainTitleStr[] =           "TIME SOAP 3";



/** Menu manager button event mapping array */
static MenuManager_ButEventMapStruct MenuManager_TimeSoap3_ButEventMap[6] =
{
  { IOMANAGER_BUTTON_STATE_START,                                     MENUMANAGER_EVENT_START_BUT             },
  { IOMANAGER_BUTTON_STATE_STOP,                                      MENUMANAGER_EVENT_STOP_BUT              },
  { IOMANAGER_BUTTON_STATE_UP,                                        MENUMANAGER_EVENT_UP_BUT                },
  { IOMANAGER_BUTTON_STATE_DOWN,                                      MENUMANAGER_EVENT_DOWN_BUT              },
  { IOMANAGER_BUTTON_STATE_ADD,                                       MENUMANAGER_EVENT_ADD_BUT               },
  { IOMANAGER_BUTTON_STATE_SUB,                                       MENUMANAGER_EVENT_SUB_BUT               }
};

/** Menu manager button event mapping configuration */
static MenuManager_ButEventMapConfStruct MenuManager_TimeSoap3_ButEventMapConf =
{
  .butEventMapNum             = (uint8_t)6U,
  .butEventMapCfg             = &MenuManager_TimeSoap3_ButEventMap
};



/** Menu manager event handlers */
static tFsmGuard MenuManager_TimeSoap3_Entry                          (tFsmContextPtr const pFsmContext, tFsmEvent event);
static tFsmGuard MenuManager_TimeSoap3_Exit                           (tFsmContextPtr const pFsmContext, tFsmEvent event);
static tFsmGuard MenuManager_TimeSoap3_Submenu1                       (tFsmContextPtr const pFsmContext, tFsmEvent event);
static tFsmGuard MenuManager_TimeSoap3_StartBut                       (tFsmContextPtr const pFsmContext, tFsmEvent event);
static tFsmGuard MenuManager_TimeSoap3_StopBut                        (tFsmContextPtr const pFsmContext, tFsmEvent event);
static tFsmGuard MenuManager_TimeSoap3_UpBut                          (tFsmContextPtr const pFsmContext, tFsmEvent event);
static tFsmGuard MenuManager_TimeSoap3_DownBut                        (tFsmContextPtr const pFsmContext, tFsmEvent event);
static tFsmGuard MenuManager_TimeSoap3_AddBut                         (tFsmContextPtr const pFsmContext, tFsmEvent event);
static tFsmGuard MenuManager_TimeSoap3_SubBut                         (tFsmContextPtr const pFsmContext, tFsmEvent event);

/** Menu manager state machine */
tFsmEventEntry MenuManager_TimeSoap3_StateMachine[9] =
{
  FSM_TRIGGER_ENTRY             (                                     MenuManager_TimeSoap3_Entry                                                     ),
  FSM_TRIGGER_EXIT              (                                     MenuManager_TimeSoap3_Exit                                                      ),
  FSM_TRIGGER_TRANSITION_ACTION ( MENUMANAGER_EVENT_SUBMENU_1,        MenuManager_TimeSoap3_Submenu1,         MENUMANAGER_STATE_SOAP_SETUP            ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_START_BUT,        MenuManager_TimeSoap3_StartBut                                                  ),
  FSM_TRIGGER_TRANSITION_ACTION ( MENUMANAGER_EVENT_STOP_BUT,         MenuManager_TimeSoap3_StopBut,          MENUMANAGER_STATE_SOAP_SETUP            ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_UP_BUT,           MenuManager_TimeSoap3_UpBut                                                     ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_DOWN_BUT,         MenuManager_TimeSoap3_DownBut                                                   ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_ADD_BUT,          MenuManager_TimeSoap3_AddBut                                                    ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_SUB_BUT,          MenuManager_TimeSoap3_SubBut                                                    )
};



/*===============================================================================================
*                                   LOCAL FUNCTION PROTOTYPES
===============================================================================================*/

static void MenuManager_TimeSoap3_LcdShowMainTitle(void);
static void MenuManager_TimeSoap3_LcdShowAdjust(void);
static void MenuManager_TimeSoap3_LcdShowDone(void);

static void MenuManager_TimeSoap3_SubMainFunction(void);
static void MenuManager_TimeSoap3_SubTickHandler(void);



/*===============================================================================================
*                                       LOCAL FUNCTIONS
===============================================================================================*/

static void MenuManager_TimeSoap3_LcdShowMainTitle(void)
{
  /* Print main title */
  MenuManager_Common_LcdClearAdjustMenuStatic();
  MenuManager_Common_LcdShowMainTitle(MenuManager_TimeSoap3_MainTitleStr);
  MenuManager_Common_LcdShowOkExitChoice();
  MenuManager_Common_LcdUpdateAdjustMenuStatic();
}

/*=============================================================================================*/
static void MenuManager_TimeSoap3_LcdShowAdjust(void)
{
  MenuManager_Common_LcdClearAdjustMenuDynamic();

  MenuManager_Common_LcdShowAdjustUnitVal
  (
    MENUMANAGER_TIMESOAP3_CURPOS_XPOS,
    MENUMANAGER_TIMESOAP3_CURPOS_YPOS,
    &(MenuManager_TimeSoap3_UnitVal(0)),
    MENUMANAGER_TIMESOAP3_UNITVAL_LENGTH
  );
  
  MenuManager_Common_LcdShowAdjustArrow
  (
    MENUMANAGER_TIMESOAP3_CURPOS_XPOS,
    MENUMANAGER_TIMESOAP3_CURPOS_YPOS,
    MenuManager_TimeSoap3_CurPos
  );

  LCD_SetCursorPos(MENUMANAGER_TIMESOAP3_UNIT_XPOS, MENUMANAGER_TIMESOAP3_UNIT_YPOS, LCD_CURSOR_BY_FONT);
  LCD_PutString((uint8_t *)MenuManager_Common_SecondStr);

  MenuManager_Common_LcdUpdateAdjustMenuDynamic();
}

/*=============================================================================================*/
static void MenuManager_TimeSoap3_LcdShowDone(void)
{
  MenuManager_Common_LcdClearAll();

  MenuManager_Common_LcdShowAdjustUnitVal
  (
    MENUMANAGER_TIMESOAP3_CURPOS_XPOS,
    MENUMANAGER_TIMESOAP3_CURPOS_YPOS,
    &(MenuManager_TimeSoap3_UnitVal(0)),
    MENUMANAGER_TIMESOAP3_UNITVAL_LENGTH
  );

  LCD_SetCursorPos(MENUMANAGER_TIMESOAP3_UNIT_XPOS, MENUMANAGER_TIMESOAP3_UNIT_YPOS, LCD_CURSOR_BY_FONT);
  LCD_PutString((uint8_t *)MenuManager_Common_SecondStr);

  MenuManager_Common_LcdUpdateAll();
}



/*=============================================================================================*/
static tFsmGuard MenuManager_TimeSoap3_Entry(tFsmContextPtr const pFsmContext, tFsmEvent event)
{
  MenuManager_SubMainFunction = MenuManager_TimeSoap3_SubMainFunction;
  MenuManager_SubTickHandler = MenuManager_TimeSoap3_SubTickHandler;

  /* Check if previous state data hierachy is not empty */
  if (pFsmContext->dataHierachy != NULL)
  {
    if (pFsmContext->dataHierachy->dataId == MENUMANAGER_STATE_SOAP_SETUP)
    {
      /* Release previous state data hierachy */
      MenuManager_free(pFsmContext->dataHierachy);
      pFsmContext->dataHierachy = NULL;

      MenuManager_InternalDataPush(MENUMANAGER_TIMESOAP3_INTERNALDATALENGTH);

      MenuManager_TimeSoap3_InternalState = MENUMANAGER_TIMESOAP3_INTERNALSTATE_READY;
      MenuManager_TimeSoap3_Counter = (uint32_t)0U;
      MenuManager_TimeSoap3_CurPos = (uint32_t)0U;

      MenuManager_TimeSoap3_ValueMin = (uint32_t)PROGRAMMANAGER_SOAPSETUP_TIME_SOAP_MIN;
      MenuManager_TimeSoap3_ValueMax = (uint32_t)PROGRAMMANAGER_SOAPSETUP_TIME_SOAP_MAX;
      MenuManager_TimeSoap3_Value = (uint32_t)(ProgramManager_gParamConfig.soapCfg.timeSoap3);

      MenuManager_Common_DecToBcdConv
      (
        &(MenuManager_TimeSoap3_Value),
        &(MenuManager_TimeSoap3_UnitVal(0)),
        (uint8_t)MENUMANAGER_TIMESOAP3_UNITVAL_LENGTH
      );
    }
    else
    {
      return kFsmGuard_False;
    }

    MenuManager_TimeSoap3_LcdShowMainTitle();
    MenuManager_TimeSoap3_LcdShowAdjust();

    return kFsmGuard_True;
  }

  return kFsmGuard_False;
}

/*=============================================================================================*/
static tFsmGuard MenuManager_TimeSoap3_Exit(tFsmContextPtr const pFsmContext, tFsmEvent event)
{
  MenuManager_SubMainFunction = NULL;
  MenuManager_SubTickHandler = NULL;

  tFsmDataHierachy* dataHierachy;

  dataHierachy = (tFsmDataHierachy *)MenuManager_malloc(sizeof(tFsmDataHierachy));
  dataHierachy->dataId = MENUMANAGER_STATE_TIME_SOAP_3;

  pFsmContext->dataHierachy = dataHierachy;

  /* Free internal data */
  MenuManager_InternalDataPop();
  
  return kFsmGuard_True;
}

/*=============================================================================================*/
static tFsmGuard MenuManager_TimeSoap3_Submenu1(tFsmContextPtr const pFsmContext, tFsmEvent event)
{
  
  
  return kFsmGuard_True;
}

/*=============================================================================================*/
static tFsmGuard MenuManager_TimeSoap3_StartBut(tFsmContextPtr const pFsmContext, tFsmEvent event)
{
  if (MenuManager_TimeSoap3_InternalState == MENUMANAGER_TIMESOAP3_INTERNALSTATE_READY)
  {
    MenuManager_TimeSoap3_InternalState = MENUMANAGER_TIMESOAP3_INTERNALSTATE_RUNNING;
  }
  
  return kFsmGuard_True;
}

/*=============================================================================================*/
static tFsmGuard MenuManager_TimeSoap3_StopBut(tFsmContextPtr const pFsmContext, tFsmEvent event)
{
  if (MenuManager_TimeSoap3_InternalState != MENUMANAGER_TIMESOAP3_INTERNALSTATE_READY)
  {
    return kFsmGuard_False;
  }
  
  return kFsmGuard_True;
}

/*=============================================================================================*/
static tFsmGuard MenuManager_TimeSoap3_UpBut(tFsmContextPtr const pFsmContext, tFsmEvent event)
{
  uint32_t tempUnitVal;
  uint32_t tempVal;

  if (MenuManager_TimeSoap3_InternalState == MENUMANAGER_TIMESOAP3_INTERNALSTATE_READY)
  {
    tempUnitVal = MenuManager_TimeSoap3_UnitVal(MenuManager_TimeSoap3_CurPos);

    MenuManager_TimeSoap3_UnitVal(MenuManager_TimeSoap3_CurPos) += (uint32_t)1U;

    if (MenuManager_TimeSoap3_UnitVal(MenuManager_TimeSoap3_CurPos) > (uint32_t)9U)
    {
      MenuManager_TimeSoap3_UnitVal(MenuManager_TimeSoap3_CurPos) = (uint32_t)0U;
    }

    MenuManager_Common_BcdToDecConv(&tempVal, &(MenuManager_TimeSoap3_UnitVal(0)), (uint8_t)MENUMANAGER_TIMESOAP3_UNITVAL_LENGTH);

    if ((tempVal < MenuManager_TimeSoap3_ValueMin) || (tempVal > MenuManager_TimeSoap3_ValueMax))
    {
      MenuManager_TimeSoap3_UnitVal(MenuManager_TimeSoap3_CurPos) = tempUnitVal;
    }

    MenuManager_TimeSoap3_LcdShowAdjust();
  }

  return kFsmGuard_True;
}

/*=============================================================================================*/
static tFsmGuard MenuManager_TimeSoap3_DownBut(tFsmContextPtr const pFsmContext, tFsmEvent event)
{
  uint32_t tempUnitVal;
  uint32_t tempVal;

  if (MenuManager_TimeSoap3_InternalState == MENUMANAGER_TIMESOAP3_INTERNALSTATE_READY)
  {
    tempUnitVal = MenuManager_TimeSoap3_UnitVal(MenuManager_TimeSoap3_CurPos);

    MenuManager_TimeSoap3_UnitVal(MenuManager_TimeSoap3_CurPos) -= (uint32_t)1U;

    if (MenuManager_TimeSoap3_UnitVal(MenuManager_TimeSoap3_CurPos) > (uint32_t)9U)
    {
      MenuManager_TimeSoap3_UnitVal(MenuManager_TimeSoap3_CurPos) = (uint32_t)9U;
    }

    MenuManager_Common_BcdToDecConv(&tempVal, &(MenuManager_TimeSoap3_UnitVal(0)), (uint8_t)MENUMANAGER_TIMESOAP3_UNITVAL_LENGTH);

    if ((tempVal < MenuManager_TimeSoap3_ValueMin) || (tempVal > MenuManager_TimeSoap3_ValueMax))
    {
      MenuManager_TimeSoap3_UnitVal(MenuManager_TimeSoap3_CurPos) = tempUnitVal;
    }

    MenuManager_TimeSoap3_LcdShowAdjust();
  }
  
  return kFsmGuard_True;
}

/*=============================================================================================*/
static tFsmGuard MenuManager_TimeSoap3_AddBut(tFsmContextPtr const pFsmContext, tFsmEvent event)
{
  if (MenuManager_TimeSoap3_InternalState == MENUMANAGER_TIMESOAP3_INTERNALSTATE_READY)
  {
    if (MenuManager_TimeSoap3_CurPos > (uint32_t)0U)
    {
      MenuManager_TimeSoap3_CurPos -= (uint32_t)1U;
    }

    MenuManager_TimeSoap3_LcdShowAdjust();
  }
  
  return kFsmGuard_True;
}

/*=============================================================================================*/
static tFsmGuard MenuManager_TimeSoap3_SubBut(tFsmContextPtr const pFsmContext, tFsmEvent event)
{
  if (MenuManager_TimeSoap3_InternalState == MENUMANAGER_TIMESOAP3_INTERNALSTATE_READY)
  {
    if (MenuManager_TimeSoap3_CurPos < (MENUMANAGER_TIMESOAP3_UNITVAL_LENGTH - (uint32_t)1U))
    {
      MenuManager_TimeSoap3_CurPos += (uint32_t)1U;
    }
    
    MenuManager_TimeSoap3_LcdShowAdjust();
  }
  
  return kFsmGuard_True;
}



/*=============================================================================================*/
static void MenuManager_TimeSoap3_SubMainFunction(void)
{
  MenuManager_Common_ButEventMapHandler(&MenuManager_TimeSoap3_ButEventMapConf);

  switch (MenuManager_TimeSoap3_InternalState)
  {
    case MENUMANAGER_TIMESOAP3_INTERNALSTATE_READY:
      break;
    case MENUMANAGER_TIMESOAP3_INTERNALSTATE_RUNNING:
    {
      uint16_t tempTimeSoap3;

      MenuManager_Common_BcdToDecConv(&(MenuManager_TimeSoap3_Value), &(MenuManager_TimeSoap3_UnitVal(0)), (uint8_t)MENUMANAGER_TIMESOAP3_UNITVAL_LENGTH);

      tempTimeSoap3 = (uint8_t)MenuManager_TimeSoap3_Value;

      ProgramManager_SoapSetup_TimeSoap3_SetData(&tempTimeSoap3);

      ProgramManager_SoapSetup_TimeSoap3_GetData(&(ProgramManager_gParamConfig.soapCfg.timeSoap3));

      MenuManager_TimeSoap3_InternalState = MENUMANAGER_TIMESOAP3_INTERNALSTATE_DONE;
      
      break;
    }
    case MENUMANAGER_TIMESOAP3_INTERNALSTATE_DONE:
    {
      MenuManager_TimeSoap3_LcdShowDone();

      MenuManager_TimeSoap3_InternalState = MENUMANAGER_TIMESOAP3_INTERNALSTATE_EXITING;

      break;
    }
    case MENUMANAGER_TIMESOAP3_INTERNALSTATE_EXITING:
    default:
      break;
  }
}

/*=============================================================================================*/
static void MenuManager_TimeSoap3_SubTickHandler(void)
{
  if (MenuManager_TimeSoap3_InternalState == MENUMANAGER_TIMESOAP3_INTERNALSTATE_EXITING)
  {
    MenuManager_TimeSoap3_Counter += (uint32_t)1U;

    if (MenuManager_TimeSoap3_Counter >= MENUMANAGER_TIMESOAP3_COUNTER_MAX)
    {
      MenuManager_TimeSoap3_Counter = (uint32_t)0U;
      
      Fsm_TriggerEvent(&MenuManager_FsmContext, (tFsmEvent)MENUMANAGER_EVENT_SUBMENU_1);
    }
  }
}



#ifdef  __cplusplus
}
#endif
