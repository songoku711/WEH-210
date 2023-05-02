/* 
 * File:   _1236000000_menu_SoapStartLevel.c
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
#define MENUMANAGER_SOAPSTARTLEVEL_INTERNALDATALENGTH                 (uint8_t)9U

#define MENUMANAGER_SOAPSTARTLEVEL_INTERNALSTATE_IDX                  0U
#define MENUMANAGER_SOAPSTARTLEVEL_COUNTER_IDX                        1U
#define MENUMANAGER_SOAPSTARTLEVEL_CURPOS_IDX                         2U
#define MENUMANAGER_SOAPSTARTLEVEL_VALUE_MIN_IDX                      3U
#define MENUMANAGER_SOAPSTARTLEVEL_VALUE_MAX_IDX                      4U
#define MENUMANAGER_SOAPSTARTLEVEL_VALUE_IDX                          5U
#define MENUMANAGER_SOAPSTARTLEVEL_UNITVAL_IDX                        6U

#define MenuManager_SoapStartLevel_InternalState                      MenuManager_GetInternalDataPtr(MENUMANAGER_SOAPSTARTLEVEL_INTERNALSTATE_IDX)
#define MenuManager_SoapStartLevel_Counter                            MenuManager_GetInternalDataPtr(MENUMANAGER_SOAPSTARTLEVEL_COUNTER_IDX)
#define MenuManager_SoapStartLevel_CurPos                             MenuManager_GetInternalDataPtr(MENUMANAGER_SOAPSTARTLEVEL_CURPOS_IDX)
#define MenuManager_SoapStartLevel_ValueMin                           MenuManager_GetInternalDataPtr(MENUMANAGER_SOAPSTARTLEVEL_VALUE_MIN_IDX)
#define MenuManager_SoapStartLevel_ValueMax                           MenuManager_GetInternalDataPtr(MENUMANAGER_SOAPSTARTLEVEL_VALUE_MAX_IDX)
#define MenuManager_SoapStartLevel_Value                              MenuManager_GetInternalDataPtr(MENUMANAGER_SOAPSTARTLEVEL_VALUE_IDX)
#define MenuManager_SoapStartLevel_UnitVal(idx)                       MenuManager_GetInternalDataPtr(MENUMANAGER_SOAPSTARTLEVEL_UNITVAL_IDX + idx)



#define MENUMANAGER_SOAPSTARTLEVEL_INTERNALSTATE_READY                (uint32_t)0U
#define MENUMANAGER_SOAPSTARTLEVEL_INTERNALSTATE_RUNNING              (uint32_t)1U
#define MENUMANAGER_SOAPSTARTLEVEL_INTERNALSTATE_DONE                 (uint32_t)2U
#define MENUMANAGER_SOAPSTARTLEVEL_INTERNALSTATE_EXITING              (uint32_t)3U

#define MENUMANAGER_SOAPSTARTLEVEL_COUNTER_MAX                        (uint32_t)50U

#define MENUMANAGER_SOAPSTARTLEVEL_CURPOS_XPOS                        (uint32_t)9U
#define MENUMANAGER_SOAPSTARTLEVEL_CURPOS_YPOS                        (uint32_t)5U

#define MENUMANAGER_SOAPSTARTLEVEL_UNITVAL_LENGTH                     (uint32_t)3U



/** Menu manager main titles and child menu titles */
static const uint8_t MenuManager_SoapStartLevel_MainTitleStr[] =      "SOAP LEVEL";



/** Menu manager button event mapping array */
static MenuManager_ButEventMapStruct MenuManager_SoapStartLevel_ButEventMap[6] =
{
  { IOMANAGER_BUTTON_STATE_START,                                     MENUMANAGER_EVENT_START_BUT             },
  { IOMANAGER_BUTTON_STATE_STOP,                                      MENUMANAGER_EVENT_STOP_BUT              },
  { IOMANAGER_BUTTON_STATE_UP,                                        MENUMANAGER_EVENT_UP_BUT                },
  { IOMANAGER_BUTTON_STATE_DOWN,                                      MENUMANAGER_EVENT_DOWN_BUT              },
  { IOMANAGER_BUTTON_STATE_ADD,                                       MENUMANAGER_EVENT_ADD_BUT               },
  { IOMANAGER_BUTTON_STATE_SUB,                                       MENUMANAGER_EVENT_SUB_BUT               }
};

/** Menu manager button event mapping configuration */
static MenuManager_ButEventMapConfStruct MenuManager_SoapStartLevel_ButEventMapConf =
{
  .butEventMapNum             = (uint8_t)6U,
  .butEventMapCfg             = &MenuManager_SoapStartLevel_ButEventMap
};



/** Menu manager event handlers */
static Fsm_GuardType MenuManager_SoapStartLevel_Entry                (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_SoapStartLevel_Exit                 (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_SoapStartLevel_StartBut             (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_SoapStartLevel_StopBut              (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_SoapStartLevel_UpBut                (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_SoapStartLevel_DownBut              (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_SoapStartLevel_AddBut               (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_SoapStartLevel_SubBut               (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);

/** Menu manager state machine */
Fsm_EventEntryStruct MenuManager_SoapStartLevel_StateMachine[9] =
{
  FSM_TRIGGER_ENTRY             (                                     MenuManager_SoapStartLevel_Entry                                                ),
  FSM_TRIGGER_EXIT              (                                     MenuManager_SoapStartLevel_Exit                                                 ),
  FSM_TRIGGER_TRANSITION        ( MENUMANAGER_EVENT_PREV,                                                     MENUMANAGER_STATE_FILL_LEVEL_SETUP      ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_START_BUT,        MenuManager_SoapStartLevel_StartBut                                             ),
  FSM_TRIGGER_TRANSITION_ACTION ( MENUMANAGER_EVENT_STOP_BUT,         MenuManager_SoapStartLevel_StopBut,     MENUMANAGER_STATE_FILL_LEVEL_SETUP      ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_UP_BUT,           MenuManager_SoapStartLevel_UpBut                                                ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_DOWN_BUT,         MenuManager_SoapStartLevel_DownBut                                              ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_ADD_BUT,          MenuManager_SoapStartLevel_AddBut                                               ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_SUB_BUT,          MenuManager_SoapStartLevel_SubBut                                               )
};



/*===============================================================================================
*                                   LOCAL FUNCTION PROTOTYPES
===============================================================================================*/

static void MenuManager_SoapStartLevel_LcdShowMainTitle(void);
static void MenuManager_SoapStartLevel_LcdShowAdjust(void);
static void MenuManager_SoapStartLevel_LcdShowDone(void);

static void MenuManager_SoapStartLevel_SubMainFunction(void);
static void MenuManager_SoapStartLevel_SubTickHandler(void);



/*===============================================================================================
*                                       LOCAL FUNCTIONS
===============================================================================================*/

static void MenuManager_SoapStartLevel_LcdShowMainTitle(void)
{
  /* Print main title */
  MenuManager_Common_LcdClearAdjustMenuStatic();
  MenuManager_Common_LcdShowMainTitle(MenuManager_SoapStartLevel_MainTitleStr);
  MenuManager_Common_LcdShowOkExitChoice();
  MenuManager_Common_LcdUpdateAdjustMenuStatic();
}

/*=============================================================================================*/
static void MenuManager_SoapStartLevel_LcdShowAdjust(void)
{
  MenuManager_Common_LcdClearAdjustMenuDynamic();

  MenuManager_Common_LcdShowAdjustUnitVal
  (
    MENUMANAGER_SOAPSTARTLEVEL_CURPOS_XPOS,
    MENUMANAGER_SOAPSTARTLEVEL_CURPOS_YPOS,
    &(MenuManager_SoapStartLevel_UnitVal(0)),
    MENUMANAGER_SOAPSTARTLEVEL_UNITVAL_LENGTH
  );
  
  MenuManager_Common_LcdShowAdjustArrow
  (
    MENUMANAGER_SOAPSTARTLEVEL_CURPOS_XPOS,
    MENUMANAGER_SOAPSTARTLEVEL_CURPOS_YPOS,
    MenuManager_SoapStartLevel_CurPos
  );

  MenuManager_Common_LcdUpdateAdjustMenuDynamic();
}

/*=============================================================================================*/
static void MenuManager_SoapStartLevel_LcdShowDone(void)
{
  MenuManager_Common_LcdClearAll();

  MenuManager_Common_LcdShowAdjustUnitVal
  (
    MENUMANAGER_SOAPSTARTLEVEL_CURPOS_XPOS,
    MENUMANAGER_SOAPSTARTLEVEL_CURPOS_YPOS,
    &(MenuManager_SoapStartLevel_UnitVal(0)),
    MENUMANAGER_SOAPSTARTLEVEL_UNITVAL_LENGTH
  );

  MenuManager_Common_LcdUpdateAll();
}



/*=============================================================================================*/
static Fsm_GuardType MenuManager_SoapStartLevel_Entry(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  HAL_StatusTypeDef retVal = HAL_OK;

  /* Check if previous state data hierachy is not empty */
  if (pFsmContext->dataHierachy != NULL)
  {
    if (pFsmContext->dataHierachy->dataId == MENUMANAGER_STATE_FILL_LEVEL_SETUP)
    {
      /* Release previous state data hierachy */
      MenuManager_free(pFsmContext->dataHierachy);
      pFsmContext->dataHierachy = NULL;

      MenuManager_InternalDataPush(MENUMANAGER_SOAPSTARTLEVEL_INTERNALDATALENGTH);

      MenuManager_SoapStartLevel_InternalState = MENUMANAGER_SOAPSTARTLEVEL_INTERNALSTATE_READY;
      MenuManager_SoapStartLevel_Counter = (uint32_t)0U;
      MenuManager_SoapStartLevel_CurPos = (uint32_t)0U;

      MenuManager_SoapStartLevel_ValueMin = (uint32_t)PROGRAMMANAGER_FILLLEVELSETUP_WATER_LEVEL_MIN;
      MenuManager_SoapStartLevel_ValueMax = (uint32_t)PROGRAMMANAGER_FILLLEVELSETUP_WATER_LEVEL_MAX;

      MenuManager_SoapStartLevel_Value = (uint32_t)(ProgramManager_gParamConfig.fillLevelCfg.soapStartLevel);

      MenuManager_Common_DecToBcdConv
      (
        &(MenuManager_SoapStartLevel_Value),
        &(MenuManager_SoapStartLevel_UnitVal(0)),
        (uint8_t)MENUMANAGER_SOAPSTARTLEVEL_UNITVAL_LENGTH
      );
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
    MenuManager_SoapStartLevel_LcdShowMainTitle();
    MenuManager_SoapStartLevel_LcdShowAdjust();

    MenuManager_SubMainFunction = MenuManager_SoapStartLevel_SubMainFunction;
    MenuManager_SubTickHandler = MenuManager_SoapStartLevel_SubTickHandler;

    return FSM_GUARD_TRUE;
  }

  return FSM_GUARD_FALSE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_SoapStartLevel_Exit(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  Fsm_DataHierachyStruct* dataHierachy;

  MenuManager_SubMainFunction = NULL;
  MenuManager_SubTickHandler = NULL;

  dataHierachy = (Fsm_DataHierachyStruct *)MenuManager_malloc(sizeof(Fsm_DataHierachyStruct));
  dataHierachy->dataId = MENUMANAGER_STATE_SOAP_START_LEVEL;

  pFsmContext->dataHierachy = dataHierachy;

  /* Free internal data */
  MenuManager_InternalDataPop();
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_SoapStartLevel_StartBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_SoapStartLevel_InternalState == MENUMANAGER_SOAPSTARTLEVEL_INTERNALSTATE_READY)
  {
    MenuManager_SoapStartLevel_InternalState = MENUMANAGER_SOAPSTARTLEVEL_INTERNALSTATE_RUNNING;
  }
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_SoapStartLevel_StopBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_SoapStartLevel_InternalState != MENUMANAGER_SOAPSTARTLEVEL_INTERNALSTATE_READY)
  {
    return FSM_GUARD_FALSE;
  }
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_SoapStartLevel_UpBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  uint32_t tempUnitVal;
  uint32_t tempVal;

  if (MenuManager_SoapStartLevel_InternalState == MENUMANAGER_SOAPSTARTLEVEL_INTERNALSTATE_READY)
  {
    tempUnitVal = MenuManager_SoapStartLevel_UnitVal(MenuManager_SoapStartLevel_CurPos);

    MenuManager_SoapStartLevel_UnitVal(MenuManager_SoapStartLevel_CurPos) += (uint32_t)1U;

    if (MenuManager_SoapStartLevel_UnitVal(MenuManager_SoapStartLevel_CurPos) > (uint32_t)9U)
    {
      MenuManager_SoapStartLevel_UnitVal(MenuManager_SoapStartLevel_CurPos) = (uint32_t)0U;
    }

    MenuManager_Common_BcdToDecConv(&tempVal, &(MenuManager_SoapStartLevel_UnitVal(0)), (uint8_t)MENUMANAGER_SOAPSTARTLEVEL_UNITVAL_LENGTH);

    if ((tempVal <= MenuManager_SoapStartLevel_ValueMin) || (tempVal >= MenuManager_SoapStartLevel_ValueMax))
    {
      MenuManager_SoapStartLevel_UnitVal(MenuManager_SoapStartLevel_CurPos) = tempUnitVal;
    }

    MenuManager_SoapStartLevel_LcdShowAdjust();
  }

  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_SoapStartLevel_DownBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  uint32_t tempUnitVal;
  uint32_t tempVal;

  if (MenuManager_SoapStartLevel_InternalState == MENUMANAGER_SOAPSTARTLEVEL_INTERNALSTATE_READY)
  {
    tempUnitVal = MenuManager_SoapStartLevel_UnitVal(MenuManager_SoapStartLevel_CurPos);

    MenuManager_SoapStartLevel_UnitVal(MenuManager_SoapStartLevel_CurPos) -= (uint32_t)1U;

    if (MenuManager_SoapStartLevel_UnitVal(MenuManager_SoapStartLevel_CurPos) > (uint32_t)9U)
    {
      MenuManager_SoapStartLevel_UnitVal(MenuManager_SoapStartLevel_CurPos) = (uint32_t)9U;
    }

    MenuManager_Common_BcdToDecConv(&tempVal, &(MenuManager_SoapStartLevel_UnitVal(0)), (uint8_t)MENUMANAGER_SOAPSTARTLEVEL_UNITVAL_LENGTH);

    if ((tempVal <= MenuManager_SoapStartLevel_ValueMin) || (tempVal >= MenuManager_SoapStartLevel_ValueMax))
    {
      MenuManager_SoapStartLevel_UnitVal(MenuManager_SoapStartLevel_CurPos) = tempUnitVal;
    }

    MenuManager_SoapStartLevel_LcdShowAdjust();
  }
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_SoapStartLevel_AddBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_SoapStartLevel_InternalState == MENUMANAGER_SOAPSTARTLEVEL_INTERNALSTATE_READY)
  {
    if (MenuManager_SoapStartLevel_CurPos > (uint32_t)0U)
    {
      MenuManager_SoapStartLevel_CurPos -= (uint32_t)1U;
    }

    MenuManager_SoapStartLevel_LcdShowAdjust();
  }
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_SoapStartLevel_SubBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_SoapStartLevel_InternalState == MENUMANAGER_SOAPSTARTLEVEL_INTERNALSTATE_READY)
  {
    if (MenuManager_SoapStartLevel_CurPos < (MENUMANAGER_SOAPSTARTLEVEL_UNITVAL_LENGTH - (uint32_t)1U))
    {
      MenuManager_SoapStartLevel_CurPos += (uint32_t)1U;
    }
    
    MenuManager_SoapStartLevel_LcdShowAdjust();
  }
  
  return FSM_GUARD_TRUE;
}



/*=============================================================================================*/
static void MenuManager_SoapStartLevel_SubMainFunction(void)
{
  MenuManager_Common_ButEventMapHandler(&MenuManager_SoapStartLevel_ButEventMapConf);

  switch (MenuManager_SoapStartLevel_InternalState)
  {
    case MENUMANAGER_SOAPSTARTLEVEL_INTERNALSTATE_READY:
      break;
    case MENUMANAGER_SOAPSTARTLEVEL_INTERNALSTATE_RUNNING:
    {
      uint16_t tempSoapStartLevel;

      MenuManager_Common_BcdToDecConv(&(MenuManager_SoapStartLevel_Value), &(MenuManager_SoapStartLevel_UnitVal(0)), (uint8_t)MENUMANAGER_SOAPSTARTLEVEL_UNITVAL_LENGTH);

      tempSoapStartLevel = (uint16_t)MenuManager_SoapStartLevel_Value;

      ProgramManager_FillLevelSetup_SoapStartLevel_SetData(&tempSoapStartLevel);

      ProgramManager_FillLevelSetup_SoapStartLevel_GetData(&(ProgramManager_gParamConfig.fillLevelCfg.soapStartLevel));

      MenuManager_SoapStartLevel_InternalState = MENUMANAGER_SOAPSTARTLEVEL_INTERNALSTATE_DONE;
      
      break;
    }
    case MENUMANAGER_SOAPSTARTLEVEL_INTERNALSTATE_DONE:
    {
      MenuManager_SoapStartLevel_LcdShowDone();

      MenuManager_SoapStartLevel_InternalState = MENUMANAGER_SOAPSTARTLEVEL_INTERNALSTATE_EXITING;

      break;
    }
    case MENUMANAGER_SOAPSTARTLEVEL_INTERNALSTATE_EXITING:
    default:
      break;
  }
}

/*=============================================================================================*/
static void MenuManager_SoapStartLevel_SubTickHandler(void)
{
  if (MenuManager_SoapStartLevel_InternalState == MENUMANAGER_SOAPSTARTLEVEL_INTERNALSTATE_EXITING)
  {
    MenuManager_SoapStartLevel_Counter += (uint32_t)1U;

    if (MenuManager_SoapStartLevel_Counter >= MENUMANAGER_SOAPSTARTLEVEL_COUNTER_MAX)
    {
      MenuManager_SoapStartLevel_Counter = (uint32_t)0U;
      
      Fsm_TriggerEvent(&MenuManager_FsmContext, (Fsm_EventType)MENUMANAGER_EVENT_PREV);
    }
  }
}



#ifdef  __cplusplus
}
#endif
