#include "main.h"
#include <string.h>
#include <stdio.h>
#include <math.h>

I2C_HandleTypeDef hi2c1;

UART_HandleTypeDef huart2;

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_USART2_UART_Init(void);

void  hardIronCal(float* xCal, float* yCal, float* zCal);
float getDirectionAngle(int16_t xMag, int16_t yMag, int16_t zMag, float xCal, float yCal, float zCal);
void printDirection(float D);
void setupMag();
float magnetometerData(float xCal,float yCal,float zCal);

void getGpsData(float* lat, float* longi, float* time, float* date, char* longDir, char* latDir);

//MAGNETOMETER
//Device Address
uint8_t magAddress = 0x1E;

//Register Addresses
uint8_t dataRegister = 0x68;
uint8_t cfgRegA = 0x60;
uint8_t cfgRegC = 0x62;

//Values to send
uint8_t initRegA = 0x0C;
uint8_t disRegA = 0x0E;
uint8_t initRegC = 0x10;

//GPS Data
//Address
uint16_t gpsAddress = 0x42;

int main(void)
{
  HAL_Init();

  SystemClock_Config();
  MX_GPIO_Init();
  MX_I2C1_Init();
  MX_USART2_UART_Init();

  //GPS
  float lat = -1;
  float longi = -1;
  float time = -1;
  float date = -1;
  char longiDir = 'x';
  char latDir = 'x';

  for (int i = 0; i < 50; i++) {
	  getGpsData(&lat, &longi, &time, &date, &longiDir, &latDir);
  }

  //MAGNEOTMETER
  float xCal, yCal, zCal;
  xCal = 0;
  yCal = 0;
  zCal = 0;
  setupMag();
  hardIronCal(&xCal, &yCal, &zCal);

  //Direction Angle
  float direction = 0;

  while (1)
  {
	  direction = magnetometerData(xCal, yCal, zCal);
  }
}


void getGpsData(float* lat, float* longi, float* time, float* date, char* longDir, char* latDir) {
	uint8_t gpsDataTx [10] = {0}; //Data to send to GPS module
	uint8_t gpsDataReady [2] = {0}; //Data to check if gps module ready
	uint8_t gpsDataRx1 [32] = {0}; //Buffer 1
	uint8_t gpsDataRx2 [32] = {0}; //Buffer 2
	uint8_t gpsDataRx3 [32] = {0}; //Buffer 3
	uint8_t gpsDataRx4 [4] = {0}; //Buffer 4

	//////////////////////////////////////////////////////////////////////////////////////////
	//INITIALIZATION
	gpsDataTx[0] = 0xB5;
	gpsDataTx[1] = 0x62;
	gpsDataTx[2] = 0xF0;
	gpsDataTx[3] = 0x04;

	while (HAL_I2C_GetState(&hi2c1) != HAL_I2C_STATE_READY){}
	while (HAL_I2C_Master_Transmit(&hi2c1, (gpsAddress << 1), gpsDataTx, 4, 20) != HAL_OK){}

//		/////////////////////////////////////////////////////////////////////////////////////////
//
//		//WAITING FOR DATA READY
	gpsDataReady[0] = 0;
	gpsDataReady[1] = 0;
	while ((gpsDataReady[0] == 0) && (gpsDataReady[1] == 0)) //We leave as soon as read something non zero
	{
		gpsDataTx[0] = 0xFD;
		HAL_I2C_Master_Transmit(&hi2c1, (gpsAddress << 1), gpsDataTx, 1, HAL_MAX_DELAY);
		gpsDataReady[0] = 0;
		gpsDataReady[1] = 0;
		HAL_I2C_Master_Receive(&hi2c1, (gpsAddress << 1 | 1), gpsDataReady, 2, HAL_MAX_DELAY);
	}


//		//Buffer 1
	gpsDataTx[0] = 0xFF;
	while (HAL_I2C_GetState(&hi2c1) != HAL_I2C_STATE_READY){}
	while (HAL_I2C_Master_Transmit(&hi2c1, (gpsAddress << 1), gpsDataTx, 1, 20) != HAL_OK){}
	while (HAL_I2C_GetState(&hi2c1) != HAL_I2C_STATE_READY){}
	while (HAL_I2C_Master_Receive(&hi2c1, (gpsAddress << 1 | 1), gpsDataRx1, 32, HAL_MAX_DELAY)){}
//
//		//Buffer 2
	gpsDataTx[0] = 0xFF;
	while (HAL_I2C_GetState(&hi2c1) != HAL_I2C_STATE_READY){}
	while (HAL_I2C_Master_Transmit(&hi2c1, (gpsAddress << 1), gpsDataTx, 1, 20) != HAL_OK){}
	while (HAL_I2C_GetState(&hi2c1) != HAL_I2C_STATE_READY){}
	while (HAL_I2C_Master_Receive(&hi2c1, (gpsAddress << 1 | 1), gpsDataRx2, 32, HAL_MAX_DELAY)){}
//
//		//Buffer 3
	gpsDataTx[0] = 0xFF;
	while (HAL_I2C_GetState(&hi2c1) != HAL_I2C_STATE_READY){}
	while (HAL_I2C_Master_Transmit(&hi2c1, (gpsAddress << 1), gpsDataTx, 1, 20) != HAL_OK){}
	while (HAL_I2C_GetState(&hi2c1) != HAL_I2C_STATE_READY){}
	while (HAL_I2C_Master_Receive(&hi2c1, (gpsAddress << 1 | 1), gpsDataRx3, 32, HAL_MAX_DELAY)){}

	//Buffer 4
	gpsDataTx[0] = 0xFF;
	while (HAL_I2C_GetState(&hi2c1) != HAL_I2C_STATE_READY){}
	while (HAL_I2C_Master_Transmit(&hi2c1, (gpsAddress << 1), gpsDataTx, 1, 20) != HAL_OK){}
	while (HAL_I2C_GetState(&hi2c1) != HAL_I2C_STATE_READY){}
	while (HAL_I2C_Master_Receive(&hi2c1, (gpsAddress << 1 | 1), gpsDataRx4, 4, HAL_MAX_DELAY)){}


	//Create concaterated buffer
	 size_t totalSize = sizeof(gpsDataRx1) + sizeof(gpsDataRx2) + sizeof(gpsDataRx3) + sizeof(gpsDataRx4);

	// Allocate memory for the concatenated array
	uint8_t *concatenatedArray = (uint8_t *)malloc(totalSize);

	// Copy the contents of each array into the concatenated array
	size_t offset = 0;
	memcpy(concatenatedArray + offset, gpsDataRx1, sizeof(gpsDataRx1));
	offset += sizeof(gpsDataRx1);
	memcpy(concatenatedArray + offset, gpsDataRx2, sizeof(gpsDataRx2));
	offset += sizeof(gpsDataRx2);
	memcpy(concatenatedArray + offset, gpsDataRx3, sizeof(gpsDataRx3));
	offset += sizeof(gpsDataRx3);
	memcpy(concatenatedArray + offset, gpsDataRx4, sizeof(gpsDataRx4));


	//String comparisons
	const char* GNRMC = "NRMC,";
	const char* minus1 = "-1";
	const char* comma = ",";

	char* gpsDataList[12];

	for (int i = 0; i < 12; i++) {
	    gpsDataList[i] = "-1";
	}

	//Parsing the data and updating our values

	if (strncmp(concatenatedArray, GNRMC, strlen(GNRMC)) == 0) {
		char* individualGpsData = strtok(concatenatedArray, ","); // Initialize strtok with the buffer

		int index = 0;

		while (individualGpsData != NULL) {
			gpsDataList[index] = individualGpsData; // Store the token in an array
			individualGpsData = strtok(NULL, ","); // Get the next token
			index++;
		}
	}

	//if the value is different from both -1 and ,, we update our time
	if ((strncmp(gpsDataList[1], minus1, strlen(minus1)) != 0) &&
	   (strncmp(gpsDataList[1], comma, strlen(comma)) != 0)) {
		*time = atoi(gpsDataList[1]);
	}

	if ((strncmp(gpsDataList[3], minus1, strlen(minus1)) != 0) &&
	   (strncmp(gpsDataList[3], comma, strlen(comma)) != 0)) {
		*lat = atoi(gpsDataList[3]);
	}

	if ((strncmp(gpsDataList[4], minus1, strlen(minus1)) != 0) &&
	   (strncmp(gpsDataList[4], comma, strlen(comma)) != 0)) {
		strcpy(latDir, gpsDataList[4]);
	}

	if ((strncmp(gpsDataList[5], minus1, strlen(minus1)) != 0) &&
	   (strncmp(gpsDataList[5], comma, strlen(comma)) != 0)) {
		*longi = atoi(gpsDataList[5]);
	}

	if ((strncmp(gpsDataList[6], minus1, strlen(minus1)) != 0) &&
	   (strncmp(gpsDataList[6], comma, strlen(comma)) != 0)) {
		strcpy(longDir, gpsDataList[6]);
	}

	if ((strncmp(gpsDataList[8], minus1, strlen(minus1)) != 0) &&
	   (strncmp(gpsDataList[8], comma, strlen(comma)) != 0)) {
		*date = atoi(gpsDataList[8]);
	}


		//Sending data via UART
	HAL_UART_Transmit(&huart2, gpsDataRx1, sizeof(gpsDataRx1), HAL_MAX_DELAY);
	HAL_UART_Transmit(&huart2, gpsDataRx2, sizeof(gpsDataRx2), HAL_MAX_DELAY);
	HAL_UART_Transmit(&huart2, gpsDataRx3, sizeof(gpsDataRx3), HAL_MAX_DELAY);
	HAL_UART_Transmit(&huart2, gpsDataRx4, sizeof(gpsDataRx4), HAL_MAX_DELAY);
	HAL_UART_Transmit(&huart2, concatenatedArray, sizeof(concatenatedArray), HAL_MAX_DELAY);
	HAL_Delay(500);

	free(concatenatedArray);
}

float magnetometerData(float xCal,float yCal,float zCal) {
	int16_t magnetometerVal [3] = {0};

	while (HAL_I2C_Master_Transmit(&hi2c1, (magAddress << 1), &dataRegister, 1, 20) != HAL_OK){} //send data register address
	while (HAL_I2C_Master_Receive(&hi2c1, (magAddress << 1 | 1), magnetometerVal, 6, HAL_MAX_DELAY)){}

	int16_t xMag = magnetometerVal[0];

	int16_t yMag = magnetometerVal[1];

	int16_t zMag = magnetometerVal[2];

	float direction = getDirectionAngle(xMag, yMag, zMag, xCal, yCal, zCal);
	printDirection(direction);

	HAL_Delay(1000);

	return direction;
}


void setupMag() {
//	uint8_t magAddress = 0x1E;
//
//	//Register Addresses
//	uint8_t dataRegister = 0x68;
//	uint8_t cfgRegA = 0x60;
//	uint8_t cfgRegC = 0x62;
//
//	//Values to send
//	uint8_t initRegA = 0x0C;
//	uint8_t disRegA = 0x0E;
//	uint8_t initRegC = 0x10;
	uint8_t magSetupRX = 0;

	uint8_t magSetupTX [2] = {0};

	//1
	while (HAL_I2C_Master_Transmit(&hi2c1, (magAddress << 1), &cfgRegC, 1, HAL_MAX_DELAY) != HAL_OK){} //send address pointer

	//2
	HAL_I2C_Master_Receive(&hi2c1, (magAddress << 1 | 1), &magSetupRX, 1, HAL_MAX_DELAY);


	//3
	magSetupTX[0] = cfgRegC;
	magSetupTX[1] = initRegC;
	while (HAL_I2C_Master_Transmit(&hi2c1, (magAddress << 1), magSetupTX, 2, HAL_MAX_DELAY) != HAL_OK){}

	//4
	while (HAL_I2C_Master_Transmit(&hi2c1, (magAddress << 1), &cfgRegA, 1, HAL_MAX_DELAY) != HAL_OK){}

	//5
	HAL_I2C_Master_Receive(&hi2c1, (magAddress << 1 | 1), &magSetupRX, 1, HAL_MAX_DELAY);

	//6
	magSetupTX[0] = cfgRegA;
	magSetupTX[1] = disRegA;
	while (HAL_I2C_Master_Transmit(&hi2c1, (magAddress << 1), magSetupTX, 2, HAL_MAX_DELAY) != HAL_OK){}

	//7
	while (HAL_I2C_Master_Transmit(&hi2c1, (magAddress << 1), &cfgRegA, 1, HAL_MAX_DELAY) != HAL_OK){}

	//8
	HAL_I2C_Master_Receive(&hi2c1, (magAddress << 1 | 1), &magSetupRX, 1, HAL_MAX_DELAY);

	//9
	magSetupTX[0] = cfgRegA;
	magSetupTX[1] = disRegA;
	while (HAL_I2C_Master_Transmit(&hi2c1, (magAddress << 1), magSetupTX, 2, HAL_MAX_DELAY) != HAL_OK){}

	//10
	while (HAL_I2C_Master_Transmit(&hi2c1, (magAddress << 1), &cfgRegC, 1, HAL_MAX_DELAY) != HAL_OK){}

	//11
	HAL_I2C_Master_Receive(&hi2c1, (magAddress << 1 | 1), &magSetupRX, 1, HAL_MAX_DELAY);


	//12
	magSetupTX[0] = cfgRegC;
	magSetupTX[1] = initRegC;
	while (HAL_I2C_Master_Transmit(&hi2c1, (magAddress << 1), magSetupTX, 2, HAL_MAX_DELAY) != HAL_OK){}

	//13
	while (HAL_I2C_Master_Transmit(&hi2c1, (magAddress << 1), &cfgRegA, 2, HAL_MAX_DELAY) != HAL_OK){}

	//14
	HAL_I2C_Master_Receive(&hi2c1, (magAddress << 1 | 1), &magSetupRX, 1, HAL_MAX_DELAY);


	//15
	magSetupTX[0] = cfgRegA;
	magSetupTX[1] = initRegA;
	while (HAL_I2C_Master_Transmit(&hi2c1, (magAddress << 1), magSetupTX, 2, HAL_MAX_DELAY) != HAL_OK){}
}

void hardIronCal(float* xCal, float* yCal, float* zCal) {
  //Print Value to terminal
  uint8_t buf[30];
  sprintf((char*)buf, "Calibrating Magnetometer...\r\n");
  HAL_UART_Transmit(&huart2, buf, strlen((char*)buf), HAL_MAX_DELAY);

  int16_t magnetometerVal [3] = {0};

  while (HAL_I2C_Master_Transmit(&hi2c1, (magAddress << 1), &dataRegister, 1, 20) != HAL_OK){} //send data register address
  while (HAL_I2C_Master_Receive(&hi2c1, (magAddress << 1 | 1), magnetometerVal, 6, HAL_MAX_DELAY)){}

  int16_t xMax = magnetometerVal[0];
  int16_t xMin = magnetometerVal[0];

  int16_t yMax = magnetometerVal[1];
  int16_t yMin = magnetometerVal[1];

  int16_t zMax = magnetometerVal[2];
  int16_t zMin = magnetometerVal[2];


  for (int i = 0; i < 50; i++) {
	while (HAL_I2C_Master_Transmit(&hi2c1, (magAddress << 1), &dataRegister, 1, 20) != HAL_OK){} //send data register address
	while (HAL_I2C_Master_Receive(&hi2c1, (magAddress << 1 | 1), magnetometerVal, 6, HAL_MAX_DELAY)){}

    //Updating xMax and xMin
    if (magnetometerVal[0] > xMax) {
      xMax = magnetometerVal[0];
    }

    else if (magnetometerVal[0] < xMin) {
      xMin = magnetometerVal[0];
    }

    //Updating yMax and yMin
    if (magnetometerVal[1] > yMax) {
      yMax = magnetometerVal[1];
    }

    else if (magnetometerVal[1] < yMin) {
      yMin = magnetometerVal[1];
    }

    //Updating zMax and zMin
    if (magnetometerVal[2] > zMax) {
      zMax = magnetometerVal[2];
    }

    else if (magnetometerVal[2] < zMin) {
      zMin = magnetometerVal[2];
    }

    HAL_Delay(250);
    uint8_t buf[30];
    sprintf((char*)buf, "Calibrating Magnetometer...\r\n");
    HAL_UART_Transmit(&huart2, buf, strlen((char*)buf), HAL_MAX_DELAY);
  }

  //finding the calibration values
  *xCal = (xMax + xMin) / 2;
  *yCal = (yMax + yMin) / 2;
  *zCal = (zMax + zMin) / 2;
}

float getDirectionAngle(int16_t xMag, int16_t yMag, int16_t zMag, float xCal, float yCal, float zCal){
  xMag -= xCal;
  yMag -= yCal;
  zMag -= zCal;

  float D = atan2(xMag, yMag) * (180 / M_PI);

  if (D > 360) {
    D -= 360;
  }
  else if (D < 0){
    D += 360;
  }

  uint8_t buf[20];
  sprintf((char*)buf, "D = %u \r\n", (unsigned int)D);
  HAL_UART_Transmit(&huart2, buf, strlen((char*)buf), HAL_MAX_DELAY);

  return D;
}

void printDirection(float D) {
	if (D > 337.25 || D < 22.5)
	{
		uint8_t buf[15];
		sprintf((char*)buf, "North \r\n");
		HAL_UART_Transmit(&huart2, buf, strlen((char*)buf), HAL_MAX_DELAY);
	}
	else if (D >= 292.5 && D <= 337.25)
	{
		uint8_t buf[15];
		sprintf((char*)buf, "North-West \r\n");
		HAL_UART_Transmit(&huart2, buf, strlen((char*)buf), HAL_MAX_DELAY);
	}
	else if (D >= 247.5 && D < 292.5)
	{
		uint8_t buf[15];
		sprintf((char*)buf, "West \r\n");
		HAL_UART_Transmit(&huart2, buf, strlen((char*)buf), HAL_MAX_DELAY);
	}
	else if (D >= 202.5 && D < 247.5)
	{
		uint8_t buf[15];
		sprintf((char*)buf, "South-West \r\n");
		HAL_UART_Transmit(&huart2, buf, strlen((char*)buf), HAL_MAX_DELAY);
	}
	else if (D >= 157.5 && D < 202.5)
	{
		uint8_t buf[15];
		sprintf((char*)buf, "South \r\n");
		HAL_UART_Transmit(&huart2, buf, strlen((char*)buf), HAL_MAX_DELAY);
	}
	else if (D >= 112.5 && D < 157.5)
	{
		uint8_t buf[15];
		sprintf((char*)buf, "South-East \r\n");
		HAL_UART_Transmit(&huart2, buf, strlen((char*)buf), HAL_MAX_DELAY);
	}
	else if (D >= 67.5 && D < 112.5)
	{
		uint8_t buf[15];
		sprintf((char*)buf, "East \r\n");
		HAL_UART_Transmit(&huart2, buf, strlen((char*)buf), HAL_MAX_DELAY);
	}
	else if (D >= 22.5 && D < 67.5)
	{
		uint8_t buf[15];
		sprintf((char*)buf, "North-East \r\n");
		HAL_UART_Transmit(&huart2, buf, strlen((char*)buf), HAL_MAX_DELAY);
	}

}



/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

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
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : LD2_Pin */
  GPIO_InitStruct.Pin = LD2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LD2_GPIO_Port, &GPIO_InitStruct);

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

#ifdef  USE_FULL_ASSERT
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
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
