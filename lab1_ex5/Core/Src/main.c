/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body - Bài 5: Đèn giao thông + 2 LED 7 đoạn
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */
int counter = 0; // Biến đếm chu kỳ 10 giây

// Mã hiển thị LED 7 đoạn Anode chung
const uint8_t segCode[10] = {
    0b11000000, // 0
    0b11111001, // 1
    0b10100100, // 2
    0b10110000, // 3
    0b10011001, // 4
    0b10010010, // 5
    0b10000010, // 6
    0b11111000, // 7
    0b10000000, // 8
    0b10010000  // 9
};
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);

/* USER CODE BEGIN PFP */
void display7SEG_L2(int num); // LED Làn 2 (PB0 -> PB6)
void display7SEG_L1(int num); // LED Làn 1 (PB8 -> PB14)
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

// Hàm hiển thị LED cho Làn 2 (Chân PB0 đến PB6)
void display7SEG_L2(int num)
{
  if (num >= 0 && num <= 9)
  {
    uint8_t code = segCode[num];
    for (int i = 0; i < 7; i++)
    {
      if ((code >> i) & 0x01)
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0 << i, GPIO_PIN_SET);
      else
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0 << i, GPIO_PIN_RESET);
    }
  }
}

// Hàm hiển thị LED cho Làn 1 (Chân PB8 đến PB14)
void display7SEG_L1(int num)
{
  if (num >= 0 && num <= 9)
  {
    uint8_t code = segCode[num];
    for (int i = 0; i < 7; i++)
    {
      // Dịch bit từ GPIO_PIN_8 lên dần để tương ứng với PB8 -> PB14
      if ((code >> i) & 0x01)
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8 << i, GPIO_PIN_SET);
      else
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8 << i, GPIO_PIN_RESET);
    }
  }
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* Configure the system clock */
  SystemClock_Config();

  /* Initialize all configured peripherals */
  MX_GPIO_Init();

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    if (counter < 3)
    {
      // PHA 1: Làn 1 ĐỎ (5s) | Làn 2 XANH (3s)
      HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);   // Đỏ 1 SÁNG
      HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_RESET); // Vàng 1 TẮT
      HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_RESET); // Xanh 1 TẮT

      HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_RESET); // Đỏ 2 TẮT
      HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_RESET); // Vàng 2 TẮT
      HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_SET);  // Xanh 2 SÁNG

      display7SEG_L1(5 - counter); // Làn 1 đếm Đỏ: 5, 4, 3
      display7SEG_L2(3 - counter); // Làn 2 đếm Xanh: 3, 2, 1
    }
    else if (counter < 5)
    {
      // PHA 2: Làn 1 ĐỎ (tiếp tục) | Làn 2 VÀNG (2s)
      HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);
      HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_RESET);
      HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_RESET);

      HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_RESET);
      HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_SET);   // Vàng 2 SÁNG
      HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_RESET);

      display7SEG_L1(5 - counter); // Làn 1 đếm Đỏ: 2, 1
      display7SEG_L2(5 - counter); // Làn 2 đếm Vàng: 2, 1
    }
    else if (counter < 8)
    {
      // PHA 3: Làn 1 XANH (3s) | Làn 2 ĐỎ (5s)
      HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
      HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_RESET);
      HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_SET);   // Xanh 1 SÁNG

      HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET);   // Đỏ 2 SÁNG
      HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_RESET);
      HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_RESET);

      display7SEG_L1(8 - counter);  // Làn 1 đếm Xanh: 3, 2, 1
      display7SEG_L2(10 - counter); // Làn 2 đếm Đỏ: 5, 4, 3
    }
    else
    {
      // PHA 4: Làn 1 VÀNG (2s) | Làn 2 ĐỎ (tiếp tục)
      HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
      HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_SET);   // Vàng 1 SÁNG
      HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_RESET);

      HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET);
      HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_RESET);
      HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_RESET);

      display7SEG_L1(10 - counter); // Làn 1 đếm Vàng: 2, 1
      display7SEG_L2(10 - counter); // Làn 2 đếm Đỏ: 2, 1
    }

    counter++;
    if (counter >= 10)
    {
      counter = 0;
    }

    HAL_Delay(1000);
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
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
  * @brief GPIO Initialization Function
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /* GIẢI PHÓNG CHÂN JTAG ĐỂ DÙNG ĐƯỢC CHÂN PB3, PB4 LÀM OUTPUT */
  __HAL_RCC_AFIO_CLK_ENABLE();
  __HAL_AFIO_REMAP_SWJ_DISABLE();

  /*Configure GPIO pin Output Level cho Đèn giao thông (PA5-PA10) */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8
                          |GPIO_PIN_9|GPIO_PIN_10, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level cho 2 LED 7 đoạn (PB0-PB6 và PB8-PB14) */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3
                          |GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_8
                          |GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12
                          |GPIO_PIN_13|GPIO_PIN_14, GPIO_PIN_RESET);

  /* Cấu hình Mode Output cho Đèn giao thông */
  GPIO_InitStruct.Pin = GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8
                          |GPIO_PIN_9|GPIO_PIN_10;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* Cấu hình Mode Output cho 2 LED 7 đoạn */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3
                          |GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_8
                          |GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12
                          |GPIO_PIN_13|GPIO_PIN_14;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

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
