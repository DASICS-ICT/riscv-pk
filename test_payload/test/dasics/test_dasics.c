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

//extern lib_printf(const char *fmt, void* func_name, void* main_call);

// inline void lib_printf(const char *fmt, void* func_name, void* main_call){
//     asm volatile (
//         "auipc t0,  0\n"\
//         "addi, t0,  t0  32\n"\
//         "mv,   a0,  zero\n"\
//         "addi, a0,  a0, 10\n"\
//         "mv    a1,  t0\n"\
//         "jal   ra,  %[main_call]\n"\
//         "mv    a0,  %[fmt]\n"\
//         "jal   ra,  %[func_name]"
//         "nop"
//         :
//         :[main_call]"r"(main_call), [fmt]"r"(fmt), [func_name]"r"(func_name)
//         :"t0", "a0", "a1", "ra"
//     );
// }

static void ATTR_ULIB_TEXT dasics_ulib2(void){}

static void ATTR_ULIB_TEXT dasics_ulib1(void) {
    //lib_printf(pub_readonly,&printf, &dasics_umaincall);                 // That's ok
    dasics_umaincall(UMAINCALL_PRINTF, pub_readonly,0,0);
    pub_readonly[15] = 'B';               // raise DasicsUStoreAccessFault (0x14)
    //lib_printf(pub_rwbuffer,&printf, &dasics_umaincall);                 // That's ok
    dasics_umaincall(UMAINCALL_PRINTF, pub_rwbuffer,0,0);

    char temp = secret[3];                // raise DasicsULoadAccessFault  (0x12)
    secret[3] = temp;                     // raise DasicsUStoreAccessFault (0x14)

    dasics_main();                // raise DasicsUInstrAccessFault (0x10)
    dasics_ulib2();                 // raise DasicsUInstrAccessFault (0x10)
    // sys_write(pub_rwbuffer);              // raise DasicsUInstrAccessFault (0x10)
    // printf(secret);                       // raise DasicsULoadAccessFault * 100

    pub_rwbuffer[19] = pub_readonly[12];  // That's ok
    pub_rwbuffer[21] = 'B';               // That's ok
    pub_rwbuffer[22] = 'B';               // That's ok
    //lib_printf(pub_rwbuffer,&printf, &dasics_umaincall);                 // That's ok
    dasics_umaincall(UMAINCALL_PRINTF, pub_rwbuffer,0,0);

}

extern main_printf(const char *fmt, void* func_name);
extern main_call_lib(void* func_name);

void ATTR_UMAIN_TEXT dasics_main(void) {
    // Handlers of DasicsLibCfgs
    int idx0, idx1, idx2, idx3;

    // Init maincall and utvec
    dasics_init_umaincall((uint64_t)&dasics_umaincall);
    asm volatile ("csrw utvec, %0" :: "rK"((uint64_t)&dasics_ufault_entry));

    //printf("Hello, this is main function!\n");

    // Print main_prompt1
    idx0 = dasics_libcfg_alloc(DASICS_LIBCFG_V | DASICS_LIBCFG_R, (uint64_t)main_prompt1, (uint64_t)(main_prompt1 + 128));
    main_printf(main_prompt1, &printf);
    dasics_libcfg_free(idx0);

    // Allocate libcfg before calling lib function
    idx0 = dasics_libcfg_alloc(DASICS_LIBCFG_V | DASICS_LIBCFG_R                  , (uint64_t)pub_readonly, (uint64_t)(pub_readonly + 128));
    idx1 = dasics_libcfg_alloc(DASICS_LIBCFG_V | DASICS_LIBCFG_R | DASICS_LIBCFG_W, (uint64_t)pub_rwbuffer, (uint64_t)(pub_rwbuffer + 128));
    idx2 = dasics_libcfg_alloc(DASICS_LIBCFG_V                                    , (uint64_t)secret,       (uint64_t)(      secret + 128));

    // Jump to lib function
    //dasics_ulib1();
    main_call_lib(&dasics_ulib1);

    // Print DasicsLibCfg0 and DasicsLibCfg1 csr value
    idx3 = dasics_libcfg_alloc(DASICS_LIBCFG_V | DASICS_LIBCFG_R, (uint64_t)main_prompt2, (uint64_t)(main_prompt2 + 128));
    //main_printf(main_prompt2, read_csr(0x880));
    dasics_libcfg_free(idx3);

    // Free those used libcfg via handlers
    dasics_libcfg_free(idx2);
    dasics_libcfg_free(idx1);
    dasics_libcfg_free(idx0);

    // Print values of DasicsReturnPC and DasicsFreeZoneReturnPC
    idx0 = dasics_libcfg_alloc(DASICS_LIBCFG_V | DASICS_LIBCFG_R, (uint64_t)main_prompt3, (uint64_t)(main_prompt3 + 128));
    //printf(main_prompt3, read_csr(0x8b1));
    dasics_libcfg_free(idx0);

    idx0 = dasics_libcfg_alloc(DASICS_LIBCFG_V | DASICS_LIBCFG_R, (uint64_t)main_prompt4, (uint64_t)(main_prompt4 + 128));
    //printf(main_prompt4, read_csr(0x8b2));
    dasics_libcfg_free(idx0);

    // Release all remained handlers and exit
    sys_exit();
}
