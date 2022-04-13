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
#include "main.h"
#include "rtc.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include "debug.h"
#include "globals.h"
#include "six_step.h"
#include <stdbool.h>

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
int __io_putchar(int ch)
{
    if (ch == '\n') {
        uint8_t ch2 = '\r';
        HAL_UART_Transmit(&huart2, &ch2, 1, HAL_MAX_DELAY);
    }

    HAL_UART_Transmit(&huart2, (uint8_t*)&ch, 1, HAL_MAX_DELAY);
    return 1;
}
/*bool is_button_pressed(void)
{
  if (HAL_GPIO_ReadPin(B1_GPIO_Port, B1_Pin) == GPIO_PIN_RESET) {
    return true;
  } else {
    return false;
  }
}*/
#define LINE_MAX_LENGTH	80

static char line_buffer[LINE_MAX_LENGTH + 1];
static uint32_t line_length;

void line_append(uint8_t value)
{
	if (value == '\r' || value == '\n') {
		// odebraliśmy znak końca linii
		if (line_length > 0) {
			// jeśli bufor nie jest pusty to dodajemy 0 na końcu linii
			line_buffer[line_length] = '\0';
			// przetwarzamy dane
			//printf("Otrzymano: %s\n", line_buffer);
			if (strcmp(line_buffer, "on") == 0) {
				HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_SET);
				printf("LD2 została włączona\n");
			} else if (strcmp(line_buffer, "off") == 0) {
				HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);
				printf("LD2 została wyłączona\n");
			} else if (strcmp(line_buffer, "hall1") == 0) {
				uint8_t val_H1 = HAL_GPIO_ReadPin(H1_GPIO_Port, H1_Pin);
				printf("Value of hal 1 sensor %d \n",val_H1);
			} else {
				printf("Nieznane polecenie: %s\n", line_buffer);
			}
			// zaczynamy zbieranie danych od nowa
			line_length = 0;
		}
	}
	else {
		if (line_length >= LINE_MAX_LENGTH) {
			// za dużo danych, usuwamy wszystko co odebraliśmy dotychczas
			line_length = 0;
		}
		// dopisujemy wartość do bufora
		line_buffer[line_length++] = value;
	}
}
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */
  void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
	  HAL_GPIO_WritePin(B1_GPIO_Port, B1_Pin, HAL_GPIO_ReadPin(LD3_GPIO_Port, LD3_Pin));
  }
  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART2_UART_Init();
  MX_TIM7_Init();
  MX_TIM3_Init();
  MX_TIM1_Init();
  MX_RTC_Init();
  /* USER CODE BEGIN 2 */

  // welcome message
  HAL_UART_Transmit(&huart2, (uint8_t *)WELCOME_MSG, strlen(WELCOME_MSG), HAL_MAX_DELAY);

  // init globals
//  glob_state = 1;
//  SS_Commutate_Type5(glob_state);
  uint8_t val_HS;
  uint8_t val_H1 = HAL_GPIO_ReadPin(H1_GPIO_Port, H1_Pin);
  uint8_t val_H2 = HAL_GPIO_ReadPin(H2_GPIO_Port, H2_Pin);
  uint8_t val_H3 = HAL_GPIO_ReadPin(H3_GPIO_Port, H3_Pin);

  uint8_t val_HS_OLD;
  uint8_t counter = 0;
  uint8_t revolution;
  uint8_t delay=5;

  bool refresh = false;

  HAL_SYSTICK_Callback();
  // start timers
//  HAL_TIM_Base_Start_IT(&htim6);
  HAL_TIM_Base_Start_IT(&htim7);

  RTC_DateTypeDef today;
  today.Year = 22;
  today.Month = 03;
  today.Date = 24;
  today.WeekDay = RTC_WEEKDAY_TUESDAY;
  HAL_RTC_SetDate(&hrtc, &today, RTC_FORMAT_BIN);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
#define SIMPLE_TEST

  while (1)
  {
	  //printf("systick = %lu\n", HAL_GetTick());
	  /*HAL_StatusTypeDef HAL_RTC_GetTime(RTC_HandleTypeDef *hrtc, RTC_TimeTypeDef *sTime, uint32_t Format);
	  HAL_StatusTypeDef HAL_RTC_GetDate(RTC_HandleTypeDef *hrtc, RTC_DateTypeDef *sDate, uint32_t Format);*/

	/*  RTC_TimeTypeDef time;
	  RTC_DateTypeDef date;

	  if (is_button_pressed()) {
	  	// ważne: nieużywane pola muszą być wyzerowane
	  	RTC_TimeTypeDef new_time = {0};

	  	// czekamy na zwolnienie przycisku
	  	while (is_button_pressed()) {}

	  	// ustawiamy godzinę 07:45:00
	  	new_time.Hours = 13;
	  	new_time.Minutes = 30;
	  	new_time.Seconds = 0;
	  	HAL_RTC_SetTime(&hrtc, &new_time, RTC_FORMAT_BIN);
	  }

	  HAL_RTC_GetTime(&hrtc, &time, RTC_FORMAT_BIN);
	  HAL_RTC_GetDate(&hrtc, &date, RTC_FORMAT_BIN);

	  printf("RTC: %04d-%02d-%02d, %02d:%02d:%02d\n", 2000 + date.Year, date.Month, date.Date, time.Hours, time.Minutes, time.Seconds);
	  HAL_Delay(200);*/

	 /* int i;

	  //for (i = 0; i < 10; i++)
		if (is_button_pressed()){
	    HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
	    HAL_Delay(100);
	  }*/
	  HAL_GPIO_WritePin(S1_H_GPIO_Port, S1_H_Pin, GPIO_PIN_RESET);
	  HAL_GPIO_WritePin(S1_L_GPIO_Port, S1_L_Pin, GPIO_PIN_RESET);
	  HAL_GPIO_WritePin(S2_H_GPIO_Port, S2_H_Pin, GPIO_PIN_RESET);
	  HAL_GPIO_WritePin(S2_L_GPIO_Port, S2_L_Pin, GPIO_PIN_RESET);
	  HAL_GPIO_WritePin(S3_H_GPIO_Port, S3_H_Pin, GPIO_PIN_RESET);
	  HAL_GPIO_WritePin(S3_L_GPIO_Port, S3_L_Pin, GPIO_PIN_RESET);
	  //HAL_Delay(100);
	/*  if(val_HS>3)
		  val_HS=val_HS-3;
*/
	  val_H1 = HAL_GPIO_ReadPin(H1_GPIO_Port, H1_Pin);
	  val_H2 = HAL_GPIO_ReadPin(H2_GPIO_Port, H2_Pin);
	  val_H3 = HAL_GPIO_ReadPin(H3_GPIO_Port, H3_Pin);
	  val_HS = HS_Calculate_State();
	  if(val_HS == 6 && val_HS != val_HS_OLD && val_HS != val_HS_OLD-1)
	  {
		  counter++;
	  		  }
	  if(val_HS != val_HS_OLD && val_HS != val_HS_OLD-1){
		  refresh = true;
	  }
	  switch (val_HS)
	  {
	  case 1:
	  case 2:
		  HAL_GPIO_WritePin(S2_H_GPIO_Port, S2_H_Pin, GPIO_PIN_SET); //do HS 2 od HS 6,1 i 3,4 // 5 opposite
		  HAL_Delay(delay);
		  HAL_GPIO_WritePin(S2_L_GPIO_Port, S2_L_Pin, GPIO_PIN_SET);
		  HAL_Delay(delay);
	 case 3:
	 case 4:
		  HAL_GPIO_WritePin(S1_H_GPIO_Port, S1_H_Pin, GPIO_PIN_SET); //do HS 4 od HS 5,6 i 2,3 // 1 opposite
		  HAL_Delay(delay);
		  HAL_GPIO_WritePin(S1_L_GPIO_Port, S1_L_Pin, GPIO_PIN_SET);
		  HAL_Delay(delay);

	 case 5:
		  HAL_GPIO_WritePin(S3_H_GPIO_Port, S3_H_Pin, GPIO_PIN_SET); // do HS 6 od HS 4,5 i 1,2 // 3 opposite
	  	  HAL_Delay(delay);
		  HAL_GPIO_WritePin(S3_L_GPIO_Port, S3_L_Pin, GPIO_PIN_SET);
		  HAL_Delay(delay);
	 case 6:
		  HAL_GPIO_WritePin(S2_H_GPIO_Port, S2_H_Pin, GPIO_PIN_SET); //do HS 2 od HS 6,1 i 3,4 // 5 opposite
		  HAL_Delay(delay);
		  HAL_GPIO_WritePin(S2_L_GPIO_Port, S2_L_Pin, GPIO_PIN_SET);
		  HAL_Delay(delay);
	  //default:
	  }
	  HAL_Delay(delay);


		  if(refresh)
		  {
			  val_HS_OLD = val_HS;
			  revolution = counter/15;
			  printf("Sensor 1: %d   Sensor 2: %d   Sensor3: %d    HS: %d    Counter: %d  Revolution: %d\n ",val_H1,val_H2,val_H3,val_HS, counter, revolution);
			  refresh = false;
		  }


    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
/*
#ifdef SIMPLE_TEST
	  for(uint8_t n = 0; n < 3; n++)
	  {
		  // reset all
		  HAL_GPIO_WritePin(S1_H_GPIO_Port, S1_H_Pin, GPIO_PIN_RESET);
		  HAL_GPIO_WritePin(S1_L_GPIO_Port, S1_L_Pin, GPIO_PIN_RESET);
		  HAL_GPIO_WritePin(S2_H_GPIO_Port, S2_H_Pin, GPIO_PIN_RESET);
		  HAL_GPIO_WritePin(S2_L_GPIO_Port, S2_L_Pin, GPIO_PIN_RESET);
		  HAL_GPIO_WritePin(S3_H_GPIO_Port, S3_H_Pin, GPIO_PIN_RESET);
		  HAL_GPIO_WritePin(S3_L_GPIO_Port, S3_L_Pin, GPIO_PIN_RESET);

		  // then set according to index
		  switch (n)
		  {
			case 0:
				HAL_GPIO_WritePin(S1_H_GPIO_Port, S1_H_Pin, GPIO_PIN_SET);
				HAL_GPIO_WritePin(S1_L_GPIO_Port, S1_L_Pin, GPIO_PIN_SET);
				break;
			case 1:
				HAL_GPIO_WritePin(S2_H_GPIO_Port, S2_H_Pin, GPIO_PIN_SET);
				HAL_GPIO_WritePin(S2_L_GPIO_Port, S2_L_Pin, GPIO_PIN_SET);
				break;
			case 2:
				HAL_GPIO_WritePin(S3_H_GPIO_Port, S3_H_Pin, GPIO_PIN_SET);
				HAL_GPIO_WritePin(S3_L_GPIO_Port, S3_L_Pin, GPIO_PIN_SET);
				break;
			default:
				break;
		  }

		  HAL_Delay(1);
	  }
#endif
*/
 }
//  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE|RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 50;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
   //User can add his own implementation to report the HAL error return state

  /* USER CODE END Error_Handler_Debug */
}

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
