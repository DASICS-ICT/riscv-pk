#include <stdio.h>
#include "stdcsr.h"
#include <stddasics.h>
#include "sys/syscall.h"

static char ATTR_ULIB1_DATA secret[128] = "[ULIB1]: It's the secret!\n";
static char ATTR_ULIB1_DATA ulib1_readonly[128] = "[ENTER ULIB1]: This is ro to ULIB1 and invisible to ULIB2!\n";
static char ATTR_ULIB1_DATA ulib1_rwbuffer[128] = "[ULIB1]: This is rw to ULIB1 and ro to ULIB2!\n";

static char ATTR_ULIB1_DATA main_prompt1[128] = "[UMAIN]: Ready to enter dasics_ulib1.\n";
static char ATTR_ULIB1_DATA main_prompt2[128] = "[UMAIN]: DasicsLibCfg0: 0x%lx\n";
static char ATTR_ULIB1_DATA main_prompt3[128] = "[UMAIN]: DasicsReturnPC: 0x%lx\n";
static char ATTR_ULIB1_DATA main_prompt4[128] = "[UMAIN]: DasicsFreeZoneReturnPC: 0x%lx\n";

static char ATTR_ULIB1_DATA ulib2_rwbuffer[128] = "[ENTER ULIB2]: This is rw to ULIB1 and ULIB2!";

void ATTR_UMAIN_TEXT dasics_main(void);
static void ATTR_ULIB1_TEXT dasics_ulib1(void);
static void ATTR_ULIB2_TEXT dasics_ulib2(void);
// extern void dasics_call_lib(void *arg0, void *arg1, void *arg2, void *arg3, void *func_name);


//extern lib_printf(const char *fmt, void* func_name, void* main_call);

// inline void lib_printf(const char *fmt, void* func_name, void* main_call){
//     asm volatile (
//         "auipc t0,  0\n"
//         "addi, t0,  t0  32\n"
//         "mv,   a0,  zero\n"
//         "addi, a0,  a0, 10\n"
//         "mv    a1,  t0\n"
//         "jal   ra,  %[main_call]\n"
//         "mv    a0,  %[fmt]\n"
//         "jal   ra,  %[func_name]"
//         "nop"
//         :
//         :[main_call]"r"(main_call), [fmt]"r"(fmt), [func_name]"r"(func_name)
//         :"t0", "a0", "a1", "ra"
//     );
// }

static void ATTR_ULIB1_TEXT dasics_ulib1_printf(uint64_t fmt){
    dasics_umaincall(UMAINCALL_PRINTF,fmt,0,0);
}

static void ATTR_ULIB2_TEXT dasics_ulib2_printf(uint64_t fmt){
    dasics_umaincall(UMAINCALL_PRINTF,fmt,0,0);
}

static void ATTR_ULIB2_TEXT dasics_ulib2(void){
    dasics_ulib2_printf((uint64_t) ulib2_rwbuffer); // That's ok

    char temp = ulib1_readonly[3];                // raise DasicsULoadAccessFault  (0x12)
    ulib1_readonly[3] = temp;                     // raise DasicsUStoreAccessFault (0x14)

    dasics_ulib1_printf((uint64_t) ulib1_rwbuffer); // raise DasicsUInstrAccessFault (0x10)
    dasics_ulib2_printf((uint64_t) ulib1_rwbuffer); // OK
    ulib1_rwbuffer[15] = 'B';                     // raise DasicsUStoreAccessFault (0x14)

    dasics_ulib1();                // raise DasicsUInstrAccessFault (0x10)

    ulib2_rwbuffer[2] = ulib1_readonly[3];  // 'T'; That's ok
    ulib2_rwbuffer[3] = 'H'; //That's ok
    dasics_ulib2_printf((uint64_t) ulib2_rwbuffer); // That's ok

}

static void ATTR_ULIB1_TEXT dasics_ulib1(void) {
    //lib_printf(pub_readonly,&printf, &dasics_umaincall);                 // That's ok
    dasics_ulib1_printf((uint64_t) ulib1_readonly);
    ulib1_readonly[15] = 'B';               // raise DasicsUStoreAccessFault (0x14)
    //lib_printf(pub_rwbuffer,&printf, &dasics_umaincall);                 // That's ok
    dasics_ulib1_printf((uint64_t) ulib1_rwbuffer);

    char temp = secret[3];                // raise DasicsULoadAccessFault  (0x12)
    secret[3] = temp;                     // raise DasicsUStoreAccessFault (0x14)

    dasics_main();                // raise DasicsUInstrAccessFault (0x10)
    
    //SET ULIB2 ENVIRONMENT
    int idx0, idx1, idx2;
    idx0 = dasics_ulib_libcfg_alloc(DASICS_LIBCFG_V | DASICS_LIBCFG_R                  , (uint64_t)ulib1_rwbuffer, (uint64_t)(ulib1_rwbuffer + 128));
    idx1 = dasics_ulib_libcfg_alloc(DASICS_LIBCFG_V | DASICS_LIBCFG_R | DASICS_LIBCFG_W, (uint64_t)ulib2_rwbuffer, (uint64_t)(ulib2_rwbuffer + 128));
    idx2 = dasics_ulib_libcfg_alloc(DASICS_LIBCFG_V                                    , (uint64_t)ulib1_readonly, (uint64_t)(ulib1_readonly + 128));
    extern char __ULIB2_TEXT_BEGIN__, __ULIB2_TEXT_END__;
    int32_t idx_ulib2 = dasics_ulib_jumpcfg_alloc((uint64_t)&__ULIB2_TEXT_BEGIN__, (uint64_t)&__ULIB2_TEXT_END__);

    //CALL ULIB2
    dasics_ulibcall_no_args(&dasics_ulib2);

    // sys_write(pub_rwbuffer);              // raise DasicsUInstrAccessFault (0x10)
    // printf(secret);                       // raise DasicsULoadAccessFault * 100

    ulib1_rwbuffer[2] = ulib1_readonly[3];  // 'T'; That's ok
    ulib1_rwbuffer[21] = 'B';               // That's ok
    ulib1_rwbuffer[22] = 'B';               // That's ok
    //lib_printf(pub_rwbuffer,&printf, &dasics_umaincall);                 
    dasics_ulib1_printf(ulib1_rwbuffer);    // That's ok
    ulib2_rwbuffer[24] = ulib1_readonly[25];// 'o'; That's ok
    ulib1_rwbuffer[25] = 'k';                // That's ok
    dasics_ulib2_printf(ulib2_rwbuffer);    // That's ok

    //FREE
    dasics_ulib_libcfg_free(idx2);
    dasics_ulib_libcfg_free(idx1);
    dasics_ulib_libcfg_free(idx0);
    dasics_ulib_jumpcfg_free(idx_ulib2);

}

void ATTR_UMAIN_TEXT dasics_main(void) {
    // Handlers of DasicsLibCfgs
    int idx0, idx1, idx2, idx3, idx4;

    // Init maincall and utvec
    dasics_init_umaincall((uint64_t)&dasics_umaincall);
    asm volatile ("csrw utvec, %0" :: "rK"((uint64_t)&dasics_ufault_entry));

    //printf("Hello, this is main function!\n");

    // Print main_prompt1
    idx0 = dasics_main_libcfg_alloc(DASICS_LIBCFG_V | DASICS_LIBCFG_R, (uint64_t)main_prompt1, (uint64_t)(main_prompt1 + 128));
    main_printf(main_prompt1);
    dasics_main_libcfg_free(idx0);

    // Allocate libcfg before calling lib function
    idx0 = dasics_main_libcfg_alloc(DASICS_LIBCFG_V | DASICS_LIBCFG_R                  , (uint64_t)ulib1_readonly, (uint64_t)(ulib1_readonly + 128));
    idx1 = dasics_main_libcfg_alloc(DASICS_LIBCFG_V | DASICS_LIBCFG_R | DASICS_LIBCFG_W, (uint64_t)ulib1_rwbuffer, (uint64_t)(ulib1_rwbuffer + 128));
    idx2 = dasics_main_libcfg_alloc(DASICS_LIBCFG_V                                    , (uint64_t)secret,       (uint64_t)(      secret + 128));
    idx4 = dasics_main_libcfg_alloc(DASICS_LIBCFG_V | DASICS_LIBCFG_R | DASICS_LIBCFG_W, (uint64_t)ulib2_rwbuffer, (uint64_t)(ulib2_rwbuffer + 128));


    extern char __ULIB1_TEXT_BEGIN__, __ULIB1_TEXT_END__;
    idx3 = dasics_main_jumpcfg_alloc((uint64_t)&__ULIB1_TEXT_BEGIN__, (uint64_t)&__ULIB1_TEXT_END__);

    // Jump to lib function
    //dasics_ulib1();
    dasics_maincall_no_args(&dasics_ulib1);
    dasics_main_jumpcfg_free(idx3);

    dasics_main_libcfg_free(idx4);
    
    // Print DasicsLibCfg0 and DasicsLibCfg1 csr value
    idx3 = dasics_main_libcfg_alloc(DASICS_LIBCFG_V | DASICS_LIBCFG_R, (uint64_t)main_prompt2, (uint64_t)(main_prompt2 + 128));
    //main_printf(main_prompt2, read_csr(0x880));
    dasics_main_libcfg_free(idx3);

    // Free those used libcfg via handlers
    dasics_main_libcfg_free(idx2);
    dasics_main_libcfg_free(idx1);
    dasics_main_libcfg_free(idx0);

    // Print values of DasicsReturnPC and DasicsFreeZoneReturnPC
    idx0 = dasics_main_libcfg_alloc(DASICS_LIBCFG_V | DASICS_LIBCFG_R, (uint64_t)main_prompt3, (uint64_t)(main_prompt3 + 128));
    //printf(main_prompt3, read_csr(0x8b1));
    dasics_main_libcfg_free(idx0);

    idx0 = dasics_main_libcfg_alloc(DASICS_LIBCFG_V | DASICS_LIBCFG_R, (uint64_t)main_prompt4, (uint64_t)(main_prompt4 + 128));
    //printf(main_prompt4, read_csr(0x8b2));
    dasics_main_libcfg_free(idx0);

    // Release all remained handlers and exit
    sys_exit();
}
