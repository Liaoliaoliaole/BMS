/* GPIO settings */
#define GPIO_MODE_INPUT     0x00
#define GPIO_MODE_OUTPUT    0x01
#define GPIO_MODE_ALTERNATE 0x02
#define GPIO_MODE_ANALOG    0x03

/* PIN settings */
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

/*Battery configuration*/
#define CELL_VOLT_MAX (float)3.6
#define REFERENCE_VOLT (float)5.0 /* Reference voltage of ±5V for the ADC */

/* MODBUS Configuration*/
#define SLAVE_ADDR 0x01
#define FUNCTION_CODE 0x04
#define MAX_REGISTERS 10
#define NUM_REGISTERS 6
#define RESPONSE_BYTE_COUNT (NUM_REGISTERS*2) // 2 bytes per register
#define START_ADDR 0x00

struct register_data {
    int LM35_temp;
    int shunt;
    int cell_vol[4];
} typedef register_data;
