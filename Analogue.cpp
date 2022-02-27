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

// Analogue to Digital Converter Driver

#include "Global.h"
#include "cmsis.h"
#include "Analogue.h"
#include "Pins.h"
#include "DefinedPins.h"
#include "tinyprintf.h"
#include "PrintSupport.h"

#include <assert.h>

#define ADC_MAX ((uint16_t) 0x0FFFU)

// This variable is used to perform basic filtering on the measured
// data
static uint16_t averaged_adc_reading = (ADC_MAX >> 1);

#if not defined(STM32F411xE)
#error This analogue driver is for the F411xE
#endif

void InitAnalogue()
{
	// Initialise the peripheral (runs once on startup)
	ADC1->CR1 = (uint32_t) 0U
		| ADC_CR1_SCAN; // Scan through all selected channels automatically
	ADC1->CR2 = (uint32_t) 0U
		| ADC_CR2_ADON; // Turn the ADC on
	// Sample rate for channels - 7 is 239.5 cycles or about 20 us.
	ADC1->SMPR2 = (uint32_t) 0U
		| (0x7U << ADC_SMPR2_SMP9_Pos); // Sample rate for channel 9 (PB1)
	// Sequence register - 1 channel to sample
	ADC1->SQR1 = (uint32_t) 0U
		| (0 << ADC_SQR1_L_Pos); // Sample 1 channel only
	// Sequence register - sample channel 9
	ADC1->SQR3 = (uint32_t) 0U
		| (9U << ADC_SQR3_SQ1_Pos) // SQ1 (first channel to sample) is #9 (PB1)
		;
	ADC1_COMMON->CCR |=
		// ADC Prescaler PCLK2 / 8 -> ADC CLK is 12 MHz
		(0x3U << ADC_CCR_ADCPRE_Pos);

	// Configure the I/O pin as an analogue input
	SetPinAsAnalogueIn(ANALOGUE_INPUT_PIN);
}

// 2^6 = 64 sample averaging
#define SUM_SHIFT 6
#define NUM_SAMPLES (1 << SUM_SHIFT)

void UpdateAnalogue()
{
	// Runs every millisecond and takes a single sample

	// Take one sample every millisecond (or whatever the main loop time is set to)
	// and then average over NUM_SAMPLES to give some noise immunity
	// With NUM_SAMPLES=64, this gives a new measurement every 64 milliseconds,
	// which is easily often enough for this application.
	static uint16_t sample_history[NUM_SAMPLES];
	static uint16_t sample_index = 0;

	// This variable is used to make sure we don't try to produce a reading until we
	// have filled the buffer for the first time
	static bool filled_buffer = false;

	static bool started = false;

	uint32_t sum;

	if ( ! started ) {
		// Start conversion - needs to be 1 us after turning it on
		// and needs to be the only bit written in this operation
		ADC1->CR2 |= ADC_CR2_ADON;
		ADC1->CR2 |= ADC_CR2_SWSTART;
		started = true;
		return;
	}

	if ((ADC1->SR & ADC_SR_EOC) != 0) {
		// Data ready (end of conversion) flag has been set, so get the latest value
		sample_history[sample_index] = (uint16_t) ADC1->DR;

		// Increment (with wrap) the index into the history buffer
		sample_index += 1;
		if (sample_index >= NUM_SAMPLES) {
			filled_buffer = true;
			sample_index = 0;
		}

		// As long as we've filled the buffer at least once, we can calculate
		// an average reading.
		if (filled_buffer) {
			sum = 0;
			for (int si=0;si<NUM_SAMPLES;si++) {
				sum += sample_history[si];
			}

			averaged_adc_reading = (uint16_t) (sum >> SUM_SHIFT);
		}

		// Start the next conversion
		ADC1->CR2 |= ADC_CR2_SWSTART;
	}
}

// Current is returned as absolute value but in ADC units
// 1 LSB is about 24 mA; however, the zero reference is
// unlikely to be very accurate - tests showed at least
// 300 mA recorded with no current flowing.
uint16_t GetAnalogueCurrent()
{
	int32_t zeroed = averaged_adc_reading - (ADC_MAX >> 1);
	if (zeroed >= 0) {
		return (uint16_t) zeroed;
	}
	else {
		return (uint16_t) (-zeroed);
	}
}
