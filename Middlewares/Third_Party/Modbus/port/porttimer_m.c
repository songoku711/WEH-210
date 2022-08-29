/*
 * FreeModbus Libary: RT-Thread Port
 * Copyright (C) 2013 Armink <armink.ztl@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * File: $Id: porttimer_m.c,v 1.60 2013/08/13 15:07:05 Armink add Master Functions$
 */

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbconfig.h"

/* ----------------------- Platform includes --------------------------------*/
#include "tim.h"

/* ----------------------- Macro definitions --------------------------------*/
#define MODBUS_MASTER_TIMER                 &htim7

/* ----------------------- Static variables ---------------------------------*/
static USHORT g_usTimeOut50us;




#if MB_MASTER_RTU_ENABLED > 0 || MB_MASTER_ASCII_ENABLED > 0
/* ----------------------- Static functions ---------------------------------*/
static void prvvTIMERExpiredISR(void);
static void timer_timeout_ind(TIM_HandleTypeDef *xTimer);


/* ----------------------- Start implementation -----------------------------*/
BOOL xMBMasterPortTimersInit(USHORT usTimeOut50us)
{
  /* Timer frequency is set to 20 kHz (period 50us) when initialization */
  MODBUS_DEBUG("Init timer!\r\n");
  g_usTimeOut50us = usTimeOut50us;
  
  __HAL_TIM_SetAutoreload(MODBUS_MASTER_TIMER, (g_usTimeOut50us - 1));
  HAL_TIM_RegisterCallback(MODBUS_MASTER_TIMER, HAL_TIM_PERIOD_ELAPSED_CB_ID, timer_timeout_ind);
  
  return TRUE;
}

void vMBMasterPortTimersT35Enable()
{
  vMBMasterSetCurTimerMode(MB_TMODE_T35);
  
  __HAL_TIM_SetAutoreload(MODBUS_MASTER_TIMER, (g_usTimeOut50us - 1));
  __HAL_TIM_SetCounter(MODBUS_MASTER_TIMER, 0);
  
  HAL_TIM_Base_Start_IT(MODBUS_MASTER_TIMER);
}

void vMBMasterPortTimersConvertDelayEnable()
{
  vMBMasterSetCurTimerMode(MB_TMODE_CONVERT_DELAY);
  
  uint16_t delay = (uint16_t)MB_MASTER_DELAY_MS_CONVERT * (uint16_t)20U - 1U;
  
  __HAL_TIM_SetAutoreload(MODBUS_MASTER_TIMER, delay);
  __HAL_TIM_SetCounter(MODBUS_MASTER_TIMER, 0);
  
  HAL_TIM_Base_Start_IT(MODBUS_MASTER_TIMER);
}

void vMBMasterPortTimersRespondTimeoutEnable()
{
  vMBMasterSetCurTimerMode(MB_TMODE_RESPOND_TIMEOUT);
  
  uint16_t delay = (uint16_t)MB_MASTER_TIMEOUT_MS_RESPOND * (uint16_t)20U - 1U;
  
  __HAL_TIM_SetAutoreload(MODBUS_MASTER_TIMER, delay);
  __HAL_TIM_SetCounter(MODBUS_MASTER_TIMER, 0);
  
  HAL_TIM_Base_Start_IT(MODBUS_MASTER_TIMER);
}

void vMBMasterPortTimersDisable()
{
  HAL_TIM_Base_Stop_IT(MODBUS_MASTER_TIMER);
}

void prvvTIMERExpiredISR(void)
{
  (void)pxMBMasterPortCBTimerExpired();
}

static void timer_timeout_ind(TIM_HandleTypeDef *xTimer)
{
  MODBUS_DEBUG("Timer callback!\r\n");
  HAL_TIM_Base_Stop_IT(MODBUS_MASTER_TIMER);

  prvvTIMERExpiredISR();
}

#endif
