/**************************************************************************************************
  Filename:       ZDProfile.c
  Revised:        $Date: 2007-10-28 18:43:04 -0700 (Sun, 28 Oct 2007) $
  Revision:       $Revision: 15800 $

  Description:    This is the Zigbee Device Profile.


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

/*********************************************************************
 * INCLUDES
 */
#include "ZComdef.h"
#include "OSAL.h"
#include "AF.h"
#include "NLMEDE.h"
#include "nwk_util.h"
#include "APS.h"

#include "AddrMgr.h"
#include "ZDConfig.h"
#include "ZDProfile.h"
#include "ZDObject.h"

#if defined( LCD_SUPPORTED )
  #include "OnBoard.h"
#endif

#include "nwk_util.h"

#if defined( MT_ZDO_FUNC )
  #include "MT_ZDO.h"
#endif

/*********************************************************************
 * MACROS
 */

#define ZADDR_TO_AFADDR( pZADDR, AFADDR ) {                            \
  (AFADDR).endPoint = ZDP_AF_ENDPOINT;                                 \
  (AFADDR).addrMode = (afAddrMode_t)(pZADDR)->addrMode;                \
  (AFADDR).addr.shortAddr = (pZADDR)->addr.shortAddr;                  \
}

#define FillAndSendBuffer( TRANSSEQ, ADDR, ID, LEN, BUF ) {     \
  afStatus_t stat;                                    \
  ZDP_TmpBuf = (BUF)+1;                               \
  stat = fillAndSend( (TRANSSEQ), (ADDR), (ID), (LEN) );          \
  osal_mem_free( (BUF) );                             \
  ZDP_TmpBuf = ZDP_Buf+1;                             \
  return stat;                                        \
}

#define FillAndSendTxOptions( TRANSSEQ, ADDR, ID, LEN, TxO ) {  \
  afStatus_t stat;                                    \
  ZDP_TxOptions = (TxO);                              \
  stat = fillAndSend( (TRANSSEQ), (ADDR), (ID), (LEN) );          \
  ZDP_TxOptions = AF_TX_OPTIONS_NONE;                 \
  return stat;                                        \
}

/*********************************************************************
 * CONSTANTS
 */

#define ZDP_BUF_SZ          80

CONST byte ZDP_AF_ENDPOINT = 0;

/*********************************************************************
 * TYPEDEFS
 */
typedef struct
{
  void *next;
  uint8 taskID;
  uint16 clusterID;
} ZDO_MsgCB_t;


/*********************************************************************
 * GLOBAL VARIABLES
 */

byte ZDP_TransID = 0;

/*********************************************************************
 * EXTERNAL VARIABLES
 */

extern endPointDesc_t ZDApp_epDesc;

/*********************************************************************
 * EXTERNAL FUNCTIONS
 */

/*********************************************************************
 * LOCAL FUNCTIONS
 */

static afStatus_t fillAndSend( uint8 *transSeq, zAddrType_t *addr, cId_t clusterID, byte len );
uint8 ZDO_SendMsgCBs( zdoIncomingMsg_t *inMsg );
void zdpProcessAddrReq( zdoIncomingMsg_t *inMsg );

/*********************************************************************
 * LOCAL VARIABLES
 */

static uint8  ZDP_Buf[ ZDP_BUF_SZ ];
static uint8 *ZDP_TmpBuf = ZDP_Buf+1;
static byte ZDP_TxOptions = AF_TX_OPTIONS_NONE;

ZDO_MsgCB_t *zdoMsgCBs = (ZDO_MsgCB_t *)NULL;

/*********************************************************************
 * ZDO Message Processing table
 */

typedef void (*pfnZDPMsgProcessor)( zdoIncomingMsg_t *inMsg );

typedef struct
{
  uint16                clusterID;
  pfnZDPMsgProcessor    pFn;
} zdpMsgProcItem_t;

CONST zdpMsgProcItem_t zdpMsgProcs[] = 
{
  { NWK_addr_req,           zdpProcessAddrReq },
  { IEEE_addr_req,          zdpProcessAddrReq },
  { Node_Desc_req,          ZDO_ProcessNodeDescReq },
  { Power_Desc_req,         ZDO_ProcessPowerDescReq },
  { Simple_Desc_req,        ZDO_ProcessSimpleDescReq },
  { Active_EP_req,          ZDO_ProcessActiveEPReq },
  { Match_Desc_req,         ZDO_ProcessMatchDescReq },
#if defined ( ZDO_MGMT_NWKDISC_RESPONSE )
  { Mgmt_NWK_Disc_req,      ZDO_ProcessMgmtNwkDiscReq },
#endif
#if defined ( ZDO_MGMT_LQI_RESPONSE ) && defined ( RTR_NWK )
  { Mgmt_Lqi_req,           ZDO_ProcessMgmtLqiReq },
#endif
#if defined ( ZDO_MGMT_RTG_RESPONSE ) && defined ( RTR_NWK )
  { Mgmt_Rtg_req,           ZDO_ProcessMgmtRtgReq },
#endif
#if defined ( ZDO_MGMT_BIND_RESPONSE ) && defined ( REFLECTOR )
  { Mgmt_Bind_req,          ZDO_ProcessMgmtBindReq },
#endif
#if defined ( ZDO_MGMT_JOINDIRECT_RESPONSE ) && defined ( RTR_NWK )
  { Mgmt_Direct_Join_req,   ZDO_ProcessMgmtDirectJoinReq },
#endif
#if defined ( ZDO_MGMT_LEAVE_RESPONSE )
  { Mgmt_Leave_req,         ZDO_ProcessMgmtLeaveReq },
#endif
#if defined ( ZDO_MGMT_PERMIT_JOIN_RESPONSE )  && defined ( RTR_NWK )
  { Mgmt_Permit_Join_req,   ZDO_ProcessMgmtPermitJoinReq },
#endif
#if defined ( ZDO_USERDESC_RESPONSE )
  { User_Desc_req,          ZDO_ProcessUserDescReq },
#endif
#if defined ( ZDO_USERDESCSET_RESPONSE )
  { User_Desc_set,          ZDO_ProcessUserDescSet },
#endif
#if defined ( ZDO_SERVERDISC_RESPONSE )
  { Server_Discovery_req,   ZDO_ProcessServerDiscReq },
#endif
#if defined ( ZDO_ENDDEVICE_ANNCE ) && defined ( RTR_NWK )
  { End_Device_annce,       ZDO_ProcessEndDeviceAnnce },
#endif
  {0xFFFF, NULL} // Last
};

/*********************************************************************
 * @fn          fillAndSend
 *
 * @brief       Combined to reduce space
 *
 * @param
 * @param
 *
 * @return      afStatus_t
 */
static afStatus_t fillAndSend( uint8 *transSeq, zAddrType_t *addr, cId_t clusterID, byte len )
{
  afAddrType_t afAddr;

  ZADDR_TO_AFADDR( addr, afAddr );

  *(ZDP_TmpBuf-1) = *transSeq;

  return AF_DataRequest( &afAddr, &ZDApp_epDesc, clusterID,
                           (uint16)(len+1), (uint8*)(ZDP_TmpBuf-1),
                           transSeq, ZDP_TxOptions,  AF_DEFAULT_RADIUS );

}

/*********************************************************************
 * @fn          ZDP_SendData
 *
 * @brief       This builds and send a request message that has
 *              NWKAddrOfInterest as its only parameter.
 *
 * @param       dstAddr - destination address
 * @param       cmd - clusterID
 * @param       dataLen - number of bytes of data
 * @param       data - pointer to the data
 * @param       SecurityEnable - Security Options
 *
 * @return      afStatus_t
 */
afStatus_t ZDP_SendData( byte *TransSeq, zAddrType_t *dstAddr, uint16 cmd,
                        byte len, byte *buf, byte SecurityEnable )
{
  byte *pBuf = ZDP_TmpBuf;
  byte cnt = len;

  while ( cnt-- )
  {
    *pBuf++ = *buf++;
  }

  FillAndSendTxOptions( TransSeq, dstAddr, cmd, len,
               (AF_DEFAULT_RADIUS | ((SecurityEnable) ? AF_EN_SECURITY : 0)) );
}

#if defined ( ZDO_NODEDESC_REQUEST ) || defined ( ZDO_POWERDESC_REQUEST ) ||\
    defined ( ZDO_ACTIVEEP_REQUEST ) || defined ( ZDO_COMPLEXDESC_REQUEST ) ||\
    defined ( ZDO_USERDESC_REQUEST )
/*********************************************************************
 * @fn          ZDP_NWKAddrOfInterestReq
 *
 * @brief       This builds and send a request message that has
 *              NWKAddrOfInterest as its only parameter.
 *
 * @param       dstAddr - destination address
 * @param       nwkAddr - 16 bit address
 * @param       SecurityEnable - Security Options
 *
 * @return      afStatus_t
 */
afStatus_t ZDP_NWKAddrOfInterestReq( zAddrType_t *dstAddr, uint16 nwkAddr,
                                     byte cmd, byte SecurityEnable )
{
  ZDP_TmpBuf[0] = LO_UINT16( nwkAddr );
  ZDP_TmpBuf[1] = HI_UINT16( nwkAddr );

  return fillAndSend( &ZDP_TransID, dstAddr, cmd, 2 );
}
#endif // ZDO_NODEDESC_REQUEST ZDO_POWERDESC_REQUEST ZDO_ACTIVEEP_REQUEST ZDO_COMPLEXDESC_REQUEST  ZDO_USERDESC_REQUEST

/*********************************************************************
 * Address Requests
 */

#if defined ( ZDO_NWKADDR_REQUEST )
/*********************************************************************
 * @fn          ZDP_NwkAddrReq
 *
 * @brief       This builds and send a NWK_addr_req message.  This
 *              function sends a broadcast message looking for a 16
 *              bit address with a 64 bit address as bait.
 *
 * @param       IEEEAddress - looking for this device
 * @param       SecurityEnable - Security Options
 *
 * @return      afStatus_t
 */
afStatus_t ZDP_NwkAddrReq( byte *IEEEAddress, byte ReqType,
                           byte StartIndex, byte SecurityEnable )
{
  byte *pBuf = ZDP_TmpBuf;
  byte len = Z_EXTADDR_LEN + 1 + 1;  // IEEEAddress + ReqType + StartIndex.
  zAddrType_t dstAddr;

  if ( osal_ExtAddrEqual( saveExtAddr, IEEEAddress ) == FALSE )
  {
    dstAddr.addrMode = AddrBroadcast;
    dstAddr.addr.shortAddr = NWK_BROADCAST_SHORTADDR;
  }
  else
  {
    dstAddr.addrMode = Addr16Bit;
    dstAddr.addr.shortAddr = ZDAppNwkAddr.addr.shortAddr;
  }

  pBuf = osal_cpyExtAddr( pBuf, IEEEAddress );

  *pBuf++ = ReqType;
  *pBuf++ = StartIndex;

  return fillAndSend( &ZDP_TransID, &dstAddr, NWK_addr_req, len );
}
#endif

#if defined ( ZDO_IEEEADDR_REQUEST )
/*********************************************************************
 * @fn          ZDP_IEEEAddrReq
 *
 * @brief       This builds and send a IEEE_addr_req message.  This
 *              function sends a unicast message looking for a 64
 *              bit IEEE address with a 16 bit address as bait.
 *
 * @param       ReqType - ZDP_IEEEADDR_REQTYPE_SINGLE or
 *                        ZDP_IEEEADDR_REQTYPE_EXTENDED
 * @param       SecurityEnable - Security Options
 *
 * @return      afStatus_t
 */
afStatus_t ZDP_IEEEAddrReq( uint16 shortAddr, byte ReqType,
                            byte StartIndex, byte SecurityEnable )
{
  byte *pBuf = ZDP_TmpBuf;
  byte len = 2 + 1 + 1;  // shortAddr + ReqType + StartIndex.
  zAddrType_t dstAddr;

  dstAddr.addrMode = (afAddrMode_t)Addr16Bit;
  dstAddr.addr.shortAddr = shortAddr;

  *pBuf++ = LO_UINT16( shortAddr );
  *pBuf++ = HI_UINT16( shortAddr );

  *pBuf++ = ReqType;
  *pBuf++ = StartIndex;

  return fillAndSend( &ZDP_TransID, &dstAddr, IEEE_addr_req, len );
}
#endif

#if defined ( ZDO_MATCH_REQUEST )
/*********************************************************************
 * @fn          ZDP_MatchDescReq
 *
 * @brief       This builds and send a Match_Desc_req message.  This
 *              function sends a broadcast or unicast message
 *              requesting the list of endpoint/interfaces that
 *              match profile ID and cluster IDs.
 *
 * @param       dstAddr - destination address
 * @param       ProfileID - Profile ID
 * @param       NumInClusters - number of input clusters
 * @param       InClusterList - input cluster ID list
 * @param       NumOutClusters - number of output clusters
 * @param       OutClusterList - output cluster ID list
 * @param       SecurityEnable - Security Options
 *
 * @return      afStatus_t
 */
afStatus_t ZDP_MatchDescReq( zAddrType_t *dstAddr, uint16 nwkAddr,
                                uint16 ProfileID,
                                byte NumInClusters, cId_t *InClusterList,
                                byte NumOutClusters, cId_t *OutClusterList,
                                byte SecurityEnable )
{
  byte *pBuf = ZDP_TmpBuf;
  // nwkAddr+ProfileID+NumInClusters+NumOutClusters.
  byte i, len = 2 + 2 + 1 + 1;  // nwkAddr+ProfileID+NumInClusters+NumOutClusters.
  
  len += (NumInClusters + NumOutClusters) * sizeof(uint16);

  if ( len >= ZDP_BUF_SZ-1 )
  {
    return afStatus_MEM_FAIL;
  }

  *pBuf++ = LO_UINT16( nwkAddr );   // NWKAddrOfInterest
  *pBuf++ = HI_UINT16( nwkAddr );

  *pBuf++ = LO_UINT16( ProfileID );   // Profile ID
  *pBuf++ = HI_UINT16( ProfileID );

  *pBuf++ = NumInClusters; // Input cluster list
  if ( NumInClusters )
  {
    for (i=0; i<NumInClusters; ++i)  {
      *pBuf++ = LO_UINT16( InClusterList[i] );
      *pBuf++ = HI_UINT16( InClusterList[i] );
    }
  }

  *pBuf++ = NumOutClusters; // Output cluster list
  if ( NumOutClusters )
  {
    for (i=0; i<NumOutClusters; ++i)  {
      *pBuf++ = LO_UINT16( OutClusterList[i] );
      *pBuf++ = HI_UINT16( OutClusterList[i] );
    }
  }

  return fillAndSend( &ZDP_TransID, dstAddr, Match_Desc_req, len );
}
#endif // ZDO_MATCH_REQUEST

#if defined ( ZDO_SIMPLEDESC_REQUEST )
/*********************************************************************
 * @fn          ZDP_SimpleDescReq
 *
 * @brief       This builds and send a NWK_Simple_Desc_req
 *              message.  This function sends unicast message to the
 *              destination device.
 *
 * @param       dstAddr - destination address
 * @param       nwkAddr - 16 bit address
 * @param       epIntf - endpoint/interface
 * @param       SecurityEnable - Security Options
 *
 * @return      afStatus_t
 */
afStatus_t ZDP_SimpleDescReq( zAddrType_t *dstAddr, uint16 nwkAddr,
                                    byte endPoint, byte SecurityEnable )

{
  ZDP_TmpBuf[0] = LO_UINT16( nwkAddr );
  ZDP_TmpBuf[1] = HI_UINT16( nwkAddr );
  ZDP_TmpBuf[2] = endPoint;

  return fillAndSend( &ZDP_TransID, dstAddr, Simple_Desc_req, 3 );
}
#endif // ZDO_SIMPLEDESC_REQUEST

#if defined ( ZDO_USERDESCSET_REQUEST )
/*********************************************************************
 * @fn          ZDP_UserDescSet
 *
 * @brief       This builds and send a User_Desc_set message to set
 *              the user descriptor.  This function sends unicast
 *              message to the destination device.
 *
 * @param       dstAddr - destination address
 * @param       nwkAddr - 16 bit address
 * @param       UserDescriptor - user descriptor
 * @param       SecurityEnable - Security Options
 *
 * @return      afStatus_t
 */
afStatus_t ZDP_UserDescSet( zAddrType_t *dstAddr, uint16 nwkAddr,
                          UserDescriptorFormat_t *UserDescriptor,
                          byte SecurityEnable )
{
  byte *pBuf = ZDP_TmpBuf;
  byte len = (UserDescriptor->len < AF_MAX_USER_DESCRIPTOR_LEN) ?
              UserDescriptor->len : AF_MAX_USER_DESCRIPTOR_LEN;
  byte addrLen = 2;

  *pBuf++ = LO_UINT16( nwkAddr );
  *pBuf++ = HI_UINT16( nwkAddr );

  *pBuf++ = len;
  addrLen = 3;

  pBuf = osal_memcpy( pBuf, UserDescriptor->desc, len );
  osal_memset( pBuf, AF_USER_DESCRIPTOR_FILL, AF_MAX_USER_DESCRIPTOR_LEN-len );

  return fillAndSend( &ZDP_TransID, dstAddr, User_Desc_set, (AF_MAX_USER_DESCRIPTOR_LEN + addrLen) );
}
#endif // ZDO_USERDESCSET_REQUEST

#if defined ( ZDO_SERVERDISC_REQUEST )
/*********************************************************************
 * @fn          ZDP_ServerDiscReq
 *
 * @brief       Build and send a Server_Discovery_req request message.
 *
 * @param       serverMask - 16-bit bit-mask of server services being sought.
 * @param       SecurityEnable - Security Options
 *
 * @return      afStatus_t
 */
afStatus_t ZDP_ServerDiscReq( uint16 serverMask, byte SecurityEnable )
{
  byte *pBuf = ZDP_TmpBuf;
  zAddrType_t dstAddr;

  dstAddr.addrMode = AddrBroadcast;
  dstAddr.addr.shortAddr = NWK_BROADCAST_SHORTADDR_DEVZCZR;

  *pBuf++ = LO_UINT16( serverMask );
  *pBuf = HI_UINT16( serverMask );

  FillAndSendTxOptions( &ZDP_TransID, &dstAddr, Server_Discovery_req, 2,
             ((SecurityEnable) ? AF_EN_SECURITY : AF_TX_OPTIONS_NONE) );
}
#endif

#if defined ( ZDO_ENDDEVICE_ANNCE_REQUEST ) || defined ( ZDO_ENDDEVICE_ANNCE_GENERATE )
/*********************************************************************
 * @fn          ZDP_EndDeviceAnnce
 *
 * @brief       This builds and send an End_Device_Annce
 *              message.  This function sends a broadcast message.
 *
 * @param       nwkAddr - 16 bit address of the device
 * @param       IEEEAddr - 64 bit address of the device
 * @param       capabilities - device capabilities.  This field is only
 *                 sent for v1.1 networks.
 * @param       SecurityEnable - Security Options
 *
 * @return      afStatus_t
 */
afStatus_t ZDP_EndDeviceAnnce( uint16 nwkAddr, byte *IEEEAddr,
                              byte capabilities, byte SecurityEnable )
{
  zAddrType_t dstAddr;
  uint8 len;

  dstAddr.addrMode = (afAddrMode_t)AddrBroadcast;
  dstAddr.addr.shortAddr = NWK_BROADCAST_SHORTADDR;

  ZDP_TmpBuf[0] = LO_UINT16( nwkAddr );
  ZDP_TmpBuf[1] = HI_UINT16( nwkAddr );
  osal_cpyExtAddr( &ZDP_TmpBuf[2], IEEEAddr );
  len = 2 + Z_EXTADDR_LEN;

  ZDP_TmpBuf[10] = capabilities;
  len++;

  return fillAndSend( &ZDP_TransID, &dstAddr, End_Device_annce, len );
}
#endif // ZDO_ENDDEVICE_ANNCE_REQUEST

/*********************************************************************
 * Address Responses
 */

/*********************************************************************
 * @fn      zdpProcessAddrReq
 *
 * @brief   Process an incoming NWK_addr_req or IEEE_addr_req message and then
 *          build and send a corresponding NWK_addr_rsp or IEEE_addr_rsp msg.
 *
 * @param   inMsg - incoming message
 *
 * @return  none
 */
void zdpProcessAddrReq( zdoIncomingMsg_t *inMsg )
{
  uint8 reqType;
  uint16 aoi = INVALID_NODE_ADDR;
  uint8 *ieee = NULL;

  reqType = inMsg->asdu[(inMsg->clusterID == NWK_addr_req) ? Z_EXTADDR_LEN : sizeof( uint16 ) ];
  
  if ( inMsg->clusterID == NWK_addr_req )
  {
    ieee = inMsg->asdu;

    if ( osal_ExtAddrEqual( saveExtAddr, ieee ) )
    {
      aoi = ZDAppNwkAddr.addr.shortAddr;
    }
    /*
    else if ( reqType == ZDP_ADDR_REQTYPE_MEMBERSHIP )
    {
      //ggg - what to do?
    }
    */
  }
  else  // if ( inMsg->clusterID == IEEE_addr_req )
  {
    aoi = BUILD_UINT16( inMsg->asdu[0], inMsg->asdu[1] );

    if ( aoi == ZDAppNwkAddr.addr.shortAddr )
    {
      ieee = saveExtAddr;
    }
  }

  if ( (aoi != INVALID_NODE_ADDR) && (ieee != NULL) )
  {
    byte *pBuf = ZDP_TmpBuf;
    // Status + IEEE-Addr + Nwk-Addr.
    byte len = 1 + Z_EXTADDR_LEN + 2;

    byte stat = ((reqType == ZDP_ADDR_REQTYPE_SINGLE) 
            || (reqType == ZDP_ADDR_REQTYPE_EXTENDED) 
          || ((reqType == ZDP_ADDR_REQTYPE_MEMBERSHIP) 
         && (inMsg->clusterID == NWK_addr_req)) ) ? ZDP_SUCCESS : ZDP_INVALID_REQTYPE;

    *pBuf++ = stat;

    pBuf = osal_cpyExtAddr( pBuf, ieee );

    *pBuf++ = LO_UINT16( aoi );
    *pBuf++ = HI_UINT16( aoi );

#if defined( RTR_NWK )
    if ( (reqType == ZDP_ADDR_REQTYPE_EXTENDED) &&
         (aoi == ZDAppNwkAddr.addr.shortAddr) )
    {
      byte cnt = 0;
      uint16 *list = AssocMakeList( &cnt );

      if ( list != NULL )
      {
        byte idx = inMsg->asdu[(((inMsg->clusterID == NWK_addr_req) ? Z_EXTADDR_LEN : sizeof( uint16 )) + 1)];
        uint16 *pList = list + idx;

        // NumAssocDev field is only present on success.
        if ( cnt > idx )
        {
          cnt -= idx;
          len += (cnt * sizeof( uint16 ));
        }
        else
        {
          cnt = 0;
        }
        *pBuf++ = cnt;
        len++;
        
        // StartIndex field is only present if NumAssocDev field is non-zero.
        *pBuf++ = idx;
        len++;

        while ( cnt != 0 )
        {
          *pBuf++ = LO_UINT16( *pList );
          *pBuf++ = HI_UINT16( *pList );
          pList++;
          cnt--;
        }

        osal_mem_free( (uint8 *)list );
      }
      else
      {
        // NumAssocDev field is only present on success.
        *pBuf++ = 0;
        len++;
      }
    }

#endif

    ZDP_TxOptions = AF_MSG_ACK_REQUEST;
    fillAndSend( &(inMsg->TransSeq), &(inMsg->srcAddr), 
                (cId_t)(inMsg->clusterID | ZDO_RESPONSE_BIT), len );
    ZDP_TxOptions = AF_TX_OPTIONS_NONE;
  }
}

/*********************************************************************
 * @fn          ZDP_NodeDescMsg
 *
 * @brief       Builds and sends a Node Descriptor message, unicast to the
 *              specified device.
 *
 * @param       inMsg - incoming message
 * @param       nwkAddr - 16 bit network address for device
 * @param       pNodeDesc - pointer to the node descriptor
 *
 * @return      afStatus_t
 */
afStatus_t ZDP_NodeDescMsg( zdoIncomingMsg_t *inMsg, 
                           uint16 nwkAddr, NodeDescriptorFormat_t *pNodeDesc )
{
  byte *pBuf = ZDP_TmpBuf;
  byte len;

  len = 1 + 2 + 10;  // Status + nwkAddr + Node descriptor

  *pBuf++ = ZDP_SUCCESS;

  *pBuf++ = LO_UINT16( nwkAddr );
  *pBuf++ = HI_UINT16( nwkAddr );

  *pBuf++ = (byte)((pNodeDesc->ComplexDescAvail << 3) |
                     (pNodeDesc->UserDescAvail << 4) |
                     (pNodeDesc->LogicalType & 0x07));

  *pBuf++ = (byte)((pNodeDesc->FrequencyBand << 3) | (pNodeDesc->APSFlags & 0x07));
  *pBuf++ = pNodeDesc->CapabilityFlags;
  *pBuf++ = pNodeDesc->ManufacturerCode[0];
  *pBuf++ = pNodeDesc->ManufacturerCode[1];
  *pBuf++ = pNodeDesc->MaxBufferSize;
  *pBuf++ = pNodeDesc->MaxTransferSize[0];
  *pBuf++ = pNodeDesc->MaxTransferSize[1];

  *pBuf++ = LO_UINT16( pNodeDesc->ServerMask );
  *pBuf++ = HI_UINT16( pNodeDesc->ServerMask );

  return fillAndSend( &(inMsg->TransSeq), &(inMsg->srcAddr), Node_Desc_rsp, len );
}

/*********************************************************************
 * @fn          ZDP_PowerDescMsg
 *
 * @brief       Builds and sends a Power Descriptor message, unicast to the
 *              specified device.
 *
 * @param       inMsg - incoming message (request)
 * @param       nwkAddr - 16 bit network address for device
 * @param       pPowerDesc - pointer to the node descriptor
 *
 * @return      afStatus_t
 */
afStatus_t ZDP_PowerDescMsg( zdoIncomingMsg_t *inMsg,
                     uint16 nwkAddr, NodePowerDescriptorFormat_t *pPowerDesc )
{
  byte *pBuf = ZDP_TmpBuf;
  byte len = 1 + 2 + 2;  // Status + nwkAddr + Node Power descriptor.

  *pBuf++ = ZDP_SUCCESS;

  *pBuf++ = LO_UINT16( nwkAddr );
  *pBuf++ = HI_UINT16( nwkAddr );

  *pBuf++ = (byte)((pPowerDesc->AvailablePowerSources << 4)
                    | (pPowerDesc->PowerMode & 0x0F));
  *pBuf++ = (byte)((pPowerDesc->CurrentPowerSourceLevel << 4)
                    | (pPowerDesc->CurrentPowerSource & 0x0F));

  return fillAndSend( &(inMsg->TransSeq), &(inMsg->srcAddr), Power_Desc_rsp, len );
}

/*********************************************************************
 * @fn          ZDP_SimpleDescMsg
 *
 * @brief       Builds and sends a Simple Descriptor message, unicast to the
 *              specified device.
 *
 * @param       inMsg - incoming message (request)
 * @param       Status - message status (ZDP_SUCCESS or other)
 * @param       pSimpleDesc - pointer to the node descriptor
 *
 * @return      afStatus_t
 */
afStatus_t ZDP_SimpleDescMsg( zdoIncomingMsg_t *inMsg, byte Status,
                              SimpleDescriptionFormat_t *pSimpleDesc )
{
  uint8 *pBuf = ZDP_TmpBuf;
  uint8 i, len;

  if ( Status == ZDP_SUCCESS && pSimpleDesc )
  {
    // Status + NWKAddrOfInterest + desc length + empty simple descriptor.
    len = 1 + 2 + 1 + 8;
    len += (pSimpleDesc->AppNumInClusters + pSimpleDesc->AppNumOutClusters) * sizeof ( uint16 );
  }
  else
  {
    len = 2; // Status + desc length
  }
  if ( len >= ZDP_BUF_SZ-1 )
  {
    return afStatus_MEM_FAIL;
  }

  *pBuf++ = Status;

  *pBuf++ = LO_UINT16( ZDAppNwkAddr.addr.shortAddr );
  *pBuf++ = HI_UINT16( ZDAppNwkAddr.addr.shortAddr );

  if ( len > 2 )
  {
    *pBuf++ = len - 4;   // Simple descriptor length

    *pBuf++ = pSimpleDesc->EndPoint;
    *pBuf++ = LO_UINT16( pSimpleDesc->AppProfId );
    *pBuf++ = HI_UINT16( pSimpleDesc->AppProfId );
    *pBuf++ = LO_UINT16( pSimpleDesc->AppDeviceId );
    *pBuf++ = HI_UINT16( pSimpleDesc->AppDeviceId );

    *pBuf++ = (byte)(pSimpleDesc->AppDevVer << 4);

    *pBuf++ = pSimpleDesc->AppNumInClusters;
    if ( pSimpleDesc->AppNumInClusters )
    {
      for (i=0; i<pSimpleDesc->AppNumInClusters; ++i)
      {
        *pBuf++ = LO_UINT16( pSimpleDesc->pAppInClusterList[i] );
        *pBuf++ = HI_UINT16( pSimpleDesc->pAppInClusterList[i] );
      }
    }
    
    *pBuf++ = pSimpleDesc->AppNumOutClusters;
    if ( pSimpleDesc->AppNumOutClusters )
    {
      for (i=0; i<pSimpleDesc->AppNumOutClusters; ++i)
      {
        *pBuf++ = LO_UINT16( pSimpleDesc->pAppOutClusterList[i] );
        *pBuf++ = HI_UINT16( pSimpleDesc->pAppOutClusterList[i] );
      }
    }
  }
  
  else
  {
    *pBuf = 0; // Description Length = 0;
  }

  return fillAndSend( &(inMsg->TransSeq), &(inMsg->srcAddr), Simple_Desc_rsp, len );
}

/*********************************************************************
 * @fn          ZDP_EPRsp
 *
 * @brief       This builds and send an endpoint list. Used in
 *              Active_EP_rsp and Match_Desc_Rsp
 *              message.  This function sends unicast message to the
 *              requesting device.
 *
 * @param       MsgType - either Active_EP_rsp or Match_Desc_Rsp
 * @param       dstAddr - destination address
 * @param       Status - message status (ZDP_SUCCESS or other)
 * @param       nwkAddr - Device's short address that this response describes
 * @param       Count - number of endpoint/interfaces in list
 * @param       pEPIntfList - Array of Endpoint/Interfaces
 * @param       SecurityEnable - Security Options
 *
 * @return      afStatus_t
 */
afStatus_t ZDP_EPRsp( uint16 MsgType, byte TransSeq, zAddrType_t *dstAddr,
                        byte Status, uint16 nwkAddr, byte Count,
                        byte *pEPList,
                        byte SecurityEnable )
{
  byte *pBuf = ZDP_TmpBuf;
  byte len = 1 + 2 + 1;  // Status + nwkAddr + endpoint/interface count.
  byte txOptions;

  if ( MsgType == Match_Desc_rsp )
    txOptions = AF_MSG_ACK_REQUEST;
  else
    txOptions = 0;

    *pBuf++ = Status;
  *pBuf++ = LO_UINT16( nwkAddr );
  *pBuf++ = HI_UINT16( nwkAddr );

  *pBuf++ = Count;   // Endpoint/Interface count

  if ( Count )
  {
    len += Count;
    osal_memcpy( pBuf, pEPList, Count );
  }

  FillAndSendTxOptions( &TransSeq, dstAddr, MsgType, len, txOptions );
}

#if defined ( ZDO_USERDESC_RESPONSE )
/*********************************************************************
 * @fn          ZDP_UserDescRsp
 *
 * @brief       Build and send the User Decriptor Response.
 *
 *
 * @param       dstAddr - destination address
 * @param       nwkAddrOfInterest -
 * @param       userDesc -
 * @param       SecurityEnable - Security Options
 *
 * @return      ZStatus_t
 */
ZStatus_t ZDP_UserDescRsp( byte TransSeq, zAddrType_t *dstAddr,
                uint16 nwkAddrOfInterest, UserDescriptorFormat_t *userDesc,
                byte SecurityEnable )
{
  byte *pBuf = ZDP_TmpBuf;
  byte len = 1 + 2 + 1;  // Status + nwkAddr + descriptor length.

  len += userDesc->len;

  *pBuf++ = ZSUCCESS;

  *pBuf++ = LO_UINT16( nwkAddrOfInterest );
  *pBuf++ = HI_UINT16( nwkAddrOfInterest );

  *pBuf++ = userDesc->len;
  osal_memcpy( pBuf, userDesc->desc, userDesc->len );

  return (ZStatus_t)fillAndSend( &TransSeq, dstAddr, User_Desc_rsp, len );
}
#endif // ZDO_USERDESC_RESPONSE

#if defined ( ZDO_SERVERDISC_RESPONSE )
/*********************************************************************
 * @fn          ZDP_ServerDiscRsp
 *
 * @brief       Build and send the Server_Discovery_rsp response.
 *
 * @param       transID - Transaction sequence number of request.
 * @param       dstAddr - Network Destination Address.
 * @param       status - Status of response to request.
 * @param       aoi - Network Address of Interest of request.
 * @param       serverMask - Bit map of service(s) being sought.
 * @param       SecurityEnable - Security Options
 *
 * @return      ZStatus_t
 */
ZStatus_t ZDP_ServerDiscRsp( byte transID, zAddrType_t *dstAddr, byte status,
                           uint16 aoi, uint16 serverMask, byte SecurityEnable )
{
  const byte len = 1  + 2;  // status + aoi + mask.
  byte *pBuf = ZDP_TmpBuf;

  *pBuf++ = status;

  *pBuf++ = LO_UINT16( serverMask );
  *pBuf++ = HI_UINT16( serverMask );

  return (ZStatus_t)fillAndSend( &transID, dstAddr, Server_Discovery_rsp, len );
}
#endif // ZDO_USERDESC_RESPONSE

/*********************************************************************
 * @fn          ZDP_GenericRsp
 *
 * @brief       Sends a response message with only the parameter status
 *              byte and the addr of interest for data.
 *              This function sends unicast message to the
 *              requesting device.
 *
 * @param       dstAddr - destination address
 * @param       status  - generic status for response
 * @param       aoi     - address of interest
 * @param       dstAddr - destination address
 * @param       rspId   - response cluster ID
 * @param       SecurityEnable - Security Options
 *
 * @return      afStatus_t
 */
afStatus_t ZDP_GenericRsp( byte TransSeq, zAddrType_t *dstAddr,
                     byte status, uint16 aoi, uint16 rspID, byte SecurityEnable )
{
  uint8 len;

  ZDP_TmpBuf[0] = status;
  ZDP_TmpBuf[1] = LO_UINT16( aoi );
  ZDP_TmpBuf[2] = HI_UINT16( aoi );

  // Length byte
  ZDP_TmpBuf[3] = 0;
  len = 4;
  
  return fillAndSend( &TransSeq, dstAddr, rspID, len );
}

/*********************************************************************
 * Binding
 */
#if defined ( ZDO_ENDDEVICEBIND_REQUEST )
/*********************************************************************
 * @fn          ZDP_EndDeviceBindReq
 *
 * @brief       This builds and sends a End_Device_Bind_req message.
 *              This function sends a unicast message.
 *
 * @param       dstAddr - destination address
 * @param       LocalCoordinator - short address of local coordinator
 * @param       epIntf - Endpoint/Interface of Simple Desc
 * @param       ProfileID - Profile ID
 *
 *   The Input cluster list is the opposite of what you would think.
 *   This is the output cluster list of this device
 * @param       NumInClusters - number of input clusters
 * @param       InClusterList - input cluster ID list
 *
 *   The Output cluster list is the opposite of what you would think.
 *   This is the input cluster list of this device
 * @param       NumOutClusters - number of output clusters
 * @param       OutClusterList - output cluster ID list
 *
 * @param       SecurityEnable - Security Options
 *
 * @return      afStatus_t
 */
afStatus_t ZDP_EndDeviceBindReq( zAddrType_t *dstAddr,
                                 uint16 LocalCoordinator,
                                 byte endPoint,
                                 uint16 ProfileID,
                                 byte NumInClusters, cId_t *InClusterList,
                                 byte NumOutClusters, cId_t *OutClusterList,
                                 byte SecurityEnable )
{
  uint8 *pBuf = ZDP_TmpBuf;
  uint8 i, len;
  uint8 *ieeeAddr;

  // LocalCoordinator + SrcExtAddr + ep + ProfileID +  NumInClusters + NumOutClusters.
  len = 2 + Z_EXTADDR_LEN + 1 + 2 + 1 + 1;
  len += (NumInClusters + NumOutClusters) * sizeof ( uint16 );
 
  if ( len >= ZDP_BUF_SZ-1 )
  {
    return afStatus_MEM_FAIL;
  }
  
  if ( LocalCoordinator != NLME_GetShortAddr() )
  {
    return afStatus_INVALID_PARAMETER;
  }

  *pBuf++ = LO_UINT16( LocalCoordinator );
  *pBuf++ = HI_UINT16( LocalCoordinator );

  ieeeAddr = NLME_GetExtAddr();
  pBuf = osal_cpyExtAddr( pBuf, ieeeAddr );
  
  *pBuf++ = endPoint;

  *pBuf++ = LO_UINT16( ProfileID );   // Profile ID
  *pBuf++ = HI_UINT16( ProfileID );

  *pBuf++ = NumInClusters; // Input cluster list
  for ( i = 0; i < NumInClusters; ++i )
  {
    *pBuf++ = LO_UINT16(InClusterList[i]);
    *pBuf++ = HI_UINT16(InClusterList[i]);
  }

  *pBuf++ = NumOutClusters; // Output cluster list
  for ( i = 0; i < NumOutClusters; ++i )
  {
    *pBuf++ = LO_UINT16(OutClusterList[i]);
    *pBuf++ = HI_UINT16(OutClusterList[i]);
  }

  return fillAndSend( &ZDP_TransID, dstAddr, End_Device_Bind_req, len );
}
#endif // ZDO_ENDDEVICEBIND_REQUEST

#if defined ( ZDO_BIND_UNBIND_REQUEST ) || defined ( ZDO_COORDINATOR )
/*********************************************************************
 * @fn          ZDP_BindUnbindReq
 *
 * @brief       This builds and send a Bind_req or Unbind_req message
 *              Depending on the ClusterID. This function
 *              sends a unicast message to the local coordinator.
 *
 * @param       BindOrUnbind - either Bind_req or Unbind_req
 * @param       dstAddr - destination address of the message
 * @param       SourceAddr - source 64 bit address of the binding
 * @param       SrcEPIntf - Source endpoint/interface
 * @param       ClusterID - Binding cluster ID
 * @param       DestinationAddr - destination 64 bit addr of binding
 * @param       DstEPIntf - destination endpoint/interface
 * @param       SecurityEnable - Security Options
 *
 * @return      afStatus_t
 */
afStatus_t ZDP_BindUnbindReq( uint16 BindOrUnbind, zAddrType_t *dstAddr,
                              byte *SourceAddr, byte SrcEndPoint,
                              cId_t ClusterID,
                              zAddrType_t *destinationAddr, byte DstEndPoint,
                              byte SecurityEnable )
{
  uint8 *pBuf = ZDP_TmpBuf;
  byte len;

  // SourceAddr + SrcEPIntf + ClusterID +  addrMode.
  len = Z_EXTADDR_LEN + 1 + sizeof( cId_t ) + sizeof( uint8 );
  if ( destinationAddr->addrMode == Addr64Bit )
    len += Z_EXTADDR_LEN + 1;     // +1 for DstEPIntf
  else if ( destinationAddr->addrMode == AddrGroup )
    len += sizeof ( uint16 );

  pBuf = osal_cpyExtAddr( pBuf, SourceAddr );
  *pBuf++ = SrcEndPoint;

  *pBuf++ = LO_UINT16( ClusterID );

  *pBuf++ = HI_UINT16( ClusterID );
  *pBuf++ = destinationAddr->addrMode;
  if ( destinationAddr->addrMode == Addr64Bit )
  {
    pBuf = osal_cpyExtAddr( pBuf, destinationAddr->addr.extAddr );
    *pBuf = DstEndPoint;
  }
  else if ( destinationAddr->addrMode == AddrGroup )
  {
    *pBuf++ = LO_UINT16( destinationAddr->addr.shortAddr );
    *pBuf++ = HI_UINT16( destinationAddr->addr.shortAddr );
  }

  FillAndSendTxOptions( &ZDP_TransID, dstAddr, BindOrUnbind, len, AF_MSG_ACK_REQUEST );
}
#endif // ZDO_BIND_UNBIND_REQUEST

/*********************************************************************
 * Network Management
 */

#if defined ( ZDO_MGMT_NWKDISC_REQUEST )
/*********************************************************************
 * @fn          ZDP_MgmtNwkDiscReq
 *
 * @brief       This builds and send a Mgmt_NWK_Disc_req message. This
 *              function sends a unicast message.
 *
 * @param       dstAddr - destination address of the message
 * @param       ScanChannels - 32 bit address bit map
 * @param       StartIndex - Starting index within the reporting network
 *                           list
 * @param       SecurityEnable - Security Options
 *
 * @return      afStatus_t
 */
afStatus_t ZDP_MgmtNwkDiscReq( zAddrType_t *dstAddr,
                               uint32 ScanChannels,
                               byte ScanDuration,
                               byte StartIndex,
                               byte SecurityEnable )
{
  byte *pBuf = ZDP_TmpBuf;
  byte len = sizeof( uint32 )+1+1;  // ScanChannels + ScanDuration + StartIndex.

  *pBuf++ = BREAK_UINT32( ScanChannels, 0 );
  *pBuf++ = BREAK_UINT32( ScanChannels, 1 );
  *pBuf++ = BREAK_UINT32( ScanChannels, 2 );
  *pBuf++ = BREAK_UINT32( ScanChannels, 3 );

  *pBuf++ = ScanDuration;
  *pBuf = StartIndex;

  return fillAndSend( &ZDP_TransID, dstAddr, Mgmt_NWK_Disc_req, len );
}
#endif // ZDO_MGMT_NWKDISC_REQUEST

#if defined ( ZDO_MGMT_JOINDIRECT_REQUEST )
/*********************************************************************
 * @fn          ZDP_MgmtDirectJoinReq
 *
 * @brief       This builds and send a Mgmt_Direct_Join_req message. This
 *              function sends a unicast message.
 *
 * @param       dstAddr - destination address of the message
 * @param       deviceAddr - 64 bit IEEE Address
 * @param       SecurityEnable - Security Options
 *
 * @return      afStatus_t
 */
afStatus_t ZDP_MgmtDirectJoinReq( zAddrType_t *dstAddr,
                               byte *deviceAddr,
                               byte capInfo,
                               byte SecurityEnable )
{
  osal_cpyExtAddr( ZDP_TmpBuf, deviceAddr );
  ZDP_TmpBuf[Z_EXTADDR_LEN] = capInfo;

  return fillAndSend( &ZDP_TransID, dstAddr, Mgmt_Direct_Join_req, (Z_EXTADDR_LEN + 1) );
}
#endif // ZDO_MGMT_JOINDIRECT_REQUEST

#if defined ( ZDO_MGMT_PERMIT_JOIN_REQUEST )
/*********************************************************************
 * @fn          ZDP_MgmtPermitJoinReq
 *
 * @brief       This builds and send a Mgmt_Permit_Join_req message.
 *
 * @param       dstAddr - destination address of the message
 * @param       duration - Permit duration
 * @param       TcSignificance - Trust Center Significance
 *
 * @return      afStatus_t
 */
afStatus_t ZDP_MgmtPermitJoinReq( zAddrType_t *dstAddr, byte duration,
                                  byte TcSignificance, byte SecurityEnable )
{
  // Build buffer
  ZDP_TmpBuf[ZDP_MGMT_PERMIT_JOIN_REQ_DURATION] = duration;
  ZDP_TmpBuf[ZDP_MGMT_PERMIT_JOIN_REQ_TC_SIG]   = TcSignificance;

  // Send the message
  return fillAndSend( &ZDP_TransID, dstAddr, Mgmt_Permit_Join_req,
                      ZDP_MGMT_PERMIT_JOIN_REQ_SIZE );
}
#endif // ZDO_MGMT_PERMIT_JOIN_REQUEST

/*********************************************************************
 * Network Management Responses
 */

#if defined ( ZDO_MGMT_NWKDISC_RESPONSE )
/*********************************************************************
 * @fn          ZDP_MgmtNwkDiscRsp
 *
 * @brief       This builds and send a Mgmt_NWK_Disc_rsp message. This
 *              function sends a unicast message.
 *
 * @param       dstAddr - destination address of the message
 * @param       Status - message status (ZDP_SUCCESS or other)
 * @param       NetworkCount - Total number of networks found
 * @param       StartIndex - Starting index within the reporting network
 *                           list
 * @param       NetworkListCount - number of network lists included
 *                                 in this message
 * @param       NetworkList - List of network descriptors
 * @param       SecurityEnable - Security Options
 *
 * @return      afStatus_t
 */
afStatus_t ZDP_MgmtNwkDiscRsp( byte TransSeq, zAddrType_t *dstAddr,
                            byte Status,
                            byte NetworkCount,
                            byte StartIndex,
                            byte NetworkListCount,
                            networkDesc_t *NetworkList,
                            byte SecurityEnable )
{
  byte *buf;
  byte *pBuf;
  byte len = 1+1+1+1;  // Status + NetworkCount + StartIndex + NetworkCountList.
  byte idx;

  len += ((NetworkListCount - StartIndex) * ( ZDP_NETWORK_EXTENDED_DISCRIPTOR_SIZE - 2 ));

  buf = osal_mem_alloc( len+1 );
  if ( buf == NULL )
  {
    return afStatus_MEM_FAIL;
  }

  pBuf = buf+1;

  *pBuf++ = Status;
  *pBuf++ = NetworkCount;
  *pBuf++ = StartIndex;
  *pBuf++ = NetworkListCount;

  for ( idx = StartIndex; idx < (NetworkListCount - StartIndex); idx++ )
  {
    osal_cpyExtAddr( pBuf, NetworkList->extendedPANID);
    pBuf += Z_EXTADDR_LEN;
   
    *pBuf++  = NetworkList->logicalChannel;                // LogicalChannel
    *pBuf    = NetworkList->stackProfile;                  // Stack profile
    *pBuf++ |= (byte)(NetworkList->version << 4);          // ZigBee Version
    *pBuf    = NetworkList->beaconOrder;                   // Beacon Order
    *pBuf++ |= (byte)(NetworkList->superFrameOrder << 4);  // Superframe Order

    if ( NetworkList->chosenRouter != INVALID_NODE_ADDR )
    {
      *pBuf++ = TRUE;                         // Permit Joining
    }
    else
    {
      *pBuf++ = FALSE;
    }

    NetworkList = NetworkList->nextDesc;    // Move to next list entry
  }

  FillAndSendBuffer( &TransSeq, dstAddr, Mgmt_NWK_Disc_rsp, len, buf );
}
#endif // ZDO_MGMT_NWKDISC_RESPONSE

#if defined ( ZDO_MGMT_LQI_RESPONSE ) && defined ( RTR_NWK )
/*********************************************************************
 * @fn          ZDP_MgmtLqiRsp
 *
 * @brief       This builds and send a Mgmt_Lqi_rsp message. This
 *              function sends a unicast message.
 *
 * @param       dstAddr - destination address of the message
 * @param       Status - message status (ZDP_SUCCESS or other)
 * @param       NeighborLqiEntries - Total number of entries found
 * @param       StartIndex - Starting index within the reporting list
 * @param       NeighborLqiCount - number of lists included
 *                                 in this message
 * @param       NeighborLqiList - List of NeighborLqiItems.  This list
 *                is the list to be sent, not the entire list
 * @param       SecurityEnable - true if secure
 *
 * @return      ZStatus_t
 */
ZStatus_t ZDP_MgmtLqiRsp( byte TransSeq, zAddrType_t *dstAddr,
                          byte Status,
                          byte NeighborLqiEntries,
                          byte StartIndex,
                          byte NeighborLqiCount,
                          ZDP_MgmtLqiItem_t* NeighborList,
                          byte SecurityEnable )
{
  ZDP_MgmtLqiItem_t* list = NeighborList;
  byte *buf, *pBuf;
  byte len, x;

  if ( ZSuccess != Status )
  {
    ZDP_TmpBuf[0] = Status;
    return fillAndSend( &TransSeq, dstAddr, Mgmt_Lqi_rsp, 1 );
  }

  // (Status + NeighborLqiEntries + StartIndex + NeighborLqiCount) +
  //  neighbor LQI data.
  len = (1 + 1 + 1 + 1) + (NeighborLqiCount * ZDP_MGMTLQI_EXTENDED_SIZE);

  buf = osal_mem_alloc( len+1 );
  if ( buf == NULL )
  {
    return afStatus_MEM_FAIL;
  }

  pBuf = buf+1;

  *pBuf++ = Status;
  *pBuf++ = NeighborLqiEntries;
  *pBuf++ = StartIndex;
  *pBuf++ = NeighborLqiCount;

  for ( x = 0; x < NeighborLqiCount; x++ )
  {
    osal_cpyExtAddr( pBuf, list->extPanID);         // Extended PanID
    pBuf += Z_EXTADDR_LEN;
   
    // EXTADDR
    pBuf = osal_cpyExtAddr( pBuf, list->extAddr );

    // NWKADDR
    *pBuf++ = LO_UINT16( list->nwkAddr );
    *pBuf++ = HI_UINT16( list->nwkAddr );

    // DEVICETYPE
    *pBuf = list->devType;

    // RXONIDLE
    *pBuf |= (uint8)(list->rxOnIdle << 2);

    // RELATIONSHIP
    *pBuf++ |= (uint8)(list->relation << 4);

    // PERMITJOINING
    *pBuf++ = (uint8)(list->permit);

    // DEPTH
    *pBuf++ = list->depth;
    
    // LQI
    *pBuf++ = list->lqi;

    list++; // next list entry
  }

  FillAndSendBuffer( &TransSeq, dstAddr, Mgmt_Lqi_rsp, len, buf );
}
#endif // ZDO_MGMT_LQI_RESPONSE && RTR_NWK

#if defined ( ZDO_MGMT_RTG_RESPONSE )
/*********************************************************************
 * @fn          ZDP_MgmtRtgRsp
 *
 * @brief       This builds and send a Mgmt_Rtg_rsp message. This
 *              function sends a unicast message.
 *
 * @param       dstAddr - destination address of the message
 * @param       Status - message status (ZDP_SUCCESS or other)
 * @param       RoutingTableEntries - Total number of entries
 * @param       StartIndex - Starting index within the reporting list
 * @param       RoutingTableListCount - number of entries included
 *                                      in this message
 * @param       RoutingTableList - List of Routing Table entries
 * @param       SecurityEnable - true to enable security for this message
 *
 * @return      ZStatus_t
 */
ZStatus_t ZDP_MgmtRtgRsp( byte TransSeq, zAddrType_t *dstAddr,
                            byte Status,
                            byte RoutingTableEntries,
                            byte StartIndex,
                            byte RoutingListCount,
                            rtgItem_t *RoutingTableList,
                            byte SecurityEnable )
{
  byte *buf;
  byte *pBuf;
  // Status + RoutingTableEntries + StartIndex + RoutingListCount.
  byte len = 1 + 1 + 1 + 1;
  byte x;

  // Add an array for Routing List data
  len += (RoutingListCount * ZDP_ROUTINGENTRY_SIZE);

  buf = osal_mem_alloc( (short)(len+1) );
  if ( buf == NULL )
  {
    return afStatus_MEM_FAIL;
  }

  pBuf = buf+1;

  *pBuf++ = Status;
  *pBuf++ = RoutingTableEntries;
  *pBuf++ = StartIndex;
  *pBuf++ = RoutingListCount;

  for ( x = 0; x < RoutingListCount; x++ )
  {
    *pBuf++ = LO_UINT16( RoutingTableList->dstAddress );  // Destination Address
    *pBuf++ = HI_UINT16( RoutingTableList->dstAddress );
    *pBuf++ = RoutingTableList->status;
    *pBuf++ = LO_UINT16( RoutingTableList->nextHopAddress );  // Next hop
    *pBuf++ = HI_UINT16( RoutingTableList->nextHopAddress );
    RoutingTableList++;    // Move to next list entry
  }

  FillAndSendBuffer( &TransSeq, dstAddr, Mgmt_Rtg_rsp, len, buf );
}
#endif // ZDO_MGMT_RTG_RESPONSE

#if defined ( ZDO_MGMT_BIND_RESPONSE ) && defined ( REFLECTOR )
/*********************************************************************
 * @fn          ZDP_MgmtBindRsp
 *
 * @brief       This builds and send a Mgmt_Bind_rsp message. This
 *              function sends a unicast message.
 *
 * @param       dstAddr - destination address of the message
 * @param       Status - message status (ZDP_SUCCESS or other)
 * @param       BindingTableEntries - Total number of entries
 * @param       StartIndex - Starting index within the reporting list
 * @param       BindingTableListCount - number of entries included
 *                                 in this message
 * @param       BindingTableList - List of Binding Table entries
 * @param       SecurityEnable - Security Options
 *
 * @return      ZStatus_t
 */
ZStatus_t ZDP_MgmtBindRsp( byte TransSeq, zAddrType_t *dstAddr,
                            byte Status,
                            byte BindingTableEntries,
                            byte StartIndex,
                            byte BindingTableListCount,
                            apsBindingItem_t *BindingTableList,
                            byte SecurityEnable )
{
  uint8 *buf;
  uint8 *pBuf;
  uint8 maxLen; // maxLen is the maximum packet length to allocate enough memory space
  uint8 len;    // Actual length varies due to different addrMode
  uint8 x;
  byte extZdpBindEntrySize = ZDP_BINDINGENTRY_SIZE + 1 + 1; // One more byte for cluserID and DstAddrMode
  byte shortZdpBindEntrySize = ZDP_BINDINGENTRY_SIZE + 1 + 1 + 2 - 8 - 1; // clusterID + DstAddrMode + shortAddr - ExtAddr - DstEndpoint

  // Status + BindingTableEntries + StartIndex + BindingTableListCount.
  maxLen = 1 + 1 + 1 + 1;
  maxLen += (BindingTableListCount * extZdpBindEntrySize );  //max length
  buf = osal_mem_alloc( maxLen + 1 );  // +1 for transaction ID

  if ( buf == NULL )
  {
    return afStatus_MEM_FAIL;
  }

  pBuf = buf+1;

  *pBuf++ = Status;
  *pBuf++ = BindingTableEntries;
  *pBuf++ = StartIndex;
  *pBuf++ = BindingTableListCount;

  // Initial length = Status + BindingTableEntries + StartIndex + BindingTableListCount.
  // length += ZDP_BINDINGENTRY_SIZE   -- Version 1.0
  //           extZdpBindEntrySize     -- Version 1.1 extended address mode
  //           shortZdpBindEntrySize   -- Version 1.1 group address mode

  len = 1 + 1 + 1 + 1;
  for ( x = 0; x < BindingTableListCount; x++ )
  {
    pBuf = osal_cpyExtAddr( pBuf, BindingTableList->srcAddr );
    *pBuf++ = BindingTableList->srcEP;

    // Cluster ID
    *pBuf++ = LO_UINT16( BindingTableList->clusterID );
    *pBuf++ = HI_UINT16( BindingTableList->clusterID );

    *pBuf++ = BindingTableList->dstAddr.addrMode;
    if ( BindingTableList->dstAddr.addrMode == Addr64Bit )
    {
      len += extZdpBindEntrySize;
      pBuf = osal_cpyExtAddr( pBuf, BindingTableList->dstAddr.addr.extAddr );
      *pBuf++ = BindingTableList->dstEP;
    }
    else
    {
      len += shortZdpBindEntrySize;
      *pBuf++ = LO_UINT16( BindingTableList->dstAddr.addr.shortAddr );
      *pBuf++ = HI_UINT16( BindingTableList->dstAddr.addr.shortAddr );
    }  
    BindingTableList++;    // Move to next list entry
  }

  FillAndSendBuffer( &TransSeq, dstAddr, Mgmt_Bind_rsp, len, buf );
}
#endif // ZDO_MGMT_BIND_RESPONSE && REFLECTOR

/*********************************************************************
 * Functions to register for ZDO Over-the-air messages
 */

/*********************************************************************
 * @fn          ZDO_RegisterForZDOMsg
 *
 * @brief       Call this function to register of an incoming over
 *              the air ZDO message - probably a response message
 *              but requests can also be received.
 *              Messages are delivered to the task with ZDO_CB_MSG
 *              as the message ID.
 *
 * @param       taskID - Where you would like the message delivered
 * @param       clusterID - What message?
 *
 * @return      ZSuccess - successful, ZMemError if not 
 */
ZStatus_t ZDO_RegisterForZDOMsg( uint8 taskID, uint16 clusterID )
{
  ZDO_MsgCB_t *pList;
  ZDO_MsgCB_t *pLast;
  ZDO_MsgCB_t *pNew;

  // Look for duplicate
  pList = zdoMsgCBs;
  while ( pList )
  {
    if ( pList->taskID == taskID && pList->clusterID == clusterID )
      return ( ZSuccess );
    pLast = pList;
    pList = (ZDO_MsgCB_t *)pList->next;
  }
  
  // Add to the list
  pNew = (ZDO_MsgCB_t *)osal_mem_alloc( sizeof ( ZDO_MsgCB_t ) );
  if ( pNew )
  {
    pNew->taskID = taskID;
    pNew->clusterID = clusterID;
    pNew->next = NULL;
    if ( zdoMsgCBs )
    {
      pLast->next = pNew;
    }
    else
      zdoMsgCBs = pNew;
    return ( ZSuccess );
  }
  else
    return ( ZMemError );
}

/*********************************************************************
 * @fn          ZDO_RemoveRegisteredCB
 *
 * @brief       Call this function if you don't want to receive the 
 *              incoming message.
 *
 * @param       taskID - Where the messages are being delivered.
 * @param       clusterID - What message?
 *
 * @return      ZSuccess - successful, ZFailure if not found
 */
ZStatus_t ZDO_RemoveRegisteredCB( uint8 taskID, uint16 clusterID )
{
  ZDO_MsgCB_t *pList;
  ZDO_MsgCB_t *pLast = NULL;

  pList = zdoMsgCBs;  
  while ( pList )
  {
    if ( pList->taskID == taskID && pList->clusterID == clusterID )
    {
      if ( pLast )
      {
        // remove this one from the linked list
        pLast->next = pList->next;
      }
      else if ( pList->next )
      {
        // remove the first one from the linked list
        zdoMsgCBs = pList->next;
      }
      else
      {
        // remove the only item from the list
        zdoMsgCBs = (ZDO_MsgCB_t *)NULL;
      }
      osal_mem_free( pList );
      return ( ZSuccess );      
    }
    pLast = pList;
    pList = pList->next;
  }
  
  return ( ZFailure );
}

/*********************************************************************
 * @fn          ZDO_SendMsgCBs
 *
 * @brief       This function sends messages to registered tasks.
 *              Local to ZDO and shouldn't be called outside of ZDO.
 *
 * @param       inMsg - incoming message
 *
 * @return      TRUE if sent to at least 1 task, FALSE if not
 */
uint8 ZDO_SendMsgCBs( zdoIncomingMsg_t *inMsg )
{
  uint8 ret = FALSE;
  ZDO_MsgCB_t *pList = zdoMsgCBs;
  while ( pList )
  {
    if ( pList->clusterID == inMsg->clusterID )
    {
      zdoIncomingMsg_t *msgPtr;

      // Send the address to the task
      msgPtr = (zdoIncomingMsg_t *)osal_msg_allocate( sizeof( zdoIncomingMsg_t ) + inMsg->asduLen );
      if ( msgPtr )
      {
        // copy struct
        osal_memcpy( msgPtr, inMsg, sizeof( zdoIncomingMsg_t ));
        
        if ( inMsg->asduLen )
        {
          msgPtr->asdu = (byte*)(((byte*)msgPtr) + sizeof( zdoIncomingMsg_t ));
          osal_memcpy( msgPtr->asdu, inMsg->asdu, inMsg->asduLen );
        }
    
        msgPtr->hdr.event = ZDO_CB_MSG;
        osal_msg_send( pList->taskID, (uint8 *)msgPtr );
        ret = TRUE;
      }
    }
    pList = (ZDO_MsgCB_t *)pList->next;
  }
  return ( ret );
}  

/*********************************************************************
 * Incoming message processor
 */

/*********************************************************************
 * @fn          ZDP_IncomingData
 *
 * @brief       This function indicates the transfer of a data PDU (ASDU)
 *              from the APS sub-layer to the ZDO.
 *
 * @param       pData - Incoming Message
 *
 * @return      none
 */
void ZDP_IncomingData( afIncomingMSGPacket_t *pData )
{
  uint8 x = 0;
  uint8 handled;
  zdoIncomingMsg_t inMsg;

  inMsg.srcAddr.addrMode = Addr16Bit;
  inMsg.srcAddr.addr.shortAddr = pData->srcAddr.addr.shortAddr;
  inMsg.wasBroadcast = pData->wasBroadcast;
  inMsg.clusterID = pData->clusterId;
  inMsg.SecurityUse = pData->SecurityUse;

  inMsg.asduLen = pData->cmd.DataLength-1;
  inMsg.asdu = pData->cmd.Data+1;
  inMsg.TransSeq = pData->cmd.Data[0];
  
  handled = ZDO_SendMsgCBs( &inMsg );
  
#if defined( MT_ZDO_FUNC )
  MT_ZdoRsp( &inMsg );
#endif

  while ( zdpMsgProcs[x].clusterID != 0xFFFF )
  {
    if ( zdpMsgProcs[x].clusterID == inMsg.clusterID )
    {
      zdpMsgProcs[x].pFn( &inMsg );
      return;
    }
    x++;
  }

  // Handle unhandled messages
  if ( !handled )
    ZDApp_InMsgCB( &inMsg );
}

/*********************************************************************
*********************************************************************/

