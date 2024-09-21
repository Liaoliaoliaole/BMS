# BMS

# Project Structure

This repository contains the following directories to organize various aspects of the Battery Management System (BMS) project:

# `db`
This directory will house all database-related files.

# `firmware`
bms_project directory include BMSServerApp source code.  
It is application running on STM32L152RE Nucleo development board.
## Cross-compilation 
### Preconditions:
In windows :  
cmake, ninja, stsw-link007,gcc-arm-none-eabi compiler are installed.
Details of enviroment set could be seen in Confluence: (TODO)   
### Build steps
Execute following command in terminal(such as windows powershell) under the directory where CMakeLists.txt existed:  
`mkdir build`   
`cd build`   
`cmake -DCMAKE_TOOLCHAIN_FILE="../toolchain.cmake" -G "Ninja" ..`   
`ninja`  
If clean the build, run: `ninja clean`   
Then connect the stm32l152re to PC, and run:   
`st-flash write stm32l152_nucleo.bin 0x08000000`

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
