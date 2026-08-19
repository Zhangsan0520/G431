/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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
#include "main.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "drv8701.h"
#include "encoder.h"
#include "motor_control.h"
#include "serial.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define MOTOR_MAX_SPEED_COUNTS 2930
#define MOTOR_TARGET_SPEED_PERCENT 50
#define MOTOR_TARGET_SPEED_COUNTS ((MOTOR_MAX_SPEED_COUNTS * MOTOR_TARGET_SPEED_PERCENT) / 100)
#define MOTOR_PID_KP 0.2f
#define MOTOR_PID_KI 0.3f
#define MOTOR_PID_KD 0.0f
#define MOTOR_DUTY_LIMIT ((DRV8701_DUTY_MAX * 20) / 100)

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
static volatile uint8_t motor_plot_ready = 0U;

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

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_TIM2_Init();
  MX_TIM1_Init();
  MX_TIM3_Init();
  MX_TIM4_Init();
  MX_TIM6_Init();
  MX_TIM8_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */
  if (MotorControl_Init() != HAL_OK)
  {
    Error_Handler();
  }

  MotorControl_SetPID(MOTOR_CONTROL_A, MOTOR_PID_KP, MOTOR_PID_KI, MOTOR_PID_KD);
  MotorControl_SetPID(MOTOR_CONTROL_B, MOTOR_PID_KP, MOTOR_PID_KI, MOTOR_PID_KD);
  MotorControl_SetPID(MOTOR_CONTROL_C, MOTOR_PID_KP, MOTOR_PID_KI, MOTOR_PID_KD);
  MotorControl_SetPID(MOTOR_CONTROL_D, MOTOR_PID_KP, MOTOR_PID_KI, MOTOR_PID_KD);
  MotorControl_SetDutyLimitAll(MOTOR_DUTY_LIMIT);
  MotorControl_SetTargetAll(MOTOR_TARGET_SPEED_COUNTS,
                            MOTOR_TARGET_SPEED_COUNTS,
                            MOTOR_TARGET_SPEED_COUNTS,
                            MOTOR_TARGET_SPEED_COUNTS);
  MotorControl_EnableAll();

  if (HAL_TIM_Base_Start_IT(&htim6) != HAL_OK)
  {
    Error_Handler();
  }

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    if (motor_plot_ready != 0U)
    {
      int32_t target[4];
      int32_t actual[4];
      int32_t duty[4];

      motor_plot_ready = 0U;
      MotorControl_GetSerialPlotData(target, actual, duty);
      (void)Serial_Printf("d:%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld,%lu,%lu,%lu,%lu,%u,%u,%u,%u,%u,%u,%u,%u\r\n",
                          (long)target[0], (long)actual[0], (long)duty[0],
                          (long)target[1], (long)actual[1], (long)duty[1],
                          (long)target[2], (long)actual[2], (long)duty[2],
                          (long)target[3], (long)actual[3], (long)duty[3],
                          (unsigned long)Encoder_GetRawCount(ENCODER_MOTOR_A),
                          (unsigned long)Encoder_GetRawCount(ENCODER_MOTOR_B),
                          (unsigned long)Encoder_GetRawCount(ENCODER_MOTOR_C),
                          (unsigned long)Encoder_GetRawCount(ENCODER_MOTOR_D),
                          (unsigned int)HAL_GPIO_ReadPin(MA_ENC_A_GPIO_Port, MA_ENC_A_Pin),
                          (unsigned int)HAL_GPIO_ReadPin(MA_ENC_B_GPIO_Port, MA_ENC_B_Pin),
                          (unsigned int)HAL_GPIO_ReadPin(MB_ENC_A_GPIO_Port, MB_ENC_A_Pin),
                          (unsigned int)HAL_GPIO_ReadPin(MB_ENC_B_GPIO_Port, MB_ENC_B_Pin),
                          (unsigned int)HAL_GPIO_ReadPin(MC_ENC_A_GPIO_Port, MC_ENC_A_Pin),
                          (unsigned int)HAL_GPIO_ReadPin(MC_ENC_B_GPIO_Port, MC_ENC_B_Pin),
                          (unsigned int)HAL_GPIO_ReadPin(MD_ENC_A_GPIO_Port, MD_ENC_A_Pin),
                          (unsigned int)HAL_GPIO_ReadPin(MD_ENC_B_GPIO_Port, MD_ENC_B_Pin));
    }
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

  /** Configure the main internal regulator output voltage
  */
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1_BOOST);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV4;
  RCC_OscInitStruct.PLL.PLLN = 85;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if (htim->Instance == TIM6)
  {
    MotorControl_Update();
    motor_plot_ready = 1U;
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
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
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
