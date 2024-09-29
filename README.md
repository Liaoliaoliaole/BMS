# BMS

# Project Structure

This repository contains the following directories to organize various aspects of the Battery Management System (BMS) project:

# `db`
This directory will house all database-related files.

# `firmware`
The firmware directory includes the BMSServerApp source code. This application runs on the STM32L152RE Nucleo development board.   

## Cross-compilation 
### Preconditions:
On Windows:   
- CMake
- Ninja
- STSW-LINK007
- GCC ARM None EABI Compiler
- 
Details of the environment setup can be found in Confluence: [How-To-CrossCompile-BMSServerApp](https://liyuanliull.atlassian.net/wiki/spaces/BMSDEVELOP/pages/426109/How-to+Set+Up+Environment+and+Building+the+BMSServerApp)

### Build scripts
Navigate to the directory where build script existed.   
Execute following command in terminal(such as windows powershell):  
`.\build_BMSServerApp.ps1`   
  
Then connect the stm32l152re to PC, and run:    
`st-flash write BMSServerApp\build\stm32l152_nucleo.bin 0x08000000`   

To remove the build, remove `build` directory.

## GitHub Actions Automation
The BMSServerApp project also supports automated builds via GitHub Actions. This allows you to build the project in the cloud whenever changes are pushed to the specified branch or a pull request is made.

### How to Use GitHub Actions: 
Trigger a Build: Any push or pull request to this branch will automatically trigger the build process.
Check Build Status: Monitor the status of the build in the "Actions" tab of the GitHub repository.
Find the Output: After the build completes, check the logs to find the output binary files. If the binary is generated successfully, it will be located in the firmware/BMSServerApp/build directory of the repository.

# `prototyping`
This section will store the code, configurations, and other resources related to early-stage project prototyping. It will be used to experiment with features, develop proof-of-concept implementations, and try out ideas before they are moved into production.

# `schematic`
The schematic directory will include electrical and hardware design diagrams, such as circuit layouts, component schematics, and PCB designs. Any files related to the hardware structure will be stored here.

# `simulation`
This folder will contain simulation models, scripts, and testing environments used for validating the BMS design and performance before physical implementation. Files related to simulation tools, test cases, and results will be stored here.

### `test`
The test directory will store all unit tests, integration tests, and hardware verification tests. Automated test scripts, test reports, and any testing frameworks will be organized here to maintain the quality of the project code.

# `views`
This directory is for the graphical user interface (GUI) and front-end design files for the BMS project, including visual representations and user interaction designs.
