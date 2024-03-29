/**************************************************************************************************
  Filename:       ZDConfig.h
  Revised:        $Date: 2007-10-28 18:43:04 -0700 (Sun, 28 Oct 2007) $
  Revision:       $Revision: 15800 $

  Description:    This file contains the configuration attributes for the Zigbee Device Object.
                  These are references to Configuration items that MUST be defined in ZDApp.c.
                  The names mustn't change.


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

#ifndef ZDCONFIG_H
#define ZDCONFIG_H

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */
#include "AF.h"

/*********************************************************************
 * Enable Features
 */
#if defined ( MT_ZDO_FUNC )
  // All of the ZDO functions are enabled for ZTool use.
  #define ZDO_NWKADDR_REQUEST
  #define ZDO_IEEEADDR_REQUEST
  #define ZDO_MATCH_REQUEST
  #define ZDO_NODEDESC_REQUEST
  #define ZDO_POWERDESC_REQUEST
  #define ZDO_SIMPLEDESC_REQUEST
  #define ZDO_ACTIVEEP_REQUEST
  
  #define ZDO_COMPLEXDESC_REQUEST
  #define ZDO_USERDESC_REQUEST
  #define ZDO_USERDESCSET_REQUEST
  #define ZDO_ENDDEVICEBIND_REQUEST
  #define ZDO_BIND_UNBIND_REQUEST
  #define ZDO_SERVERDISC_REQUEST
  #define ZDO_NETWORKSTART_REQUEST

  #define ZDO_COMPLEXDESC_RESPONSE
  #define ZDO_USERDESC_RESPONSE
  #define ZDO_USERDESCSET_RESPONSE
  #define ZDO_SERVERDISC_RESPONSE
  

#if defined ( MT_ZDO_MGMT )
  #define ZDO_MGMT_NWKDISC_REQUEST
  #define ZDO_MGMT_LQI_REQUEST
  #define ZDO_MGMT_RTG_REQUEST
  #define ZDO_MGMT_BIND_REQUEST
  #define ZDO_MGMT_LEAVE_REQUEST
  #define ZDO_MGMT_JOINDIRECT_REQUEST
  #define ZDO_MGMT_PERMIT_JOIN_REQUEST
  #define ZDO_ENDDEVICE_ANNCE_REQUEST
  #define ZDO_MGMT_NWKDISC_RESPONSE
  #define ZDO_MGMT_LQI_RESPONSE
  #define ZDO_MGMT_RTG_RESPONSE
  #define ZDO_MGMT_BIND_RESPONSE
  #define ZDO_MGMT_LEAVE_RESPONSE
  #define ZDO_MGMT_JOINDIRECT_RESPONSE
  #define ZDO_MGMT_PERMIT_JOIN_RESPONSE
  #define ZDO_ENDDEVICE_ANNCE
#endif

#else // !MT_ZDO_FUNC

  // Normal operation and sample apps only use End Device Bind
  // and Match Request.

  //#define ZDO_NWKADDR_REQUEST
  //#define ZDO_IEEEADDR_REQUEST
  #define ZDO_MATCH_REQUEST
  //#define ZDO_NODEDESC_REQUEST
  //#define ZDO_POWERDESC_REQUEST
  //#define ZDO_SIMPLEDESC_REQUEST
  //#define ZDO_ACTIVEEP_REQUEST
  //#define ZDO_COMPLEXDESC_REQUEST
  //#define ZDO_USERDESC_REQUEST
  //#define ZDO_USERDESCSET_REQUEST
  #define ZDO_ENDDEVICEBIND_REQUEST
  //#define ZDO_BIND_UNBIND_REQUEST
  //#define ZDO_SERVERDISC_REQUEST

  //#define ZDO_BIND_UNBIND_RESPONSE
  //#define ZDO_COMPLEXDESC_RESPONSE
  //#define ZDO_USERDESC_RESPONSE
  //#define ZDO_USERDESCSET_RESPONSE
  //#define ZDO_SERVERDISC_RESPONSE

  //#define ZDO_MGMT_NWKDISC_REQUEST
  //#define ZDO_MGMT_LQI_REQUEST
  //#define ZDO_MGMT_RTG_REQUEST
  //#define ZDO_MGMT_BIND_REQUEST
  //#define ZDO_MGMT_LEAVE_REQUEST
  //#define ZDO_MGMT_JOINDIRECT_REQUEST
  //#define ZDO_MGMT_PERMIT_JOIN_REQUEST
  //#define ZDO_ENDDEVICE_ANNCE_REQUEST
  //#define ZDO_MGMT_NWKDISC_RESPONSE
  //#define ZDO_MGMT_LQI_RESPONSE
  //#define ZDO_MGMT_RTG_RESPONSE
  //#define ZDO_MGMT_BIND_RESPONSE
  //#define ZDO_MGMT_LEAVE_RESPONSE
  //#define ZDO_MGMT_JOINDIRECT_RESPONSE
  //#define ZDO_MGMT_PERMIT_JOIN_RESPONSE
  //#define ZDO_ENDDEVICE_ANNCE

#if defined ( REFLECTOR  )
  // Binding needs this request to do a 64 to 16 bit conversion
  #define ZDO_NWKADDR_REQUEST
  #define ZDO_IEEEADDR_REQUEST
  #define ZDO_BIND_UNBIND_RESPONSE
#endif

#endif  // !MT_ZDO_FUNC

/*********************************************************************
 * Constants
 */

#define MAX_BUFFER_SIZE		        	80		
#define MAX_TRANSFER_SIZE	        	80

#define MAX_ENDPOINTS	            	240

// Node Description Bitfields
#define ZDOLOGICALTYPE_MASK		    	0x07
#define ZDOAPSFLAGS_MASK		      	0x07
#define ZDOFREQUENCYBANDS_MASK    	0x1F
#define ZDOAPSFLAGS_BITLEN	    		3

#define SIMPLE_DESC_DATA_SIZE				7
#define NODE_DESC_DATA_SIZE					10

// Simple Description Bitfields
#define ZDOENDPOINT_BITLEN		      5
#define ZDOENDPOINT_MASK		        0x1F
#define ZDOINTERFACE_MASK	      		0x07
#define ZDOAPPFLAGS_MASK	      		0x0F
#define ZDOAPPDEVVER_MASK		      	0x0F
#define ZDOAPPDEVVER_BITLEN		    	4

/*********************************************************************
 * Attributes
 */

extern NodeDescriptorFormat_t ZDO_Config_Node_Descriptor;
extern NodePowerDescriptorFormat_t ZDO_Config_Power_Descriptor;

/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* ZDCONFIG_H */
