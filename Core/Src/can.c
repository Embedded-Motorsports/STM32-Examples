/************************************************
* @file    can.c 
* @author  APBashara
* @date    7/2024
* 
* @brief   CAN Driver Implementation
***********************************************/

#include "stm32f407xx.h"

/**
 * @brief Initializes CAN1
 * 
 * @note Pins PA11(Rx) and PA12 (Tx)
 * @note Baud Rate: 500kbps
 */
void CAN1_Init() {
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    RCC->APB1ENR |= RCC_APB1ENR_CAN1EN;

    GPIOA->MODER &= ~GPIO_MODER_MODE11 & ~GPIO_MODER_MODE12;
    GPIOA->MODER |= (0x2 << GPIO_MODER_MODE11_Pos) | (0x2 << GPIO_MODER_MODE12_Pos);
    GPIOA->AFR[1] |= (0x9 << GPIO_AFRH_AFSEL11_Pos) | (0x9 << GPIO_AFRH_AFSEL12_Pos);
    GPIOA->OSPEEDR |= (0x3 << GPIO_OSPEEDR_OSPEED11_Pos) | (0x3 << GPIO_OSPEEDR_OSPEED12_Pos);

    CAN1->MCR |= CAN_MCR_INRQ; // Request Initialization Mode
    while (!(CAN1->MSR & CAN_MSR_INAK)); // Wait until Initialization Mode is entered

    // http://www.bittiming.can-wiki.info/
    CAN1->BTR = 0x001a0005; // Set Baud Rate to 500kbps
}