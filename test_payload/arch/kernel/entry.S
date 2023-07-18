#include <arch/asm.h>
#include <arch/csr.h>

.macro SAVE_CONTEXT
  .local _restore_kernel_tpsp
  .local _save_context
  /*
   * If coming from userspace, preserve the user thread pointer and load
   * the kernel thread pointer.  If we came from the kernel, sscratch
   * will contain 0, and we should continue on the current TP.
   */
  csrrw tp, CSR_SSCRATCH, tp
  bnez tp, _save_context

_restore_kernel_tpsp:
  csrr tp, CSR_SSCRATCH
  sd sp, PCB_KERNEL_SP(tp)
_save_context:
  sd sp, PCB_USER_SP(tp)
  ld sp, PCB_KERNEL_SP(tp)
  addi sp, sp, -(OFFSET_SIZE)

  sd x1,  OFFSET_REG_RA(sp)
  sd x3,  OFFSET_REG_GP(sp)
  sd x5,  OFFSET_REG_T0(sp)
  sd x6,  OFFSET_REG_T1(sp)
  sd x7,  OFFSET_REG_T2(sp)
  sd x8,  OFFSET_REG_S0(sp)
  sd x9,  OFFSET_REG_S1(sp)
  sd x10, OFFSET_REG_A0(sp)
  sd x11, OFFSET_REG_A1(sp)
  sd x12, OFFSET_REG_A2(sp)
  sd x13, OFFSET_REG_A3(sp)
  sd x14, OFFSET_REG_A4(sp)
  sd x15, OFFSET_REG_A5(sp)
  sd x16, OFFSET_REG_A6(sp)
  sd x17, OFFSET_REG_A7(sp)
  sd x18, OFFSET_REG_S2(sp)
  sd x19, OFFSET_REG_S3(sp)
  sd x20, OFFSET_REG_S4(sp)
  sd x21, OFFSET_REG_S5(sp)
  sd x22, OFFSET_REG_S6(sp)
  sd x23, OFFSET_REG_S7(sp)
  sd x24, OFFSET_REG_S8(sp)
  sd x25, OFFSET_REG_S9(sp)
  sd x26, OFFSET_REG_S10(sp)
  sd x27, OFFSET_REG_S11(sp)
  sd x28, OFFSET_REG_T3(sp)
  sd x29, OFFSET_REG_T4(sp)
  sd x30, OFFSET_REG_T5(sp)
  sd x31, OFFSET_REG_T6(sp)

  # /* Save N Extension CSRs */
  # csrr s0, CSR_USTATUS
  # sd   s0, OFFSET_REG_USTATUS(sp)
  # csrr s0, CSR_UEPC
  # sd   s0, OFFSET_REG_UEPC(sp)
  # csrr s0, CSR_UTVAL
  # sd   s0, OFFSET_REG_UBADADDR(sp)
  # csrr s0, CSR_UCAUSE
  # sd   s0, OFFSET_REG_UCAUSE(sp)
  # csrr s0, CSR_UTVEC
  # sd   s0, OFFSET_REG_UTVEC(sp)
  # csrr s0, CSR_UIE
  # sd   s0, OFFSET_REG_UIE(sp)
  # csrr s0, CSR_UIP
  # sd   s0, OFFSET_REG_UIP(sp)
  # csrr s0, CSR_USCRATCH
  # sd   s0, OFFSET_REG_USCRATCH(sp)

  # /* Save DASICS context */
  # csrr s0, CSR_DUMCFG
  # sd   s0, OFFSET_REG_DUMCFG(sp)
  # csrr s0, CSR_DUMBOUNDLO
  # sd   s0, OFFSET_REG_DUMBOUNDLO(sp)
  # csrr s0, CSR_DUMBOUNDHI
  # sd   s0, OFFSET_REG_DUMBOUNDHI(sp)

  # csrr s0, CSR_DLCFG0
  # sd   s0, OFFSET_REG_DLCFG0(sp)

  # csrr s0, CSR_DLBOUND0LO
  # sd   s0, OFFSET_REG_DLBOUND0LO(sp)
  # csrr s0, CSR_DLBOUND0HI
  # sd   s0, OFFSET_REG_DLBOUND0HI(sp)
  # csrr s0, CSR_DLBOUND1LO
  # sd   s0, OFFSET_REG_DLBOUND1LO(sp)
  # csrr s0, CSR_DLBOUND1HI
  # sd   s0, OFFSET_REG_DLBOUND1HI(sp)
  # csrr s0, CSR_DLBOUND2LO
  # sd   s0, OFFSET_REG_DLBOUND2LO(sp)
  # csrr s0, CSR_DLBOUND2HI
  # sd   s0, OFFSET_REG_DLBOUND2HI(sp)
  # csrr s0, CSR_DLBOUND3LO
  # sd   s0, OFFSET_REG_DLBOUND3LO(sp)
  # csrr s0, CSR_DLBOUND3HI
  # sd   s0, OFFSET_REG_DLBOUND3HI(sp)
  # csrr s0, CSR_DLBOUND4LO
  # sd   s0, OFFSET_REG_DLBOUND4LO(sp)
  # csrr s0, CSR_DLBOUND4HI
  # sd   s0, OFFSET_REG_DLBOUND4HI(sp)
  # csrr s0, CSR_DLBOUND5LO
  # sd   s0, OFFSET_REG_DLBOUND5LO(sp)
  # csrr s0, CSR_DLBOUND5HI
  # sd   s0, OFFSET_REG_DLBOUND5HI(sp)
  # csrr s0, CSR_DLBOUND6LO
  # sd   s0, OFFSET_REG_DLBOUND6LO(sp)
  # csrr s0, CSR_DLBOUND6HI
  # sd   s0, OFFSET_REG_DLBOUND6HI(sp)
  # csrr s0, CSR_DLBOUND7LO
  # sd   s0, OFFSET_REG_DLBOUND7LO(sp)
  # csrr s0, CSR_DLBOUND7HI
  # sd   s0, OFFSET_REG_DLBOUND7HI(sp)
  # csrr s0, CSR_DLBOUND8LO
  # sd   s0, OFFSET_REG_DLBOUND8LO(sp)
  # csrr s0, CSR_DLBOUND8HI
  # sd   s0, OFFSET_REG_DLBOUND8HI(sp)
  # csrr s0, CSR_DLBOUND9LO
  # sd   s0, OFFSET_REG_DLBOUND9LO(sp)
  # csrr s0, CSR_DLBOUND9HI
  # sd   s0, OFFSET_REG_DLBOUND9HI(sp)
  # csrr s0, CSR_DLBOUND10LO
  # sd   s0, OFFSET_REG_DLBOUND10LO(sp)
  # csrr s0, CSR_DLBOUND10HI
  # sd   s0, OFFSET_REG_DLBOUND10HI(sp)
  # csrr s0, CSR_DLBOUND11LO
  # sd   s0, OFFSET_REG_DLBOUND11LO(sp)
  # csrr s0, CSR_DLBOUND11HI
  # sd   s0, OFFSET_REG_DLBOUND11HI(sp)
  # csrr s0, CSR_DLBOUND12LO
  # sd   s0, OFFSET_REG_DLBOUND12LO(sp)
  # csrr s0, CSR_DLBOUND12HI
  # sd   s0, OFFSET_REG_DLBOUND12HI(sp)
  # csrr s0, CSR_DLBOUND13LO
  # sd   s0, OFFSET_REG_DLBOUND13LO(sp)
  # csrr s0, CSR_DLBOUND13HI
  # sd   s0, OFFSET_REG_DLBOUND13HI(sp)
  # csrr s0, CSR_DLBOUND14LO
  # sd   s0, OFFSET_REG_DLBOUND14LO(sp)
  # csrr s0, CSR_DLBOUND14HI
  # sd   s0, OFFSET_REG_DLBOUND14HI(sp)
  # csrr s0, CSR_DLBOUND15LO
  # sd   s0, OFFSET_REG_DLBOUND15LO(sp)
  # csrr s0, CSR_DLBOUND15HI 
  # sd   s0, OFFSET_REG_DLBOUND15HI(sp)

  # csrr s0, CSR_DMAINCALL
  # sd   s0, OFFSET_REG_DMAINCALL(sp)
  # csrr s0, CSR_DRETURNPC
  # sd   s0, OFFSET_REG_DRETURNPC(sp)
  # csrr s0, CSR_DFZRETURN
  # sd   s0, OFFSET_REG_DFZRETURN(sp)

  # csrr s0, CSR_DJBOUND0LO
  # sd   s0, OFFSET_REG_DJBOUND0LO(sp)
  # csrr s0, CSR_DJBOUND0HI
  # sd   s0, OFFSET_REG_DJBOUND0HI(sp)
  # csrr s0, CSR_DJBOUND1LO
  # sd   s0, OFFSET_REG_DJBOUND1LO(sp)
  # csrr s0, CSR_DJBOUND1HI
  # sd   s0, OFFSET_REG_DJBOUND1HI(sp)
  # csrr s0, CSR_DJBOUND2LO
  # sd   s0, OFFSET_REG_DJBOUND2LO(sp)
  # csrr s0, CSR_DJBOUND2HI
  # sd   s0, OFFSET_REG_DJBOUND2HI(sp)
  # csrr s0, CSR_DJBOUND3LO
  # sd   s0, OFFSET_REG_DJBOUND3LO(sp)
  # csrr s0, CSR_DJBOUND3HI
  # sd   s0, OFFSET_REG_DJBOUND3HI(sp)

  # csrr s0, CSR_DJCFG
  # sd   s0, OFFSET_REG_DJCFG(sp)

  /*
   * Disable user-mode memory access as it should only be set in the
   * actual user copy routines.
   *
   * Disable the FPU to detect illegal usage of floating point in kernel
   * space.
   */
  li t0, SR_SUM | SR_FS

  ld s0, PCB_USER_SP(tp)
  csrrc s1, CSR_SSTATUS, t0
  csrr s2, CSR_SEPC
  csrr s3, CSR_STVAL
  csrr s4, CSR_SCAUSE
  csrr s5, CSR_SSCRATCH
  sd s0,   OFFSET_REG_SP(sp)
  sd s1,   OFFSET_REG_SSTATUS(sp)
  sd s2,   OFFSET_REG_SEPC(sp)
  sd s3,   OFFSET_REG_SBADADDR(sp)
  sd s4,   OFFSET_REG_SCAUSE(sp)
  sd s5,   OFFSET_REG_TP(sp)
.endm

.macro RESTORE_CONTEXT
  ld a0, OFFSET_REG_SSTATUS(sp)
  ld a2, OFFSET_REG_SEPC(sp)
  csrw CSR_SSTATUS, a0
  csrw CSR_SEPC, a2

  # /* Restore N Extension CSRs */
  # ld   s0, OFFSET_REG_USTATUS(sp)
  # csrw CSR_USTATUS, s0
  # ld   s0, OFFSET_REG_UEPC(sp)
  # csrw CSR_UEPC, s0
  # ld   s0, OFFSET_REG_UBADADDR(sp)
  # csrw CSR_UTVAL, s0
  # ld   s0, OFFSET_REG_UCAUSE(sp)
  # csrw CSR_UCAUSE, s0
  # ld   s0, OFFSET_REG_UTVEC(sp)
  # csrw CSR_UTVEC, s0
  # ld   s0, OFFSET_REG_UIE(sp)
  # csrw CSR_UIE, s0
  # ld   s0, OFFSET_REG_UIP(sp)
  # csrw CSR_UIP, s0
  # ld   s0, OFFSET_REG_USCRATCH(sp)
  # csrw CSR_USCRATCH, s0

  # /* Restore DASICS context */
  # ld   s0, OFFSET_REG_DUMCFG(sp)
  # csrw CSR_DUMCFG, s0
  # ld   s0, OFFSET_REG_DUMBOUNDLO(sp)
  # csrw CSR_DUMBOUNDLO, s0
  # ld   s0, OFFSET_REG_DUMBOUNDHI(sp)
  # csrw CSR_DUMBOUNDHI, s0

  # ld   s0, OFFSET_REG_DLCFG0(sp)
  # csrw CSR_DLCFG0, s0

  # ld   s0, OFFSET_REG_DLBOUND0LO(sp)
  # csrw CSR_DLBOUND0LO, s0
  # ld   s0, OFFSET_REG_DLBOUND0HI(sp)
  # csrw CSR_DLBOUND0HI, s0
  # ld   s0, OFFSET_REG_DLBOUND1LO(sp)
  # csrw CSR_DLBOUND1LO, s0
  # ld   s0, OFFSET_REG_DLBOUND1HI(sp)
  # csrw CSR_DLBOUND1HI, s0
  # ld   s0, OFFSET_REG_DLBOUND2LO(sp)
  # csrw CSR_DLBOUND2LO, s0
  # ld   s0, OFFSET_REG_DLBOUND2HI(sp)
  # csrw CSR_DLBOUND2HI, s0
  # ld   s0, OFFSET_REG_DLBOUND3LO(sp)
  # csrw CSR_DLBOUND3LO, s0
  # ld   s0, OFFSET_REG_DLBOUND3HI(sp)
  # csrw CSR_DLBOUND3HI, s0
  # ld   s0, OFFSET_REG_DLBOUND4LO(sp)
  # csrw CSR_DLBOUND4LO, s0
  # ld   s0, OFFSET_REG_DLBOUND4HI(sp)
  # csrw CSR_DLBOUND4HI, s0
  # ld   s0, OFFSET_REG_DLBOUND5LO(sp)
  # csrw CSR_DLBOUND5LO, s0
  # ld   s0, OFFSET_REG_DLBOUND5HI(sp)
  # csrw CSR_DLBOUND5HI, s0
  # ld   s0, OFFSET_REG_DLBOUND6LO(sp)
  # csrw CSR_DLBOUND6LO, s0
  # ld   s0, OFFSET_REG_DLBOUND6HI(sp)
  # csrw CSR_DLBOUND6HI, s0
  # ld   s0, OFFSET_REG_DLBOUND7LO(sp)
  # csrw CSR_DLBOUND7LO, s0
  # ld   s0, OFFSET_REG_DLBOUND7HI(sp)
  # csrw CSR_DLBOUND7HI, s0
  # ld   s0, OFFSET_REG_DLBOUND8LO(sp)
  # csrw CSR_DLBOUND8LO, s0
  # ld   s0, OFFSET_REG_DLBOUND8HI(sp)
  # csrw CSR_DLBOUND8HI, s0
  # ld   s0, OFFSET_REG_DLBOUND9LO(sp)
  # csrw CSR_DLBOUND9LO, s0
  # ld   s0, OFFSET_REG_DLBOUND9HI(sp)
  # csrw CSR_DLBOUND9HI, s0
  # ld   s0, OFFSET_REG_DLBOUND10LO(sp)
  # csrw CSR_DLBOUND10LO, s0
  # ld   s0, OFFSET_REG_DLBOUND10HI(sp)
  # csrw CSR_DLBOUND10HI, s0
  # ld   s0, OFFSET_REG_DLBOUND11LO(sp)
  # csrw CSR_DLBOUND11LO, s0
  # ld   s0, OFFSET_REG_DLBOUND11HI(sp)
  # csrw CSR_DLBOUND11HI, s0
  # ld   s0, OFFSET_REG_DLBOUND12LO(sp)
  # csrw CSR_DLBOUND12LO, s0
  # ld   s0, OFFSET_REG_DLBOUND12HI(sp)
  # csrw CSR_DLBOUND12HI, s0
  # ld   s0, OFFSET_REG_DLBOUND13LO(sp)
  # csrw CSR_DLBOUND13LO, s0
  # ld   s0, OFFSET_REG_DLBOUND13HI(sp)
  # csrw CSR_DLBOUND13HI, s0
  # ld   s0, OFFSET_REG_DLBOUND14LO(sp)
  # csrw CSR_DLBOUND14LO, s0
  # ld   s0, OFFSET_REG_DLBOUND14HI(sp)
  # csrw CSR_DLBOUND14HI, s0
  # ld   s0, OFFSET_REG_DLBOUND15LO(sp)
  # csrw CSR_DLBOUND15LO, s0
  # ld   s0, OFFSET_REG_DLBOUND15HI(sp)
  # csrw CSR_DLBOUND15HI, s0

  # ld   s0, OFFSET_REG_DMAINCALL(sp)
  # csrw CSR_DMAINCALL, s0
  # ld   s0, OFFSET_REG_DRETURNPC(sp)
  # csrw CSR_DRETURNPC, s0
  # ld   s0, OFFSET_REG_DFZRETURN(sp)
  # csrw CSR_DFZRETURN, s0

  # ld   s0, OFFSET_REG_DJBOUND0LO(sp)
  # csrw CSR_DJBOUND0LO, s0
  # ld   s0, OFFSET_REG_DJBOUND0HI(sp)
  # csrw CSR_DJBOUND0HI, s0
  # ld   s0, OFFSET_REG_DJBOUND1LO(sp)
  # csrw CSR_DJBOUND1LO, s0
  # ld   s0, OFFSET_REG_DJBOUND1HI(sp)
  # csrw CSR_DJBOUND1HI, s0
  # ld   s0, OFFSET_REG_DJBOUND2LO(sp)
  # csrw CSR_DJBOUND2LO, s0
  # ld   s0, OFFSET_REG_DJBOUND2HI(sp)
  # csrw CSR_DJBOUND2HI, s0
  # ld   s0, OFFSET_REG_DJBOUND3LO(sp)
  # csrw CSR_DJBOUND3LO, s0
  # ld   s0, OFFSET_REG_DJBOUND3HI(sp)
  # csrw CSR_DJBOUND3HI, s0

  # ld   s0, OFFSET_REG_DJCFG(sp)
  # csrw CSR_DJCFG, s0

  ld x1,  OFFSET_REG_RA(sp)
  ld x3,  OFFSET_REG_GP(sp)
  ld x4,  OFFSET_REG_TP(sp)
  ld x5,  OFFSET_REG_T0(sp)
  ld x6,  OFFSET_REG_T1(sp)
  ld x7,  OFFSET_REG_T2(sp)
  ld x8,  OFFSET_REG_S0(sp)
  ld x9,  OFFSET_REG_S1(sp)
  ld x10, OFFSET_REG_A0(sp)
  ld x11, OFFSET_REG_A1(sp)
  ld x12, OFFSET_REG_A2(sp)
  ld x13, OFFSET_REG_A3(sp)
  ld x14, OFFSET_REG_A4(sp)
  ld x15, OFFSET_REG_A5(sp)
  ld x16, OFFSET_REG_A6(sp)
  ld x17, OFFSET_REG_A7(sp)
  ld x18, OFFSET_REG_S2(sp)
  ld x19, OFFSET_REG_S3(sp)
  ld x20, OFFSET_REG_S4(sp)
  ld x21, OFFSET_REG_S5(sp)
  ld x22, OFFSET_REG_S6(sp)
  ld x23, OFFSET_REG_S7(sp)
  ld x24, OFFSET_REG_S8(sp)
  ld x25, OFFSET_REG_S9(sp)
  ld x26, OFFSET_REG_S10(sp)
  ld x27, OFFSET_REG_S11(sp)
  ld x28, OFFSET_REG_T3(sp)
  ld x29, OFFSET_REG_T4(sp)
  ld x30, OFFSET_REG_T5(sp)
  ld x31, OFFSET_REG_T6(sp)

  ld x2,  OFFSET_REG_SP(sp)
.endm

ENTRY(enable_preempt)
  ld t1, current_running
  ld t0, PCB_PREEMPT_COUNT(t1)
  beq t0, zero, do_enable
	addi t0, t0, -1
	sd t0, PCB_PREEMPT_COUNT(t1)
  beq t0, zero, do_enable
  jr ra
do_enable:
  not t0, x0
  csrs CSR_SIE, t0
  jr ra
ENDPROC(enable_preempt)

ENTRY(disable_preempt)
  csrw CSR_SIE, zero
  ld t1, current_running
  ld t0, PCB_PREEMPT_COUNT(t1)
  addi t0, t0, 1
  sd t0, PCB_PREEMPT_COUNT(t1)
  jr ra
ENDPROC(disable_preempt)

ENTRY(enable_interrupt)
  li t0, SR_SIE
  csrs CSR_SSTATUS, t0
  jr ra
ENDPROC(enable_interrupt)

ENTRY(disable_interrupt)
  li t0, SR_SIE
  csrs CSR_SSTATUS, t0
  jr ra
ENDPROC(disable_interrupt)

// the address of previous pcb in a0
// the address of next pcb in a1
ENTRY(switch_to)
  // save all callee save registers on kernel stack
  addi sp, sp, -(SWITCH_TO_SIZE)
  sd ra,  SWITCH_TO_RA(sp)
  sd s0,  SWITCH_TO_S0(sp)
  sd s1,  SWITCH_TO_S1(sp)
  sd s2,  SWITCH_TO_S2(sp)
  sd s3,  SWITCH_TO_S3(sp)
  sd s4,  SWITCH_TO_S4(sp)
  sd s5,  SWITCH_TO_S5(sp)
  sd s6,  SWITCH_TO_S6(sp)
  sd s7,  SWITCH_TO_S7(sp)
  sd s8,  SWITCH_TO_S8(sp)
  sd s9,  SWITCH_TO_S9(sp)
  sd s10, SWITCH_TO_S10(sp)
  sd s11, SWITCH_TO_S11(sp)
  sd sp,  SWITCH_TO_SP(sp)
  sd sp,  PCB_KERNEL_SP(a0)

  // restore next
  add tp, a1, zero
  ld sp,  PCB_KERNEL_SP(tp)
  ld ra,  SWITCH_TO_RA(sp)
  ld s0,  SWITCH_TO_S0(sp)
  ld s1,  SWITCH_TO_S1(sp)
  ld s2,  SWITCH_TO_S2(sp)
  ld s3,  SWITCH_TO_S3(sp)
  ld s4,  SWITCH_TO_S4(sp)
  ld s5,  SWITCH_TO_S5(sp)
  ld s6,  SWITCH_TO_S6(sp)
  ld s7,  SWITCH_TO_S7(sp)
  ld s8,  SWITCH_TO_S8(sp)
  ld s9,  SWITCH_TO_S9(sp)
  ld s10, SWITCH_TO_S10(sp)
  ld s11, SWITCH_TO_S11(sp)
  ld sp,  SWITCH_TO_SP(sp)
  addi sp, sp, SWITCH_TO_SIZE
  jr ra
ENDPROC(switch_to)

ENTRY(ret_from_exception)
  /* check if we return to kernel or a normal thread */
  la t0, pid0_pcb
  beq tp, t0, restore

  /* return to a normal thread(not the pid0 thread) now */
  addi t0, sp, OFFSET_SIZE
  sd t0, PCB_KERNEL_SP(tp)
  csrw CSR_SSCRATCH, tp


restore:
  RESTORE_CONTEXT
  sret
ENDPROC(ret_from_exception)

ENTRY(exception_handler_entry)
  SAVE_CONTEXT

  csrw CSR_SSCRATCH, x0

  /* Load the global pointer */
  .option push
  .option norelax
  la gp, __global_pointer$
  .option pop

  la ra, ret_from_exception

  move a0,sp
  move a1,s3
  move a2,s4

  la t0, interrupt_helper
  jr t0
ENDPROC(exception_handler_entry)
