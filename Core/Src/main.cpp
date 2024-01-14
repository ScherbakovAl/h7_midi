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

#include <midi_keyboard.h>
#include <main.h>
#include <usb_device.h>

TIM_HandleTypeDef htim2;

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM2_Init(void);

extern "C" void Error_Handler();

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

int main(void) {
	SCB_EnableICache();
	SCB_EnableDCache();
	HAL_Init();
	SystemClock_Config();
	MX_GPIO_Init();
	MX_TIM2_Init();
	MX_USB_DEVICE_Init();
	HAL_TIM_Base_Start(&htim2);
	HAL_Delay(10);
	keys.wheel();

	while (1) {
	}
}

void SystemClock_Config(void) {
	RCC_OscInitTypeDef RCC_OscInitStruct = { 0 };
	RCC_ClkInitTypeDef RCC_ClkInitStruct = { 0 };
	HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);
	__HAL_RCC_SYSCFG_CLK_ENABLE();
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE0);
	while (!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {
	}

	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI48
			| RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	RCC_OscInitStruct.HSI48State = RCC_HSI48_ON;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLM = 2;
	RCC_OscInitStruct.PLL.PLLN = 96; //20 == 100mhz, 96 == 480mhz
	RCC_OscInitStruct.PLL.PLLP = 2;
	RCC_OscInitStruct.PLL.PLLQ = 2;
	RCC_OscInitStruct.PLL.PLLR = 2;
	RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_3;
	RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
	RCC_OscInitStruct.PLL.PLLFRACN = 0;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
		Error_Handler();
	}

	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
			| RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2 | RCC_CLOCKTYPE_D3PCLK1
			| RCC_CLOCKTYPE_D1PCLK1;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1; //<<<<<<<<<<<<< 1
	RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2; // 1 or 2 (после проца делитель)
	RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2; // 1 or 2
	RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2; // 1 or 2
	RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2; // 1 or 2
	RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2; // 1 or 2

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK) {
		Error_Handler();
	}
}

static void MX_TIM2_Init(void) {
	TIM_ClockConfigTypeDef sClockSourceConfig = { 0 };
	TIM_MasterConfigTypeDef sMasterConfig = { 0 };
	htim2.Instance = TIM2;
	htim2.Init.Prescaler = 300;
	htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim2.Init.Period = 4294967295;
	htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	if (HAL_TIM_Base_Init(&htim2) != HAL_OK) {
		Error_Handler();
	}
	sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK) {
		Error_Handler();
	}
	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig)
			!= HAL_OK) {
		Error_Handler();
	}

}

static void MX_GPIO_Init(void) {
	GPIO_InitTypeDef GPIO_InitStruct = { 0 };
	__HAL_RCC_GPIOH_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOD_CLK_ENABLE();

	HAL_GPIO_WritePin(GPIOA,
			SH_LD_Pin | CLK_Pin | AND_Pin | AND_OFF_Pin | (uint16_t) 0x0010
					| (uint16_t) 0x0020, //for test
			GPIO_PIN_RESET);

	GPIO_InitStruct.Pin = SH_LD_Pin | CLK_Pin | AND_Pin | AND_OFF_Pin
			| (uint16_t) 0x0010 | (uint16_t) 0x0020; //for test
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_0
			| GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5
			| GPIO_PIN_6 | GPIO_PIN_7;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
	GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

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

//===============================================================================================================================================
// extern "C" extern "C" extern "C" extern "C" extern "C" extern "C" extern "C" extern "C" extern "C" extern "C" extern "C" extern "C" extern "C"
extern "C" {
void EXTI0_IRQHandler(void) {
	EXTI->PR1 = extpr0;
	keys.interrupt(interrupt0);
}
void EXTI1_IRQHandler(void) {
	EXTI->PR1 = extpr1;
	keys.interrupt(interrupt1);
}
void EXTI2_IRQHandler(void) {
	EXTI->PR1 = extpr2;
	keys.interrupt(interrupt2);
}
void EXTI3_IRQHandler(void) {
	EXTI->PR1 = extpr3;
	keys.interrupt(interrupt3);
}
void EXTI4_IRQHandler(void) {
	EXTI->PR1 = extpr4;
	keys.interrupt(interrupt4);
}
void EXTI9_5_IRQHandler(void) {
	if ((EXTI->PR1 & EXTI_PR1_PR5) == EXTI_PR1_PR5) {
		EXTI->PR1 = extpr5;
		keys.interrupt(interrupt5);
	}
	if ((EXTI->PR1 & EXTI_PR1_PR6) == EXTI_PR1_PR6) {
		EXTI->PR1 = extpr6;
		keys.interrupt(interrupt6);
	}
	if ((EXTI->PR1 & EXTI_PR1_PR7) == EXTI_PR1_PR7) {
		EXTI->PR1 = extpr7;
		keys.interrupt(interrupt7);
	}
	if ((EXTI->PR1 & EXTI_PR1_PR8) == EXTI_PR1_PR8) {
		EXTI->PR1 = extpr8;
		keys.interrupt(interrupt8);
	}
	if ((EXTI->PR1 & EXTI_PR1_PR9) == EXTI_PR1_PR9) {
		EXTI->PR1 = extpr9;
		keys.interrupt(interrupt9);
	}
}

void EXTI15_10_IRQHandler(void) {
	if ((EXTI->PR1 & EXTI_PR1_PR10) == EXTI_PR1_PR10) {
		EXTI->PR1 = extpr10;
		keys.interrupt(interrupt10);
	}
}
extern PCD_HandleTypeDef hpcd_USB_OTG_FS;
void OTG_FS_IRQHandler(void) {
	HAL_PCD_IRQHandler(&hpcd_USB_OTG_FS);
}
} // extern "C"

// extern "C" extern "C" extern "C" extern "C" extern "C" extern "C" extern "C" extern "C" extern "C" extern "C" extern "C" extern "C" extern "C"
//===============================================================================================================================================

void Error_Handler(void) {
	__disable_irq();
	while (1) {
	}
}
