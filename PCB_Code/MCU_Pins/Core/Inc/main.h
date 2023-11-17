/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define Extra_PC0_Pin GPIO_PIN_0
#define Extra_PC0_GPIO_Port GPIOC
#define Extra_PC1_Pin GPIO_PIN_1
#define Extra_PC1_GPIO_Port GPIOC
#define Voltage_Divider_Pin GPIO_PIN_0
#define Voltage_Divider_GPIO_Port GPIOA
#define Extra_PA1_Pin GPIO_PIN_1
#define Extra_PA1_GPIO_Port GPIOA
#define Extra_PA2_Pin GPIO_PIN_2
#define Extra_PA2_GPIO_Port GPIOA
#define Extra_PA3_Pin GPIO_PIN_3
#define Extra_PA3_GPIO_Port GPIOA
#define Extra_PA4_Pin GPIO_PIN_4
#define Extra_PA4_GPIO_Port GPIOA
#define Extra_PC4_Pin GPIO_PIN_4
#define Extra_PC4_GPIO_Port GPIOC
#define Extra_PB12_Pin GPIO_PIN_12
#define Extra_PB12_GPIO_Port GPIOB
#define Extra_PB13_Pin GPIO_PIN_13
#define Extra_PB13_GPIO_Port GPIOB
#define Extra_PB14_Pin GPIO_PIN_14
#define Extra_PB14_GPIO_Port GPIOB
#define Extra_PB15_Pin GPIO_PIN_15
#define Extra_PB15_GPIO_Port GPIOB
#define Solar_Panel_Voltage_LED_Pin GPIO_PIN_7
#define Solar_Panel_Voltage_LED_GPIO_Port GPIOC
#define Debug_LED_Pin GPIO_PIN_8
#define Debug_LED_GPIO_Port GPIOC
#define I2C_Light_SDA_Pin GPIO_PIN_9
#define I2C_Light_SDA_GPIO_Port GPIOC
#define I2C_Light_SCL_Pin GPIO_PIN_8
#define I2C_Light_SCL_GPIO_Port GPIOA
#define Bluetooth_RX_Pin GPIO_PIN_9
#define Bluetooth_RX_GPIO_Port GPIOA
#define Bluetooth_TX_Pin GPIO_PIN_10
#define Bluetooth_TX_GPIO_Port GPIOA
#define SWDIO_Pin GPIO_PIN_13
#define SWDIO_GPIO_Port GPIOA
#define SWCLK_Pin GPIO_PIN_14
#define SWCLK_GPIO_Port GPIOA
#define Step_Rod_Logic_Pin GPIO_PIN_10
#define Step_Rod_Logic_GPIO_Port GPIOC
#define Direction_Rod_Logic_Pin GPIO_PIN_11
#define Direction_Rod_Logic_GPIO_Port GPIOC
#define Enable_Rod_Logic_Pin GPIO_PIN_12
#define Enable_Rod_Logic_GPIO_Port GPIOC
#define nSleep_Rod_Logic_Pin GPIO_PIN_2
#define nSleep_Rod_Logic_GPIO_Port GPIOD
#define Extra_PB3_Pin GPIO_PIN_3
#define Extra_PB3_GPIO_Port GPIOB
#define Enable_Susan_Logic_Pin GPIO_PIN_4
#define Enable_Susan_Logic_GPIO_Port GPIOB
#define Step_Susan_Logic_Pin GPIO_PIN_5
#define Step_Susan_Logic_GPIO_Port GPIOB
#define Direction_Susan_Logic_Pin GPIO_PIN_6
#define Direction_Susan_Logic_GPIO_Port GPIOB
#define nSleep_Susan_Logic_Pin GPIO_PIN_7
#define nSleep_Susan_Logic_GPIO_Port GPIOB
#define GPS_and_MAG_I2C_SCL_Pin GPIO_PIN_8
#define GPS_and_MAG_I2C_SCL_GPIO_Port GPIOB
#define GPS_and_MAG_I2C_SDA_Pin GPIO_PIN_9
#define GPS_and_MAG_I2C_SDA_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
