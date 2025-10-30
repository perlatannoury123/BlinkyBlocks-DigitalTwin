/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include <flash_storage.h>
#include <hwAccelerometer.h>

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stm32f0xx_it.h>
#include <user_code.h>
#include <hwLED.h>

// New with lib
#include <i2c.h>
#include <initialization.h>
#include <layer2.h>
#include <layer3_generic.h>
#include <microphone.h>
#include <serial.h>
#include <speaker.h>
#include <sublayer12.h>
#include <timers.h>
#include <usart.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint32_t time_to_jump;
uint32_t address_to_jump = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/

/* USER CODE BEGIN PFP */
__weak uint8_t process_standard_packet(L3_packet *p) {
	// Process packets addressed to the application
	return 0;
}

__weak void process_standard_ack(L3_packet *p) {
}

__weak void process_standard_unack(L3_packet *p) {
}
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
uint8_t must_reboot = 0;
uint32_t reboot_time = 0;
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void) {
	HAL_Init();
	init_application();

  set_standard_packet_handler(process_standard_packet);
  set_standard_ack_handler(process_standard_ack);
  set_standard_unack_handler(process_standard_unack);

  init_sublayer12();
  init_frames_buffer();
  init_L3_packets();
  init_hello_protocol();
  load_configuration();

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  uint32_t start_time = HAL_GetTick();
  uint32_t last_hello_check = start_time;

  while (HAL_GetTick() < start_time + 250) {
	  uint32_t now = HAL_GetTick();
	  if (last_hello_check + 50 < now) {
		  check_last_sent();
		  last_hello_check = now;
	  }
  }

  BBinit();
  while (1) {
	  uint32_t now = HAL_GetTick();

	  check_pending_frames();
	  check_reemitted_frames();

	  if (last_hello_check + 50 < now) {
		  check_last_sent();
		  last_hello_check = now;
	  }

	  BBloop();

	  // Reboot if required
	  if (must_reboot && now >= reboot_time)
		  NVIC_SystemReset();


    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}


/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */

/* USER CODE BEGIN 4 */


/* USER CODE END 4 */

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
