#include <stdcsr.h>
#include <stddasics.h>
#include <sys/syscall.h>
#include <stdio.h>

void ATTR_UMAIN_TEXT dasics_init_umaincall(uint64_t entry)
{
    write_csr(0x8b0, entry);  // DasicsMaincallEntry
}


extern int main_printf(const char *fmt, void* func_name);

void ATTR_UMAIN_TEXT dasics_ufault_entry(void) {
    // Save some registers that should be saved by callees
    uint64_t dasics_return_pc = read_csr(0x8b1);            // DasicsReturnPC
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
            main_printf("[HANDLE_U_DASICS]: Detect UInst Access Fault! Skip this instruction!\n", &printf);
            break;
        case EXCC_DASICS_ULOAD_FAULT:
            main_printf("[HANDLE_U_DASICS]: Detect ULoad Access Fault! Skip this instruction!\n", &printf);
            break;
        case EXCC_DASICS_USTORE_FAULT:
            main_printf("[HANDLE_U_DASICS]: Detect UStore Access Fault! Skip this instruction!\n", &printf);
            break;
        default:
            main_printf("[HANDLE_U_DASICS]: Invalid cause! exit..", &printf);
            //printf("[HANDLE_U_DASICS]: Invalid cause 0x%lx detected!\n");
            // printf("ustatus: 0x%lx uepc: 0x%lx ucause: %lu\n\r",
            //     ustatus, uepc, ucause);
            sys_exit();
            break;
    }
    write_csr(uepc, uepc + 4);  // For debugging
// #endif

    // Restore those saved registers
    write_csr(0x8b1, dasics_return_pc);
    write_csr(0x8b2, dasics_free_zone_return_pc);

    asm volatile ("ld   ra, 104(sp)\n"\
                  "ld   s0, 96(sp)\n"\
                  "addi sp, sp, 112\n"\
                  "uret");
}


uint64_t ATTR_UMAIN_TEXT dasics_umaincall(UmaincallTypes type, uint64_t arg0, uint64_t arg1, uint64_t arg2)
{
    uint64_t dasics_return_pc = read_csr(0x8b1);            // DasicsReturnPC
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
            main_printf((void*)arg0, &printf);
            break; 
        default:
            printf("Warning: Invalid umaincall number %u!\n", type);
            break;
    }

    write_csr(0x8b1, dasics_return_pc);             // DasicsReturnPC
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

int32_t ATTR_UMAIN_TEXT dasics_libcfg_alloc(uint64_t cfg, uint64_t lo, uint64_t hi) {
    uint64_t libcfg = read_csr(0x880);  // DasicsLibCfg
    int32_t max_cfgs = DASICS_LIBCFG_WIDTH;
    int32_t step = 4;

    for (int32_t idx = 0; idx < max_cfgs; ++idx) {
        uint64_t curr_cfg = (libcfg >> (idx * step)) & DASICS_LIBCFG_MASK;

        if ((curr_cfg & DASICS_LIBCFG_V) == 0)  // Found available config
        {
            // Write DASICS bounds csr
            switch (idx) {
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
            libcfg &= ~(DASICS_LIBCFG_MASK << (idx * step));
            libcfg |= (cfg & DASICS_LIBCFG_MASK) << (idx * step);
            write_csr(0x880, libcfg);   // DasicsLibCfg

            return idx;
        }
    }

    return -1;
}

int32_t ATTR_UMAIN_TEXT dasics_libcfg_free(int32_t idx) {
    if (idx < 0 || idx >= DASICS_LIBCFG_WIDTH) return -1;

    int32_t step = 4;
    uint64_t libcfg = read_csr(0x880);  // DasicsLibCfg
    libcfg &= ~(DASICS_LIBCFG_V << (idx * step));
    write_csr(0x880, libcfg);   // DasicsLibCfg
    return 0;
}

uint32_t dasics_libcfg_get(int32_t idx) {
    if (idx < 0 || idx >= DASICS_LIBCFG_WIDTH) return -1;

    int32_t step = 4;
    uint64_t libcfg = read_csr(0x880);  // DasicsLibCfg
    return (libcfg >> (idx * step)) & DASICS_LIBCFG_MASK;
}