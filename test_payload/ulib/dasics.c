#include <stdcsr.h>
#include <stddasics.h>
#include <sys/syscall.h>
#include <stdio.h>

void ATTR_UMAIN_TEXT dasics_init_umaincall(uint64_t entry)
{
    write_csr(0x8b0, entry);  // DasicsMaincallEntry
}




void ATTR_UMAIN_TEXT dasics_ufault_entry(void) {
    // Save some registers that should be saved by callees
    // DasicsReturnPC
    uint64_t dasics_return_pc0 = read_csr(0x8b4);
    uint64_t dasics_return_pc1 = read_csr(0x8b5); 
    uint64_t dasics_return_pc2 = read_csr(0x8b6); 
    uint64_t dasics_return_pc3 = read_csr(0x8b7); 
    uint64_t dasics_free_zone_return_pc = read_csr(0x8b2);  // DasicsFreeZoneReturnPC


    uint64_t ustatus = read_csr(ustatus);
    uint64_t ucause = read_csr(ucause);
    uint64_t utval = read_csr(utval);
    uint64_t uepc = read_csr(uepc);

    //printf("Info: ufault occurs, ucause = 0x%x, uepc = 0x%x, utval = 0x%x\n", ucause, uepc, utval);
// #ifndef DASICS_DEBUG
//     printf("Info: ready to shutdown the program due to ufault ...\n");
//     sys_exit();
// #else
    switch (ucause)
    {
        case EXCC_DASICS_UINSTR_FAULT:
            //const char* message_1 = "[HANDLE_U_DASICS]: Detect UInst Access Fault! Skip this instruction!\n";          
            main_printf("[HANDLE_U_DASICS]: Detect UInst Access Fault! Skip this instruction!\n");
            main_printf_1("uepc: 0x%lx\t", uepc);
            main_printf_1("utval: 0x%lx\n", utval);
            break;
        case EXCC_DASICS_ULOAD_FAULT:
            main_printf("[HANDLE_U_DASICS]: Detect ULoad Access Fault! Skip this instruction!\n");
            main_printf_1("uepc: 0x%lx\t", uepc);
            main_printf_1("utval: 0x%lx\n", utval);
            break;
        case EXCC_DASICS_USTORE_FAULT:
            main_printf("[HANDLE_U_DASICS]: Detect UStore Access Fault! Skip this instruction!\n");
            main_printf_1("uepc: 0x%lx\t", uepc);
            main_printf_1("utval: 0x%lx\n", utval);
            break;
        default:
            main_printf("[HANDLE_U_DASICS]: Invalid cause! exit..");
            //printf("[HANDLE_U_DASICS]: Invalid cause 0x%lx detected!\n");
            // printf("ustatus: 0x%lx uepc: 0x%lx ucause: %lu\n\r",
            //     ustatus, uepc, ucause);
            sys_exit();
            break;
    }
    write_csr(uepc, uepc + 4);  // For debugging
// #endif

    // Restore those saved registers
    write_csr(0x8b4, dasics_return_pc0);
    write_csr(0x8b5, dasics_return_pc1);
    write_csr(0x8b6, dasics_return_pc2);
    write_csr(0x8b7, dasics_return_pc3);
    write_csr(0x8b2, dasics_free_zone_return_pc);

    asm volatile ("ld   ra, 152(sp)\n"\
                  "ld   s0, 144(sp)\n"\
                  "addi sp, sp, 160\n"\
                  "uret");
}


uint64_t ATTR_UMAIN_TEXT dasics_umaincall(UmaincallTypes type, uint64_t arg0, uint64_t arg1, uint64_t arg2)
{
    uint64_t dasics_return_pc = read_csr(0x8b4);            // DasicsReturnPC0
    uint64_t dasics_free_zone_return_pc = read_csr(0x8b2);  // DasicsFreeZoneReturnPC

    uint64_t retval = 0;

    switch (type)
    {
        case UMAINCALL_EXIT:
            sys_exit();
            break;
        case UMAINCALL_YIELD:
            sys_yield();
            break;
        case UMAINCALL_SLEEP:
            sys_sleep((uint32_t)arg0);
            break;
        case UMAINCALL_WRITE:
            sys_write((char *)arg0);
            break;
        case UMAINCALL_REFLUSH:
            sys_reflush();
            break;
        case UMAINCALL_MOVE_CURSOR:
            sys_move_cursor((int)arg0, (int)arg1);
            break;
        case UMAINCALL_FUTEX_WAIT:
            sys_futex_wait((volatile uint64_t *)arg0, (uint64_t)arg1);
            break;
        case UMAINCALL_FUTEX_WAKEUP:
            sys_futex_wakeup((volatile uint64_t *)arg0, (int)arg1);
            break;
        case UMAINCALL_GET_TIMEBASE:
            retval = (uint64_t)sys_get_timebase();
            break;
        case UMAINCALL_GET_TICK:
            retval = (uint64_t)sys_get_tick();
            break;
        case UMAINCALL_PRINTF:
            // asm volatile (
            //     "auipc t0,  0\n"
            //     "addi  t0,  t0,  20\n"
            //     "csrw  0x8b1,  t0\n"
            //     "mv    a0,  %[fmt]\n"
            //     "jal   ra,  %[func_name]\n"
            //     "nop"
            //     :
            //     :[fmt]"r"((void*)arg0), [func_name]"i"(&printf)
            //     :"t0", "a0", "ra"
            // );
            main_printf_1((void*)arg0, (uint64_t)arg1);
            break; 
        default:
            printf("Warning: Invalid umaincall number %u!\n", type);
            break;
    }

    write_csr(0x8b4, dasics_return_pc);             // DasicsReturnPC0
    write_csr(0x8b2, dasics_free_zone_return_pc);   // DasicsFreeZoneReturnPC

    //if(type == UMAINCALL_WRITE_AZONE_RETPC) write_csr(0x8b2, arg0); 

    // TODO: Use compiler to optimize such ugly code in the future ...
    asm volatile ("mv       a0, a5\n"\
                  "ld       ra, 88(sp)\n"\
                  "ld       s0, 80(sp)\n"\
                  "addi     sp, sp, 96\n"\
                  "ret\n"\
                  //"pulpret  x0,  0, x1\n" /* .word 0x0000f00b in little endian */
                  "nop");

    return retval;
}

// UMAIN FUNCTION

// uint32_t ATTR_UMAIN_TEXT dasics_libcfg_get(int32_t idx) {
//     if (idx < 0 || idx >= DASICS_LIBCFG_WIDTH) return -1;
//     uint64_t libcfg = read_csr(0x880);  // DasicsLibCfg
//     return (libcfg >> (idx * 4)) & DASICS_LIBCFG_MASK;
// }

int32_t ATTR_UMAIN_TEXT dasics_umain_libcfg_alloc(uint64_t cfg, uint64_t lo, uint64_t hi) {
    
    uint64_t libcfg = read_csr(0x880);  // DasicsLibCfg
    int32_t max_cfgs = DASICS_LIBCFG_WIDTH;
    int32_t target_idx;
    for (target_idx = 0; target_idx < max_cfgs; ++target_idx) {
        uint64_t curr_cfg = (libcfg >> (target_idx * 4)) & DASICS_LIBCFG_MASK;

        if ((curr_cfg & DASICS_LIBCFG_V) == 0)  // Found available config
        {
            switch (target_idx) { // write bound directly
                case 0:
                    write_csr(0x890, lo);   // DasicsLibBound0Lo
                    write_csr(0x891, hi);   // DasicsLibBound0Hi
                    break;
                case 1:
                    write_csr(0x892, lo);   // DasicsLibBound1Lo
                    write_csr(0x893, hi);   // DasicsLibBound1Hi
                    break;
                case 2:
                    write_csr(0x894, lo);   // DasicsLibBound2Lo
                    write_csr(0x895, hi);   // DasicsLibBound2Hi
                    break;
                case 3:
                    write_csr(0x896, lo);   // DasicsLibBound3Lo
                    write_csr(0x897, hi);   // DasicsLibBound3Hi
                    break;
                case 4:
                    write_csr(0x898, lo);   // DasicsLibBound4Lo
                    write_csr(0x899, hi);   // DasicsLibBound4Hi
                    break;
                case 5:
                    write_csr(0x89a, lo);   // DasicsLibBound5Lo
                    write_csr(0x89b, hi);   // DasicsLibBound5Hi
                    break;
                case 6:
                    write_csr(0x89c, lo);   // DasicsLibBound6Lo
                    write_csr(0x89d, hi);   // DasicsLibBound6Hi
                    break;
                case 7:
                    write_csr(0x89e, lo);   // DasicsLibBound7Lo
                    write_csr(0x89f, hi);   // DasicsLibBound7Hi
                    break;
                case 8:
                    write_csr(0x8a0, lo);   // DasicsLibBound8Lo
                    write_csr(0x8a1, hi);   // DasicsLibBound8Hi
                    break;
                case 9:
                    write_csr(0x8a2, lo);   // DasicsLibBound9Lo
                    write_csr(0x8a3, hi);   // DasicsLibBound9Hi
                    break;
                case 10:
                    write_csr(0x8a4, lo);   // DasicsLibBound10Lo
                    write_csr(0x8a5, hi);   // DasicsLibBound10Hi
                    break;
                case 11:
                    write_csr(0x8a6, lo);   // DasicsLibBound11Lo
                    write_csr(0x8a7, hi);   // DasicsLibBound11Hi
                    break;
                case 12:
                    write_csr(0x8a8, lo);   // DasicsLibBound12Lo
                    write_csr(0x8a9, hi);   // DasicsLibBound12Hi
                    break;
                case 13:
                    write_csr(0x8aa, lo);   // DasicsLibBound13Lo
                    write_csr(0x8ab, hi);   // DasicsLibBound13Hi
                    break;
                case 14:
                    write_csr(0x8ac, lo);   // DasicsLibBound14Lo
                    write_csr(0x8ad, hi);   // DasicsLibBound14Hi
                    break;
                case 15:
                    write_csr(0x8ae, lo);   // DasicsLibBound15Lo
                    write_csr(0x8af, hi);   // DasicsLibBound15Hi
                    break;
                default:
                    break;
            }
            
            // Write config
            libcfg &= ~(DASICS_LIBCFG_MASK << (target_idx * 4));
            libcfg |= (cfg & DASICS_LIBCFG_MASK) << (target_idx * 4);
            write_csr(0x880, libcfg);   // DasicsLibCfg

            return target_idx;
        }
    }

    return -1;
}

int32_t ATTR_UMAIN_TEXT dasics_umain_libcfg_free(int32_t idx) {
    if (idx < 0 || idx >= DASICS_LIBCFG_WIDTH) return -1;
    uint64_t libcfg = read_csr(0x880);  // DasicsLibCfg
    libcfg &= ~(DASICS_LIBCFG_V << (idx * 4));
    write_csr(0x880, libcfg);   // DasicsLibCfg
    return 0;
}



int32_t ATTR_UMAIN_TEXT dasics_umain_jumpcfg_alloc(uint64_t lo, uint64_t hi)
{
    uint64_t jumpcfg = read_csr(0x8c8);    // DasicsJumpCfg
    int32_t max_cfgs = DASICS_JUMPCFG_WIDTH;
    int32_t target_idx;
    for (target_idx = 0; target_idx < max_cfgs; ++target_idx) {
        uint64_t curr_cfg = (jumpcfg >> (target_idx * 16)) & DASICS_JUMPCFG_MASK;

        if ((curr_cfg & DASICS_JUMPCFG_V) == 0)  // Found available config
        {
            switch (target_idx) { // write bound directly
                case 0:
                    write_csr(0x8c0, lo);  // DasicsJumpBound0Lo
                    write_csr(0x8c1, hi);  // DasicsJumpBound0Hi
                    break;
                case 1:
                    write_csr(0x8c2, lo);  // DasicsJumpBound1Lo
                    write_csr(0x8c3, hi);  // DasicsJumpBound1Hi
                    break;
                case 2:
                    write_csr(0x8c4, lo);  // DasicsJumpBound2Lo
                    write_csr(0x8c5, hi);  // DasicsJumpBound2Hi
                    break;
                case 3:
                    write_csr(0x8c6, lo);  // DasicsJumpBound3Lo
                    write_csr(0x8c7, hi);  // DasicsJumpBound3Hi
                    break;
                default:
                    break;
            }
            jumpcfg &= ~(DASICS_JUMPCFG_MASK << (target_idx * 16));
            jumpcfg |= DASICS_JUMPCFG_V << (target_idx * 16);
            write_csr(0x8c8, jumpcfg); // DasicsJumpCfg

            return target_idx;
        }
    }

    return -1;
}

int32_t ATTR_UMAIN_TEXT dasics_umain_jumpcfg_free(int32_t idx) {
    if (idx < 0 || idx >= DASICS_JUMPCFG_WIDTH) return -1;
    uint64_t jumpcfg = read_csr(0x8c8);    // DasicsJumpCfg
    jumpcfg &= ~(DASICS_JUMPCFG_V << (idx * 16));
    write_csr(0x8c8, jumpcfg); // DasicsJumpCfg
    return 0;
}

// ULIB1 FUNCTION

static void ATTR_ULIB1_TEXT dasics_ulib1_printf_1(uint64_t fmt, uint64_t arg0){
    dasics_umaincall(UMAINCALL_PRINTF,fmt,arg0,0);
}

int32_t ATTR_ULIB1_TEXT dasics_ulib_libcfg_alloc(uint64_t cfg, uint64_t lo, uint64_t hi) {
    
    uint64_t libcfg = read_csr(0x880);  // DasicsLibCfg
    int32_t max_cfgs = DASICS_LIBCFG_WIDTH;
    uint64_t mem_bound_status = dasics_ulib_query_bound(TYPE_MEM_BOUND);
    int32_t target_idx,orig_idx;
    for (target_idx= 0; target_idx < max_cfgs; ++target_idx) {
        uint64_t curr_status = (mem_bound_status >> (target_idx * 2)) & 0x3;

        if (curr_status == 0x3){

            // try to find origin libcfg 
            for (orig_idx = 0; orig_idx < max_cfgs; ++orig_idx){
                uint64_t orig_status = (mem_bound_status >> (orig_idx * 2)) & 0x3;
                if (orig_status == 0x1){ // libcfg in the same level
                    uint64_t orig_cfg = (libcfg >> (orig_idx * 4)) & DASICS_LIBCFG_MASK;
                    uint64_t orig_lo,orig_hi;
                    switch (orig_idx) { // read origin libcfg
                    case 0:
                        orig_lo = read_csr(0x890);   // DasicsLibBound0Lo
                        orig_hi = read_csr(0x891);   // DasicsLibBound0Hi
                        break;
                    case 1:
                        orig_lo = read_csr(0x892);   // DasicsLibBound1Lo
                        orig_hi = read_csr(0x893);   // DasicsLibBound1Hi
                        break;
                    case 2:
                        orig_lo = read_csr(0x894);   // DasicsLibBound2Lo
                        orig_hi = read_csr(0x895);   // DasicsLibBound2Hi
                        break;
                    case 3:
                        orig_lo = read_csr(0x896);   // DasicsLibBound3Lo
                        orig_hi = read_csr(0x897);   // DasicsLibBound3Hi
                        break;
                    case 4:
                        orig_lo = read_csr(0x898);   // DasicsLibBound4Lo
                        orig_hi = read_csr(0x899);   // DasicsLibBound4Hi
                        break;
                    case 5:
                        orig_lo = read_csr(0x89a);   // DasicsLibBound5Lo
                        orig_hi = read_csr(0x89b);   // DasicsLibBound5Hi
                        break;
                    case 6:
                        orig_lo = read_csr(0x89c);   // DasicsLibBound6Lo
                        orig_hi = read_csr(0x89d);   // DasicsLibBound6Hi
                        break;
                    case 7:
                        orig_lo = read_csr(0x89e);   // DasicsLibBound7Lo
                        orig_hi = read_csr(0x89f);   // DasicsLibBound7Hi
                        break;
                    case 8:
                        orig_lo = read_csr(0x8a0);   // DasicsLibBound8Lo
                        orig_hi = read_csr(0x8a1);   // DasicsLibBound8Hi
                        break;
                    case 9:
                        orig_lo = read_csr(0x8a2);   // DasicsLibBound9Lo
                        orig_hi = read_csr(0x8a3);   // DasicsLibBound9Hi
                        break;
                    case 10:
                        orig_lo = read_csr(0x8a4);   // DasicsLibBound10Lo
                        orig_hi = read_csr(0x8a5);   // DasicsLibBound10Hi
                        break;
                    case 11:
                        orig_lo = read_csr(0x8a6);   // DasicsLibBound11Lo
                        orig_hi = read_csr(0x8a7);   // DasicsLibBound11Hi
                        break;
                    case 12:
                        orig_lo = read_csr(0x8a8);   // DasicsLibBound12Lo
                        orig_hi = read_csr(0x8a9);   // DasicsLibBound12Hi
                        break;
                    case 13:
                        orig_lo = read_csr(0x8aa);   // DasicsLibBound13Lo
                        orig_hi = read_csr(0x8ab);   // DasicsLibBound13Hi
                        break;
                    case 14:
                        orig_lo = read_csr(0x8ac);   // DasicsLibBound14Lo
                        orig_hi = read_csr(0x8ad);   // DasicsLibBound14Hi
                        break;
                    case 15:
                        orig_lo = read_csr(0x8ae);   // DasicsLibBound15Lo
                        orig_hi = read_csr(0x8af);   // DasicsLibBound15Hi
                        break;
                    default:
                        break;
                    }
                    if (orig_lo <= lo && hi <= orig_hi && !(cfg & ~orig_cfg)) break; // current field smaller than origin, OK
                }
            }
            if (orig_idx == max_cfgs) return -1; // no origin libcfg
            dasics_ulib_copy_bound(TYPE_MEM_BOUND,orig_idx,target_idx); // copy origin libcfg to target
            switch (target_idx) { // modify target libcfg according to arg
                case 0:
                    write_csr(0x890, lo);   // DasicsLibBound0Lo
                    write_csr(0x891, hi);   // DasicsLibBound0Hi
                    break;
                case 1:
                    write_csr(0x892, lo);   // DasicsLibBound1Lo
                    write_csr(0x893, hi);   // DasicsLibBound1Hi
                    break;
                case 2:
                    write_csr(0x894, lo);   // DasicsLibBound2Lo
                    write_csr(0x895, hi);   // DasicsLibBound2Hi
                    break;
                case 3:
                    write_csr(0x896, lo);   // DasicsLibBound3Lo
                    write_csr(0x897, hi);   // DasicsLibBound3Hi
                    break;
                case 4:
                    write_csr(0x898, lo);   // DasicsLibBound4Lo
                    write_csr(0x899, hi);   // DasicsLibBound4Hi
                    break;
                case 5:
                    write_csr(0x89a, lo);   // DasicsLibBound5Lo
                    write_csr(0x89b, hi);   // DasicsLibBound5Hi
                    break;
                case 6:
                    write_csr(0x89c, lo);   // DasicsLibBound6Lo
                    write_csr(0x89d, hi);   // DasicsLibBound6Hi
                    break;
                case 7:
                    write_csr(0x89e, lo);   // DasicsLibBound7Lo
                    write_csr(0x89f, hi);   // DasicsLibBound7Hi
                    break;
                case 8:
                    write_csr(0x8a0, lo);   // DasicsLibBound8Lo
                    write_csr(0x8a1, hi);   // DasicsLibBound8Hi
                    break;
                case 9:
                    write_csr(0x8a2, lo);   // DasicsLibBound9Lo
                    write_csr(0x8a3, hi);   // DasicsLibBound9Hi
                    break;
                case 10:
                    write_csr(0x8a4, lo);   // DasicsLibBound10Lo
                    write_csr(0x8a5, hi);   // DasicsLibBound10Hi
                    break;
                case 11:
                    write_csr(0x8a6, lo);   // DasicsLibBound11Lo
                    write_csr(0x8a7, hi);   // DasicsLibBound11Hi
                    break;
                case 12:
                    write_csr(0x8a8, lo);   // DasicsLibBound12Lo
                    write_csr(0x8a9, hi);   // DasicsLibBound12Hi
                    break;
                case 13:
                    write_csr(0x8aa, lo);   // DasicsLibBound13Lo
                    write_csr(0x8ab, hi);   // DasicsLibBound13Hi
                    break;
                case 14:
                    write_csr(0x8ac, lo);   // DasicsLibBound14Lo
                    write_csr(0x8ad, hi);   // DasicsLibBound14Hi
                    break;
                case 15:
                    write_csr(0x8ae, lo);   // DasicsLibBound15Lo
                    write_csr(0x8af, hi);   // DasicsLibBound15Hi
                    break;
                default:
                    break;
            }
            // Write config
            libcfg &= ~(DASICS_LIBCFG_MASK << (target_idx * 4));
            libcfg |= (cfg & DASICS_LIBCFG_MASK) << (target_idx * 4);
            write_csr(0x880, libcfg);   // DasicsLibCfg

            return target_idx;
        }
    }
    return -1;
}


int32_t ATTR_ULIB1_TEXT dasics_ulib_libcfg_copy(int src_idx) {
    uint64_t libcfg = read_csr(0x880);  // DasicsLibCfg
    int32_t max_cfgs = DASICS_LIBCFG_WIDTH;
    uint64_t mem_bound_status = dasics_ulib_query_bound(TYPE_MEM_BOUND);
    uint64_t src_status = (mem_bound_status >> (src_idx * 2)) & 0x3;
    if (src_status == 0 || src_status == 3) return -1;  // cannot copy
    int32_t target_idx;
    for (target_idx= 0; target_idx < max_cfgs; ++target_idx) {
        uint64_t curr_status = (mem_bound_status >> (target_idx * 2)) & 0x3;

        if (curr_status == 0x3){
            dasics_ulib_copy_bound(TYPE_MEM_BOUND,src_idx,target_idx); // copy origin libcfg to target
            return target_idx;
        }
    }
    return -1;
}

int32_t ATTR_ULIB1_TEXT dasics_ulib_libcfg_free(int32_t idx) {
    uint64_t mem_bound_status = dasics_ulib_query_bound(TYPE_MEM_BOUND);
    if (!(mem_bound_status >> (idx * 2) & 0x3)) return -1; // no permission

    if (idx < 0 || idx >= DASICS_LIBCFG_WIDTH) return -1;
    uint64_t libcfg = read_csr(0x880);  // DasicsLibCfg
    libcfg &= ~(DASICS_LIBCFG_V << (idx * 4));
    write_csr(0x880, libcfg);   // DasicsLibCfg
    return 0;
}

// uint32_t dasics_libcfg_get(int32_t idx) {
//     if (idx < 0 || idx >= DASICS_LIBCFG_WIDTH) return -1;
//     uint64_t libcfg = read_csr(0x880);  // DasicsLibCfg
//     return (libcfg >> (idx * 4)) & DASICS_LIBCFG_MASK;
// }

int32_t ATTR_ULIB1_TEXT dasics_ulib_jumpcfg_alloc(uint64_t lo, uint64_t hi)
{
    uint64_t jumpcfg = read_csr(0x8c8);    // DasicsJumpCfg
    int32_t max_cfgs = DASICS_JUMPCFG_WIDTH;
    uint64_t jmp_bound_status = dasics_ulib_query_bound(TYPE_JMP_BOUND);
    int32_t target_idx,orig_idx;
    for (target_idx = 0; target_idx < max_cfgs; ++target_idx) {
        uint64_t curr_status = (jmp_bound_status >> (target_idx * 2)) & 0x3;
        if (curr_status == 0x3) // found available cfg
        {
            // try to find origin jmpcfg 
            for (orig_idx = 0; orig_idx < max_cfgs; ++orig_idx){
                uint64_t orig_status = (jmp_bound_status >> (orig_idx * 2)) & 0x3;
                if (orig_status == 0x1){ // jmpcfg in the same level
                    uint64_t orig_lo,orig_hi;
                    switch (orig_idx) { // read origin jmpcfg
                        case 0:
                            orig_lo = read_csr(0x8c0);  // DasicsJumpBound0Lo
                            orig_hi = read_csr(0x8c1);  // DasicsJumpBound0Hi
                            break;
                        case 1:
                            orig_lo = read_csr(0x8c2);  // DasicsJumpBound1Lo
                            orig_hi = read_csr(0x8c3);  // DasicsJumpBound1Hi
                            break;
                        case 2:
                            orig_lo = read_csr(0x8c4);  // DasicsJumpBound2Lo
                            orig_hi = read_csr(0x8c5);  // DasicsJumpBound2Hi
                            break;
                        case 3:
                            orig_lo = read_csr(0x8c6);  // DasicsJumpBound3Lo
                            orig_hi = read_csr(0x8c7);  // DasicsJumpBound3Hi
                            break;
                        default:
                            break;
                    }
                    if (orig_lo <= lo && hi <= orig_hi) break; // current field smaller than origin, OK
                }
            }
            if (orig_idx == max_cfgs) return -1; // no origin jmpcfg
            dasics_ulib_copy_bound(TYPE_JMP_BOUND,orig_idx,31); // copy origin jmpcfg to scratchpad
            write_csr(0x8d2, lo);  // scratchpad_lo
            write_csr(0x8d3, hi);  // scratchpad_hi
            dasics_ulib_copy_bound(TYPE_JMP_BOUND,31,target_idx); // copy scratchpad to target jmpcfg

            jumpcfg &= ~(DASICS_JUMPCFG_MASK << (target_idx * 16));
            jumpcfg |= DASICS_JUMPCFG_V << (target_idx * 16);
            write_csr(0x8c8, jumpcfg); // DasicsJumpCfg
            return target_idx;
        }
    }
    return -1;
}

int32_t ATTR_ULIB1_TEXT dasics_ulib_jumpcfg_free(int32_t idx) {
    uint64_t jmp_bound_status = dasics_ulib_query_bound(TYPE_JMP_BOUND);
    if (!(jmp_bound_status >> (idx * 2) & 0x3)) return -1; // no permission
    
    if (idx < 0 || idx >= DASICS_JUMPCFG_WIDTH) return -1;
    uint64_t jumpcfg = read_csr(0x8c8);    // DasicsJumpCfg
    jumpcfg &= ~(DASICS_JUMPCFG_V << (idx * 16));
    write_csr(0x8c8, jumpcfg); // DasicsJumpCfg
    return 0;
}