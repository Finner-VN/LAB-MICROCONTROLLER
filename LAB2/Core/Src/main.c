/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Gộp bài Ex10 (LED Ma Trận) & Ex8 (Đồng hồ LED 7 đoạn)
  ******************************************************************************
  */
/* USER CODE END Header */
#include "main.h"

/* USER CODE BEGIN PV */
TIM_HandleTypeDef htim2;

// --- SOFTWARE TIMER TEMPLATE (4 TIMERS CHO 4 TÁC VỤ SONG SONG) ---
#define MAX_TIMERS 4
int timer_counter[MAX_TIMERS] = {0};
int timer_flag[MAX_TIMERS] = {0};

// ================= 1. CÁC BIẾN CỦA MA TRẬN LED =================
const int MAX_LED_MATRIX = 8;
int index_led_matrix = 0;
// Mảng 8 byte mã hóa chữ "A"
uint8_t matrix_A[8] = {
    0x03, 0xF5, 0xF6, 0xF6, 0xF6, 0xF6, 0xF5, 0x03
};

// ================= 2. CÁC BIẾN CỦA ĐỒNG HỒ SỐ =================
const int MAX_LED = 4;
int index_led = 0;
int led_buffer[4] = {2, 0, 2, 6}; // Mặc định 20:26
int hour = 20, minute = 26, second = 50;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM2_Init(void);

void setTimer(int index, int duration);
void timerRun(void);

// Các hàm Ma trận
void updateLEDMatrix(int index);
void shiftLeft(void);

// Các hàm Đồng hồ
void display7SEG(int num);
void update7SEG(int index);
void updateClockBuffer(void);

/* USER CODE BEGIN 0 */
// --- HÀM TẠO TIMING SOFTWARE TIMER ---
void setTimer(int index, int duration) {
    if (index >= 0 && index < MAX_TIMERS) {
        timer_counter[index] = duration;
        timer_flag[index] = 0;
    }
}

void timerRun(void) {
    for (int i = 0; i < MAX_TIMERS; i++) {
        if (timer_counter[i] > 0) {
            timer_counter[i]--;
            if (timer_counter[i] <= 0) {
                timer_flag[i] = 1;
            }
        }
    }
}

// ================= HÀM XỬ LÝ MA TRẬN LED =================
void shiftLeft(void) {
    uint8_t temp = matrix_A[0];
    for (int i = 0; i < 7; i++) {
        matrix_A[i] = matrix_A[i + 1];
    }
    matrix_A[7] = temp;
}

void updateLEDMatrix(int index) {
    // 1. Tắt tất cả các Cột
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_10 | GPIO_PIN_11 |
                             GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15, GPIO_PIN_SET);

    // 2. Xuất dữ liệu Hàng
    uint8_t data = matrix_A[index];
    for (int i = 0; i < 8; i++) {
        if ((data >> i) & 0x01) {
            HAL_GPIO_WritePin(GPIOB, (GPIO_PIN_8 << i), GPIO_PIN_SET);   // Tắt
        } else {
            HAL_GPIO_WritePin(GPIOB, (GPIO_PIN_8 << i), GPIO_PIN_RESET); // Sáng
        }
    }

    // 3. Bật Cột tương ứng
    switch (index) {
        case 0: HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2,  GPIO_PIN_RESET); break;
        case 1: HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3,  GPIO_PIN_RESET); break;
        case 2: HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_RESET); break;
        case 3: HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_RESET); break;
        case 4: HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_RESET); break;
        case 5: HAL_GPIO_WritePin(GPIOA, GPIO_PIN_13, GPIO_PIN_RESET); break;
        case 6: HAL_GPIO_WritePin(GPIOA, GPIO_PIN_14, GPIO_PIN_RESET); break;
        case 7: HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_RESET); break;
    }
}

// ================= HÀM XỬ LÝ ĐỒNG HỒ 7-ĐOẠN =================
void updateClockBuffer(void) {
    led_buffer[0] = hour / 10;
    led_buffer[1] = hour % 10;
    led_buffer[2] = minute / 10;
    led_buffer[3] = minute % 10;
}

void display7SEG(int num) {
    uint8_t segment_map[10] = { 0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F };
    if (num >= 0 && num <= 9) {
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6, GPIO_PIN_SET);
        uint8_t code = segment_map[num];
        for (int i = 0; i < 7; i++) {
            if ((code >> i) & 0x01) {
                HAL_GPIO_WritePin(GPIOB, (GPIO_PIN_0 << i), GPIO_PIN_RESET);
            }
        }
    }
}

void update7SEG(int index) {
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
    }
}

// Ngắt Hardware Timer 2 - đếm lùi toàn bộ Timer
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
    if (htim->Instance == TIM2) {
        timerRun();
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
  HAL_TIM_Base_Start_IT(&htim2);

  updateClockBuffer();
  updateLEDMatrix(index_led_matrix);

  // Khởi tạo 4 bộ đếm song song
  setTimer(0, 3);   // Timer 0: Quét ma trận LED
  setTimer(1, 120); // Timer 1: Dịch chữ "A"
  setTimer(2, 15);  // Timer 2: Quét LED 7-đoạn
  setTimer(3, 100); // Timer 3: Đếm đồng hồ 1s
  /* USER CODE END 2 */

  /* USER CODE BEGIN WHILE */
  while (1)
  {
      // --- TASK 1: QUÉT HIỂN THỊ MA TRẬN LED ---
      if (timer_flag[0] == 1) {
          setTimer(0, 3);
          index_led_matrix = (index_led_matrix + 1) % MAX_LED_MATRIX;
          updateLEDMatrix(index_led_matrix);
      }

      // --- TASK 2: DỊCH CHUYỂN CHỮ "A" ---
      if (timer_flag[1] == 1) {
          setTimer(1, 120);
          shiftLeft();
      }

      // --- TASK 3: QUÉT HIỂN THỊ ĐỒNG HỒ 7 ĐOẠN ---
      if (timer_flag[2] == 1) {
          setTimer(2, 15);
          update7SEG(index_led);
          index_led = (index_led + 1) % MAX_LED;
      }

      // --- TASK 4: ĐẾM GIỜ VÀ CHỚP LED STATUS ---
      if (timer_flag[3] == 1) {
          setTimer(3, 100);

          HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_4 | GPIO_PIN_5);

          second++;
          if (second >= 60) {
              second = 0;
              minute++;
              if (minute >= 60) {
                  minute = 0;
                  hour++;
                  if (hour >= 24) hour = 0;
              }
          }
          updateClockBuffer();
      }
  }
  /* USER CODE END WHILE */
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

  HAL_NVIC_SetPriority(TIM2_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(TIM2_IRQn);
}

static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_AFIO_CLK_ENABLE();
  __HAL_AFIO_REMAP_SWJ_DISABLE(); // Tắt JTAG

  // Khởi tạo các chân GPIOA: PA2..PA15 (kết hợp cả LED 7 đoạn và Ma trận)
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7|
                           GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12|
                           GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15, GPIO_PIN_SET);

  GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7|
                        GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12|
                        GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  // Khởi tạo các chân GPIOB: PB0..PB6 và PB8..PB15
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|
                           GPIO_PIN_6|GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11|
                           GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15, GPIO_PIN_SET);

  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|
                        GPIO_PIN_6|GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11|
                        GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

void Error_Handler(void) {
  __disable_irq();
  while (1) {}
}
