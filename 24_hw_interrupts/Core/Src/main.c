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
#include <stdarg.h>
#include <stdio.h>

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
typedef struct
{
    GPIO_TypeDef *port;
    uint16_t pin;
    uint32_t half_period;
    uint32_t last_update_timestamp;
} Led;

typedef enum
{
    BUTTON_EVENT_HANDLED = 0u,
    BUTTON_EVENT_PENDING,
    BUTTON_EVENT_SHORT_PENDING,
    BUTTON_EVENT_LONG_PENDING,
    BUTTON_EVENT_DOUBLE_PENDING,
} Button_Event;

typedef enum
{
    BUTTON_STATE_IDLE = 0u,
    BUTTON_STATE_PRESSED,
    BUTTON_STATE_HELD,
    BUTTON_STATE_RELEASED,
} Button_State;

typedef struct
{
    GPIO_TypeDef *port;
    uint16_t pin;
    uint32_t press_count;
    uint8_t raw;
    uint8_t stable;
    uint8_t in_debounce;
    Button_Event event;
    uint32_t last_read_timestamp;
    Button_State state;

    // btn_6 -- multiclick
    uint8_t is_double;
    uint32_t is_double_timestamp;
    uint32_t fall_timestamp;
    uint32_t rise_timestamp;
} Button;

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define BUTTON_PRESS_DOUBLE_DURATION_MS 350u
#define BUTTON_PRESS_SHORT_DURATION_MS 1000u
#define BUTTON_PRESS_LONG_DURATION_MS 2000u
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef htim2;

UART_HandleTypeDef huart2;
DMA_HandleTypeDef hdma_usart2_tx;

/* USER CODE BEGIN PV */
static uint32_t now = 0u;

static Led heartbeat = {
	.port = LED_HEARTBEAT_GPIO_Port,
	.pin = LED_HEARTBEAT_Pin,
	.half_period = 1000u,
    .last_update_timestamp = 0u
};

// no debounce
static volatile Button btn_1_exti = {
	.port = BTN_1_EXTI_GPIO_Port,
	.pin = BTN_1_EXTI_Pin,
	.press_count = 0u
};

// 50ms debounce via timer
static volatile Button btn_2_exti = {
	.port = BTN_2_EXTI_GPIO_Port,
	.pin = BTN_2_EXTI_Pin,
	.in_debounce = 0u,
	.raw = 0u,
	.stable = 0u
};

// event/state based debounce
static volatile Button btn_3_exti = {
	.port = BTN_3_EXTI_GPIO_Port,
	.pin = BTN_3_EXTI_Pin,
	.press_count = 0u,
	.event = BUTTON_EVENT_HANDLED,
};

// polling + debounce
static Button btn_4 = {
	.port = BTN_4_GPIO_Port,
	.pin = BTN_4_Pin,
	.press_count = 0u,
	.last_read_timestamp = 0u,
	.raw = 0u,
	.state = BUTTON_STATE_IDLE,
};

static Button btn_5_rc = {
	.port = BTN_5_RC_GPIO_Port,
	.pin = BTN_5_RC_Pin,
	.press_count = 0u,
	.in_debounce = 0u,
	.raw = 0u,
}
;
static volatile Button btn_6_exti_multi = {
	.port = BTN_6_EXTI_MULTI_GPIO_Port,
	.pin = BTN_6_EXTI_MULTI_Pin,
	.in_debounce = 0u,
	.raw = 0u,
	.stable = 0u,
	.event = BUTTON_EVENT_HANDLED,
	.state = BUTTON_STATE_IDLE,
};

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_TIM2_Init(void);
static void MX_USART2_UART_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void Log_Write(UART_HandleTypeDef *huart, const char *fmt, ...)
{
    assert_param(huart != NULL);
    assert_param(fmt != NULL);
    assert_param(strlen(fmt) < 256);

    uint32_t now = HAL_GetTick();
    char buffer[256] = { 0 };

    uint32_t pos = sprintf(buffer, "[%u] ", now);

    va_list args;
    va_start(args, fmt);
    pos += vsprintf(buffer + pos, fmt, args);
    va_end(args);

    buffer[pos++] = '\r';
    buffer[pos++] = '\n';
    buffer[pos++] = '\0';

    HAL_UART_Transmit(huart, (uint8_t *)buffer, pos, 100);
}

void Led_Heartbeat(const uint32_t now)
{
    if ((uint32_t)(now - heartbeat.last_update_timestamp) >= heartbeat.half_period)
    {
        HAL_GPIO_TogglePin(heartbeat.port, heartbeat.pin);
        heartbeat.last_update_timestamp = now;
    }
}

const char *Button_State_To_String(const Button_State state)
{
    switch (state)
    {
    case BUTTON_STATE_IDLE:
        return "BUTTON_STATE_IDLE";
    case BUTTON_STATE_PRESSED:
        return "BUTTON_STATE_PRESSED";
    case BUTTON_STATE_HELD:
        return "BUTTON_STATE_HELD";
    case BUTTON_STATE_RELEASED:
        return "BUTTON_STATE_RELEASED";
    default:
        return "Non existent state";
    }
}

const char *Button_Event_To_String(const Button_Event event)
{
    switch (event)
    {
    case BUTTON_EVENT_HANDLED:
        return "BUTTON_EVENT_HANDLED";
    case BUTTON_EVENT_PENDING:
        return "BUTTON_EVENT_PENDING";
    case BUTTON_EVENT_SHORT_PENDING:
        return "BUTTON_EVENT_SHORT_PENDING";
    case BUTTON_EVENT_LONG_PENDING:
        return "BUTTON_EVENT_LONG_PENDING";
    case BUTTON_EVENT_DOUBLE_PENDING:
        return "BUTTON_EVENT_DOUBLE_PENDING";
    default:
        return "Non existent event";
    }
}

void Button_3_Process_Event(volatile Button *btn)
{
    if (btn->event == BUTTON_EVENT_PENDING)
    {
        btn->press_count++;
        btn->event = BUTTON_EVENT_HANDLED;
    }
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    switch (GPIO_Pin)
    {
    case BTN_1_EXTI_Pin:
        // do nothing
        btn_1_exti.press_count++;
        break;
    case BTN_2_EXTI_Pin:
        btn_2_exti.raw = (HAL_GPIO_ReadPin(btn_2_exti.port, btn_2_exti.pin) == GPIO_PIN_RESET);
        btn_2_exti.in_debounce = 1u;
        break;
    case BTN_3_EXTI_Pin:
        if (btn_3_exti.event == BUTTON_EVENT_HANDLED)
        {
            btn_3_exti.event = BUTTON_EVENT_PENDING;
        }
        break;
    case BTN_6_EXTI_MULTI_Pin:
        btn_6_exti_multi.raw = (HAL_GPIO_ReadPin(btn_6_exti_multi.port, btn_6_exti_multi.pin) == GPIO_PIN_RESET);
        btn_6_exti_multi.in_debounce = 1u;
        break;
    }
}

static void Button_Timer_Debounce(volatile Button *btn)
{
    const uint8_t raw = (HAL_GPIO_ReadPin(btn->port, btn->pin) == GPIO_PIN_RESET);
    if ((btn->in_debounce == 1u) && (raw == btn->raw))
    {
        btn->stable = raw;
        btn->press_count++;
    }
    btn->in_debounce = 0u;
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM2)
    {
        Button_Timer_Debounce(&btn_2_exti);
        Button_Timer_Debounce(&btn_6_exti_multi);
    }
}

void Button_6_EXTI_Multi_Handle(volatile Button *btn, const uint32_t now)
{
    if (btn->event != BUTTON_EVENT_HANDLED)
        return;

    switch (btn->state)
    {
    case BUTTON_STATE_IDLE:
        if (btn->stable == 1u)
        {
            btn->state = BUTTON_STATE_PRESSED;
            btn->fall_timestamp = now;
        }

        // short button event is deferred in case of double click
        if (btn->is_double == 1u)
        {
            if ((uint32_t)(now - btn->is_double_timestamp) >= BUTTON_PRESS_DOUBLE_DURATION_MS)
            {
                btn->event = BUTTON_EVENT_SHORT_PENDING;
                btn->is_double = 0u;
            }
        }
        break;
    case BUTTON_STATE_PRESSED:
    case BUTTON_STATE_HELD:
        if (btn->stable == 1u)
        {
            btn->state = BUTTON_STATE_HELD;
        }
        else
        {
            btn->state = BUTTON_STATE_RELEASED;
        }
        break;
    case BUTTON_STATE_RELEASED:
        btn->state = BUTTON_STATE_IDLE;
        btn->rise_timestamp = now;

        const uint32_t time_diff = (uint32_t)(btn->rise_timestamp - btn->fall_timestamp);
        if (time_diff <= BUTTON_PRESS_SHORT_DURATION_MS)
        {
            if (btn->is_double == 1u)
            {
                btn->event = BUTTON_EVENT_DOUBLE_PENDING;
                btn->is_double = 0u;
            }
            else
            {
                btn->is_double = 1u;
                btn->is_double_timestamp = now;
            }
        }
        else if (time_diff > BUTTON_PRESS_SHORT_DURATION_MS)
        {
            btn->event = BUTTON_EVENT_LONG_PENDING;
        }
        break;
    default:
        break;
    }
}

void Button_4_sm_debouce(Button *btn, const uint32_t now)
{
    if ((uint32_t)(now - btn->last_read_timestamp) >= 10u)
    {
        btn->last_read_timestamp = now;
        btn->raw = (HAL_GPIO_ReadPin(btn->port, btn->pin) == GPIO_PIN_RESET);
        switch (btn->state)
        {
        case BUTTON_STATE_IDLE:
            if (btn->raw == 1u)
            {
                btn->state = BUTTON_STATE_PRESSED;
            }
            break;
        case BUTTON_STATE_PRESSED:
            if (btn->raw == 1u)
            {
                btn->press_count++;
            }
        case BUTTON_STATE_HELD:
            if (btn->raw == 1u)
            {
                btn->state = BUTTON_STATE_HELD;
            }
            else
            {
                btn->state = BUTTON_STATE_IDLE;
            }
            break;
        default:
            break;
        }
    }
}

void Button_5_RC_Read(Button *btn)
{
    btn->raw = (HAL_GPIO_ReadPin(btn->port, btn->pin) == GPIO_PIN_RESET);
    if (btn->raw == 1u)
    {
        if (btn->in_debounce == 0u)
        {
            btn->press_count++;
            btn->in_debounce = 1u;
        }
    }
    else
    {
        btn->in_debounce = 0u;
    }
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
    MX_DMA_Init();
    MX_TIM2_Init();
    MX_USART2_UART_Init();
    /* USER CODE BEGIN 2 */
    uint32_t last_update = 0u;

    HAL_TIM_Base_Start_IT(&htim2);

    Log_Write(&huart2, "Board started...");
    /* USER CODE END 2 */

    /* Infinite loop */
    /* USER CODE BEGIN WHILE */
    while (1)
    {
        now = HAL_GetTick();
        Led_Heartbeat(now);

        Button_3_Process_Event(&btn_3_exti);
        Button_4_sm_debouce(&btn_4, now);
        Button_5_RC_Read(&btn_5_rc);
        Button_6_EXTI_Multi_Handle(&btn_6_exti_multi, now);

        switch (btn_6_exti_multi.event)
        {
        case BUTTON_EVENT_SHORT_PENDING:
            Log_Write(&huart2,
                      ">>>btn_6_exti_multi: short | press duration: %u | rise: %u | fall: %u\r\n\t"
                      "state: %s | event: %s\r\n",
                      (uint32_t) (btn_6_exti_multi.rise_timestamp - btn_6_exti_multi.fall_timestamp),
                      btn_6_exti_multi.rise_timestamp, btn_6_exti_multi.fall_timestamp,
                      Button_State_To_String(btn_6_exti_multi.state), Button_Event_To_String(btn_6_exti_multi.event));
            btn_6_exti_multi.event = BUTTON_EVENT_HANDLED;
            break;
        case BUTTON_EVENT_LONG_PENDING:
            Log_Write(&huart2,
                      ">>>btn_6_exti_multi: long | press duration: %u | rise: %u | fall: %u\r\n\t"
                      "state: %s | event: %s\r\n",
                      (uint32_t) (btn_6_exti_multi.rise_timestamp - btn_6_exti_multi.fall_timestamp),
                      btn_6_exti_multi.rise_timestamp, btn_6_exti_multi.fall_timestamp,
                      Button_State_To_String(btn_6_exti_multi.state), Button_Event_To_String(btn_6_exti_multi.event));
            btn_6_exti_multi.event = BUTTON_EVENT_HANDLED;
            break;
        case BUTTON_EVENT_DOUBLE_PENDING:
            Log_Write(&huart2,
                      ">>>btn_6_exti_multi: double | press duration: %u | rise: %u | fall: %u\r\n\t"
                      "state: %s | event: %s\r\n",
                      (uint32_t) (btn_6_exti_multi.rise_timestamp - btn_6_exti_multi.fall_timestamp),
                      btn_6_exti_multi.rise_timestamp, btn_6_exti_multi.fall_timestamp,
                      Button_State_To_String(btn_6_exti_multi.state), Button_Event_To_String(btn_6_exti_multi.event));
            btn_6_exti_multi.event = BUTTON_EVENT_HANDLED;
            break;
        case BUTTON_EVENT_PENDING:
        case BUTTON_EVENT_HANDLED:
            break;
        }

        if ((uint32_t) (now - last_update) >= 1000u)
        {
            last_update = now;
            Log_Write(&huart2, "btn_1_exti: raw: %u | stable: %u | in_debounce: %u | press_count: %u",
                      btn_1_exti.raw, btn_1_exti.stable, btn_1_exti.in_debounce, btn_1_exti.press_count);
            Log_Write(&huart2, "btn_2_exti: raw: %u | stable: %u | in_debounce: %u | press_count: %u",
                      btn_2_exti.raw, btn_2_exti.stable, btn_2_exti.in_debounce, btn_2_exti.press_count);
            Log_Write(&huart2, "btn_3_exti: raw: %u | stable: %u | in_debounce: %u | press_count: %u",
                      btn_3_exti.raw, btn_3_exti.stable, btn_3_exti.in_debounce, btn_3_exti.press_count);
            Log_Write(&huart2, "btn_4: raw: %u | stable: %u | in_debounce: %u | press_count: %u",
                      btn_4.raw, btn_4.stable, btn_4.in_debounce, btn_4.press_count);
            Log_Write(&huart2, "btn_5_rc: raw: %u | stable: %u | in_debounce: %u | press_count: %u\r\n",
                      btn_5_rc.raw, btn_5_rc.stable, btn_5_rc.in_debounce, btn_5_rc.press_count);
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
 * @brief TIM2 Initialization Function
 * @param None
 * @retval None
 */
static void MX_TIM2_Init(void)
{

    /* USER CODE BEGIN TIM2_Init 0 */

    /* USER CODE END TIM2_Init 0 */

    TIM_ClockConfigTypeDef sClockSourceConfig = { 0 };
    TIM_MasterConfigTypeDef sMasterConfig = { 0 };

    /* USER CODE BEGIN TIM2_Init 1 */

    /* USER CODE END TIM2_Init 1 */
    htim2.Instance = TIM2;
    htim2.Init.Prescaler = 999;
    htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim2.Init.Period = 4999;
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
    /* USER CODE BEGIN TIM2_Init 2 */

    /* USER CODE END TIM2_Init 2 */

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
 * Enable DMA controller clock
 */
static void MX_DMA_Init(void)
{

    /* DMA controller clock enable */
    __HAL_RCC_DMA1_CLK_ENABLE();

    /* DMA interrupt init */
    /* DMA1_Stream6_IRQn interrupt configuration */
    HAL_NVIC_SetPriority(DMA1_Stream6_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(DMA1_Stream6_IRQn);

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
    HAL_GPIO_WritePin(LED_HEARTBEAT_GPIO_Port, LED_HEARTBEAT_Pin, GPIO_PIN_RESET);

    /*Configure GPIO pin : LED_HEARTBEAT_Pin */
    GPIO_InitStruct.Pin = LED_HEARTBEAT_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(LED_HEARTBEAT_GPIO_Port, &GPIO_InitStruct);

    /*Configure GPIO pins : BTN_1_EXTI_Pin BTN_2_EXTI_Pin BTN_3_EXTI_Pin */
    GPIO_InitStruct.Pin = BTN_1_EXTI_Pin | BTN_2_EXTI_Pin | BTN_3_EXTI_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /*Configure GPIO pins : BTN_4_Pin BTN_5_RC_Pin */
    GPIO_InitStruct.Pin = BTN_4_Pin | BTN_5_RC_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /*Configure GPIO pin : BTN_6_EXTI_MULTI_Pin */
    GPIO_InitStruct.Pin = BTN_6_EXTI_MULTI_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(BTN_6_EXTI_MULTI_GPIO_Port, &GPIO_InitStruct);

    /* EXTI interrupt init*/
    HAL_NVIC_SetPriority(EXTI3_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(EXTI3_IRQn);

    HAL_NVIC_SetPriority(EXTI4_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(EXTI4_IRQn);

    HAL_NVIC_SetPriority(EXTI9_5_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

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
