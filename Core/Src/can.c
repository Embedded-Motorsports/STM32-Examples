/************************************************
* @file    can.c 
* @author  APBashara
* @date    7/2024
* 
* @brief   CAN Driver Implementation
***********************************************/

#include "stm32f407xx.h"
#include "can.h"

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

    // Configure CAN1
    CAN1->MCR &= ~CAN_MCR_TXFP & ~CAN_MCR_NART & ~CAN_MCR_RFLM 
                & ~CAN_MCR_TTCM & ~CAN_MCR_ABOM;
    
    // http://www.bittiming.can-wiki.info/
    // Sample Point at 85.7% Reg Value = 0x001a0005
    // CAN1->BTR = 0x001a0005; // Set Baud Rate to 500kbps
    CAN1->BTR |= (6u << CAN_BTR_BRP_Pos) | (11u << CAN_BTR_TS1_Pos) 
                | (2u << CAN_BTR_TS2_Pos) | (1u << CAN_BTR_SJW_Pos);
    // TODO: Loopback Mode For Testing
    CAN1->BTR |= CAN_BTR_LBKM; // Loopback Mode
    CAN1->MCR &= ~CAN_MCR_INRQ; // Exit Initialization Mode
    while (CAN1->MSR & CAN_MSR_INAK); // Wait until Normal Mode is entered
}

/**
 * @brief Transmit a CAN Frame
 * 
 * @param frame [CAN_Frame*] Frame struct to transmit
 */
void CAN1_Transmit(CAN_Frame* frame) {
    //TODO: Implement
}