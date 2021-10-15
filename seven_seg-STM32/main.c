/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdbool.h>

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* defining digits as bytes for 7SEG */

#define _0 0b00111111
#define _1 0b00000110
#define _2 0b01011011
#define _3 0b01001111
#define _4 0b01100110
#define _5 0b01101101
#define _6 0b01111101
#define _7 0b00000111
#define _8 0b01111111
#define _9 0b01101111
#define _D 0b00000001
#define CL 0b00000000


/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

_Bool int_flag = false;
_Bool is_stopped = false;

int pins[] = {
                pin_a_Pin,
                pin_b_Pin,
                pin_c_Pin,
                pin_d_Pin,
                pin_e_Pin,
                pin_f_Pin,
                pin_g_Pin,
                pin_dp_Pin
};

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
/* USER CODE BEGIN PFP */

void set_digit(int digit);
void clear();
void test_segs(int mills);
void write_byte(uint8_t b);
void animate(int mills);
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

        int counter = 0;

        /* USER CODE END 1 */

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

        /* USER CODE BEGIN 2 */
        /* USER CODE END 2 */

        /* Infinite loop */
        /* USER CODE BEGIN WHILE */
        while (1) {

                if (counter > 6) {
                        counter = 1;
                }

                if (int_flag) {
                        int_flag = !int_flag;
                        is_stopped = !is_stopped;
                        set_digit(counter);
                } else if (int_flag && is_stopped) {
                        int_flag = !int_flag;
                        is_stopped = !is_stopped;
                }
                if (!is_stopped) {animate(50);}
                counter++;

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
        RCC_OscInitTypeDef RCC_OscInitStruct = { 0 };
        RCC_ClkInitTypeDef RCC_ClkInitStruct = { 0 };

        /** Initializes the RCC Oscillators according to the specified parameters
         * in the RCC_OscInitTypeDef structure.
         */
        RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
        RCC_OscInitStruct.HSIState = RCC_HSI_ON;
        RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
        RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
        if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
                Error_Handler();
        }
        /** Initializes the CPU, AHB and APB buses clocks
         */
        RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
                        | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
        RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
        RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
        RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
        RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

        if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0)
                        != HAL_OK) {
                Error_Handler();
        }
}

/**
 * @brief GPIO Initialization Function
 * @param None
 * @retval None
 */
static void MX_GPIO_Init(void)
{
        GPIO_InitTypeDef GPIO_InitStruct = { 0 };

        /* GPIO Ports Clock Enable */
        __HAL_RCC_GPIOC_CLK_ENABLE();
        __HAL_RCC_GPIOA_CLK_ENABLE();
        __HAL_RCC_GPIOB_CLK_ENABLE();

        /*Configure GPIO pin Output Level */
        HAL_GPIO_WritePin(GPIOA,
                        pin_dp_Pin | pin_g_Pin | pin_f_Pin | pin_b_Pin
                                        | pin_c_Pin, GPIO_PIN_RESET);

        /*Configure GPIO pin Output Level */
        HAL_GPIO_WritePin(GPIOB, pin_a_Pin | pin_e_Pin, GPIO_PIN_RESET);

        /*Configure GPIO pin Output Level */
        HAL_GPIO_WritePin(pin_d_GPIO_Port, pin_d_Pin, GPIO_PIN_RESET);

        /*Configure GPIO pin : PC13 */
        GPIO_InitStruct.Pin = GPIO_PIN_13;
        GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

        /*Configure GPIO pins : pin_dp_Pin pin_g_Pin pin_f_Pin pin_b_Pin
         pin_c_Pin */
        GPIO_InitStruct.Pin = pin_dp_Pin | pin_g_Pin | pin_f_Pin | pin_b_Pin
                        | pin_c_Pin;
        GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

        /*Configure GPIO pins : pin_a_Pin pin_e_Pin */
        GPIO_InitStruct.Pin = pin_a_Pin | pin_e_Pin;
        GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
        HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

        /*Configure GPIO pin : pin_d_Pin */
        GPIO_InitStruct.Pin = pin_d_Pin;
        GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
        HAL_GPIO_Init(pin_d_GPIO_Port, &GPIO_InitStruct);

        /* EXTI interrupt init*/
        HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
        HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

}

/* USER CODE BEGIN 4 */

/*
 * print specific digit into 7seg
 * digit must be in range 0-9
 */
void set_digit(int digit)
{
        switch (digit) {
        case 0: write_byte(_0); break;
        case 1: write_byte(_1); break;
        case 2: write_byte(_2); break;
        case 3: write_byte(_3); break;
        case 4: write_byte(_4); break;
        case 5: write_byte(_5); break;
        case 6: write_byte(_6); break;
        case 7: write_byte(_7); break;
        case 8: write_byte(_8); break;
        case 9: write_byte(_9); break;
        default: write_byte(_D); break;
        }
}

/* reset all segments */
void clear()
{
        write_byte(CL);
}

/* test all segments */
void test_segs(int mills)
{
        HAL_Delay(mills);
        HAL_GPIO_WritePin(GPIOB, pin_a_Pin, GPIO_PIN_SET); // 0
        HAL_Delay(mills);
        clear();
        HAL_GPIO_WritePin(GPIOA, pin_b_Pin, GPIO_PIN_SET); // 1
        HAL_Delay(mills);
        clear();
        HAL_GPIO_WritePin(GPIOA, pin_c_Pin, GPIO_PIN_SET); // 2
        HAL_Delay(mills);
        clear();
        HAL_GPIO_WritePin(GPIOC, pin_d_Pin, GPIO_PIN_SET); // 3
        HAL_Delay(mills);
        clear();
        HAL_GPIO_WritePin(GPIOB, pin_e_Pin, GPIO_PIN_SET); // 4
        HAL_Delay(mills);
        clear();
        HAL_GPIO_WritePin(GPIOA, pin_f_Pin, GPIO_PIN_SET); // 5
        HAL_Delay(mills);
        clear();
        HAL_GPIO_WritePin(GPIOA, pin_g_Pin, GPIO_PIN_SET); // 6
        HAL_Delay(mills);
        HAL_GPIO_WritePin(GPIOA, pin_dp_Pin, GPIO_PIN_SET);// 7
        HAL_Delay(mills);
        clear();
}

void animate(int mills)
{
        uint8_t byte = 0b00000001;
        int i = 0;

        for (i = 0; i < 6; ++i) {
                clear();
                write_byte(byte);
                byte <<= 1;
                HAL_Delay(mills);
        }
}

void write_byte(uint8_t b)
{
/* A 1, 2, 5, 6, 7
 * B 0, 4
 * C 3
 */
        int i = 0;
        for (i = 0; i < 8; i++)  {
                if (i == 0 || i == 4) {                                         // B port
                        HAL_GPIO_WritePin(GPIOB, pins[i], b & 1);
                } else if (i == 1 || i == 2 || i == 5 || i == 6 || i == 7) {    // A port
                        HAL_GPIO_WritePin(GPIOA, pins[i], b & 1);
                } else if (i == 3) {                                            // C port
                        HAL_GPIO_WritePin(GPIOC, pins[i], b & 1);
                }
                b >>= 1;
        }
}
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
        while (1) {
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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
