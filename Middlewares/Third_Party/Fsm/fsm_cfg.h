/* 
 * File:   Fsm_Cfg.h
 * Author: Long
 *
 * Created on September 15, 2019, 11:06 AM
 */

#ifndef FSM_CFG_H
#define FSM_CFG_H

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



/*===============================================================================================
*                                     DEFINES AND MACROS
===============================================================================================*/

/** Maximum hierarchy depth (state nesting) */
#define FSM_MAX_HIERARCHY_DEPTH        8u

/** Debugging switch for FSM */
// #define FSM_DISABLE_DEBUGGING

#if defined( FSM_ENABLE_DEBUGGING )
#include <stdio.h>
#endif

#if defined( FSM_ENABLE_DEBUGGING )
  # define FSM_DEBUG_PRINT1(instance, currentState, pendingState)     (void)printf("Fsm%d: St %d -> %d\n", instance, currentState, pendingState)
  # define FSM_DEBUG_PRINT2(instance, event, state)                   (void)printf("Fsm%d: Ev %d/St %d\n", instance, event, state)
  # define FSM_DEBUG_PRINT3(instance, guard)                          (void)printf("Fsm%d: Gu %d\n", instance, guard)
  # define FSM_DEBUG_ERROR_PRINT(instance)                            (void)printf("Fsm%d: ERROR at %s:%d\n", instance, __FUNCTION__, __LINE__)
#else
  # define FSM_DEBUG_PRINT1(instance, currentState, pendingState)
  # define FSM_DEBUG_PRINT2(instance, event, state)
  # define FSM_DEBUG_PRINT3(instance, guard)
  # define FSM_DEBUG_ERROR_PRINT(instance)
#endif



#ifdef  __cplusplus
}
#endif

#endif /* FSM_CFG_H */
