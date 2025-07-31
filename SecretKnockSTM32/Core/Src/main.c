/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define MAX_KNOCKS 20
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;

/* USER CODE BEGIN PV */
uint8_t secretCode[MAX_KNOCKS] = {50, 25, 25, 50, 100, 50};
uint16_t knockReadings[MAX_KNOCKS];

int threshold = 500;
const int rejectValue = 25;
const int averageRejectValue = 15;
const int knockFadeTime = 150;
const int lockOperateTime = 2500;
const int knockComplete = 1200;

int knockSensorValue = 0;
uint8_t programModeActive = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_ADC1_Init(void);
/* USER CODE BEGIN PFP */
void doorUnlock(uint32_t duration);
void chirp(uint32_t playTime, uint32_t delayTime);

uint32_t Read_Piezo_ADC(void);
void knockDelay(void);
void listenToSecretKnock(void);
bool validateKnock(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
uint32_t Read_Piezo_ADC(void)
{
    uint32_t value = 0;
    HAL_ADC_Start(&hadc1);
    if (HAL_ADC_PollForConversion(&hadc1, 10) == HAL_OK) {
        value = HAL_ADC_GetValue(&hadc1);
    }
    HAL_ADC_Stop(&hadc1);
    return value;
}

void knockDelay(void)
{
    for (int i = 0; i < knockFadeTime / 20; i++) {
        HAL_Delay(10);
        Read_Piezo_ADC();
        HAL_Delay(10);
    }
}

void listenToSecretKnock(void)
{
    memset(knockReadings, 0, sizeof(knockReadings));
    int currentKnockNumber = 0;
    uint32_t startTime = HAL_GetTick();
    uint32_t now = HAL_GetTick();

    do {
        knockSensorValue = Read_Piezo_ADC();
        if (knockSensorValue > threshold) {
            now = HAL_GetTick();
            knockReadings[currentKnockNumber] = now - startTime;
            currentKnockNumber++;
            startTime = now;
            knockDelay();
        }
        now = HAL_GetTick();
    } while ((now - startTime < knockComplete) && (currentKnockNumber < MAX_KNOCKS));

    if (!programModeActive) {
        if (validateKnock()) {
            doorUnlock(lockOperateTime);
        }
    } else {
        validateKnock(); // sadece kayıt için
    }
}

bool validateKnock(void)
{
    int maxInterval = 0, currentCount = 0, storedCount = 0, totalDifference = 0;

    for (int i = 0; i < MAX_KNOCKS; i++) {
        if (knockReadings[i] > 0) currentCount++;
        if (secretCode[i] > 0) storedCount++;
        if (knockReadings[i] > maxInterval) maxInterval = knockReadings[i];
    }

    if (programModeActive) {
        for (int i = 0; i < MAX_KNOCKS; i++) {
            secretCode[i] = (uint8_t)((knockReadings[i] * 100) / maxInterval);
        }
        programModeActive = 0;
        return false;
    }

    if (currentCount != storedCount) return false;

    for (int i = 0; i < MAX_KNOCKS; i++) {
        int normalized = (knockReadings[i] * 100) / maxInterval;
        int diff = abs((int)secretCode[i] - normalized);
        if (diff > rejectValue) return false;
        totalDifference += diff;
    }

    return (totalDifference / storedCount <= averageRejectValue);
}
/* USER CODE END 0 */

int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */
  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();
  MX_ADC1_Init();

  /* USER CODE BEGIN 2 */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_RESET);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    knockSensorValue = Read_Piezo_ADC();

    if (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13) == GPIO_PIN_SET) {
      HAL_Delay(100);
      if (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13) == GPIO_PIN_SET) {
          programModeActive = !programModeActive;
          HAL_Delay(500);
      }
    }

    if (knockSensorValue > threshold) {
        listenToSecretKnock();
    }

    HAL_Delay(10);
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/* USER CODE BEGIN 4 */
void chirp(uint32_t playTime, uint32_t delayTime)
{
    for(uint32_t i = 0; i < (playTime * 1000) / delayTime; i++)
    {
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_SET);
        HAL_Delay(0.001); // delayMicroseconds karşılığı
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_RESET);
    }
}

void doorUnlock(uint32_t duration)
{
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET); // Solenoid ON
    HAL_Delay(duration);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET); // Solenoid OFF
}
void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();

  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = GPIO_PIN_0;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

void MX_ADC1_Init(void)
{
  __HAL_RCC_ADC1_CLK_ENABLE();

  ADC_ChannelConfTypeDef sConfig = {0};
  hadc1.Instance = ADC1;
  hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc1.Init.ContinuousConvMode = ENABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 1;
  HAL_ADC_Init(&hadc1);

  sConfig.Channel = ADC_CHANNEL_0;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
  HAL_ADC_ConfigChannel(&hadc1, &sConfig);
}

void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  HAL_RCC_OscConfig(&RCC_OscInitStruct);

  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK |
                                RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0);
}

/* USER CODE END 4 */
