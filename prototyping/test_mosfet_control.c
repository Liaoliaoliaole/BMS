/*
******************************************************************************
Depreciated!
PWM not used in Li-on battery mosfet control.

******************************************************************************
*/

/* Includes */
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "stm32l1xx.h"


/* Private typedef */
/* Private define  */
/* Private macro */
#define PWM_FREQUENCY 10000    // 10 kHz for example
#define PWM_MAX_DUTY 1000      // Max duty cycle for PWM (10-bit resolution)

#define LOW_THRESHOLD 1000   // Current threshold for enabling charge MOSFET (in ADC)
#define HIGH_THRESHOLD 3000 // Current threshold for enabling both MOSFETs (in ADC)
#define SAFETY_TIMER_MS 1000 // Safety timer in milliseconds

#define SOFT_START_STEPS 10 // Number of steps for soft start
#define SOFT_START_DELAY 100 // Delay between steps in ms

#define CHARGE_MOSFET                  (uint32_t)(1 << 6)   // PA6
#define DISCHARGE_MOSFET               (uint32_t)(1 << 7)   // PA7

/* Private variables */
typedef enum {
    STATE_DISABLE,
    STATE_CHARGE,
    STATE_DISCHARGE,
    STATE_BOTH
} MosfetState;

static MosfetState state = STATE_DISABLE; // Initialize state
static uint32_t lastSwitchTime = 0;       // Last time the state switched
static uint32_t softStartDuty = 0;        // Duty cycle for soft start
static uint32_t currentDuty = 0;          // Current duty cycle for PWM
static uint32_t systick_ms;                // Mock systick for example
static uint32_t mock_current = 0; // Simulated current value

/* Private function prototypes */
uint32_t Read_Current(void);
void PWM_init(void);
void PWM_set_dutyCycle(uint32_t mosfet, uint32_t dutyCycle);
void PWM_gradual_start(uint32_t mosfet, uint32_t target_duty_cycle, uint32_t delay_time_ms);
void Mosfets_Control(uint32_t current);
void delay_ms(uint32_t delay);

/* Private functions */
void SetSysClock(void);

/**
**===========================================================================
**
**  Abstract: main program
**
**===========================================================================
*/
int main(void)
{
  PWM_init();

  /**
  *  IMPORTANT NOTE!
  *  See the <system_*.c> file and how/if the SystemInit() function updates
  *  SCB->VTOR register. Sometimes the symbol VECT_TAB_SRAM needs to be defined
  *  when building the project if code has been located to RAM and interrupts
  *  are used. Otherwise the interrupt table located in flash will be used.
  *  E.g.  SCB->VTOR = 0x20000000;
  */

  /* Configure the system clock to 32 MHz and update SystemCoreClock */
  SetSysClock();
  SystemCoreClockUpdate();

  /* TODO - Add your application code here */
  //PWM_set_dutyCycle(CHARGE_MOSFET,500);
  //PWM_set_dutyCycle(DISCHARGE_MOSFET,500);


  /* Infinite loop */
  while (1)
  {
	  uint32_t current = Read_Current(); // Read the current
	  Mosfets_Control(current);
	  delay_ms(200);
  }
  return 0;
}


uint32_t Read_Current(void)
{
    mock_current += 100;
    if (mock_current > 4095) {
        mock_current = 0;
    }
    return mock_current;
}

void PWM_init(void) {
    // Enable GPIOA clock
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;

    // Set PA6 and PA7 to alternate function mode
    GPIOA->MODER &= ~((GPIO_MODER_MODER6) | (GPIO_MODER_MODER7)); // Clear mode bits
    GPIOA->MODER |= (GPIO_MODER_MODER6_1 | GPIO_MODER_MODER7_1); // Set PA6 and PA7 to AF mode page 40 PA6 AND PA7 TIM3_CH1 AND CH2

    // Set PA6 and PA7 to AF2 (TIM3)
    GPIOA->AFR[0] &= ~((GPIO_AFRL_AFRL6) | (GPIO_AFRL_AFRL7)); // Clear AFR bits for PA6 and PA7
    GPIOA->AFR[0] |= ((2 << (6 * 4)) | (2 << (7 * 4))); // Set AF2 for PA6 and PA7 (TIM3)

    // Enable TIM3 clock
    RCC->APB1ENR |= RCC_APB1ENR_TIM3EN; // Enable TIM3 clock

    // Configure TIM3
    TIM3->PSC = (SystemCoreClock / PWM_FREQUENCY) - 1; // Set prescaler
    TIM3->ARR = PWM_MAX_DUTY - 1; // Set auto-reload register for max duty cycle

    // Configure PWM mode for TIM3 channel 1 (PA6)
    TIM3->CCMR1 |= (6 << TIM_CCMR1_OC1M_Pos); // PWM mode 1
    TIM3->CCMR1 |= TIM_CCMR1_OC1PE; // Enable preload for channel 1
    TIM3->CCER |= TIM_CCER_CC1E; // Enable output for channel 1

    // Configure PWM mode for TIM3 channel 2 (PA7)
    TIM3->CCMR1 |= (6 << TIM_CCMR1_OC2M_Pos); // PWM mode 1 for channel 2
    TIM3->CCMR1 |= TIM_CCMR1_OC2PE; // Enable preload for channel 2
    TIM3->CCER |= TIM_CCER_CC2E; // Enable output for channel 2

    TIM3->CCR1 = 0; // Initialize channel 1 (PA6) duty cycle
    TIM3->CCR2 = 0; // Initialize channel 2 (PA7) duty cycle

    // Start the timer
    TIM3->CR1 |= TIM_CR1_CEN; // Enable the timer
}

void PWM_set_dutyCycle(uint32_t mosfet, uint32_t dutyCycle) {
	if (dutyCycle > PWM_MAX_DUTY) {
	        dutyCycle = PWM_MAX_DUTY;
	    }

    if (mosfet == CHARGE_MOSFET) {
    	TIM3->CCR1 = dutyCycle; // Set duty cycle for MOSFET 1
    } else if (mosfet == DISCHARGE_MOSFET) {
    	TIM3->CCR2 = dutyCycle; // Set duty cycle for MOSFET 2
    }
}

void PWM_gradual_start(uint32_t mosfet, uint32_t target_duty_cycle, uint32_t delay_time_ms) {
	static uint32_t current_duty_cycle = 0;

    if (target_duty_cycle > PWM_MAX_DUTY) {
        target_duty_cycle = PWM_MAX_DUTY;
    }

    if (target_duty_cycle > current_duty_cycle) {
        while (current_duty_cycle < target_duty_cycle) {
            PWM_set_dutyCycle(mosfet, current_duty_cycle++);
            delay_ms(delay_time_ms);
        }
    } else {
        while (current_duty_cycle > target_duty_cycle) {
            PWM_set_dutyCycle(mosfet, current_duty_cycle--);
            delay_ms(delay_time_ms);
        }
    }
}

//void Mosfets_Control(uint32_t current) {
//    uint32_t currentTime = systick_ms; // Get the current time in milliseconds
//
//    // Determine the state based on the current value
//    if (current < LOW_THRESHOLD) {
//        state = STATE_CHARGE; // Set state to charge if current is below low threshold
//    } else if (current >= HIGH_THRESHOLD) {
//        state = STATE_DISCHARGE; // Set to both if current is above high threshold
//    } else {
//        state = STATE_BOTH; // Reset to disable state if current is in between thresholds
//    }
//
//    // Perform actions based on the current state
//    switch (state) {
//        case STATE_DISABLE:
//            // Turn off both MOSFETs
//            PWM_gradual_start(CHARGE_MOSFET | DISCHARGE_MOSFET, 0, 5); // Set PWM to 0%
//            break;
//
//        case STATE_CHARGE:
//            // Soft start logic for charging MOSFET
//            if (currentDuty < (PWM_MAX_DUTY / 2)) {
//                softStartDuty += (PWM_MAX_DUTY / 2) / SOFT_START_STEPS; // Increase duty cycle gradually
//                if (softStartDuty > (PWM_MAX_DUTY / 2)) {
//                    softStartDuty = (PWM_MAX_DUTY / 2); // Cap to max charge duty
//                }
//            }
//            PWM_gradual_start(CHARGE_MOSFET, currentDuty, 10); // Set PWM for charge MOSFET
//            break;
//
//        case STATE_BOTH:
//            if ((currentTime - lastSwitchTime) >= SAFETY_TIMER_MS) {
//                // Enable both MOSFETs with soft start
//                if (currentDuty < PWM_MAX_DUTY) {
//                    currentDuty += PWM_MAX_DUTY / SOFT_START_STEPS; // Gradually increase duty cycle
//                    if (currentDuty > PWM_MAX_DUTY) {
//                        currentDuty = PWM_MAX_DUTY; // Cap to max duty
//                    }
//                }
//                PWM_gradual_start(CHARGE_MOSFET | DISCHARGE_MOSFET, currentDuty, 10); // Set PWM to 100% for both
//                lastSwitchTime = currentTime; // Update last switch time
//            }
//            break;
//
//        case STATE_DISCHARGE:
//            // Turn off MOSFETs
//            PWM_gradual_start(CHARGE_MOSFET | DISCHARGE_MOSFET, 0, 5); // Turn off MOSFETs
//            break;
//
//        default:
//            // Handle unexpected state
//            state = STATE_DISABLE; // Reset to a known state
//            break;
//    }
//}

void Mosfets_Control(uint32_t current) {
    // Determine the state based on the current value
    if (current < LOW_THRESHOLD) {
        state = STATE_CHARGE; // Set state to charge if current is below low threshold
    } else if (current >= HIGH_THRESHOLD) {
        state = STATE_DISCHARGE; // Set state to discharge if current is above high threshold
    } else {
        state = STATE_BOTH; // Set to both if current is in the middle range
    }

    // Perform actions based on the current state
    switch (state) {
        case STATE_DISABLE:
            // Turn off both MOSFETs
            PWM_gradual_start(CHARGE_MOSFET | DISCHARGE_MOSFET, 0, 5); // Set PWM to 0%
            break;

        case STATE_CHARGE:
            // Enable only the charge MOSFET
            PWM_gradual_start(CHARGE_MOSFET, PWM_MAX_DUTY / 2, 10); // Set PWM to 50% for charge
            PWM_gradual_start(DISCHARGE_MOSFET, 0, 5); // Turn off discharge MOSFET
            break;

        case STATE_BOTH:
            // Enable both MOSFETs immediately
            PWM_gradual_start(CHARGE_MOSFET | DISCHARGE_MOSFET, PWM_MAX_DUTY, 10); // Set PWM to 100% for both
            break;

        case STATE_DISCHARGE:
            // Enable only the discharge MOSFET
            PWM_gradual_start(CHARGE_MOSFET, 0, 5); // Turn off charge MOSFET
            PWM_gradual_start(DISCHARGE_MOSFET, PWM_MAX_DUTY / 2, 10); // Set PWM to 50% for discharge
            break;

        default:
            // Reset to a known state
            state = STATE_DISABLE;
            break;
    }
}



void SysTick_Handler(void) {
    systick_ms++;  // Increment tick count every 1 ms
}

uint32_t GetTick(void) {
    return systick_ms;  // Return the tick count in milliseconds
}

void delay_ms(uint32_t delay) {
    int i = 0;
    for (; delay > 0; delay--) {
        for (i = 0; i < 2460; i++);
    }
}

void SetSysClock(void)
{
	uint32_t status = 0;

  /* Enable HSI */
  RCC->CR |= (uint32_t)1;

  /* Wait till HSI is ready and if Time out is reached exit */
 while(!(RCC->CR & (uint32_t)2)){} //CR bit 1 HSIRDY sets when HSI oscillator is stable. p141

  if ((RCC->CR & (uint32_t)2) != 0) //if CR bit 1 HSIDRY high when oscillator is stable.
  {
    status = 1;
  }
  else
  {
    status = 0;
  }

  if (status == 1)
  {
    /*  PLL configuration: PLLCLK = (HSI * 4)/2 = 32 MHz */
	  //PLLSCR bit 16, PLLMUL bits 18-21, PLLDIV bits 22,23.
	  //0000 0000 1111 1101 0000 0000 0000 0000 corresponding bits
	  //these bits PLLSRC, PLLMUL and PLLDIV cleared
	  RCC->CFGR &= ~(0x00FD0000);
	  //PLLMUL must be 0001 when multiplication=4 and PLLDIV must be 01 when division=2
	  //PLLSCR bit 16 must be 1 --> HSE oscillator clock selected as PLL input clock (done in previous line)
	  //0000 0000 0100 0100 0000 0000 0000 0000
	  RCC->CFGR |= 0x00440000;
  }

  else
  {
    /* If HSI fails to start-up, the application will have wrong clock
    configuration. User can add here some code to deal with this error */
  }

  /*64-bit access is configured by setting the ACC64 bit in the Flash access control register (FLASH_ACR).
   *This access mode accelerates the execution of program operations.*/
  FLASH->ACR |= (uint32_t)4; //ACC64 bit 2, 64-bit access. 64-bit access is used to improve the performance. p84
    /*Prefetch is enabled by setting the PRFTEN bit in the FLASH_ACR register.
    *This feature is useful if at least one wait state is needed to access the Flash memory.
	*Figure 5 shows the execution of sequential 32-bit instructions*/
  FLASH->ACR |= (uint32_t)2; //PRFTEN bit 1, prefetch enable. p84
  FLASH->ACR |= (uint32_t)1; //LATENCY one wait state bit 0. One wait state enabled. p84. p59

  RCC->APB1ENR |= 0x10000000; //bit 28 PWREN: Power interface clock enable. p158. p101
  PWR->CR = (uint32_t)(1<<11); //Bits 12:11 VOS[1:0]: Voltage scaling range selection, 01: 1.8 V (range 1). p121
  	/*A delay is required for the internal regulator to be ready after the voltage range is changed.
	The VOSF bit indicates that the regulator has reached the voltage level defined with bits VOS
	of PWR_CR register. p102 1.8V needed for 32 MHz clock and lower voltages to save power.

	0: Regulator is ready in the selected voltage range
	1: Regulator voltage output is changing to the required VOS level.
	*/
  while((PWR->CSR & (uint32_t)(1<<4)) != 0){} //bit 4 VOSF: Voltage Scaling select flag. p125

  RCC->CFGR &=(uint32_t)~(1<<7); //Bits 7:4 HPRE[3:0]: AHB prescaler. 0xxx: SYSCLK not divided. p144
  RCC->CFGR &=(uint32_t)~(1<<13); //Bits 13:11 PPRE2[2:0]: APB high-speed prescaler (APB2). p144
  RCC->CFGR &=(uint32_t)~(1<<10); //Bits 10:8 PPRE1[2:0]: APB low-speed prescaler (APB1)p. 144

  RCC->CR |= (1<<24); //Bit 24 PLLON: PLL enable. p140
  while((RCC->CR & (uint32_t)(1<<25)) == 0){} //Bit 25 PLLRDY: PLL clock ready flag. p140

  /* Select PLL as system clock source */
  RCC->CFGR |= (uint32_t)3; //Bits 1:0 SW[1:0]: System clock switch, 11: PLL used as system clock

  /*Bits 3:2 SWS[1:0]: System clock switch status, 11: PLL used as system clock.
   * These bits are set and cleared by hardware to indicate which clock source is used as
	system clock. 11: PLL used as system clock. p145.
   */
  while ((RCC->CFGR & (uint32_t)(3<<2))==0){}
}
