/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include <midi_keyboard.h>
#include "main.h"
#include "usb_device.h"
// git test in "изменение_из_midi_controller............"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
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

TIM_HandleTypeDef htim2;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM2_Init(void);
/* USER CODE BEGIN PFP */
extern "C" void Error_Handler();
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
cuint extpr0 = 1;
cuint extpr1 = 2;
cuint extpr2 = 4;
cuint extpr3 = 8;
cuint extpr4 = 16;
cuint extpr5 = 32;
cuint extpr6 = 64;
cuint extpr7 = 128;
cuint extpr8 = 256;
cuint extpr9 = 512;
cuint extpr10 = 1024;

cuint interrupt0 = 0;
cuint interrupt1 = 1;
cuint interrupt2 = 2;
cuint interrupt3 = 3;
cuint interrupt4 = 4;
cuint interrupt5 = 5;
cuint interrupt6 = 6;
cuint interrupt7 = 7;
cuint interrupt8 = 8;
cuint interrupt9 = 9;
cuint interrupt10 = 10;
Keys keys;
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* Enable I-Cache---------------------------------------------------------*/
  SCB_EnableICache();

  /* Enable D-Cache---------------------------------------------------------*/
  SCB_EnableDCache();

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_TIM2_Init();
//  MX_USB_DEVICE_Init();
  /* USER CODE BEGIN 2 */
  HAL_Delay(10);
//  SysTick->CTRL = 0;// HAL_Delay тогда не работает и usb глючит
  HAL_TIM_Base_Start(&htim2);
//  keys.init_bit_mask();
  keys.wheel();
//  gpio_bsrr bsrrr;//for tests

  //  SysTick->CTRL = 0;//!!!!!!!!!!!!!!!!!!!!!!!!
  //  SCB_EnableICache();//!!!!!!!!!!!!!!!!!!!!!!!????????????????????????

    //https://metebalci.com/blog/stm32h7-gpio-toggling/
    // ssss ppppp eeee  ddddd !!!!!
    /*
     *
     * register volatile uint32_t* bsrr = (uint32_t*) (GPIOB_BASE + 24);
  register const uint32_t set = (uint32_t) GPIO_PIN_11;
  register const uint32_t reset = (uint32_t) GPIO_PIN_11 << 16U;

  while (1)
  {
    *bsrr = set;
    *bsrr = reset;
  }
     */

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  //скорость..
	  //RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV64;
	  //или 1 или 2 или 4.. 8 ..16
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
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

  /** Supply configuration update enable
  */
  HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);

  /** Configure the main internal regulator output voltage
  */__HAL_RCC_SYSCFG_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE0);

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI48|RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSI48State = RCC_HSI48_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 2;
  RCC_OscInitStruct.PLL.PLLN = 96;//20 == 100mhz, 96 == 480mhz
  RCC_OscInitStruct.PLL.PLLP = 2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_3;
  RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
  RCC_OscInitStruct.PLL.PLLFRACN = 0;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                              |RCC_CLOCKTYPE_D3PCLK1|RCC_CLOCKTYPE_D1PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;//<<<<<<<<<<<<< 1
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;// 1 or 2 (после проца делитель)
  RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;// 1 or 2
  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;// 1 or 2
  RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;// 1 or 2
  RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2;// 1 or 2

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 999;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 4294967295;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */
  /* USER CODE END MX_GPIO_Init_1 */

    /* GPIO Ports Clock Enable */
    __HAL_RCC_GPIOH_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();

    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(GPIOA, CLK_Pin|AND_OFF_Pin|SH_LD_Pin|AND_Pin, GPIO_PIN_RESET);

    /*Configure GPIO pins : CLK_Pin AND_OFF_Pin SH_LD_Pin AND_Pin */
    GPIO_InitStruct.Pin = CLK_Pin|AND_OFF_Pin|SH_LD_Pin|AND_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /*Configure GPIO pins : PD8 PD9 PD10 PD0
                             PD1 PD2 PD3 PD4
                             PD5 PD6 PD7 */
    GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_0
                            |GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4
                            |GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI0_IRQn, 1, 1);
  HAL_NVIC_EnableIRQ(EXTI0_IRQn);

  HAL_NVIC_SetPriority(EXTI1_IRQn, 1, 1);
  HAL_NVIC_EnableIRQ(EXTI1_IRQn);

  HAL_NVIC_SetPriority(EXTI2_IRQn, 1, 1);
  HAL_NVIC_EnableIRQ(EXTI2_IRQn);

  HAL_NVIC_SetPriority(EXTI3_IRQn, 1, 1);
  HAL_NVIC_EnableIRQ(EXTI3_IRQn);

  HAL_NVIC_SetPriority(EXTI4_IRQn, 1, 1);
  HAL_NVIC_EnableIRQ(EXTI4_IRQn);

  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 1, 1);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 1, 1);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

}

/* USER CODE BEGIN 4 */
//============================================================================================================
// extern "C" extern "C" extern "C" extern "C" extern "C" extern "C" extern "C" extern "C" extern "C" extern "C" extern "C" extern "C" extern "C"
extern "C" {
void EXTI0_IRQHandler(void) {
	EXTI->PR1 = extpr0;
//	if (selector) {
//		Interrupt(0, muxx);
//	} else {
	keys.interrupt(interrupt0);
//	}
}
void EXTI1_IRQHandler(void) {
	EXTI->PR1 = extpr1;
//	if (selector) {
//		Interrupt(1, muxx);
//	} else {
	keys.interrupt(interrupt1);
//	}
}
void EXTI2_IRQHandler(void) {
	EXTI->PR1 = extpr2;
//	Interrupt(2, muxx);
	keys.interrupt(interrupt2);
}
void EXTI3_IRQHandler(void) {
	EXTI->PR1 = extpr3;
//	Interrupt(3, muxx);
	keys.interrupt(interrupt3);
}
void EXTI4_IRQHandler(void) {
	EXTI->PR1 = extpr4;
//	Interrupt(4, muxx);
	keys.interrupt(interrupt4);
}
void EXTI9_5_IRQHandler(void) {
	if ((EXTI->PR1 & EXTI_PR1_PR5) == EXTI_PR1_PR5) {
		EXTI->PR1 = extpr5;
//		Interrupt(5, muxx);
//		keys.interrupt(interrupt5);
	}
	if ((EXTI->PR1 & EXTI_PR1_PR6) == EXTI_PR1_PR6) {
		EXTI->PR1 = extpr6;
//		Interrupt(6, muxx);
//		keys.interrupt(interrupt6);
	}
	if ((EXTI->PR1 & EXTI_PR1_PR7) == EXTI_PR1_PR7) {
		EXTI->PR1 = extpr7;
//		Interrupt(7, muxx);
//		keys.interrupt(interrupt7);
	}
	if ((EXTI->PR1 & EXTI_PR1_PR8) == EXTI_PR1_PR8) {
		EXTI->PR1 = extpr8;
//		Interrupt(8, muxx);
//		keys.interrupt(interrupt8);
	}
	if ((EXTI->PR1 & EXTI_PR1_PR9) == EXTI_PR1_PR9) {
		EXTI->PR1 = extpr9;
//		Interrupt(9, muxx);
//		keys.interrupt(interrupt9);
	}
}
void EXTI15_10_IRQHandler(void) {
	if ((EXTI->PR1 & EXTI_PR1_PR10) == EXTI_PR1_PR10) {
		EXTI->PR1 = extpr10;
//		Interrupt(10, muxx);
//		keys.interrupt(interrupt10);
	}
}
}									//extern "C"
// extern "C" extern "C" extern "C" extern "C" extern "C" extern "C" extern "C" extern "C" extern "C" extern "C" extern "C" extern "C" extern "C"
//============================================================================================================

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
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
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
