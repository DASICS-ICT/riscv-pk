#include <stdio.h>
#include "stdcsr.h"
#include "sys/syscall.h"

void dasics_example(void) {
    // unsigned long dasicsMainCfg = read_csr(0x880);
    printf("[MAIN] Hello!\n");
    sys_exit();
}
