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

// Transmitter code to control remote-controlled switches

#ifndef TRANSMITTER_H
#define TRANSMITTER_H

void InitTransmitter();
void UpdateTransmitter();
void StartTransmitting(bool on);
void StopTransmitting();
void NextTransmitterState();
uint8_t GetTransmitterState();
uint32_t GetTransmitWord();
void StartTransmittingValue(uint16_t value);
uint8_t IsTransmitting();

#endif
