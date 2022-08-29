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
#include "mb.h"
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

USHORT  usModbusUserData[8];
USHORT  usLastModbusUserData[8];
UCHAR   usModbusUserCoil[8];
uint8_t modbusCmdTaskCount = 0;
/* USER CODE END Variables */
/* Definitions for menuTask */
osThreadId_t menuTaskHandle;
const osThreadAttr_t menuTask_attributes = {
  .name = "menuTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for modbusPollTask */
osThreadId_t modbusPollTaskHandle;
const osThreadAttr_t modbusPollTask_attributes = {
  .name = "modbusPollTask",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityNormal1,
};
/* Definitions for modbusCmdTask */
osThreadId_t modbusCmdTaskHandle;
const osThreadAttr_t modbusCmdTask_attributes = {
  .name = "modbusCmdTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
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
/* Definitions for modbusTimer */
osTimerId_t modbusTimerHandle;
const osTimerAttr_t modbusTimer_attributes = {
  .name = "modbusTimer"
};
/* Definitions for mainFunctionTimer */
osTimerId_t mainFunctionTimerHandle;
const osTimerAttr_t mainFunctionTimer_attributes = {
  .name = "mainFunctionTimer"
};
/* Definitions for modbusEvent */
osEventFlagsId_t modbusEventHandle;
const osEventFlagsAttr_t modbusEvent_attributes = {
  .name = "modbusEvent"
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
void startModbusPollTask(void *argument);
void startModbusCmdTask(void *argument);
void startLcdTask(void *argument);
void startSystemInitTask(void *argument);
void startIoTask(void *argument);
void startProgramTask(void *argument);
void startIoExtIrqCbkTask(void *argument);
void modbusTimerHandler(void *argument);
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
  /* creation of modbusTimer */
  modbusTimerHandle = osTimerNew(modbusTimerHandler, osTimerPeriodic, NULL, &modbusTimer_attributes);

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

  /* creation of modbusPollTask */
  modbusPollTaskHandle = osThreadNew(startModbusPollTask, NULL, &modbusPollTask_attributes);

  /* creation of modbusCmdTask */
  modbusCmdTaskHandle = osThreadNew(startModbusCmdTask, NULL, &modbusCmdTask_attributes);

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
  /* creation of modbusEvent */
  modbusEventHandle = osEventFlagsNew(&modbusEvent_attributes);

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
  uint32_t recvEvent;
  
  /* Wait for system initialization done */
  recvEvent = osEventFlagsWait( systemInitEventHandle, \
                                SYSTEM_INIT_EVENT_MCAL_READY, \
                                osFlagsWaitAny | osFlagsNoClear, \
                                osWaitForever);
  
  recvEvent = osEventFlagsWait( systemInitEventHandle, \
                                SYSTEM_INIT_EVENT_IO_READY, \
                                osFlagsWaitAny | osFlagsNoClear, \
                                osWaitForever);
  
  recvEvent = osEventFlagsWait( systemInitEventHandle, \
                                SYSTEM_INIT_EVENT_PROG_READY, \
                                osFlagsWaitAny | osFlagsNoClear, \
                                osWaitForever);
  
  recvEvent = osEventFlagsWait( systemInitEventHandle, \
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

/* USER CODE BEGIN Header_startModbusPollTask */
/**
* @brief Function implementing the modbusPollTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_startModbusPollTask */
void startModbusPollTask(void *argument)
{
  /* USER CODE BEGIN startModbusPollTask */
  uint32_t recvEvent;
  
  /* Wait for system initialization done */
  recvEvent = osEventFlagsWait( systemInitEventHandle, \
                                SYSTEM_INIT_EVENT_MCAL_READY, \
                                osFlagsWaitAny | osFlagsNoClear, \
                                osWaitForever);
  
  recvEvent = osEventFlagsWait( systemInitEventHandle, \
                                SYSTEM_INIT_EVENT_MB_READY, \
                                osFlagsWaitAny | osFlagsNoClear, \
                                osWaitForever);

  /* Main task loop */
  while (1)
  {
    eMBMasterPoll();
  }
  /* USER CODE END startModbusPollTask */
}

/* USER CODE BEGIN Header_startModbusCmdTask */
/**
* @brief Function implementing the modbusCmdTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_startModbusCmdTask */
void startModbusCmdTask(void *argument)
{
  /* USER CODE BEGIN startModbusCmdTask */
  uint32_t recvEvent;
  
  /* Wait for system initialization done */
  recvEvent = osEventFlagsWait( systemInitEventHandle, \
                                SYSTEM_INIT_EVENT_MCAL_READY, \
                                osFlagsWaitAny | osFlagsNoClear, \
                                osWaitForever);
    
  eMBMasterInit(MB_RTU, 2, 38400, MB_PAR_NONE);
  eMBMasterEnable();

  osEventFlagsSet(systemInitEventHandle, SYSTEM_INIT_EVENT_MB_READY);

  /* Wait for menu initialization done */
  recvEvent = osEventFlagsWait( systemInitEventHandle, \
                                SYSTEM_INIT_EVENT_MENU_READY, \
                                osFlagsWaitAny | osFlagsNoClear, \
                                osWaitForever);
  
  /* Main task loop */ 
  while (1)
  {
    recvEvent = osEventFlagsWait( modbusEventHandle, \
                                  MODBUS_EVENT_READ_COILS | MODBUS_EVENT_READ_DIS_INPUTS | MODBUS_EVENT_READ_INPUTS | MODBUS_EVENT_READ_HOLDINGS, \
                                  osFlagsWaitAny, \
                                  osWaitForever);
    
    switch (recvEvent)
    {
      case MODBUS_EVENT_READ_COILS:
      {
//        (void)eMBMasterReqReadCoils(1, 1, 3, 100);
        break;
      }
      case MODBUS_EVENT_READ_DIS_INPUTS:
      {
//        (void)eMBMasterReqReadDiscreteInputs(1, 1, 3, 100);
        break;
      }
      case MODBUS_EVENT_READ_INPUTS:
      {
//        (void)eMBMasterReqReadInputRegister(1, 1, 5, 100);
        break;
      }
      case MODBUS_EVENT_READ_HOLDINGS:
      {
//        (void)eMBMasterReqReadHoldingRegister(1, 1, 5, 100);
        break;
      }
      default:
        break;
    }
  }
  /* USER CODE END startModbusCmdTask */
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
  uint32_t recvEvent;
  
  /* Wait for system initialization done */
  recvEvent = osEventFlagsWait( systemInitEventHandle, \
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
  
  /* Start modbus timer - 100ms period */
  osTimerStart(modbusTimerHandle, (uint32_t)100U);
  
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
  uint32_t recvEvent;
  
  /* Wait for system initialization done */
  recvEvent = osEventFlagsWait( systemInitEventHandle, \
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
  uint32_t recvEvent;
  
  /* Wait for system initialization done */
  recvEvent = osEventFlagsWait( systemInitEventHandle, \
                                SYSTEM_INIT_EVENT_MCAL_READY, \
                                osFlagsWaitAny | osFlagsNoClear, \
                                osWaitForever);
    
  ProgramManager_Init();

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
  uint32_t recvEvent;
  
  /* Wait for system initialization done */
  recvEvent = osEventFlagsWait( systemInitEventHandle, \
                                SYSTEM_INIT_EVENT_MCAL_READY, \
                                osFlagsWaitAny | osFlagsNoClear, \
                                osWaitForever);
  
  recvEvent = osEventFlagsWait( systemInitEventHandle, \
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

/* modbusTimerHandler function */
void modbusTimerHandler(void *argument)
{
  /* USER CODE BEGIN modbusTimerHandler */
  modbusCmdTaskCount++;
  
  if (modbusCmdTaskCount == (uint8_t)2U)
  {
    osEventFlagsSet(modbusEventHandle, MODBUS_EVENT_READ_COILS);
  }
  else if (modbusCmdTaskCount == (uint8_t)4U)
  {
    osEventFlagsSet(modbusEventHandle, MODBUS_EVENT_READ_DIS_INPUTS);
  }
  else if (modbusCmdTaskCount == (uint8_t)6U)
  {
    osEventFlagsSet(modbusEventHandle, MODBUS_EVENT_READ_INPUTS);
  }
  else if (modbusCmdTaskCount == (uint8_t)8U)
  {
    osEventFlagsSet(modbusEventHandle, MODBUS_EVENT_READ_HOLDINGS);
    modbusCmdTaskCount = (uint8_t)0U;
  }
  else if (modbusCmdTaskCount > (uint8_t)8U)
  {
    modbusCmdTaskCount = (uint8_t)0U;
  }
  else
  {
    /* shit */
  }
  /* USER CODE END modbusTimerHandler */
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

