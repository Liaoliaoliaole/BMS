# BMS

# Project Structure

This repository contains the following directories to organize various aspects of the Battery Management System (BMS) project:

# `firmware`
The firmware directory includes the BMSServerApp source code. This application runs on the STM32L152RE Nucleo development board.   

## Cross-compilation
### Preconditions:
On Windows:   
- CMake
- Ninja
- STSW-LINK007
- GCC ARM None EABI Compiler

Details of the environment setup can be found in Confluence: [How-To-CrossCompile-BMSServerApp](https://liyuanliull.atlassian.net/wiki/spaces/BMSDEVELOP/pages/426109/How-to+Set+Up+Environment+and+Building+the+BMSServerApp)

### Build scripts
Navigate to the directory where the build script exists.   
Execute the following command in the terminal (such as Windows PowerShell):  
`./build_BMSServerApp.ps1 [-TestBuild]`   

- **Test Build**: Add the optional `-TestBuild` flag to include the testing features in your build, which will enable mock components or extra diagnostics during testing. For example:  
  `./build_BMSServerApp.ps1 -TestBuild`

Then connect the STM32L152RE to your PC, and run:    
`st-flash write BMSServerApp\build\BMSServerApp.bin 0x08000000`   

To remove the build, remove the `build` directory.

## GitHub Actions for CI/CD
The BMSServerApp project also supports automated builds via GitHub Actions. This allows you to build the project in the cloud whenever changes are pushed to the specified branch or a pull request is made.

### Build Types in GitHub Actions:
- The workflow is configured to build three different types: **Debug**, **Release**, and **Test**.
- The **Test** build defines a special `TEST` macro during compilation, enabling mock data and diagnostic features.

### How to Use GitHub Actions:
- **Trigger a Build**: Any push or pull request to the specified branch will automatically trigger the build process.
- **Check Build Status**: Monitor the status of the build in the "Actions" tab of the GitHub repository.
- **Find the Output**:   
  1. Click on the "Actions" tab.
  2. Select the Workflow Run: Find the specific workflow run to view its details.
  3. Scroll down to the "Artifacts" section on the workflow run page. This section shows the files that were uploaded during the workflow execution.
  4. Download the Binary File:  Click on the link for the binary file you want to download (e.g., `BMSServerApp-Debug-bin`, `BMSServerApp-Release-bin`, or `BMSServerApp-Test-bin`).   

# `prototyping`
This section will store the code, configurations, and other resources related to early-stage project prototyping. It will be used to experiment with features, develop proof-of-concept implementations, and try out ideas before they are moved into production.

# `schematic`
The schematic directory will include electrical and hardware design diagrams, such as circuit layouts, component schematics, and PCB designs. Any files related to the hardware structure will be stored here.

# `simulation`
This folder will contain simulation models, scripts, and testing environments used for validating the BMS design and performance before physical implementation. Files related to simulation tools, test cases, and results will be stored here.

# `views`
This directory is for the graphical user interface (GUI) and front-end design files for the BMS project, including visual representations and user interaction designs.
