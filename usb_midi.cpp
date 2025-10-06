#include "usb_midi.h"
#include <string.h>
#include "bsp/board_api.h"
#include "tusb.h"
#include "pio_usb.h"

// host descriptor info
static uint8_t in_endpt = 0;

// usb host transfer buffer
static uint8_t in_buf[64];

// midi message callback
static usb_midi_callback_t midi_callback = NULL;

// midi buffer
#define MIDI_BUFFER_SIZE 64

typedef struct
{
    uint8_t packet[4];
} midi_packet_t;

static midi_packet_t midi_buffer[MIDI_BUFFER_SIZE];
static volatile uint8_t midi_write_idx = 0;
static volatile uint8_t midi_read_idx = 0;

static inline bool midi_buffer_full(void)
{
    return ((midi_write_idx + 1) % MIDI_BUFFER_SIZE) == midi_read_idx;
}

static inline bool midi_buffer_empty(void)
{
    return midi_write_idx == midi_read_idx;
}

static inline bool forward_to_device(const uint8_t *packet)
{
    if (!tud_midi_mounted())
        return false;

    // wait until tinyusb can accept 4 bytes
    while (tud_midi_available() < 4)
    {
        tud_task(); // let tinyusb send previous data
        sleep_us(100);
    }

    tud_midi_stream_write(0, packet, 4);
    return true;
}

// callback for when data is recived on usb host port
static void data_received(tuh_xfer_t *xfer)
{
    uint8_t *buf = (uint8_t *)xfer->user_data;

    if (xfer->result == XFER_RESULT_SUCCESS)
    {
        // each usb-midi packet is 4 bytes (cable + CIN + 3 data bytes)
        for (uint32_t i = 0; i < xfer->actual_len; i += 4)
        {
            uint8_t cin = buf[i] & 0x0F;
            if (cin == 0)
                continue; // skip empty/invalid slots

            if (!midi_buffer_full())
            {
                memcpy(midi_buffer[midi_write_idx].packet, &buf[i], 4);
                midi_write_idx = (midi_write_idx + 1) % MIDI_BUFFER_SIZE;
            }
        }
    }

    // resubmit transfer so the host keeps streaming midi in traffic
    xfer->buflen = sizeof(in_buf);
    xfer->buffer = in_buf;
    tuh_edpt_xfer(xfer);
}

void tuh_mount_cb(uint8_t daddr)
{
    uint8_t tmp[256];
    if (tuh_descriptor_get_configuration_sync(daddr, 0, tmp, sizeof(tmp)) != XFER_RESULT_SUCCESS)
    {
        return;
    }

    tusb_desc_configuration_t const *cfg =
        (tusb_desc_configuration_t const *)tmp;
    uint8_t const *p = (uint8_t const *)cfg;
    uint8_t const *end = p + tu_le16toh(cfg->wTotalLength);

    while (p < end)
    {
        if (tu_desc_type(p) == TUSB_DESC_ENDPOINT)
        {
            tusb_desc_endpoint_t const *ep =
                (tusb_desc_endpoint_t const *)p;
            if (tu_edpt_dir(ep->bEndpointAddress) == TUSB_DIR_IN &&
                tuh_edpt_open(daddr, ep))
            {
                in_endpt = ep->bEndpointAddress;
                static tuh_xfer_t xfer;
                xfer = (tuh_xfer_t){
                    .daddr = daddr,
                    .ep_addr = in_endpt,
                    .buflen = sizeof(in_buf),
                    .buffer = in_buf,
                    .complete_cb = data_received,
                    .user_data = (uintptr_t)in_buf,
                };
                tuh_edpt_xfer(&xfer);
                break;
            }
        }
        p = tu_desc_next(p);
    }
}

void usb_midi_init(uint8_t host_dp_pin)
{
    // configure PIO USB for host
    pio_usb_configuration_t pio_cfg = PIO_USB_DEFAULT_CONFIG;
    pio_cfg.pin_dp = host_dp_pin;
    tuh_configure(1, TUH_CFGID_RPI_PIO_USB_CONFIGURATION, &pio_cfg);

    // initialise host and device stacks
    tuh_init(BOARD_TUH_RHPORT);
    tud_init(BOARD_TUD_RHPORT);
}

void usb_midi_set_callback(usb_midi_callback_t callback)
{
    midi_callback = callback;
}

void usb_midi_process(void)
{
    if (midi_buffer_empty())
    {
        return;
    }

    while (!midi_buffer_empty())
    {
        uint8_t i = midi_read_idx;
        // try to send; if device FIFO full, stop for now
        if (!forward_to_device(midi_buffer[i].packet))
            break;

        // local copy to TRS or other callback
        if (midi_callback)
            midi_callback(midi_buffer[i].packet);

        // advance read index once it's sent
        midi_read_idx = (i + 1) % MIDI_BUFFER_SIZE;
    }
}

void usb_midi_task(void)
{
    tuh_task();
    tud_task();
    usb_midi_process();
}
