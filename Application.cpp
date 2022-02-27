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

// Main application state machine

#include <assert.h>
#include <stdlib.h>

#include "Global.h"
#include "Clock.h"
#include "Pins.h"
#include "DefinedPins.h"
#include "Analogue.h"
#include "PrintSupport.h"
#include "Switches.h"
#include "Transmitter.h"

#include "Application.h"

#define CURRENT_HYSTERESIS_HIGH ((uint16_t) 70)
#define CURRENT_HYSTERESIS_LOW ((uint16_t) 50)

// If set, this will force transmission of the measured current.  This is
// useful if you want to assemble the unit, then plug it into a power tool
// and read how much current is being measured.
//#define TRANSMIT_CURRENT

void InitApplication()
{
	InitAnalogue();
	InitTransmitter();
}

void UpdateApplication()
{
	static uint32_t state_timer;
	static uint32_t button_timer;
	static enum {
		IdleState,
		TurningOnState,
		DelayState,
		TurningOffState
	} current_state = IdleState;
	static bool current_control = true;
	static bool transmit_current = false;
	static uint32_t delayed_start_timer = 0;
	static bool delayed_start_complete = false;

#ifdef TRANSMIT_CURRENT
	// Forced on
	transmit_current = true;
#endif

	UpdateAnalogue();
	UpdateTransmitter();

	if ( ! delayed_start_complete) {
		if (MillisecondsHaveElapsed(delayed_start_timer, 1000U)) {
			delayed_start_complete = true;
		}
		// Ignore momentary push buttons for 1 second after start-up
		(void) HasReceivedMomentaryButton(PushButtonSwitch);
	}
	else {
		// Pressing the push button briefly will cause the transmitter to switch
		// state, regardless of current (unless we're in transmit_current mode)
		if (( ! transmit_current) && (HasReceivedMomentaryButton(PushButtonSwitch))) {
			NextTransmitterState();
			if (IsTransmitting()) {
				current_control = false;
			}
			else {
				current_control = true;
			}
		}
	}

	if (GetSwitchState(PushButtonSwitch) && MillisecondsHaveElapsed(button_timer, 2000)) {
		// Button has been held down for 2 seconds; switch into 
		// current transmit mode (for diagnostic purposes)
		current_control = false;
		transmit_current = true;
	}
	else if (! GetSwitchState(PushButtonSwitch)) {
		button_timer = GetMillisecondCounter();
	}
	else {
	}

	// If we're in transmit_current mode, just send the latest current and don't
	// bother with the state machine
	if (transmit_current) {
		StartTransmittingValue(GetAnalogueCurrent());
		return;
	}

	// If we're in current_control mode (the default), run a state machine to
	// monitor the current and control the socket accordingly.
	if (current_control) {
		switch(current_state) {
			default:
			case IdleState:
				// Idle: wait here until the current crosses the upper
				// hysteresis band
				if (GetAnalogueCurrent() > CURRENT_HYSTERESIS_HIGH) {
					// Current has gone high, so start transmitting the turn-on
					// signal and go to the turning on state
					StartTransmitting(true);
					current_state = TurningOnState;
				}
				else {
					// Should be unnecessary, but doesn't hurt
					StopTransmitting();
				}
				break;

			case TurningOnState:
				// We're now transmitting "turn on".  Keep doing that
				// until the current drops below the lower threshold
				if (GetAnalogueCurrent() < CURRENT_HYSTERESIS_LOW) {
					// Current has gone low, leave the vacuum cleaner
					// running for a little while to catch the last
					// bits of sawdust
					current_state = DelayState;
					state_timer = GetMillisecondCounter();
				}
				break;

			case DelayState:
				if (GetAnalogueCurrent() > CURRENT_HYSTERESIS_LOW) {
					// Current didn't stay below lower threshold,
					// so go back to turn-on state
					current_state = TurningOnState;
				}
				else if (MillisecondsHaveElapsed(state_timer, 2000)) {
					// Current has been low for two seconds now,
					// so start sending the "turn off" command
					current_state = TurningOffState;
					StartTransmitting(false);
					state_timer = GetMillisecondCounter();
				}
				else {
					// Just wait
				}
				break;

			case TurningOffState:
				if (GetAnalogueCurrent() > CURRENT_HYSTERESIS_HIGH) {
					// Current has gone back high again, so
					// go straight back to turning on
					current_state = TurningOnState;
				}
				else if (MillisecondsHaveElapsed(state_timer, 2000)) {
					// We've been transmitting "turn off" for two
					// seconds now: if it hasn't worked by now it
					// probably won't!
					current_state = IdleState;
					StopTransmitting();
				}
				else {
				}
				break;
		}
	}
}
