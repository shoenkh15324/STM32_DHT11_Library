/*
 * dht11.c
 *
 *  Created on: Aug 11, 2024
 *      Author: mok07
 */

#include "dht11.h"


static GPIO_TypeDef* DHT11_Port;
static uint32_t DHT11_Pin;


void DHT11_Init(GPIO_TypeDef* DataPort, uint16_t DataPin)
{
  DHT11_Port = DataPort;
  DHT11_Pin = DataPin;
}

//change pin mode
void SingleBusMode(uint8_t mode)
{
  GPIO_InitTypeDef GPIO_InitStruct;
  GPIO_InitStruct.Pin = (uint32_t)DHT11_Port;
  GPIO_InitStruct.Mode = DHT11_Pin;
  GPIO_InitStruct.Pull = GPIO_NOPULL;

  if(mode == OUTPUT_MODE)
  {
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  }
  else if(mode == INPUT_MODE)
  {
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  }

  HAL_GPIO_Init(DHT11_Port, &GPIO_InitStruct);
}

//microsecond delay
void uDelay(__IO uint32_t nCount)
{
    if (nCount > 1) {
        uint32_t count = nCount * 8 - 6; // 딜레이 시간 계산
        while (count--); // 딜레이를 위해 루프를 돔
    } else {
        uint32_t count = 2; // 최소 딜레이 시간을 설정
        while (count--); // 딜레이를 위해 루프를 돔
    }
}

//Begin Communication
void DHT11_StartCommunication()
{
  SingleBusMode(OUTPUT_MODE);
  HAL_GPIO_WritePin(DHT11_Port, DHT11_Pin, GPIO_PIN_RESET);
  HAL_Delay(20);
  HAL_GPIO_WritePin(DHT11_Port, DHT11_Pin, GPIO_PIN_SET);
  SingleBusMode(INPUT_MODE);
}

// Check DHT11 Response
void DHT11_Response()
{
  uDelay(40);
  // DHT sends out response signal. (pin low)
  while(1 == HAL_GPIO_ReadPin(DHT11_Port, DHT11_Pin));

  uDelay(40);
  // DHT pulls up. (pin high)
  while(0 == HAL_GPIO_ReadPin(DHT11_Port, DHT11_Pin));

  // Start data transmission. (pin low)
  while(1 == HAL_GPIO_ReadPin(DHT11_Port, DHT11_Pin));
}

// Read 5 bytes Raw Data
void DHT11_ReadRawData(uint8_t *data)
{
  uint32_t rawBits = 0;
  uint8_t checksumBits = 0;

  uDelay(50);

  // Get raw temp, humid data.
  for(int8_t i=31; i>=0; i--)
  {
    while(0 == HAL_GPIO_ReadPin(DHT11_Port, DHT11_Pin)); // pin high
    uDelay(40);

    if(0 == HAL_GPIO_ReadPin(DHT11_Port, DHT11_Pin)) // if pin is low
    {
      rawBits &= (1UL << i); // write 0
    }
    else
    {
      rawBits |= (1UL << i); // write 1
    }
  }

  // Get raw checksum data.
  for(int8_t i=7; i>=0; i--)
  {
    while(0 == HAL_GPIO_ReadPin(DHT11_Port, DHT11_Pin)); // pin high
    uDelay(40);

    if(0 == HAL_GPIO_ReadPin(DHT11_Port, DHT11_Pin))
    {
      checksumBits &= (1UL << i); // write 0
    }
    else
    {
      checksumBits |= (1UL << i); // write 1
    }
  }

  // Copy raw data to array of bytes
  data[0] = (rawBits>>24) & 0xFF;
  data[1] = (rawBits>>16) & 0xFF;
  data[2] = (rawBits>>8) & 0xFF;
  data[3] = (rawBits>>0) & 0xFF;
  data[4] = (checksumBits) & 0xFF;
}

//Get Temp & Humid Data
bool DHT11_GetData(float* temp, float* humid)
{
  uint8_t dataArray[6], myChecksum;
  uint16_t temp16, humid16;

  DHT11_StartCommunication();
  DHT11_Response();
  DHT11_ReadRawData(dataArray);
  myChecksum = 0;

  for(uint8_t i=0; i<4; i++)
  {
    myChecksum += dataArray[i];
  }

  if(myChecksum == dataArray[4])
  {
    temp16 = (dataArray[2] << 8) | dataArray[3];
    humid16 = (dataArray[0] << 8) | dataArray[1];

    *temp = temp16 / 10.0f;
    *humid = humid16 / 10.0f;
    return 1;
  }

  return 0;
}
