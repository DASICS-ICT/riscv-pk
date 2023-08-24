/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Copyright (C) 2012 Regents of the University of California
 */

#include <asm.h>
#include <csr.h>

/* maybe you need another stack for booting? */
.equ boot_stack_top_base, 0x0000000080202000

.section ".entry_function","ax"
ENTRY(_boot)

  // Mask all interrupt
  csrw CSR_SIE, zero
  csrw CSR_SIP, zero

    # li a0, 50
    # li a7, 1
    # ecall
  // Disable FPU in kernel space
  li t0, SR_FS
  csrc sstatus, t0

#   la sp, boot_stack_top_base
    li sp, 0x0000000085000000
    # lui sp, %hi(boot_stack_top_base)
    # addi sp, sp, %lo(boot_stack_top_base)

#   csrr a0, CSR_MHARTID 
    # li a0, 51
    # li a7, 1
    # ecall

  li a0, 0

  // this will never return
  call boot_kernel

loop:
  wfi
  j loop

END(_boot)
