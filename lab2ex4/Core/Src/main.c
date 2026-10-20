/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body - Lab 2 Exercise 4 (Digital Clock)
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stm32f1xx_hal_tim.h"
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
// Khai báo 3 Software Timer (Đơn vị 10ms)
int timer1_counter = 0; int timer1_flag = 0; // Timer 1: Quét LED 7-đoạn (250ms/LED)
int timer2_counter = 0; int timer2_flag = 0; // Timer 2: Chớp tắt LED DOT (1000ms)
int timer3_counter = 0; int timer3_flag = 0; // Timer 3: Cập nhật đồng hồ (1000ms)

// Bộ đệm hiển thị & chỉ số LED
const int MAX_LED = 4;
int led_buffer[4] = {1, 5, 0, 8}; // Giá trị khởi tạo mặc định
int index_led = 0;

// Các biến lưu thời gian Đồng hồ
int hour = 15;
int minute = 8;
int second = 50; // Đặt sẵn 50s để dễ test nhảy Phút/Giờ
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM2_Init(void);
/* USER CODE BEGIN PFP */
void setTimer1(int duration);
void setTimer2(int duration);
void setTimer3(int duration);
void timerRun(void);

void display7SEG(int num);
void update7SEG(int index);
void updateClockBuffer(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
// 1. Quản lý Software Timer
void setTimer1(int duration) { timer1_counter = duration / 10; timer1_flag = 0; }
void setTimer2(int duration) { timer2_counter = duration / 10; timer2_flag = 0; }
void setTimer3(int duration) { timer3_counter = duration / 10; timer3_flag = 0; }

void timerRun(void) {
    if (timer1_counter > 0) {
        timer1_counter--;
        if (timer1_counter <= 0) timer1_flag = 1;
    }
    if (timer2_counter > 0) {
        timer2_counter--;
        if (timer2_counter <= 0) timer2_flag = 1;
    }
    if (timer3_counter > 0) {
        timer3_counter--;
        if (timer3_counter <= 0) timer3_flag = 1;
    }
}

// 2. Tách biến Giờ & Phút đưa vào led_buffer
void updateClockBuffer(void) {
    led_buffer[0] = hour / 10;   // Chữ số hàng chục của Giờ
    led_buffer[1] = hour % 10;   // Chữ số hàng đơn vị của Giờ
    led_buffer[2] = minute / 10; // Chữ số hàng chục của Phút
    led_buffer[3] = minute % 10; // Chữ số hàng đơn vị của Phút
}

// 3. Xuất mã 7-đoạn Anode chung ra Port B
void display7SEG(int num) {
    uint8_t seg_code[10] = {
        0xC0, // 0
        0xF9, // 1
        0xA4, // 2
        0xB0, // 3
        0x99, // 4
        0x92, // 5
        0x82, // 6
        0xF8, // 7
        0x80, // 8
        0x90  // 9
    };
    if (num >= 0 && num <= 9) {
        uint8_t code = seg_code[num];
        for (int i = 0; i < 7; i++) {
            HAL_GPIO_WritePin(GPIOB, (GPIO_PIN_0 << i), (code & (1 << i)) ? GPIO_PIN_SET : GPIO_PIN_RESET);
        }
    }
}

// 4. Bật LED tương ứng với index
void update7SEG(int index) {
    // Tắt tất cả 4 LED (PA6, PA7, PA8, PA9 = HIGH)
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9, GPIO_PIN_SET);

    switch (index) {
        case 0:
            display7SEG(led_buffer[0]);
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_RESET);
            break;
        case 1:
            display7SEG(led_buffer[1]);
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_RESET);
            break;
        case 2:
            display7SEG(led_buffer[2]);
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_RESET);
            break;
        case 3:
            display7SEG(led_buffer[3]);
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_RESET);
            break;
        default:
            break;
    }
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();
  MX_TIM2_Init();

  /* USER CODE BEGIN 2 */
  HAL_TIM_Base_Start_IT(&htim2); // Kích hoạt ngắt TIM2 (10ms)

  // Đặt giá trị ban đầu cho buffer
  updateClockBuffer();

  // Đặt thời gian cho 3 Software Timer
  setTimer1(250);  // Quét mỗi LED trong 250ms (4 LED = 1s)
  setTimer2(1000); // Chớp tắt LED DOT mỗi 1s
  setTimer3(1000); // Đếm 1s thời gian thực cho đồng hồ
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
      // 1. Quét 4 LED 7-đoạn mỗi 250ms
      if (timer1_flag == 1) {
          setTimer1(250);
          update7SEG(index_led);
          index_led = (index_led + 1) % MAX_LED;
      }

      // 2. Chớp tắt LED DOT (PA4) mỗi 1s
      if (timer2_flag == 1) {
          setTimer2(1000);
          HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_4);
      }

      // 3. Đếm thời gian Đồng hồ mỗi 1s
      if (timer3_flag == 1) {
          setTimer3(1000);

          second++;
          if (second >= 60) {
              second = 0;
              minute++;
              if (minute >= 60) {
                  minute = 0;
                  hour++;
                  if (hour >= 24) {
                      hour = 0;
                  }
              }
          }

          // Cập nhật lại dữ liệu hiển thị sau khi thời gian thay đổi
          updateClockBuffer();
      }
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

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
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
  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 7999;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 9;
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
}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9, GPIO_PIN_SET);
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3
                          |GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6, GPIO_PIN_RESET);

  GPIO_InitStruct.Pin = GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3
                          |GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

/* USER CODE BEGIN 4 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
    if (htim->Instance == TIM2) {
        timerRun(); // Gọi hàm chạy Software Timer mỗi 10ms
    }
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
void assert_failed(uint8_t *file, uint32_t line)
{
}
#endif /* USE_FULL_ASSERT */
