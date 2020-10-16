
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

1. &#9744; [RCC](docs/RCC.md)
1. &#9744; [GPIO](docs/GPIO.md)
1. &#9744; [UART](docs/UART.md)
1. &#9744; [Timers](docs/Timers.md)
1. &#9744; [ADC](docs/ADC.md)
1. &#9744; [I&#178;C](docs/I&#178;C.md)
1. &#9744; [SPI](docs/SPI.md)
1. &#9744; [USB](docs/USB.md)
1. &#9744; [Watchdog](docs/Watchdog.md)

### Interface API

The interface/controls provided by this library include simple inputs/outputs such as:

#### Inputs

1. &#9744; Buttons  - momnetary
1. &#9744; Switches - toggle
1. &#9744; Potentiometers - with Linear/Log/Antilog behavior
1. &#9744; Button matrix
1. &#9744; Rotary encoders
1. &#9744; Touch "buttons"

#### Outputs

1. &#9744; LEDs - single/dual/RGB colors
1. &#9744; Led matrixes
1. &#9744; 7-segment leds
1. &#9744; Serial RGB leds (WS2812)
