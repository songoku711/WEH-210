/* 
 * File:   menuManager.c
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
*                                       GLOBAL VARIABLES
===============================================================================================*/

/** Application state machine */
/** State powerOn (0) */
extern tFsmEventEntry MenuManager_PowerOn_StateMachine[2];
/** State init (1) */
extern tFsmEventEntry MenuManager_Init_StateMachine[3];
/** State home (2) */
extern tFsmEventEntry MenuManager_Home_StateMachine[10];
/** State mainSetting (2) */
extern tFsmEventEntry MenuManager_MainSetting_StateMachine[7];



/** Menu internal data structure */
MenuManager_InternalDataStruct MenuManager_InternalData;

/** Context Structure of the Finite State Machine (FSM) */
tFsmContext MenuManager_FsmContext;

/** List of all states */
tFsmStateDefinition MenuManager_StateMachine[4] =
{
  { FSM_DEFAULT_STATE, FSM_SIZE_AND_ARRAY(MenuManager_PowerOn_StateMachine) },
  { FSM_DEFAULT_STATE, FSM_SIZE_AND_ARRAY(MenuManager_Init_StateMachine) },
  { FSM_DEFAULT_STATE, FSM_SIZE_AND_ARRAY(MenuManager_Home_StateMachine) },
  { FSM_DEFAULT_STATE, FSM_SIZE_AND_ARRAY(MenuManager_MainSetting_StateMachine) }
};

void (*MenuManager_SubMainFunction)(void);
void (*MenuManager_SubTickHandler)(void);



/*===============================================================================================
*                                       GLOBAL FUNCTIONS
===============================================================================================*/

void MenuManager_Init(void)
{
  MenuManager_SubMainFunction = NULL;
  MenuManager_SubTickHandler = NULL;

  MenuManager_InternalData.internalDataCurIdx = (uint8_t)0U;
  
  /* Start State Machine */
	MenuManager_FsmContext.stateDefinitions = MenuManager_StateMachine;
	MenuManager_FsmContext.stateCount = FSM_ARRAY_SIZE(MenuManager_StateMachine);
  
  /* Execute POWER ON state */
	Fsm_Init(&MenuManager_FsmContext, (tFsmState)MENUMANAGER_STATE_POWER_ON);

  /* Switch to INIT state */
  Fsm_TriggerEvent(&MenuManager_FsmContext, (tFsmEvent)MENUMANAGER_EVENT_NEXT);
  Fsm_StateTask(&MenuManager_FsmContext);
}

/*=============================================================================================*/
void MenuManager_MainFunction(void)
{
  Fsm_StateTask(&MenuManager_FsmContext);
  
  if (MenuManager_SubMainFunction != NULL)
  {
    MenuManager_SubMainFunction();
  }
}



/*=============================================================================================*/
void MenuManager_InternalDataPush(uint8_t length)
{
  if (MenuManager_InternalData.internalDataCurIdx <= MENUMANAGER_SUBMENU_TREE_DEPTH)
  {
    MenuManager_InternalData.internalDataCurIdx++;

    MenuManager_InternalData.internalDataArr[MenuManager_InternalData.internalDataCurIdx].length = length;
    MenuManager_InternalData.internalDataArr[MenuManager_InternalData.internalDataCurIdx].ptr = (uint32_t *)MenuManager_malloc(length);
  }
}

/*=============================================================================================*/
void MenuManager_InternalDataPop(void)
{
  if (MenuManager_InternalData.internalDataCurIdx > (uint8_t)0U)
  {
    MenuManager_InternalData.internalDataArr[MenuManager_InternalData.internalDataCurIdx].length = (uint8_t)0U;
    MenuManager_free(MenuManager_InternalData.internalDataArr[MenuManager_InternalData.internalDataCurIdx].ptr);

    MenuManager_InternalData.internalDataCurIdx--;
  }
}



#ifdef  __cplusplus
}
#endif
