# BMS

# Project Structure

This repository contains the following directories to organize various aspects of the Battery Management System (BMS) project:
Meeting note and scrum report for scrum could be found from [Jira](https://liyuanliull.atlassian.net/jira/software/projects/SCRUM/boards/1/reports).

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
This directory contains code, configurations, and other resources related to early-stage prototyping of the BMS project. It is used for experimenting with features, creating proof-of-concept implementations, and testing ideas before moving to production.  

- **Breadboard Prototyping**: The breadboard setup for the [current measurement module](https://liyuanliull.atlassian.net/wiki/spaces/BMSDEVELOP/pages/6520833/Voltage+Temperature+and+Current+measurement+Overview) and [temperature measurement prototypes](https://liyuanliull.atlassian.net/wiki/spaces/BMSDEVELOP/pages/8257538/Temperature+Measurement+Module+Prototyping+Documentation.) are stored in confluence.   
- **Calibration Functions**: Software calibration functions are developed to accommodate real-world measurements from the hardware prototypes, especially for [current mesurement](https://liyuanliull.atlassian.net/wiki/spaces/BMSDEVELOP/pages/19791874/Current+measurement+prototyping+and+calibration+calculation).   
Detailed documentation can be found in Confluence.  

# `schematic`
The schematic directory contains electrical and hardware design files, including circuit layouts, component schematics, and PCB designs. These designs are created using KiCad 7.0 and include PCB design files as well. All hardware-related files are stored here for reference.   

# `simulation`
This folder contains simulation models, scripts, and testing environments used for validating the BMS design and performance before physical implementation. Files related to simulation tools, test cases, and results are stored here.   

LTSpice Simulations: Simulation files created in LTSpice are used to validate the electrical design prior to prototyping.   

# `test`
This directory contains both hardware and software test cases and configurations for validating the BMS system.   

**Hardware Testing**: Includes [test cases](https://liyuanliull.atlassian.net/wiki/spaces/BMSDEVELOP/pages/20250626/Hardware+testing+notes) and documentation for validating the physical components of the BMS system: mosfet board, main board, current measurement board and temperature measurement board.   

**Software Testing**: The software testing for the BMSServerApp and BMSClientApp was conducted using several approaches to ensure robust functionality and communication. Key testing scenarios include:     
- ADC Converter Testing: Simulated mock ADC values to validate the proper functioning of the ADC conversion process in the software, ensuring accurate data handling.   
- Modbus Communication Testing: Tested Modbus function code 04 to verify reliable communication between the server and client, ensuring proper data exchange over the Modbus protocol.   
- Grafana Visualization: Validated the integration with Grafana by verifying the display of MCU-sent values on the Grafana dashboard, ensuring accurate and real-time monitoring of system metrics.   
- MOSFET Control Scenarios: Tested various MOSFET control scenarios by simulating different voltage, current, and temperature conditions. This validated the software's ability to control MOSFETs based on these parameters, ensuring correct operation under varying environmental and system conditions.

Detailed results of these hardware and software tests can be found in test dir README.

# `views`
The BMS project uses Grafana for visualization. A short documentation on how to log in and set up Grafana dashboards for monitoring BMS metrics is provided.

### How to Use Grafana

**Logging in as Guest**
1. Access Grafana: Open your web browser and navigate to the [Grafana URL](http://16.16.145.115:3000/login).
2. Guest Login: On the login page use username *Grafana_Viewer* and password *8qr8MzZM*.
3. Browse Dashboards: As a guest, you will have read-only access to the dashboards. You can browse through the available visualizations, interact with the data by zooming in on graphs, and apply filters to view specific data ranges.

**Logging in as Admin**
1. Access Grafana: Open your web browser and navigate to the [Grafana URL](http://16.16.145.115:3000/login).
2. Admin Login: Enter the admin username and password. You can obtain this data on request.
3. Admin Dashboard: Once logged in, you will have full access to all Grafana features, including the ability to create, edit, and delete dashboards, manage data sources, and configure system settings.

**Adding New Visualizations**
1. Add a Panel: Click on "Add new panel" to start creating a new visualization.
2. Select Data Source: Choose the data source from which you want to pull data. This is BMS database in our case.
3. Configure Query: Write the query to fetch the data you need. 
4. Choose Visualization Type: Select the type of visualization you want to create, such as a graph, table or heatmap.
5. Save the Panel: Once you are satisfied with the configuration, click "Apply" to add the panel to your dashboard. Don't forget to save the dashboard itself to retain your changes.

### Testing

**For Test Purposes with Static Values Compile BMSServerApp (STM32) with Symbol TEST**

1. Open the BMSServerApp project in your IDE.
2. Locate the project settings where preprocessor symbols are defined. This is typically found under project properties or build settings.
3. Add the symbol `TEST` to the list of preprocessor definitions.
4. Build the project. This will generate the firmware binary with the test mode enabled.
5. Flash the compiled firmware onto the STM32 microcontroller using a programmer or debugger tool.
6. With the test firmware running, the BMSServerApp will use predefined static values instead of real sensor data, allowing you to test the system's functionality without needing actual sensor inputs.

**For Testing Grafana Without Using STM32 Run `test_data_generator.py` from Views Folder**
