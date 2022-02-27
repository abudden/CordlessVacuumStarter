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

// Switch functions

#include "Global.h"
#include "cmsis.h"

#include "SwitchDebounce.h"
#include "Switches.h"

#include "Pins.h"
#include "DefinedPins.h"
#include "tinyprintf.h"

#include <assert.h>

#define SWITCH_COUNT (((int) LastSwitchIndex)+1)

// Strings for debugging
static const char *ON_STRING = "on";
static const char *OFF_STRING = "off";

// List of supported switches
static const struct {
	SwitchName name;
	GPIO_TypeDef *port;
	uint8_t pin;
	bool momentary;
	bool false_is_pressed;
	const char *displayname;
} SwitchList[SWITCH_COUNT] = {
	{PushButtonSwitch,  PUSH_BUTTON_PIN,    true,  true, "Push Button"},
};

// States of each switch
static enum {
	SwitchOff = 0,
	SwitchHeld,
	SwitchReleased
} momentary_states[SWITCH_COUNT];

// Debouncer implementations for each switch (instantiated in InitSwitches)
static SwitchDebounce *debouncers[SWITCH_COUNT];

void InitSwitches()
{
	int i;
	for (i=0;i<SWITCH_COUNT;i++) {
		// Check that the switches are in the right order in the array
		assert(SwitchList[i].name == ((int) i));

		debouncers[i] = new SwitchDebounce(SwitchList[i].port, SwitchList[i].pin);
		momentary_states[i] = SwitchOff;
	}
}

void UpdateSwitches()
{
	// Update each debouncer and handle momentary switch monitoring
	for (int i=0;i<SWITCH_COUNT;i++) {
		debouncers[i]->Update();
		if (SwitchList[i].momentary) {
			bool switch_state = debouncers[i]->GetState();
			if (SwitchList[i].false_is_pressed) {
				switch_state = ! switch_state;
			}
			if ((momentary_states[i] == SwitchOff) && switch_state) {
				momentary_states[i] = SwitchHeld;
			}
			else if ((momentary_states[i] == SwitchHeld) && ( ! switch_state)) {
				momentary_states[i] = SwitchReleased;
			}
			else {
				// Do nothing
			}
		}
	}
}

bool GetSwitchState(SwitchName name)
{
	bool switch_state = debouncers[(int) name]->GetState();
	if (SwitchList[(int) name].false_is_pressed) {
		switch_state = ! switch_state;
	}
	return switch_state;
}

const char * GetSwitchStateString(SwitchName name)
{
	if (GetSwitchState(name)) {
		return ON_STRING;
	}
	else {
		return OFF_STRING;
	}
}

bool HasReceivedMomentaryButton(SwitchName name)
{
	if (SwitchList[(int) name].momentary) {
		if (momentary_states[(int) name] == SwitchReleased) {
			momentary_states[(int) name] = SwitchOff;
			return true;
		}
	}
	return false;
}

bool GetPushButtonState()
{
	return GetSwitchState(PushButtonSwitch);
}
