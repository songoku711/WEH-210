/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "eMB.h"
#include "lcd.h"

#include "menuManager.h"
#include "programManager.h"
#include "ioManager.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* Event flag for modbusEvent */
#define MODBUS_EVENT_READ_COILS                   (uint32_t)(0x01)
#define MODBUS_EVENT_READ_DIS_INPUTS              (uint32_t)(0x02)
#define MODBUS_EVENT_READ_INPUTS                  (uint32_t)(0x04)
#define MODBUS_EVENT_READ_HOLDINGS                (uint32_t)(0x08)

/* Event flag for systemInitEvent */
#define SYSTEM_INIT_EVENT_MCAL_READY              (uint32_t)(0x000F)
#define SYSTEM_INIT_EVENT_LCD_READY               (uint32_t)(0x0010)
#define SYSTEM_INIT_EVENT_MB_READY                (uint32_t)(0x0020)
#define SYSTEM_INIT_EVENT_IO_READY                (uint32_t)(0x0040)
#define SYSTEM_INIT_EVENT_PROG_READY              (uint32_t)(0x0080)
#define SYSTEM_INIT_EVENT_MENU_READY              (uint32_t)(0x0100)

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

uint16_t usModbusUserData[8];
uint16_t usLastModbusUserData[8];
uint8_t  usModbusUserCoil[8];
uint8_t  modbusCmdTaskCount = 0;

extern const eMB_ConfigStruct eMB_WehConfig;

/* USER CODE END Variables */
/* Definitions for menuTask */
osThreadId_t menuTaskHandle;
const osThreadAttr_t menuTask_attributes = {
  .name = "menuTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for eMBTask */
osThreadId_t eMBTaskHandle;
const osThreadAttr_t eMBTask_attributes = {
  .name = "eMBTask",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityNormal1,
};
/* Definitions for lcdTask */
osThreadId_t lcdTaskHandle;
const osThreadAttr_t lcdTask_attributes = {
  .name = "lcdTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for systemInitTask */
osThreadId_t systemInitTaskHandle;
const osThreadAttr_t systemInitTask_attributes = {
  .name = "systemInitTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityHigh,
};
/* Definitions for ioTask */
osThreadId_t ioTaskHandle;
const osThreadAttr_t ioTask_attributes = {
  .name = "ioTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for programTask */
osThreadId_t programTaskHandle;
const osThreadAttr_t programTask_attributes = {
  .name = "programTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for ioExtIrqCbkTask */
osThreadId_t ioExtIrqCbkTaskHandle;
const osThreadAttr_t ioExtIrqCbkTask_attributes = {
  .name = "ioExtIrqCbkTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal7,
};
/* Definitions for ioButtonStateQueue */
osMessageQueueId_t ioButtonStateQueueHandle;
const osMessageQueueAttr_t ioButtonStateQueue_attributes = {
  .name = "ioButtonStateQueue"
};
/* Definitions for mainFunctionTimer */
osTimerId_t mainFunctionTimerHandle;
const osTimerAttr_t mainFunctionTimer_attributes = {
  .name = "mainFunctionTimer"
};
/* Definitions for systemInitEvent */
osEventFlagsId_t systemInitEventHandle;
const osEventFlagsAttr_t systemInitEvent_attributes = {
  .name = "systemInitEvent"
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void startMenuTask(void *argument);
void starteMBTask(void *argument);
void startLcdTask(void *argument);
void startSystemInitTask(void *argument);
void startIoTask(void *argument);
void startProgramTask(void *argument);
void startIoExtIrqCbkTask(void *argument);
void mainFunctionTimerHandler(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* Create the timer(s) */
  /* creation of mainFunctionTimer */
  mainFunctionTimerHandle = osTimerNew(mainFunctionTimerHandler, osTimerPeriodic, NULL, &mainFunctionTimer_attributes);

  /* USER CODE BEGIN RTOS_TIMERS */
  
  /* USER CODE END RTOS_TIMERS */

  /* Create the queue(s) */
  /* creation of ioButtonStateQueue */
  ioButtonStateQueueHandle = osMessageQueueNew (1, sizeof(uint32_t), &ioButtonStateQueue_attributes);

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of menuTask */
  menuTaskHandle = osThreadNew(startMenuTask, NULL, &menuTask_attributes);

  /* creation of eMBTask */
  eMBTaskHandle = osThreadNew(starteMBTask, NULL, &eMBTask_attributes);

  /* creation of lcdTask */
  lcdTaskHandle = osThreadNew(startLcdTask, NULL, &lcdTask_attributes);

  /* creation of systemInitTask */
  systemInitTaskHandle = osThreadNew(startSystemInitTask, NULL, &systemInitTask_attributes);

  /* creation of ioTask */
  ioTaskHandle = osThreadNew(startIoTask, NULL, &ioTask_attributes);

  /* creation of programTask */
  programTaskHandle = osThreadNew(startProgramTask, NULL, &programTask_attributes);

  /* creation of ioExtIrqCbkTask */
  ioExtIrqCbkTaskHandle = osThreadNew(startIoExtIrqCbkTask, NULL, &ioExtIrqCbkTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* Create the event(s) */
  /* creation of systemInitEvent */
  systemInitEventHandle = osEventFlagsNew(&systemInitEvent_attributes);

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_startMenuTask */
/**
  * @brief  Function implementing the menuTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_startMenuTask */
void startMenuTask(void *argument)
{
  /* USER CODE BEGIN startMenuTask */

  /* Wait for system initialization done */
  (void)osEventFlagsWait( systemInitEventHandle, \
                          SYSTEM_INIT_EVENT_MCAL_READY, \
                          osFlagsWaitAny | osFlagsNoClear, \
                          osWaitForever);
  
  (void)osEventFlagsWait( systemInitEventHandle, \
                          SYSTEM_INIT_EVENT_IO_READY, \
                          osFlagsWaitAny | osFlagsNoClear, \
                          osWaitForever);
  
  (void)osEventFlagsWait( systemInitEventHandle, \
                          SYSTEM_INIT_EVENT_PROG_READY, \
                          osFlagsWaitAny | osFlagsNoClear, \
                          osWaitForever);
  
  (void)osEventFlagsWait( systemInitEventHandle, \
                          SYSTEM_INIT_EVENT_LCD_READY, \
                          osFlagsWaitAny | osFlagsNoClear, \
                          osWaitForever);
  
  MenuManager_Init();
  
  osEventFlagsSet(systemInitEventHandle, SYSTEM_INIT_EVENT_MENU_READY);
  
  /* Main task loop */
  while (1)
  {
    MenuManager_MainFunction();
    
    osDelay(5);
  }
  /* USER CODE END startMenuTask */
}

/* USER CODE BEGIN Header_starteMBTask */
/**
* @brief Function implementing the eMBTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_starteMBTask */
void starteMBTask(void *argument)
{
  /* USER CODE BEGIN starteMBTask */

  /* Wait for system initialization done */
  (void)osEventFlagsWait( systemInitEventHandle, \
                          SYSTEM_INIT_EVENT_MCAL_READY, \
                          osFlagsWaitAny | osFlagsNoClear, \
                          osWaitForever);
  
  eMB_Init(&eMB_WehConfig);
  eMB_Enable();

  osEventFlagsSet(systemInitEventHandle, SYSTEM_INIT_EVENT_MB_READY);
  
  /* Main task loop */
  while (1)
  {
    eMB_MainFunction();
    
    osDelay(5);
  }
  /* USER CODE END starteMBTask */
}

/* USER CODE BEGIN Header_startLcdTask */
/**
* @brief Function implementing the lcdTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_startLcdTask */
void startLcdTask(void *argument)
{
  /* USER CODE BEGIN startLcdTask */
  
  /* Wait for system initialization done */
  (void)osEventFlagsWait( systemInitEventHandle, \
                          SYSTEM_INIT_EVENT_MCAL_READY, \
                          osFlagsWaitAny | osFlagsNoClear, \
                          osWaitForever);
    
  osEventFlagsSet(systemInitEventHandle, SYSTEM_INIT_EVENT_LCD_READY);
  
  /* Main task loop */
  while (1)
  {
    LCD_FillMain();
    
    osDelay(10);
  }
  /* USER CODE END startLcdTask */
}

/* USER CODE BEGIN Header_startSystemInitTask */
/**
* @brief Function implementing the systemInitTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_startSystemInitTask */
void startSystemInitTask(void *argument)
{
  /* USER CODE BEGIN startSystemInitTask */
  HAL_StatusTypeDef retVal;
  
  /* Initialize external memory interface */
  retVal = MemIf_Init();
  
  if (retVal != HAL_OK)
  {
    Error_Handler();
  }

  /* Initialize state machine */
	Fsm_InitPowerOn();
  
  /* Initialize LCD */
  retVal = LCD_Init();
  
  if (retVal != HAL_OK)
  {
    Error_Handler();
  }
  
  LCD_SetContrast(50);
  
  /* Start main function timer - 20ms period */
  osTimerStart(mainFunctionTimerHandle, (uint32_t)20U);
  
  /* Set flag to notify to other tasks to start executing */
  osEventFlagsSet(systemInitEventHandle, SYSTEM_INIT_EVENT_MCAL_READY);
  
  /* Waiting to be terminated */
  osDelay(2000);
  
  osThreadTerminate(systemInitTaskHandle);
  /* USER CODE END startSystemInitTask */
}

/* USER CODE BEGIN Header_startIoTask */
/**
* @brief Function implementing the ioTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_startIoTask */
void startIoTask(void *argument)
{
  /* USER CODE BEGIN startIoTask */
  
  /* Wait for system initialization done */
  (void)osEventFlagsWait( systemInitEventHandle, \
                          SYSTEM_INIT_EVENT_MCAL_READY, \
                          osFlagsWaitAny | osFlagsNoClear, \
                          osWaitForever);
    
  IoManager_Init();

  osEventFlagsSet(systemInitEventHandle, SYSTEM_INIT_EVENT_IO_READY);
  
  /* Main task loop */
  while (1)
  {
    IoManager_MainFunction();
    
    osDelay(5);
  }
  /* USER CODE END startIoTask */
}

/* USER CODE BEGIN Header_startProgramTask */
/**
* @brief Function implementing the programTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_startProgramTask */
void startProgramTask(void *argument)
{
  /* USER CODE BEGIN startProgramTask */
  
  /* Wait for system initialization done */
  (void)osEventFlagsWait( systemInitEventHandle, \
                          SYSTEM_INIT_EVENT_MCAL_READY, \
                          osFlagsWaitAny | osFlagsNoClear, \
                          osWaitForever);
  
  ProgramManager_Init();
  
  (void)osEventFlagsWait( systemInitEventHandle, \
                          SYSTEM_INIT_EVENT_MB_READY, \
                          osFlagsWaitAny | osFlagsNoClear, \
                          osWaitForever);

  osEventFlagsSet(systemInitEventHandle, SYSTEM_INIT_EVENT_PROG_READY);
  
  /* Main task loop */
  while (1)
  {
    ProgramManager_MainFunction();

    osDelay(5);
  }
  /* USER CODE END startProgramTask */
}

/* USER CODE BEGIN Header_startIoExtIrqCbkTask */
/**
* @brief Function implementing the ioExtIrqCbkTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_startIoExtIrqCbkTask */
void startIoExtIrqCbkTask(void *argument)
{
  /* USER CODE BEGIN startIoExtIrqCbkTask */
  
  /* Wait for system initialization done */
  (void)osEventFlagsWait( systemInitEventHandle, \
                          SYSTEM_INIT_EVENT_MCAL_READY, \
                          osFlagsWaitAny | osFlagsNoClear, \
                          osWaitForever);
  
  (void)osEventFlagsWait( systemInitEventHandle, \
                          SYSTEM_INIT_EVENT_IO_READY, \
                          osFlagsWaitAny | osFlagsNoClear, \
                          osWaitForever);
  
  /* Main task loop */
  while (1)
  {
    IoManager_ExtIrptMainFunction();
  }
  /* USER CODE END startIoExtIrqCbkTask */
}

/* mainFunctionTimerHandler function */
void mainFunctionTimerHandler(void *argument)
{
  /* USER CODE BEGIN mainFunctionTimerHandler */
  if (MenuManager_SubTickHandler != NULL)
  {
    MenuManager_SubTickHandler();
  }
  
  if (ProgramManager_SubTickHandler != NULL)
  {
    ProgramManager_SubTickHandler();
  }
  /* USER CODE END mainFunctionTimerHandler */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

