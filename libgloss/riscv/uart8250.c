/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 * Copyright (c) 2024 SoCHub Finland.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 *   Andreas Stergiopoulos <andreas.stergiopoulos@tuni.fi>
 *
 * For details regarding the driver, please check the associated header 
 * file.
 */
#include <uart8250.h>
#include <stdint.h>

/* clang-format off */

#define UART_RBR_OFFSET		0		/* In:  Recieve Buffer Register */
#define UART_THR_OFFSET		0		/* Out: Transmitter Holding Register */
#define UART_DLL_OFFSET		0		/* Out: Divisor Latch Low */
#define UART_IER_OFFSET		0x4		/* I/O: Interrupt Enable Register */
#define UART_DLM_OFFSET		0x4		/* Out: Divisor Latch High */
#define UART_FCR_OFFSET		0x8		/* Out: FIFO Control Register */
#define UART_IIR_OFFSET		0x8		/* I/O: Interrupt Identification Register */
#define UART_LCR_OFFSET		0xC		/* Out: Line Control Register */
#define UART_MCR_OFFSET		0x10	/* Out: Modem Control Register */
#define UART_LSR_OFFSET		0x14	/* In:  Line Status Register */
#define UART_MSR_OFFSET		0x18	/* In:  Modem Status Register */
#define UART_SCR_OFFSET		0x1C	/* I/O: Scratch Register */
#define UART_MDR1_OFFSET	0x20	/* I/O:  Mode Register */

#define UART_LSR_FIFOE		0x80	/* Fifo error */
#define UART_LSR_TEMT		0x40	/* Transmitter empty */
#define UART_LSR_THRE		0x20	/* Transmit-hold-register empty */
#define UART_LSR_BI		0x10	/* Break interrupt indicator */
#define UART_LSR_FE		0x08	/* Frame error indicator */
#define UART_LSR_PE		0x04	/* Parity error indicator */
#define UART_LSR_OE		0x02	/* Overrun error indicator */
#define UART_LSR_DR		0x01	/* Receiver data ready */
#define UART_LSR_BRK_ERROR_BITS	0x1E	/* BI, FE, PE, OE bits */
/* clang-format on */

static volatile char *uart8250_base;
static uint32_t uart8250_in_freq;
static uint32_t uart8250_baudrate;
static uint32_t uart8250_reg_width;
static uint32_t uart8250_reg_shift;

uint32_t get_reg(uint32_t num)
{
	return *((volatile char*)(uart8250_base + num));
}

void set_reg(
    uint32_t num, 
    uint32_t val
)
{
	*((volatile char*)(uart8250_base + num)) = (char)val;
}

void uart8250_putc(char ch)
{
	while ((get_reg(UART_LSR_OFFSET) & UART_LSR_THRE) == 0);

	set_reg(UART_THR_OFFSET, ch);
}

char uart8250_getc(void)
{
	while ((get_reg(UART_LSR_OFFSET) & UART_LSR_DR) == 0);
	
	return get_reg(UART_RBR_OFFSET);
}

/**
 * At the original implementation, this init function took a 
 * number of arguments such as the base address and Baud rate.
 * 
 * In order to reduce the amount of workload for the newlib user,
 * these arguments have been phased out and their default values 
 * are the same as those used in openSBI.
 * 
 * The arguments are kept so that the driver may be restored in 
 * some later iteration of newlib.
 */
int uart8250_init()
{
	/** Original arguments start */
	unsigned long base 	= 0x1FFF00000;
    uint32_t in_freq	= 30000000;
    uint32_t baudrate	= 9600;
    uint32_t reg_shift	= 0;
	uint32_t reg_width	= 1;
    uint32_t reg_offset	= 0;
	/** Original arguments end */

	uint16_t bdiv 		= 0;

	uart8250_base      = (volatile char *)base + reg_offset;
	uart8250_reg_shift = reg_shift;
	uart8250_reg_width = reg_width;
	uart8250_in_freq   = in_freq;
	uart8250_baudrate  = baudrate;

	if (uart8250_baudrate) {
		bdiv = (uart8250_in_freq + 8 * uart8250_baudrate) /
		       (16 * uart8250_baudrate);
	}

	/* Disable GPIO behavior for UART pins. as per the apb_uart0 Rust example*/
	volatile uint32_t *pad_conf_uart0_tx = (volatile uint32_t *)0x1FFF07064;
	/* I have no idea what registers we are supposed to unmask. */
	uint32_t mask = (0 | (0b1 << 5) | (0b1 << 10));
	/* Read - Modify - Write */
	uint32_t reg_val = *pad_conf_uart0_tx;	/* Read */
	reg_val &= ~mask;						/* Modify */					
	*pad_conf_uart0_tx = reg_val;			/* Write */

	/* Disable all interrupts */
	set_reg(UART_IER_OFFSET, 0x00);
	/* Enable DLAB */
	set_reg(UART_LCR_OFFSET, 0x80);

	if (bdiv) {
		/* Set divisor low byte */
		set_reg(UART_DLL_OFFSET, bdiv & 0xff);
		/* Set divisor high byte */
		set_reg(UART_DLM_OFFSET, (bdiv >> 8) & 0xff);
	}

	/* 8 bits, no parity, one stop bit */
	set_reg(UART_LCR_OFFSET, 0x03);
	/* Enable FIFO */
	set_reg(UART_FCR_OFFSET, 0x01);
	/* No modem control DTR RTS */
	set_reg(UART_MCR_OFFSET, 0x00);
	/* Clear line status */
	get_reg(UART_LSR_OFFSET);
	/* Read receive buffer */
	get_reg(UART_RBR_OFFSET);
	/* Set scratchpad */
	set_reg(UART_SCR_OFFSET, 0x00);

	return 0;
}