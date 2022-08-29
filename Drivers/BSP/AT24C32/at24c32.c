/* 
 * File:   at24c32.c
 * Author: Huy Long
 * Comments:  File contains definitions, structures and interfaces to AT24C32.
 * Revision history: 1.0
 */

#ifdef   __cplusplus
extern "C" {
#endif



/*===============================================================================================
*                                         INCLUDE FILES
* 1) system and project includes
* 2) needed interfaces from external units
* 3) internal and external interfaces from this unit
===============================================================================================*/

#include "at24c32.h"
#include "i2c.h"



/*===============================================================================================
*                                       DEFINES AND MACROS
===============================================================================================*/

/**
 * @brief      AT24C32 I2C data buffer size
 *
 */
#define AT24C32_BUFFER_SIZE                               (uint8_t)40U

/**
 * @brief      AT24C32 I2C instance
 *
 */
#define AT24C32_I2C_INSTANCE                              hi2c1

/**
 * @brief      AT24C32 I2C device ready timeout maximum number
 *
 */
#define AT24C32_I2C_TIMEOUT_MAX                           (uint8_t)255U



/*===============================================================================================
*                                       LOCAL VARIABLES
===============================================================================================*/

/**
 * @brief      AT24C32 I2C read/write data buffer
 *
 */
uint8_t at24c32_u8Data[AT24C32_BUFFER_SIZE];

/**
 * @brief      AT24C32 I2C timeout for checking device ready
 *
 */
uint8_t at24c32_u8DeviceReadyTimeout;



/*===============================================================================================
*                                   LOCAL FUNCTION PROTOTYPES
===============================================================================================*/

HAL_StatusTypeDef AT24C32_IsDeviceReady
(
  void
);



/*===============================================================================================
*                                       LOCAL FUNCTIONS
===============================================================================================*/

HAL_StatusTypeDef AT24C32_IsDeviceReady
(
  void
)
{
  at24c32_u8DeviceReadyTimeout = AT24C32_I2C_TIMEOUT_MAX;
  
  while ( \
          (HAL_OK != HAL_I2C_IsDeviceReady(&AT24C32_I2C_INSTANCE, AT24C32_SLAVE_ADDRESS, 1, HAL_MAX_DELAY)) && \
          (at24c32_u8DeviceReadyTimeout > (uint8_t)0U) \
        )
  {
    at24c32_u8DeviceReadyTimeout--;
  }
  
  if ((uint8_t)0U == at24c32_u8DeviceReadyTimeout)
  {
    return HAL_TIMEOUT;
  }
  
  return HAL_OK;
}



/*===============================================================================================
*                                       GLOBAL FUNCTIONS
===============================================================================================*/

/**
 * @brief      AT24C32 initialization function
 * @details    AT24C32 initialization function
 * @param[in]  None
 *
 * @return     HAL_StatusTypeDef
 *             HAL_OK - Initialization successful
 *             HAL_ERROR - Initialization failed
 */
HAL_StatusTypeDef AT24C32_Init
(
  void
)
{
  uint8_t index;

  /* Initialize internal variable */
  at24c32_u8DeviceReadyTimeout = AT24C32_I2C_TIMEOUT_MAX;
  
  for (index = (uint8_t)0U; index < AT24C32_BUFFER_SIZE; index++)
  {
    at24c32_u8Data[index] = (uint8_t)0U;
  }
  
  return HAL_OK;
}



/*=============================================================================================*/
/**
 * @brief      AT24C32 write array of bytes function
 * @details    This function will write array of bytes consecutively
 *             into AT24C32
 * @param[in]  uint16_t   address      Address of first written byte
 *             uint8_t*   data         Pointer to array of bytes
 *             uint8_t    length       Number of byte data to be written
 *
 * @return     HAL_StatusTypeDef
 *             HAL_OK - Write successful
 *             HAL_ERROR - Write failed
 */
HAL_StatusTypeDef AT24C32_WriteByte
(
  uint16_t     address,
  uint8_t*     data,
  uint8_t      length
)
{
  HAL_StatusTypeDef ret = HAL_OK;
  
  if (HAL_OK != AT24C32_IsDeviceReady())
  {
    return HAL_ERROR;
  }
  
  ret = HAL_I2C_Mem_Write(&AT24C32_I2C_INSTANCE, AT24C32_SLAVE_ADDRESS, address, I2C_MEMADD_SIZE_16BIT, data, (uint16_t)length, HAL_MAX_DELAY);
  
  return ret;
}



/*=============================================================================================*/
/**
 * @brief      AT24C32 read array of bytes function
 * @details    This function will read array of bytes consecutively
 *             from AT24C32
 * @param[in]  uint16_t   address      Address of first read byte
 *             uint8_t*   data         Pointer to storing array of bytes
 *             uint8_t    length       Number of byte data to be read
 *
 * @return     HAL_StatusTypeDef
 *             HAL_OK - Read successful
 *             HAL_ERROR - Read failed
 */
HAL_StatusTypeDef AT24C32_ReadByte
(
  uint16_t     address,
  uint8_t*     data,
  uint8_t      length
)
{
  HAL_StatusTypeDef ret = HAL_OK;

  if (HAL_OK != AT24C32_IsDeviceReady())
  {
    return HAL_ERROR;
  }

  ret = HAL_I2C_Mem_Read(&AT24C32_I2C_INSTANCE, AT24C32_SLAVE_ADDRESS, address, I2C_MEMADD_SIZE_16BIT, data, (uint16_t)length, HAL_MAX_DELAY);
  
  return ret;
}



/*=============================================================================================*/
/**
 * @brief      AT24C32 write array of integers (half-word) function
 * @details    This function will write array of integers consecutively
 *             into AT24C32
 * @param[in]  uint16_t   address      Address of first written integer
 *             uint16_t*   data        Pointer to array of integers
 *             uint8_t    length       Number of integer data to be written
 *
 * @return     HAL_StatusTypeDef
 *             HAL_OK - Write successful
 *             HAL_ERROR - Write failed
 */
HAL_StatusTypeDef AT24C32_WriteInteger
(
  uint16_t address,
  uint16_t* data,
  uint8_t length
)
{
  HAL_StatusTypeDef ret = HAL_OK;
  uint8_t index, data_index;

  data_index = (uint8_t)0U;

  for (index = (uint8_t)0U; index < length; index++)
  {
    at24c32_u8Data[data_index++] = (uint8_t)((uint16_t)(*(data + index) & (uint16_t)0xFF00U) >> 8U);
    at24c32_u8Data[data_index++] = (uint8_t)((uint16_t)(*(data + index) & (uint16_t)0x00FFU));
  }
  
  if (HAL_OK != AT24C32_IsDeviceReady())
  {
    return HAL_ERROR;
  }

  ret = HAL_I2C_Mem_Write(&AT24C32_I2C_INSTANCE, AT24C32_SLAVE_ADDRESS, address, I2C_MEMADD_SIZE_16BIT, at24c32_u8Data, (uint16_t)data_index, HAL_MAX_DELAY);
  
  return ret;
}



/*=============================================================================================*/
/**
 * @brief      AT24C32 read array of integers (half-word) function
 * @details    This function will read array of integers consecutively
 *             from AT24C32
 * @param[in]  uint16_t   address      Address of first read integer
 *             uint16_t*  data         Pointer to storing array of integers
 *             uint8_t    length       Number of integer data to be read
 *
 * @return     HAL_StatusTypeDef
 *             HAL_OK - Read successful
 *             HAL_ERROR - Read failed
 */
HAL_StatusTypeDef AT24C32_ReadInteger
(
  uint16_t address,
  uint16_t* data,
  uint8_t length
)
{
  HAL_StatusTypeDef ret = HAL_OK;
  uint8_t index, data_index;

  data_index = (uint8_t)(length * (uint8_t)2U);
  
  if (HAL_OK != AT24C32_IsDeviceReady())
  {
    return HAL_ERROR;
  }

  ret = HAL_I2C_Mem_Read(&AT24C32_I2C_INSTANCE, AT24C32_SLAVE_ADDRESS, address, I2C_MEMADD_SIZE_16BIT, at24c32_u8Data, (uint16_t)data_index, HAL_MAX_DELAY);

  if (ret != HAL_OK)
  {
    return ret;
  }

  data_index = (uint8_t)0U;

  for (index = (uint8_t)0U; index < length; index++)
  {
    *(data + index) = (uint16_t)((uint16_t)(at24c32_u8Data[data_index++]) << 8U);
    *(data + index) |= (uint16_t)(at24c32_u8Data[data_index++]);
  }
  
  return ret;
}



/*=============================================================================================*/
/**
 * @brief      AT24C32 write array of word function
 * @details    This function will write array of words consecutively
 *             into AT24C32
 * @param[in]  uint16_t   address      Address of first written word
 *             uint32_t*  data         Pointer to array of words
 *             uint8_t    length       Number of word data to be written
 *
 * @return     HAL_StatusTypeDef
 *             HAL_OK - Write successful
 *             HAL_ERROR - Write failed
 */
HAL_StatusTypeDef AT24C32_WriteWord
(
  uint16_t address,
  uint32_t* data,
  uint8_t length
)
{
  HAL_StatusTypeDef ret = HAL_OK;
  uint8_t index, data_index;

  data_index = (uint8_t)0U;

  for (index = (uint8_t)0U; index < length; index++)
  {
    at24c32_u8Data[data_index++] = (uint8_t)((uint32_t)(*(data + index) & (uint32_t)0xFF000000U) >> 24U);
    at24c32_u8Data[data_index++] = (uint8_t)((uint32_t)(*(data + index) & (uint32_t)0x00FF0000U) >> 16U);
    at24c32_u8Data[data_index++] = (uint8_t)((uint32_t)(*(data + index) & (uint32_t)0x0000FF00U) >> 8U);
    at24c32_u8Data[data_index++] = (uint8_t)((uint32_t)(*(data + index) & (uint32_t)0x000000FFU));
  }
  
  if (HAL_OK != AT24C32_IsDeviceReady())
  {
    return HAL_ERROR;
  }

  ret = HAL_I2C_Mem_Write(&AT24C32_I2C_INSTANCE, AT24C32_SLAVE_ADDRESS, address, I2C_MEMADD_SIZE_16BIT, at24c32_u8Data, (uint16_t)data_index, HAL_MAX_DELAY);
  
  return ret;
}



/*=============================================================================================*/
/**
 * @brief      AT24C32 read array of word function
 * @details    This function will read array of words consecutively
 *             from AT24C32
 * @param[in]  uint16_t   address      Address of first read word
 *             uint32_t*  data         Pointer to storing array of words
 *             uint8_t    length       Number of word data to be read
 *
 * @return     HAL_StatusTypeDef
 *             HAL_OK - Read successful
 *             HAL_ERROR - Read failed
 */
HAL_StatusTypeDef AT24C32_ReadWord
(
  uint16_t address,
  uint32_t* data,
  uint8_t length
)
{
  HAL_StatusTypeDef ret = HAL_OK;
  uint8_t index, data_index;

  data_index = (uint8_t)(length * (uint8_t)4U);
  
  if (HAL_OK != AT24C32_IsDeviceReady())
  {
    return HAL_ERROR;
  }

  ret = HAL_I2C_Mem_Read(&AT24C32_I2C_INSTANCE, AT24C32_SLAVE_ADDRESS, address, I2C_MEMADD_SIZE_16BIT, at24c32_u8Data, (uint16_t)data_index, HAL_MAX_DELAY);

  if (ret != HAL_OK)
  {
    return ret;
  }

  data_index = (uint8_t)0U;

  for (index = (uint8_t)0U; index < length; index++)
  {
    *(data + index)  = (uint32_t)((uint32_t)(at24c32_u8Data[data_index++]) << 24U);
    *(data + index) |= (uint32_t)((uint32_t)(at24c32_u8Data[data_index++]) << 16U);
    *(data + index) |= (uint32_t)((uint32_t)(at24c32_u8Data[data_index++]) << 8U);
    *(data + index) |= (uint32_t)(at24c32_u8Data[data_index++]);
  }
  
  return ret;
}



#ifdef __cplusplus
}
#endif
