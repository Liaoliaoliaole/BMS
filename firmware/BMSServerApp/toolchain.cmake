# Toolchain file for ARM Cortex-M3 (STM32L1xx)

# target architecture
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

# Set the cross-compiler toolchain
set(CMAKE_C_COMPILER arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER arm-none-eabi-g++)
set(CMAKE_OBJCOPY arm-none-eabi-objcopy)

# Set the compiler flags
set(CMAKE_C_FLAGS "-mcpu=cortex-m3 -mthumb -std=gnu99 -Wall -O2 -ffreestanding")
set(CMAKE_EXE_LINKER_FLAGS "-T${CMAKE_SOURCE_DIR}/linker/stm32_flash.ld -Wl,--gc-sections")

# Add definitions
add_definitions(-DSTM32L152xE)

# Include directories
include_directories(
    ${CMAKE_SOURCE_DIR}/Drivers/CMSIS/Include
    ${CMAKE_SOURCE_DIR}/Drivers/CMSIS/Device/ST/STM32L1xx/Include
)
