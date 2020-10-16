# RCC Documentation

The RCC - reset and clock control is the first peripheral that must be implemented, as it controls all of the clock and reset signals. 

## API:

No exact functions yet, there are the main functions:

- clock_init - initializes the core clock to run at 72MHz

- clock_deinit - reset all clock registers to the default value

- periph\_clock\_control - enable/disable clock for the given peripheral

- periph_reset - reset a peripheral

- get\_AHB/APB1/APB2\_freq - returns the clock frequency for the given domain

- get\_periph\_freq - returns the clock frequency for the given peripheral

- get\_reset\_code - returns the source for the last clock (Internaly used the Backup domain for that) (Not tested it actually works yet)

- reset - generated a system reset. will restart the program

[Go Back](../README.md)