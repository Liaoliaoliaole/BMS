## Test mosfet control logic in PC
Compile:   
`gcc .\test_mosfet.c .\mosfet_control.c -o test`   
Run:   
`./test`   
Should see:   

##### Test Case 1: MOSFET State Control with Sensor Values #####   
Setting Charge MOSFET: OFF (Internal state: 0)   
Setting Discharge MOSFET: OFF   
Debounce Passed: 0   
High current detected. Setting body diode protection flag.   
Debounce Passed: 1   
High current detected. Setting body diode protection flag.   
Setting Charge MOSFET: ON (Internal state: 1)   
Setting Discharge MOSFET: ON   
NORMAL_OPERATION: Charge ON, Discharge ON.   
#####   
Debounce Passed: 0   
High current detected. Setting body diode protection flag.   
Temperature out of range. Setting fault flag.   
Debounce Passed: 1   
High current detected. Setting body diode protection flag.   
Temperature out of range. Setting fault flag.   
Setting Charge MOSFET: OFF (Internal state: 0)   
Setting Discharge MOSFET: OFF   
BOTH_OFF: Charge OFF, Discharge OFF.   
#####   
Debounce Passed: 0   
High current detected. Setting body diode protection flag.   
Debounce Passed: 1   
High current detected. Setting body diode protection flag.   
Setting Charge MOSFET: ON (Internal state: 1)   
Setting Discharge MOSFET: ON   
NORMAL_OPERATION: Charge ON, Discharge ON.   
Debounce Passed: 0   
Overcurrent detected. Setting fault flag.   
High current detected. Setting body diode protection flag.   
Debounce Passed: 1   
Overcurrent detected. Setting fault flag.   
High current detected. Setting body diode protection flag.   
Setting Charge MOSFET: OFF (Internal state: 0)   
Setting Discharge MOSFET: OFF   
BOTH_OFF: Charge OFF, Discharge OFF.   
Debounce Passed: 0   
High current detected. Setting body diode protection flag.   
Debounce Passed: 1   
High current detected. Setting body diode protection flag.   
Setting Charge MOSFET: ON (Internal state: 1)   
Setting Discharge MOSFET: ON   
NORMAL_OPERATION: Charge ON, Discharge ON.   
Debounce Passed: 0   
Overvoltage detected. Setting fault flag.   
Debounce Passed: 1   
Overvoltage detected. Setting fault flag.   
Setting Charge MOSFET: OFF (Internal state: 0)   
Setting Discharge MOSFET: ON   
DISCHARGE_ONLY: Charge OFF, Discharge ON.   
Debounce Passed: 0   
Debounce Passed: 1   
Setting Charge MOSFET: ON (Internal state: 1)   
Setting Discharge MOSFET: ON   
NORMAL_OPERATION: Charge ON, Discharge ON.   
##### Test Case 1 Passed #####   