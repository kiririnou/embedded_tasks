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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define BTN_IDLE 0x0000
#define BTN_PRESSED 0xFFFF
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
static uint32_t now = 0u;

typedef enum
{
    LEDS_OFF = 0u,
    LEDS_ON
} Leds_Status;
static Leds_Status leds_status = LEDS_ON;

typedef struct
{
    GPIO_TypeDef *port;
    uint16_t pin;
    uint32_t half_period;
    uint32_t last_change_timestamp;
} Led;

Led heartbeat = {
    .port = LED_HEARTBEAT_GPIO_Port,
    .pin = LED_HEARTBEAT_Pin,
    .half_period = 500u,
    .last_change_timestamp = 0u
};

void Led_Heartbeat(const uint32_t now)
{
    if ((uint32_t)(now - heartbeat.last_change_timestamp) >= heartbeat.half_period)
    {
        HAL_GPIO_TogglePin(heartbeat.port, heartbeat.pin);
        heartbeat.last_change_timestamp = now;
    }
}

#define LEDS_SIZE 3
Led LEDS[LEDS_SIZE] = {
    [0] = { .port = LED_1_GPIO_Port, .pin = LED_1_Pin, .half_period = 100u, .last_change_timestamp = 0u },
    [1] = { .port = LED_2_GPIO_Port, .pin = LED_2_Pin, .half_period = 250u, .last_change_timestamp = 0u },
    [2] = { .port = GPIOC, .pin = GPIO_PIN_13, .half_period = 500u, .last_change_timestamp = 0u },
};

typedef struct
{
    GPIO_TypeDef *port;
    uint16_t pin;
    uint16_t mask;
    uint32_t write_idx;
    uint32_t last_update_timestamp;
    uint32_t update_interval;
    uint8_t pressed;
} Button;

Button btn_leds_stop = {
    .port = BTN_1_GPIO_Port,
    .pin = BTN_1_Pin,
    .mask = 0u,
    .write_idx = 0u,
    .last_update_timestamp = 0u,
    .update_interval = 0u,
    .pressed = 0u
};

Button btn_led_2_change_interval = {
    .port = BTN_2_GPIO_Port,
    .pin = BTN_2_Pin,
    .mask = 0u,
    .write_idx = 0u,
    .last_update_timestamp = 0u,
    .update_interval = 100u,
    .pressed = 0u
};

void Button_Read(Button *btn)
{
    uint8_t raw = (HAL_GPIO_ReadPin(btn->port, btn->pin) == GPIO_PIN_RESET);
    if (raw == 1u)
    {
        btn->mask |= (1 << btn->write_idx);
    }
    else
    {
        btn->mask &= ~(1 << btn->write_idx);
    }
    btn->write_idx = (btn->write_idx + 1) % 16;
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
    MX_USART2_UART_Init();
    /* USER CODE BEGIN 2 */

    /* USER CODE END 2 */

    /* Infinite loop */
    /* USER CODE BEGIN WHILE */
    while (1)
    {
        now = HAL_GetTick();
        Led_Heartbeat(now);

        Button_Read(&btn_leds_stop);
        Button_Read(&btn_led_2_change_interval);

        const uint32_t btn_led_2_time_diff = (uint32_t)(now - btn_led_2_change_interval.last_update_timestamp);
        if ((leds_status == LEDS_ON) && (btn_led_2_time_diff >= btn_led_2_change_interval.update_interval))
        {
            if ((btn_led_2_change_interval.mask == BTN_PRESSED) && (btn_led_2_change_interval.pressed == 0u))
            {
                LEDS[1].half_period = now % 1000u;
                btn_led_2_change_interval.last_update_timestamp = now;
                btn_led_2_change_interval.pressed = 1u;
            }
            else if (btn_led_2_change_interval.mask == BTN_IDLE)
            {
                btn_led_2_change_interval.pressed = 0u;
            }
        }

        if (btn_leds_stop.mask != BTN_PRESSED)
        {
            if (leds_status != LEDS_ON)
            {
                leds_status = LEDS_ON;
            }

            for (uint8_t idx = 0; idx < LEDS_SIZE; idx++)
            {
                Led *led = &LEDS[idx];
                if ((uint32_t)(now - led->last_change_timestamp) >= led->half_period)
                {
                    HAL_GPIO_TogglePin(led->port, led->pin);
                    led->last_change_timestamp = now;
                }
            }
        }
        else
        {
            if (leds_status != LEDS_OFF)
            {
                HAL_GPIO_WritePin(LEDS[0].port, LEDS[0].pin, GPIO_PIN_RESET);
                HAL_GPIO_WritePin(LEDS[1].port, LEDS[1].pin, GPIO_PIN_RESET);
                HAL_GPIO_WritePin(LEDS[2].port, LEDS[2].pin, GPIO_PIN_SET);
                leds_status = LEDS_OFF;
            }
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
    RCC_OscInitTypeDef RCC_OscInitStruct = { 0 };
    RCC_ClkInitTypeDef RCC_ClkInitStruct = { 0 };

    /** Configure the main internal regulator output voltage
     */
    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    /** Initializes the RCC Oscillators according to the specified parameters
     * in the RCC_OscInitTypeDef structure.
     */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLM = 12;
    RCC_OscInitStruct.PLL.PLLN = 96;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
    RCC_OscInitStruct.PLL.PLLQ = 4;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        Error_Handler();
    }

    /** Initializes the CPU, AHB and APB buses clocks
     */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
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

    /* USER CODE BEGIN USART2_Init 0 */

    /* USER CODE END USART2_Init 0 */

    /* USER CODE BEGIN USART2_Init 1 */

    /* USER CODE END USART2_Init 1 */
    huart2.Instance = USART2;
    huart2.Init.BaudRate = 115200;
    huart2.Init.WordLength = UART_WORDLENGTH_8B;
    huart2.Init.StopBits = UART_STOPBITS_1;
    huart2.Init.Parity = UART_PARITY_NONE;
    huart2.Init.Mode = UART_MODE_TX_RX;
    huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart2.Init.OverSampling = UART_OVERSAMPLING_16;
    if (HAL_UART_Init(&huart2) != HAL_OK)
    {
        Error_Handler();
    }
    /* USER CODE BEGIN USART2_Init 2 */

    /* USER CODE END USART2_Init 2 */

}

/**
 * @brief GPIO Initialization Function
 * @param None
 * @retval None
 */
static void MX_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = { 0 };
    /* USER CODE BEGIN MX_GPIO_Init_1 */

    /* USER CODE END MX_GPIO_Init_1 */

    /* GPIO Ports Clock Enable */
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOH_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();

    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);

    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(GPIOB, LED_HEARTBEAT_Pin | LED_2_Pin | LED_1_Pin, GPIO_PIN_RESET);

    /*Configure GPIO pin : PC13 */
    GPIO_InitStruct.Pin = GPIO_PIN_13;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    /*Configure GPIO pins : LED_HEARTBEAT_Pin LED_2_Pin LED_1_Pin */
    GPIO_InitStruct.Pin = LED_HEARTBEAT_Pin | LED_2_Pin | LED_1_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /*Configure GPIO pins : BTN_2_Pin BTN_1_Pin */
    GPIO_InitStruct.Pin = BTN_2_Pin | BTN_1_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

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
     printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
