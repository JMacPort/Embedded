# What it does

GPIO pins were configured for an LED and a button. LED adjusts brightness based on a pattern variable that is set but a button click. 

There is a slow fade pattern in which the brightness gradually fades up and there is a step change pattern where the brightness goes up by 25% after 100 timer interrupts.

# Main Ideas

- Interrupts
- Interrupt Handlers
- GPIO Configs
- Timers
