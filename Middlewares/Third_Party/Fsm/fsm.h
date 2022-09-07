/* 
 * File:   Fsm.h
 * Author: Long
 *
 * Created on September 15, 2019, 11:06 AM
 */

#ifndef FSM_H
#define FSM_H

#ifdef  __cplusplus
extern "C" {
#endif



/*===============================================================================================
*                                         INCLUDE FILES
* 1) system and project includes
* 2) needed interfaces from external units
* 3) internal and external interfaces from this unit
===============================================================================================*/

#include "fsm_cfg.h"



/*===============================================================================================
*                                     DEFINES AND MACROS
===============================================================================================*/

/** Default (invalid) state value */
#define FSM_DEFAULT_STATE                                             ((Fsm_StateType)-1)

/** Helper macro to get number of array entries */
#define FSM_ARRAY_SIZE(arr)                      	                    (sizeof(arr) / sizeof((arr)[0u]))

/** Helper macro for filling state structures (enhance readability) */
#define FSM_SIZE_AND_ARRAY(array)   				                          FSM_ARRAY_SIZE(array), (array)



/** Dummy entry */
#define FSM_TRIGGER_NONE()                                            { Fsm_EventHandlerDefault,    FSM_DEFAULTEVENT_NONE,        FSM_DEFAULT_STATE   }
/** State entry handler */
#define FSM_TRIGGER_ENTRY(action)                                     { (action),                   FSM_DEFAULTEVENT_ENTRY,       FSM_DEFAULT_STATE   }
/** State exit handler */
#define FSM_TRIGGER_EXIT(action)                                      { (action),                   FSM_DEFAULTEVENT_EXIT,        FSM_DEFAULT_STATE   }
/** Event handler for "any" event */
#define FSM_TRIGGER_ANY(action, state)                                { (action),                   FSM_DEFAULTEVENT_ANY,         (state)             }
/** Internal event handler, no state transition */
#define FSM_TRIGGER_INTERNAL(event, action)                           { (action),                   (event),                      FSM_DEFAULT_STATE   }
/** State transition on event */
#define FSM_TRIGGER_TRANSITION(event, state)                          { Fsm_EventHandlerDefault,    (event),                      (state)             }
/** State transition on event with additional action */
#define FSM_TRIGGER_TRANSITION_ACTION(event, action, state)           { (action),                   (event),                      (state)             }
/** Switch state when guard condition is true */
#define FSM_TRIGGER_CHOICE(guard, state)                              { (guard),                    FSM_DEFAULTEVENT_ENTRY,       (state)             }
/** Else case of choice */
#define FSM_TRIGGER_CHOICE_ELSE(state)                                { Fsm_EventHandlerDefault,    FSM_DEFAULTEVENT_ENTRY,       (state)             }
/** Unconditional state transition (any entry handler has to return negative guard result) */
#define FSM_TRIGGER_UNCONDITIONAL(state)                              { Fsm_EventHandlerDefault,    FSM_DEFAULTEVENT_ENTRY,       (state)             }
/** Unconditional state transition with additional action (any entry handler has to return negative guard result) */
#define FSM_TRIGGER_UNCONDITIONAL_ACTION(action, state)               { (action),                   FSM_DEFAULTEVENT_ENTRY,       (state)             }



/** State type for state machine (the real states need to be defined by the component(s) using this module) */
typedef uint16_t Fsm_StateType;

/** Event type for state machine (the real events need to be defined by the component(s) using this module) */
typedef uint16_t Fsm_EventType;

#if defined( FSM_ENABLE_DEBUGGING )
/** Instance number */
typedef uint8_t Fsm_InstanceType;
#endif

/** Event type (components using this module shall start to enumerate their events with FSM_DEFAULTEVENT_NUM) */
typedef enum
{
  FSM_DEFAULTEVENT_NONE,                          /* No event */
  FSM_DEFAULTEVENT_ENTRY,                         /* State entry event */
  FSM_DEFAULTEVENT_EXIT,                          /* State exit event */
  FSM_DEFAULTEVENT_ANY,                           /* Don't care event */

  FSM_DEFAULTEVENT_NUM                            /* Number of default events */
} Fsm_DefaultEventType;

/** Guard type */
typedef enum
{
  FSM_GUARD_TRUE,                                 /* True: State transition is allowed */
  FSM_GUARD_FALSE,                                /* False: State transition is prohibited */
  FSM_GUARD_OVERWRITE                             /* Allow pending state to set next state (override state table configuration), defaults to no transition */
} Fsm_GuardType;

/** Forward declaration of context structure */
typedef struct Fsm_ContextStructType Fsm_ContextStruct;

/** Pointer to context structure */
typedef Fsm_ContextStruct* Fsm_ContextStructPtr;

/** Event handler function pointer */
typedef Fsm_GuardType (* Fsm_EventHandlerType)(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);

/** Data hierachy between states */
typedef struct
{
  uint32_t                    dataId;             /* Data hierachy ID type */
} Fsm_DataHierachyStruct;

typedef struct
{
  Fsm_EventHandlerType        handler;            /* Event handler */
  Fsm_EventType               trigger;            /* Event that triggers execution of handler */
  Fsm_StateType               nextState;          /* Next state if event handler returns FSM_GUARD_TRUE */
} Fsm_EventEntryStruct;

/** State definition */
typedef struct
{
  Fsm_StateType               superState;         /* Super state (encapsulating state) */
  uint16_t                    triggerCount;       /* Size of array structure element 'triggers' points to */
  Fsm_EventEntryStruct*       triggers;           /* Array containing triggers (events) and the corresponding actions for state */
} Fsm_StateDefStruct;

/** Context structure */
struct Fsm_ContextStructType
{
  Fsm_StateDefStruct*         stateDefinitions;   /* Pointer to array containing state definitions (set by component(s) using this module) */
  uint16_t                    stateCount;         /* Number of elements in stateDefinitions (set by component(s) using this module) */
  Fsm_StateType               state;              /* Current state (intended for internal usage) */
  Fsm_StateType               pendingState;       /* Pending state (intended for internal usage) */
  Fsm_EventType               pendingEvent;       /* Pending event (intended for internal usage) */
  Fsm_DataHierachyStruct*     dataHierachy;       /* Data hierachy (intended for internal usage) */
#if defined( FSM_ENABLE_DEBUGGING )
  Fsm_InstanceType            instance;           /* State machine instance number */
#endif
};

/**********************************************************************************************************************
 *  GLOBAL FUNCTION PROTOTYPES
 *********************************************************************************************************************/

void Fsm_InitPowerOn( void );
void Fsm_Init( Fsm_ContextStructPtr const pFsmContext, Fsm_StateType initialState );
void Fsm_DeInit( Fsm_ContextStructPtr const pFsmContext );
void Fsm_StateTask( Fsm_ContextStructPtr const pFsmContext );
void Fsm_TriggerEvent( Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event );
Fsm_GuardType Fsm_EventHandlerDefault( Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event );



#ifdef  __cplusplus
}
#endif

#endif /* FSM_H */
