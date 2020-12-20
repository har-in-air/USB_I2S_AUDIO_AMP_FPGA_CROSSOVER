/**
  ******************************************************************************
  * File Name          : SPI.c
  * Description        : This file provides code for the configuration
  *                      of the SPI instances.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
#include "spi.h"


SPI_HandleTypeDef hspi1;

static void spi_cs_init(void);
static void MX_SPI1_Init(void);

void spi_init(void){
	MX_SPI1_Init();
	spi_cs_init();
	spi_xmit_rcv(0); // dummy transfer
	}


// The fpga spi slave module is configured for idle spi clock = 0,
// and captures the mosi data on the falling edge of spi clock.
// Similarly, the fpga slave sets up the miso data on the rising
// edge of spi clock, and this master (STM32F407) needs to capture
// the miso data on the falling edge of spi clock.

void MX_SPI1_Init(void)
{

  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_2EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_128;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }

}

void HAL_SPI_MspInit(SPI_HandleTypeDef* spiHandle){
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(spiHandle->Instance == SPI1)  {
    __HAL_RCC_SPI1_CLK_ENABLE();
  
    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**SPI1 GPIO Configuration    
    PA5     ------> SPI1_SCK
    PA6     ------> SPI1_MISO
    PA7     ------> SPI1_MOSI 
    */
    GPIO_InitStruct.Pin = GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF5_SPI1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
  }
}

void HAL_SPI_MspDeInit(SPI_HandleTypeDef* spiHandle){
  if(spiHandle->Instance == SPI1)   {
    __HAL_RCC_SPI1_CLK_DISABLE();
  
    /**SPI1 GPIO Configuration    
    PA5     ------> SPI1_SCK
    PA6     ------> SPI1_MISO
    PA7     ------> SPI1_MOSI 
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7);
  }
} 

void spi_cs_init(void){
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	FPGA_CS_PORT_ENABLE();
	HAL_GPIO_WritePin(FPGA_CS_PORT, FPGA_CS_PIN, GPIO_PIN_SET);
	GPIO_InitStruct.Pin = FPGA_CS_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
	HAL_GPIO_Init(FPGA_CS_PORT, &GPIO_InitStruct);
	spi_deselect();
	}


uint8_t	spi_xmit_rcv(uint8_t data){
	uint8_t	ret;
	HAL_SPI_TransmitReceive(&hspi1, &data, &ret, 1,100);
	return ret;
}

void spi_select(void){
	HAL_GPIO_WritePin(FPGA_CS_PORT, FPGA_CS_PIN, GPIO_PIN_RESET);
	}

void spi_deselect(void){
	HAL_GPIO_WritePin(FPGA_CS_PORT, FPGA_CS_PIN, GPIO_PIN_SET);
	}
