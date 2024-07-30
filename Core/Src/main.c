/************************************************
* @file    main.c 
* @author  APBashara
* @date    6/2024
* 
* @brief   Program to test drivers with FreeRTOS
***********************************************/

#include "stm32f407xx.h"
#include "cmsis_os2.h"

#include <stdio.h>

#include "sysclk.h"
#include "uart.h"
#include "timer.h"
#include "gpio.h"
#include "adc.h"
#include "spi.h"
#include "i2c.h"

#define HSE_VALUE 8000000U

#define SPI_CS 12

// Data Frames
/*----------------------------------------------------------------*/
// Possible Idea to make Structs for each type of Data Frame used (CAN, SPI, etc)
// Should go in a defines file as opposed to main
typedef struct {
  uint8_t command;
  uint8_t data;
} SPI_Frame;

// Global Variables
/*----------------------------------------------------------------*/
uint8_t message[64];
volatile uint16_t adc_buffer[16];


// Function Prototypes
/*----------------------------------------------------------------*/
void SysClock_Config();
void ADC_Task(void *argument);
void USART_Print(void *argument);
void SPI_Send(void *argument);
void Status_LED(void *argument);
void I2C_Send(void *argument);


// FreeRTOS Threads
/*----------------------------------------------------------------*/

// Prints out ADC values to USART3
osThreadId_t ADC_Read_Handle;
const osThreadAttr_t ADC_Read_Attr = {
  .name = "ADC_Task",
  .stack_size = 128 * 4,
  .priority = osPriorityNormal
};

// Prints out "Hello World!" to USART3
osThreadId_t USART_Print_Handle;
const osThreadAttr_t USART_Print_Attr = {
  .name = "USART_Print",
  .stack_size = 128 * 4,
  .priority = osPriorityNormal
};

osThreadId_t SPI_Send_Handle;
const osThreadAttr_t SPI_Attr = {
  .name = "SPI_Send",
  .stack_size = 128 * 4,
  .priority = osPriorityNormal
};

// Blinks the LED on PD14
osThreadId_t Status_LED_Handle;
const osThreadAttr_t Status_LED_Attr = {
  .name = "Status_LED",
  .stack_size = 128 * 4,
  .priority = osPriorityNormal
};

osThreadId_t I2C_Send_Handle;
const osThreadAttr_t I2C_Attr = {
  .name = "I2C_Send",
  .stack_size = 128 * 4,
  .priority = osPriorityNormal
};

/**
 * @brief Handle Timer 2 Interrupt
 * 
 */
void TIM2_IRQHandler(void) {
  if (TIM2->SR & TIM_SR_UIF) { // Check status register for update interrupt flag
    TIM2->SR &= ~(TIM_SR_UIF); // Reset the update interrupt flag
    Toggle_Pin(GPIOD, 15);  // Toggle the LED output pin.
  }
}

/**
 * @brief Main Function
 * 
 * @note Initializes the System Clock, Timer 2, USART3, ADC1, and DMA2
 * @return int 
 */
int main() {
  Sysclock_168();
  LED_Init();
  TIM2_Init();
  USART3_Init();
  ADC_Init();
  DMA_ADC1_Init(adc_buffer);
  SPI2_Init();
  I2C1_Init();

  uint8_t ADC_Val[32];

  osKernelInitialize(); // Initialize FreeRTOS

  ADC_Read_Handle = osThreadNew(ADC_Task, NULL, &ADC_Read_Attr);
  USART_Print_Handle = osThreadNew(USART_Print, NULL, &USART_Print_Attr);
  Status_LED_Handle = osThreadNew(Status_LED, NULL, &Status_LED_Attr);
  SPI_Send_Handle = osThreadNew(SPI_Send, NULL, &SPI_Attr);
  I2C_Send_Handle = osThreadNew(I2C_Send, NULL, &I2C_Attr);

  osKernelStart(); // Start FreeRTOS

  while(1) {
    // Should not reach here
  }
  return 0;
}


// FreeRTOS Thread Functions
/*----------------------------------------------------------------*/

/**
 * @brief Thread for reading and printing ADC values
 * 
 * @param argument 
 */
void ADC_Task(void *argument) {
  while(1) {
    if (adc_buffer[1] > 250) {
      Set_Pin(GPIOD, 12);
      Clear_Pin(GPIOD, 13);
    } else {
      Clear_Pin(GPIOD, 12);
      Set_Pin(GPIOD, 13);
    }
    sprintf(message, "%d, %d\n", adc_buffer[1], adc_buffer[5]);
    send_String(USART3, message);
    osDelay(50);
  }
}

/**
 * @brief Thread for printing message to USART3
 * 
 * @param argument 
 */
void USART_Print(void *argument) {
  while(1) {
    osDelay(1000);
    sprintf(message, "Hello World!\n");
    send_String(USART3, message);
  }
}

/**
 * @brief Send SPI Message over SPI2
 * 
 * @param argument 
 */
void SPI_Send(void *argument) {
  Set_Pin(GPIOB, SPI_CS); // Pull GPIOB12 High for SPI CS
  uint8_t frame[] = {0x0F, 0x00};
  while(1) {
    SPI_Transmit_Frame(SPI2, frame, sizeof(frame), SPI_CS);
    frame[1]++;
    osDelay(100);
  }
}

/**
 * @brief Send a I2C data byte
 * 
 * @param argument 
 */
void I2C_Send(void *argument) {
  uint8_t addr = 0x68;
  uint8_t data[] = {0, 1, 2, 3};
  uint16_t temp = 0;
  uint8_t DegC[64];
  while(1) {
    I2C_Write(I2C1, addr, data, sizeof(data));
    osDelay(50);
    temp = I2C_Read(I2C1, addr, 65) << 8;
    temp |= I2C_Read(I2C1, addr, 66);
    temp = temp + 21;
    sprintf(DegC, "IMU Temp: %d\n", temp);
    send_String(USART3, DegC);
    osDelay(50);
  }
}

/**
 * @brief Thread for blinking the status led
 * 
 * @param argument 
 */
void Status_LED(void *argument) {
  while(1) {
    osDelay(1000);
    Toggle_Pin(GPIOD, 14);
  }
}