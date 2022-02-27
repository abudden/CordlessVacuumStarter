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

#ifndef SWITCHDEBOUNCE_H
#define SWITCHDEBOUNCE_H

#include "cmsis.h"

class SwitchDebounce
{
	public:
		SwitchDebounce(GPIO_TypeDef *port, uint8_t pin);
		void Update();
		bool GetState();
		bool IsInitialised();

	private:
		GPIO_TypeDef *sw_port;
		uint8_t sw_pin;

		bool last_state;
		bool validated_state;
		bool initialised;
		uint_fast8_t counter;
};

#endif
