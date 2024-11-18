#include "main.h"

void SYS_Init();
volatile int counter = 0;
volatile uint8_t last_button = 0;

int main() {
	SYS_Init();

	while (1) {

	}
}

void EXTI4_IRQHandler(void) {                                                         // Adjusts LED brightness depending on button presses
    if(EXTI->PR & (1 << 4)) {
    	EXTI->PR |= (1 << 4);
    	uint8_t current = TIM2 -> CNT;
    	if (current - last_button > 150) {
    		counter++;
    		switch (counter) {
    			case 1:
    					TIM2 -> CCR3 = 250;
    						break;
    			case 2:
    					TIM2 -> CCR3 = 500;
    					break;
    			case 3:
    					TIM2 -> CCR3 = 750;
    					break;
    			case 4:
    					TIM2 -> CCR3 = 999;
    					break;
    			default:
    					counter = 0;
    					TIM2 -> CCR3 = 0;
    					break;
    		}
    	}

    }
}

void GPIO_Init() {
	RCC -> AHB1ENR |= (1 << 1); 				                                                // Enable GPIOB Clock

	GPIOB -> MODER &= ~(0x3 << (4 * 2));		                                            // GPIOB PB4	   BUTTON as INPUT
    GPIOB->PUPDR &= ~(3 << (4 * 2));  		                                           	// Set Pull-UP
    GPIOB->PUPDR |= (1 << (4 * 2));

	GPIOB -> MODER &= ~(0x3 << (10 * 2));                                            		// GPIOB PB10      LED
	GPIOB -> MODER |= (0x2 << (10 * 2));		                                            // Alternate Function

	GPIOB -> AFR[1] &= ~(0xF << (2 * 4));		                                            // Set AFR on PB10 to AF1/TIM2
	GPIOB -> AFR[1] |= (1 << (2 * 4));
}

void Timer_Init() {
	RCC -> APB1ENR |= (1 << 0);					                                                // Enables TIM2 Clock

	TIM2 -> PSC = 15;
	TIM2 -> ARR = 999;							                                                    // Sets 1ms clock cycle

	TIM2 -> CCMR2 |= (0x6 << 4);				                                                // Enables channel 3 PWM mode 1
	TIM2 -> CCER |= (1 << 8);

	TIM2 -> CCR3 = 0;							                                                      // Sets initial duty cycle to 0

	TIM2 -> CR1 |= (1 << 0);					                                                  // Enables TIM2
}

void Interrupt_Init() {
	RCC -> APB2ENR |= (1 << 14);				                                                // Enables SYSCFG

	SYSCFG -> EXTICR[1] &= ~(0xF << 0);			                                            // Configures Pin 4 in GPIOB to watch for interrupts
	SYSCFG -> EXTICR[1] |= (1 << 0);

	EXTI -> IMR |= (1 << 4);					                                                  // Enables interrupt mask
	EXTI -> FTSR |= (1 << 4);					                                                  // Falling Trigger

	NVIC -> ISER[0] |= (1 << 10);				                                                // Enables interrupt in NVIC
}

void SYS_Init() {
	GPIO_Init();
	Timer_Init();
	Interrupt_Init();
}
