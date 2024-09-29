# STM32 CI/CD Build Workflow

This repository contains the source code for the STM32 project along with a Continuous Integration (CI) pipeline using GitHub Actions. The pipeline is designed to automate the build process for STM32 firmware using CMake, Ninja, and the ARM GCC toolchain (`arm-none-eabi-gcc`).

## Features

- **Automated builds** for STM32 firmware on each push and pull request.
- Builds for **Windows-based environments** using the `arm-none-eabi-gcc` toolchain.
- Supports **Debug** and **Release** build configurations.
- Uses **CMake** and **Ninja** for efficient and reliable builds.
- Output binary (`.bin` file) is automatically generated and copied to the appropriate directory.

## Workflow Overview

The GitHub Actions workflow is defined in the `.github/workflows/build.yml` file. It performs the following steps:

1. **Checkout the source code** from the repository.
2. **Install the ARM GCC toolchain** for cross-compiling the firmware.
3. **Set up Ninja** as the build system.
4. **Configure the build** using CMake with the specified toolchain file (`toolchain.cmake`).
5. **Compile the project** using Ninja in both **Debug** and **Release** modes.
6. **Copy the binary output** (`.bin` file) from the build directory to the project root.

## Triggering the Workflow

The workflow is triggered automatically on:

- **Pushes** to the `SCRUM-16-Inspection-CI-CD-pipe-intergration-build-automation-in-github` branch.
- **Pull requests** targeting the same branch.

## Toolchain Information

The project uses the **GNU Arm Embedded Toolchain** (`arm-none-eabi-gcc`). This toolchain allows cross-compilation of embedded applications for Arm Cortex-M microcontrollers.

- **Toolchain Version**: `10.3-2021.10`
- **Source**: Installed using the GitHub Action [`ryanwinter/arm-none-eabi-gcc`](https://github.com/marketplace/actions/gnu-arm-embedded-toolchain).

## Prerequisites

Before running the workflow locally or contributing to the project, ensure you have the following installed:

- **ARM GCC Toolchain**: [Download](https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm)
- **CMake**: [Download](https://cmake.org/download/)
- **Ninja Build System**: [Download](https://ninja-build.org/)

## Building Locally can navigate to firmware directory instruction.

## References:
1. https://ryanwinter.org/creating-a-github-workflow-for-the-gnu-arm-compiler/
2. https://github.com/ryanwinter/arm-none-eabi-gcc?tab=readme-ov-file
