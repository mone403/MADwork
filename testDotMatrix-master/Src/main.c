/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "eth.h"
#include "i2c.h"
#include "rtc.h"
#include "usart.h"
#include "usb_otg.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "max_matrix_stm32.h"
#include "string.h"
#include "stdio.h"
#include "dwt_stm32_delay.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
extern RTC_HandleTypeDef hrtc;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
char timebuff[50];
char datebuff[50];
char timerbuff[50];
char alarmbuff[50];
char backtime[50];
int state = 0;
uint8_t alarm = 0;
int set = 0; // in clock
int mode = 0; // change mode
int set_timer = 0;
uint16_t setHours = 0x10; 
uint16_t setMinutes = 0x20; 
uint16_t setDate = 0x22; 
uint16_t setMonth = RTC_MONTH_NOVEMBER; 
uint16_t setYear = 0x19; 
uint16_t set_alarm_Hours = 0x16;
uint16_t set_alarm_minute = 0x22;
uint8_t timer_count = 0;
uint8_t timer_minute = 0;
uint8_t timer_second = 0;
uint8_t timerdown_minute = 0;
uint8_t timerdown_second = 0;
int backup_timer_minute = 0 ;
int backup_timer_second = 0;
float h,t;
uint8_t step=0;
uint16_t CRC16_2(uint8_t *,uint8_t);
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void Clock_Metrix_Display();
void get_timer();
void get_alarm();
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void set_time (void){
	
	RTC_TimeTypeDef sTime;
  RTC_DateTypeDef sDate;
	
//	setHours = 0x10;
//	setMinutes = 0x20;
//	setDate = 0x22;
//	setMonth = RTC_MONTH_NOVEMBER;
//	setYear = 0x19;
	if(mode == 1 && set == 1) {
		if(HAL_GPIO_ReadPin(GPIOG, GPIO_PIN_1) == 1) {
			setHours += 0x1;
			HAL_Delay(500);
		}
		else if(HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_11) == 1) {
			setHours -= 0x1;
			HAL_Delay(500);
		}
		if(setHours > 0x23){
			setHours = 0x0;
		}
		if(setHours < 0x0) {
			setHours = 0x23;
		}
	}
	else if(mode == 1 && set == 2) {
		if(HAL_GPIO_ReadPin(GPIOG, GPIO_PIN_1) == 1) {
			setMinutes += 0x1;
			HAL_Delay(500);
		}
		else if(HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_11) == 1) {
			setMinutes -= 0x1;
			HAL_Delay(500);
		}
		if(setMinutes > 0x60){
			setMinutes = 0x0;
		}
		if(setMinutes < 0x0){
			setMinutes = 0x59;
		}
	}
	if(mode == 2 && set == 1) {
		if(HAL_GPIO_ReadPin(GPIOG, GPIO_PIN_1) == 1) {
			setDate += 0x1;
			HAL_Delay(500);
			if(setDate > 0x30){
			setDate = 0x1;
			HAL_Delay(500);
		}
		if(setDate < 0x1){
			setDate = 0x30;
			HAL_Delay(500);
		}
		}
		else if(HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_11) == 1) {
			setDate -= 0x1;
			HAL_Delay(500);
			if(setDate > 0x30){
			setDate = 0x1;
			HAL_Delay(500);
		}
		if(setDate < 0x1){
			setDate = 0x30;
			HAL_Delay(500);
		}
		}
		
	}
	else if(mode == 2 && set == 2) {
		if(HAL_GPIO_ReadPin(GPIOG, GPIO_PIN_1) == 1) {
				setMonth += 0x1;
				HAL_Delay(500);
			}
			else if(HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_11) == 1) {
				setMonth -= 0x1;
				HAL_Delay(500);
			}
			if(setMonth > 0x12){
				setMonth = 0x1;
			}
			if(setMonth < 0x1){
				setMonth = 0x12;
			}
	}
	else if(mode == 2 && set == 3){
		if(HAL_GPIO_ReadPin(GPIOG, GPIO_PIN_1) == 1) {
			setYear += 0x1;
		}
		else if(HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_11) == 1) {
			setYear -= 0x1;
		}
	} 
	
	sTime.Hours = setHours ;
  sTime.Minutes = setMinutes;
  sTime.Seconds = 0x30;
  sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  sTime.StoreOperation = RTC_STOREOPERATION_RESET;
  if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BCD) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN RTC_Init 3 */

  /* USER CODE END RTC_Init 3 */

  sDate.WeekDay = RTC_WEEKDAY_FRIDAY;
	sDate.Month = setMonth ;
  sDate.Date = setDate;
  sDate.Year = setYear;

  if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BCD) != HAL_OK)
  {
    Error_Handler();
  }
	

	 /* USER CODE BEGIN RTC_Init 4 */
	HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR1, 0x32F2); // backup register
  /* USER CODE END RTC_Init 4 */
}

void set_alarm (void) 
{
	RTC_AlarmTypeDef sAlarm;
	
	if(mode == 4 && set == 1) {
		if(HAL_GPIO_ReadPin(GPIOG, GPIO_PIN_1) == GPIO_PIN_SET) {
			set_alarm_Hours += 0x1;
			HAL_Delay(500);
			if(set_alarm_Hours > 0x23){
				set_alarm_Hours = 0x0;
			}
		}
		else if(HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_11) == GPIO_PIN_SET) {
			set_alarm_Hours -= 0x1;
			HAL_Delay(500);
			if(set_alarm_Hours < 0x0) {
			set_alarm_Hours = 0x23;
		}
		}
		
	}
	else if(mode == 4 && set == 2) {
		if(HAL_GPIO_ReadPin(GPIOG, GPIO_PIN_1) == GPIO_PIN_SET) {
			set_alarm_minute += 0x1;
			HAL_Delay(500);
			if(set_alarm_minute > 0x59){
			set_alarm_minute = 0x0;
		}
		else if(HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_11) == GPIO_PIN_SET) {
			set_alarm_minute -= 0x1;
			HAL_Delay(500);
			
			}
			if(set_alarm_minute < 0x0){
				set_alarm_minute = 0x59;
			}
		}
		
	}
	
	
	
	sAlarm.AlarmTime.Hours = set_alarm_Hours;
  sAlarm.AlarmTime.Minutes = set_alarm_minute;
  sAlarm.AlarmTime.Seconds = 0x00;
  sAlarm.AlarmTime.SubSeconds = 0x0;
  sAlarm.AlarmTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  sAlarm.AlarmTime.StoreOperation = RTC_STOREOPERATION_RESET;
  sAlarm.AlarmMask = RTC_ALARMMASK_NONE;
  sAlarm.AlarmSubSecondMask = RTC_ALARMSUBSECONDMASK_ALL;
  sAlarm.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_DATE;
  sAlarm.AlarmDateWeekDay = setDate;
  sAlarm.Alarm = RTC_ALARM_A;
  if (HAL_RTC_SetAlarm_IT(&hrtc, &sAlarm, RTC_FORMAT_BCD) != HAL_OK)
  {
    Error_Handler();
  }
	get_alarm();
	
  /* USER CODE BEGIN RTC_Init 5 */

  /* USER CODE END RTC_Init 5 */
}

void timer_up (void) {
	if(HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_9) == GPIO_PIN_SET ){
		backup_timer_minute = timer_minute;
		backup_timer_second = timer_second;
		set = 4;
	}
	if(timer_count % 1000){
		timer_second += 1 ;
		if(timer_second%60 == 0) {
			timer_second = 0;
			timer_minute +=1;
		}
	}
	
}

void timer_down (void) {
	if(HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_9) == GPIO_PIN_SET ){
		backup_timer_minute = timer_minute;
		backup_timer_second = timer_second;
		set = 4;
	}
	
	if(timer_count % 1000){
		if(timer_second == 0 && timer_minute > 0){
			timer_minute -= 1;
			timer_second = 59;
		} 
		if(timer_second > 0) {
		timer_second -= 1 ; }
		
		if(timer_second%60 == 0) {
			if(timer_minute == 0 && timer_second == 0){
				timer_minute = 0;
				timer_second =0;
				backup_timer_minute = 0;
				backup_timer_second = 0;
				state = 1;
				set = 4;		
			}
			else  {timer_second = 59;
			timer_minute -=1; }
		}
		
	}

}

void checkstate_timer (void) {

	
	if(mode == 3 && set == 1){
		if(HAL_GPIO_ReadPin(GPIOG, GPIO_PIN_1) == GPIO_PIN_SET ) {
			timer_minute += 1;
		} 
		if (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_11) == GPIO_PIN_SET ) {
			timer_minute -=1;
		}
		if(timer_minute > 59) {
			timer_minute = 0;
		}
		else if(timer_minute <0 ) {
			timer_minute = 59;
		}
		get_timer();
	}
	else if(mode == 3 && set==2){
		if(HAL_GPIO_ReadPin(GPIOG, GPIO_PIN_1) == GPIO_PIN_SET ) {
			timer_second += 1;
		} 
		if (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_11) == GPIO_PIN_SET ) {
			timer_second -=1;
		}
		if(timer_second > 59) {
			timer_second = 0;
		}
		else if(timer_second <0 ) {
			timer_second = 59;
		}
		get_timer();
	}
	else if (mode == 3 && set==3) {
		if(timer_minute == 0 && timer_second == 0) 
		{
			timer_count = 0;
			state = 0;
			while(set==3) {
				timer_up();
				get_timer();
			}
		} else {
			timer_count = 0;
			state = 0;
			while(set==3){
				timer_down();
				get_timer();
			}
		}
	}
	else if (set == 4) {
		while(set == 4){
		if(state == 1){
			timer_minute = 0; timer_second = 0; 
		}
		sprintf(backtime, "Timer stop : %02d : %02d\n\r" , timer_minute, timer_second);
		HAL_UART_Transmit(&huart3, (uint8_t*) backtime ,30,1000);
		HAL_Delay(1000);
		HAL_UART_Transmit(&huart3, (uint8_t*) '\r' ,2,1000);}
	}
	get_timer();
}
void get_alarm(void) {
	RTC_AlarmTypeDef setAlarm;
	HAL_RTC_GetAlarm(&hrtc, &setAlarm, RTC_ALARM_A , RTC_FORMAT_BIN);
	sprintf(alarmbuff, "Alarm : %02d : %02d\n\r" , setAlarm.AlarmTime.Hours, setAlarm.AlarmTime.Minutes);
	HAL_UART_Transmit(&huart3, (uint8_t*) alarmbuff ,30,1000);
	HAL_Delay(1000);
	// HAL_UART_Transmit(&huart3, (uint8_t*) '\r' ,2,1000);
}


void get_timer(void) {
	sprintf(timerbuff, "Timer : %02d : %02d\r" , timer_minute, timer_second);
	HAL_UART_Transmit(&huart3, (uint8_t*) timerbuff ,30,1000);
	HAL_Delay(1000);
	HAL_UART_Transmit(&huart3, (uint8_t*) '\r' ,2,1000);
}

// current value
void get_time(void)
{
	
	RTC_DateTypeDef gDate;
	RTC_TimeTypeDef gTime;
	// get RTC current time
	HAL_RTC_GetTime(&hrtc, &gTime, RTC_FORMAT_BIN);
	// get RTC current Date
	HAL_RTC_GetDate(&hrtc, &gDate, RTC_FORMAT_BIN);
	// format hh:mm:ss
	sprintf(timebuff, "Time : %02d:%02d:%02d\r", gTime.Hours , gTime.Minutes, gTime.Seconds);
	// format dd - mm - yy
	sprintf(datebuff, "Date : %02d-%02d-%2d\r", gDate.Date , gDate.Month, 2000 + gDate.Year);
}

void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *hrtc) {
	alarm = 1;
}

void to_do_alarm (void) {
	while(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_3) == 0)
	{
		char str[] = "ALARM!!!";
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_12, GPIO_PIN_SET);
		while(__HAL_UART_GET_FLAG(&huart3, UART_FLAG_TC)==RESET) {}
		HAL_UART_Transmit(&huart3, (uint8_t*) str ,strlen(str),1000);
		HAL_Delay(1000);
		
	}
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_12, GPIO_PIN_RESET);
	alarm = 0;

}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
char BufferTemp[100];
char BufferHum[100];
	uint8_t cmdBuffer[3];
	uint8_t dataBuffer[8];
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
  MX_ETH_Init();
  MX_USART3_UART_Init();
  MX_USB_OTG_FS_PCD_Init();
  MX_ADC1_Init();
  MX_I2C1_Init();
  MX_RTC_Init();
	
  /* USER CODE BEGIN 2 */
DWT_Delay_Init();
uint32_t samplingTime = 280;
uint32_t deltaTime = 40;
uint32_t sleepTime = 9680;

int voMeasured = 0;
float calcVoltage = 0;
float dustDensity = 0;
int AQI = 0;
int Imax[4] = {25,50,100,200};
int Imin[4] = {0,26,51,101};
int Cmax[4] = {25,37,50,90};
int Cmin[4] = {0,26,38,51};
char BufferAQI[100],n;
	 max_init (0x03);
	cmdBuffer[0] = 0x03;
	cmdBuffer[1] = 0x00;
	cmdBuffer[2] = 0x04;
int mode = 5;


if(HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR1) != 0x32F2)
	{
		set_time();
	}
//	set_time();
	set_alarm();
//	write_char ('1',4);  // write 'A' on 4th display			// write_char(DEC_Ascii, display#)
//	write_char ('0',3);  // write '2' on 3rd display
//	write_char ('0',2);  // print heart on 2nd display
//	write_char ('0',1);  // write 'm'on st display
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
		sprintf(BufferTemp,"Temperature:%4.1f",t);
		scroll_string((uint8_t *)BufferTemp,150,left);
		HAL_Delay(100);
		scroll_string((uint8_t *)"               ",1,left);
		sprintf(BufferHum,"Humidity:%4.1f",h);
		scroll_string((uint8_t *)BufferHum,150,left);
		HAL_Delay(100);
		scroll_string((uint8_t *)"               ",1,left);
		HAL_Delay(5000);
		HAL_GPIO_TogglePin(GPIOB,GPIO_PIN_0);
		
		//Wake up Sensor
		HAL_I2C_Master_Transmit(&hi2c1,0x5c<<1,cmdBuffer,3,200);
		//Send reading command
		HAL_I2C_Master_Transmit(&hi2c1,0x5c<<1,cmdBuffer,3,200);
		
		HAL_Delay(1);
		
		//Receive sensor data
		HAL_I2C_Master_Receive(&hi2c1,0x5c<<1,dataBuffer,8,200);
		
		//CRC Check
		uint16_t Rcrc = dataBuffer[7] <<8;
		Rcrc += dataBuffer[6];
		if(Rcrc == CRC16_2(dataBuffer,6)){
			uint16_t temperature = ((dataBuffer[4] & 0x7F) << 8) + dataBuffer[5];
			t = temperature/10.0;
			t=(((dataBuffer[4]&0x80)>>7)==1) ? (t * (-1)) : t;
			
			uint16_t humidity = (dataBuffer[2] << 8) + dataBuffer[3];
			h = humidity/10.0;
		}
	
		HAL_GPIO_WritePin(GPIOC,GPIO_PIN_7,GPIO_PIN_RESET);
		DWT_Delay_us(samplingTime);
		
		while(HAL_ADC_PollForConversion(&hadc1,100)!= HAL_OK){}
		voMeasured = HAL_ADC_GetValue(&hadc1);
		
		
		DWT_Delay_us(deltaTime);
		HAL_GPIO_WritePin(GPIOC,GPIO_PIN_7,GPIO_PIN_SET);
		DWT_Delay_us(sleepTime);
		
		calcVoltage = voMeasured * (5.0/4096.0);
		dustDensity = ((0.17*calcVoltage) - 0.1)*100;
		
		if(dustDensity < 0){
			dustDensity = 0.0;
		}
		if(dustDensity<=25){
			AQI = (((Imax[0]-Imin[0])/(Cmax[0]-Cmin[0]))*(dustDensity-Cmin[0]))+Imin[0];
		}else if(dustDensity > 25 & dustDensity <=37){
			AQI = (((Imax[1]-Imin[1])/(Cmax[1]-Cmin[1]))*(dustDensity-Cmin[1]))+Imin[1];
		}else if(dustDensity > 37 & dustDensity <=50){
			AQI = (((Imax[2]-Imin[2])/(Cmax[2]-Cmin[2]))*(dustDensity-Cmin[2]))+Imin[2];
		}else if(dustDensity > 51 & dustDensity <= 90){
			AQI = (((Imax[3]-Imin[3])/(Cmax[3]-Cmin[3]))*(dustDensity-Cmin[3]))+Imin[3];
		}else{
			
		}				
		sprintf(BufferAQI,"AQI: %d",AQI);
		scroll_string((uint8_t *)BufferAQI,150,left);
		HAL_Delay(100);
		scroll_string((uint8_t *)"               ",1,left);
		
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
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

  /** Configure LSE Drive Capability 
  */
  HAL_PWR_EnableBkUpAccess();
  __HAL_RCC_LSEDRIVE_CONFIG(RCC_LSEDRIVE_LOW);
  /** Configure the main internal regulator output voltage 
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE|RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 216;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Activate the Over-Drive mode 
  */
  if (HAL_PWREx_EnableOverDrive() != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_7) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC|RCC_PERIPHCLK_USART3
                              |RCC_PERIPHCLK_I2C1|RCC_PERIPHCLK_CLK48;
  PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
  PeriphClkInitStruct.Usart3ClockSelection = RCC_USART3CLKSOURCE_PCLK1;
  PeriphClkInitStruct.I2c1ClockSelection = RCC_I2C1CLKSOURCE_PCLK1;
  PeriphClkInitStruct.Clk48ClockSelection = RCC_CLK48SOURCE_PLL;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
uint16_t CRC16_2(uint8_t *ptr,uint8_t length){
	uint16_t crc=0xFFFF;
	uint8_t s = 0x00;
	
	while(length--){
		crc ^= *ptr++;
		for(s = 0;s < 8; s++){
			if((crc & 0x01) != 0){
				crc >>=1;
				crc ^= 0xA001;
			}else crc >>=1;
		}
	}
	return crc;
}
void Clock_Metrix_Display(uint32_t Mode,char strOut[], char strScroll[]){
		if(Mode == 1 || Mode == 2){					// Normal Clock, Date Month Year, Timer, Alarm
			write_char (strOut[0],4);
			write_char (strOut[1],3);
			write_char (strOut[2],2);
			write_char (strOut[3],1);
		}
		else if(Mode == 3 || Mode == 4){		// AQI, Tempreasure, 
			// Load Buffer From AQI , Tempreasure to strScroll
			if(Mode == 3){
				scroll_string ((uint8_t *) "AQI  ", 100, left);
				HAL_Delay(500);
				scroll_string ((uint8_t *) "           ", 1, left);
				strScroll = "";
			}
			else if(Mode == 4){
				scroll_string ((uint8_t *) "Tempreasure", 100, left);
				HAL_Delay(500);
				scroll_string ((uint8_t *) "           ", 1, left);
				strScroll = "";
			}
		}
		scroll_string ((uint8_t *) strScroll, 150, left);
		//statusSend = 1;
}
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{	
	// mode
	if(GPIO_Pin == GPIO_PIN_3)
	{
		set = 0;
		HAL_Delay(1500);
		 // clear set init --> 0
		mode +=1;
		if (mode > 6) {
			mode = 0;
		}
	}
	// set in function
	if(GPIO_Pin == GPIO_PIN_9)
	{
		HAL_Delay(1500);
		set +=1;
		if( mode == 1 ) {	// set_time
			if(set > 2 ){
				set = 0;
			}
		}
		else if(mode == 2) {	// set_date
			if(set>3){
				set = 0;
			}
		}
		else if(mode == 3) {	// timer
			if(set >4) {
				set = 0;
			}
		}
		else if(mode == 4) {	// alarm
			if (set>2){
				set = 0;
			}
		}
	}
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

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
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
