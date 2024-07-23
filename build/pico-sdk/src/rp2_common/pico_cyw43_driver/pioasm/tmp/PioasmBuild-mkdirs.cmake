# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

# If CMAKE_DISABLE_SOURCE_CHANGES is set to true and the source directory is an
# existing directory in our source tree, calling file(MAKE_DIRECTORY) on it
# would cause a fatal error, even though it would be a no-op.
if(NOT EXISTS "/Users/attilio/.pico-sdk/sdk/1.5.1/tools/pioasm")
  file(MAKE_DIRECTORY "/Users/attilio/.pico-sdk/sdk/1.5.1/tools/pioasm")
endif()
file(MAKE_DIRECTORY
  "/Users/attilio/Desktop/Code/rp2040_kernel/build/pioasm"
  "/Users/attilio/Desktop/Code/rp2040_kernel/build/pico-sdk/src/rp2_common/pico_cyw43_driver/pioasm"
  "/Users/attilio/Desktop/Code/rp2040_kernel/build/pico-sdk/src/rp2_common/pico_cyw43_driver/pioasm/tmp"
  "/Users/attilio/Desktop/Code/rp2040_kernel/build/pico-sdk/src/rp2_common/pico_cyw43_driver/pioasm/src/PioasmBuild-stamp"
  "/Users/attilio/Desktop/Code/rp2040_kernel/build/pico-sdk/src/rp2_common/pico_cyw43_driver/pioasm/src"
  "/Users/attilio/Desktop/Code/rp2040_kernel/build/pico-sdk/src/rp2_common/pico_cyw43_driver/pioasm/src/PioasmBuild-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/Users/attilio/Desktop/Code/rp2040_kernel/build/pico-sdk/src/rp2_common/pico_cyw43_driver/pioasm/src/PioasmBuild-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/Users/attilio/Desktop/Code/rp2040_kernel/build/pico-sdk/src/rp2_common/pico_cyw43_driver/pioasm/src/PioasmBuild-stamp${cfgdir}") # cfgdir has leading slash
endif()
