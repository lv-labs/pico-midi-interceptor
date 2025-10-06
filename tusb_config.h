#ifndef _TUSB_CONFIG_H_
#define _TUSB_CONFIG_H_

#ifdef __cplusplus
extern "C"
{
#endif

//--------------------------------------------------------------------+
// Board Specific Configuration
//--------------------------------------------------------------------+

// RHPort number used for host can be defined by board.mk, default to port 0
// #ifndef BOARD_TUH_RHPORT
#define BOARD_TUH_RHPORT 1 //// Set PORT for HUB - Pico PIO USB
// #endif

// RHPort max operational speed can defined by board.mk
#ifndef BOARD_TUH_MAX_SPEED
#define BOARD_TUH_MAX_SPEED OPT_MODE_DEFAULT_SPEED
#endif

// RHPort number used for device can be defined by board.mk, default to port 0
#ifndef BOARD_TUD_RHPORT
#define BOARD_TUD_RHPORT 0 //// Set POR for DEVICE
#endif

// RHPort max operational speed can defined by board.mk
#ifndef BOARD_TUD_MAX_SPEED
#define BOARD_TUD_MAX_SPEED OPT_MODE_DEFAULT_SPEED
#endif

//--------------------------------------------------------------------
// COMMON CONFIGURATION
//--------------------------------------------------------------------

// defined by compiler flags for flexibility
#ifndef CFG_TUSB_MCU
#error CFG_TUSB_MCU must be defined
#endif

#ifndef CFG_TUSB_OS
#define CFG_TUSB_OS OPT_OS_NONE
#endif

#ifndef CFG_TUSB_DEBUG
#define CFG_TUSB_DEBUG 0
#endif

// Enable Device stack, Default is max speed that hardware controller could support with on-chip PHY
#define CFG_TUD_ENABLED 1 //// Enable DEVICE
#define CFG_TUD_MAX_SPEED BOARD_TUD_MAX_SPEED

// Enable Host stack
#define CFG_TUH_ENABLED 1 //// Enable HOST
#define CFG_TUH_MAX_SPEED BOARD_TUH_MAX_SPEED

#if CFG_TUSB_MCU == OPT_MCU_RP2040
// Use pico-pio-usb as host controller for raspberry rp2040
#define CFG_TUH_RPI_PIO_USB 1 //// Enable Pico PIO USB
#endif

// Default is max speed that hardware controller could support with on-chip PHY
#define CFG_TUH_MAX_SPEED BOARD_TUH_MAX_SPEED

/* USB DMA on some MCUs can only access a specific SRAM region with restriction on alignment.
 * Tinyusb use follows macros to declare transferring memory so that they can be put
 * into those specific section.
 * e.g
 * - CFG_TUSB_MEM SECTION : __attribute__ (( section(".usb_ram") ))
 * - CFG_TUSB_MEM_ALIGN   : __attribute__ ((aligned(4)))
 */
#ifndef CFG_TUSB_MEM_SECTION
#define CFG_TUSB_MEM_SECTION
#endif

#ifndef CFG_TUSB_MEM_ALIGN
#define CFG_TUSB_MEM_ALIGN __attribute__((aligned(4)))
#endif

    //--------------------------------------------------------------------
    // DEVICE CONFIGURATION
    //--------------------------------------------------------------------

#ifndef CFG_TUD_ENDPOINT0_SIZE
#define CFG_TUD_ENDPOINT0_SIZE 64
#endif

//------------- CLASS -------------//
#define CFG_TUD_CDC 1
#define CFG_TUD_MSC 0
#define CFG_TUD_HID 0
#define CFG_TUD_MIDI 1 //// To get MIDI device class
#define CFG_TUD_VENDOR 0

#define CFG_TUD_CDC_RX_BUFSIZE 64
#define CFG_TUD_CDC_TX_BUFSIZE 64

// MIDI FIFO size of TX and RX
#define CFG_TUD_MIDI_RX_BUFSIZE 512
#define CFG_TUD_MIDI_TX_BUFSIZE 512

//--------------------------------------------------------------------
// HOST CONFIGURATION
//--------------------------------------------------------------------

// Size of buffer to hold descriptors and other data used for enumeration
#define CFG_TUH_ENUMERATION_BUFSIZE 256

#define CFG_TUH_HUB 1 //// Allow for a USB Hub

// max device support (excluding hub device)
// 1 hub typically has 4 ports
#define CFG_TUH_DEVICE_MAX (CFG_TUH_HUB ? 4 : 1)

// Max endpoint per device
#define CFG_TUH_ENDPOINT_MAX 8

// Enable tuh_edpt_xfer() API
#define CFG_TUH_API_EDPT_XFER 1

#ifdef __cplusplus
}
#endif

#endif /* _TUSB_CONFIG_H_ */
