#include <stdio.h>
#include "stdcsr.h"
#include <stddasics.h>
#include "sys/syscall.h"

static char ATTR_ULIB_DATA secret[128] = "[ULIB1]: It's the secret!\n";
static char ATTR_ULIB_DATA pub_readonly[128] = "[ULIB1]: Enter dasics_ulib1 ...\n";
static char ATTR_ULIB_DATA pub_rwbuffer[128] = "[ULIB1]: It's public rw buffer!\n";

static char ATTR_ULIB_DATA main_prompt1[128] = "[UMAIN]: Ready to enter dasics_ulib1.\n";
static char ATTR_ULIB_DATA main_prompt2[128] = "[UMAIN]: DasicsLibCfg0: 0x%lx\n";
static char ATTR_ULIB_DATA main_prompt3[128] = "[UMAIN]: DasicsReturnPC: 0x%lx\n";
static char ATTR_ULIB_DATA main_prompt4[128] = "[UMAIN]: DasicsFreeZoneReturnPC: 0x%lx\n";

static void ATTR_ULIB_TEXT dasics_ulib1(void) {
    printf(pub_readonly);                 // That's ok
    pub_readonly[15] = 'B';               // raise DasicsUStoreAccessFault (0x14)
    printf(pub_rwbuffer);                 // That's ok

    char temp = secret[3];                // raise DasicsULoadAccessFault  (0x12)
    secret[3] = temp;                     // raise DasicsUStoreAccessFault (0x14)

    dasics_main();                // raise DasicsUInstrAccessFault (0x10)
    // sys_write(pub_rwbuffer);              // raise DasicsUInstrAccessFault (0x10)
    // printf(secret);                       // raise DasicsULoadAccessFault * 100

    pub_rwbuffer[19] = pub_readonly[12];  // That's ok
    pub_rwbuffer[21] = 'B';               // That's ok
    pub_rwbuffer[22] = 'B';               // That's ok
    printf(pub_rwbuffer);                 // That's ok

}

void ATTR_UMAIN_TEXT dasics_main(void) {
    // Handlers of DasicsLibCfgs
    int idx0, idx1, idx2, idx3;

    // Init maincall and utvec
    dasics_init_umaincall((uint64_t)&dasics_umaincall);
    asm volatile ("csrw utvec, %0" :: "rK"((uint64_t)&dasics_ufault_entry));

    //printf("Hello, this is main function!\n");

    // Print main_prompt1
    idx0 = dasics_libcfg_alloc(DASICS_LIBCFG_V | DASICS_LIBCFG_R, (uint64_t)main_prompt1, (uint64_t)(main_prompt1 + 128));
    printf(main_prompt1);
    dasics_libcfg_free(idx0);

    // Allocate libcfg before calling lib function
    idx0 = dasics_libcfg_alloc(DASICS_LIBCFG_V | DASICS_LIBCFG_R                  , (uint64_t)pub_readonly, (uint64_t)(pub_readonly + 128));
    idx1 = dasics_libcfg_alloc(DASICS_LIBCFG_V | DASICS_LIBCFG_R | DASICS_LIBCFG_W, (uint64_t)pub_rwbuffer, (uint64_t)(pub_rwbuffer + 128));
    idx2 = dasics_libcfg_alloc(DASICS_LIBCFG_V                                    , (uint64_t)secret,       (uint64_t)(      secret + 128));

    // Jump to lib function
    dasics_ulib1();

    // Print DasicsLibCfg0 and DasicsLibCfg1 csr value
    idx3 = dasics_libcfg_alloc(DASICS_LIBCFG_V | DASICS_LIBCFG_R, (uint64_t)main_prompt2, (uint64_t)(main_prompt2 + 128));
    printf(main_prompt2, read_csr(0x880));
    dasics_libcfg_free(idx3);

    // Free those used libcfg via handlers
    dasics_libcfg_free(idx2);
    dasics_libcfg_free(idx1);
    dasics_libcfg_free(idx0);

    // Print values of DasicsReturnPC and DasicsFreeZoneReturnPC
    idx0 = dasics_libcfg_alloc(DASICS_LIBCFG_V | DASICS_LIBCFG_R, (uint64_t)main_prompt3, (uint64_t)(main_prompt3 + 128));
    printf(main_prompt3, read_csr(0x8b1));
    dasics_libcfg_free(idx0);

    idx0 = dasics_libcfg_alloc(DASICS_LIBCFG_V | DASICS_LIBCFG_R, (uint64_t)main_prompt4, (uint64_t)(main_prompt4 + 128));
    printf(main_prompt4, read_csr(0x8b2));
    dasics_libcfg_free(idx0);

    // Release all remained handlers and exit
    sys_exit();
}
