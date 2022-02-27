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

#include <stdint.h>
#include "cmsis.h"
#include "Pins.h"
#include "tinyprintf.h"

void SetPinAsGPO_PP(GPIO_TypeDef *port, uint8_t pin)
{
	if (pin > 15) {
		return;
	}
	// GPO
	uint32_t mode_mask = GPIO_MODER_MODE0_Msk << (pin*2);
	// MODE: 01 - GPO
	uint32_t mode_set = GPIO_MODER_MODE0_0 << (pin*2);
	port->MODER &= ~(mode_mask);
	port->MODER |= mode_set;
	// High speed:
	uint32_t speed_mask = GPIO_OSPEEDR_OSPEED0_Msk << (pin*2);
	uint32_t speed_set = (GPIO_OSPEEDR_OSPEED0_1 | GPIO_OSPEEDR_OSPEED0_0) << (pin*2);
	port->OSPEEDR &= ~(speed_mask);
	port->OSPEEDR |= speed_set;
	// Push-pull
	port->OTYPER &= ~(1U << pin);
	// No pull-up/down
	uint32_t pupd_mask = GPIO_PUPDR_PUPD0_Msk << (pin*2);
	uint32_t pupd_set = 0U << (pin*2);
	port->PUPDR &= ~(pupd_mask);
	port->PUPDR |= pupd_set;
}

void SetPinAsInputPullUp(GPIO_TypeDef *port, uint8_t pin)
{
	if (pin > 15) {
		return;
	}

	// TODO
}

void SetPinAsInputFloat(GPIO_TypeDef *port, uint8_t pin)
{
	if (pin > 15) {
		return;
	}
	// GPO
	uint32_t mode_mask = GPIO_MODER_MODE0_Msk << (pin*2);
	// MODE: 00 - Input
	port->MODER &= ~(mode_mask);
	// No pull-up/down
	uint32_t pupd_mask = GPIO_PUPDR_PUPD0_Msk << (pin*2);
	uint32_t pupd_set = 0U << (pin*2);
	port->PUPDR &= ~(pupd_mask);
	port->PUPDR |= pupd_set;
}

void SetPinAsAFO_PP(GPIO_TypeDef *port, uint8_t pin, uint8_t afnum)
{
	if ((pin > 15) || (afnum > 15)) {
		return;
	}
	// GPO
	uint32_t mode_mask = GPIO_MODER_MODE0_Msk << (pin*2);
	// MODE: 10 - AFO
	uint32_t mode_set = GPIO_MODER_MODE0_1 << (pin*2);
	port->MODER &= ~(mode_mask);
	port->MODER |= mode_set;
	// High speed:
	uint32_t speed_mask = GPIO_OSPEEDR_OSPEED0_Msk << (pin*2);
	uint32_t speed_set = (GPIO_OSPEEDR_OSPEED0_1 | GPIO_OSPEEDR_OSPEED0_0) << (pin*2);
	port->OSPEEDR &= ~(speed_mask);
	port->OSPEEDR |= speed_set;
	// Push-pull
	port->OTYPER &= ~(1U << pin);
	// No pull-up/down
	uint32_t pupd_mask = GPIO_PUPDR_PUPD0_Msk << (pin*2);
	uint32_t pupd_set = 0U << (pin*2);
	port->PUPDR &= ~(pupd_mask);
	port->PUPDR |= pupd_set;

	if (pin < 8) {
		port->AFR[0] &= ~(GPIO_AFRL_AFSEL0_Msk << (pin*4));
		port->AFR[0] |= (afnum) << (pin*4);
	}
	else {
		port->AFR[1] &= ~(GPIO_AFRH_AFSEL8_Msk << ((pin-8)*4));
		port->AFR[1] |= (afnum) << ((pin-8)*4);
	}
}

void SetPinAsAnalogueIn(GPIO_TypeDef *port, uint8_t pin)
{
	if (pin > 15) {
		return;
	}
	// GPO
	uint32_t mode_mask = GPIO_MODER_MODE0_Msk << (pin*2);
	// MODE: 11 - Analogue
	uint32_t mode_set = (GPIO_MODER_MODE0_1 | GPIO_MODER_MODE0_0) << (pin*2);
	port->MODER &= ~(mode_mask);
	port->MODER |= mode_set;
	// Medium speed:
	uint32_t speed_mask = GPIO_OSPEEDR_OSPEED0_Msk << (pin*2);
	uint32_t speed_set = GPIO_OSPEEDR_OSPEED0_0 << (pin*2);
	port->OSPEEDR &= ~(speed_mask);
	port->OSPEEDR |= speed_set;
	// Push-pull
	port->OTYPER &= ~(1U << pin);
	// No pull-up/down
	uint32_t pupd_mask = GPIO_PUPDR_PUPD0_Msk << (pin*2);
	uint32_t pupd_set = 0U << (pin*2);
	port->PUPDR &= ~(pupd_mask);
	port->PUPDR |= pupd_set;
}

void SetPinState(GPIO_TypeDef *port, uint8_t pin, bool high)
{
	if (pin > 15) {
		return;
	}
	if (high) {
		port->ODR |= (0x1U << pin);
	}
	else {
		port->ODR &= ~(0x1U << pin);
	}
}

bool GetPinState(GPIO_TypeDef *port, uint8_t pin)
{
	uint16_t mask = 0x1U << pin;
	if (pin > 15) {
		return false;
	}
	if ((port->IDR & mask) == mask) {
		return true;
	}
	else {
		return false;
	}
}

void TogglePin(GPIO_TypeDef *port, uint8_t pin)
{
	if (pin > 15) {
		return;
	}
	port->ODR ^= (0x1U << pin);
}
