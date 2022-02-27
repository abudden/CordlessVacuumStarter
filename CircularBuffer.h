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

// A simple circular buffer implementation, used as part of the debug / uart system

#ifndef CIRCULARBUFFER_H
#define CIRCULARBUFFER_H

#ifndef CIRCULARBUFFER_LENGTH
#define CIRCULARBUFFER_LENGTH 1000
#endif

#include <stdint.h>

// I could have made this a template I guess, but I decided it
// was easier to just use a #defined data type
#define DTYPE uint8_t

class CircularBuffer
{
	public:
		void addEntry(DTYPE data);
		DTYPE getEntry(void);
		bool isEmpty(void);
		bool containsData(void);
		bool isFull(void);
		uint16_t getNumEntries(void);
		uint16_t getSpace(void);
		void clear(void);

	private:
		static const uint16_t bufLen = CIRCULARBUFFER_LENGTH;
		DTYPE buffer[CIRCULARBUFFER_LENGTH];
		uint16_t readIndex;
		uint16_t writeIndex;
};

#endif
