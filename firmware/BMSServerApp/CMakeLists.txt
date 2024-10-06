cmake_minimum_required(VERSION 3.16)

# Specify the toolchain file (should be before project())
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/toolchain.cmake)

# Project name and version
project(BMSServerApp VERSION 1.0)

# Enable assembly language (for the startup file)
enable_language(C ASM)

# Option for TEST build
option(TEST "Build with TEST mode enabled" OFF)

# Include directories
include_directories(
    ${CMAKE_SOURCE_DIR}/include
)

# Source files
file(GLOB SOURCES ${CMAKE_SOURCE_DIR}/src/*.c)

# Add startup assembly file
set(STARTUP_FILE ${CMAKE_SOURCE_DIR}/startup/startup_stm32l152xe.s)

# Create an executable
add_executable(${PROJECT_NAME}.elf ${SOURCES} ${STARTUP_FILE})

# Linker script (optional, if you have one)
set(LINKER_SCRIPT ${CMAKE_SOURCE_DIR}/linker_script.ld)
target_link_options(${PROJECT_NAME}.elf PRIVATE "-T${LINKER_SCRIPT}")

# Define TEST macro if TEST mode is enabled
if(TEST)
    message(STATUS "Building in TEST mode")
    target_compile_definitions(${PROJECT_NAME}.elf PRIVATE -DTEST)
endif()

# Create a binary file after the executable is built
add_custom_command(TARGET ${PROJECT_NAME}.elf POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:${PROJECT_NAME}.elf> ${PROJECT_NAME}.bin
    COMMENT "Generating binary file ${PROJECT_NAME}.bin"
)
