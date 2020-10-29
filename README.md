
# synth-lib

This repo is a library for synthesizer with hardware controlls base on the STM32F103C8T6 microcontroller, more known as the Blue Pill board.  

The purpose of this repo is to be a simple API for synth controls, such as Buttons, Potentiometers, LEDs and many more.

## Structure

The overall structure is made from 2 parts:  
Peripheral API,  
Interface API

### Peripheral API

This API will be used to access the internal hardware of the chip.  
Since this chip has many peripherals, and the library doesn't require to use all peripherals, this library will implement API for the following peripherals:

[//]: # "unticked: &#9744;"
[//]: # "ticked: &#9745;"

Docs aren't implemented yet, all of the documentation is in the header files for now, when I get to V1.0 of the basic API there will be a proper docs

1. &#9745; [RCC](docs/RCC.md)
1. &#9745; [GPIO](docs/GPIO.md)
1. &#9744; [UART](docs/UART.md)
1. &#9744; [Timers](docs/Timers.md)
1. &#9744; [ADC](docs/ADC.md)
1. &#9744; [I&#178;C](docs/I&#178;C.md)
1. &#9744; [SPI](docs/SPI.md)
1. &#9744; [USB](docs/USB.md)
1. &#9744; [Watchdog](docs/Watchdog.md)

# Versioning planning:

Bolded versions are released

1. __V0.1 - GPIO, RCC, overall initial files for project__
1. V0.2 - UART and DMA
1. V0.3 - ADC
1. V0.4 - TIMERS
1. V0.5 - EXTI and NVIC, interrupts in general
1. V0.6 - I&#178;C
1. V0.7 - SPI
1. V0.8 - USB
1. V0.9 - Debug message, Watchdogs and stability checking