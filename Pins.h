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

// Pin configuration / drive functions

#ifndef PINS_H
#define PINS_H

#include <stdint.h>
#include "cmsis.h"

void SetPinAsGPO_PP(GPIO_TypeDef *port, uint8_t pin);
void SetPinAsInputPullUp(GPIO_TypeDef *port, uint8_t pin);
void SetPinAsInputFloat(GPIO_TypeDef *port, uint8_t pin);
void SetPinAsAFO_PP(GPIO_TypeDef *port, uint8_t pin, uint8_t afnum);
void SetPinAsAnalogueIn(GPIO_TypeDef *port, uint8_t pin);
void SetPinState(GPIO_TypeDef *port, uint8_t pin, bool high);
bool GetPinState(GPIO_TypeDef *port, uint8_t pin);
void TogglePin(GPIO_TypeDef *port, uint8_t pin);

#endif
