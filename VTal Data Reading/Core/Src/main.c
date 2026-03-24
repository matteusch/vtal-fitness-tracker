/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <string.h>
/* USER CODE END Includes */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
// --- MAX30102 ---
#define MAX30102_ADDR   (0x57 << 1)
#define REG_MODE_CONFIG 0x09
#define REG_SPO2_CONFIG 0x0A
#define REG_LED1_PA     0x0C
#define REG_LED2_PA     0x0D
#define MAX_REG_DATA    0x07

// --- BMP390 ---
#define BMP390_ADDR     (0x77 << 1) //If fails to connect, try 0x76
#define BMP_REG_PWR     0x1B        // Power control register
#define BMP_REG_DATA    0x04        // Start of Pressure data

// --- LSM6DSOX ---
#define LSM6DSOX_ADDR   (0x6A << 1) //If fails to connect, try 0x6B
#define LSM_REG_CTRL1   0x10        // Accelerometer control register
#define LSM_REG_DATA    0x28        // Start of Acceldata
/* USER CODE END PD */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;
DMA_HandleTypeDef hdma_i2c1_rx;
UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
uint8_t rx_max[6];
uint8_t rx_bmp[6];
uint8_t rx_lsm[6];

volatile uint8_t data_ready = 0;
char uart_buf[100]; //UART buffer size
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_I2C1_Init(void);
static void MX_USART2_UART_Init(void);

/* USER CODE BEGIN 0 */
/* USER CODE END 0 */

int main(void)
{
  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_I2C1_Init();
  MX_USART2_UART_Init();

  /* USER CODE BEGIN 2 */
  uint8_t config_data;

  HAL_Delay(50);

  // --- 1. WAKE UP MAX30102 ---
  config_data = 0x03; // SpO2 Mode
  HAL_I2C_Mem_Write(&hi2c1, MAX30102_ADDR, REG_MODE_CONFIG, 1, &config_data, 1, 100);
  config_data = 0x27;
  HAL_I2C_Mem_Write(&hi2c1, MAX30102_ADDR, REG_SPO2_CONFIG, 1, &config_data, 1, 100);
  config_data = 0x24;
  HAL_I2C_Mem_Write(&hi2c1, MAX30102_ADDR, REG_LED1_PA, 1, &config_data, 1, 100);
  config_data = 0x24;
  HAL_I2C_Mem_Write(&hi2c1, MAX30102_ADDR, REG_LED2_PA, 1, &config_data, 1, 100);

  // --- 2. WAKE UP BMP390 (THE TWO-STEP METHOD) ---

  // Step 1: Turn on the internal Pressure and Temperature sensors first (0x03)
  config_data = 0x03;
  HAL_I2C_Mem_Write(&hi2c1, BMP390_ADDR, BMP_REG_PWR, 1, &config_data, 1, 100);

  HAL_Delay(10); // Let the internal state machine catch up

  // Step 2: Now tell it to continuously sample in Normal Mode (0x33)
  config_data = 0x33;
  HAL_I2C_Mem_Write(&hi2c1, BMP390_ADDR, BMP_REG_PWR, 1, &config_data, 1, 100);

  // --- 3. WAKE UP LSM6DSOX ---
  config_data = 0x50;
  HAL_I2C_Mem_Write(&hi2c1, LSM6DSOX_ADDR, LSM_REG_CTRL1, 1, &config_data, 1, 100);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
      // SENSOR 1: MAX30102
      HAL_I2C_Mem_Read_DMA(&hi2c1, MAX30102_ADDR, MAX_REG_DATA, 1, rx_max, 6);
      while (data_ready == 0) {} // Wait for DMA to finish
      data_ready = 0;            // Reset flag

      // SENSOR 2: BMP390
      HAL_I2C_Mem_Read_DMA(&hi2c1, BMP390_ADDR, BMP_REG_DATA, 1, rx_bmp, 6);
      while (data_ready == 0) {}
      data_ready = 0;

      // SENSOR 3: LSM6DSOX
      HAL_I2C_Mem_Read_DMA(&hi2c1, LSM6DSOX_ADDR, LSM_REG_DATA, 1, rx_lsm, 6);
      while (data_ready == 0) {}
      data_ready = 0;

      // PROCESS AND PRINT ALL DATA

      uint32_t red_led = ((rx_max[0] << 16) | (rx_max[1] << 8) | rx_max[2]) & 0x03FFFF;
      uint32_t ir_led =  ((rx_max[3] << 16) | (rx_max[4] << 8) | rx_max[5]) & 0x03FFFF;

      // BMP390
      uint32_t raw_press = (rx_bmp[2] << 16) | (rx_bmp[1] << 8) | rx_bmp[0];

      // LSM6DSOX
      int16_t accel_x = (int16_t)((rx_lsm[1] << 8) | rx_lsm[0]);
      int16_t accel_y = (int16_t)((rx_lsm[3] << 8) | rx_lsm[2]);
      int16_t accel_z = (int16_t)((rx_lsm[5] << 8) | rx_lsm[4]);

      // (Red, IR, Pressure, AccelX, AccelY, AccelZ)
      sprintf(uart_buf, "%lu,%lu,%lu,%d,%d,%d\r\n", red_led, ir_led, raw_press, accel_x, accel_y, accel_z);

      // Send to PC
      HAL_UART_Transmit(&huart2, (uint8_t*)uart_buf, strlen(uart_buf), 100);

      HAL_Delay(10);
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
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure LSE Drive Capability
  */
  HAL_PWR_EnableBkUpAccess();
  __HAL_RCC_LSEDRIVE_CONFIG(RCC_LSEDRIVE_LOW);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSE|RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = 0;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_MSI;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 16;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
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

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Enable MSI Auto calibration
  */
  HAL_RCCEx_EnableMSIPLLMode();
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{
  hi2c1.Instance = I2C1;
  hi2c1.Init.Timing = 0x0060112F;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Analogue filter
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Digital filter
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c1, 0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{
  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Channel7_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel7_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel7_IRQn);
}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(BUSY_LED_GPIO_Port, BUSY_LED_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : BUSY_LED_Pin */
  GPIO_InitStruct.Pin = BUSY_LED_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(BUSY_LED_GPIO_Port, &GPIO_InitStruct);
}

/* USER CODE BEGIN 4 */
void HAL_I2C_MemRxCpltCallback(I2C_HandleTypeDef *hi2c)
{
    if (hi2c->Instance == I2C1)
    {
        data_ready = 1;
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

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  * where the assert_param error has occurred.
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
