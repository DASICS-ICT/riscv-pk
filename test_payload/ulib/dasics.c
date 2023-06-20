#include <stdcsr.h>
#include <stddasics.h>
#include <sys/syscall.h>
#include <stdio.h>

void ATTR_UMAIN_TEXT dasics_ufault_entry(void) {
    uint64_t ucause = read_csr(ucause);
    uint64_t utval = read_csr(utval);
    uint64_t uepc = read_csr(uepc);

    printf("Info: ufault occurs, ucause = 0x%x, uepc = 0x%x, utval = 0x%x\n", ucause, uepc, utval);
    sys_exit();
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