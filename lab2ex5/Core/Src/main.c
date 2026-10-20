/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body - Lab 2 Exercise 5 (Standard Skeleton)
  ******************************************************************************
  */
/* USER CODE END Header */
#include "main.h"
#include "stm32f1xx_hal_tim.h"

TIM_HandleTypeDef htim2;

/* USER CODE BEGIN PV */
// Biến cho 4 LED 7-đoạn quét trong ngắt
const int MAX_LED = 4;
int index_led = 0;
int led_buffer[4] = {2, 0, 2, 6}; // Mặc định 15:08

// Biến khởi tạo thời gian theo đúng skeleton đề bài
int hour = 20, minute = 26, second = 50;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM2_Init(void);

void display7SEG(int num);
void update7SEG(int index);
void updateClockBuffer(void);

/* USER CODE BEGIN 0 */
// --- HÀM TÁCH GIỜ PHÚT VÀO LED_BUFFER (TRẢ LỜI REPORT 1) ---
void updateClockBuffer(void) {
    led_buffer[0] = hour / 10;   // Hàng chục của Giờ
    led_buffer[1] = hour % 10;   // Hàng đơn vị của Giờ
    led_buffer[2] = minute / 10; // Hàng chục của Phút
    led_buffer[3] = minute % 10; // Hàng đơn vị của Phút
}

// --- HÀM XUẤT MÃ ANODE CHUNG RA PORT B (PB0..PB6) ---
void display7SEG(int num) {
    uint8_t segment_map[10] = {
        0x3F, // 0
        0x06, // 1
        0x5B, // 2
        0x4F, // 3
        0x66, // 4
        0x6D, // 5
        0x7D, // 6
        0x07, // 7
        0x7F, // 8
        0x6F  // 9
    };
    if (num >= 0 && num <= 9) {
        // Tắt tất cả các thanh PB0..PB6 (đưa lên Mức 1)
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6, GPIO_PIN_SET);

        // Bật các thanh tương ứng (kéo xuống Mức 0)
        uint8_t code = segment_map[num];
        for (int i = 0; i < 7; i++) {
            if ((code >> i) & 0x01) {
                HAL_GPIO_WritePin(GPIOB, (GPIO_PIN_0 << i), GPIO_PIN_RESET);
            }
        }
    }
}

// --- HÀM QUÉT LED 7-ĐOẠN (PA6..PA9) ---
void update7SEG(int index) {
    // Tắt tất cả 4 Transistor PNP (PA6..PA9 = 1)
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9, GPIO_PIN_SET);

    // Bật Transistor tương ứng (kéo chân EN xuống 0)
    switch (index) {
        case 0:
            display7SEG(led_buffer[0]);
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_RESET); // EN0
            break;
        case 1:
            display7SEG(led_buffer[1]);
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_RESET); // EN1
            break;
        case 2:
            display7SEG(led_buffer[2]);
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_RESET); // EN2
            break;
        case 3:
            display7SEG(led_buffer[3]);
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_RESET); // EN3
            break;
        default:
            break;
    }
}

// --- TRÌNH PHỤC VỤ NGẮT HARDWARE TIM2 (QUÉT LED MỖI 10ms) ---
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
    if (htim->Instance == TIM2) {
        update7SEG(index_led);
        index_led = (index_led + 1) % MAX_LED;
    }
}
/* USER CODE END 0 */

int main(void)
{
  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();
  MX_TIM2_Init();

  /* USER CODE BEGIN 2 */
  HAL_TIM_Base_Start_IT(&htim2); // Bật ngắt Hardware Timer 2

  // Khởi tạo buffer hiển thị ban đầu
  updateClockBuffer();
  /* USER CODE END 2 */

  /* USER CODE BEGIN WHILE */
  while (1)
  {
      // --- THEO ĐÚNG SKELETON PROGRAM 1.5 CỦA EXERCISE 5 ---
      second++;
      if (second >= 60) {
          second = 0;
          minute++;
      }
      if (minute >= 60) {
          minute = 0;
          hour++;
      }
      if (hour >= 24) {
          hour = 0;
      }
      updateClockBuffer();

      // Nhấp nháy LED PA5 và 2 LED DOT (PA4) nhịp 1s
      HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
      HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_4);

      HAL_Delay(1000); // Lệnh delay ngáng 1000ms đúng theo skeleton đề[cite: 3]
    /* USER CODE END WHILE */
  }
}

void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) { Error_Handler(); }

  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK) { Error_Handler(); }
}

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
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK) { Error_Handler(); }

  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK) { Error_Handler(); }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK) { Error_Handler(); }
}

static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  // Khởi tạo PA4..PA9
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9, GPIO_PIN_SET);

  GPIO_InitStruct.Pin = GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  // Khởi tạo PB0..PB6 cho LED 7-đoạn
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6, GPIO_PIN_SET);

  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

void Error_Handler(void) {
  __disable_irq();
  while (1) {}
}
