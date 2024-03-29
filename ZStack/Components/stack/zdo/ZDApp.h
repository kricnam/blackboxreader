/**************************************************************************************************
  Filename:       ZDApp.h
  Revised:        $Date: 2007-10-28 18:43:04 -0700 (Sun, 28 Oct 2007) $
  Revision:       $Revision: 15800 $

  Description:    This file contains the interface to the Zigbee Device Application. This is the
                  Application part that the use can change. This also contains the Task functions.


  Copyright 2004-2007 Texas Instruments Incorporated. All rights reserved.

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

#ifndef ZDAPP_H
#define ZDAPP_H

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */
#include "ZComDef.h"
#include "ZMac.h"
#include "NLMEDE.h"
#include "APS.h"
#include "AF.h"
#include "ZDProfile.h"

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * CONSTANTS
 */

// Set this value for use in choosing a PAN to join
// modify ZDApp.c to enable this...
#define ZDO_CONFIG_MAX_BO             15

  // Task Events
#define ZDO_NETWORK_INIT          0x0001
#define ZDO_NETWORK_START         0x0002
#define ZDO_DEVICE_RESET          0x0004
#define ZDO_COMMAND_CNF           0x0008
#define ZDO_STATE_CHANGE_EVT      0x0010
#define ZDO_ROUTER_START          0x0020
#define ZDO_NEW_DEVICE            0x0040
#define ZDO_DEVICE_AUTH           0x0080
#define ZDO_SECMGR_EVENT          0x0100
#define ZDO_NWK_UPDATE_NV         0x0200
#define ZDO_FRAMECOUNTER_CHANGE   0x0400

// Incoming to ZDO
#define ZDO_NWK_DISC_CNF        0x01
#define ZDO_NWK_JOIN_IND        0x02
#define ZDO_NWK_JOIN_REQ        0x03
#define ZDO_ESTABLISH_KEY_CFM   0x04
#define ZDO_ESTABLISH_KEY_IND   0x05
#define ZDO_TRANSPORT_KEY_IND   0x06
#define ZDO_UPDATE_DEVICE_IND   0x07
#define ZDO_REMOVE_DEVICE_IND   0x08
#define ZDO_REQUEST_KEY_IND     0x09
#define ZDO_SWITCH_KEY_IND      0x0A

//  ZDO command message fields
#define ZDO_CMD_ID     0
#define ZDO_CMD_ID_LEN 1

//  ZDO security message fields
#define ZDO_ESTABLISH_KEY_CFM_LEN   \
  ((uint8)                          \
   (sizeof(ZDO_EstablishKeyCfm_t) ) )

#define ZDO_ESTABLISH_KEY_IND_LEN   \
  ((uint8)                          \
   (sizeof(ZDO_EstablishKeyInd_t) ) )

#define ZDO_TRANSPORT_KEY_IND_LEN   \
  ((uint8)                          \
   (sizeof(ZDO_TransportKeyInd_t) ) )

#define ZDO_UPDATE_DEVICE_IND_LEN   \
  ((uint8)                          \
   (sizeof(ZDO_UpdateDeviceInd_t) ) )

#define ZDO_REMOVE_DEVICE_IND_LEN   \
  ((uint8)                          \
   (sizeof(ZDO_RemoveDeviceInd_t) ) )

#define ZDO_REQUEST_KEY_IND_LEN   \
  ((uint8)                        \
   (sizeof(ZDO_RequestKeyInd_t) ) )

#define ZDO_SWITCH_KEY_IND_LEN   \
  ((uint8)                       \
   (sizeof(ZDO_SwitchKeyInd_t) ) )

#define NWK_RETRY_DELAY             1000   // in milliseconds

#define ZDO_MATCH_DESC_ACCEPT_ACTION    1   // Message field

// Options for ZDApp_StartUpFromApp()
#define ZDAPP_STARTUP_COORD   2       // Start up as coordinator only
#define ZDAPP_STARTUP_ROUTER  1       // Start up as router only
#define ZDAPP_STARTUP_AUTO    0       // Startup in auto, look for coord,
                                      // if not found, become coord.

#define NUM_DISC_ATTEMPTS           2

// ZDOInitDevice return values
#define ZDO_INITDEV_RESTORED_NETWORK_STATE      0x00
#define ZDO_INITDEV_NEW_NETWORK_STATE           0x01
#define ZDO_INITDEV_LEAVE_NOT_STARTED           0x02

#if defined ( MANAGED_SCAN )
  // Only use in a battery powered device

  // This is the number of times a channel is scanned at the shortest possible
  // scan time (which is 30 MS (2 x 15).  The idea is to scan one channel at a
  // time (from the channel list), but scan it multiple times.
  #define MANAGEDSCAN_TIMES_PRE_CHANNEL         5
  #define MANAGEDSCAN_DELAY_BETWEEN_SCANS       150   // milliseconds

  extern byte zdoDiscCounter;

#endif // MANAGED_SCAN

/*********************************************************************
 * TYPEDEFS
 */
typedef enum
{
  DEV_HOLD,               // Initialized - not started automatically
  DEV_INIT,               // Initialized - not connected to anything
  DEV_NWK_DISC,           // Discovering PAN's to join
  DEV_NWK_JOINING,        // Joining a PAN
  DEV_NWK_REJOIN,         // ReJoining a PAN, only for end devices
  DEV_END_DEVICE_UNAUTH,  // Joined but not yet authenticated by trust center
  DEV_END_DEVICE,         // Started as device after authentication
  DEV_ROUTER,             // Device joined, authenticated and is a router
  DEV_COORD_STARTING,     // Started as Zigbee Coordinator
  DEV_ZB_COORD,           // Started as Zigbee Coordinator
  DEV_NWK_ORPHAN          // Device has lost information about its parent..
} devStates_t;

typedef enum
{
  ZDO_SUCCESS,
  ZDO_FAIL
} zdoStatus_t;

typedef struct
{
  osal_event_hdr_t hdr;
  uint8      panIdLSB;
  uint8      panIdMSB;
  uint8      logicalChannel;
  uint8      version;
  uint8      extendedPANID[Z_EXTADDR_LEN];
} ZDO_NetworkDiscoveryCfm_t;

typedef struct
{
  osal_event_hdr_t hdr;
  uint8       dstAddrDstEP;
  zAddrType_t dstAddr;
  uint8       dstAddrClusterIDLSB;
  uint8       dstAddrClusterIDMSB;
  uint8       dstAddrRemove;
  uint8       dstAddrEP;
} ZDO_NewDstAddr_t;

//  ZDO security message types
typedef struct
{
  osal_event_hdr_t hdr;
  uint8            partExtAddr[Z_EXTADDR_LEN];
  uint8            status;
} ZDO_EstablishKeyCfm_t;

typedef struct
{
  osal_event_hdr_t hdr;
  uint16           srcAddr;
  uint8            initExtAddr[Z_EXTADDR_LEN];
  uint8            method;
  uint8            secure;
} ZDO_EstablishKeyInd_t;

typedef struct
{
  osal_event_hdr_t hdr;
  uint16           srcAddr;
  uint8            keyType;
  uint8            keySeqNum;
  uint8            key[SEC_KEY_LEN];
  uint8            srcExtAddr[Z_EXTADDR_LEN];
  uint8            initiator;
  uint8            secure;
} ZDO_TransportKeyInd_t;

typedef struct
{
  osal_event_hdr_t hdr;
  uint16           srcAddr;
  uint8            devExtAddr[Z_EXTADDR_LEN];
  uint16           devAddr;
  uint8            status;
} ZDO_UpdateDeviceInd_t;

typedef struct
{
  osal_event_hdr_t hdr;
  uint16           srcAddr;
  uint8            childExtAddr[Z_EXTADDR_LEN];
} ZDO_RemoveDeviceInd_t;

typedef struct
{
  osal_event_hdr_t hdr;
  uint16           srcAddr;
  uint8            keyType;
  uint8            partExtAddr[Z_EXTADDR_LEN];
} ZDO_RequestKeyInd_t;

typedef struct
{
  osal_event_hdr_t hdr;
  uint16           srcAddr;
  uint8            keySeqNum;
} ZDO_SwitchKeyInd_t;

typedef struct
{
  osal_event_hdr_t hdr;
  uint16 nwkAddr;
  uint8 numInClusters;
  uint16 *pInClusters;
  uint8 numOutClusters;
  uint16 *pOutClusters;
} ZDO_MatchDescRspSent_t;

/*********************************************************************
 * GLOBAL VARIABLES
 */
extern byte ZDAppTaskID;
extern byte nwkStatus;
extern devStates_t devState;

/* Always kept up to date by the network state changed logic, so use this addr
 * in function calls the require my network address as one of the parameters.
 */
extern zAddrType_t ZDAppNwkAddr;
extern byte saveExtAddr[];  // Loaded with value by ZDApp_Init().

#if defined ( ZDO_MGMT_NWKDISC_RESPONSE )
  extern byte zdappMgmtNwkDiscRspTransSeq;
  extern byte zdappMgmtNwkDiscReqInProgress;
  extern zAddrType_t zdappMgmtNwkDiscRspAddr;
  extern byte zdappMgmtNwkDiscStartIndex;
  extern byte zdappMgmtSavedNwkState;
#endif

/*********************************************************************
 * FUNCTIONS - API
 */

/*********************************************************************
 * Task Level Control
 */
  /*
   * ZdApp Task Initialization Function
   */
  extern void ZDApp_Init( byte task_id );

  /*
   * ZdApp Task Event Processing Function
   */
  extern UINT16 ZDApp_event_loop( byte task_id, UINT16 events );

/*********************************************************************
 * Application Level Functions
 */

/*
 *  Start the device in the network.  This function will read 
 *   ZCD_NV_STARTUP_OPTION (NV item) to determine whether or not to 
 *   restore the network state of the device.
 * 
 *  startDelay - timeDelay to start device (in milliseconds).
 *      There is a jitter added to this delay:
 *              ((NWK_START_DELAY + startDelay)
 *              + (osal_rand() & EXTENDED_JOINING_RANDOM_MASK)) 
 *  
 *  NOTE:   If the application would like to force a "new" join, the
 *          application should set the ZCD_STARTOPT_DEFAULT_NETWORK_STATE
 *          bit in the ZCD_NV_STARTUP_OPTION NV item before calling 
 *          this function.
 *
 *  returns: 
 *    ZDO_INITDEV_RESTORED_NETWORK_STATE  - The device's network state was
 *          restored.
 *    ZDO_INITDEV_NEW_NETWORK_STATE - The network state was initialized.
 *          This could mean that ZCD_NV_STARTUP_OPTION said to not restore, or
 *          it could mean that there was no network state to restore.
 *    ZDO_INITDEV_LEAVE_NOT_STARTED - Before the reset, a network leave was issued
 *          with the rejoin option set to TRUE.  So, the device was not
 *          started in the network (one time only).  The next time this
 *          function is called it will start.
 */
extern uint8 ZDOInitDevice( uint16 startDelay );

/*
 * Sends an osal message to ZDApp with parameter as the msg data byte.
 */
extern void ZDApp_SendEventMsg( byte cmd, byte len, byte *buf );

/*
 * ZdApp Function for Establishing a Link Key
 */
extern ZStatus_t ZDApp_EstablishKey( uint8  endPoint,
                                     uint16 nwkAddr,
                                     uint8* extAddr );

/*
 * Start the network formation process
 *    delay - millisecond wait before
 */
extern void ZDApp_NetworkInit( uint16 delay );

/*********************************************************************
 * Internal FUNCTIONS - API
 */

/*********************************************************************
 * Call Back Functions from NWK  - API
 */

/*
 * ZDO_NetworkDiscoveryConfirmCB - scan results
 */

extern ZStatus_t ZDO_NetworkDiscoveryConfirmCB( byte ResultCount,
                              networkDesc_t *NetworkList );

/*
 * ZDO_NetworkFormationConfirm - results of the request to
 *              initialize a coordinator in a network
 */
extern void ZDO_NetworkFormationConfirmCB( ZStatus_t Status );
/*
 * ZDO_JoinConfirmCB - results of its request to join itself or another
 *              device to a network
 */
extern void ZDO_JoinConfirmCB( uint16 PanId, ZStatus_t Status );

/*
 * ZDO_JoinIndicationCB - notified of a remote join request
 */
extern ZStatus_t ZDO_JoinIndicationCB( uint16 ShortAddress,
                      byte *ExtendedAddress, byte CapabilityInformation );

/*
 * ZDO_StartRouterConfirm -  results of the request to
 *              start functioning as a router in a network
 */
extern void ZDO_StartRouterConfirmCB( ZStatus_t Status );

/*
 * ZDO_LeaveCnf - results of the request for this or a child device
 *                to leave
 */
extern void ZDO_LeaveCnf( NLME_LeaveCnf_t* cnf );

/*
 * ZDO_LeaveInd - notified of a remote leave request or indication
 */
extern void ZDO_LeaveInd( NLME_LeaveInd_t* ind );

/*
 * ZDO_SyncIndicationCB - notified of sync loss with parent
 */
extern void ZDO_SyncIndicationCB( byte type, uint16 shortAddr );

/*
 * ZDO_PollConfirmCB - notified of poll confirm
 */
extern void ZDO_PollConfirmCB( byte status );

/*********************************************************************
 * Call Back Functions from Security  - API
 */
extern ZStatus_t ZDO_UpdateDeviceIndication( byte *extAddr, byte status );



/*
 * ZDApp_InMsgCB - Allow the ZDApp to handle messages that are not supported
 */
extern void ZDApp_InMsgCB( zdoIncomingMsg_t *inMsg );

extern void ZDO_StartRouterConfirm( ZStatus_t Status );

/*
 * ZDApp_EndDeviceAnnounceCB - Received an End Device Announce
 */
extern void ZDApp_EndDeviceAnnounceCB( uint16 SrcAddr, uint16 nwkAddr,
                                      uint8 *extAddr, uint8 capabilities );

/*********************************************************************
 * ZDO Control Functions
 */

/*
 * ZDApp_ChangeMatchDescRespPermission
 *    - Change the Match Descriptor Response permission.
 */
extern void ZDApp_ChangeMatchDescRespPermission( uint8 endpoint, uint8 action );

/*
 * ZDApp_ResetNwkKey
 *    - Re initialize the NV Nwk Key
 */
extern void ZDApp_ResetNwkKey( void );

/*
 * ZDApp_StartUpFromApp
 *    - Start the device.  This function will only start a device
 *          that is in the Holding state.
 *
 * mode - ZDAPP_STARTUP_COORD - Start up as coordinator only
 *        ZDAPP_STARTUP_ROUTER - Start up as router only
 *        ZDAPP_STARTUP_AUTO - Startup in auto, look for coord,
 *                               if not found, become coord.
 *
 * returns  ZSucess if started, ZFailure if in the wrong mode
 */
extern ZStatus_t ZDApp_StartUpFromApp( uint8 mode );

/*
 * ZDApp_StopStartUp
 *    - Stops the joining process of a router.  This will only
 *          work if the router is in the scanning process and
 *          the SOFT_START feature is enabled.
 *
 *    returns  TRUE if SOFT_START is enabled, FALSE if not possible
 */
extern uint8 ZDApp_StopStartUp( void );


/*
 * ZDApp_StartJoiningCycle
 *    - Starts the joining cycle of a device.  This will only continue an
 *      already started (or stopped) joining cycle.
 *
 *    returns  TRUE if joining started, FALSE if not in joining or rejoining
 */
extern uint8 ZDApp_StartJoiningCycle( void );

/*
 * ZDApp_StopJoiningCycle
 *    - Stops the joining or rejoining process of a device.
 *
 *    returns  TRUE if joining stopped, FALSE if joining or rejoining
 */
extern uint8 ZDApp_StopJoiningCycle( void );

/*
 * Callback from network layer when coordinator start has a conflict with
 * an existing PAN ID.
 */
extern uint16 ZDApp_CoordStartPANIDConflictCB( uint16 panid );

/*
 * ZDApp_NVUpdate - Initiate an NV update
 */
extern void ZDApp_NVUpdate( void );



/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* ZDOBJECT_H */
