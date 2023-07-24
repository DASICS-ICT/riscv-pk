.section .umaintext
.global main_printf
    
main_printf:
    addi   sp, sp, -8
    sd     ra, 0(sp)
    .word 0x0005908b    // dasicscall.jr ra, a1
    ld     ra, 0(sp)
    addi   sp, sp, 8
    ret

.section .umaintext
.global main_call_lib
    
main_call_lib:
    addi   sp, sp, -8
    sd     ra, 0(sp)
    .word 0x0005108b    // dasicscall.jr ra, a0
    ld     ra, 0(sp)
    addi   sp, sp, 8
    ret
