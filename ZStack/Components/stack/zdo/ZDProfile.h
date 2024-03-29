/**************************************************************************************************
  Filename:       ZDProfile.h
  Revised:        $Date: 2007-10-28 18:43:04 -0700 (Sun, 28 Oct 2007) $
  Revision:       $Revision: 15800 $

  Description:    This file contains the interface to the Zigbee Device Object.


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

#ifndef ZDPROFILE_H
#define ZDPROFILE_H

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */
#include "ZComDef.h"
#include "NLMEDE.h"
#include "AF.h"
#include "ZDConfig.h"

/*********************************************************************
 * CONSTANTS
 */

#define ZDO_EP 0   // Endpoint of ZDO
#define ZDO_PROFILE_ID 0

// IEEE_addr_req request types
#define ZDP_ADDR_REQTYPE_SINGLE     0
#define ZDP_ADDR_REQTYPE_EXTENDED   1
#define ZDP_ADDR_REQTYPE_MEMBERSHIP 2

// ZDO Status Values
#define ZDP_SUCCESS            0x00  // Operation completed successfully
#define ZDP_INVALID_REQTYPE    0x80  // The supplied request type was invalid
#define ZDP_DEVICE_NOT_FOUND   0x81  // Reserved
#define ZDP_INVALID_EP         0x82  // Invalid endpoint value
#define ZDP_NOT_ACTIVE         0x83  // Endpoint not described by a simple desc.
#define ZDP_NOT_SUPPORTED      0x84  // Optional feature not supported
#define ZDP_TIMEOUT            0x85  // Operation has timed out
#define ZDP_NO_MATCH           0x86  // No match for end device bind
#define ZDP_TABLE_FULL         0x87  // Bind request failed, no resources
#define ZDP_NO_ENTRY           0x88  // Unbind request failed, no entry

#define ZDP_INSUFFICIENT_SPACE 0x8a
#define ZDP_NOT_PERMITTED      0x8b

#define ZDP_NETWORK_DISCRIPTOR_SIZE  8
#define ZDP_NETWORK_EXTENDED_DISCRIPTOR_SIZE  14
#define ZDP_RTG_DISCRIPTOR_SIZE      5
#define ZDP_BIND_DISCRIPTOR_SIZE  19

// Mgmt_Permit_Join_req fields
#define ZDP_MGMT_PERMIT_JOIN_REQ_DURATION 0
#define ZDP_MGMT_PERMIT_JOIN_REQ_TC_SIG   1
#define ZDP_MGMT_PERMIT_JOIN_REQ_SIZE     2

// Mgmt_Lqi_rsp - (neighbor table) device type
#define ZDP_MGMT_DT_COORD  0x0
#define ZDP_MGMT_DT_ROUTER 0x1
#define ZDP_MGMT_DT_ENDDEV 0x2

// Mgmt_Lqi_rsp - (neighbor table) relationship
#define ZDP_MGMT_REL_PARENT  0x0
#define ZDP_MGMT_REL_CHILD   0x1
#define ZDP_MGMT_REL_SIBLING 0x2
#define ZDP_MGMT_REL_UNKNOWN 0x3

// Mgmt_Lqi_rsp - (neighbor table) unknown boolean value
#define ZDP_MGMT_BOOL_UNKNOWN 0x02

/*********************************************************************
 * Message/Cluster IDS
 */

// ZDO Cluster IDs
#define ZDO_RESPONSE_BIT_V1_0   ((uint8)0x80)
#define ZDO_RESPONSE_BIT        ((uint16)0x8000)

#define NWK_addr_req            ((uint16)0x0000)
#define IEEE_addr_req           ((uint16)0x0001)
#define Node_Desc_req           ((uint16)0x0002)
#define Power_Desc_req          ((uint16)0x0003)
#define Simple_Desc_req         ((uint16)0x0004)
#define Active_EP_req           ((uint16)0x0005)
#define Match_Desc_req          ((uint16)0x0006)
#define NWK_addr_rsp            (NWK_addr_req | ZDO_RESPONSE_BIT)
#define IEEE_addr_rsp           (IEEE_addr_req | ZDO_RESPONSE_BIT)
#define Node_Desc_rsp           (Node_Desc_req | ZDO_RESPONSE_BIT)
#define Power_Desc_rsp          (Power_Desc_req | ZDO_RESPONSE_BIT)
#define Simple_Desc_rsp         (Simple_Desc_req | ZDO_RESPONSE_BIT)
#define Active_EP_rsp           (Active_EP_req | ZDO_RESPONSE_BIT)
#define Match_Desc_rsp          (Match_Desc_req | ZDO_RESPONSE_BIT)

#define Complex_Desc_req        ((uint16)0x0010)
#define User_Desc_req           ((uint16)0x0011)
#define Discovery_Cache_req     ((uint16)0x0012)
#define End_Device_annce        ((uint16)0x0013)
#define User_Desc_set           ((uint16)0x0014)
#define Server_Discovery_req    ((uint16)0x0015)
#define Complex_Desc_rsp        (Complex_Desc_req | ZDO_RESPONSE_BIT)
#define User_Desc_rsp           (User_Desc_req | ZDO_RESPONSE_BIT)
#define Discovery_Cache_rsp     (Discovery_Cache_req | ZDO_RESPONSE_BIT)
#define User_Desc_conf          (User_Desc_set | ZDO_RESPONSE_BIT)
#define Server_Discovery_rsp    (Server_Discovery_req | ZDO_RESPONSE_BIT)

#define End_Device_Bind_req     ((uint16)0x0020)
#define Bind_req                ((uint16)0x0021)
#define Unbind_req              ((uint16)0x0022)
#define Bind_rsp                (Bind_req | ZDO_RESPONSE_BIT)
#define End_Device_Bind_rsp     (End_Device_Bind_req | ZDO_RESPONSE_BIT)
#define Unbind_rsp              (Unbind_req | ZDO_RESPONSE_BIT)

#define Mgmt_NWK_Disc_req       ((uint16)0x0030)
#define Mgmt_Lqi_req            ((uint16)0x0031)
#define Mgmt_Rtg_req            ((uint16)0x0032)
#define Mgmt_Bind_req           ((uint16)0x0033)
#define Mgmt_Leave_req          ((uint16)0x0034)
#define Mgmt_Direct_Join_req    ((uint16)0x0035)
#define Mgmt_Permit_Join_req    ((uint16)0x0036)
#define Mgmt_NWK_Disc_rsp       (Mgmt_NWK_Disc_req | ZDO_RESPONSE_BIT)
#define Mgmt_Lqi_rsp            (Mgmt_Lqi_req | ZDO_RESPONSE_BIT)
#define Mgmt_Rtg_rsp            (Mgmt_Rtg_req | ZDO_RESPONSE_BIT)
#define Mgmt_Bind_rsp           (Mgmt_Bind_req | ZDO_RESPONSE_BIT)
#define Mgmt_Leave_rsp          (Mgmt_Leave_req | ZDO_RESPONSE_BIT)
#define Mgmt_Direct_Join_rsp    (Mgmt_Direct_Join_req | ZDO_RESPONSE_BIT)
#define Mgmt_Permit_Join_rsp    (Mgmt_Permit_Join_req | ZDO_RESPONSE_BIT)

/*********************************************************************
 * TYPEDEFS
 */

#define ZDP_BINDINGENTRY_SIZE   19
  
typedef struct
{
  osal_event_hdr_t hdr;
  zAddrType_t      srcAddr;
  uint8            wasBroadcast;
  cId_t            clusterID;
  uint8            SecurityUse;
  uint8            TransSeq;
  uint8            asduLen;
  uint8            *asdu;
} zdoIncomingMsg_t;

// This structure is used to build the Mgmt Network Discover Response
typedef struct
{
  uint8 extendedPANID[Z_EXTADDR_LEN];   // The extended PAN ID
  uint16 PANId;           // The network PAN ID
  byte   logicalChannel;  // Network's channel
  byte   stackProfile;    // Network's profile
  byte   version;         // Network's Zigbee version
  byte   beaconOrder;     // Beacon Order
  byte   superFrameOrder; 
  byte   permitJoining;   // PermitJoining. 1 or 0
} mgmtNwkDiscItem_t;

// This structure is used to build the Mgmt LQI Response
typedef struct
{
  uint16 nwkAddr;         // device's short address
  uint16 PANId;           // The neighbor device's PAN ID
  uint8  extPANId[Z_EXTADDR_LEN]; // The neighbor device's Extended PanID
  byte   txQuality;       // Transmit quality
  byte   rxLqi;           // Receive LQI
} neighborLqiItem_t;
#define ZDP_NEIGHBORLQI_SIZE    12

// This structure is used to build the Mgmt_Lqi_rsp
typedef struct
{
  uint16 panID;                  // PAN Id
  uint8  extPanID[Z_EXTADDR_LEN];// Extended Pan ID
  uint8  extAddr[Z_EXTADDR_LEN]; // Extended address
  uint16 nwkAddr;                // Network address
  uint8  devType;                // Device type
  uint8  rxOnIdle;               // RxOnWhenIdle
  uint8  relation;               // Relationship
  uint8  permit;                 // Permit joining
  uint8  depth;                  // Depth
  uint8  lqi;                    // LQI
} ZDP_MgmtLqiItem_t;
// devType, rxOnIdle, relation, are all packed into 1 byte: 18-2=16.
#define ZDP_MGMTLQI_SIZE        15
#define ZDP_MGMTLQI_EXTENDED_SIZE  22   // One extra byte for permitJointing, also with extended PanID instead of PanID 15+8-2+1 = 22

// This structure is used to build the Mgmt Routing Response
//   Notice, if you change this structure, you must also change
//   rtgEntry_t in rtg.h
typedef struct
{
    uint16  dstAddress;     // Destination short address
    uint16  nextHopAddress; // next hop short address
    byte    expiryTime;     // expiration time - not used for response
    byte    status;         // route entry status
} rtgItem_t;
// expiryTime is not packed & sent OTA.
#define ZDP_ROUTINGENTRY_SIZE   5

typedef struct
{
  uint8  TransSeq;
  byte SecurityUse;
  uint16 srcAddr;
  uint16 localCoordinator;
  uint8  ieeeAddr[Z_EXTADDR_LEN];
  uint8  endpoint;
  uint16 profileID;
  uint8  numInClusters;
  uint16 *inClusters;
  uint8  numOutClusters;
  uint16 *outClusters;
} ZDEndDeviceBind_t;

/*********************************************************************
 * GLOBAL VARIABLES
 */

extern byte ZDP_TransID;

/*********************************************************************
 * MACROS
 */

/*
 * Generic data send function
 */
extern afStatus_t ZDP_SendData( uint8 *transSeq, zAddrType_t *dstAddr, uint16 cmd, byte len,
                                              byte *buf, byte SecurityEnable );

#if defined ( ZDO_NODEDESC_REQUEST )
/*
 * ZDP_NodeDescReq - Request a Node Description
 *
 * @MT SPI_CMD_ZDO_NODE_DESC_REQ
 * (UInt16 DstAddr,
 *  UInt16 NWKAddrOfInterest,
 *  byte SecuritySuite)
 *
 */
#define ZDP_NodeDescReq( dstAddr, NWKAddrOfInterest, SecurityEnable ) \
                          ZDP_NWKAddrOfInterestReq(  dstAddr, \
                              NWKAddrOfInterest, Node_Desc_req, SecurityEnable )
#endif

#if defined ( ZDO_POWERDESC_REQUEST )
/*
 * ZDP_PowerDescReq - Request a Power Description
 *
 * @MT  SPI_CMD_ZDO_POWER_DESC_REQ
 * (UInt16 DstAddr,
 *  UInt16 NWKAddrOfInterest,
 *  byte SecuritySuit)
 *
 */
#define ZDP_PowerDescReq( dstAddr, NWKAddrOfInterest, SecurityEnable ) \
                          ZDP_NWKAddrOfInterestReq(  dstAddr, \
                              NWKAddrOfInterest, Power_Desc_req, SecurityEnable )
#endif

#if defined ( ZDO_ACTIVEEP_REQUEST )
/*
 * ZDP_ActiveEPReq - Request a device's endpoint list
 *
 * @MT SPI_CMD_ZDO_ACTIVE_EPINT_REQ
 * (UInt16 DstAddr,
 *  UInt16 NWKAddrOfInterest,
 *  byte SecuritySuite)
 *
 */
#define ZDP_ActiveEPReq( dstAddr, NWKAddrOfInterest, SecurityEnable ) \
                          ZDP_NWKAddrOfInterestReq(  dstAddr, \
                            NWKAddrOfInterest, Active_EP_req, SecurityEnable )
#endif

#if defined ( ZDO_COMPLEXDESC_REQUEST )
/*
 * ZDP_ComplexDescReq - Request a device's complex description
 *
 * @MT SPI_CMD_ZDO_COMPLEX_DESC_REQ
 * (UInt16 DstAddr,
 *  UInt16 NWKAddrOfInterest,
 *  byte SecuritySuite)
 *
 */
#define ZDP_ComplexDescReq( dstAddr, NWKAddrOfInterest, SecurityEnable ) \
                          ZDP_NWKAddrOfInterestReq(  dstAddr, \
                            NWKAddrOfInterest, Complex_Desc_req, SecurityEnable )
#endif

#if defined ( ZDO_USERDESC_REQUEST )
/*
 * ZDP_UserDescReq - Request a device's user description
 *
 * @MT SPI_CMD_ZDO_USER_DESC_REQ
 * (UInt16 DstAddr,
 *  UInt16 NWKAddrOfInterest,
 *  byte SecuritySuite)
 *
 */
#define ZDP_UserDescReq( dstAddr, NWKAddrOfInterest, SecurityEnable ) \
                          ZDP_NWKAddrOfInterestReq(  dstAddr, \
                            NWKAddrOfInterest, User_Desc_req, SecurityEnable )
#endif

#if defined ( ZDO_BIND_UNBIND_REQUEST )
/*
 * ZDP_BindReq - bind request
 *
 * @MT SPI_CMD_ZDO_BIND_REQ
 * (UInt16 DstAddr,
 *  UInt64 SrcAddress,
 *  byte SrcEndpoint,
 *  uint16 ClusterID,
 *  zAddrType_t *DstAddress,
 *  byte DstEndpoint,
 *  byte SecuritySuite)
 *
 */
#define ZDP_BindReq( dstAddr, SourceAddr, SrcEP, \
              ClusterID, DestinationAddr, DstEP, SecurityEnable ) \
                       ZDP_BindUnbindReq( Bind_req, dstAddr, \
                            SourceAddr, SrcEP, ClusterID, \
                            DestinationAddr, DstEP, SecurityEnable )
#endif

#if defined ( ZDO_BIND_UNBIND_REQUEST )
/*
 * ZDP_UnbindReq - Unbind request
 *
 * @MT SPI_CMD_ZDO_UNBIND_REQ
 * (UInt16 DstAddr,
 *  UInt64 SrcAddress,
 *  byte SrcEndpoint,
 *  uint16 ClusterID,
 *  zAddrType_t DestinationAddr,
 *  byte DstEndpoint,
 *  byte SecuritySuite)
 *
 */
#define ZDP_UnbindReq( dstAddr, SourceAddr, SrcEP, \
              ClusterID, DestinationAddr, DstEP, SecurityEnable ) \
                       ZDP_BindUnbindReq( Unbind_req, dstAddr, \
                            SourceAddr, SrcEP, ClusterID, \
                            DestinationAddr, DstEP, SecurityEnable )
#endif

#if defined ( ZDO_MGMT_LQI_REQUEST )
/*
 * ZDP_MgmtLqiReq - Send a Management LQI Request
 *
 * @MT SPI_CMD_ZDO_MGMT_LQI_REQ
 * (UInt16 DstAddr,
 *  byte StartIndex)
 *
 */
#define ZDP_MgmtLqiReq( dstAddr, StartIndex, SecurityEnable ) \
          ZDP_SendData( &ZDP_TransID, dstAddr, Mgmt_Lqi_req, 1, &StartIndex, SecurityEnable )
#endif

#if defined ( ZDO_MGMT_RTG_REQUEST )
/*
 * ZDP_MgmtRtgReq - Send a Management Routing Table Request
 *
 * @MT SPI_CMD_ZDO_MGMT_RTG_REQ
 * (UInt16 DstAddr,
 *  byte StartIndex)
 *
 */
#define ZDP_MgmtRtgReq( dstAddr, StartIndex, SecurityEnable ) \
          ZDP_SendData( &ZDP_TransID, dstAddr, Mgmt_Rtg_req, 1, &StartIndex, SecurityEnable )
#endif

#if defined ( ZDO_MGMT_BIND_REQUEST )
/*
 * ZDP_MgmtBindReq - Send a Management Binding Table Request
 *
 * @MT SPI_CMD_ZDO_MGMT_BIND_REQ
 * (UInt16 DstAddr,
 *  byte StartIndex)
 *
 */
#define ZDP_MgmtBindReq( dstAddr, StartIndex, SecurityEnable ) \
         ZDP_SendData( &ZDP_TransID, dstAddr, Mgmt_Bind_req, 1, &StartIndex, SecurityEnable )
#endif

#if defined ( ZDO_MGMT_LEAVE_REQUEST )
/*
 * ZDP_MgmtLeaveReq - Send a Management Leave Request
 *
 * @MT SPI_CMD_ZDO_MGMT_LEAVE_REQ
 * (UInt16 DstAddr,
 *  UInt64 DeviceAddress)
 */
#define ZDP_MgmtLeaveReq( dstAddr, IEEEAddr, SecurityEnable ) \
  ZDP_SendData( &ZDP_TransID, dstAddr, Mgmt_Leave_req, Z_EXTADDR_LEN, IEEEAddr, SecurityEnable)
#endif

#ifdef REMOVE_BY_LAF
/*
 * ZDP_NWKAddrRsp - Send a Network Address Response
 */
#define ZDP_NWKAddrRsp( TransSeq, dstAddr, Status, IEEEAddrRemoteDev, ReqType, nwkAddr, NumAssocDev, \
                 StartIndex, NWKAddrAssocDevList, SecurityEnable ) \
                          ZDP_AddrRsp( NWK_addr_rsp, TransSeq, dstAddr, Status, \
                                IEEEAddrRemoteDev, ReqType, nwkAddr, NumAssocDev, StartIndex, \
                                NWKAddrAssocDevList, SecurityEnable )

/*
 * ZDP_IEEEAddrRsp - Send an IEEE Address Response
 */
#define ZDP_IEEEAddrRsp( TransSeq, dstAddr, Status, IEEEAddrRemoteDev, ReqType, nwkAddr, NumAssocDev, \
                 StartIndex, NWKAddrAssocDevList, SecurityEnable ) \
                          ZDP_AddrRsp( IEEE_addr_rsp, TransSeq, dstAddr, Status, \
                                IEEEAddrRemoteDev, ReqType, nwkAddr, NumAssocDev, StartIndex, \
                                NWKAddrAssocDevList, SecurityEnable )
#endif
/*
 * ZDP_ActiveEPRsp - Send an list of active endpoint
 */
#define ZDP_ActiveEPRsp( TransSeq, dstAddr, Status, nwkAddr, Count, \
                  pEPList, SecurityEnable ) \
                      ZDP_EPRsp( Active_EP_rsp, TransSeq, dstAddr, Status, \
                           nwkAddr, Count, pEPList, SecurityEnable )

/*
 * ZDP_MatchDescRsp - Send an list of endpoint that match
 */
#define ZDP_MatchDescRsp( TransSeq, dstAddr, Status, nwkAddr, Count, \
                  pEPList, SecurityEnable ) \
                      ZDP_EPRsp( Match_Desc_rsp, TransSeq, dstAddr, Status, \
                           nwkAddr, Count, pEPList, SecurityEnable )

/*
 * ZDP_ComplexDescRsp - This message isn't supported until we fix it.
 */
#define ZDP_ComplexDescRsp( dstAddr, SecurityEnable ) \
        ZDP_GenericRsp( dstAddr, Complex_Desc_rsp, SecurityEnable )

/*
 * ZDP_UserDescConf - Send a User Descriptor Set Response
 */
#define ZDP_UserDescConf( TransSeq, dstAddr, Status, SecurityEnable ) \
            ZDP_SendData( &TransSeq, dstAddr, User_Desc_conf, 1, &Status, SecurityEnable )

/*
 * ZDP_EndDeviceBindRsp - Send a End Device Bind Response
 */
#define ZDP_EndDeviceBindRsp( TransSeq, dstAddr, Status, SecurityEnable ) \
       ZDP_SendData( &TransSeq, dstAddr, End_Device_Bind_rsp, 1, &Status, SecurityEnable )

/*
 * ZDP_BindRsp - Send a Bind Response
 */
#define ZDP_BindRsp( TransSeq, dstAddr, Status, SecurityEnable ) \
                  ZDP_SendData( &TransSeq, dstAddr, Bind_rsp, 1, &Status, SecurityEnable )

/*
 * ZDP_UnbindRsp - Send an Unbind Response
 */
#define ZDP_UnbindRsp( TransSeq, dstAddr, Status, SecurityEnable ) \
                ZDP_SendData( &TransSeq, dstAddr, Unbind_rsp, 1, &Status, SecurityEnable )

/*
 * ZDP_MgmtLeaveRsp - Send a Management Leave Response
 */
#define ZDP_MgmtLeaveRsp( TransSeq, dstAddr, Status, SecurityEnable ) \
            ZDP_SendData( &TransSeq, dstAddr, Mgmt_Leave_rsp, 1, &Status, SecurityEnable )

/*
 * ZDP_MgmtPermitJoinRsp - Send a Management Permit Join Response
 */
#define ZDP_MgmtPermitJoinRsp( TransSeq, dstAddr, Status, SecurityEnable ) \
      ZDP_SendData( &TransSeq, dstAddr, Mgmt_Permit_Join_rsp, 1, &Status, SecurityEnable )

/*
 * ZDP_MgmtDirectJoinRsp - Send a Mgmt_DirectJoining_Rsp Response
 */
#define ZDP_MgmtDirectJoinRsp( TransSeq, dstAddr, Status, SecurityEnable ) \
      ZDP_SendData( &TransSeq, dstAddr, Mgmt_Direct_Join_rsp, 1, &Status, SecurityEnable )

/*********************************************************************
 * FUNCTIONS - API
 */

/*
 * ZDP_NWKAddrOfInterestReq - Send request with NWKAddrOfInterest as parameter
 */
extern afStatus_t ZDP_NWKAddrOfInterestReq( zAddrType_t *dstAddr,
                              uint16 nwkAddr, byte cmd, byte SecurityEnable );
/*
 * ZDP_NwkAddrReq - Request a Network address
 *
 * @MT SPI_CMD_ZDO_NWK_ADDR_REQ
 * (UInt64 IEEEAddress,
 *  byte ReqType,
 *  byte StarIndex,
 *  byte SecurityEnable)
 *
 */
extern afStatus_t ZDP_NwkAddrReq( byte *IEEEAddress, byte ReqType,
                               byte StartIndex, byte SecurityEnable );

/*
 * ZDP_IEEEAddrReq - Request an IEEE address
 *
 * @MT SPI_CMD_ZDO_IEEE_ADDR_REQ
 * (UInt16 shortAddr,
 *  byte ReqType,
 *  byte StartIndex,
 *  byte SecurityEnable)
 *
 */
extern afStatus_t ZDP_IEEEAddrReq( uint16 shortAddr, byte ReqType,
                                byte StartIndex, byte SecurityEnable );

/*
 * ZDP_MatchDescReq - Request matching device's endpoint list
 *
 * @MT SPI_CMD_ZDO_MATCH_DESC_REQ
 * (UInt16 DstAddr,
 *  UInt16 NWKAddrOfInterest,
 *  UInt16 ProfileID,
 *  byte NumInClusters,
 *  uint16 InClusterList[15],
 *  byte NumOutClusters,
 *  uint16 OutClusterList[15],
 *  byte SecuritySuite)
 *
 */
extern afStatus_t ZDP_MatchDescReq( zAddrType_t *dstAddr, uint16 nwkAddr,
                                uint16 ProfileID,
                                byte NumInClusters, uint16 *InClusterList,
                                byte NumOutClusters, uint16 *OutClusterList,
                                byte SecurityEnable );

/*
 * ZDP_SimpleDescReq - Request Simple Descriptor
 *
 * @MT SPI_CMD_ZDO_SIMPLE_DESC_REQ
 * (UInt16 DstAddr,
 *  UInt16 NWKAddrOfInterest,
 *  byte Endpoint,
 *  byte Security)
 *
 */
extern afStatus_t ZDP_SimpleDescReq( zAddrType_t *dstAddr, uint16 nwkAddr,
                                    byte ep, byte SecurityEnable );

/*
 * ZDP_UserDescSet - Set the User Descriptor
 *
 * @MT SPI_CMD_ZDO_USER_DESC_SET
 * (UInt16 DstAddr,
 *  UInt16 NWKAddrOfInterest,
 *  byte DescLen,
 *  byte Descriptor[15],
 *  byte SecuritySuite)
 *
 */
extern afStatus_t ZDP_UserDescSet( zAddrType_t *dstAddr, uint16 nwkAddr,
                          UserDescriptorFormat_t *UserDescriptor,
                          byte SecurityEnable );

#if defined ( ZDO_SERVERDISC_REQUEST )
/*
 * ZDP_ServerDiscReq - Build and send a Server_Discovery_req request message.
 */
afStatus_t ZDP_ServerDiscReq( uint16 serverMask, byte SecurityEnable );
#endif

/*
 * ZDP_EndDeviceAnnce - End Device Announce
 *
 * @MT SPI_CMD_ZDO_END_DEV_ANNCE
 * (UInt16 DevAddr,
 *  byte DeviceAddress,
 *  byte SecuritySuite)
 *
 */
extern afStatus_t ZDP_EndDeviceAnnce( uint16 nwkAddr, byte *IEEEAddr,
                         byte capabilities, byte SecurityEnable );

/*
 * ZDP_EndDeviceBindReq - End Device (hand) bind request
 *
 * @MT SPI_CMD_ZDO_END_DEV_BIND_REQ
 * (UInt16 DstAddr,
 *  UInt16 LocalCoordinator,
 *  byte Endpoint,
 *  UInt16 Profile,
 *  byte NumInClusters,
 *  uint16 InClusterList[15],
 *  byte NumOutClusters,
 *  uint16 OutClusterList[15],
 *  byte SecuritySuite)
 *
 */
extern afStatus_t ZDP_EndDeviceBindReq( zAddrType_t *dstAddr,
                              uint16 LocalCoordinator,
                              byte ep,
                              uint16 ProfileID,
                              byte NumInClusters, uint16 *InClusterList,
                              byte NumOutClusters, uint16 *OutClusterList,
                              byte SecurityEnable );

/*
 * ZDP_BindUnbindReq - bind request
 */
extern afStatus_t ZDP_BindUnbindReq( uint16 BindOrUnbind, zAddrType_t *dstAddr,
                            byte *SourceAddr, byte SrcEP,
                            cId_t  ClusterID,
                            zAddrType_t *DestinationAddr, byte DstEP,
                            byte SecurityEnable );

/*
 * ZDP_MgmtNwkDiscReq - Send a Management Network Discovery Request
 *
 * @MT SPI_CMD_ZDO_MGMT_NWKDISC_REQ
 * (UInt16 DstAddr,
 *  UInt32 ScanChannels,
 *  byte StartIndex)
 *
 */
extern afStatus_t ZDP_MgmtNwkDiscReq( zAddrType_t *dstAddr,
                            uint32 ScanChannels,
                            byte ScanDuration,
                            byte StartIndex,
                            byte SecurityEnable );

/*
 * ZDP_MgmtDirectJoinReq - Send a Management Direct Join Request
 *
 * @MT SPI_CMD_ZDO_MGMT_DIRECT_JOIN_REQ
 * (UInt16 DstAddr,
 *  UInt64 DeviceAddress,
 *  byte CapInfo)
 *
 */
extern afStatus_t ZDP_MgmtDirectJoinReq( zAddrType_t *dstAddr,
                               byte *deviceAddr,
                               byte capInfo,
                               byte SecurityEnable );

/*
 * ZDP_MgmtPermitJoinReq - Send a Management Permit Join Request
 *
 * @MT SPI_CMD_ZDO_MGMT_PERMIT_JOIN_REQ
 * (UInt16 DstAddr,
 *  byte duration,
 *  byte TcSignificance)
 *
 */
extern afStatus_t ZDP_MgmtPermitJoinReq( zAddrType_t *dstAddr,
                               byte duration,
                               byte TcSignificance,
                               byte SecurityEnable );

/*********************************************************************
 * @fn      ZDP_AddrRsp
 *
 * @brief   Build and send a NWK_addr_rsp or IEEE_addr_rsp message.
 *
 * @param   cId - ClusterID of the rsp, either NWK_addr_rsp or IEEE_addr_rsp.
 * @param   seq - Message sequence number of the corresponding request.
 * @param   dst - Destination address for the response.
 * @param   stat - Response status: ZDP_SUCCESS or other value from ZDProfile.h
 * @param   ieee - 64-bit IEEE address of the response.
 * @param   reqType - Type of response requested (single, extended, etc.)
 * @param   nwkAddr - 16-bit network address of the response.
 * @param   devCnt  - Number of associated devices in the device address list.
 * @param   strtIdx - Starting index into the dev addr array if extended rsp.
 * @param   devAddr - Array of 16-bit network addresses of the associated devs.
 * @param   secOpt  - Security Enable Options.
 *
 * @return  afStatus_t
 */
afStatus_t ZDP_AddrRsp( byte cId, byte seq, zAddrType_t *dst, byte stat,
  byte *ieee, byte reqType, uint16 nwkAddr, byte devCnt, byte strtIdx,
  uint16 *devAddr, byte secOpt );

/*
 * ZDP_NodeDescMsg - Send a Node Descriptor message.
 */
extern afStatus_t ZDP_NodeDescMsg( zdoIncomingMsg_t *inMsg,
                    uint16 nwkAddr, NodeDescriptorFormat_t *pNodeDesc );

/*
 * ZDP_PowerDescMsg - Send a Node Power Descriptor message.
 */
extern afStatus_t ZDP_PowerDescMsg( zdoIncomingMsg_t *inMsg,
 uint16 nwkAddr, NodePowerDescriptorFormat_t *pPowerDesc );

/*
 * ZDP_SimpleDescMsg - Send a Simple Descriptor message.
 */
extern afStatus_t ZDP_SimpleDescMsg( zdoIncomingMsg_t *inMsg,
                     byte Status, SimpleDescriptionFormat_t *pSimpleDesc );

/*
 * ZDP_EPRsp - Send a list of endpoint
 */
extern afStatus_t ZDP_EPRsp( uint16 MsgType, byte TransSeq, zAddrType_t *dstAddr, byte Status,
                                uint16 nwkAddr, byte Count, byte *pEPList,
                                byte SecurityEnable );

/*
 * ZDP_GenericRsp - Sends a response message with only the parameter response
 *                                     byte and the addr of interest for data.
 */
extern afStatus_t ZDP_GenericRsp( byte TransSeq, zAddrType_t *dstAddr,
                    byte status, uint16 aoi, uint16 rspID, byte SecurityEnable );

/*
 * ZDP_MgmtNwkDiscRsp - Sends the Management Network Discovery Response.
 */
extern afStatus_t ZDP_MgmtNwkDiscRsp( byte TransSeq, zAddrType_t *dstAddr,
                            byte Status,
                            byte NetworkCount,
                            byte StartIndex,
                            byte NetworkCountList,
                            networkDesc_t *NetworkList,
                            byte SecurityEnable );

/*
 * ZDP_MgmtLqiRsp - Sends the Management LQI Response.
 */
extern ZStatus_t ZDP_MgmtLqiRsp( byte TransSeq, zAddrType_t *dstAddr,
                          byte Status,
                          byte NeighborLqiEntries,
                          byte StartIndex,
                          byte NeighborLqiCount,
                          ZDP_MgmtLqiItem_t* NeighborList,
                          byte SecurityEnable );

/*
 * ZDP_MgmtRtgRsp - Sends the Management Routing Response.
 */
extern ZStatus_t ZDP_MgmtRtgRsp( byte TransSeq, zAddrType_t *dstAddr,
                            byte Status,
                            byte RoutingTableEntries,
                            byte StartIndex,
                            byte RoutingListCount,
                            rtgItem_t *RoutingTableList,
                            byte SecurityEnable );

/*
 * ZDP_MgmtBindRsp - Sends the Management Binding Response.
 */
extern ZStatus_t ZDP_MgmtBindRsp( byte TransSeq, zAddrType_t *dstAddr,
                            byte Status,
                            byte BindingTableEntries,
                            byte StartIndex,
                            byte BindingTableListCount,
                            apsBindingItem_t *BindingTableList,
                            byte SecurityEnable );

/*
 * ZDP_UserDescRsp - Sends the user descriptor response message.
 */
extern ZStatus_t ZDP_UserDescRsp( byte TransSeq, zAddrType_t *dstAddr,
                uint16 nwkAddrOfInterest, UserDescriptorFormat_t *userDesc,
                byte SecurityEnable );

/*
 * ZDP_ServerDiscRsp - Build and send the User Decriptor Response.
 */
#if defined ( ZDO_SERVERDISC_RESPONSE )
ZStatus_t ZDP_ServerDiscRsp( byte transID, zAddrType_t *dstAddr, byte status,
                           uint16 aoi, uint16 serverMask, byte SecurityEnable );
#endif

/*
 * ZDP_IncomingData - Incoming data callback from AF layer
 */
extern void ZDP_IncomingData( afIncomingMSGPacket_t *pData );

extern ZStatus_t ZDO_RegisterForZDOMsg( uint8 taskID, uint16 clusterID );
extern ZStatus_t ZDO_RemoveRegisteredCB( uint8 taskID, uint16 clusterID );


/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* ZDPROFILE_H */
