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

// Switch Debouncing Functions

#include "Global.h"
#include "cmsis.h"
#include "SwitchDebounce.h"
#include "Pins.h"

#define DEFAULT_STATE false
#define DEBOUNCE_MS ((uint_fast8_t) 100)

SwitchDebounce::SwitchDebounce(GPIO_TypeDef *port, uint8_t pin)
{
	this->sw_port = port;
	this->sw_pin = pin;

	SetPinAsInputFloat(port, pin);
	this->last_state = GetPinState(port, pin);
	this->initialised = false;
	this->validated_state = ! this->last_state;
}

void SwitchDebounce::Update()
{
	bool current_state = GetPinState(this->sw_port, this->sw_pin);
	if ((current_state != this->validated_state) || ( ! this->initialised)) {
		if (current_state != this->last_state) {
			this->counter = 0;
			this->last_state = current_state;
		}
		else if (this->counter >= DEBOUNCE_MS) {
			this->validated_state = current_state;
			this->initialised = true;
		}
		else {
			this->counter++;
		}
	}
}

bool SwitchDebounce::GetState()
{
	if (this->initialised) {
		return this->validated_state;
	}
	else {
		return DEFAULT_STATE;
	}
}

bool SwitchDebounce::IsInitialised()
{
	return this->initialised;
}
