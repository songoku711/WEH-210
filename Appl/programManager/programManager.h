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
#include "ProgramManager_Common.h"



/*===============================================================================================
*                                     DEFINES AND MACROS
===============================================================================================*/

#define ProgramManager_malloc(size)                     pvPortMalloc(size)
#define ProgramManager_free(ptr)                        vPortFree(ptr)



/** Program manager application states */
typedef enum
{
  PROGRAMMANAGER_APPL_STATE_POWER_ON,
  PROGRAMMANAGER_APPL_STATE_INIT,
  PROGRAMMANAGER_APPL_STATE_IDLE,
  PROGRAMMANAGER_APPL_STATE_AUTO,
  PROGRAMMANAGER_APPL_STATE_MANUAL,
  PROGRAMMANAGER_APPL_STATE_FAIL
} ProgramManager_ApplState;

/** Program manager application events */
typedef enum
{
  PROGRAMMANAGER_APPL_EVENT_PREV = FSM_DEFAULTEVENT_NUM,
  PROGRAMMANAGER_APPL_EVENT_NEXT,
  PROGRAMMANAGER_APPL_EVENT_NEXT_2
} ProgramManager_ApplEvent;



extern const ProgramManager_ParamConfigSetupStruct ProgramManager_gParamDefConfig;
extern const ProgramManager_AutoSeqConfigStruct ProgramManager_gAutoSeqDefConfig;
extern const ProgramManager_ManualSeqConfigStruct ProgramManager_gManualSeqDefConfig;

extern ProgramManager_ParamConfigSetupStruct ProgramManager_gParamConfig;
extern ProgramManager_AutoSeqConfigStruct ProgramManager_gAutoSeqConfig;
extern ProgramManager_ManualSeqConfigStruct ProgramManager_gManualSeqConfig;



/** Context Structure of the Finite State Machine (FSM) */
extern Fsm_ContextStruct ProgramManager_FsmContext;

extern void (*ProgramManager_SubMainFunction)(void);
extern void (*ProgramManager_SubTickHandler)(void);



/*===============================================================================================
*                                     FUNCTION PROTOTYPES
===============================================================================================*/

void ProgramManager_Init(void);
void ProgramManager_MainFunction(void);



#ifdef  __cplusplus
}
#endif

#endif  /* PROGRAMMANAGER_H */
