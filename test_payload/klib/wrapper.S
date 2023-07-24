//.section .slibtext
.global main_printk
    
main_printk:
    addi   sp, sp, -8
    sd     ra, 0(sp)
    .word 0x0005908b    // dasicscall.jr ra, a1
    ld     ra, 0(sp)
    addi   sp, sp, 8
    ret

