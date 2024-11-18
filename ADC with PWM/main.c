#include "main.h"


void SYS_Init();
uint16_t ADC_READ();


int main() {

	SYS_Init();

	while (1) {
		uint16_t adc_value = ADC_READ();
		uint16_t pwm_value = (adc_value * 9) / 4095;

		TIM2 -> CCR3 = pwm_value;
	}

}

void GPIO_Init() {
	RCC -> AHB1ENR |= (1 << 0);				                                                // Enable Clock for GPIOA for PA0 - Potentiometer
	RCC -> AHB1ENR |= (1 << 1);				                                                // Enable Clock for GPIOB for PB 10 - LED OUTPUT/TIM2 Channel 3

	GPIOA -> MODER &= ~(0x3 << 0);			                                              // PA0 to analog mode
	GPIOA -> MODER |= (0x3 << 0);

	GPIOB -> MODER &= ~(0x3 << (10 * 2));	                                            // PB10 to AFR
	GPIOB -> MODER |= (0x2 << (10 * 2));

	GPIOB -> AFR[1] |= (1 << (2 * 4));		                                            // Enables AFR on pin 10
}

void Timer_Init() {
	RCC -> APB1ENR |= (1 << 0);				                                                // Enables Clock for TIM2

	TIM2 -> PSC = 15999;					                                                    // 1ms tick rate
	TIM2 -> ARR = 9;
	TIM2 -> CCMR2 |= (0x6 << 4);			                                                // Sets PWM mode for Channel 3
	TIM2 -> CCER = (1 << 8);				                                                  // Enables Channel 3
	TIM2 -> CCR3 = 0;						                                                      // Initial duty cycle to 0; LED is off

	TIM2 -> CR1 |= (1 << 0);				                                                  // Enables timer
}

void ADC_Init() {
	RCC -> APB2ENR |= (1 << 8);				                                                // Enables Clock for ADC1

	ADC1 -> CR1 &= ~(0x3 << 24);			                                                // Sets default 12-bit resolution

	ADC1 -> SMPR2 &= ~(0x3 << 0);			                                                // Clears channel 0 bits
	ADC1 -> SMPR2 |= (0x4 << 0);			                                                // Sets sampling time to 84 cycles

	ADC1 -> SQR1 &= ~(0xF << 20);			                                                // 1 conversion
	ADC1 -> SQR3 &= ~(0xF << 0);			                                                // Channel 0
	ADC1 -> SQR3 |= (0 << 0); 				                                                // Selects Channel 0

	ADC1 -> CR2 |= (1 << 0);				                                                  // Enables ADC
}

void SYS_Init() {
	GPIO_Init();
	Timer_Init();
	ADC_Init();
}

uint16_t ADC_READ() {
	ADC1 -> CR2 |= (1 << 30);                                                        // Start conversion
	while (!(ADC1 -> SR & (1 << 1)));                                                // Wait for EOC (End Of Conversion) flag
	uint16_t adc_value = (uint16_t) ADC1 -> DR;                                      // Read the converted value
	ADC1 -> SR &= ~(1 << 1);                                                         // Clear EOC flag
	return adc_value;
}




