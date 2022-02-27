/*
 * This file is part of the Cordless Power Tool Vacuum Start distribution
 * (https://github.com/abudden/cordlessvacuumstart).
 * Copyright (c) 2022 A. S. Budden
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

// Entrypoint & main loop

#include "Global.h"
#include <stdio.h> // putchar
#include "cmsis.h"

#include "Clock.h"
#include "PrintSupport.h"
#include "Pins.h"
#include "Switches.h"
#include "Debug.h"
#include "Application.h"
#include "DefinedPins.h"
#include "tinyprintf.h"

int main()
{
	// NOTE: the SystemInit function is called from the start-up code prior to running main!
	// SystemInit();

	// Timing pin for testing
	SetPinAsGPO_PP(LOOPTIME_PIN); // B10

	SetupClocks();
	InitSwitches();
	InitPrintSupport();
	InitApplication();
	InitDebug();

	putstring("\fStarting..\n");

	while (true) {
		// LOOPTIME_PIN is used to measure how long the main loop is taking
		// (in order to ensure that it is << 1 ms and we're not overworking the
		// microcontroller).
		SetPinState(LOOPTIME_PIN, true);

		// Loop runs once per millisecond for non-time-critical updates
		UpdatePrintSupport();
		UpdateSwitches();
		UpdateDebug();
		UpdateApplication();

		SetPinState(LOOPTIME_PIN, false);

		/* Enter wait mode, and do not exit until SysTick_Handler() clears
		   sleep-on-exit bit. System will respond to other interrupts,
		   and then go back to sleep */
		SCB->SCR |= (uint32_t) SCB_SCR_SLEEPONEXIT_Msk;

		__WFI();
	}

	return 0;
}

