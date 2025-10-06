#ifndef USB_MIDI_H
#define USB_MIDI_H

#include <stdint.h>
#include <stdbool.h>

// Callback type for MIDI message notifications
typedef void (*usb_midi_callback_t)(const uint8_t *packet);

// Initialize USB MIDI host and device
void usb_midi_init(uint8_t host_dp_pin);

// Set callback for received MIDI messages
void usb_midi_set_callback(usb_midi_callback_t callback);

// Task functions - call these in main loop
void usb_midi_task(void);

#endif // USB_MIDI_H