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

#include "CircularBuffer.h"

void CircularBuffer::addEntry(DTYPE data) {
	if (this->isFull()) {
		return;
	}

	this->buffer[this->writeIndex] = data;
	if (this->writeIndex >= (this->bufLen-1)) {
		this->writeIndex = 0;
	}
	else {
		this->writeIndex++;
	}
}

DTYPE CircularBuffer::getEntry(void) {
	if (this->isEmpty()) {
		return 0;
	}

	DTYPE result = this->buffer[this->readIndex];
	if (this->readIndex >= (this->bufLen-1)) {
		this->readIndex = 0;
	}
	else {
		this->readIndex++;
	}
	return result;
}

bool CircularBuffer::isEmpty(void) {
	if (this->readIndex == this->writeIndex) {
		return true;
	}
	else {
		return false;
	}
}

bool CircularBuffer::containsData(void) {
	return (! this->isEmpty());
}

bool CircularBuffer::isFull(void) {
	if ((this->readIndex == (this->writeIndex+1))
			|| ((this->readIndex == 0) && (this->writeIndex == (this->bufLen-1)))) {
		return true;
	}
	else {
		return false;
	}
}

uint16_t CircularBuffer::getNumEntries(void) {
	if (this->writeIndex >= this->readIndex) {
		return (this->writeIndex - this->readIndex);
	}
	else {
		return ((this->bufLen - this->readIndex) + this->writeIndex);
	}
}

uint16_t CircularBuffer::getSpace(void) {
	return (this->bufLen-1) - this->getNumEntries();
}

void CircularBuffer::clear(void) {
	this->readIndex = 0;
	this->writeIndex = 0;
}

