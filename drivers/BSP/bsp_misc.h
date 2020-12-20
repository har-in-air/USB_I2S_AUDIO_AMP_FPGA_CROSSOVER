#ifndef __BSP_MISC_H
#define __BSP_MISC_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "stm32f4xx_hal.h"

typedef enum {
 LED_RED = 0,
 LED_GREEN,
 LED_BLUE
} Led_TypeDef;


extern volatile uint32_t BtnPressed; // PA0

// 3 Leds are connected to MCU directly on PE10, PE11, PE12
#define LED_GPIO_PORT                   GPIOE

#define LED_GPIO_CLK_ENABLE()           __HAL_RCC_GPIOE_CLK_ENABLE()

#define LED_RED_PIN                      GPIO_PIN_10
#define LED_GREEN_PIN                    GPIO_PIN_11
#define LED_BLUE_PIN                     GPIO_PIN_12

#define ONBOARD_LED_PORT				GPIOA
#define ONBOARD_LED_PIN					GPIO_PIN_1
#define ONBOARD_LED_GPIO_CLK_ENABLE()   __HAL_RCC_GPIOA_CLK_ENABLE()


void bsp_init(void);

void BSP_LED_Init(void);
void BSP_LED_DeInit(void);
void BSP_LED_On(Led_TypeDef Led);
void BSP_LED_Off(Led_TypeDef Led);
void BSP_LED_Toggle(Led_TypeDef Led);
void BSP_PB_Init(void);
void BSP_OnboardLED_On(void);
void BSP_OnboardLED_Off(void);
void BSP_OnboardLED_Toggle(void);
uint32_t BSP_PB_GetState(void);

#ifdef __cplusplus
}
#endif

#endif 


