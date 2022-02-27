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

// Switch functions

#ifndef SWITCHES_H
#define SWITCHES_H

typedef enum _Switches
{
	PushButtonSwitch,
	LastSwitchIndex = PushButtonSwitch
} SwitchName;

void InitSwitches();
void UpdateSwitches();

bool GetSwitchState(SwitchName name);
const char *GetSwitchStateString(SwitchName name);
bool HasReceivedMomentaryButton(SwitchName name);

// Debug functions
bool GetPushButtonState();

#endif
