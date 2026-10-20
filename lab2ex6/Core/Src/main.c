/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body - Lab 2 Exercise 6 (Software Multi-Timer)
  ******************************************************************************
  */
/* USER CODE END Header */
#include "main.h"
#include "stm32f1xx_hal_tim.h"

TIM_HandleTypeDef htim2;

/* USER CODE BEGIN PV */
// --- 1. SOFTWARE TIMER TEMPLATE (2 TIMERS) ---
#define MAX_TIMERS 2

int timer_counter[MAX_TIMERS] = {0};
int timer_flag[MAX_TIMERS] = {0};

// --- 2. CÁC BIẾN CỦA ĐỒNG HỒ SỐ (EX 5) ---
const int MAX_LED = 4;
int index_led = 0;
int led_buffer[4] = {1, 5, 0, 8}; // Mặc định 15:08
int hour = 15, minute = 8, second = 50;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM2_Init(void);

// Các hàm cho Software Timer
void setTimer(int index, int duration);
void timerRun(void);

// Các hàm cho LED 7-đoạn & Đồng hồ
void display7SEG(int num);
void update7SEG(int index);
void updateClockBuffer(void);

/* USER CODE BEGIN 0 */
// --- HÀM TẠO TIMING SOFTWARE TIMER (THEO TEMPLATE) ---
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

// --- HÀM HIỂN THỊ MÃ ANODE CHUNG RA PORT B (PB0..PB6) ---
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
        // Tắt tất cả các thanh LED (đưa về Mức 1)
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
    // Tắt tất cả 4 Transistor PNP bằng cách cho chân B lên HIGH (1)
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9, GPIO_PIN_SET);

    // Bật Transistor tương ứng bằng cách kéo chân B xuống LOW (0)
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

void updateClockBuffer(void) {
    led_buffer[0] = hour / 10;
    led_buffer[1] = hour % 10;
    led_buffer[2] = minute / 10;
    led_buffer[3] = minute % 10;
}

// --- HÀM NGẮT TIMER 2 (GỌI MỖI 10ms) ---
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
    if (htim->Instance == TIM2) {
        timerRun(); // Ngắt 10ms chỉ làm duy nhất việc trừ counter
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

  // Timer 0: Quét LED 7 đoạn (1 tick = 10ms)
  setTimer(0, 50);

  // Timer 1: Đếm đồng hồ + đảo LED PA5 (100 ticks = 1000ms = 1 giây)
  setTimer(1, 100);
  /* USER CODE END 2 */

  /* USER CODE BEGIN WHILE */
  while (1)
  {
      // --- TASK 1: QUÉT LED 7-ĐOẠN (MỖI 10ms) ---
      if (timer_flag[0] == 1) {
          setTimer(0, 50); // Đặt lại 1 tick (10ms)
          update7SEG(index_led);
          index_led = (index_led + 1) % MAX_LED;
      }

      // --- TASK 2: ĐẾM ĐỒNG HỒ SỐ & CHỚP LED PA5 (MỖI 1000ms) ---
      if (timer_flag[1] == 1) {
          setTimer(1, 100); // Đặt lại 100 ticks (1000ms)

          // 1. Đảo trạng thái LED đơn PA5
          HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);

          // 2. Logic tăng đồng hồ Giờ : Phút : Giây
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

          // 3. Cập nhật dữ liệu số hiển thị
          updateClockBuffer();
      }
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

  // Khởi tạo các chân PORT A (PA4..PA9)
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9, GPIO_PIN_SET);

  GPIO_InitStruct.Pin = GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  // Khởi tạo các chân PORT B (PB0..PB6)
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
