/* 
 * File:   _1310000000_menu_ProgramStepSetup.c
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
#define MENUMANAGER_PROGRAMSTEPSETUP_INTERNALDATALENGTH               (uint8_t)3U

#define MENUMANAGER_PROGRAMSTEPSETUP_LISTINDEX_IDX                    0U
#define MENUMANAGER_PROGRAMSTEPSETUP_CURPOS_IDX                       1U
#define MENUMANAGER_PROGRAMSTEPSETUP_SEQIDX_IDX                       2U

#define MenuManager_ProgramStepSetup_ListIndex                        MenuManager_GetInternalDataPtr(MENUMANAGER_PROGRAMSTEPSETUP_LISTINDEX_IDX)
#define MenuManager_ProgramStepSetup_CurPos                           MenuManager_GetInternalDataPtr(MENUMANAGER_PROGRAMSTEPSETUP_CURPOS_IDX)
#define MenuManager_ProgramStepSetup_SeqIdx                           MenuManager_GetInternalDataPtr(MENUMANAGER_PROGRAMSTEPSETUP_SEQIDX_IDX)

#define MENUMANAGER_PROGRAMSTEPSETUP_LCD_ELEMENT_MENU_LENGTH          (16U)
#define MENUMANAGER_PROGRAMSTEPSETUP_LCD_STEP_ACTIVE_STAT_XPOS        (18U)



/** Menu manager button event mapping array */
static MenuManager_ButEventMapStruct MenuManager_ProgramStepSetup_ButEventMap[4] =
{
  { IOMANAGER_BUTTON_STATE_START,                                     MENUMANAGER_EVENT_START_BUT             },
  { IOMANAGER_BUTTON_STATE_STOP,                                      MENUMANAGER_EVENT_STOP_BUT              },
  { IOMANAGER_BUTTON_STATE_UP,                                        MENUMANAGER_EVENT_UP_BUT                },
  { IOMANAGER_BUTTON_STATE_DOWN,                                      MENUMANAGER_EVENT_DOWN_BUT              }
};

/** Menu manager button event mapping configuration */
static MenuManager_ButEventMapConfStruct MenuManager_ProgramStepSetup_ButEventMapConf =
{
  .butEventMapNum             = (uint8_t)4U,
  .butEventMapCfg             = &MenuManager_ProgramStepSetup_ButEventMap
};



/** Menu manager event handlers */
static Fsm_GuardType MenuManager_ProgramStepSetup_Entry               (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_ProgramStepSetup_StartBut            (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_ProgramStepSetup_StopBut             (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_ProgramStepSetup_UpBut               (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_ProgramStepSetup_DownBut             (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);

/** Menu manager state machine */
Fsm_EventEntryStruct MenuManager_ProgramStepSetup_StateMachine[6] =
{
  FSM_TRIGGER_ENTRY             (                                     MenuManager_ProgramStepSetup_Entry                                              ),
  FSM_TRIGGER_TRANSITION        ( MENUMANAGER_EVENT_SUBMENU_1,                                                MENUMANAGER_STATE_STEP_NORM_SETUP       ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_UP_BUT,           MenuManager_ProgramStepSetup_UpBut                                              ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_DOWN_BUT,         MenuManager_ProgramStepSetup_DownBut                                            ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_START_BUT,        MenuManager_ProgramStepSetup_StartBut                                           ),
  FSM_TRIGGER_TRANSITION_ACTION ( MENUMANAGER_EVENT_STOP_BUT,         MenuManager_ProgramStepSetup_StopBut,   MENUMANAGER_STATE_PROGRAM_SETUP         )
};



/*===============================================================================================
*                                   LOCAL FUNCTION PROTOTYPES
===============================================================================================*/

static void MenuManager_ProgramStepSetup_LcdShowMainTitle(void);
static void MenuManager_ProgramStepSetup_LcdShowList(void);

static void MenuManager_ProgramStepSetup_SubMainFunction(void);
static void MenuManager_ProgramStepSetup_SubTickHandler(void);



/*===============================================================================================
*                                       LOCAL FUNCTIONS
===============================================================================================*/

static void MenuManager_ProgramStepSetup_LcdShowMainTitle(void)
{
  uint8_t tempStr[20];

  /* Print main title */
  MenuManager_Common_LcdClearElementMenuStatic();

  sprintf((char *)tempStr, (const char *)MenuManager_Common_ProgramStr, MenuManager_ProgramStepSetup_SeqIdx + 1U);

  MenuManager_Common_LcdShowMainTitle(tempStr);
  MenuManager_Common_LcdUpdateElementMenuStatic();
}

/*=============================================================================================*/
static void MenuManager_ProgramStepSetup_LcdShowList(void)
{
  uint32_t currentListIndex;
  uint32_t currentCursorPos;
  uint32_t listLength;
  uint8_t  tempStr[20];
  bool     tempIsActive;
  
  currentListIndex = MenuManager_Common_LcdGetStartListIndex(MenuManager_ProgramStepSetup_ListIndex, MenuManager_ProgramStepSetup_CurPos);
  currentCursorPos = (uint32_t)MENUMANAGER_COMMON_LCD_CURSOR_MIN;

  listLength = MenuManager_Common_LcdGetListLength((uint32_t)PROGRAMMANAGER_STEP_NUM_MAX);
  
  MenuManager_Common_LcdClearElementMenuDynamic();
  
  for (; currentCursorPos < (listLength + (uint32_t)MENUMANAGER_COMMON_LCD_CURSOR_MIN); currentCursorPos++, currentListIndex++)
  {
    sprintf((char *)tempStr, (const char *)MenuManager_Common_StepStr, currentListIndex + 1U);

    MenuManager_Common_LcdShowListElementName
    (
      tempStr,
      MENUMANAGER_PROGRAMSTEPSETUP_LCD_ELEMENT_MENU_LENGTH,
      MENUMANAGER_COMMON_LCD_ELEMENT_MENU_XPOS,
      currentCursorPos
    );

    ProgramManager_NormStepConfig_IsActive_GetData((uint8_t)MenuManager_ProgramStepSetup_SeqIdx, (uint8_t)currentListIndex, &tempIsActive);

    if (tempIsActive != false)
    {
      LCD_SetCursorPos(MENUMANAGER_PROGRAMSTEPSETUP_LCD_STEP_ACTIVE_STAT_XPOS, currentCursorPos, LCD_CURSOR_BY_FONT);
      LCD_PutChar('+');
    }
  }
  
  MenuManager_Common_LcdShowChosenMark(MenuManager_ProgramStepSetup_CurPos);
  
  MenuManager_Common_LcdShowScrollBar(MenuManager_ProgramStepSetup_ListIndex, MenuManager_ProgramStepSetup_CurPos, (uint8_t)PROGRAMMANAGER_STEP_NUM_MAX);
  
  MenuManager_Common_LcdUpdateElementMenuDynamic();
}



/*=============================================================================================*/
static Fsm_GuardType MenuManager_ProgramStepSetup_Entry(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  MenuManager_Common_ProgramSetupStruct* enterDataHierachy;
  HAL_StatusTypeDef retVal = HAL_OK;

  /* Check if previous state data hierachy is not empty */
  if (pFsmContext->dataHierachy != NULL)
  {
    if (pFsmContext->dataHierachy->dataId == MENUMANAGER_STATE_STEP_NORM_SETUP)
    {
      /* Release previous state data hierachy */
      MenuManager_free(pFsmContext->dataHierachy);
      pFsmContext->dataHierachy = NULL;
    }
    else if (pFsmContext->dataHierachy->dataId == MENUMANAGER_STATE_PROGRAM_SETUP)
    {
      enterDataHierachy = (MenuManager_Common_ProgramSetupStruct *)(pFsmContext->dataHierachy);

      MenuManager_InternalDataPush(MENUMANAGER_PROGRAMSTEPSETUP_INTERNALDATALENGTH);

      MenuManager_ProgramStepSetup_ListIndex = (uint32_t)0U;
      MenuManager_ProgramStepSetup_CurPos = MENUMANAGER_COMMON_LCD_CURSOR_MIN;
      MenuManager_ProgramStepSetup_SeqIdx = enterDataHierachy->seqIdx;

      /* Release previous state data hierachy */
      MenuManager_free(pFsmContext->dataHierachy);
      pFsmContext->dataHierachy = NULL;
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
    MenuManager_ProgramStepSetup_LcdShowMainTitle();
    MenuManager_ProgramStepSetup_LcdShowList();

    MenuManager_SubMainFunction = MenuManager_ProgramStepSetup_SubMainFunction;
    MenuManager_SubTickHandler = MenuManager_ProgramStepSetup_SubTickHandler;

    return FSM_GUARD_TRUE;
  }

  return FSM_GUARD_FALSE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_ProgramStepSetup_StartBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  MenuManager_Common_ProgramSetupStruct* dataHierachy;

  MenuManager_SubMainFunction = NULL;
  MenuManager_SubTickHandler = NULL;

  dataHierachy = (MenuManager_Common_ProgramSetupStruct *)MenuManager_malloc(sizeof(MenuManager_Common_ProgramSetupStruct));
  dataHierachy->dataId = MENUMANAGER_STATE_PROGRAM_STEP_SETUP;
  dataHierachy->seqIdx = MenuManager_ProgramStepSetup_SeqIdx;
  dataHierachy->stepIdx = MenuManager_ProgramStepSetup_ListIndex;

  pFsmContext->dataHierachy = (Fsm_DataHierachyStruct *)dataHierachy;

  Fsm_TriggerEvent(&MenuManager_FsmContext, (Fsm_EventType)MENUMANAGER_EVENT_SUBMENU_1);
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_ProgramStepSetup_StopBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  Fsm_DataHierachyStruct* dataHierachy;

  MenuManager_SubMainFunction = NULL;
  MenuManager_SubTickHandler = NULL;

  dataHierachy = (Fsm_DataHierachyStruct *)MenuManager_malloc(sizeof(Fsm_DataHierachyStruct));
  dataHierachy->dataId = MENUMANAGER_STATE_PROGRAM_STEP_SETUP;

  pFsmContext->dataHierachy = dataHierachy;

  /* Free internal data */
  MenuManager_InternalDataPop();
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_ProgramStepSetup_UpBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_ProgramStepSetup_ListIndex > (uint32_t)0U)
  {
    MenuManager_ProgramStepSetup_ListIndex -= (uint32_t)1U;
  }

  if (MenuManager_ProgramStepSetup_CurPos > MENUMANAGER_COMMON_LCD_CURSOR_MIN)
  {
    MenuManager_ProgramStepSetup_CurPos -= (uint32_t)1U;
  }

  MenuManager_ProgramStepSetup_LcdShowList();
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_ProgramStepSetup_DownBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_ProgramStepSetup_ListIndex < ((uint32_t)(PROGRAMMANAGER_STEP_NUM_MAX) - (uint32_t)1U))
  {
    MenuManager_ProgramStepSetup_ListIndex += (uint32_t)1U;
  }
  
  if (MenuManager_ProgramStepSetup_CurPos < MenuManager_Common_LcdMaxCurPos((uint32_t)(PROGRAMMANAGER_STEP_NUM_MAX)))
  {
    MenuManager_ProgramStepSetup_CurPos += (uint32_t)1U;
  }
  
  MenuManager_ProgramStepSetup_LcdShowList();
  
  return FSM_GUARD_TRUE;
}



/*=============================================================================================*/
static void MenuManager_ProgramStepSetup_SubMainFunction(void)
{
  MenuManager_Common_ButEventMapHandler(&MenuManager_ProgramStepSetup_ButEventMapConf);
}

/*=============================================================================================*/
static void MenuManager_ProgramStepSetup_SubTickHandler(void)
{
  
}



#ifdef  __cplusplus
}
#endif
