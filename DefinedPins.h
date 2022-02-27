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

// A place for all pin definitions to live, to make it easier to
// see which pins have been used and which haven't.

#ifndef DEFINEDPINS_H
#define DEFINEDPINS_H

// PORT A

// Push button switch (pulls down on press)
#if defined(WEACT_BLACKPILL_F411CE)
#define PUSH_BUTTON_PIN         GPIOA, 0U
#elif defined(ST_NUCLEO_F411RE)
#define PUSH_BUTTON_PIN         GPIOC, 13
#endif

// Transmit output (T2CH2)
#define TRANSMIT_PIN            GPIOA, 1U
// USB UART TX
#define UART_TX_PIN             GPIOA, 2U
// USB UART RX
#define UART_RX_PIN             GPIOA, 3U

// PORT B
// Analogue input
#define ANALOGUE_INPUT_PIN      GPIOB, 1U

// PORT C

// LED
// Used for debug - loop timing
#define LOOPTIME_PIN            GPIOB, 10U
// A random pin needed by the CMSIS clock code for some unknown reason
#define CLOCK_RANDOM_PIN        GPIOC, 15U

#if defined(WEACT_BLACKPILL_F411CE)
// The LED on the black pill board
#define LED_PIN                 GPIOC, 13U
#elif defined(ST_NUCLEO_F411RE)
#define LED_PIN                 GPIOA, 5U
#endif

#endif
