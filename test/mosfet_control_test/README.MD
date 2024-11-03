## Test mosfet control logic in PC
Compile:   
`gcc .\test_mosfet.c .\mosfet_control.c -o test`   
Run:   
`./test`   
Should see:   

##### Test MOSFET State Control with Sensor Values #####
All faults cleared.
Setting Charge MOSFET: OFF (Internal state: 0)
Setting Discharge MOSFET: OFF
All faults cleared.
All faults cleared.
Setting Charge MOSFET: OFF (Internal state: 0)
Setting Discharge MOSFET: OFF
All faults cleared.
##### Test Case 1: Temperature -35 degree #####
Debounce Passed: 0
Temperature below -20 C. Setting fault flag.
Debounce Passed: 1
Temperature below -20 C. Setting fault flag.
BOTH_OFF: Charge OFF, Discharge OFF
All faults cleared.
Setting Charge MOSFET: OFF (Internal state: 0)
Setting Discharge MOSFET: OFF
All faults cleared.
##### Test Case 1: Temperature -15 degree #####
Debounce Passed: 0
Temperature between -20 C and 5 C. Discharge only allowed.
Debounce Passed: 1
Temperature between -20 C and 5 C. Discharge only allowed.
Setting Charge MOSFET: OFF (Internal state: 0)
Setting Discharge MOSFET: ON
DISCHARGE_ONLY: Charge OFF, Discharge ON.
DISCHARGE_ONLY: Charge OFF, Discharge ON
All faults cleared.
Setting Charge MOSFET: OFF (Internal state: 0)
Setting Discharge MOSFET: OFF
All faults cleared.
##### Test Case 1: Temperature 20 degree #####
Debounce Passed: 0
Debounce Passed: 1
Setting Charge MOSFET: ON (Internal state: 1)
Setting Discharge MOSFET: ON
NORMAL_OPERATION: Charge ON, Discharge ON.
NORMAL_OPERATION: Charge ON, Discharge ON
All faults cleared.
Setting Charge MOSFET: OFF (Internal state: 0)
Setting Discharge MOSFET: OFF
All faults cleared.
##### Test Case 1: Temperature 35 degree #####
Debounce Passed: 0
Temperature between 30 C and 40 C. Discharge only allowed.
Debounce Passed: 1
Temperature between 30 C and 40 C. Discharge only allowed.
Setting Charge MOSFET: OFF (Internal state: 0)
Setting Discharge MOSFET: ON
DISCHARGE_ONLY: Charge OFF, Discharge ON.
DISCHARGE_ONLY: Charge OFF, Discharge ON
All faults cleared.
Setting Charge MOSFET: OFF (Internal state: 0)
Setting Discharge MOSFET: OFF
All faults cleared.
##### Test Case 1: Temperature 45 degree #####
Debounce Passed: 0
Temperature above 40┬░C. Setting fault flag.
Debounce Passed: 1
Temperature above 40┬░C. Setting fault flag.
Setting Charge MOSFET: OFF (Internal state: 0)
Setting Discharge MOSFET: OFF
BOTH_OFF: Charge OFF, Discharge OFF.
BOTH_OFF: Charge OFF, Discharge OFF
##### Test Case 2 :Overcurrent Testing for Normal Temperature Rang #####
All faults cleared.
Setting Charge MOSFET: OFF (Internal state: 0)
Setting Discharge MOSFET: OFF
All faults cleared.
##### Test Case 2: Overcurrent 1.200000 A #####
Debounce Passed: 0
Debounce Passed: 1
Setting Charge MOSFET: ON (Internal state: 1)
Setting Discharge MOSFET: ON
NORMAL_OPERATION: Charge ON, Discharge ON.
NORMAL_OPERATION: Charge ON, Discharge ON
All faults cleared.
Setting Charge MOSFET: OFF (Internal state: 0)
Setting Discharge MOSFET: OFF
All faults cleared.
##### Test Case 2: Overcurrent 10.000000 A #####
Debounce Passed: 0
High current detected. Body diode protection activated. Turning on both MOSFETs to protect diode.Overcurrent detected. Setting fault flag.
Debounce Passed: 1
Overcurrent detected. Setting fault flag.
OVERCURRENT: Charge OFF, Discharge OFF
All faults cleared.
Setting Charge MOSFET: OFF (Internal state: 0)
Setting Discharge MOSFET: OFF
All faults cleared.
##### Test Case 2: Overcurrent 55.000000 A #####
Debounce Passed: 0
High current detected. Body diode protection activated. Turning on both MOSFETs to protect diode.Overcurrent detected. Setting fault flag.
Debounce Passed: 1
Overcurrent detected. Setting fault flag.
OVERCURRENT: Charge OFF, Discharge OFF
##### Test Case 3: Voltage Protection #####
All faults cleared.
Setting Charge MOSFET: OFF (Internal state: 0)
Setting Discharge MOSFET: OFF
All faults cleared.
Debounce Passed: 0
Undervoltage detected. Setting fault flag.
Debounce Passed: 1
Undervoltage detected. Setting fault flag.
Setting Charge MOSFET: ON (Internal state: 1)
Setting Discharge MOSFET: OFF
CHARGE_ONLY: Charge ON, Discharge OFF.
#3.1 UNDERVOLTAGE: Charge ON, Discharge OFF
All faults cleared.
Setting Charge MOSFET: OFF (Internal state: 0)
Setting Discharge MOSFET: OFF
All faults cleared.
Debounce Passed: 0
Overvoltage detected. Setting fault flag.
Debounce Passed: 1
Overvoltage detected. Setting fault flag.
Setting Charge MOSFET: OFF (Internal state: 0)
Setting Discharge MOSFET: ON
DISCHARGE_ONLY: Charge OFF, Discharge ON.
#3.2 OVERVOLTAGE: Charge OFF, Discharge ON
All faults cleared.
Setting Charge MOSFET: OFF (Internal state: 0)
Setting Discharge MOSFET: OFF
All faults cleared.
Debounce Passed: 0
Debounce Passed: 1
Setting Charge MOSFET: ON (Internal state: 1)
Setting Discharge MOSFET: ON
NORMAL_OPERATION: Charge ON, Discharge ON.
#3.3 VOLTAGE_NORMAL: Charge ON, Discharge ON
##### Test Case 4: Body Diode Protection #####
All faults cleared.
Debounce Passed: 0
High current detected. Body diode protection activated. Turning on both MOSFETs to protect diode.Debounce Passed: 1    
# 4.1 BODY_DIODE_PROTECTION_TRIGGERED: Charge ON, Discharge ON
##### Test Case 5: Body Diode Protection Cleared #####
Debounce Passed: 1
Debounce Passed: 1
#4.2 BODY_DIODE_PROTECTION_CLEARED: Charge ON, Discharge ON
##### All Test Cases Passed #####