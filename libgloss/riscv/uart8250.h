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
 * This is a bare-metal driver for the 8250 UART module. This driver 
 * is a modified version of the original 8250 UART driver found in the
 * RISCV OpenSBI repo.
 * 
 * The actual silicon and the virtual prototype implement the NS16650 
 * hardware, which is backwards compativle to the 8250. Their difference is 
 * that the NS16650 is FIFO-based and thus better for multiprogramming 
 * environments, but in this case where we do bare-metal programming, 
 * the FIFO does not provide any significant advantage.
 *
 * To use this driver, the user should first call the UART 
 * initialization function in order to bring the hardware to a 
 * known state.
 */

#ifndef __SERIAL_UART8250_H__
#define __SERIAL_UART8250_H__

/**
 * 
 */
int uart8250_init();

void uart8250_putc(char ch);

char uart8250_getc(void);

#endif
