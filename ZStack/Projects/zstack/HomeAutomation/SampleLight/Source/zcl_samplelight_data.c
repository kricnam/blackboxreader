/**************************************************************************************************
  Filename:       zcl_samplelight_data.c
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
 * INCLUDES
 */
#include "ZComDef.h"
#include "OSAL.h"
#include "AF.h"
#include "ZDConfig.h"

#include "zcl.h"
#include "zcl_general.h"
#include "zcl_ha.h"

#include "zcl_samplelight.h"

/*********************************************************************
 * CONSTANTS
 */

#define SAMPLELIGHT_DEVICE_VERSION     0
#define SAMPLELIGHT_FLAGS              0

#define SAMPLELIGHT_HWVERSION          1
#define SAMPLELIGHT_ZCLVERSION         1

/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * GLOBAL VARIABLES
 */

// Basic Cluster
CONST uint8 zclSampleLight_HWRevision = SAMPLELIGHT_HWVERSION;
CONST uint8 zclSampleLight_ZCLVersion = SAMPLELIGHT_ZCLVERSION;
CONST uint8 zclSampleLight_ManufacturerName[] = { 16, 'T','e','x','a','s','I','n','s','t','r','u','m','e','n','t','s' };
CONST uint8 zclSampleLight_ModelId[] = { 16, 'T','I','0','0','0','1',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ' };
CONST uint8 zclSampleLight_DateCode[] = { 16, '2','0','0','6','0','8','3','1',' ',' ',' ',' ',' ',' ',' ',' ' };
CONST uint8 zclSampleLight_PowerSource = POWER_SOURCE_MAINS_1_PHASE;

uint8 zclSampleLight_LocationDescription[17] = { 16, ' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ' };
uint8 zclSampleLight_PhysicalEnvironment = 0;
uint8 zclSampleLight_DeviceEnable = DEVICE_ENABLED;

// Identify Cluster
uint16 zclSampleLight_IdentifyTime = 0;

// On/Off Cluster
uint8  zclSampleLight_OnOff = LIGHT_OFF;

/*********************************************************************
 * ATTRIBUTE DEFINITIONS - Uses REAL cluster IDs
 */
CONST zclAttrRec_t zclSampleLight_Attrs[SAMPLELIGHT_MAX_ATTRIBUTES] =
{
  // *** General Basic Cluster Attributes ***
  {
    ZCL_HA_CLUSTER_ID_GEN_BASIC,          // Real Cluster IDs - not logical - defined in the profile (ie. zcl_ha.h)
    {  // Attribute record
      ATTRID_BASIC_HW_VERSION,            // Attribute ID - Found in Cluster Library header (ie. zcl_general.h)
      ZCL_DATATYPE_UINT8,                 // Data Type - found in zcl.h
      ACCESS_CONTROL_READ,                // Variable access control - found in zcl.h
      (void *)&zclSampleLight_HWRevision  // Pointer to attribute variable
    }
  },
  {
    ZCL_HA_CLUSTER_ID_GEN_BASIC,
    { // Attribute record
      ATTRID_BASIC_ZCL_VERSION,
      ZCL_DATATYPE_UINT8,
      ACCESS_CONTROL_READ,
      (void *)&zclSampleLight_ZCLVersion
    }
  },
  {
    ZCL_HA_CLUSTER_ID_GEN_BASIC,
    { // Attribute record
      ATTRID_BASIC_MANUFACTURER_NAME,
      ZCL_DATATYPE_CHAR_STR,
      ACCESS_CONTROL_READ,
      (void *)zclSampleLight_ManufacturerName
    }
  },
  {
    ZCL_HA_CLUSTER_ID_GEN_BASIC,
    { // Attribute record
      ATTRID_BASIC_MODEL_ID,
      ZCL_DATATYPE_CHAR_STR,
      ACCESS_CONTROL_READ,
      (void *)zclSampleLight_ModelId
    }
  },
  {
    ZCL_HA_CLUSTER_ID_GEN_BASIC,
    { // Attribute record
      ATTRID_BASIC_DATE_CODE,
      ZCL_DATATYPE_CHAR_STR,
      ACCESS_CONTROL_READ,
      (void *)zclSampleLight_DateCode
    }
  },
  {
    ZCL_HA_CLUSTER_ID_GEN_BASIC,
    { // Attribute record
      ATTRID_BASIC_POWER_SOURCE,
      ZCL_DATATYPE_UINT8,
      ACCESS_CONTROL_READ,
      (void *)&zclSampleLight_PowerSource
    }
  },
  {
    ZCL_HA_CLUSTER_ID_GEN_BASIC,
    { // Attribute record
      ATTRID_BASIC_LOCATION_DESC,
      ZCL_DATATYPE_CHAR_STR,
      (ACCESS_CONTROL_READ | ACCESS_CONTROL_WRITE),
      (void *)zclSampleLight_LocationDescription
    }
  },
  {
    ZCL_HA_CLUSTER_ID_GEN_BASIC,
    { // Attribute record
      ATTRID_BASIC_PHYSICAL_ENV,
      ZCL_DATATYPE_UINT8,
      (ACCESS_CONTROL_READ | ACCESS_CONTROL_WRITE),
      (void *)&zclSampleLight_PhysicalEnvironment
    }
  },
  {
    ZCL_HA_CLUSTER_ID_GEN_BASIC,
    { // Attribute record
      ATTRID_BASIC_DEVICE_ENABLED,
      ZCL_DATATYPE_BOOLEAN,
      (ACCESS_CONTROL_READ | ACCESS_CONTROL_WRITE),
      (void *)&zclSampleLight_DeviceEnable
    }
  },

  // *** Identify Cluster Attribute ***
  {
    ZCL_HA_CLUSTER_ID_GEN_IDENTIFY,
    { // Attribute record
      ATTRID_IDENTIFY_TIME,
      ZCL_DATATYPE_UINT16,
      (ACCESS_CONTROL_READ | ACCESS_CONTROL_WRITE),
      (void *)&zclSampleLight_IdentifyTime
    }
  },

  // *** On/Off Cluster Attributes ***
  {
    ZCL_HA_CLUSTER_ID_GEN_ON_OFF,
    { // Attribute record
      ATTRID_ON_OFF,
      ZCL_DATATYPE_UINT8,
      ACCESS_CONTROL_READ,
      (void *)&zclSampleLight_OnOff
    }
  },
};

/*********************************************************************
 * SIMPLE DESCRIPTOR
 */
// This is the Cluster ID List and should be filled with Application
// specific cluster IDs.
#define ZCLSAMPLELIGHT_MAX_INCLUSTERS       5
const cId_t zclSampleLight_InClusterList[ZCLSAMPLELIGHT_MAX_INCLUSTERS] =
{
  ZCL_HA_CLUSTER_ID_GEN_BASIC,
  ZCL_HA_CLUSTER_ID_GEN_SCENES,
  ZCL_HA_CLUSTER_ID_GEN_GROUPS,
  ZCL_HA_CLUSTER_ID_GEN_ON_OFF,
  ZCL_HA_CLUSTER_ID_GEN_LEVEL_CONTROL
};

#define ZCLSAMPLELIGHT_MAX_OUTCLUSTERS       1
const cId_t zclSampleLight_OutClusterList[ZCLSAMPLELIGHT_MAX_OUTCLUSTERS] =
{
  ZCL_HA_CLUSTER_ID_GEN_BASIC
};

SimpleDescriptionFormat_t zclSampleLight_SimpleDesc =
{
  SAMPLELIGHT_ENDPOINT,                  //  int Endpoint;
  ZCL_HA_PROFILE_ID,                     //  uint16 AppProfId[2];
  ZCL_HA_DEVICEID_DIMMABLE_LIGHT,        //  uint16 AppDeviceId[2];
  SAMPLELIGHT_DEVICE_VERSION,            //  int   AppDevVer:4;
  SAMPLELIGHT_FLAGS,                     //  int   AppFlags:4;
  ZCLSAMPLELIGHT_MAX_INCLUSTERS,         //  byte  AppNumInClusters;
  (cId_t *)zclSampleLight_InClusterList, //  byte *pAppInClusterList;
  ZCLSAMPLELIGHT_MAX_OUTCLUSTERS,        //  byte  AppNumInClusters;
  (cId_t *)zclSampleLight_OutClusterList //  byte *pAppInClusterList;
};

/*********************************************************************
 * GLOBAL FUNCTIONS
 */

/*********************************************************************
 * LOCAL FUNCTIONS
 */

/****************************************************************************
****************************************************************************/


