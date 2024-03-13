.section .umaintext
.global dasics_call_lib
    
// arg0, arg1, arg2, arg3, dest_addr
dasics_call_lib:
    addi   sp, sp, -8
    sd     ra, 0(sp)
    .word 0x0007108b    // dasicscall.jr ra, a4
    ld     ra, 0(sp)
    addi   sp, sp, 8
    ret

// bound_src, bound_dst
.section .umaintext
.global dasics_copy_mem_bound
dasics_copy_mem_bound:
    .word 0x00b5200b // dibndmv a0, a1, 0
    ret

.section .umaintext
.global dasics_copy_jmp_bound
dasics_copy_jmp_bound:
    .word 0x00b5208b // dibndmv a0, a1, 1
    ret

// void
.section .umaintext
.global dasics_query_mem_bound
dasics_query_mem_bound:
    .word 0x0000350b //dibndquery a0, 0
    ret

.section .umaintext
.global dasics_query_jmp_bound
dasics_query_jmp_bound:
    .word 0x0010350b //dibndquery a0, 1
    ret

