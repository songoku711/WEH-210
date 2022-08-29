/* 
 * File:   fsm.c
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

#include "fsm.h"



/*===============================================================================================
*                                       DEFINES AND MACROS
===============================================================================================*/

/** Type to select whether super-states should be iterated in case no matching event handler was found */
typedef enum
{
  kFsmIterateHierarchy_Disabled,   /**< Do not iterate super-states */
  kFsmIterateHierarchy_Enabled     /**< Iterate super-states until matching event handler was found */
} tFsmIterateHierarchy;



/*===============================================================================================
*                                       GLOBAL VARIABLES
===============================================================================================*/

#if defined( FSM_ENABLE_DEBUGGING )
/** Global state machine instance counter */
static tFsmInstance g_Instance;
#endif



/*===============================================================================================
*                                   LOCAL FUNCTION PROTOTYPES
===============================================================================================*/

static void Fsm_HandleEvent(tFsmContextPtr const pFsmContext, tFsmEvent event, tFsmIterateHierarchy iterate);
static uint8_t Fsm_GetStateHierachy(const tFsmContext* const pFsmContext, tFsmState state, tFsmState* hierarchy);



/*===============================================================================================
*                                       LOCAL FUNCTIONS
===============================================================================================*/

/************************************************************************************************
 *  Fsm_GetStateHierachy
 ***********************************************************************************************/
/*! \brief        Provides the state hierarchy starting with the given state
 *  \param[in]    pFsmContext Pointer to context of state machine
 *  \param[in]    state The current state
 *  \param[out]   hierarchy The returned state hierarchy
 ***********************************************************************************************/
static uint8_t Fsm_GetStateHierachy(const tFsmContext* const pFsmContext, tFsmState state, tFsmState* hierarchy)
{
  uint8_t index;

  index = 0u;

  while (FSM_DEFAULT_STATE != state)
  {
    /* Check for hierarchy depth and stateCount to prevent buffer overrun(s) */
    if ( (index >= FSM_MAX_HIERARCHY_DEPTH)
      || (state >= pFsmContext->stateCount))
    {
      FSM_DEBUG_ERROR_PRINT(pFsmContext->instance);

      index = 0u;
      state = FSM_DEFAULT_STATE;
    }
    else
    {
      hierarchy[index] = state;
      index++;

      state = pFsmContext->stateDefinitions[state].superState;
    }
  }

  return index;
}



/************************************************************************************************
 *  Fsm_HandleEvent
 ***********************************************************************************************/
/*! \brief        Handles the given event
 *  \param[in]    pFsmContext Pointer to context of state machine
 *  \param[in]    event The event to be handled
 *  \param[out]   iterate Indicates whether state hierarchy iteration is allowed
 ***********************************************************************************************/
static void Fsm_HandleEvent(tFsmContextPtr const pFsmContext, tFsmEvent event, tFsmIterateHierarchy iterate)
{
  uint16_t index;
  uint16_t hierarchy;
  tFsmState state;
  tFsmGuard guard;
  tFsmStateDefinition* stateDef;
  tFsmEventEntry* triggers;

  state = pFsmContext->state;
  hierarchy = 0u;

  FSM_DEBUG_PRINT2(pFsmContext->instance, event, state);

  while ((FSM_DEFAULT_STATE != state)
      && (state < pFsmContext->stateCount))
  {
    stateDef = &pFsmContext->stateDefinitions[state];
    triggers = stateDef->triggers;

    /* Search for matching event handler */
    for (index = 0u; index < stateDef->triggerCount; index++)
    {
      if ((triggers[index].trigger == event)
        || (triggers[index].trigger == kFsmDefaultEvent_Any))
      {
        /* Handler matching event found, now execute handler */
        guard = triggers[index].handler(pFsmContext, event);

        FSM_DEBUG_PRINT3(pFsmContext->instance, guard);

        if (kFsmGuard_False != guard)
        {
          if (kFsmGuard_True == guard)
          {
            /* Pending state may be "none" */
            pFsmContext->pendingState = triggers[index].nextState;
          }
          else /* kFsmGuard_Overwrite */
          {
            /* Do not set pendingState, this is (has to be) done by the event handler */
          }

          /* Stop searching for event handlers as event was handled */
          state = FSM_DEFAULT_STATE;

          break;
        }
        else /* kFsmGuard_False */
        {
          /* State shall not be left, continue to search for other matching event handlers */
        }
      }
    }

    /* Check if event has been already handled */
    if (FSM_DEFAULT_STATE != state)
    {
      /* No matching event handler returning kFsmGuard_True could be found for current state,
      * continue search in (encapsulating) super state, if possible */
      if (kFsmIterateHierarchy_Enabled == iterate)
      {
        hierarchy++;

        if (hierarchy < FSM_MAX_HIERARCHY_DEPTH)
        {
          state = stateDef->superState;
        }
        else
        {
          /* Abort search to avoid (potential) infinite loop */
          FSM_DEBUG_ERROR_PRINT(pFsmContext->instance);

          state = FSM_DEFAULT_STATE;
        }
      }
      else
      {
        /* State traversing not allowed, abort search at this point */
        state = FSM_DEFAULT_STATE;
      }
    }
  }

#if defined( FSM_ENABLE_DEBUGGING )
  if ((state >= pFsmContext->stateCount)
   && (FSM_DEFAULT_STATE != state))
  {
    FSM_DEBUG_ERROR_PRINT(pFsmContext->instance);
  }
#endif /* FSM_ENABLE_DEBUGGING */
}



/*===============================================================================================
*                                       GLOBAL FUNCTIONS
===============================================================================================*/

/************************************************************************************************
 *  Fsm_InitPowerOn
 ***********************************************************************************************/
/*! \brief        Module initialization
 ***********************************************************************************************/
void Fsm_InitPowerOn( void )
{
#if defined( FSM_ENABLE_DEBUGGING )
  g_Instance = 0u;
#endif
}



/************************************************************************************************
 *  Fsm_Init
 ***********************************************************************************************/
/*! \brief        State machine initialization
 *  \param[in]    pFsmContext Pointer to context of state machine
 *  \param[in]    initialState The initial state of the state machine
 ***********************************************************************************************/
void Fsm_Init( tFsmContextPtr const pFsmContext, tFsmState initialState )
{
#if defined( FSM_ENABLE_DEBUGGING )
   pFsmContext->instance = g_Instance;
   g_Instance++;
#endif

   /* Start with internal default state */
   pFsmContext->state = FSM_DEFAULT_STATE;
   pFsmContext->pendingState = initialState;
   pFsmContext->pendingEvent = kFsmDefaultEvent_None;

   /* Enter provided initial state */
   Fsm_StateTask(pFsmContext);
}

/**********************************************************************************************************************
 *  Fsm_DeInit
 *********************************************************************************************************************/
/*! \brief        State machine deinitialization
 *  \param[in]    pFsmContext Pointer to context of state machine
 *********************************************************************************************************************/
void Fsm_DeInit( tFsmContextPtr const pFsmContext )
{
   /* Switch to internal default state to avoid further processing */
   pFsmContext->state = FSM_DEFAULT_STATE;
   pFsmContext->pendingState = FSM_DEFAULT_STATE;
   pFsmContext->pendingEvent = kFsmDefaultEvent_None;
}

/**********************************************************************************************************************
 *  Fsm_StateTask
 *********************************************************************************************************************/
/*! \brief        State machine handling
 *  \details      This function checks for pending events / state transitions and handles them appropriately.
 *  \param[in]    pFsmContext Pointer to context of state machine
 *********************************************************************************************************************/
void Fsm_StateTask( tFsmContextPtr const pFsmContext )
{
   tFsmEvent   event;

   tFsmState   stateHierarchyExit[FSM_MAX_HIERARCHY_DEPTH];
   tFsmState   stateHierarchyEntry[FSM_MAX_HIERARCHY_DEPTH];

   tFsmState   currentState;
   tFsmState   pendingState;

   int8_t      index;
   uint8_t     exitCount;
   uint8_t     entryCount;
   int8_t      exitIndex;
   int8_t      entryIndex;

   /* Check for pending event, which could cause state change */
   event = pFsmContext->pendingEvent;

   if (kFsmDefaultEvent_None != event)
   {
      /* Reset event */
      pFsmContext->pendingEvent = kFsmDefaultEvent_None;

      /* Process event and trigger state change if necessary */
      Fsm_HandleEvent(pFsmContext, event, kFsmIterateHierarchy_Enabled);
   }

   /* Check whether state has to be changed */
   while (FSM_DEFAULT_STATE != pFsmContext->pendingState)
   {
      currentState = pFsmContext->state;
      pendingState = pFsmContext->pendingState;

      FSM_DEBUG_PRINT1(pFsmContext->instance, currentState, pendingState);

      /* Reset event which triggered state change */
      pFsmContext->pendingState = FSM_DEFAULT_STATE;

      /* Get state hierarchy of current state and pending state */
      exitCount = Fsm_GetStateHierachy(pFsmContext, currentState, stateHierarchyExit);
      entryCount = Fsm_GetStateHierachy(pFsmContext, pendingState, stateHierarchyEntry);

      /* Check for re-entry of current state */
      if (currentState == pendingState)
      {
         exitIndex = 0;
         entryIndex = 0;
      }
      else
      {
         exitIndex = (int8_t)(exitCount - 1);
         entryIndex = (int8_t)(entryCount - 1);

         /* Search for common super state of current state and pending state */
         while ( (exitIndex >= 0)
               && (stateHierarchyExit[exitIndex] == stateHierarchyEntry[entryIndex]) )
         {
            exitIndex--;
            entryIndex--;
         }

         /* An exitIndex / entryIndex larger than 0 indicates that both state have a common super state */
      }

      /* Leave all super-states
       * A state is always left, even if the pending state is the same (re-entry),
       * but not if the target state is a sub-state of the current state.
       * An exit handler is executed after the transition action has been executed */
      for (index = 0; index <= exitIndex; index++)
      {
         pFsmContext->state = stateHierarchyExit[index];

         /* Leave previous state by calling exit handler */
         Fsm_HandleEvent(pFsmContext, kFsmDefaultEvent_Exit, kFsmIterateHierarchy_Disabled);
      }

      /* Enter all super-states
       * A pending state is only entered if it is not a previous super-state */
      for (index = entryIndex; index >= 0; index--)
      {
         pFsmContext->state = stateHierarchyEntry[index];

         /* Now enter new state by calling entry handler */
         Fsm_HandleEvent(pFsmContext, kFsmDefaultEvent_Entry, kFsmIterateHierarchy_Disabled);
      }

      /* Set final state, necessary if new state is super-state of previous state (no re-entry) */
      pFsmContext->state = pendingState;
   }
}

/**********************************************************************************************************************
 *  Fsm_TriggerEvent
 *********************************************************************************************************************/
/*! \brief        Triggers event for given state machine
 *  \param[in]    pFsmContext Pointer to context of state machine
 *  \param[in]    event The event to be triggered
 *********************************************************************************************************************/
void Fsm_TriggerEvent( tFsmContextPtr const pFsmContext, tFsmEvent event )
{
   pFsmContext->pendingEvent = event;
}

/**********************************************************************************************************************
 *  Fsm_EventHandlerDefault
 *********************************************************************************************************************/
/*! \brief        Default event handler
 *  \param[in]    pFsmContext Pointer to context of state machine
 *  \param[in]    event The event that triggered the handler execution
 *********************************************************************************************************************/
tFsmGuard Fsm_EventHandlerDefault(tFsmContextPtr const pFsmContext, tFsmEvent event)
{
#if defined( V_ENABLE_USE_DUMMY_STATEMENT )
   (void)pFsmContext;
   (void)event;
#endif /* V_ENABLE_USE_DUMMY_STATEMENT */

   /* Do nothing, but allow state transition */

   return kFsmGuard_True;
}
