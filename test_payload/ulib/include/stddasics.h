#ifndef _STDDASICS_H_
#define _STDDASICS_H_

#include <stdint.h>
#include <stdattr.h>

#define DASICS_LIBCFG_WIDTH 16
#define DASICS_LIBCFG_MASK  0xfUL
#define DASICS_LIBCFG_V     0x8UL
#define DASICS_LIBCFG_R     0x2UL
#define DASICS_LIBCFG_W     0x1UL

void ATTR_UMAIN_TEXT dasics_ufault_entry(void);
int32_t ATTR_UMAIN_TEXT dasics_libcfg_alloc(uint64_t cfg, uint64_t lo, uint64_t hi);
int32_t ATTR_UMAIN_TEXT dasics_libcfg_free(int32_t idx);
uint32_t dasics_libcfg_get(int32_t idx);

#endif