#ifndef FSM_H
#define FSM_H

#include "main.h"

#include "fsm_cfg.h"



/**********************************************************************************************************************
 *  DEFINES
 *********************************************************************************************************************/

/** Default (invalid) state value */
#define FSM_DEFAULT_STATE                                      ((tFsmState)-1)

/** Helper macro to get number of array entries */
#define FSM_ARRAY_SIZE(arr)                      	            (sizeof(arr) / sizeof((arr)[0u]))

/** Helper macro for filling state structures (enhance readability) */
#define FSM_SIZE_AND_ARRAY(array)   				               FSM_ARRAY_SIZE(array), (array)

/**********************************************************************************************************************
 *  MACROS
 *********************************************************************************************************************/

/** Dummy entry */
#define FSM_TRIGGER_NONE()                                    { Fsm_EventHandlerDefault,    kFsmDefaultEvent_None,  FSM_DEFAULT_STATE     }
/** State entry handler */
#define FSM_TRIGGER_ENTRY(action)                             { (action),                  kFsmDefaultEvent_Entry, FSM_DEFAULT_STATE     }
/** State exit handler */
#define FSM_TRIGGER_EXIT(action)                              { (action),                  kFsmDefaultEvent_Exit,  FSM_DEFAULT_STATE     }
/** Event handler for "any" event */
#define FSM_TRIGGER_ANY(action, state)                        { (action),                  kFsmDefaultEvent_Any,   (state)               }
/** Internal event handler, no state transition */
#define FSM_TRIGGER_INTERNAL(event, action)                   { (action),                  (event),                FSM_DEFAULT_STATE     }
/** State transition on event */
#define FSM_TRIGGER_TRANSITION(event, state)                  { Fsm_EventHandlerDefault,    (event),                (state)               }
/** State transition on event with additional action */
#define FSM_TRIGGER_TRANSITION_ACTION(event, action, state)   { (action),                  (event),                (state)               }
/** Switch state when guard condition is true */
#define FSM_TRIGGER_CHOICE(guard, state)                      { (guard),                   kFsmDefaultEvent_Entry, (state)               }
/** Else case of choice */
#define FSM_TRIGGER_CHOICE_ELSE(state)                        { Fsm_EventHandlerDefault,    kFsmDefaultEvent_Entry, (state)               }
/** Unconditional state transition (any entry handler has to return negative guard result) */
#define FSM_TRIGGER_UNCONDITIONAL(state)                      { Fsm_EventHandlerDefault,    kFsmDefaultEvent_Entry, (state)               }
/** Unconditional state transition with additional action (any entry handler has to return negative guard result) */
#define FSM_TRIGGER_UNCONDITIONAL_ACTION(action, state)       { (action),                  kFsmDefaultEvent_Entry, (state)               }

/**********************************************************************************************************************
 *  GLOBAL DATA TYPES AND STRUCTURES
 *********************************************************************************************************************/

/** State type for state machine (the real states need to be defined by the component(s) using this module) */
typedef uint16_t tFsmState;

/** Event type for state machine (the real events need to be defined by the component(s) using this module) */
typedef uint16_t tFsmEvent;

#if defined( FSM_ENABLE_DEBUGGING )
/** Instance number */
typedef vuint8 tFsmInstance;
#endif /* FSM_ENABLE_DEBUGGING */

/** Event type (components using this module shall start to enumerate their events with kFsmDefaultEvent_Count) */
typedef enum
{
   kFsmDefaultEvent_None,              /**< No event */
   kFsmDefaultEvent_Entry,             /**< State entry event */
   kFsmDefaultEvent_Exit,              /**< State exit event */
   kFsmDefaultEvent_Any,               /**< Don't care event */

   kFsmDefaultEvent_Count              /**< Decimal number of default events */
} tFsmDefaultEvents;

/** Guard type */
typedef enum
{
   kFsmGuard_True,                     /**< True: State transition is allowed */
   kFsmGuard_False,                    /**< False: State transition is prohibited */
   kFsmGuard_Overwrite                 /**< Allow pending state to set next state (override state table configuration), defaults to no transition */
} tFsmGuard;

/** Forward declaration of context structure */
typedef struct sFsmContext tFsmContext;

/** Pointer to context structure */
typedef tFsmContext* tFsmContextPtr;

/** Event handler function pointer */
typedef tFsmGuard (* tFsmEventHandler)(tFsmContextPtr const pFsmContext, tFsmEvent event);

/** Data hierachy between states */
typedef struct
{
   uint32_t             dataId;        /**< Data hierachy ID type */
} tFsmDataHierachy;

typedef struct
{
   tFsmEventHandler     handler;       /**< Event handler */
   tFsmEvent            trigger;       /**< Event that triggers execution of handler */
   tFsmState            nextState;     /**< Next state if event handler returns kFsmGuard_True */
} tFsmEventEntry;

/** State definition */
typedef struct
{
   tFsmState            superState;    /**< Super state (encapsulating state) */
   uint16_t             triggerCount;  /**< Size of array structure element 'triggers' points to */
   /** Array containing triggers (events) and the corresponding actions for state */
   tFsmEventEntry*      triggers;
} tFsmStateDefinition;

/** Context structure */
struct sFsmContext
{
   /** Pointer to array containing state definitions (set by component(s) using this module) */
   tFsmStateDefinition* stateDefinitions;
   uint16_t             stateCount;    /**< Number of elements in stateDefinitions (set by component(s) using this module) */
   tFsmState            state;         /**< Current state (intended for internal usage) */
   tFsmState            pendingState;  /**< Pending state (intended for internal usage) */
   tFsmEvent            pendingEvent;  /**< Pending event (intended for internal usage) */
   tFsmDataHierachy*    dataHierachy;  /**< Data hierachy (intended for internal usage) */
#if defined( FSM_ENABLE_DEBUGGING )
   tFsmInstance         instance;      /**< State machine instance number */
#endif
};

/**********************************************************************************************************************
 *  GLOBAL FUNCTION PROTOTYPES
 *********************************************************************************************************************/

#if defined( __cplusplus )
extern "C" {
#endif

void Fsm_InitPowerOn( void );
void Fsm_Init( tFsmContextPtr const pFsmContext, tFsmState initialState );
void Fsm_DeInit( tFsmContextPtr const pFsmContext );
void Fsm_StateTask( tFsmContextPtr const pFsmContext );
void Fsm_TriggerEvent( tFsmContextPtr const pFsmContext, tFsmEvent event );
tFsmGuard Fsm_EventHandlerDefault( tFsmContextPtr const pFsmContext, tFsmEvent event );

#if defined( __cplusplus )
} /* extern "C" */
#endif

#endif /* _FSM_H */
