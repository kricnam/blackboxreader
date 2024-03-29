/**************************************************************************************************
  Filename:       zcl_sampleLight.c
  Revised:        $Date: 2007-11-20 13:57:50 -0800 (Tue, 20 Nov 2007) $
  Revision:       $Revision: 15954 $


  Description:    Zigbee Cluster Library - sample device application.


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

/*********************************************************************
  This device will be like a Light device.  This application is not
  intended to be a Light device, but will use the device description
  to implement this sample code.
*********************************************************************/

/*********************************************************************
 * INCLUDES
 */
#include "ZComDef.h"
#include "OSAL.h"
#include "AF.h"
#include "ZDApp.h"

#include "zcl.h"
#include "zcl_general.h"
#include "zcl_ha.h"

#include "zcl_samplelight.h"

#include "onboard.h"

/* HAL */
#include "hal_lcd.h"
#include "hal_led.h"
#include "hal_key.h"


/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * CONSTANTS
 */
/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * GLOBAL VARIABLES
 */
byte zclSampleLight_TaskID;

/*********************************************************************
 * GLOBAL FUNCTIONS
 */

/*********************************************************************
 * LOCAL VARIABLES
 */
//static afAddrType_t zclSampleLight_DstAddr;

#define ZCLSAMPLELIGHT_BINDINGLIST       2
static cId_t bindingInClusters[ZCLSAMPLELIGHT_BINDINGLIST] =
{
  ZCL_HA_CLUSTER_ID_GEN_ON_OFF,
  ZCL_HA_CLUSTER_ID_GEN_LEVEL_CONTROL
};

// Test Endpoint to allow SYS_APP_MSGs
static endPointDesc_t sampleLight_TestEp =
{
  20,                                 // Test endpoint
  &zclSampleLight_TaskID,
  (SimpleDescriptionFormat_t *)NULL,  // No Simple description for this test endpoint
  (afNetworkLatencyReq_t)0            // No Network Latency req
};

/*********************************************************************
 * LOCAL FUNCTIONS
 */
static void zclSampleLight_HandleKeys( byte shift, byte keys );
static void zclSampleLight_BasicResetCB( void );
static void zclSampleLight_IdentifyCB( zclIdentify_t *pCmd );
static void zclSampleLight_IdentifyQueryRspCB( zclIdentifyQueryRsp_t *pRsp );
static void zclSampleLight_OnOffCB( uint8 cmd );
static void zclSampleLight_ProcessIdentifyTimeChange( void );

/*********************************************************************
 * ZCL General Profile Callback table
 */
static zclGeneral_AppCallbacks_t zclSampleLight_CmdCallbacks =
{
  zclSampleLight_BasicResetCB,              // Basic Cluster Reset command
  zclSampleLight_IdentifyCB,                // Identify command  
  zclSampleLight_IdentifyQueryRspCB,        // Identify Query Response command
  zclSampleLight_OnOffCB,                   // On/Off cluster command
  NULL,                                     // Level Control Move to Level command
  NULL,                                     // Level Control Move command
  NULL,                                     // Level Control Step command
  NULL,                                     // Group Response commands
  NULL,                                     // Scene Store Request command
  NULL,                                     // Scene Recall Request command
  NULL,                                     // Scene Response command
  NULL,                                     // Alarm (Response) command
  NULL,                                     // RSSI Location commands
  NULL,                                     // RSSI Location Response commands
};

/*********************************************************************
 * @fn          zclSampleLight_Init
 *
 * @brief       Initialization function for the zclGeneral layer.
 *
 * @param       none
 *
 * @return      none
 */
void zclSampleLight_Init( byte task_id )
{
  zclSampleLight_TaskID = task_id;

  // Set destination address to indirect
  //zclSampleLight_DstAddr.addrMode = (afAddrMode_t)AddrNotPresent;
  //zclSampleLight_DstAddr.endPoint = 0;
  //zclSampleLight_DstAddr.addr.shortAddr = 0;

  // This app is part of the Home Automation Profile
  zclHA_Init( &zclSampleLight_SimpleDesc );

  // Register the ZCL General Cluster Library callback functions
  zclGeneral_RegisterCmdCallbacks( SAMPLELIGHT_ENDPOINT, &zclSampleLight_CmdCallbacks );

  // Register the application's attribute list
  zcl_registerAttrList( SAMPLELIGHT_ENDPOINT, SAMPLELIGHT_MAX_ATTRIBUTES, zclSampleLight_Attrs );

  // Register for all key events - This app will handle all key events
  RegisterForKeys( zclSampleLight_TaskID );

  // Register for a test endpoint
  afRegister( &sampleLight_TestEp );
}

/*********************************************************************
 * @fn          zclSample_event_loop
 *
 * @brief       Event Loop Processor for zclGeneral.
 *
 * @param       none
 *
 * @return      none
 */
uint16 zclSampleLight_event_loop( uint8 task_id, uint16 events )
{
  afIncomingMSGPacket_t *MSGpkt;

  if ( events & SYS_EVENT_MSG )
  {
    while ( (MSGpkt = (afIncomingMSGPacket_t *)osal_msg_receive( zclSampleLight_TaskID )) )
    {
      switch ( MSGpkt->hdr.event )
      {
        case KEY_CHANGE:
          zclSampleLight_HandleKeys( ((keyChange_t *)MSGpkt)->state, ((keyChange_t *)MSGpkt)->keys );
          break;

        default:
          break;
      }

      // Release the memory
      osal_msg_deallocate( (uint8 *)MSGpkt );
    }

    // return unprocessed events
    return (events ^ SYS_EVENT_MSG);
  }

  if ( events & SAMPLELIGHT_IDENTIFY_TIMEOUT_EVT )
  {
    if ( zclSampleLight_IdentifyTime > 0 )
      zclSampleLight_IdentifyTime--;
    zclSampleLight_ProcessIdentifyTimeChange();

    return ( events ^ SAMPLELIGHT_IDENTIFY_TIMEOUT_EVT );
  }

  // Discard unknown events
  return 0;
}

/*********************************************************************
 * @fn      zclSampleLight_HandleKeys
 *
 * @brief   Handles all key events for this device.
 *
 * @param   shift - true if in shift/alt.
 * @param   keys - bit field for key events. Valid entries:
 *                 HAL_KEY_SW_4
 *                 HAL_KEY_SW_3
 *                 HAL_KEY_SW_2
 *                 HAL_KEY_SW_1
 *
 * @return  none
 */
static void zclSampleLight_HandleKeys( byte shift, byte keys )
{
  zAddrType_t dstAddr;

  if ( keys & HAL_KEY_SW_2 )
  {
    // Initiate an End Device Bind Request, this bind request will
    // only use a cluster list that is important to binding.
    dstAddr.addrMode = afAddr16Bit;
    dstAddr.addr.shortAddr = 0;   // Coordinator makes the match
    ZDP_EndDeviceBindReq( &dstAddr, NLME_GetShortAddr(),
                           SAMPLELIGHT_ENDPOINT,
                           ZCL_HA_PROFILE_ID,
                           ZCLSAMPLELIGHT_BINDINGLIST, bindingInClusters,
                           0, NULL,   // No Outgoing clusters to bind
                           TRUE );
  }

  if ( keys & HAL_KEY_SW_3 )
  {
  }

  if ( keys & HAL_KEY_SW_4 )
  {
  }
}

/*********************************************************************
 * @fn      zclSampleLight_ProcessIdentifyTimeChange
 *
 * @brief   Called to process any change to the IdentifyTime attribute.
 *
 * @param   none
 *
 * @return  none
 */
static void zclSampleLight_ProcessIdentifyTimeChange( void )
{
  if ( zclSampleLight_IdentifyTime > 0 )
  {
    osal_start_timerEx( zclSampleLight_TaskID, SAMPLELIGHT_IDENTIFY_TIMEOUT_EVT, 1000 );
    HalLedBlink ( HAL_LED_4, 0xFF, HAL_LED_DEFAULT_DUTY_CYCLE, HAL_LED_DEFAULT_FLASH_TIME );
  }
  else
  {
    if ( zclSampleLight_OnOff )
      HalLedSet ( HAL_LED_4, HAL_LED_MODE_ON );
    else
      HalLedSet ( HAL_LED_4, HAL_LED_MODE_OFF );
    osal_stop_timerEx( zclSampleLight_TaskID, SAMPLELIGHT_IDENTIFY_TIMEOUT_EVT );
  }
}

/*********************************************************************
 * @fn      zclSampleLight_BasicResetCB
 *
 * @brief   Callback from the ZCL General Cluster Library
 *          to set all the Basic Cluster attributes to default values.
 *
 * @param   none
 *
 * @return  none
 */
static void zclSampleLight_BasicResetCB( void )
{
  // Reset all attributes to default values
}

/*********************************************************************
 * @fn      zclSampleLight_IdentifyCB
 *
 * @brief   Callback from the ZCL General Cluster Library when
 *          it received an Identity Command for this application.
 *
 * @param   srcAddr - source address and endpoint of the response message
 * @param   identifyTime - the number of seconds to identify yourself
 *
 * @return  none
 */
static void zclSampleLight_IdentifyCB( zclIdentify_t *pCmd )
{
  zclSampleLight_IdentifyTime = pCmd->identifyTime;
  zclSampleLight_ProcessIdentifyTimeChange();
}

/*********************************************************************
 * @fn      zclSampleLight_IdentifyQueryRspCB
 *
 * @brief   Callback from the ZCL General Cluster Library when
 *          it received an Identity Query Response Command for this application.
 *
 * @param   srcAddr - requestor's address
 * @param   timeout - number of seconds to identify yourself (valid for query response)
 *
 * @return  none
 */
static void zclSampleLight_IdentifyQueryRspCB(  zclIdentifyQueryRsp_t *pRsp )
{
  // Query Response (with timeout value)
}

/*********************************************************************
 * @fn      zclSampleLight_OnOffCB
 *
 * @brief   Callback from the ZCL General Cluster Library when
 *          it received an On/Off Command for this application.
 *
 * @param   cmd - COMMAND_ON, COMMAND_OFF or COMMAND_TOGGLE
 *
 * @return  none
 */
static void zclSampleLight_OnOffCB( uint8 cmd )
{
  // Turn on the light
  if ( cmd == COMMAND_ON )
    zclSampleLight_OnOff = LIGHT_ON;

  // Turn off the light
  else if ( cmd == COMMAND_OFF )
    zclSampleLight_OnOff = LIGHT_OFF;

  // Toggle the light
  else
  {
    if ( zclSampleLight_OnOff == LIGHT_OFF )
      zclSampleLight_OnOff = LIGHT_ON;
    else
      zclSampleLight_OnOff = LIGHT_OFF;
  }

  // In this sample app, we use LED4 to simulate the Light
  if ( zclSampleLight_OnOff == LIGHT_ON )
    HalLedSet( HAL_LED_4, HAL_LED_MODE_ON );
  else
    HalLedSet( HAL_LED_4, HAL_LED_MODE_OFF );
}


/****************************************************************************
****************************************************************************/


