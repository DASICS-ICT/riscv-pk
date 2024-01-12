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

.section .umaintext
.global main_call_lib1
main_call_lib1:
    addi   sp, sp, -8
    sd     ra, 0(sp)
    .word 0x0005908b    // dasicscall.jr ra, a1
    ld     ra, 0(sp)
    addi   sp, sp, 8
    ret

.section .umaintext
.global main_call_lib2
main_call_lib2:
    addi   sp, sp, -8
    sd     ra, 0(sp)
    .word 0x0006108b    // dasicscall.jr ra, a2
    ld     ra, 0(sp)
    addi   sp, sp, 8
    ret

.section .umaintext
.global main_call_lib4
main_call_lib4:
    addi   sp, sp, -8
    sd     ra, 0(sp)
    .word 0x0007108b    // dasicscall.jr ra, a4
    ld     ra, 0(sp)
    addi   sp, sp, 8
    ret

.section .umaintext
.global main_call_lib6
main_call_lib6:
    addi   sp, sp, -8
    sd     ra, 0(sp)
    .word 0x0008108b    // dasicscall.jr ra, a6
    ld     ra, 0(sp)
    addi   sp, sp, 8
    ret
