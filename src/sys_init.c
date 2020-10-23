/*
File: system.c

Author: HammerPiano

Purpose: Initialize the basic functionality of the chip, including the clock,
usb debug msgs and watchdogs
*/

#include "RCC.h"

void chip_init()
{
	RCC_init_clock();
}