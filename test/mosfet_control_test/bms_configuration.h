#pragma once

/* Mosfet Control Configuration*/
#define SHORT_CIRCUIT_THRESHOLD 50.0f          // Amperes
#define OVERCURRENT_THRESHOLD 9.0f             // Amperes

#define DISCHARGE_OVER_TEMPERATURE_THRESHOLD 40.0f       // Degrees Celsius (for discharging)
#define DISCHARGE_UNDER_TEMPERATURE_THRESHOLD -20.0f     // Degrees Celsius (for discharging)

#define CHARGE_OVER_TEMPERATURE_THRESHOLD 30.0f // Degrees Celsius (for charging)
#define CHARGE_UNDER_TEMPERATURE_THRESHOLD 5.0f // Degrees Celsius (for charging)

#define LOW_VOLTAGE_THRESHOLD 2.8f             // Volts per cell
#define HIGH_VOLTAGE_THRESHOLD 3.2f            // Volts per cell

#define HIGH_CURRENT_THRESHOLD 1.1f            // Amperes (for body diode protection)

#define DEBOUNCE_DELAY_MS 10                   // Milliseconds (reduced for testing)