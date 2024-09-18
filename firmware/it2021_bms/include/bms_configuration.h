/* GPIO settings */
#define GPIO_MODE_INPUT     0x00
#define GPIO_MODE_OUTPUT    0x01
#define GPIO_MODE_ALTERNATE 0x02
#define GPIO_MODE_ANALOG    0x03

/* ADC PIN settings */
#define PA0                 0
#define PA1                 1
#define PA4                 4
#define PC1                 1

/*ADC settings*/
#define ADC_SAMPLING_TIME_384CYC  7
#define ADC_CR2_ADON_Pos    0 // Bit position of ADON in ADC_CR2 register
#define ADC_CR2_SWSTART_Pos 30 // Bit position for SWSTART in ADC_CR2 register
#define ADC_ENABLE()    (ADC1->CR2 |= (1 << ADC_CR2_ADON_Pos))
#define ADC_DISABLE()   (ADC1->CR2 &= ~(1 << ADC_CR2_ADON_Pos))
#define ADC_START_CONVERSION()  (ADC1->CR2 |= (1 << ADC_CR2_SWSTART_Pos))

/* ADC Channel Definitions */
#define CHANNEL_BATTERY_TEMP0  0
#define CHANNEL_BATTERY_TEMP1  1
#define CHANNEL_SHUNT_CURRENT  2
#define CHANNEL_CELL_VOLT_1   3
#define CHANNEL_CELL_VOLT_2   4
#define CHANNEL_CELL_VOLT_3   5
#define CHANNEL_CELL_VOLT_4   6

/* MUX PIN setting */
#define MUX_PIN_A (1U << 3)  // PB3
#define MUX_PIN_B (1U << 4)  // PB4
#define MUX_PIN_C (1U << 5)  // PB5

#define MUX_PIN_MASK (MUX_PIN_A | MUX_PIN_B | MUX_PIN_C)  // Mask for all MUX pins

// Bit position masks
#define MUX_MODER_MASK ((0x3 << 6) | (0x3 << 8) | (0x3 << 10)) // MODER bits for PB3, PB4, PB5
#define MUX_OTYPER_MASK (MUX_PIN_A | MUX_PIN_B | MUX_PIN_C)     // OTYPER bits for PB3, PB4, PB5
#define MUX_OSPEEDR_MASK ((0x3 << 6) | (0x3 << 8) | (0x3 << 10)) // OSPEEDR bits for PB3, PB4, PB5
#define MUX_PUPDR_MASK ((0x3 << 6) | (0x3 << 8) | (0x3 << 10))   // PUPDR bits for PB3, PB4, PB5

#define MUX_IDR_ODR_MASK (MUX_PIN_A | MUX_PIN_B | MUX_PIN_C)  // ODR and IDR mask for MUX pins


/*Battery configuration*/
#define CELL_VOLT_MAX (float)3.6
#define REFERENCE_VOLT (float)5.0 /* Reference voltage of ±5V for the ADC */

/* MODBUS Configuration*/
#define HSI_VALUE    ((uint32_t)16000000)
#define SLAVE_ADDR 0x01
#define FUNCTION_CODE 0x04
#define START_ADDR 0x00
#define NUM_REGISTERS 6
#define RESPONSE_BYTE_COUNT (NUM_REGISTERS*2) // 2 bytes per register
#define MAX_REGISTERS 10
#define BUFFER_SIZE 20


/* Sensor Data Structure
typedef struct {
    int LM35_temp;
    int shunt;
    int cell_vol[4];
} BmsSensorData; */
