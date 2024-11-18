#include "main. h"

void EXTI15_10_IRQHandler(void);
void Interrupt_init(void);
void Timer_init(void);
void GPIO_init(void);
void System_init(void);

volatile uint32_t pattern_speed = 1000;
volatile uint8_t pattern_type = 0;
volatile uint32_t brightness = 0;
volatile uint8_t direction = 1;
volatile uint8_t counter = 0;

int main(void) {

	System_init();

	TIM2 -> CR1  |= (1 << 0);						                                         // Enables TIM2

	while (1)
   {

   }

}


void TIM2_IRQHandler(void) {
	if (TIM2 -> SR & (1 << 0)) {					                                      // Checks if the counter has reached the end
		if (pattern_type == 0) {					                                        // Patter 0: Slow fade
			if (direction) {
				brightness++;
				if (brightness >= 100) {			                                        // Checks if brightness is maxed out, if changes direction
					direction = 0;
				}
			} else {
				brightness--;						                                              // If direction is downward, brightness is decremented until 0;
				if (brightness == 0) direction = 1;
			}
		} else {									                                                // Pattern 1: Step Change
			counter++;
			if (counter >= 100) {					                                          // Changes brightness every 100 interrupts
				counter = 0;
			switch(brightness) {
			        case 0:
			        	brightness = 25;			                                        // 25% brightness
			        	break;
			        case 25:
			        	brightness = 50;			                                        // 50% brightness
			        	break;
			        case 50:
			        	brightness = 75;			                                        // 75% brightness
			        	break;
			        case 75:
			        	brightness = 100;			                                        // 100% brightness
			        	break;
			        default:
			        	brightness = 0;				                                        // Resets to 0 upon reaching max
			        	break;
			}
			}
		}

		TIM2 -> CCR1 = brightness;					                                      // Every interrupt the CCR1 value is used to control the brightness
		TIM2 -> SR &= ~(1 << 0);					                                        // Interrupt flag cleared
	}
}

void EXTI15_10_IRQHandler(void) {
    if(EXTI->PR & (1 << 13)) {						                                    // Checks for button press
    	pattern_type = !pattern_type;				                                    // Flips pattern

    	brightness = 0;								                                          // Resets brightness
    	direction = 1;								                                          // Sets direction to count upward

    	TIM2 -> EGR |= (1 << 0);					                                      // Enables generate event to reset ARR
    	EXTI -> PR |= (1 << 13);					                                      // Clears interrupt

    }
}


void GPIO_init(void) {
	RCC -> AHB1ENR |= (1 << 0); 	  				                                    // GPIOA Clock enabled
	RCC -> AHB1ENR |= (1 << 2);  					                                      // GPIOC Clock enabled

	GPIOA -> MODER &= ~(0x3 << (5 * 2)); 			                                  // Clearing GPIOA Bits
    GPIOA -> MODER |= (0x2 << (5 * 2)); 			                                // Setting GPIOC to alternate output

    GPIOA -> AFR[0] &= ~(0xF << (5 * 4));			                                // AFR[0] is for pins 0-7. These bits are cleared
    GPIOA -> AFR[0] |= (0x1 << (5 * 4));			                                // Each pin has 4 bits. This enables pin 5 to listen to AFR[0] which is TIM2

	GPIOC -> MODER &= ~(0x3 << (13 * 2)); 			                                // Clearing GPIOC Bits
}

void Timer_init(void) {
	RCC -> APB1ENR |= (1 << 0);       				                                  // TIM2 Clock enabled

	TIM2 -> PSC = 15999;							                                          // Sets prescaler to 15999
	TIM2 -> ARR = 100;								                                          // 100ms per cycle

	TIM2 -> CCMR1 &= ~(0x7 << 4);					                                      // Clearing CCMR1 bits
	TIM2 -> CCMR1 |= (0x6 << 4); 					                                      // Setting PWM mode 1 / Used for brightness changes
	TIM2 -> CCMR1 |= (1 << 3);						                                      // Enable preload for CCR1

	TIM2 -> CCR1 = brightness; 						                                      // Initial cycle value / Brightness value

	TIM2 -> CCER |= (1 << 0);						                                        // Enables Ch.1 output

	TIM2 -> DIER |= (1 << 0);						                                        // Enables interrupt
	TIM2 -> CR1 |= (1 << 7);						                                        // Enable ARPE
}

void Interrupt_init(void) {
	RCC -> APB2ENR |= (1 << 14); 					                                      // SYSCFG Clock enabled

	SYSCFG -> EXTICR[3] &= ~(0xF << 4); 			                                  // Clearing bits for pins 12-15
	SYSCFG -> EXTICR[3] |= (0x2 << 4); 				                                  // Setting pin 13 on Port C

	EXTI -> IMR |= (1 << 13); 						                                      // Enabled interrupt mask
	EXTI -> FTSR |= (1 << 13); 						                                      // Watches for button press

	NVIC -> ISER[0] |= (1 << 28); 					                                    // Enables interrupt register for timer on TIM2
	NVIC -> ISER[1] |= (1 << 8); 					                                      // Enables interrupt register for button on PC13

	NVIC -> IP[28] = (2 << 4);						                                      // Sets higher priority for timer
	NVIC -> IP[40] = (5 << 4);						                                      // Sets lower priority for button
}


// Initialize peripherals
void System_init(void) {
	GPIO_init();
	Timer_init();
	Interrupt_init();
}
