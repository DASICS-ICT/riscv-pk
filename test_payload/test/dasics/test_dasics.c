#include <stdio.h>
#include "stdcsr.h"
#include <stddasics.h>
#include "sys/syscall.h"

static char ATTR_ULIB_DATA pub_readonly[100] = "[ULIB]: Entering dasics_ulib1...\n";
static char ATTR_ULIB_DATA pub_rwbuffer[100] = "[ULIB]: It's a RW buffer!\n";

static char ATTR_ULIB_DATA main_prompt1[100] = "[UMAIN]: Ready to enter dasics_ulib1.\n";

static void ATTR_ULIB_TEXT dasics_ulib1(void) {
    printf(pub_readonly);
    pub_readonly[15] = 'B';     // raise DasicsUStoreFault (0x1c)
    printf(pub_rwbuffer);

}

void dasics_main(void) {
    // Handlers of DasicsLibCfgs
    int idx0, idx1, idx2, idx3;

    // init utvec
    asm volatile ("csrw utvec, %0" :: "rK"((uint64_t)&dasics_ufault_entry));

    // Print main_prompt1
    idx0 = dasics_libcfg_alloc(DASICS_LIBCFG_V | DASICS_LIBCFG_R,
        (uint64_t)main_prompt1, (uint64_t)(main_prompt1 + 100));
    printf(main_prompt1);
    dasics_libcfg_free(idx0);

    // Allocate libcfg for dasics_ulib1
    idx0 = dasics_libcfg_alloc(DASICS_LIBCFG_V | DASICS_LIBCFG_R,
        (uint64_t) pub_readonly, (uint64_t)(pub_readonly + 100));
    idx1 = dasics_libcfg_alloc(DASICS_LIBCFG_V | DASICS_LIBCFG_R | DASICS_LIBCFG_W,
        (uint64_t) pub_rwbuffer, (uint64_t)(pub_rwbuffer + 100));

    sys_exit();
}
