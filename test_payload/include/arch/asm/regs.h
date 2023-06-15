/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * * * * *
 *            Copyright (C) 2018 Institute of Computing Technology, CAS
 *               Author : Han Shukai (email : hanshukai@ict.ac.cn)
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * * * * *
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * * * * * */

#ifndef INCLUDE_REGS_H_
#define INCLUDE_REGS_H_

/* This is for struct TrapFrame in scheduler.h
 * Stack layout for all exceptions:
 *
 * ptrace needs to have all regs on the stack. If the order here is changed,
 * it needs to be updated in include/asm-mips/ptrace.h
 *
 * The first PTRSIZE*5 bytes are argument save space for C subroutines.
 */

#define OFFSET_REG_ZERO         0

/* return address */
#define OFFSET_REG_RA           8

/* pointers */
#define OFFSET_REG_SP           16 // stack
#define OFFSET_REG_GP           24 // global
#define OFFSET_REG_TP           32 // thread

/* temporary */
#define OFFSET_REG_T0           40
#define OFFSET_REG_T1           48
#define OFFSET_REG_T2           56

/* saved register */
#define OFFSET_REG_S0           64
#define OFFSET_REG_S1           72

/* args */
#define OFFSET_REG_A0           80
#define OFFSET_REG_A1           88
#define OFFSET_REG_A2           96
#define OFFSET_REG_A3           104
#define OFFSET_REG_A4           112
#define OFFSET_REG_A5           120
#define OFFSET_REG_A6           128
#define OFFSET_REG_A7           136

/* saved register */
#define OFFSET_REG_S2           144
#define OFFSET_REG_S3           152
#define OFFSET_REG_S4           160
#define OFFSET_REG_S5           168
#define OFFSET_REG_S6           176
#define OFFSET_REG_S7           184
#define OFFSET_REG_S8           192
#define OFFSET_REG_S9           200
#define OFFSET_REG_S10          208
#define OFFSET_REG_S11          216

/* temporary register */
#define OFFSET_REG_T3           224
#define OFFSET_REG_T4           232
#define OFFSET_REG_T5           240
#define OFFSET_REG_T6           248

/* privileged register */
#define OFFSET_REG_SSTATUS      256
#define OFFSET_REG_SEPC         264
#define OFFSET_REG_SBADADDR     272
#define OFFSET_REG_SCAUSE       280

#define OFFSET_REG_USTATUS      288
#define OFFSET_REG_UEPC         296
#define OFFSET_REG_UBADADDR     304
#define OFFSET_REG_UCAUSE       312
#define OFFSET_REG_UTVEC        320
#define OFFSET_REG_UIE          328
#define OFFSET_REG_UIP          336
#define OFFSET_REG_USCRATCH     344

/* DASICS register */
#define OFFSET_REG_DUMCFG       352
#define OFFSET_REG_DUMBOUNDHI   360
#define OFFSET_REG_DUMBOUNDLO   368

#define OFFSET_REG_DLCFG0       376
#define OFFSET_REG_DLCFG1       384

#define OFFSET_REG_DLBOUND0LO   392
#define OFFSET_REG_DLBOUND0HI   400
#define OFFSET_REG_DLBOUND1LO   408
#define OFFSET_REG_DLBOUND1HI   416
#define OFFSET_REG_DLBOUND2LO   424
#define OFFSET_REG_DLBOUND2HI   432
#define OFFSET_REG_DLBOUND3LO   440
#define OFFSET_REG_DLBOUND3HI   448
#define OFFSET_REG_DLBOUND4LO   456
#define OFFSET_REG_DLBOUND4HI   464
#define OFFSET_REG_DLBOUND5LO   472
#define OFFSET_REG_DLBOUND5HI   480
#define OFFSET_REG_DLBOUND6LO   488
#define OFFSET_REG_DLBOUND6HI   496
#define OFFSET_REG_DLBOUND7LO   504
#define OFFSET_REG_DLBOUND7HI   512
#define OFFSET_REG_DLBOUND8LO   520
#define OFFSET_REG_DLBOUND8HI   528
#define OFFSET_REG_DLBOUND9LO   536
#define OFFSET_REG_DLBOUND9HI   544
#define OFFSET_REG_DLBOUND10LO  552
#define OFFSET_REG_DLBOUND10HI  560
#define OFFSET_REG_DLBOUND11LO  568
#define OFFSET_REG_DLBOUND11HI  576
#define OFFSET_REG_DLBOUND12LO  584
#define OFFSET_REG_DLBOUND12HI  592
#define OFFSET_REG_DLBOUND13LO  600
#define OFFSET_REG_DLBOUND13HI  608
#define OFFSET_REG_DLBOUND14LO  616
#define OFFSET_REG_DLBOUND14HI  624
#define OFFSET_REG_DLBOUND15LO  632
#define OFFSET_REG_DLBOUND15HI  640

#define OFFSET_REG_DMAINCALL    648
#define OFFSET_REG_DRETURNPC    656
#define OFFSET_REG_DFZRETURN    664

#define OFFSET_REG_DJBOUND0LO   672
#define OFFSET_REG_DJBOUND0HI   680
#define OFFSET_REG_DJBOUND1LO   688
#define OFFSET_REG_DJBOUND1HI   696
#define OFFSET_REG_DJBOUND2LO   704
#define OFFSET_REG_DJBOUND2HI   712
#define OFFSET_REG_DJBOUND3LO   720
#define OFFSET_REG_DJBOUND3HI   728

#define OFFSET_REG_DJCFG        736

/* Size of stack frame, word/double word alignment */
#define OFFSET_SIZE             744

#define PCB_KERNEL_SP          0
#define PCB_USER_SP            8
#define PCB_PREEMPT_COUNT      16
#define PCB_KERNEL_STACK_BASE  24
#define PCB_USER_STACK_BASE    32

/* offset in switch_to */
#define SWITCH_TO_RA     0
#define SWITCH_TO_SP     8
#define SWITCH_TO_S0     16
#define SWITCH_TO_S1     24
#define SWITCH_TO_S2     32
#define SWITCH_TO_S3     40
#define SWITCH_TO_S4     48
#define SWITCH_TO_S5     56
#define SWITCH_TO_S6     64
#define SWITCH_TO_S7     72
#define SWITCH_TO_S8     80
#define SWITCH_TO_S9     88
#define SWITCH_TO_S10    96
#define SWITCH_TO_S11    104

#define SWITCH_TO_SIZE   112

#endif
