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

// Functions required to make printf and suchlike work

#include <stdio.h>
#include <errno.h>
#include <sys/unistd.h> // STDOUT_FILENO, STDERR_FILENO

#include "Global.h"
#include "Uart.h"
#include "CircularBuffer.h"
#include "tinyprintf.h"

// UART to make printf etc work
static Uart uart;

void putcfunc(void *dummy, char ch) {
	(void) dummy;
	uart.outgoingBuffer->addEntry((DTYPE) ch);
}

void printchar(char ch) {
	putcfunc(NULL, ch);
}

bool bytes_waiting() {
	return uart.incomingBuffer->containsData();
}

DTYPE get_incoming_byte() {
	return uart.incomingBuffer->getEntry();
}

void InitPrintSupport()
{
	setbuf(stdout, NULL);
	uart.Init();
	init_printf(NULL, &putcfunc);
}

void UpdatePrintSupport()
{
	uart.Update();
}

void putstring(const char *data) {
	int index = 0;
	while (data[index]) {
		putchar(data[index]);
		index++;
	}
}

/*
 * fputc redefinition; used by printf
 */
extern "C" int fputc(int ch, FILE *f)
{
	/* Write a character to the USART */
	uart.outgoingBuffer->addEntry((DTYPE) ch);

	return ch;
}

/* _write redefinition - used by GCC printf */
extern "C" int _write(int file, char *data, int len)
{
	if ((file != STDOUT_FILENO) && (file != STDERR_FILENO)) {
		errno = EBADF;
		return -1;
	}
	int writeCount = 0;
	int space = uart.outgoingBuffer->getSpace();

	while ((len > 0) && (space > 0)) {
		uart.outgoingBuffer->addEntry((DTYPE) data[writeCount]);
		writeCount++;
		space--;
		len--;
	}

	return writeCount;
}
