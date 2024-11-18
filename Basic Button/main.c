#include "main.h"

void SYS_Init();
volatile uint8_t led_state = 0;
volatile uint8_t pattern_state = 0;

int main(void) {
    SYS_Init();

    while(1) {

        if(pattern_state && led_state) {                                            // Only pattern if LED is on
            GPIOA->ODR ^= (1 << 8);
            for(volatile int i = 0; i < 1000000; i++);                              // Non-blocking delay
        }
    }
}

void EXTI9_5_IRQHandler() {
	if (EXTI -> PR & (1 << 9)) {				                    // Button PA9 Toggle
		GPIOA -> ODR ^= (1 << 8);
		led_state ^= 1;
		pattern_state = 0;
		EXTI -> PR |= (1 << 9);
	}
}

void EXTI15_10_IRQHandler() {
    if(EXTI->PR & (1 << 10)) {                                                      // Button PA10 Toggle
    		if(led_state) {
    			pattern_state ^= 1;
    		}
        EXTI->PR |= (1 << 10);
    }
}


void GPIO_Init() {
	RCC -> AHB1ENR |= (1 << 0);					            // Enable GPIOA Clock

	GPIOA -> MODER &= ~(0x3 << (8 * 2));		                            // Sets PA8 to output
	GPIOA -> MODER |= (1 << (8 * 2));

	GPIOA -> MODER &= ~(0x3 << (9 * 2));		                            // Sets PA9 to input and pull up
	GPIOA -> PUPDR &= ~(0x3 << (9 * 2));
	GPIOA -> PUPDR |= (1 << (9 * 2));

	GPIOA -> MODER &= ~(0x3 << (10 * 2));		                            // Sets PA10 to input and pull up
	GPIOA -> PUPDR &= ~(0x3 << (10 * 2));
	GPIOA -> PUPDR |= (1 << (10 * 2));
}

void Interrupt_Init() {
	RCC -> APB2ENR |= (1 << 14);				                    // Enable SYSCFG Clock

	SYSCFG -> EXTICR[2] &= ~(0xF << 4);			                    // Clear PA9 and PA10
	SYSCFG -> EXTICR[2] &= ~(0xF << 8);

	EXTI -> IMR |= (1 << 9) | (1 << 10);		                            // Interrupt mask for PA9 and PA10
	EXTI -> RTSR |= (1 << 9) | (1 << 10);		                            // Falling trigger for PA9 and PA10

	NVIC->ISER[0] |= (1 << 23);					            // Enable PA9 Interrupt in NVIC
	NVIC->ISER[1] |= (1 << 8);					            // Enable PA10 Interrupt in NVIC

	NVIC->IP[23] = (0 << 4);					            // Set Interrupt priorities
	NVIC->IP[40] = (1 << 4);
}

void SYS_Init() {
	GPIO_Init();
	Interrupt_Init();
}
