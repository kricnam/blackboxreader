/**************************************************************************************************
  Filename:       mac_tx.h
  Revised:        $Date: 2007-10-29 22:38:47 -0700 (Mon, 29 Oct 2007) $
  Revision:       $Revision: 15812 $

  Description:    Describe the purpose and contents of the file.


  Copyright 2006-2007 Texas Instruments Incorporated. All rights reserved.

  IMPORTANT: Your use of this Software is limited to those specific rights
  granted under the terms of a software license agreement between the user
  who downloaded the software, his/her employer (which must be your employer)
  and Texas Instruments Incorporated (the "License").  You may not use this
  Software unless you agree to abide by the terms of the License. The License
  limits your use, and you acknowledge, that the Software may not be modified,
  copied or distributed unless embedded on a Texas Instruments microcontroller
  or used solely and exclusively in conjunction with a Texas Instruments radio
  frequency transceiver, which is integrated into your product.  Other than for
  the foregoing purpose, you may not use, reproduce, copy, prepare derivative
  works of, modify, distribute, perform, display or sell this Software and/or
  its documentation for any purpose.

  YOU FURTHER ACKNOWLEDGE AND AGREE THAT THE SOFTWARE AND DOCUMENTATION ARE
  PROVIDED �AS IS� WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED, 
  INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF MERCHANTABILITY, TITLE, 
  NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT SHALL
  TEXAS INSTRUMENTS OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER CONTRACT,
  NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR OTHER
  LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
  INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE
  OR CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT
  OF SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
  (INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.

  Should you have any questions regarding your right to use this Software,
  contact Texas Instruments Incorporated at www.TI.com. 
**************************************************************************************************/

#ifndef MAC_TX_H
#define MAC_TX_H

/* ------------------------------------------------------------------------------------------------
 *                                         Includes
 * ------------------------------------------------------------------------------------------------
 */
#include "hal_types.h"


/* ------------------------------------------------------------------------------------------------
 *                                          Define
 * ------------------------------------------------------------------------------------------------
 */
/* bit value used to form values of macTxActive */
#define MAC_TX_ACTIVE_PHYSICALLY_BV      0x80

/* state values for macTxActive; note zero is reserved for inactive state */
#define MAC_TX_ACTIVE_NO_ACTIVITY           0x00 /* zero reserved for boolean use, e.g. !macTxActive */
#define MAC_TX_ACTIVE_INITIALIZE            0x01
#define MAC_TX_ACTIVE_QUEUED                0x02
#define MAC_TX_ACTIVE_GO                   (0x03 | MAC_TX_ACTIVE_PHYSICALLY_BV)
#define MAC_TX_ACTIVE_CHANNEL_BUSY          0x04
#define MAC_TX_ACTIVE_DONE                 (0x05 | MAC_TX_ACTIVE_PHYSICALLY_BV)
#define MAC_TX_ACTIVE_LISTEN_FOR_ACK       (0x06 | MAC_TX_ACTIVE_PHYSICALLY_BV)
#define MAC_TX_ACTIVE_POST_ACK             (0x07 | MAC_TX_ACTIVE_PHYSICALLY_BV)


/* ------------------------------------------------------------------------------------------------
 *                                          Define
 * ------------------------------------------------------------------------------------------------
 */
#define MAC_TX_IS_PHYSICALLY_ACTIVE()       (macTxActive & MAC_TX_ACTIVE_PHYSICALLY_BV)


/* ------------------------------------------------------------------------------------------------
 *                                   Global Variable Externs
 * ------------------------------------------------------------------------------------------------
 */
extern uint8 macTxActive;
extern uint8 macTxBe;
extern uint8 macTxType;
extern uint8 macTxCsmaBackoffDelay;


/* ------------------------------------------------------------------------------------------------
 *                                       Prototypes
 * ------------------------------------------------------------------------------------------------
 */
void macTxInit(void);
void macTxHaltCleanup(void);
void macTxStartQueuedFrame(void);
void macTxChannelBusyCallback(void);
void macTxDoneCallback(void);
void macTxAckReceivedCallback(uint8 seqn, uint8 pendingFlag);
void macTxAckNotReceivedCallback(void);
void macTxTimestampCallback(void);
void macTxCollisionWithRxCallback(void);


/**************************************************************************************************
 */
#endif
