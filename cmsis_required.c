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

// Stuff that's required by the cmsis headers

#include <stdlib.h>
#include <stddef.h>
#include <reent.h>
#include "cmsis.h"

void SystemInit(void)
{
	// Make sure peripheral power is turned on
	RCC->AHB1LPENR = (uint32_t) 0
		| RCC_AHB1LPENR_GPIOALPEN
		| RCC_AHB1LPENR_GPIOBLPEN
		| RCC_AHB1LPENR_GPIOCLPEN
		| RCC_AHB1LPENR_FLITFLPEN  /* Enable in sleep mode */
		| RCC_AHB1LPENR_SRAM1LPEN;  /* Enable in sleep mode */
	RCC->APB1LPENR = (uint32_t) 0
		| RCC_APB1LPENR_USART2LPEN
		| RCC_APB1LPENR_TIM2LPEN;
	RCC->APB2LPENR = (uint32_t) 0
		| RCC_APB2LPENR_SYSCFGLPEN
		| RCC_APB2LPENR_ADC1LPEN;
	RCC->AHB1ENR = (uint32_t) 0
		| RCC_AHB1ENR_GPIOAEN
		| RCC_AHB1ENR_GPIOBEN
		| RCC_AHB1ENR_GPIOCEN;
	RCC->APB1ENR = (uint32_t) 0
		| RCC_APB1ENR_USART2EN
		| RCC_APB1ENR_TIM2EN
		| RCC_APB1ENR_PWREN;
	RCC->APB2ENR = (uint32_t) 0
		| RCC_APB2ENR_SYSCFGEN
		| RCC_APB2ENR_ADC1EN;
}

extern int __real_main(void);
int __wrap_main(void)
{
	return __real_main();
}

void __wrap_exit(int return_code)
{
	while (1) {}
}

extern void *__real__malloc_r(struct _reent *r, size_t size);
extern void __real__free_r(struct _reent *r, void *ptr);
extern void *__real__realloc_r(struct _reent *r, void *ptr, size_t size);
extern void *__real__calloc_r(struct _reent *r, size_t nmemb, size_t size);

void *__wrap__malloc_r(struct _reent *r, size_t size)
{
	void *ptr = NULL;
	ptr = __real__malloc_r(r, size);
	return ptr;
}

void __wrap__free_r(struct _reent *r, void *ptr)
{
	__real__free_r(r, ptr);
}


void *__wrap__realloc_r(struct _reent *r, void *ptr, size_t size)
{
	void *new_ptr = NULL;
	new_ptr = __real__realloc_r(r, ptr, size);
	return new_ptr;
}

void *__wrap__calloc_r(struct _reent *r, size_t nmemb, size_t size)
{
	void *ptr = NULL;
	ptr = __real__calloc_r(r, nmemb, size);
	return ptr;
}

void __HAL_RCC_AFIO_CLK_ENABLE(void) {
	/* Called by hal_gpio, but implemented elsewhere */
}

void __HAL_RCC_SYSCFG_CLK_ENABLE(void) {
	/* Called by hal_gpio, but implemented elsewhere */
}
