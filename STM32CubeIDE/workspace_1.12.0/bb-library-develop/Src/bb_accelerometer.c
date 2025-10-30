//------------------------------------------------------------------------------
// include
//------------------------------------------------------------------------------
#include <hwAccelerometer.h>
#include "stm32f0xx_hal.h"

//------------------------------------------------------------------------------
// define
//------------------------------------------------------------------------------
#define ACCEL_ADDR_W 0x98
#define ACCEL_ADDR_R 0x99


//------------------------------------------------------------------------------
// Variables
//------------------------------------------------------------------------------
extern I2C_HandleTypeDef hi2c1;

tAccel _tAccelAcc;
uint8_t ucNewData;




//------------------------------------------------------------------------------
// int newHWAccelData(void)
//------------------------------------------------------------------------------
int newHWAccelData(void)
{
	if (ucNewData)
	{
		ucNewData = 0;
		return (1);
	}
	else
	{
		return (0);
	}
}



//------------------------------------------------------------------------------
// void getHWAccel(tAccel *ptAccelData)
//------------------------------------------------------------------------------
void getHWAccel(tAccel *ptAccelData)
{
	ptAccelData->ucX = _tAccelAcc.ucX;
	ptAccelData->ucY = _tAccelAcc.ucY;
	ptAccelData->ucZ = _tAccelAcc.ucZ;
	ptAccelData->ucStatus = _tAccelAcc.ucStatus;
}


//------------------------------------------------------------------------------
// void initHWAccel(void)
//------------------------------------------------------------------------------
void initHWAccel(void)
{
	uint8_t tabucBuffer[2];
	static HAL_StatusTypeDef HAL_Status_Retour;

	// set mode to STANDBY if it isn't already (can't update registers in ACTIVE mode!)
	tabucBuffer[0] = 0x07;
	tabucBuffer[1] = 0x18;
	HAL_Status_Retour = HAL_I2C_Master_Transmit(&hi2c1, ACCEL_ADDR_W, &tabucBuffer[0], sizeof(tabucBuffer), 500);

	// set sleep mode
	tabucBuffer[0] = 0x05;
	tabucBuffer[1] = 0x00;	// no sleep
	HAL_Status_Retour = HAL_I2C_Master_Transmit(&hi2c1, ACCEL_ADDR_W, &tabucBuffer[0], sizeof(tabucBuffer), 500);

	// set interrupts
	tabucBuffer[0] = 0x06;
	tabucBuffer[1] = 0xEF;	// 0x06 Interrupt Setup Register -> PDINT | PLINT | FBINT
	HAL_Status_Retour = HAL_I2C_Master_Transmit(&hi2c1, ACCEL_ADDR_W, &tabucBuffer[0], sizeof(tabucBuffer), 500);

	// set filter rate
	tabucBuffer[0] = 0x08;
	tabucBuffer[1] = 0x08;	// 0x08 Sample Register -> Fil[0:2]
	//HAL_Status_Retour = HAL_I2C_Master_Transmit(&hi2c1, ACCEL_ADDR_W, &tabucBuffer[0], sizeof(tabucBuffer), 500);

	// set tap detection
	tabucBuffer[0] = 0x09;
	tabucBuffer[1] = 0x10;	// 0x09	Pulse detection -> +- 4 counts
	//HAL_Status_Retour = HAL_I2C_Master_Transmit(&hi2c1, ACCEL_ADDR_W, &tabucBuffer[0], sizeof(tabucBuffer), 500);

	// set tap debounce
	tabucBuffer[0] = 0x0A;
	tabucBuffer[1] = 0x10;	// 0x0A Tap debounce ->	4 detections
	//HAL_Status_Retour = HAL_I2C_Master_Transmit(&hi2c1, ACCEL_ADDR_W, &tabucBuffer[0], sizeof(tabucBuffer), 500);

	// enable the accelerometer
	tabucBuffer[0] = 0x07;
	tabucBuffer[1] = 0x19;
	HAL_Status_Retour = HAL_I2C_Master_Transmit(&hi2c1, ACCEL_ADDR_W, &tabucBuffer[0], sizeof(tabucBuffer), 500);
}



//------------------------------------------------------------------------------
// void setAccelRegister(unsigned char ucOne, unsigned char ucTwo)
//------------------------------------------------------------------------------
void setAccelRegister(uint8_t ucOne, uint8_t ucTwo)
{
	uint8_t tabucBuffer[2];

	tabucBuffer[0] = ucOne;
	tabucBuffer[1] = ucTwo;

  HAL_I2C_Master_Transmit(&hi2c1, ACCEL_ADDR_W, &tabucBuffer[0], sizeof(tabucBuffer), 500);
}

/**
 * Actualise la position de l'accéléromètre et calcul les angles.
 *   (cf voir mon tuto Orientation01062019.pdf)
 * Ajout NB 01/06/2019
 */
#define ACCELEROMETER_ANGLE_STEP 4.090909091 // (90 degrés / 22)
void refreshXYZ(tAccel * ptAccelData)
  {
    uint8_t buffer[3]; // octets (non signés)
    int valeurs[3]; // entiers signés
    static HAL_StatusTypeDef HAL_Status_Retour;
    buffer[0] = 0x00;
    __disable_irq();
    HAL_Status_Retour = HAL_I2C_Master_Transmit(&hi2c1, ACCEL_ADDR_W, &buffer[0], 1, 500);
    HAL_Status_Retour = HAL_I2C_Master_Receive(&hi2c1, ACCEL_ADDR_R, buffer, 3, 500);
    __enable_irq();
    for (uint8_t i=0;i<3;i++)
      {
        valeurs[i]=buffer[i] & 0x3F;
        if (valeurs[i]> 31)
          {
            valeurs[i]=valeurs[i]-64;
          }
        // d'après mes mesures, on peut aller jusqu'à 22,23,24... donc on va filtrer
        if (valeurs[i]> 22)
          {
            valeurs[i]=22;
          }
        else if (valeurs[i]<-23)
          {
            valeurs[i]=-23;
          }
      }
    ptAccelData->ucX = buffer[0];
    ptAccelData->ucY = buffer[1];
    ptAccelData->ucZ = buffer[2];
    ptAccelData->angleX=ACCELEROMETER_ANGLE_STEP*(float)valeurs[0];
    ptAccelData->angleY=ACCELEROMETER_ANGLE_STEP*(float)valeurs[1];
    ptAccelData->angleZ=ACCELEROMETER_ANGLE_STEP*(float)valeurs[2];
  }



//------------------------------------------------------------------------------
// void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
//------------------------------------------------------------------------------
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	uint8_t tabucBuffer[4];
	static HAL_StatusTypeDef HAL_Status_Retour;

	if (GPIO_Pin == GPIO_PIN_6)
  {
		// initiate a read with a write before
		tabucBuffer[0] = 0x00;
		HAL_Status_Retour = HAL_I2C_Master_Transmit(&hi2c1, ACCEL_ADDR_W, &tabucBuffer[0], 1, 500);
		HAL_Status_Retour = HAL_I2C_Master_Receive(&hi2c1, ACCEL_ADDR_R, &tabucBuffer[0], sizeof(tabucBuffer), 500);

		// I2C reads in as unsigned - we need to convert back to 2s complement (+- 32)
		_tAccelAcc.ucX = (tabucBuffer[0] & 0x3F);
		_tAccelAcc.ucY = (tabucBuffer[1] & 0x3F);
		_tAccelAcc.ucZ = (tabucBuffer[2] & 0x3F);
		_tAccelAcc.ucStatus = tabucBuffer[3];

		ucNewData = 1;

		/* USER CODE BEGIN */

		/* USER CODE END */
	}
}
