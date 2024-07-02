cmake_minimum_required(VERSION 3.13)
set(PICO_SDK_PATH /Users/attilio/.pico-sdk/sdk/1.5.1)
set(PICO_TOOLCHAIN_PATH /Users/attilio/.pico-sdk/toolchain/13_2_Rel1)
set(CMAKE_BUILD_TYPE Debug)


include(./pico_sdk_import.cmake)

project(kernel C CXX ASM)
add_compile_options(-DPICO_CMSIS_RENAME_EXCEPTIONS=1)

pico_sdk_init()

add_executable(kernel
    kernel.c
    task.c
    assembly.s
)

# set(RUST_LIB_PATH "${CMAKE_SOURCE_DIR}/rust_lib/target/thumbv6m-none-eabi/release")
# set(LIB_EXCEPTIONS "/Users/attilio/Desktop/MA2-INFO/Memoire/Brol/pico-sdk/src/rp2_common/hardware_exception/include/hardware/exception.h")
# set(RUST_LIB "rust_lib")

target_link_libraries(kernel pico_stdlib 
# ${RUST_LIB_PATH}/lib${RUST_LIB}.a
)

pico_enable_stdio_usb(kernel 0)
pico_enable_stdio_uart(kernel 1)

pico_add_extra_outputs(kernel)
