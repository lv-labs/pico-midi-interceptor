#ifndef TRS_MIDI_H
#define TRS_MIDI_H

#include <stdint.h>
#include "hardware/uart.h"

// initialise trs midi output
void trs_midi_init(uart_inst_t *uart, uint tx_pin, uint32_t baud);

// send a usb midi packet to trs midi (extracts actual midi bytes)
void trs_midi_send_packet(const uint8_t *usb_packet);

// Send raw midi bytes directly
void trs_midi_send_bytes(const uint8_t *data, uint8_t length);

#endif // TRS_MIDI_H