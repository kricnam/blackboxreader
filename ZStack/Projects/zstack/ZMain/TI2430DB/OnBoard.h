/**************************************************************************************************
  Filename:       OnBoard.h
  Revised:        $Date: 2007-10-27 17:16:54 -0700 (Sat, 27 Oct 2007) $
  Revision:       $Revision: 15793 $

  Description:    Defines stuff for EVALuation boards
  Notes:          This file targets the Chipcon CC2430DB/CC2430EB


  Copyright 2005-2007 Texas Instruments Incorporated. All rights reserved.

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

#ifndef ONBOARD_H
#define ONBOARD_H

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */

// Chipcon/Figure8 compatibility
#include "hal_mcu.h"
#include "hal_uart.h"
#include "hal_sleep.h"
#include "hal_mailbox.h"
#include "osal.h"


/*********************************************************************
 * GLOBAL VARIABLES
 */

// 64-bit Extended Address of this device
extern uint8 aExtendedAddress[8];

/*********************************************************************
 * CONSTANTS
 */

// Timer clock and power-saving definitions
#define TIMER_DECR_TIME    1  // 1ms - has to be matched with TC_OCC

/* OSAL timer defines */
#define TICK_TIME   1000   // Timer per tick - in micro-sec
/*
  Timer4 interrupts @ 1.0 msecs using 1/128 pre-scaler
  TICK_COUNT = (CPUMHZ / 128) / 1000
*/
#ifdef CPU_16MHZ
  #define TICK_COUNT  1  // 16 Mhz Output Compare Count
  #define RETUNE_THRESHOLD 1  // Threshold for power saving algorithm
#elif defined CPU32MHZ
  #define TICK_COUNT  1  // 32 Mhz Output Compare Count
  #define RETUNE_THRESHOLD 1  // Threshold for power saving algorithm
#endif

/* OSAL Timer define */
#define OSAL_TIMER  HAL_TIMER_2

/* CC2430 DEFINITIONS */

// MEMCTR bit definitions
#define ALWAYS1    0x01  // Reserved: always=1
#define CACHDIS    0x02  // Flash cache: disable=1
#define FMAP0      0x10  // Flash bank map, bit0
#define FMAP1      0x20  // Flash bank map, bit1
#define FMAP       0x30  // Flash bank map, mask
#define MUNIF      0x40  // Memory mapping: unified=1

// PCON bit definitions
#define PMODESET   0x01  // Power mode control: 1=set PMx

// Reset bit definitions
#define RESETPO    0x00  // Power-On reset
#define RESETEX    0x08  // External reset
#define RESETWD    0x10  // WatchDog reset

/* GPIO PORT DEFINITIONS */
// GPIO bit definitions
#define GPIO_0     0x01  // Px_0: GPIO=0, PIO=1
#define GPIO_1     0x02  // Px_1: GPIO=0, PIO=1
#define GPIO_2     0x04  // Px_2: GPIO=0, PIO=1
#define GPIO_3     0x08  // Px_3: GPIO=0, PIO=1
#define GPIO_4     0x10  // Px_4: GPIO=0, PIO=1
#define GPIO_5     0x20  // Px_5: GPIO=0, PIO=1
#define GPIO_6     0x40  // Px_6: GPIO=0, PIO=1
#define GPIO_7     0x80  // Px_7: GPIO=0, PIO=1

/* WATCHDOG TIMER DEFINITIONS */
// WDCTL bit definitions

#define WDINT0     0x01  // Interval, bit0
#define WDINT1     0x02  // Interval, bit1
#define WDINT      0x03  // Interval, mask
#define WDMODE     0x04  // Mode: watchdog=0, timer=1
#define WDEN       0x08  // Timer: disabled=0, enabled=1
#define WDCLR0     0x10  // Clear timer, bit0
#define WDCLR1     0x20  // Clear timer, bit1
#define WDCLR2     0x40  // Clear timer, bit2
#define WDCLR3     0x80  // Clear timer, bit3
#define WDCLR      0xF0  // Clear timer, mask

// WD timer intervals
#define WDTISH     0x03  // Short: clk * 64
#define WDTIMD     0x02  // Medium: clk * 512
#define WDTILG     0x01  // Long: clk * 8192
#define WDTIMX     0x00  // Maximum: clk * 32768

// WD clear timer patterns
#define WDCLP1     0xA0  // Clear pattern 1
#define WDCLP2     0x50  // Clear pattern 2

/*********************************************************************
 * MACROS
 */

// DB peripheral VDD control
#ifdef CC2430DB
  #define INIT_DBIO() { P1DIR |= GPIO_2; P1_2 = 0; }
  #define STOP_DBIO() { P1_2 = 1; }
#else // CC2430BB or CC2430EB
  #define INIT_DBIO()
  #define STOP_DBIO()
#endif

// These Key definitions are unique to this development system.
// They are used to bypass functions when starting up the device.
#define SW_BYPASS_NV    HAL_KEY_SW_5  // Bypass Network layer NV restore
#define SW_BYPASS_START HAL_KEY_SW_1  // Bypass Network initialization

/* LIQUID CRYSTAL DISPLAY DEFINITIONS */
// LCD Support Defintions
#ifdef LCD_SUPPORTED
  #ifdef CC2430EB
    #define LCD_HW  // LCD-hardware
  #endif
  #if LCD_SUPPORTED==DEBUG
    #define LCD_SD  // Serial-debug
  #endif
#else // No LCD support
  #undef LCD_HW  // No hardware
  #undef LCD_SD  // No serial-debug
#endif

/* SERIAL PORT DEFINITIONS */
// Serial Ports ID Codes
#if defined (ZAPP_P1) || defined (ZTOOL_P1)
  #if defined (CC2430EB)
    #define SERIAL_PORT1 HAL_UART_PORT_0
  #elif defined (CC2430DB)
    #define SERIAL_PORT1 HAL_UART_PORT_1
  #endif
#else
  #undef SERIAL_PORT1
#endif

#if defined (ZAPP_P2) || defined (ZTOOL_P2)
  #if defined (CC2430EB)
    #define SERIAL_PORT2 HAL_UART_PORT_0
  #elif defined (CC2430DB)
    #define SERIAL_PORT2 HAL_UART_PORT_1
  #endif
#else
  #undef SERIAL_PORT2
#endif

// Application Serial Port Assignments
#if defined (ZAPP_P1)
  #define ZAPP_PORT SERIAL_PORT1
#elif defined (ZAPP_P2)
  #define ZAPP_PORT SERIAL_PORT2
#else
  #undef ZAPP_PORT
#endif
#if defined (ZTOOL_P1)
  #define ZTOOL_PORT SERIAL_PORT1
#elif defined (ZTOOL_P2)
  #define ZTOOL_PORT SERIAL_PORT2
#else
  #undef ZTOOL_PORT
#endif

// Tx and Rx buffer size defines - max rx/tx = 128 for DMA, else 255 ... unless defining BIG_BUFS.
#define SPI_TX_BUFF_MAX  128   // Required for BUFFER_TEST_RESPONSE
// Min threshold=48 & rxBuf=64 for DMA - probably should move these to hal_board_cfg.h
#define SPI_RX_BUFF_MAX  128
#define SPI_THRESHOLD    48
#define SPI_IDLE_TIMEOUT 6

/* WATCHDOG TIMER DEFINITIONS */
#define WatchDogEnable(wdti) \
{ \
  WDCTL = WDCLP1 | WDEN | (wdti & WDINT); \
  WDCTL = WDCLP2 | WDEN | (wdti & WDINT); \
}

// Restart system from absolute beginning
// Disables interrupts, forces WatchDog reset
#define SystemReset()       \
{                           \
  HAL_DISABLE_INTERRUPTS(); \
  WatchDogEnable( WDTISH ); \
  while (1)  asm("NOP");    \
}

// Jump to F8W Serial Port Bootloader
#define BootLoader()                 \
{                                    \
  HAL_DISABLE_INTERRUPTS();          \
  mboxMsg.BootRead = MBOX_SBL_SHELL; \
  WatchDogEnable( WDTISH );          \
  while (1)  asm("NOP");             \
}

// read boot message to application.
#define READ_BOOT_MBOX(x)       \
{                               \
  x = (uint32)mboxMsg.AppRead;  \
  mboxMsg.BootRead = 0;         \
}

// Wait for specified microseconds
#define MicroWait(t) Onboard_wait(t)

#define OSAL_SET_CPU_INTO_SLEEP(timeout) halSleep(timeout); /* Called from OSAL_PwrMgr */

// Internal (MCU) RAM addresses
#define MCU_RAM_BEG 0xE000
#define MCU_RAM_END 0xFFFF
#define MCU_RAM_LEN (MCU_RAM_END - MCU_RAM_BEG + 1)

// Internal (MCU) Stack addresses
#define CSTK_PTR _Pragma("segment=\"XSP\"") __segment_begin("XSP")
#define CSTK_BEG _Pragma("segment=\"XSTACK\"") __segment_begin("XSTACK")
#define RSTK_END _Pragma("segment=\"ISTACK\"") __segment_end("ISTACK")

// Stack Initialization Value
#define STACK_INIT_VALUE  0xCD

// Internal (MCU) heap size
#if !defined( INT_HEAP_LEN )
  #if defined( ZDO_COORDINATOR )
    #define INT_HEAP_LEN  4096
  #elif defined( RTR_NWK )
    #define INT_HEAP_LEN  3072
  #else
    #define INT_HEAP_LEN  1664
  #endif
#endif

// Memory Allocation Heap
#define MAXMEMHEAP INT_HEAP_LEN  // Typically, 0.70-1.50K

#define KEY_CHANGE_SHIFT_IDX 1
#define KEY_CHANGE_KEYS_IDX  2

// Eval board LCD emulation
#define MAX_LCD_CHARS 16

// Initialization levels
#define OB_COLD  0
#define OB_WARM  1
#define OB_READY 2

#ifdef LCD_SUPPORTED
  #define BUZZER_OFF  0
  #define BUZZER_ON   1
  #define BUZZER_BLIP 2
#endif

typedef struct
{
  osal_event_hdr_t hdr;
  byte             state; // shift
  byte             keys;  // keys
} keyChange_t;

/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * FUNCTIONS
 */

  /*
   * Initialize the Peripherals
   *    level: 0=cold, 1=warm, 2=ready
   */
  extern void InitBoard( byte level );

 /*
  * Get elapsed timer clock counts
  */
  extern uint32 TimerElapsed( void );

  /*
   * Register for all key events
   */
  extern byte RegisterForKeys( byte task_id );

/* Keypad Control Functions */

  /*
   * Send "Key Pressed" message to application
   */
  extern byte OnBoard_SendKeys(  byte keys, byte shift);

  /*
   * Read the keyboard on eval board.
   */
  extern byte OnBoard_GetKeys( void );


/* LCD Emulation/Control Functions */
  /*
   * Convert an interger to an ascii string
   */
  extern void _itoa(uint16 num, byte *buf, byte radix);


  extern void Dimmer( byte lvl );

/* External I/O Processing Functions */
  /*
   * Turn on an external lamp
   */
  extern void BigLight_On( void );

  /*
   * Turn off an external lamp
   */
  extern void BigLight_Off( void );

  /*
   * Turn on/off an external buzzer
   *   on:   BUZZER_ON or BUZZER_OFF
   */
  extern void BuzzerControl( byte on );

  /*
   * Get setting of external dip switch
   */
  extern byte GetUserDipSw( void );

  /*
   * Calculate the size of used stack
   */
  extern uint16 OnBoard_stack_used( void );

 /*
  * Callback function to handle timer
  */
  extern void Onboard_TimerCallBack ( uint8 timerId, uint8 channel, uint8 channelMode);

  /*
   * Callback routine to handle keys
   */
  extern void OnBoard_KeyCallback ( uint8 keys, uint8 state );

  /*
   * Board specific random number generator
   */
  extern uint16 Onboard_rand( void );

  /*
   * Board specific micro-second wait
   */
  extern void Onboard_wait( uint16 timeout );

/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif // ONBOARD_H
