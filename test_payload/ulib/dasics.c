#include <stdint.h>
#include <stdcsr.h>
#include <stdattr.h>
#include <stddasics.h>
#include <sys/syscall.h>
#include <stdio.h>

void ATTR_UMAIN_TEXT dasics_init_umaincall(uint64_t entry)
{
    write_csr(0x8a3, entry);  // DasicsMaincallEntry
}

uint64_t ATTR_UMAIN_TEXT dasics_umaincall(UmaincallTypes type, uint64_t arg0, uint64_t arg1, uint64_t arg2)
{
    uint64_t dasics_return_pc = read_csr(0x8a4);            // DasicsReturnPC
    uint64_t dasics_free_zone_return_pc = read_csr(0x8a5);  // DasicsFreeZoneReturnPC

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
        default:
            printf("Warning: Invalid umaincall number %u!\n", type);
            break;
    }

    write_csr(0x8a4, dasics_return_pc);             // DasicsReturnPC
    write_csr(0x8a5, dasics_free_zone_return_pc);   // DasicsFreeZoneReturnPC

    // TODO: Use compiler to optimize such ugly code in the future ...
    asm volatile ("mv       a0, a5\n"\
                  "ld       ra, 88(sp)\n"\
                  "ld       s0, 80(sp)\n"\
                  "addi     sp, sp, 96\n"\
                  ".word    0x0000f00b\n" /*"pulpret   x0,  0, x1\n"*/\
                  "nop");

    return retval;
}

void ATTR_UMAIN_TEXT dasics_ufault_entry(void)
{
    // Save some registers that should be saved by callees
    uint64_t dasics_return_pc = read_csr(0x8a4);
    uint64_t dasics_free_zone_return_pc = read_csr(0x8a5);

    uint64_t ucause = read_csr(ucause);
    uint64_t utval = read_csr(utval);
    uint64_t uepc = read_csr(uepc);

    printf("Info: ufault occurs, ucause = 0x%x, uepc = 0x%x, utval = 0x%x\n", ucause, uepc, utval);

#ifdef DASICS_SKIP_FAULT_INST
    write_csr(uepc, uepc + 4);
#else
    printf("Info: ready to shutdown the program ...\n");
    sys_exit();
#endif

    // Restore those saved registers
    write_csr(0x8a4, dasics_return_pc);
    write_csr(0x8a5, dasics_free_zone_return_pc);

    asm volatile ("ld   ra, 88(sp)\n"\
                  "ld   s0, 80(sp)\n"\
                  "addi sp, sp, 96\n"\
                  "uret");
}

int32_t ATTR_UMAIN_TEXT dasics_libcfg_alloc(uint64_t cfg, uint64_t hi, uint64_t lo)
{
    uint64_t libcfg0 = read_csr(0x881);  // DasicsLibCfg0

#ifndef RV32
    int32_t step = 8;  // rv64
#else
    int32_t step = 4;  // rv32
#endif

    for (int32_t idx = 0; idx < DASICS_LIBCFG_WIDTH; ++idx)
    {
        uint64_t curr_cfg = (libcfg0 >> (idx * step)) & DASICS_LIBCFG_MASK;

        if ((curr_cfg & DASICS_LIBCFG_V) == 0)  // Find avaliable cfg
        {
            libcfg0 &= ~(DASICS_LIBCFG_MASK << (idx * step));
            libcfg0 |= (cfg & DASICS_LIBCFG_MASK) << (idx * step);
            write_csr(0x881, libcfg0);  // DasicsLibCfg0

            // Write DASICS boundary csrs
            switch (idx)
            {
                case 0:
                    write_csr(0x883, hi);  // DasicsLibBound0
                    write_csr(0x884, lo);  // DasicsLibBound1
                    break;
                case 1:
                    write_csr(0x885, hi);  // DasicsLibBound2
                    write_csr(0x886, lo);  // DasicsLibBound3
                    break;
                case 2:
                    write_csr(0x887, hi);  // DasicsLibBound4
                    write_csr(0x888, lo);  // DasicsLibBound5
                    break;
                case 3:
                    write_csr(0x889, hi);  // DasicsLibBound6
                    write_csr(0x88a, lo);  // DasicsLibBound7
                    break;
                case 4:
                    write_csr(0x88b, hi);  // DasicsLibBound8
                    write_csr(0x88c, lo);  // DasicsLibBound9
                    break;
                case 5:
                    write_csr(0x88d, hi);  // DasicsLibBound10
                    write_csr(0x88e, lo);  // DasicsLibBound11
                    break;
                case 6:
                    write_csr(0x88f, hi);  // DasicsLibBound12
                    write_csr(0x890, lo);  // DasicsLibBound13
                    break;
                default:
                    write_csr(0x891, hi);  // DasicsLibBound14
                    write_csr(0x892, lo);  // DasicsLibBound15
                    break;
            }

            return idx;
        }
    }

    return -1;
}

int32_t ATTR_UMAIN_TEXT dasics_libcfg_free(int32_t idx)
{
    if (idx < 0 || idx >= DASICS_LIBCFG_WIDTH)
    {
        return -1;
    }

#ifndef RV32
    int32_t step = 8;  // rv64
#else
    int32_t step = 4;  // rv32
#endif

    uint64_t libcfg0 = read_csr(0x881);  // DasicsLibCfg0
    libcfg0 &= ~(DASICS_LIBCFG_V << (idx * step));

    write_csr(0x881, libcfg0);  // DasicsLibCfg0

    return 0;
}

uint32_t dasics_libcfg_get(int32_t idx)
{
    if (idx < 0 || idx >= DASICS_LIBCFG_WIDTH)
    {
        return -1;
    }

#ifndef RV32
    int32_t step = 8;  // rv64
#else
    int32_t step = 4;  // rv32
#endif

    uint64_t libcfg0 = read_csr(0x881);  // DasicsLibCfg0

    return (libcfg0 >> (idx * step)) & DASICS_LIBCFG_MASK;
}