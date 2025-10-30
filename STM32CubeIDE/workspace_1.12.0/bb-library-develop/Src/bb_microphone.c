/*
 * microphone.c
 *
 *  Created on: 11 juil. 2020
 *      Author: flassabe
 */
#include <microphone.h>
#include <microphone_a.h>
#include <stm32f0xx_hal.h>
#include <stdlib.h>
#include <error_handler.h>

#define MIC_THRES 2200
#define SLOPE_TOL 6
#define TIMER_TOL 6
#define ADC_FREQ 58455.114

ADC_HandleTypeDef hadc;
unsigned short _usPeriod;
unsigned short maxLevel = 0;


uint16_t lower_bound = 0xffff;
uint16_t upper_bound = 0;
uint8_t was_initialized = 0;

uint16_t get_mic_upper_bound() {
	return upper_bound;
}

uint16_t get_mic_lower_bound() {
	return lower_bound;
}

/*!
 * \brief are_bounds_initialized returns 0 if no sound was captured and processed
 * by the microphone, 1 else
 * \return 0 or 1 according to microphone initialization.
 */
uint8_t are_bounds_initialized() {
	return was_initialized;
}

/**
  * @brief This function handles ADC and COMP interrupts (COMP interrupts through EXTI lines 21 and 22).
  */
void ADC1_COMP_IRQHandler(void)
{
  /* USER CODE BEGIN ADC1_COMP_IRQn 0 */

  /* USER CODE END ADC1_COMP_IRQn 0 */
  HAL_ADC_IRQHandler(&hadc);
  /* USER CODE BEGIN ADC1_COMP_IRQn 1 */

  /* USER CODE END ADC1_COMP_IRQn 1 */
}

/**
  * @brief ADC Initialization Function
  * @param None
  * @retval None
  */
void MX_ADC_Init(void)
{

  /* USER CODE BEGIN ADC_Init 0 */

  /* USER CODE END ADC_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC_Init 1 */

  /* USER CODE END ADC_Init 1 */
  /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
  */
  hadc.Instance = ADC1;
  hadc.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV1;
  hadc.Init.Resolution = ADC_RESOLUTION_12B;
  hadc.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc.Init.ScanConvMode = ADC_SCAN_DIRECTION_FORWARD;
  hadc.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  hadc.Init.LowPowerAutoWait = DISABLE;
  hadc.Init.LowPowerAutoPowerOff = DISABLE;
  hadc.Init.ContinuousConvMode = ENABLE;
  hadc.Init.DiscontinuousConvMode = DISABLE;
  hadc.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc.Init.DMAContinuousRequests = DISABLE;
  hadc.Init.Overrun = ADC_OVR_DATA_PRESERVED;
  if (HAL_ADC_Init(&hadc) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure for the selected ADC regular channel to be converted.
  */
  sConfig.Channel = ADC_CHANNEL_7;
  sConfig.Rank = ADC_RANK_CHANNEL_NUMBER;
  sConfig.SamplingTime = ADC_SAMPLETIME_239CYCLES_5;
  if (HAL_ADC_ConfigChannel(&hadc, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC_Init 2 */
  HAL_ADC_Start_IT(&hadc);
  /* USER CODE END ADC_Init 2 */

}


//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
float get_microphone_frequency (void)
{
	return ((float)ADC_FREQ/(float)_usPeriod);
}

uint16_t get_microphone_level (void) {
	if (maxLevel>1) maxLevel-=1;
	else maxLevel=0;
	return maxLevel;
}

void updateLevel(unsigned short l) {
	if (l>maxLevel) {
		maxLevel=l;
	} else {
		if (maxLevel>1) maxLevel-=1;
		else maxLevel=0;
	}
}


//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
	static unsigned short prevData = 0;
	static unsigned short newData = 0;
	static unsigned short time = 0;
	static unsigned short slope[10];
	static unsigned short timer[10];
	static unsigned short totalTimer = 0;
	static unsigned char index = 0;
	static unsigned short newSlope = 0;
	static unsigned short maxSlope = 0;
	static unsigned short noMatch = 0;
	unsigned char i;


	prevData = newData;
	newData = HAL_ADC_GetValue(hadc);

	if (newData > upper_bound)
		upper_bound = newData;
	if (newData < lower_bound)
		lower_bound = newData;
	was_initialized = 1;

	updateLevel(newData);

	// if increasing and crossing midpoint
	if ((prevData < MIC_THRES) && (newData >= MIC_THRES))
	{
		newSlope = newData - prevData;
		// if slopes are ==
		if (abs(newSlope - maxSlope) < SLOPE_TOL)
		{
			// record new date and reset time
			slope[index] = newSlope;
			timer[index] = time;
			time = 0;
			// new max slope just reset
			if (index == 0)
			{
				noMatch = 0;
				index++;
			}
			// if timer duration and slopes match
			else if ((abs(timer[0] - timer[index] < TIMER_TOL)) && (abs(slope[0] - newSlope) < SLOPE_TOL))
			{
				// sum timer values
				totalTimer = 0;
				for (i=0;i<index;i++)
				{
					totalTimer += timer[i];
				}
				_usPeriod = totalTimer;
				// reset new zero index values to compare with
				timer[0] = timer[index];
				slope[0] = slope[index];
				index = 1;
				noMatch = 0;
			}
			// crossing midpoint but not match
			else
			{
				index++;
				if (index > 9)
				{
					index = 0;
					noMatch = 0;
					maxSlope = 0;
				}
			}
		}
		// if new slope is much larger than max slope
		else if (newSlope > maxSlope)
		{
			maxSlope = newSlope;
			time = 0;
			noMatch = 0;
			index = 0;
		}
		// slope not steep enough
		else
		{
			noMatch++;
			if (noMatch > 9)
			{
				index = 0;
				noMatch = 0;
				maxSlope = 0;
			}
		}
	}
	time++;
}


