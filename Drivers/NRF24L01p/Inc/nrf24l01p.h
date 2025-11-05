#ifndef NRF24L01P_INC_NRF24L01P_H_
#define NRF24L01P_INC_NRF24L01P_H_

// Libraries to be used
#include "stm32f4xx_hal.h"  // HAL
#include "stm32f407xx.h"    //CMSIS
#include <stdint.h>


// GPIO setup
#define NRF24_SPI1_PORT 	GPIOA
#define NRF24_SPI1_SCLK_PIN	GPIO_PIN_5
#define NRF24_SPI1_MISO_PIN	GPIO_PIN_6
#define NRF24_SPI1_MOSI_PIN	GPIO_PIN_7

#define NRF24_CE_PORT 	GPIOC
#define NRF24_CE_PIN    GPIO_PIN_4

#define NRF24_NSS_PORT 	GPIOC
#define NRF24_NSS_PIN 	GPIO_PIN_5

#define NRF24_IRQ_PORT 	GPIOB
#define NRF24_IRQ_PIN 	GPIO_PIN_0


// SPI1 Handler
extern SPI_HandleTypeDef hspi1;
#define NRF24_SPI_HANDLER hspi1


#endif // NRF24L01P_INC_NRF24L01P_H_
