#ifndef _STDDASICS_H_
#define _STDDASICS_H_

#include <stdint.h>
#include <stdattr.h>

#define DASICS_LIBCFG_WIDTH 16
#define DASICS_LIBCFG_MASK  0xfUL
#define DASICS_LIBCFG_V     0x8UL
#define DASICS_LIBCFG_R     0x2UL
#define DASICS_LIBCFG_W     0x1UL

#define DASICS_JUMPCFG_WIDTH 4
#define DASICS_JUMPCFG_MASK 0xffffUL
#define DASICS_JUMPCFG_V    0x1UL

typedef enum {
    UMAINCALL_EXIT,
    UMAINCALL_YIELD,
    UMAINCALL_SLEEP,
    UMAINCALL_WRITE,
    UMAINCALL_REFLUSH,
    UMAINCALL_MOVE_CURSOR,
    UMAINCALL_FUTEX_WAIT,
    UMAINCALL_FUTEX_WAKEUP,
    UMAINCALL_GET_TIMEBASE,
    UMAINCALL_GET_TICK,
    UMAINCALL_PRINTF
} UmaincallTypes;

enum ExcCode
{
    EXCC_DASICS_UINSTR_FAULT = 24,
    EXCC_DASICS_ULOAD_FAULT = 26,
    EXCC_DASICS_USTORE_FAULT = 28,
    EXCC_DASICS_UECALL_FAULT = 30,
};

#define TYPE_MEM_BOUND 0
#define TYPE_JMP_BOUND 1

//MAIN

void     ATTR_UMAIN_TEXT dasics_init_umaincall(uint64_t entry);
uint64_t ATTR_UMAIN_TEXT dasics_umaincall(UmaincallTypes type, uint64_t arg0, uint64_t arg1, uint64_t arg2);
void     ATTR_UMAIN_TEXT dasics_ufault_entry(void);
// uint32_t ATTR_UMAIN_TEXT dasics_libcfg_get(int32_t idx);
int32_t  ATTR_UMAIN_TEXT dasics_main_libcfg_alloc(uint64_t cfg, uint64_t lo ,uint64_t hi);
int32_t  ATTR_UMAIN_TEXT dasics_main_libcfg_free(int32_t idx);
int32_t  ATTR_UMAIN_TEXT dasics_main_jumpcfg_alloc(uint64_t lo, uint64_t hi);
int32_t  ATTR_UMAIN_TEXT dasics_main_jumpcfg_free(int32_t idx);

void dasics_umain_libcall(void *arg0, void *arg1, void *arg2, void *arg3, void *func_name);
#define dasics_umain_libcall_no_args(func_name) (dasics_umain_libcall(0, 0, 0, 0, func_name))
#define main_printf(fmt) (dasics_umain_libcall(fmt,0,0,0,&printf))
#define main_printf_1(fmt, arg0) (dasics_umain_libcall(fmt, arg0, 0, 0, &printf))

//ULIB

int32_t  ATTR_ULIB1_TEXT dasics_ulib_libcfg_alloc(uint64_t cfg, uint64_t lo ,uint64_t hi);
int32_t  ATTR_ULIB1_TEXT dasics_ulib_libcfg_free(int32_t idx);
int32_t  ATTR_ULIB1_TEXT dasics_ulib_jumpcfg_alloc(uint64_t lo, uint64_t hi);
int32_t  ATTR_ULIB1_TEXT dasics_ulib_jumpcfg_free(int32_t idx);

void dasics_ulib_libcall(void *arg0, void *arg1, void *arg2, void *arg3, void *func_name);
#define dasics_ulib_libcall_no_args(func_name) (dasics_ulib_libcall(0, 0, 0, 0, func_name))

void dasics_ulib_copy_mem_bound(int bound_src, int bound_dst);
void dasics_ulib_copy_jmp_bound(int bound_src, int bound_dst);
uint64_t dasics_ulib_query_mem_bound(void);
uint64_t dasics_ulib_query_jmp_bound(void);

#define dasics_ulib_copy_bound(type,bound_src,bound_dst) ((type)?\
                                                    dasics_ulib_copy_jmp_bound(bound_src, bound_dst):\
                                                    dasics_ulib_copy_mem_bound(bound_src, bound_dst))
#define dasics_ulib_query_bound(type) ((type)?\
                                  dasics_ulib_query_jmp_bound():\
                                  dasics_ulib_query_mem_bound())
#endif
