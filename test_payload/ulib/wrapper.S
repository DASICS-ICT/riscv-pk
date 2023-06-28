.section .umaintext
.global main_printf
    
main_printf:
    addi   sp, sp, -8
    sd     ra, 0(sp)
    auipc  t0, 0
    addi   t0, t0, 16
    csrw   0x8b1, t0
    jalr   ra, a1
    ld     ra, 0(sp)
    addi   sp, sp, 8
    ret

.section .umaintext
.global main_call_lib
    
main_call_lib:
    addi   sp, sp, -8
    sd     ra, 0(sp)
    auipc  t0, 0
    addi   t0, t0, 16
    csrw   0x8b1, t0
    jalr   ra, a0
    ld     ra, 0(sp)
    addi   sp, sp, 8
    ret
