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
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define VIBRATION_THRESHOLD_MG 150
#define CONDITION_WARNING_MG 300
#define CONDITION_CRITICAL_MG 500
#define MPU6050_ADDR        (0x68 << 1)
#define MPU6050_PWR_MGMT_1  0x6B
#define MPU6050_GYRO_CONFIG 0x1B
#define MPU6050_ACCEL_CONFIG 0x1C
#define MPU6050_GYRO_XOUT_H  0x43
#define MPU6050_ACCEL_XOUT_H  0x3B
#define MPU6050_ACCEL_XOUT_L  0x3C
#define MPU6050_ACCEL_YOUT_H  0x3D
#define MPU6050_ACCEL_YOUT_L  0x3E
#define MPU6050_ACCEL_ZOUT_H  0x3F
#define MPU6050_ACCEL_ZOUT_L  0x40

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;

UART_HandleTypeDef huart1;

/* USER CODE BEGIN PV */
int32_t vibration_deviation_mg = 0;
const uint8_t uart_test_message[] =
"\r\n========================================\r\n"
"STM32 EQUIPMENT CONDITION MONITOR V2\r\n"
"[UART] USART1 DIAGNOSTIC TEST\r\n"
"[UART] PASS - TRANSMITTER ACTIVE\r\n"
"========================================\r\n";

int32_t baseline_sum = 0;
int32_t baseline_mag_mg = 0;
uint8_t baseline_ready = 0;
uint8_t baseline_count = 0;

#define VALIDATION_HISTORY_SIZE 10

uint8_t validation_history[VALIDATION_HISTORY_SIZE];
uint8_t validation_history_index = 0;
uint8_t validation_history_count = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_I2C1_Init(void);
/* USER CODE BEGIN PFP */

static void Print_Validation_History(void);

/* USER CODE END PFP */
/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
#define EVENT_SELF_TEST_PASS      1
#define EVENT_SENSOR_VALID        2
#define EVENT_CONDITION_NORMAL    3
#define EVENT_CONDITION_WARNING   4
#define EVENT_CONDITION_CRITICAL  5
static void Add_Validation_History(uint8_t event)
{
    validation_history[validation_history_index] = event;

    validation_history_index++;

    if (validation_history_index >= VALIDATION_HISTORY_SIZE)
    {
        validation_history_index = 0;
    }

    if (validation_history_count < VALIDATION_HISTORY_SIZE)
    {
        validation_history_count++;
    }
}
static void Print_Validation_History(void)
{
    char buffer[80];

    HAL_UART_Transmit(&huart1,
                      (uint8_t *)"\r\n================================\r\n",
                      sizeof("\r\n================================\r\n") - 1,
                      100);

    HAL_UART_Transmit(&huart1,
                      (uint8_t *)"VALIDATION HISTORY\r\n",
                      sizeof("VALIDATION HISTORY\r\n") - 1,
                      100);

    HAL_UART_Transmit(&huart1,
                      (uint8_t *)"================================\r\n",
                      sizeof("================================\r\n") - 1,
                      100);

    uint8_t start_index = 0;

    if (validation_history_count == VALIDATION_HISTORY_SIZE)
    {
        start_index = validation_history_index;
    }

    for (uint8_t i = 0; i < validation_history_count; i++)
    {
        uint8_t index =
            (start_index + i) % VALIDATION_HISTORY_SIZE;

        switch (validation_history[index])
        {
            case EVENT_SELF_TEST_PASS:
                sprintf(buffer, "[%d] SELF-TEST PASS\r\n", i + 1);
                break;

            case EVENT_SENSOR_VALID:
                sprintf(buffer, "[%d] SENSOR VALID\r\n", i + 1);
                break;

            case EVENT_CONDITION_NORMAL:
                sprintf(buffer, "[%d] CONDITION NORMAL\r\n", i + 1);
                break;

            case EVENT_CONDITION_WARNING:
                sprintf(buffer, "[%d] CONDITION WARNING\r\n", i + 1);
                break;

            case EVENT_CONDITION_CRITICAL:
                sprintf(buffer, "[%d] CONDITION CRITICAL\r\n", i + 1);
                break;

            default:
                sprintf(buffer, "[%d] UNKNOWN EVENT\r\n", i + 1);
                break;
        }

        HAL_UART_Transmit(&huart1,
                          (uint8_t *)buffer,
                          strlen(buffer),
                          100);
    }

    HAL_UART_Transmit(&huart1,
                      (uint8_t *)"================================\r\n",
                      sizeof("================================\r\n") - 1,
                      100);
}
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
  MX_USART1_UART_Init();
  MX_I2C1_Init();
  /* USER CODE BEGIN 2 */
  uint8_t sensor_valid = 1;
  uint8_t accel_read_ok = 0;
  uint8_t gyro_read_ok = 0;
  uint8_t self_test_pass = 1;
  uint8_t self_test_accel_ok = 0;
  uint8_t self_test_gyro_ok = 0;
  uint8_t mpu6050_whoami = 0;

  uint8_t sensor_event_recorded = 0;
  uint8_t previous_condition = 0;
  uint8_t current_condition = 0;

  uint8_t mpu6050_data;
  uint8_t accel_data[6];
  uint8_t gyro_data[6];
  int16_t accel_x;
  int16_t accel_y;
  int16_t accel_z;
  int32_t accel_mag_mg;



  /* Read MPU6050 WHO_AM_I register */
  if (HAL_I2C_Mem_Read(&hi2c1,
                       MPU6050_ADDR,
                       0x75,
                       I2C_MEMADD_SIZE_8BIT,
                       &mpu6050_whoami,
                       1,
                       100) != HAL_OK)
  {
      HAL_UART_Transmit(&huart1,
                        (uint8_t *)"[SENSOR] WHO_AM_I READ FAIL\r\n",
                        30,
                        100);
  }
  else if (mpu6050_whoami == 0x68)


  {
      HAL_UART_Transmit(&huart1,
                        (uint8_t *)"MPU6050 I2C PASS - WHO_AM_I = 0x68\r\n",
                        38,
                        100);
  }
  else
  {
      HAL_UART_Transmit(&huart1,
                        (uint8_t *)"MPU6050 I2C FAIL\r\n",
                        19,
                        100);
  }

  /* Wake MPU6050 */
  mpu6050_data = 0x00;

  HAL_I2C_Mem_Write(&hi2c1,
                    MPU6050_ADDR,
                    MPU6050_PWR_MGMT_1,
                    I2C_MEMADD_SIZE_8BIT,
                    &mpu6050_data,
                    1,
                    100);

  /* Accelerometer: ±2g */
  mpu6050_data = 0x00;

  HAL_I2C_Mem_Write(&hi2c1,
                    MPU6050_ADDR,
                    MPU6050_ACCEL_CONFIG,
                    I2C_MEMADD_SIZE_8BIT,
                    &mpu6050_data,
                    1,
                    100);

  /* Gyroscope: ±250 °/s */
  mpu6050_data = 0x00;

  HAL_I2C_Mem_Write(&hi2c1,
                    MPU6050_ADDR,
                    MPU6050_GYRO_CONFIG,
                    I2C_MEMADD_SIZE_8BIT,
                    &mpu6050_data,
                    1,
                    100);

  /* Verify MPU6050 power-management register */
  if (HAL_I2C_Mem_Read(&hi2c1,
                       MPU6050_ADDR,
                       MPU6050_PWR_MGMT_1,
                       I2C_MEMADD_SIZE_8BIT,
                       &mpu6050_data,
                       1,
                       100) != HAL_OK)
  {
      HAL_UART_Transmit(&huart1,
                        (uint8_t *)"[SENSOR] PWR_MGMT READ FAIL\r\n",
                        30,
                        100);
  }
  else if (mpu6050_data == 0x00)
  {
      HAL_UART_Transmit(&huart1,
                        (uint8_t *)"MPU6050 INITIALIZATION PASS\r\n",
                        30,
                        100);
  }
  else
  {
      HAL_UART_Transmit(&huart1,
                        (uint8_t *)"MPU6050 INITIALIZATION FAIL\r\n",
                        30,
                        100);
  }
  /* Integrated MPU6050 functional self-test */

  if (HAL_I2C_Mem_Read(&hi2c1,
                       MPU6050_ADDR,
                       MPU6050_ACCEL_XOUT_H,
                       I2C_MEMADD_SIZE_8BIT,
                       accel_data,
                       6,
                       100) == HAL_OK)
  {
      self_test_accel_ok = 1;
  }
  else
  {
      self_test_pass = 0;
  }

  if (HAL_I2C_Mem_Read(&hi2c1,
                       MPU6050_ADDR,
                       MPU6050_GYRO_XOUT_H,
                       I2C_MEMADD_SIZE_8BIT,
                       gyro_data,
                       6,
                       100) == HAL_OK)
  {
      self_test_gyro_ok = 1;
  }
  else
  {
      self_test_pass = 0;
  }

  /* Report self-test */

  HAL_UART_Transmit(&huart1,
                    (uint8_t *)"\r\n[SELF-TEST] MPU6050 FUNCTIONAL TEST\r\n",
                    sizeof("\r\n[SELF-TEST] MPU6050 FUNCTIONAL TEST\r\n") - 1,
                    100);

  if (self_test_accel_ok)
  {
      HAL_UART_Transmit(&huart1,
                        (uint8_t *)"[SELF-TEST] ACCEL READ : PASS\r\n",
                        sizeof("[SELF-TEST] ACCEL READ : PASS\r\n") - 1,
                        100);
  }
  else
  {
      HAL_UART_Transmit(&huart1,
                        (uint8_t *)"[SELF-TEST] ACCEL READ : FAIL\r\n",
                        sizeof("[SELF-TEST] ACCEL READ : FAIL\r\n") - 1,
                        100);
  }

  if (self_test_gyro_ok)
  {
      HAL_UART_Transmit(&huart1,
                        (uint8_t *)"[SELF-TEST] GYRO READ  : PASS\r\n",
                        sizeof("[SELF-TEST] GYRO READ  : PASS\r\n") - 1,
                        100);
  }
  else
  {
      HAL_UART_Transmit(&huart1,
                        (uint8_t *)"[SELF-TEST] GYRO READ  : FAIL\r\n",
                        sizeof("[SELF-TEST] GYRO READ  : FAIL\r\n") - 1,
                        100);
  }

  if (self_test_pass &&
      self_test_accel_ok &&
      self_test_gyro_ok)
  {
	  Add_Validation_History(EVENT_SELF_TEST_PASS);
	  HAL_UART_Transmit(&huart1,
                        (uint8_t *)"[SELF-TEST] RESULT     : PASS\r\n\r\n",
                        sizeof("[SELF-TEST] RESULT     : PASS\r\n\r\n") - 1,
                        100);
  }
  else
  {
      HAL_UART_Transmit(&huart1,
                        (uint8_t *)"[SELF-TEST] RESULT     : FAIL\r\n\r\n",
                        sizeof("[SELF-TEST] RESULT     : FAIL\r\n\r\n") - 1,
                        100);
  }

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  /* Read accelerometer */
	  if (HAL_I2C_Mem_Read(&hi2c1,
	                       MPU6050_ADDR,
	                       MPU6050_ACCEL_XOUT_H,
	                       I2C_MEMADD_SIZE_8BIT,
	                       accel_data,
	                       6,
	                       100) == HAL_OK)
	  {
	      accel_read_ok = 1;
	  }
	  else
	  {
	      accel_read_ok = 0;

	      HAL_UART_Transmit(&huart1,
	                        (uint8_t *)"[SENSOR] ACCEL READ FAIL\r\n",
	                        sizeof("[SENSOR] ACCEL READ FAIL\r\n") - 1,
	                        100);

	      HAL_Delay(100);
	      continue;
	  }

      accel_x = (int16_t)((accel_data[0] << 8) | accel_data[1]);
      accel_y = (int16_t)((accel_data[2] << 8) | accel_data[3]);
      accel_z = (int16_t)((accel_data[4] << 8) | accel_data[5]);

      /* Read gyroscope */
      if (HAL_I2C_Mem_Read(&hi2c1,
                           MPU6050_ADDR,
                           MPU6050_GYRO_XOUT_H,
                           I2C_MEMADD_SIZE_8BIT,
                           gyro_data,
                           6,
                           100) == HAL_OK)
      {
          gyro_read_ok = 1;
      }
      else
      {
          gyro_read_ok = 0;

          HAL_UART_Transmit(&huart1,
                            (uint8_t *)"[SENSOR] GYRO READ FAIL\r\n",
                            sizeof("[SENSOR] GYRO READ FAIL\r\n") - 1,
                            100);

          HAL_Delay(100);
          continue;
      }
      if (accel_read_ok && gyro_read_ok)
      {
          sensor_valid = 1;

          if (!sensor_event_recorded)
          {
              Add_Validation_History(EVENT_SENSOR_VALID);
              sensor_event_recorded = 1;
          }
      }
      else
      {
          sensor_valid = 0;

          HAL_UART_Transmit(&huart1,
                            (uint8_t *)"[SENSOR] INVALID - I2C READ ERROR\r\n",
                            sizeof("[SENSOR] INVALID - I2C READ ERROR\r\n") - 1,
                            100);

          HAL_Delay(100);
          continue;
      }



      int32_t ax_mg;
      int32_t ay_mg;
      int32_t az_mg;

      ax_mg = ((int32_t)accel_x * 1000) / 16384;
      ay_mg = ((int32_t)accel_y * 1000) / 16384;
      az_mg = ((int32_t)accel_z * 1000) / 16384;

      float accel_mag;

      accel_mag = sqrtf(
          ((float)ax_mg * ax_mg) +
          ((float)ay_mg * ay_mg) +
          ((float)az_mg * az_mg)
      );

      accel_mag_mg = (int32_t)accel_mag;
      if (baseline_ready)
      {
          vibration_deviation_mg = accel_mag_mg - baseline_mag_mg;

          if (vibration_deviation_mg < 0)
          {
              vibration_deviation_mg = -vibration_deviation_mg;
          }
      }
      else
      {
          vibration_deviation_mg = 0;
      }
      if (!baseline_ready)
      {
          baseline_sum += accel_mag_mg;
          baseline_count++;

          if (baseline_count >= 20)
          {
              baseline_mag_mg = baseline_sum / 20;
              baseline_ready = 1;

              HAL_UART_Transmit(&huart1,
                                (uint8_t *)"[BASELINE] READY\r\n",
                                sizeof("[BASELINE] READY\r\n") - 1,
                                100);
          }
      }

      /* Determine machine condition */

      if (!baseline_ready)
      {
          current_condition = 0;
      }
      else if (!sensor_valid)
      {
          current_condition = EVENT_CONDITION_CRITICAL;
      }
      else if (vibration_deviation_mg >= CONDITION_CRITICAL_MG)
      {
          current_condition = EVENT_CONDITION_CRITICAL;
      }
      else if (vibration_deviation_mg >= CONDITION_WARNING_MG)
      {
          current_condition = EVENT_CONDITION_WARNING;
      }
      else
      {
          current_condition = EVENT_CONDITION_NORMAL;
      }

      /* Record only condition changes */

      if (current_condition != 0 &&
          current_condition != previous_condition)
      {
          Add_Validation_History(current_condition);
          previous_condition = current_condition;

          Print_Validation_History();
      }

      char buffer[220];
      int len;
      const char *vibration_status;

      if (!baseline_ready)
      {
          vibration_status = "BASELINE";
      }
      else if (vibration_deviation_mg >= CONDITION_CRITICAL_MG)
      {
          vibration_status = "CRITICAL";
      }
      else if (vibration_deviation_mg >= CONDITION_WARNING_MG)
      {
          vibration_status = "WARNING";
      }
      else if (vibration_deviation_mg >= VIBRATION_THRESHOLD_MG)
      {
          vibration_status = "ELEVATED";
      }
      else
      {
          vibration_status = "NORMAL";
      }

      if (!baseline_ready)
      {
          len = sprintf(buffer,
                        "ACC X=%4ld Y=%4ld Z=%4ld | MAG=%4ld | BASELINE\r\n",
                        ax_mg,
                        ay_mg,
                        az_mg,
                        accel_mag_mg);
      }
      else if (!sensor_valid)
      {
          len = sprintf(buffer,
                        "SENSOR INVALID                         | CRITICAL\r\n");
      }
      else if (vibration_deviation_mg >= CONDITION_CRITICAL_MG)
      {
          len = sprintf(buffer,
                        "ACC X=%4ld Y=%4ld Z=%4ld | MAG=%4ld | DEV=%4ld | VIB=%-8s | CRITICAL\r\n",
                        ax_mg,
                        ay_mg,
                        az_mg,
                        accel_mag_mg,
                        vibration_deviation_mg,
                        vibration_status);
      }
      else if (vibration_deviation_mg >= CONDITION_WARNING_MG)
      {
          len = sprintf(buffer,
                        "ACC X=%4ld Y=%4ld Z=%4ld | MAG=%4ld | DEV=%4ld | VIB=%-8s | WARNING \r\n",
                        ax_mg,
                        ay_mg,
                        az_mg,
                        accel_mag_mg,
                        vibration_deviation_mg,
                        vibration_status);
      }
      else
      {
          len = sprintf(buffer,
                        "ACC X=%4ld Y=%4ld Z=%4ld | MAG=%4ld | DEV=%4ld | VIB=%-8s | NORMAL  \r\n",
                        ax_mg,
                        ay_mg,
                        az_mg,
                        accel_mag_mg,
                        vibration_deviation_mg,
                        vibration_status);
      }
      HAL_UART_Transmit(&huart1,
                        (uint8_t *)buffer,
                        len,
                        500);

      HAL_GPIO_TogglePin(STATUS_LED_GPIO_Port, STATUS_LED_Pin);

      HAL_Delay(500);
  }
}
/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
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
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

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
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(STATUS_LED_GPIO_Port, STATUS_LED_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : STATUS_LED_Pin */
  GPIO_InitStruct.Pin = STATUS_LED_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(STATUS_LED_GPIO_Port, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
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
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
      HAL_UART_Transmit(&huart1,
                        (uint8_t *)uart_test_message,
                        sizeof(uart_test_message) - 1,
                        100);

      HAL_GPIO_TogglePin(STATUS_LED_GPIO_Port, STATUS_LED_Pin);

      HAL_Delay(1000);
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
