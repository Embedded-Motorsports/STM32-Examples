/************************************************
* @file    can.h 
* @author  APBashara
* @date    7/2024
* 
* @brief   Prototype Functions for CAN Driver
***********************************************/

#include "stm32f407xx.h"

typedef struct {
    uint16_t id; // 11-bit ID
    uint8_t dlc; // Data Length Code
    uint8_t data[8]; // Data Bytes
} CAN_Frame;

/**
 * @brief Initializes CAN1
 * 
 * @note Pins PA11(Rx) and PA12 (Tx)
 * @note Baud Rate: 500kbps
 */
void CAN1_Init();

/**
 * @brief Transmit a CAN Frame
 * 
 * @param frame [CAN_Frame*] Frame to transmit
 */
void CAN1_Transmit(CAN_Frame* frame);