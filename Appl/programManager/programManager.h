/* 
 * File:   programManager.h
 * Author: Long
 *
 * Created on September 15, 2019, 11:06 AM
 */

#ifndef PROGRAMMANAGER_H
#define PROGRAMMANAGER_H

#ifdef  __cplusplus
extern "C" {
#endif



/*===============================================================================================
*                                         INCLUDE FILES
* 1) system and project includes
* 2) needed interfaces from external units
* 3) internal and external interfaces from this unit
===============================================================================================*/

#include "main.h"

#include "fsm.h"
#include "memIf.h"
#include "cmsis_os.h"

#include "ProgramManager_Types.h"
#include "ProgramManager_Mem.h"
#include "ProgramManager_Control.h"
#include "ProgramManager_Common.h"



/*===============================================================================================
*                                     DEFINES AND MACROS
===============================================================================================*/

#define PROGRAMMANAGER_SUBMENU_TREE_DEPTH                             (10U)

#define PROGRAMMANAGER_SUBMAINFUNCTION_NUM_MAX                        (10U)

#define ProgramManager_malloc(size)                                   pvPortMalloc(size)
#define ProgramManager_free(ptr)                                      vPortFree(ptr)

#define ProgramManager_GetInternalDataPtr(index)                      *(ProgramManager_InternalData.internalDataArr[ProgramManager_InternalData.internalDataCurIdx].ptr + index)



/** Program manager application states */
typedef enum
{
  PROGRAMMANAGER_STATE_POWER_ON,
  PROGRAMMANAGER_STATE_INIT,
  PROGRAMMANAGER_STATE_IDLE,
  PROGRAMMANAGER_STATE_AUTO_PRE_RUN,
  PROGRAMMANAGER_STATE_AUTO_RUN_WATER_HEAT,
  PROGRAMMANAGER_STATE_AUTO_RUN_WASH,
  PROGRAMMANAGER_STATE_AUTO_RUN_DRAIN,
  PROGRAMMANAGER_STATE_AUTO_RUN_EXTRACT,
  PROGRAMMANAGER_STATE_AUTO_POST_RUN,
  PROGRAMMANAGER_STATE_AUTO_RUN_BALANCE,
  PROGRAMMANAGER_STATE_FAIL
} ProgramManager_StateType;

/** Program manager application events */
typedef enum
{
  PROGRAMMANAGER_EVENT_PREV = FSM_DEFAULTEVENT_NUM,
  PROGRAMMANAGER_EVENT_NEXT,
  PROGRAMMANAGER_EVENT_SUBMENU_1,
  PROGRAMMANAGER_EVENT_SUBMENU_2,
  PROGRAMMANAGER_EVENT_SUBMENU_3,
  PROGRAMMANAGER_EVENT_SUBMENU_4,
  PROGRAMMANAGER_EVENT_SUBMENU_5,
  PROGRAMMANAGER_EVENT_SUBMENU_6,
  PROGRAMMANAGER_EVENT_SUBMENU_7,
} ProgramManager_EventType;



/** Program manager sub-main function array */
typedef struct
{
  uint8_t                     subMainFunctionNum;
  void                        (*subMainFunction[PROGRAMMANAGER_SUBMAINFUNCTION_NUM_MAX])(void);
} ProgramManager_SubMainFunctionStruct;

/** Program manager internal data structure */
typedef struct
{
  uint8_t length;
  uint32_t *ptr;
} ProgramManager_InternalDataElementStruct;

/** Program manager internal data stack structure */
typedef struct
{
  uint8_t internalDataCurIdx;
  ProgramManager_InternalDataElementStruct internalDataArr[PROGRAMMANAGER_SUBMENU_TREE_DEPTH];
} ProgramManager_InternalDataStruct;



/*===============================================================================================
*                                         VARIABLES
===============================================================================================*/

extern const ProgramManager_ParamConfigSetupStruct ProgramManager_gParamDefConfig;
extern const ProgramManager_AutoSeqConfigStruct ProgramManager_gAutoSeqDefConfig;
extern const ProgramManager_ManualSeqConfigStruct ProgramManager_gManualSeqDefConfig;

extern ProgramManager_ParamConfigSetupStruct ProgramManager_gParamConfig;
extern ProgramManager_AutoSeqConfigStruct ProgramManager_gAutoSeqConfig;
extern ProgramManager_ManualSeqConfigStruct ProgramManager_gManualSeqConfig;



/** Program internal data structure */
extern ProgramManager_InternalDataStruct ProgramManager_InternalData;

/** Context Structure of the Finite State Machine (FSM) */
extern Fsm_ContextStruct ProgramManager_FsmContext;

extern ProgramManager_SubMainFunctionStruct ProgramManager_gSubMainFunctionConfig;
extern void (*ProgramManager_SubTickHandler)(void);



/*===============================================================================================
*                                     FUNCTION PROTOTYPES
===============================================================================================*/

void ProgramManager_Init(void);
void ProgramManager_MainFunction(void);

void ProgramManager_SubMainFunctionPush(void (*subMainFunction)(void));
void ProgramManager_SubMainFunctionPop(void);

ProgramManager_StateType ProgramManager_GetCurrentState(void);

void ProgramManager_InternalDataPush(uint8_t length);
void ProgramManager_InternalDataPop(void);


#ifdef  __cplusplus
}
#endif

#endif  /* PROGRAMMANAGER_H */
