/************************************************
* @file    can.h 
* @author  APBashara
* @date    7/2024
* 
* @brief   Prototype Functions for CAN Driver
***********************************************/

#include "stm32f407xx.h"

typedef enum {
    CAN_RTR_Data,
    CAN_RTR_Remote
} CAN_RTR;

typedef enum {
    CAN_OK,
    CAN_TX_Req,
    CAN_Error,
    CAN_Mailbox_Error
} CAN_Status;

typedef struct {
    uint16_t id; // 11-bit ID
    uint8_t dlc; // Data Length Code
    CAN_RTR rtr; // Remote Transmission Request
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
 * @param CAN [CAN_TypeDef*] CAN Peripheral to receive from
 * @param frame [CAN_Frame*] Frame to transmit
 * @return [CAN_Status] Status of Transmission
 */
CAN_Status CAN_Transmit(CAN_TypeDef* CAN, CAN_Frame* frame);

/**
 * @brief Receive a CAN Frame
 * 
 * @param CAN [CAN_TypeDef*] CAN Peripheral to receive from
 * @param frame [CAN_Frame*] Frame struct to receive
 * @return [CAN_Status] Status of Reception
 */
CAN_Status CAN_Receive(CAN_TypeDef* CAN, CAN_Frame* frame);