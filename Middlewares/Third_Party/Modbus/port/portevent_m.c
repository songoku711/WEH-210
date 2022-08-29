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
 * File: $Id: portevent_m.c v 1.60 2013/08/13 15:07:05 Armink add Master Functions$
 */

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbconfig.h"

#if MB_MASTER_RTU_ENABLED > 0 || MB_MASTER_ASCII_ENABLED > 0
/* ----------------------- Static variables ---------------------------------*/
static osSemaphoreId_t xMasterRunRes;
static osEventFlagsId_t xMasterOsEvent;

/* ----------------------- Start implementation -----------------------------*/
BOOL xMBMasterPortEventInit(void)
{
  xMasterOsEvent = osEventFlagsNew(NULL);

  if (xMasterOsEvent == NULL)
  {
    MODBUS_DEBUG("xMBPortEventInit ERR=%d!\r\n", xMasterOsEvent);
    return FALSE;
  }
  
  return TRUE;
}

BOOL xMBMasterPortEventPost(eMBMasterEventType eEvent)
{
  MODBUS_DEBUG("Set event=%d!\r\n", eEvent);
  osEventFlagsSet(xMasterOsEvent, eEvent);
  
  return TRUE;
}

BOOL xMBMasterPortEventGet(eMBMasterEventType *eEvent)
{
  uint32_t recvEvent;
  
  /* waiting forever OS event */
  MODBUS_DEBUG("Wait for event...\r\n");
  recvEvent = osEventFlagsWait(xMasterOsEvent, EV_MASTER_READY | EV_MASTER_FRAME_RECEIVED | EV_MASTER_EXECUTE | EV_MASTER_FRAME_SENT | EV_MASTER_ERROR_PROCESS,
                               osFlagsWaitAny, MODBUS_EVENT_TIMEOUT);
  MODBUS_DEBUG("Received event=%d\r\n", recvEvent);
  
  /* the enum type couldn't convert to int type */
  switch (recvEvent)
  {
    case EV_MASTER_READY:
      *eEvent = EV_MASTER_READY;
      break;
    case EV_MASTER_FRAME_RECEIVED:
      *eEvent = EV_MASTER_FRAME_RECEIVED;
      break;
    case EV_MASTER_EXECUTE:
      *eEvent = EV_MASTER_EXECUTE;
      break;
    case EV_MASTER_FRAME_SENT:
      *eEvent = EV_MASTER_FRAME_SENT;
      break;
    case EV_MASTER_ERROR_PROCESS:
      *eEvent = EV_MASTER_ERROR_PROCESS;
      break;
    default:
      return FALSE;
  }
  
  return TRUE;
}

/**
 * This function is initialize the OS resource for modbus master.
 * Note:The resource is define by OS.If you not use OS this function can be empty.
 *
 */
void vMBMasterOsResInit(void)
{
  xMasterRunRes = osSemaphoreNew(1U, 1U, NULL);
  
  if (xMasterRunRes == NULL)
  {
    MODBUS_DEBUG("vMBMasterOsResInit ERR=%d!\r\n", xMasterRunRes);
  }
}

/**
 * This function is take Modbus Master running resource.
 * Note:The resource is define by OS. If you not use OS this function can be just return TRUE.
 *
 * @param lTimeOut the waiting time.
 *
 * @return resource taked result
 */
BOOL xMBMasterRunResTake(LONG lTimeOut)
{
  osStatus_t status;
  
  /* Take resource */
  status = osSemaphoreAcquire(xMasterRunRes, lTimeOut);
  
  if (status == osOK)
  {
    return TRUE;
  }

  return FALSE;
}

/**
 * This function is release Mobus Master running resource.
 * Note:The resource is define by Operating System.If you not use OS this function can be empty.
 *
 */
void vMBMasterRunResRelease(void)
{
  /* Release resource */
  (void)osSemaphoreRelease(xMasterRunRes);
}

/**
 * This is modbus master respond timeout error process callback function.
 * @note There functions will block modbus master poll while execute OS waiting.
 * So, for real-time of system, do not execute too much waiting process.
 *
 * @param ucDestAddress destination salve address
 * @param pucPDUData PDU buffer data
 * @param ucPDULength PDU buffer length
 *
 */
void vMBMasterErrorCBRespondTimeout(UCHAR ucDestAddress, const UCHAR* pucPDUData, USHORT ucPDULength)
{
  xMBMasterPortEventPost(EV_MASTER_ERROR_RESPOND_TIMEOUT);
}

/**
 * This is modbus master receive data error process callback function.
 * @note There functions will block modbus master poll while execute OS waiting.
 * So, for real-time of system, do not execute too much waiting process.
 *
 * @param ucDestAddress destination salve address
 * @param pucPDUData PDU buffer data
 * @param ucPDULength PDU buffer length
 *
 */
void vMBMasterErrorCBReceiveData(UCHAR ucDestAddress, const UCHAR* pucPDUData, USHORT ucPDULength)
{
  xMBMasterPortEventPost(EV_MASTER_ERROR_RECEIVE_DATA);
}

/**
 * This is modbus master execute function error process callback function.
 * @note There functions will block modbus master poll while execute OS waiting.
 * So, for real-time of system, do not execute too much waiting process.
 *
 * @param ucDestAddress destination salve address
 * @param pucPDUData PDU buffer data
 * @param ucPDULength PDU buffer length
 *
 */
void vMBMasterErrorCBExecuteFunction(UCHAR ucDestAddress, const UCHAR* pucPDUData, USHORT ucPDULength)
{
  xMBMasterPortEventPost(EV_MASTER_ERROR_EXECUTE_FUNCTION);
}

/**
 * This is modbus master request process success callback function.
 * @note There functions will block modbus master poll while execute OS waiting.
 * So, for real-time of system, do not execute too much waiting process.
 *
 */
void vMBMasterCBRequestScuuess(void)
{
  xMBMasterPortEventPost(EV_MASTER_PROCESS_SUCESS);
}

/**
 * This function is wait for modbus master request finish and return result.
 * Waiting result include request process success, request respond timeout,
 * receive data error and execute function error. You can use the above callback function.
 * @note If you are use OS, you can use OS's event mechanism. Otherwise you have to run
 * much user custom delay for waiting.
 *
 * @return request error code
 */
eMBMasterReqErrCode eMBMasterWaitRequestFinish(void)
{
  eMBMasterReqErrCode eErrStatus = MB_MRE_NO_ERR;
  uint32_t recvEvent;
  
  /* waiting forever OS event */
  MODBUS_DEBUG("Wait for event...\r\n");
  recvEvent = osEventFlagsWait(xMasterOsEvent, EV_MASTER_PROCESS_SUCESS | EV_MASTER_ERROR_RESPOND_TIMEOUT | EV_MASTER_ERROR_RECEIVE_DATA | EV_MASTER_ERROR_EXECUTE_FUNCTION,
                               osFlagsWaitAny, MODBUS_EVENT_TIMEOUT);
  MODBUS_DEBUG("Received event=%d\r\n", recvEvent);
  
  /* the enum type couldn't convert to int type */
  switch (recvEvent)
  {
    case EV_MASTER_PROCESS_SUCESS:
        break;
    case EV_MASTER_ERROR_RESPOND_TIMEOUT:
    {
      eErrStatus = MB_MRE_TIMEDOUT;
      break;
    }
    case EV_MASTER_ERROR_RECEIVE_DATA:
    {
      eErrStatus = MB_MRE_REV_DATA;
      break;
    }
    case EV_MASTER_ERROR_EXECUTE_FUNCTION:
    {
      eErrStatus = MB_MRE_EXE_FUN;
      break;
    }
    default:
      eErrStatus = MB_MRE_TIMEDOUT;
      break;
  }
  
  return eErrStatus;
}

#endif
