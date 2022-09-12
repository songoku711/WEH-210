/* 
 * File:   ioManager.c
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

#include "ioManager.h"



/*===============================================================================================
*                                       DEFINES AND MACROS
===============================================================================================*/

/* Timer instance for debouncing timeout - 10ms period */
#define IOMANAGER_TIMER_INSTANCE                          (&htim10)

/* Timeout flag for OS thread notification */
#define IOMANAGER_BUTTON_TIMEOUT_FLAG                     (uint32_t)0x10000000

/* Number of buttons */
#define IOMANAGER_BUTTON_NUM                              (13U)

/* Maximum number of simutaneous-pressed buttons */
#define IOMANAGER_BUTTON_SIM_PRESSED_NUM_MAX              (2U)

/* Delay for short-pressed buttons - 30 milliseconds = 3 x 10ms */
#define IOMANAGER_BUTTON_SHORT_PRESS_DELAY_MAX            (uint16_t)3U

/* Delay for long-pressed buttons - 2.5 seconds = 250 x 10ms */
#define IOMANAGER_BUTTON_LONG_PRESS_DELAY_MAX             (uint16_t)250U

typedef enum
{
  IOMANAGER_BUTTON_DELAY_STATE_COUNTING,
  IOMANAGER_BUTTON_DELAY_STATE_DONE,
  IOMANAGER_BUTTON_DELAY_STATE_NOT_OVER
} IoManager_ButtonDelayStateType;

/* Button configuration structure */
typedef struct
{
  GPIO_TypeDef *port;
  uint16_t pin;
  uint32_t butState;
  uint32_t butPress;
} IoManager_ButtonConfStruct;

/* Button delay counter structure */
typedef struct
{
  uint8_t buttonCfgIdx;
  IoManager_ButtonDelayStateType delayState;
  uint16_t delay;
  uint16_t maxDelay;
} IoManager_ButtonDelayCntStruct;


/*===============================================================================================
*                                       GLOBAL VARIABLES
===============================================================================================*/

static uint32_t IoManager_curButtonState;
static uint8_t IoManager_curNumButtonPressed;

static IoManager_ButtonDelayCntStruct IoManager_buttonDelayCnt[IOMANAGER_BUTTON_SIM_PRESSED_NUM_MAX];

static IoManager_ButtonConfStruct IoManager_buttonCfg[IOMANAGER_BUTTON_NUM] =
{
  { BUT_DOWN_GPIO_Port,      BUT_DOWN_Pin,      (uint32_t)IOMANAGER_BUTTON_STATE_DOWN,      (uint32_t)(IOMANAGER_BUTTON_PRESS_SHORT) },
  { BUT_START_GPIO_Port,     BUT_START_Pin,     (uint32_t)IOMANAGER_BUTTON_STATE_START,     (uint32_t)(IOMANAGER_BUTTON_PRESS_SHORT | IOMANAGER_BUTTON_PRESS_LONG) },
  { BUT_UP_GPIO_Port,        BUT_UP_Pin,        (uint32_t)IOMANAGER_BUTTON_STATE_UP,        (uint32_t)(IOMANAGER_BUTTON_PRESS_SHORT) },
  { BUT_MENU_GPIO_Port,      BUT_MENU_Pin,      (uint32_t)IOMANAGER_BUTTON_STATE_HEAT,      (uint32_t)(IOMANAGER_BUTTON_PRESS_SHORT) },
  { BUT_STOP_GPIO_Port,      BUT_STOP_Pin,      (uint32_t)IOMANAGER_BUTTON_STATE_STOP,      (uint32_t)(IOMANAGER_BUTTON_PRESS_SHORT | IOMANAGER_BUTTON_PRESS_LONG) },
  { BUT_DRAIN_GPIO_Port,     BUT_DRAIN_Pin,     (uint32_t)IOMANAGER_BUTTON_STATE_DRAIN,     (uint32_t)(IOMANAGER_BUTTON_PRESS_SHORT) },
  { BUT_WASH_GPIO_Port,      BUT_WASH_Pin,      (uint32_t)IOMANAGER_BUTTON_STATE_WASH,      (uint32_t)(IOMANAGER_BUTTON_PRESS_SHORT) },
  { BUT_FUNC_GPIO_Port,      BUT_FUNC_Pin,      (uint32_t)IOMANAGER_BUTTON_STATE_LEVEL,     (uint32_t)(IOMANAGER_BUTTON_PRESS_SHORT) },
  { BUT_EXTRACT_GPIO_Port,   BUT_EXTRACT_Pin,   (uint32_t)IOMANAGER_BUTTON_STATE_EXTRACT,   (uint32_t)(IOMANAGER_BUTTON_PRESS_SHORT) },
  { BUT_SUPP_1_GPIO_Port,    BUT_SUPP_1_Pin,    (uint32_t)IOMANAGER_BUTTON_STATE_ADD,       (uint32_t)(IOMANAGER_BUTTON_PRESS_SHORT) },
  { BUT_COOL_GPIO_Port,      BUT_COOL_Pin,      (uint32_t)IOMANAGER_BUTTON_STATE_COLDWTR,   (uint32_t)(IOMANAGER_BUTTON_PRESS_SHORT) },
  { BUT_SUPP_2_GPIO_Port,    BUT_SUPP_2_Pin,    (uint32_t)IOMANAGER_BUTTON_STATE_SUB,       (uint32_t)(IOMANAGER_BUTTON_PRESS_SHORT) },
  { BUT_HEAT_GPIO_Port,      BUT_HEAT_Pin,      (uint32_t)IOMANAGER_BUTTON_STATE_HOTWTR,    (uint32_t)(IOMANAGER_BUTTON_PRESS_SHORT) }
};

static IoManager_StateType IoManager_gState = IOMANAGER_STATE_INIT;



extern osThreadId_t ioExtIrqCbkTaskHandle;

extern osMessageQueueId_t ioButtonStateQueueHandle;



/*===============================================================================================
*                                   LOCAL FUNCTION PROTOTYPES
===============================================================================================*/

static void IoManager_TimerElapsedCbk(TIM_HandleTypeDef *xTimer);



/*===============================================================================================
*                                       LOCAL FUNCTIONS
===============================================================================================*/

static void IoManager_TimerElapsedCbk(TIM_HandleTypeDef *xTimer)
{
  osThreadFlagsSet(ioExtIrqCbkTaskHandle, (uint32_t)IOMANAGER_BUTTON_TIMEOUT_FLAG);
}



/*===============================================================================================
*                                       GLOBAL FUNCTIONS
===============================================================================================*/

void IoManager_Init(void)
{
  IoManager_curButtonState = (uint32_t)0U;
  IoManager_curNumButtonPressed = (uint8_t)0U;
  
  HAL_TIM_RegisterCallback(IOMANAGER_TIMER_INSTANCE, HAL_TIM_PERIOD_ELAPSED_CB_ID, IoManager_TimerElapsedCbk);

  IoManager_gState = IOMANAGER_STATE_READY;
}



/*=============================================================================================*/
IoManager_StateType IoManager_GetCurrentState(void)
{
  return IoManager_gState;
}



/*=============================================================================================*/
void IoManager_MainFunction(void)
{

}

/*=============================================================================================*/
void IoManager_ExtIrptMainFunction(void)
{
  uint32_t recvFlag;
  uint8_t index;
  uint8_t buttonCfgIdx;
  uint8_t buttonDelayStateCountingNum;

  recvFlag = osThreadFlagsWait((uint32_t)GPIO_PIN_MASK | IOMANAGER_BUTTON_TIMEOUT_FLAG, osFlagsWaitAny, osWaitForever);

  /* External interrupt from button pressed */
  if (recvFlag & (uint32_t)GPIO_PIN_MASK)
  {
    /* Search pin caused interrupt index in table */
    for (index = (uint8_t)0U; index < IOMANAGER_BUTTON_NUM; index++)
    {
      if (IoManager_buttonCfg[index].pin == recvFlag)
      {
        /* Check if there are rooms for storing button delay counter */
        if (IoManager_curNumButtonPressed < IOMANAGER_BUTTON_SIM_PRESSED_NUM_MAX)
        {
          /* Store delay counter parameters */
          IoManager_buttonDelayCnt[IoManager_curNumButtonPressed].buttonCfgIdx = index;
          IoManager_buttonDelayCnt[IoManager_curNumButtonPressed].delayState = IOMANAGER_BUTTON_DELAY_STATE_COUNTING;
          IoManager_buttonDelayCnt[IoManager_curNumButtonPressed].delay = (uint16_t)0U;

          if (IoManager_buttonCfg[index].butPress & (uint32_t)IOMANAGER_BUTTON_PRESS_LONG)
          {
            IoManager_buttonDelayCnt[IoManager_curNumButtonPressed].maxDelay = IOMANAGER_BUTTON_LONG_PRESS_DELAY_MAX;
          }
          else
          {
            IoManager_buttonDelayCnt[IoManager_curNumButtonPressed].maxDelay = IOMANAGER_BUTTON_SHORT_PRESS_DELAY_MAX;
          }

          /* Increase number of buttons simutaneously pressed */
          IoManager_curNumButtonPressed++;

          /* Start or reset timer counter */
          __HAL_TIM_SetCounter(IOMANAGER_TIMER_INSTANCE, 0);
          HAL_TIM_Base_Start_IT(IOMANAGER_TIMER_INSTANCE);
        }

        break;
      }
    }
  }
  /* One period of button pressed timeout passed */
  else if (recvFlag & IOMANAGER_BUTTON_TIMEOUT_FLAG)
  {
    /* Stop the timer */
    HAL_TIM_Base_Stop_IT(IOMANAGER_TIMER_INSTANCE);

    buttonDelayStateCountingNum = (uint8_t)0U;

    /* Loop all button input levels that counting delay */
    for (index = (uint8_t)0U; index < IoManager_curNumButtonPressed; index++)
    {
      buttonCfgIdx = IoManager_buttonDelayCnt[index].buttonCfgIdx;

      /* Check if button pressed level still hold after a period of delay */
      if (GPIO_PIN_RESET == HAL_GPIO_ReadPin(IoManager_buttonCfg[buttonCfgIdx].port, IoManager_buttonCfg[buttonCfgIdx].pin))
      {
        /* If delay counters reach maximum interval for each press types, set delay state to DONE,
           otherwise continue counting */
        if (IoManager_buttonDelayCnt[index].delayState == IOMANAGER_BUTTON_DELAY_STATE_COUNTING)
        {
          IoManager_buttonDelayCnt[index].delay++;

          if (IoManager_buttonDelayCnt[index].delay >= IoManager_buttonDelayCnt[index].maxDelay)
          {
            IoManager_buttonDelayCnt[index].delayState = IOMANAGER_BUTTON_DELAY_STATE_DONE;
          }
        }
      }
      /* Button pressed level is not stable, due to bouncing or noise */
      else
      {
        /* Set delay state to NOT OVER, and stop delay counting */
        if (IoManager_buttonDelayCnt[index].delayState == IOMANAGER_BUTTON_DELAY_STATE_COUNTING)
        {
          IoManager_buttonDelayCnt[index].delayState = IOMANAGER_BUTTON_DELAY_STATE_NOT_OVER;
        }
      }

      /* Update number of buttons that still count delay */
      if (IoManager_buttonDelayCnt[index].delayState == IOMANAGER_BUTTON_DELAY_STATE_COUNTING)
      {
        buttonDelayStateCountingNum++;
      }
    }

    /* If there are buttons that is still counting delay */
    if (buttonDelayStateCountingNum != (uint8_t)0U)
    {
      /* Start or reset timer counter */
      __HAL_TIM_SetCounter(IOMANAGER_TIMER_INSTANCE, 0);
      HAL_TIM_Base_Start_IT(IOMANAGER_TIMER_INSTANCE);
    }
    /* No buttons counting delay, only DONE or NOT OVER state */
    else
    {
      IoManager_curButtonState = (uint8_t)0U;

      /* Loop all button input levels that stop counting delay */
      for (index = (uint8_t)0U; index < IoManager_curNumButtonPressed; index++)
      {
        buttonCfgIdx = IoManager_buttonDelayCnt[index].buttonCfgIdx;

        /* Button counting delay DONE, store button state and its press type */
        if (IoManager_buttonDelayCnt[index].delayState == IOMANAGER_BUTTON_DELAY_STATE_DONE)
        {
          if (IoManager_buttonCfg[buttonCfgIdx].butPress & (uint32_t)IOMANAGER_BUTTON_PRESS_LONG)
          {
            IoManager_curButtonState |= IO_LONG_PRESS(IoManager_buttonCfg[buttonCfgIdx].butState);
          }
          else
          {
            IoManager_curButtonState |= IoManager_buttonCfg[buttonCfgIdx].butState;
          }
        }
        /* Button counting delay NOT OVER, store button state if its delay count is greater than short press delay,
           consider as it's short-pressed (for button that has long press type), otherwise discard it */
        else if (IoManager_buttonDelayCnt[index].delayState == IOMANAGER_BUTTON_DELAY_STATE_NOT_OVER)
        {
          if (IoManager_buttonDelayCnt[index].delay >= IOMANAGER_BUTTON_SHORT_PRESS_DELAY_MAX)
          {
            IoManager_curButtonState |= IoManager_buttonCfg[buttonCfgIdx].butState;
          }
        }
        else
        {
          /* There's no way code will execute here */
        }
      }

      if (IoManager_curButtonState != (uint32_t)0U)
      {
        IoManager_curNumButtonPressed = (uint8_t)0U;

        osMessageQueuePut(ioButtonStateQueueHandle, &IoManager_curButtonState, 0U, 0U);
      }
    }
  }
}



/*=============================================================================================*/
void IoManager_ExtIrptCallback(uint32_t arg)
{
  osThreadFlagsSet(ioExtIrqCbkTaskHandle, arg);
}



#ifdef  __cplusplus
}
#endif
