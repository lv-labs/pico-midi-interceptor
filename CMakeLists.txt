# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.2.0)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.2.0)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico2 CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(pico-midi-interceptor C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# add pico_pio_usb files
set(PICO_PIO_USB_SRC ${CMAKE_CURRENT_SOURCE_DIR}/include/pico_pio_usb)

# Add executable. Default name is the project name, version 0.1

add_executable(pico-midi-interceptor main.cpp
    usb_midi.cpp
    trs_midi.cpp
    usb_descriptors.c
    
    ${PICO_PIO_USB_SRC}/pio_usb.c
    ${PICO_PIO_USB_SRC}/pio_usb_host.c
    ${PICO_PIO_USB_SRC}/pio_usb_device.c
    ${PICO_PIO_USB_SRC}/usb_crc.c 

    ${PICO_SDK_PATH}/lib/tinyusb/src/portable/raspberrypi/pio_usb/hcd_pio_usb.c
    ${PICO_SDK_PATH}/lib/tinyusb/src/portable/raspberrypi/pio_usb/dcd_pio_usb.c)

pico_generate_pio_header(pico-midi-interceptor ${PICO_PIO_USB_SRC}/usb_tx.pio)
pico_generate_pio_header(pico-midi-interceptor ${PICO_PIO_USB_SRC}/usb_rx.pio)

pico_set_program_name(pico-midi-interceptor "pico-midi-interceptor")
pico_set_program_version(pico-midi-interceptor "0.1")

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(pico-midi-interceptor 0)
pico_enable_stdio_usb(pico-midi-interceptor 0)

# Add the standard library to the build
target_link_libraries(pico-midi-interceptor
        pico_stdlib
        tinyusb_board
        tinyusb_device
        tinyusb_host
        hardware_pio
        hardware_dma
        hardware_uart)

# Add the standard include files to the build
target_include_directories(pico-midi-interceptor PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
        ${PICO_PIO_USB_SRC}
)

# add_compile_definitions(PICO_CONFIG_USB_DEVICE_MIDI=1)


pico_add_extra_outputs(pico-midi-interceptor)

