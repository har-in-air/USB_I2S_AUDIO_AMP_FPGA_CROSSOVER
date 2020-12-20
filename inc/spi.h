/**
  ******************************************************************************
  * File Name          : SPI.h
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
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __spi_H
#define __spi_H
#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

#define FPGA_CS_PORT_ENABLE()	__HAL_RCC_GPIOA_CLK_ENABLE()
#define FPGA_CS_PORT		GPIOA
#define FPGA_CS_PIN         GPIO_PIN_4

extern SPI_HandleTypeDef hspi1;

void spi_init(void);
void spi_deselect(void);
void spi_select(void);
uint8_t	spi_xmit_rcv(uint8_t data);

#ifdef __cplusplus
}
#endif
#endif /*__ spi_H */


