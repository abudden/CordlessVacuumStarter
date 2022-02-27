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

// Clock configuration

#include "Clock.h"
#include "cmsis.h"
#include "Pins.h"
#include "DefinedPins.h"

#define RCC_CR_Default       ((uint32_t) 0x00000081)
#define RCC_CFGR_Default     ((uint32_t) 0x24003010)

#if defined(WEACT_BLACKPILL_F411CE)
#warning Compiling for F411CE Black Pill
#elif defined(ST_NUCLEO_F411RE)
#warning Compiling for Nucleo F411RE
#else
#error Unknown target
#endif

static volatile uint32_t MillisecondCounter = 0U;
static uint8_t ClockSpeedMHz = 0U;

extern "C" void SysTick_Handler(void)
{
	// Read the control register (clears the interrupt flag)
	uint32_t dummy = SysTick->CTRL;
	(void) dummy; // Get rid of a compiler warning

	// This interrupt runs every millisecond: used for duration timing etc
	MillisecondCounter++;

	// For some reason that I don't currently understand, if this line is
	// removed, the "millisecond interrupt" only runs every 2 ms instead of
	// every 1.  I suspect it's either down to an unfortunate compiler
	// optimisation being prevented by the volatile write in the pin toggle
	// function, or (less likely) just increasing the time before the SCR bit
	// is set.
	TogglePin(CLOCK_RANDOM_PIN);

	/* Do not return to wait mode after exiting this interrupt */
	SCB->SCR &= (uint32_t) ~((uint32_t) SCB_SCR_SLEEPONEXIT_Msk);
}

uint32_t GetMillisecondCounter(void)
{
	uint32_t local_copy = MillisecondCounter;
	return local_copy;
}

bool MillisecondsHaveElapsed(uint32_t start_time, uint32_t duration)
{
	// Local copy of volatile
	uint32_t current_time = MillisecondCounter;
	uint32_t end_time = start_time + duration;
	if ((end_time >= start_time) &&             // End time hasn't wrapped AND
	        ((current_time >= end_time) ||      // system time has passed the end time OR
	            (current_time < start_time)))   // system time has wrapped
	{
		return true;
	}
	else if ((end_time <= start_time) &&        // End time has wrapped AND
	        ((current_time >= end_time) &&      // system time has passed the end time AND
	            (current_time < start_time)))   // system time has wrapped
	{
		return true;
	}
	else {
		return false;
	}
}

uint32_t ElapsedMilliseconds(uint32_t start_time)
{
	// Local copy of volatile
	uint32_t current_time = MillisecondCounter;
	if (current_time > start_time) {
		return (current_time - start_time);
	}
	else {
		return ((UINT32_MAX - start_time) + current_time + 1U);
	}
}

void SetupClocks(void)
{
	SetPinAsGPO_PP(CLOCK_RANDOM_PIN);

	// Select internal oscillator for configuration process
	RCC->CFGR &= ~RCC_CFGR_SW;
	RCC->CFGR |= RCC_CFGR_SW_HSI;
	while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_HSI) {
		// Wait until internal oscillator is in use
	}

	// Turn off PLL
	RCC->CR &= ~RCC_CR_PLLON;
	// Reset registers to default values
	RCC->CR = RCC_CR_Default;
	RCC->CFGR = RCC_CFGR_Default;

#if defined(ST_NUCLEO_F411RE)
	// Use the clock from the ST-Link as the crystal isn't fitted by default
	RCC->CR |= RCC_CR_HSEBYP;
#endif

	// Start external oscillator
	RCC->CR |= RCC_CR_HSEON;
	while ((RCC->CR & RCC_CR_HSERDY) != RCC_CR_HSERDY) {
		// Wait until HSE is oscillating
	}

	// Speed up flash access
	FLASH->ACR |= FLASH_ACR_PRFTEN;

	// 2 wait states (required for 72 MHz operation - see table 5 in reference manual RM0383)
	FLASH->ACR &= ~(FLASH_ACR_LATENCY_Msk);
	FLASH->ACR |= (0x02U << FLASH_ACR_LATENCY_Pos);

	RCC->PLLCFGR &= ~(RCC_PLLCFGR_PLLSRC | RCC_PLLCFGR_PLLP_Msk | RCC_PLLCFGR_PLLN_Msk | RCC_PLLCFGR_PLLM_Msk);

#if defined(ST_NUCLEO_F411RE)
#warning Configuring for 8 MHz crystal
	RCC->PLLCFGR = (uint32_t) 0
		| (0x1U << RCC_PLLCFGR_PLLSRC_Pos) // Set source to PLL as HSE clock (8 MHz)
		| (4U << RCC_PLLCFGR_PLLM_Pos)     // Divide source by 4 to get 2 MHz input clock as specified
		| (144U << RCC_PLLCFGR_PLLN_Pos)   // Multiply 2 MHz input by 144 to get 288 MHz VCO frequency
		| (0x01U << RCC_PLLCFGR_PLLP_Pos)  // Divide VCO clock by 4 to get 72 MHz SYSCLK
		| (0x06U << RCC_PLLCFGR_PLLQ_Pos)  // Divide VCO clock by 6 to get 48 MHz clock for USB OTG FS and SDIO
	;

	RCC->CFGR = (uint32_t) 0
		| (0U << RCC_CFGR_HPRE_Pos)        // Don't divide AHB from SYSCLK - 72 MHz
		| (0x4U << RCC_CFGR_PPRE1_Pos)     // Divide AHB by 2 to get APB1 36 MHz (can't go above 50 MHz)
		                                   // Note that APB1 timer clocks run off double this frequency
		| (0x0U << RCC_CFGR_PPRE2_Pos)     // Divide AHB by 1 to get APB2 72 MHz (can't go above 100 MHz)
		                                   // Note that APB2 timer clocks run off this frequency without doubling
		| (0x8U << RCC_CFGR_RTCPRE_Pos)    // Divide HSE by 8 to get RTC Clock 1 MHz (not used at present)
	;
#elif defined(WEACT_BLACKPILL_F411CE)
#warning Configuring for 25 MHz crystal
	RCC->PLLCFGR = (uint32_t) 0
		| (0x1U << RCC_PLLCFGR_PLLSRC_Pos) // Set source to PLL as HSE clock (25 MHz)
		| (25U << RCC_PLLCFGR_PLLM_Pos)    // Divide source by 25 to get 1 MHz input clock as specified
		| (288U << RCC_PLLCFGR_PLLN_Pos)   // Multiply 1 MHz input by 288 to get 288 MHz VCO frequency
		| (0x01U << RCC_PLLCFGR_PLLP_Pos)  // Divide VCO clock by 4 to get 72 MHz SYSCLK
		| (0x06U << RCC_PLLCFGR_PLLQ_Pos)  // Divide VCO clock by 6 to get 48 MHz clock for USB OTG FS and SDIO
	;

	RCC->CFGR = (uint32_t) 0
		| (0U << RCC_CFGR_HPRE_Pos)        // Don't divide AHB from SYSCLK - 72 MHz
		| (0x4U << RCC_CFGR_PPRE1_Pos)     // Divide AHB by 2 to get APB1 36 MHz (can't go above 50 MHz)
		                                   // Note that APB1 timer clocks run off double this frequency
		| (0x0U << RCC_CFGR_PPRE2_Pos)     // Divide AHB by 1 to get APB2 72 MHz (can't go above 100 MHz)
		                                   // Note that APB2 timer clocks run off this frequency without doubling
		| (0x25U << RCC_CFGR_RTCPRE_Pos)   // Divide HSE by 25 to get RTC Clock 1 MHz (not used at present)
	;
#endif

	/* Enable PLL and wait until ready */
	RCC->CR |= RCC_CR_PLLON;
	while ((RCC->CR & RCC_CR_PLLRDY) != RCC_CR_PLLRDY)
	{
	}

	RCC->CFGR |= RCC_CFGR_SW_PLL;
	while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL) {
		// Wait until PLL is used
	}

	SysTick_Config(72000); // AHB ( = 72 MHz) / 72000 = 1 kHz
	ClockSpeedMHz = 72;
}

uint8_t GetClockSpeedMHz(void)
{
	return ClockSpeedMHz;
}

