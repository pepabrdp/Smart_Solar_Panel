#include "main.h"
#include <string.h>
#include <stdio.h>
#include <math.h>


ADC_HandleTypeDef hadc1;

I2C_HandleTypeDef hi2c1;

TIM_HandleTypeDef htim1;

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_ADC1_Init(void);
static void MX_I2C1_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_TIM1_Init(void);

//MAGNETOMETER
void  hardIronCal(float* xCal, float* yCal, float* zCal);
float getDirectionAngle(int16_t xMag, int16_t yMag, int16_t zMag, float xCal, float yCal, float zCal);
void printDirection(float D);
void setupMag();
float magnetometerData(float xCal,float yCal,float zCal);

//GPS
void getGpsData(float* lat, float* longi, float* time, float* date, char* longDir, char* latDir);

//VOLTAGE MEASUREMENT
float getSolarPanelVoltage();

//LIGHT SENSOR
void setupLightSensor(uint8_t lightAddress);
float getLightData(uint8_t lightAddress);

//MOTOR CONTROL
void microDelay (uint16_t delay);
void setupMotor();
void turnOffMotor();
void rotateMotor(float angle);
void setClockwise();
void setCounterClockwise();

#define DIR_PIN GPIO_PIN_7
#define DIR_PORT GPIOA
#define STEP_PIN GPIO_PIN_6
#define STEP_PORT GPIOA
#define SLEEP_PIN GPIO_PIN_8
#define SLEEP_PORT GPIOA
#define ENABLE_PIN GPIO_PIN_6
#define ENABLE_PORT GPIOB

//BLUETOOTH
void bluetoothSend(float direction, float lightDataGND, float lightDataVDD, float lightDataSDA, float lightDataSCL, float solarPanelVoltage);


//VARIABLES
//////////////////////////////////
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
///////////////////////////////////
//GPS
//Address
uint16_t gpsAddress = 0x42;
///////////////////////////////////
//LIGHT SENSORS
uint8_t lightAddressGND = 0x44;
uint8_t lightAddressVDD = 0x45;
uint8_t lightAddressSDA = 0x46;
uint8_t lightAddressSCL = 0x47;
///////////////////////////////////
//Motor Control
//Motor Control Variables
int stepDelay = 500; // 1000us more delay means less speed
int stepsPerRev = 200;
float stepAngle = 1.8;
///////////////////////////////////

int main(void)
{

  HAL_Init();

  SystemClock_Config();

  MX_GPIO_Init();
  MX_ADC1_Init();
  MX_I2C1_Init();
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  MX_TIM1_Init();

  ///////////////////////////////////////////////////////////////
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

  //////////////////////////////////////////////////////////////
  //LIGHT SENSOR
  setupLightSensor(lightAddressGND);
  float lightDataGND = 0;
  float lightDataVDD = 0;
  float lightDataSDA = 0;
  float lightDataSCL = 0;

  //////////////////////////////////////////////////////////////
  //MAGNETOMETER
  float xCal, yCal, zCal;
  xCal = 0;
  yCal = 0;
  zCal = 0;
  setupMag();
  hardIronCal(&xCal, &yCal, &zCal);

  //Direction Angle
  float direction = 0;
  //////////////////////////////////////////////////////////////

  //////////////////////////////////////////////////////////////
  //SOLAR PANEL VOLTAGE
  float solarPanelVoltage = 0;
  //////////////////////////////////////////////////////////////
  //MOTOR CONTROL
  HAL_TIM_Base_Start(&htim1);
  setCounterClockwise();
  float motorCounter = 0;
  ///////////////////////////////////////////////////////////////

  int count = 0;
  while (1)
  {

    direction = magnetometerData(xCal, yCal, zCal);
    lightDataGND = getLightData(lightAddressGND);
    solarPanelVoltage = getSolarPanelVoltage();

//    if (count == 20) {
//    	bluetoothSend(direction, lightDataGND, lightDataVDD, lightDataSDA, lightDataSCL, solarPanelVoltage);
//    	count = 0;
//    }



    count += 1;

    if ((HAL_GetTick() - motorCounter) > 5000) {
	  setupMotor();
	  rotateMotor(28);
	  turnOffMotor();

	  motorCounter = HAL_GetTick();
	}


  }
}

void rotateMotor(float angle) {

	int steps = (angle / stepAngle);

	for(int x=0; x < steps; x++)
	{
		HAL_GPIO_WritePin(STEP_PORT, STEP_PIN, GPIO_PIN_SET);
		microDelay(stepDelay);
		HAL_GPIO_WritePin(STEP_PORT, STEP_PIN, GPIO_PIN_RESET);
		microDelay(stepDelay);
	}
}

void setClockwise() {
	HAL_GPIO_WritePin(DIR_PORT, DIR_PIN, GPIO_PIN_SET);
}

void setCounterClockwise() {
	HAL_GPIO_WritePin(DIR_PORT, DIR_PIN, GPIO_PIN_RESET);
}

void microDelay (uint16_t delay)
{
  __HAL_TIM_SET_COUNTER(&htim1, 0);
  while (__HAL_TIM_GET_COUNTER(&htim1) < delay);
}

void setupMotor() {
	HAL_GPIO_WritePin(SLEEP_PORT, SLEEP_PIN, GPIO_PIN_SET);
	HAL_GPIO_WritePin(ENABLE_PORT, ENABLE_PIN, GPIO_PIN_RESET);
}

void turnOffMotor() {
	HAL_GPIO_WritePin(SLEEP_PORT, SLEEP_PIN, GPIO_PIN_RESET);
}

void bluetoothSend(float direction, float lightDataGND, float lightDataVDD, float lightDataSDA, float lightDataSCL, float solarPanelVoltage) {
	uint8_t bluetoothBuffer[120];
	sprintf((char*)bluetoothBuffer,"Magnetometer Angle: %u.%u\r\n Light GND Addr LUX: %u.%u\r\n Light VDD Addr LUX: %u.%u\r\n Light SDA Addr LUX: %u.%u\r\n Light SCL Addr LUX: %u.%u\r\n Solar Panel Voltage: %u.%u\r\n",
			(unsigned int)direction, (unsigned int)direction%100, (unsigned int)lightDataGND, (unsigned int)lightDataGND%100,
			(unsigned int)lightDataVDD, (unsigned int)lightDataVDD%100, (unsigned int)lightDataSDA, (unsigned int)lightDataSDA%100,
			(unsigned int)lightDataSCL, (unsigned int)lightDataSCL%100, (unsigned int)solarPanelVoltage, (unsigned int)solarPanelVoltage%100);

	HAL_UART_Transmit_IT(&huart1, bluetoothBuffer, sizeof (bluetoothBuffer));
}

void setupLightSensor(uint8_t lightAddress) {
	uint8_t data_send[3] = {0};
	data_send[0] = 0x0A;
	data_send[1] = 0x32;
	data_send[2] = 0xF8;
	while (HAL_I2C_Master_Transmit(&hi2c1, (lightAddress << 1), data_send, 3, 20) != HAL_OK){}
}

float getLightData(uint8_t lightAddress) {
	uint8_t data_send[4] = {0};
	uint8_t data_in [2] = {0};
	uint16_t exp;
	uint16_t man;
	uint16_t alldata;
	uint8_t buf[12];
	float ans = 0;

	data_send[0] = 0x00;
	while (HAL_I2C_GetState(&hi2c1) != HAL_I2C_STATE_READY){}
	while (HAL_I2C_Master_Transmit(&hi2c1, (lightAddress << 1), data_send, 1, 20) != HAL_OK){}
	while (HAL_I2C_GetState(&hi2c1) != HAL_I2C_STATE_READY){}
	while (HAL_I2C_Master_Receive(&hi2c1, (lightAddress << 1 | 1), data_in, 2, HAL_MAX_DELAY)){}

	alldata = data_in[0];
	alldata = (alldata << 8) | data_in[1];
	man = alldata & 0x0FFF;
	exp = (alldata & 0xF000) >> 12;

	ans = man * (0.01 * pow(2,exp));

	sprintf((char*)buf,"LUX: %.2f\r\n",ans);
	HAL_UART_Transmit(&huart2, buf, strlen((char*)buf), HAL_MAX_DELAY);

	return ans;
}


float getSolarPanelVoltage() {
	unsigned int analogValue = 0;

	HAL_ADC_Start(&hadc1);
	if(HAL_ADC_PollForConversion(&hadc1, 300) == HAL_OK) {
		analogValue = HAL_ADC_GetValue(&hadc1);
	}

	HAL_ADC_Stop(&hadc1);

	//Value obtained from voltage divider
	int voltageDividerLargeRes = 14;
	float voltageDividerSmallRes = 3.1;
	float measuredVoltage = (analogValue * 3.1 /  4096);
	float solarPanelVoltage = (measuredVoltage * (voltageDividerLargeRes + voltageDividerSmallRes) / voltageDividerSmallRes);


	uint8_t buf[30];
	sprintf((char*)buf,"Solar Panel Voltage: %.2f\r\n",solarPanelVoltage);
	HAL_UART_Transmit(&huart2, buf, strlen((char*)buf), HAL_MAX_DELAY);

	return solarPanelVoltage;
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
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */

  /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  hadc1.Init.ScanConvMode = DISABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 1;
  hadc1.Init.DMAContinuousRequests = DISABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_0;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

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
  * @brief TIM1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM1_Init(void)
{

  /* USER CODE BEGIN TIM1_Init 0 */

  /* USER CODE END TIM1_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM1_Init 1 */

  /* USER CODE END TIM1_Init 1 */
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 71;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 65535;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM1_Init 2 */

  /* USER CODE END TIM1_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

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
  HAL_GPIO_WritePin(GPIOA, LD2_Pin|GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8
                          |GPIO_PIN_9, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_10|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_RESET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : LD2_Pin PA6 PA7 PA8
                           PA9 */
  GPIO_InitStruct.Pin = LD2_Pin|GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8
                          |GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PB10 PB4 PB5 PB6 */
  GPIO_InitStruct.Pin = GPIO_PIN_10|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : PC7 */
  GPIO_InitStruct.Pin = GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

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
