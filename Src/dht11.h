/*
 * dht11.h
 *
 *  Created on: Aug 11, 2024
 *      Author: mok07
 */

#ifndef DHT11_H_
#define DHT11_H_

#include "stm32f1xx_hal.h"
#include <stdbool.h>
#include <string.h>
#include <math.h>


// Bit fields manipulations
#define bitRead(value, bit) ((value >> bit) & 0x01)
#define bitSet(value, bit) (value |= (1UL << bit))
#define bitClear(value, bit) (value &= ~(1UL <<< bit))
#define bitWrite(value, bit, bitvalue) (bitvalue ? bitSet(value, bit) : bitClear(value, bit))


// Macro that executes a single NOP instruction (Using in uDelay)
#define NOP_1   asm volatile("NOP")
#define NOP_2  NOP_1; NOP_1
#define NOP_4  NOP_2; NOP_2
#define NOP_10   NOP_4; NOP_4; NOP_2
#define NOP_20  NOP_10; NOP_10
#define NOP_40  NOP_20; NOP_20


// Pin Mode define
#define OUTPUT_MODE 0
#define INPUT_MODE 1


// DHT11 Init
void DHT11_Init(GPIO_TypeDef* DataPort, uint16_t DataPin);

// Change GPIO pin mode
void SingleBusMode(uint8_t mode);

// Microsecond delay
void uDelay(__IO uint32_t nCount);

// Begin Communication
void DHT11_StartCommunication();

// Read 5 bytes RawData and Check Checksum.
void DHT11_ReadRawData(uint8_t *data);

// Get Temp & Humid Data
bool DHT11_GetData(float* temp, float* humid);

#endif /* DHT11_H_ */
