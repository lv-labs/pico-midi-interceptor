#include <stdio.h>
#include <stdarg.h>

#include "bsp/board_api.h"
#include "tusb.h"

#include "usb_midi.h"
#include "trs_midi.h"

static void midi_received_callback(const uint8_t *packet)
{
    trs_midi_send_packet(packet);
}

int main(void)
{
    board_init();

    usb_midi_init(2);
    trs_midi_init(uart0, 0, 31250);
    usb_midi_set_callback(midi_received_callback);

    while (1)
    {
        usb_midi_task();
        tight_loop_contents();
    }

    return 0;
}