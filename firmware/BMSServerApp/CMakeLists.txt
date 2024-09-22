cmake_minimum_required(VERSION 3.16)

# Project name and version
project(BMSServerApp VERSION 1.0)

# Enable assembly language (for the startup file)
enable_language(ASM)

# Specify the toolchain file
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/toolchain.cmake)

# Include directories
include_directories(
    ${CMAKE_SOURCE_DIR}/include
)

file(GLOB SOURCES ${CMAKE_SOURCE_DIR}/src/*.c)

# Add startup assembly file
set(STARTUP_FILE ${CMAKE_SOURCE_DIR}/startup/startup_stm32l152xe.s)

# Create an executable
add_executable(${PROJECT_NAME}.elf ${SOURCES} ${STARTUP_FILE})

# Create a binary file after the executable is built
add_custom_command(TARGET ${PROJECT_NAME}.elf POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:${PROJECT_NAME}.elf> ${PROJECT_NAME}.bin
    COMMENT "Generating binary file ${PROJECT_NAME}.bin"
)
