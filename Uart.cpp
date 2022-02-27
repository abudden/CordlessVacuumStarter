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

// UART driver

#include "Global.h"
#include "cmsis.h"
#include "Uart.h"
#include "CircularBuffer.h"
#include "Pins.h"
#include "DefinedPins.h"
#include "Clock.h"
#include <assert.h>

#if defined(WEACT_BLACKPILL_F411CE) or defined(ST_NUCLEO_F411RE)
#define BASE_BAUD_DIVIDER ((uint16_t) 625) // 72 MHz clock divided by 625 gives 115200 baud
#else
#error Unknown clock frequency; cannot configure UART
#endif

/** Determine whether a byte has been received by the selected UART. */
#define byteReceived(USART)  ((((USART->SR) & USART_SR_RXNE) == USART_SR_RXNE) && (((USART->CR1) & USART_CR1_RXNEIE) == USART_CR1_RXNEIE))
/** Determine whether the transmit buffer is empty for the selected UART. */
#define transmitEmpty(USART) ((((USART->SR) & USART_SR_TXE) == USART_SR_TXE) && (((USART->CR1) & USART_CR1_TXEIE) == USART_CR1_TXEIE))
#define IRQEnableTransmitter(USART)     DO(USART->CR1 |= USART_CR1_TXEIE;)
#define IRQEnableReceiver(USART)        DO(USART->CR1 |= USART_CR1_RXNEIE;)
#define IRQDisableTransmitter(USART)    DO(USART->CR1 &= (uint16_t) ~USART_CR1_TXEIE;)
#define IRQDisableReceiver(USART)       DO(USART->CR1 &= (uint16_t) ~USART_CR1_RXNEIE;)

/* Small buffer for ISR to use with volatile data/indices */
#define ISRBUFSIZE 20u
static volatile uint8_t isrRxBuffer[ISRBUFSIZE];
static uint8_t isrTxBuffer[ISRBUFSIZE];
static uint16_t isrTxWriteIndex;
static volatile uint16_t isrTxReadIndex;
static volatile uint16_t isrRxWriteIndex;
static uint16_t isrRxReadIndex;
static const uint16_t isrBufLen = ISRBUFSIZE;

#define USB_UART

#ifdef USB_UART
#define UART_NUMBER 2
#else
#define UART_NUMBER 6
#endif

#if UART_NUMBER == 2
// UART 2 runs of APB1, which is half the speed of APB2
// +1 is due to rounding down effect of right-shift - the
// numbers work better with the plus 1.
#define BAUD_DIVIDER ((uint16_t) ((BASE_BAUD_DIVIDER+1) >> 1))
#define UART_IRQHandler USART2_IRQHandler
#define UART_STRUCT USART2
#define UART_TX_PIN_AF UART_TX_PIN, 7
#define UART_RX_PIN_AF UART_RX_PIN, 7
#define UART_IRQ USART2_IRQn
#elif UART_NUMBER == 6
// UART 6 runs of APB2
#define BAUD_DIVIDER BASE_BAUD_DIVIDER
#define UART_IRQHandler USART6_IRQHandler
#define UART_STRUCT USART6
#define UART_TX_PIN_AF GPIOC, 6, 8
#define UART_RX_PIN_AF GPIOC, 7, 8
#define UART_IRQ USART6_IRQn
#else
#error Unrecognised UART number
#endif

static bool rxFull(void);

extern "C" void UART_IRQHandler(void);
extern "C" void UART_IRQHandler(void)
{
	uint8_t rxData;
	USART_TypeDef *USART = UART_STRUCT;

	/* --------------------------------------------- */
	/* Byte received */
	if (byteReceived(USART))
	{
		/* Get data; clears interrupt and error flags */
		rxData = (uint8_t) USART->DR;

		/* Store data in buffer, if space */
		if (!rxFull())
		{
			/* Store data in buffer */
			isrRxBuffer[isrRxWriteIndex] = rxData;

			/* Circular buffer write address */
			isrRxWriteIndex++;

			if (isrRxWriteIndex >= isrBufLen)
			{
				isrRxWriteIndex = 0;
			} /* if */
		} /* if */

		/* Turn off receive interrupt if buffer is full */
		if (rxFull())
		{
			IRQDisableReceiver(USART);
		} /* if */
	} /* if */

	/* --------------------------------------------- */
	/* Byte to send */
	if (transmitEmpty(USART))
	{
		/* If buffer is not empty... */
		if (isrTxReadIndex != isrTxWriteIndex)
		{
			/* Transmit byte */
			USART->DR = isrTxBuffer[isrTxReadIndex];

			/* Circular buffer read address */
			isrTxReadIndex++;
			if (isrTxReadIndex >= isrBufLen)
			{
				isrTxReadIndex = 0;
			} /* if */

		} /* if */
		else
		{
			/* If no data left, disable Tx interrupt */
			IRQDisableTransmitter(USART);
		} /* else */
	} /* if */
}

void Uart::Init()
{
	USART_TypeDef *USART = UART_STRUCT;
	this->outgoingBuffer = new CircularBuffer();
	this->incomingBuffer = new CircularBuffer();

	isrTxReadIndex = 0;
	isrTxWriteIndex = 0;
	isrRxReadIndex = 0;
	isrRxWriteIndex = 0;

	/* Set up USART */
	USART->BRR = BAUD_DIVIDER;
	USART->CR3 = 0x00; /* Default; no flow control or special features */
	USART->CR2 = USART_CR2_LBDL; /* detect break after 11 bits */

	USART->CR1 = USART_CR1_UE | USART_CR1_TE | USART_CR1_RE;

	/* Enable Rx interrupt */
	USART->CR1 |= USART_CR1_RXNEIE;

	/* Set up the GPIO ports - PA2 is TX, PA3 is RX for UART2;
	 * PC6 is TX, PC7 is RX for UART6 */
	SetPinAsAFO_PP(UART_TX_PIN_AF);
	SetPinAsAFO_PP(UART_RX_PIN_AF);

	/* Set up global interrupt priority */
	NVIC_EnableIRQ(UART_IRQ);
	NVIC_SetPriority(UART_IRQ, 8u);
} /* Uart_Init() */

/* --------------------------------------------------------------- */

/*
 * This function must be called frequently enough to ensure that
 * the buffers do not become full at the configured baud rate.
 * The sizes of the buffers are defined in UartLocal.h.
 */
void Uart::Update(void)
{
	USART_TypeDef *USART = UART_STRUCT;
	uint16_t bytesToAddToBuffer;
	uint16_t bytesAvailableToTx;
	uint16_t bytesAvailableToRx;

	IRQDisableTransmitter(USART);

	/* Push Bytes */

	/* Calculate free space - don't need local copy of volatile as transmitter is disabled */
	if (isrTxWriteIndex >= isrTxReadIndex) {
		bytesToAddToBuffer = (isrBufLen-1) - (isrTxWriteIndex - isrTxReadIndex);
	}
	else {
		bytesToAddToBuffer = isrTxReadIndex - (isrTxWriteIndex + 1);
	}

	bytesAvailableToTx = outgoingBuffer->getNumEntries();
	if (bytesToAddToBuffer > bytesAvailableToTx) {
		bytesToAddToBuffer = bytesAvailableToTx;
	} /* if */
	while (bytesToAddToBuffer > 0) {
		isrTxBuffer[isrTxWriteIndex] = outgoingBuffer->getEntry();

		if (isrTxWriteIndex >= (isrBufLen - 1)) {
			isrTxWriteIndex = 0;
		}
		else {
			isrTxWriteIndex++;
		}

		bytesToAddToBuffer--;
	} /* while */

	if (isrTxWriteIndex != isrTxReadIndex)
	{
		IRQEnableTransmitter(USART);
	} /* if */
	/* Receiver */

	/* Pop Bytes */

	/* How much data is available? */
	uint16_t writeIndex = isrRxWriteIndex;
	if (writeIndex >= isrRxReadIndex)
	{
		bytesAvailableToRx = writeIndex - isrRxReadIndex;
	}
	else
	{
		/* Write address less than read address. */
		bytesAvailableToRx = ((isrBufLen - isrRxReadIndex)
				+ writeIndex);
	}

	bytesToAddToBuffer = incomingBuffer->getSpace();
	if (bytesAvailableToRx > bytesToAddToBuffer) {
		bytesAvailableToRx = bytesToAddToBuffer;
	}
	while (bytesAvailableToRx > 0) {
		incomingBuffer->addEntry(isrRxBuffer[isrRxReadIndex]);
		if (isrRxReadIndex >= (isrBufLen - 1)) {
			isrRxReadIndex = 0;
		}
		else {
			isrRxReadIndex++;
		}

		bytesAvailableToRx--;
	}

	IRQEnableReceiver(USART);
}

/**
 * Returns true if Rx buffer is full; only called by interrupt.
 */
static bool rxFull(void)
{
	// Local copy of volatile
	uint16_t copyIsrRxWriteIndex;

	copyIsrRxWriteIndex = isrRxWriteIndex;

	return (((copyIsrRxWriteIndex == (isrBufLen - 1))
			 && (isrRxReadIndex == 0))
			||
			(copyIsrRxWriteIndex == (isrRxReadIndex - 1)));
}
