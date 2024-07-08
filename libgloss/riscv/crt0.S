/* Copyright (c) 2017  SiFive Inc. All rights reserved.

   This copyrighted material is made available to anyone wishing to use,
   modify, copy, or redistribute it subject to the terms and conditions
   of the FreeBSD License.   This program is distributed in the hope that
   it will be useful, but WITHOUT ANY WARRANTY expressed or implied,
   including the implied warranties of MERCHANTABILITY or FITNESS FOR
   A PARTICULAR PURPOSE.  A copy of this license is available at
   http://www.opensource.org/licenses.
*/

#include "newlib.h"

#=========================================================================
# crt0.S : Entry point for RISC-V user programs
#=========================================================================

.text
.global _enter
.type   _enter, @function
.global uart8250_init
.type   uart8250_init, @function
.global _stack_start
  
_enter:
  # Initialize global pointer
.option push
.option norelax
1:auipc gp, %pcrel_hi(__global_pointer$)
  addi  gp, gp, %pcrel_lo(1b)
.option pop
  la    sp, _stack_top

  # Initialize .data
  la      a0, metal_segment_data_target_start
  la      a1, metal_segment_data_target_end
  la      a2, metal_segment_data_source_start

copy_data:
  beq a0, a1, clear_bss /* If we have reached the end of .data, move to .bss */
  lw t0, 0(a2)          /* Load word from source address */
  sw t0, 0(a0)          /* Store word to destination address */
  addi a0, a0, 4        /* Increment destination address */
  addi a2, a2, 4        /* Increment source address */
  j copy_data           /* Repeat until done */

clear_bss:
  # Clear the bss segment
  la      a0, metal_segment_bss_target_start    # Load start of bss
  la      a2, metal_segment_bss_target_end      # Load end of bss
  sub     a2, a2, a0                            # Get size of bss section
  li      a1, 0                                 # Load fill value
  call    memset

  # Exit handling
  la      a0, __libc_fini_array   # Register global termination functions
  call    atexit                  #  to be called upon exit
  call    __libc_init_array       # Run global initialization functions

  # Call UART initialization function
  lw      a0, 0(sp)                  # a0 = argc
  addi    a1, sp, __SIZEOF_POINTER__ # a1 = argv
  li      a2, 0                      # a2 = envp = NULL
  call    uart8250_init

  # Call main
  lw      a0, 0(sp)                  # a0 = argc
  addi    a1, sp, __SIZEOF_POINTER__ # a1 = argv
  li      a2, 0                      # a2 = envp = NULL
  call    main
  tail    exit
  .size  _enter, .-_enter
