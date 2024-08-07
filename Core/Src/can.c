/************************************************
* @file    can.c 
* @author  APBashara
* @date    7/2024
* 
* @brief   CAN Driver Implementation
***********************************************/

#include "stm32f407xx.h"
#include "can.h"

static uint8_t Get_Empty_Mailbox() {
    if (CAN1->TSR & CAN_TSR_TME0) {
        return 0u;
    } else if (CAN1->TSR & CAN_TSR_TME1) {
        return 1u;
    } else if (CAN1->TSR & CAN_TSR_TME2) {
        return 2u;
    } else {
        return 0xFFu;
    }
}

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
CAN_Status CAN_Transmit(CAN_TypeDef* CAN, CAN_Frame* frame) {
    uint8_t mailbox = Get_Empty_Mailbox();
    if (mailbox == 0xFFu) {
        return CAN_Mailbox_Error;
    }

    // Set ID, DLC, Frame Type, and Data
    CAN->sTxMailBox[mailbox].TIR = 0x0UL; // Clear the mailbox register
    CAN->sTxMailBox[mailbox].TIR |= (frame->id << CAN_TI0R_STID_Pos)
                                    | (frame->rtr << CAN_TI0R_RTR_Pos);
    CAN->sTxMailBox[mailbox].TDTR = (frame->dlc << CAN_TDT0R_DLC_Pos);
    CAN->sTxMailBox[mailbox].TDLR = (frame->data[0] << CAN_TDL0R_DATA0_Pos) 
                                    | (frame->data[1] << CAN_TDL0R_DATA1_Pos)
                                    | (frame->data[2] << CAN_TDL0R_DATA2_Pos) 
                                    | (frame->data[3] << CAN_TDL0R_DATA3_Pos);
    CAN->sTxMailBox[mailbox].TDHR = (frame->data[4] << CAN_TDH0R_DATA4_Pos) 
                                    | (frame->data[5] << CAN_TDH0R_DATA5_Pos) 
                                    | (frame->data[6] << CAN_TDH0R_DATA6_Pos) 
                                    | (frame->data[7] << CAN_TDH0R_DATA7_Pos);
    // Request Transmission
    CAN->sTxMailBox[mailbox].TIR |= CAN_TI0R_TXRQ;
    return CAN_TX_Req;
}