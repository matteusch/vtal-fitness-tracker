/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : VTal Bicep Fitness Tracker Firmware
  * @details        : Handles asynchronous I2C/DMA reads from MAX30102,
  * LSM6DSOX, and BMP390.
  * Implements time-domain R-R interval smoothing for bicep PPG
  * and transmits unified telemetry over UART1 and UART2.
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "bmp3.h"
#include "bmp3_defs.h"
/* USER CODE END Includes */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
// MAX30102
#define MAX30102_ADDR   (0x57 << 1)
#define REG_MODE_CONFIG 0x09
#define REG_SPO2_CONFIG 0x0A
#define REG_LED1_PA     0x0C
#define REG_LED2_PA     0x0D
#define MAX_REG_DATA    0x07

// BMP390
#define BMP390_ADDR     (0x77 << 1)
#define BMP_REG_PWR     0x1B
#define BMP_REG_DATA    0x04

// LSM6DSOX
#define LSM6DSOX_ADDR   (0x6A << 1)
#define LSM_REG_CTRL1   0x10
#define LSM_REG_DATA    0x28
/* USER CODE END PD */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;
DMA_HandleTypeDef hdma_i2c1_rx;
UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
// DMA variables
volatile uint8_t dma_transfer_complete = 0;
uint8_t sensor_sequence_step = 0;
volatile uint8_t use_dma_buffer = 0;

uint8_t rx_max[6];
uint8_t rx_bmp[6];
uint8_t rx_lsm[6];

// LSM6DSOX variables
uint8_t step_state = 0;
uint32_t step_count = 0;
uint32_t last_step_time = 0;

// MAX30102 variables
float dc_baseline_red = 0;
float dc_baseline_ir = 0;
float alpha = 0.85f;
uint8_t filter_initialized = 0;
uint8_t beat_state = 0;
uint32_t beat_count = 0;
uint32_t last_beat_time = 0;

float beta = 0.85f;
float smooth_red = 0.0f;
float smooth_ir = 0.0f;
float previous_smooth_ir = 0.0f;

float displayed_bpm = 0.0f;
float avg_beat_interval = 0.0f;

// BMP390 variables
float current_altitude = 0.0f;
float current_temp = 0.0f;
float initial_baseline_altitude = 0.0f;
uint8_t altitude_calibrated = 0;
uint8_t calibration_counter = 0;

// Telemetry variables
volatile uint8_t data_ready = 0;
char uart_buf[100];
uint32_t last_telemetry_time = 0;
float distance_covered = 0.0f;
const float STRIDE_LENGTH_M = 0.762f;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_I2C1_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_USART2_UART_Init(void);
BMP3_INTF_RET_TYPE user_i2c_read(uint8_t reg_addr, uint8_t *reg_data, uint32_t len, void *intf_ptr);
BMP3_INTF_RET_TYPE user_i2c_write(uint8_t reg_addr, const uint8_t *reg_data, uint32_t len, void *intf_ptr);
void user_delay_us(uint32_t period, void *intf_ptr);

/* USER CODE BEGIN 0 */
/* USER CODE END 0 */

int main(void)
{
  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_I2C1_Init();
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();

  /* USER CODE BEGIN 2 */
  uint8_t config_data;
  HAL_Delay(50);

  // MAX30102 Init
  config_data = 0x03;
  HAL_I2C_Mem_Write(&hi2c1, MAX30102_ADDR, REG_MODE_CONFIG, 1, &config_data, 1, 100);
  config_data = 0x27;
  HAL_I2C_Mem_Write(&hi2c1, MAX30102_ADDR, REG_SPO2_CONFIG, 1, &config_data, 1, 100);
  config_data = 0x24; // Red ~7mA
  HAL_I2C_Mem_Write(&hi2c1, MAX30102_ADDR, REG_LED1_PA, 1, &config_data, 1, 100);
  config_data = 0x24; // IR ~7mA
  HAL_I2C_Mem_Write(&hi2c1, MAX30102_ADDR, REG_LED2_PA, 1, &config_data, 1, 100);

  // BMP390 Init
  struct bmp3_dev bmp;
  struct bmp3_settings settings = {0};
  uint8_t bmp_addr = BMP390_ADDR;

  bmp.intf = BMP3_I2C_INTF;
  bmp.intf_ptr = &bmp_addr;
  bmp.read = user_i2c_read;
  bmp.write = user_i2c_write;
  bmp.delay_us = user_delay_us;

  int8_t rslt = bmp3_init(&bmp);

  settings.press_en = BMP3_ENABLE;
  settings.temp_en = BMP3_ENABLE;
  settings.odr_filter.press_os = BMP3_OVERSAMPLING_8X;
  settings.odr_filter.temp_os = BMP3_OVERSAMPLING_2X;
  settings.odr_filter.iir_filter = BMP3_IIR_FILTER_COEFF_3;
  settings.odr_filter.odr = BMP3_ODR_25_HZ;

  uint16_t settings_sel = BMP3_SEL_PRESS_EN | BMP3_SEL_TEMP_EN | BMP3_SEL_PRESS_OS | BMP3_SEL_TEMP_OS | BMP3_SEL_ODR | BMP3_SEL_IIR_FILTER;
  rslt = bmp3_set_sensor_settings(settings_sel, &settings, &bmp);

  settings.op_mode = BMP3_MODE_NORMAL;
  rslt = bmp3_set_op_mode(&settings, &bmp);

  // LSM6DSOX Init
  config_data = 0x54;
  HAL_I2C_Mem_Write(&hi2c1, LSM6DSOX_ADDR, LSM_REG_CTRL1, 1, &config_data, 1, 100);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  HAL_I2C_Mem_Read_DMA(&hi2c1, MAX30102_ADDR, MAX_REG_DATA, 1, rx_max, 6);

  while (1)
  {
      if (dma_transfer_complete == 1)
      {
          dma_transfer_complete = 0;
          uint32_t current_time = HAL_GetTick();

          // --- STEP 0: MAX30102 ---
          if (sensor_sequence_step == 0)
          {
        	  uint32_t red_led = ((rx_max[0] << 16) | (rx_max[1] << 8) | rx_max[2]) & 0x03FFFF;
        	  uint32_t ir_led  = ((rx_max[3] << 16) | (rx_max[4] << 8) | rx_max[5]) & 0x03FFFF;

        	  // Proximity Check
        	  if (ir_led < 80000) {
        		  filter_initialized = 0;
        		  displayed_bpm = 0.0f;
        		  avg_beat_interval = 0.0f;
        		  beat_state = 0;
        		  beat_count = 0;
        	  }
        	  else {
        		  // DC Filter
        		  if (filter_initialized == 0) {
        			  dc_baseline_red = (float)red_led;
        			  dc_baseline_ir = (float)ir_led;
        			  filter_initialized = 1;
        		  }
        		  dc_baseline_red = (alpha * dc_baseline_red) + ((1.0f - alpha) * (float)red_led);
        		  dc_baseline_ir  = (alpha * dc_baseline_ir)  + ((1.0f - alpha) * (float)ir_led);

        		  float ac_red = (float)red_led - dc_baseline_red;
        		  float ac_ir  = (float)ir_led  - dc_baseline_ir;

        		  // AC Smoothing
        		  smooth_red = (beta * smooth_red) + ((1.0f - beta) * ac_red);
        		  smooth_ir  = (beta * smooth_ir)  + ((1.0f - beta) * ac_ir);

        		  float ir_delta = smooth_ir - previous_smooth_ir;
        		  previous_smooth_ir = smooth_ir;

        		  char debug_msg[60];
        		  sprintf(debug_msg, "Raw IR: %lu | Delta: %.2f\r\n", ir_led, ir_delta);
        		  HAL_UART_Transmit(&huart2, (uint8_t*)debug_msg, strlen(debug_msg), 10);

                  float UPPER_THRESHOLD_BEAT = 4.0f;
                  float LOWER_THRESHOLD_BEAT = -3.0f;

                  // Dynamic refractory period
                  uint32_t current_refractory = 400;
                  if (displayed_bpm > 30.0f) {
                      current_refractory = (uint32_t)((60000.0f / displayed_bpm) * 0.6f);
                      if (current_refractory < 350) current_refractory = 350;
                      if (current_refractory > 800) current_refractory = 800;
                  }

                  // Peak detection
                  if (beat_state == 0 && ir_delta > UPPER_THRESHOLD_BEAT && (current_time - last_beat_time) > current_refractory) {
                      beat_state = 1;
                      uint32_t beat_time_ms = current_time - last_beat_time;

                      // Validate beat interval (30-240 BPM limits)
                      if (beat_time_ms > 250 && beat_time_ms < 2000) {
                          beat_count++;

                          // Warm-up phase
                          if (beat_count < 4) {
                              if (avg_beat_interval == 0.0f) {
                                  avg_beat_interval = (float)beat_time_ms;
                              } else {
                                  avg_beat_interval = (0.50f * avg_beat_interval) + (0.50f * (float)beat_time_ms);
                              }
                          }
                          // Locked phase
                          else {
                              float diff = (float)beat_time_ms - avg_beat_interval;

                              // Outlier clamping
                              if (diff > 100.0f) {
                                  beat_time_ms = (uint32_t)(avg_beat_interval + 25.0f);
                              }
                              else if (diff < -100.0f) {
                                  beat_time_ms = (uint32_t)(avg_beat_interval - 25.0f);
                              }

                              // Interval smoothing
                              avg_beat_interval = (0.95f * avg_beat_interval) + (0.05f * (float)beat_time_ms);
                          }

                          displayed_bpm = 60000.0f / avg_beat_interval;

                          char beat_msg[60];
                          sprintf(beat_msg, "Beat! Interval: %.0fms | DISPLAY BPM: %.0f\r\n", avg_beat_interval, displayed_bpm);
                          HAL_UART_Transmit(&huart2, (uint8_t*)beat_msg, strlen(beat_msg), 10);
                      }
                      last_beat_time = current_time;
                  }
                  else if (beat_state == 1 && ir_delta < LOWER_THRESHOLD_BEAT) {
                      beat_state = 0;
                  }
        	  }

        	  sensor_sequence_step = 1;
        	  HAL_I2C_Mem_Read_DMA(&hi2c1, BMP390_ADDR, BMP_REG_DATA, 1, rx_bmp, 6);
          }

          // --- STEP 1: BMP390 ---
          else if (sensor_sequence_step == 1)
          {
              struct bmp3_data comp_data;

              use_dma_buffer = 1;
              bmp3_get_sensor_data(BMP3_PRESS | BMP3_TEMP, &comp_data, &bmp);
              use_dma_buffer = 0;

              current_temp = (float)comp_data.temperature - 5.0;
              float absolute_altitude = 44330.0f * (1.0f - pow((comp_data.pressure / 101325.0f), 0.190295f));

              // IIR Stabilization
              if (altitude_calibrated == 0) {
                  calibration_counter++;
                  if (calibration_counter > 25) {
                      initial_baseline_altitude = absolute_altitude;
                      altitude_calibrated = 1;
                  }
              }
              else {
                  current_altitude = absolute_altitude - initial_baseline_altitude;
              }

              sensor_sequence_step = 2;
              HAL_I2C_Mem_Read_DMA(&hi2c1, LSM6DSOX_ADDR, LSM_REG_DATA, 1, rx_lsm, 6);
          }

          // --- STEP 2: LSM6DSOX ---
          else if (sensor_sequence_step == 2)
          {
              int16_t accel_x = (int16_t)((rx_lsm[1] << 8) | rx_lsm[0]);
              int16_t accel_y = (int16_t)((rx_lsm[3] << 8) | rx_lsm[2]);
              int16_t accel_z = (int16_t)((rx_lsm[5] << 8) | rx_lsm[4]);

              float g_x = accel_x * 0.488f / 1000.0f;
              float g_y = accel_y * 0.488f / 1000.0f;
              float g_z = accel_z * 0.488f / 1000.0f;

              float dynamic_movement = sqrt(g_x * g_x + g_y * g_y + g_z * g_z) - 1.0f;

              // Cadence Peak Detector
              float UPPER_THRESHOLD_STEP = 0.4f;
              float LOWER_THRESHOLD_STEP = 0.15f;

              if (step_state == 0 && dynamic_movement > UPPER_THRESHOLD_STEP) {
                  step_state = 1;
                  step_count++;

                  uint32_t step_time_ms = current_time - last_step_time;
                  if (step_time_ms > 0) {
                      float steps_per_minute = 60000.0f / (float)step_time_ms;
                      char step_msg[60];
                      sprintf(step_msg, "STEP! Total: %lu | Cadence: %.1f SPM\r\n", step_count, steps_per_minute);
                      HAL_UART_Transmit(&huart2, (uint8_t*)step_msg, strlen(step_msg), 100);
                  }
                  last_step_time = current_time;
              }
              else if (step_state == 1 && dynamic_movement < LOWER_THRESHOLD_STEP) {
                  step_state = 0;
              }

              // --- 1 HZ Telemetry Transmission ---
              if (current_time - last_telemetry_time >= 1000) {

            	  distance_covered = (float)step_count * STRIDE_LENGTH_M;

            	  sprintf(uart_buf, "%.0f,%.1f,%.1f,%lu,%.2f\n",
            	          displayed_bpm, current_temp, current_altitude, step_count, distance_covered);

            	  HAL_UART_Transmit(&huart2, (uint8_t*)uart_buf, strlen(uart_buf), 100);
            	  HAL_UART_Transmit(&huart1, (uint8_t*)uart_buf, strlen(uart_buf), 100);
            	  last_telemetry_time = current_time;
              }

              sensor_sequence_step = 0;
              HAL_Delay(10);
              HAL_I2C_Mem_Read_DMA(&hi2c1, MAX30102_ADDR, MAX_REG_DATA, 1, rx_max, 6);
          }
      }
  }
  /* USER CODE END WHILE */
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

  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }

  HAL_PWR_EnableBkUpAccess();
  __HAL_RCC_LSEDRIVE_CONFIG(RCC_LSEDRIVE_LOW);

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
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c1, 0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief USART1 Initialization Function (HC-05 Bluetooth)
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  __HAL_RCC_USART1_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  GPIO_InitStruct.Pin = GPIO_PIN_9 | GPIO_PIN_10;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF7_USART1;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  huart1.Instance = USART1;
  huart1.Init.BaudRate = 38400;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief USART2 Initialization Function (PC USB)
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
  __HAL_RCC_DMA1_CLK_ENABLE();
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

  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  HAL_GPIO_WritePin(BUSY_LED_GPIO_Port, BUSY_LED_Pin, GPIO_PIN_RESET);

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
        dma_transfer_complete = 1;
    }
}

// Bosch API I2C Read Wrapper
BMP3_INTF_RET_TYPE user_i2c_read(uint8_t reg_addr, uint8_t *reg_data, uint32_t len, void *intf_ptr) {
    if (use_dma_buffer == 1) {
        if (reg_addr == BMP_REG_DATA) {
            memcpy(reg_data, rx_bmp, len);
            return 0;
        }
        if (reg_addr == 0x03) {
            reg_data[0] = 0x60;
            return 0;
        }
        memset(reg_data, 0, len);
        return 0;
    }

    uint8_t dev_addr = *(uint8_t*)intf_ptr;
    if (HAL_I2C_Mem_Read(&hi2c1, dev_addr, reg_addr, 1, reg_data, len, 100) == HAL_OK) return 0;
    return -1;
}

// Bosch API I2C Write Wrapper
BMP3_INTF_RET_TYPE user_i2c_write(uint8_t reg_addr, const uint8_t *reg_data, uint32_t len, void *intf_ptr) {
    uint8_t dev_addr = *(uint8_t*)intf_ptr;
    if (HAL_I2C_Mem_Write(&hi2c1, dev_addr, reg_addr, 1, (uint8_t*)reg_data, len, 100) == HAL_OK) return 0;
    return -1;
}

// Bosch API Microsecond Delay Wrapper
void user_delay_us(uint32_t period, void *intf_ptr) {
    uint32_t delay_ms = (period / 1000) + 1;
    HAL_Delay(delay_ms);
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  __disable_irq();
  while (1)
  {
  }
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
}
#endif /* USE_FULL_ASSERT */
