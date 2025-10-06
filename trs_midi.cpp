#include "trs_midi.h"
#include "hardware/uart.h"
#include "hardware/gpio.h"

static uart_inst_t *midi_uart = NULL;

// extract MIDI message length from USB MIDI Code Index Number (CIN)
static uint8_t get_midi_message_length(uint8_t cin)
{
    cin &= 0x0F; // Get lower nibble

    switch (cin)
    {
    case 0x0:
        return 0; // reserved
    case 0x1:
        return 0; // reserved
    case 0x2:
        return 2; // two-byte system common (e.g., MTC, song select)
    case 0x3:
        return 3; // three-byte system common (e.g., SPP)
    case 0x4:
        return 3; // sysex starts or continues
    case 0x5:
        return 1; // single-byte system common or sysex ends
    case 0x6:
        return 2; // sysex ends with following two bytes
    case 0x7:
        return 3; // sysex ends with following three bytes
    case 0x8:
        return 3; // note-off
    case 0x9:
        return 3; // note-on
    case 0xA:
        return 3; // poly-keypress
    case 0xB:
        return 3; // control change
    case 0xC:
        return 2; // program change
    case 0xD:
        return 2; // channel pressure
    case 0xE:
        return 3; // pitchBend change
    case 0xF:
        return 1; // single byte
    default:
        return 0;
    }
}

void trs_midi_init(uart_inst_t *uart, uint tx_pin, uint32_t baud)
{
    midi_uart = uart;
    uart_init(uart, baud);
    gpio_set_function(tx_pin, GPIO_FUNC_UART);
    uart_set_fifo_enabled(uart, true);
}

void trs_midi_send_packet(const uint8_t *usb_packet)
{
    if (!midi_uart || !usb_packet)
        return;

    uint8_t cin = usb_packet[0] & 0x0F;
    uint8_t msg_len = get_midi_message_length(cin);

    if (msg_len > 0)
    {
        // send the actual MIDI bytes (skip USB packet header at byte 0)
        uart_write_blocking(midi_uart, &usb_packet[1], msg_len);
    }
}

void trs_midi_send_bytes(const uint8_t *data, uint8_t length)
{
    if (!midi_uart || !data || length == 0)
        return;

    uart_write_blocking(midi_uart, data, length);
}