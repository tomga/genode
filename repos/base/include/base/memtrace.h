/*
 * \brief  Writing traces to memory buffer
 * \author Tomasz Gajewski
 * \date   2019-01-26
 */

/*
 * Copyright (C) 2016-2019 Genode Labs GmbH
 *
 * This file is part of the Genode OS framework, which is distributed
 * under the terms of the GNU Affero General Public License version 3.
 */

#ifndef _INCLUDE__BASE__MEMTRACE_H_
#define _INCLUDE__BASE__MEMTRACE_H_

#ifdef GENODE_RELEASE
#define DISABLE_MEMTRACE
#endif


// comment to enable
#define DISABLE_MEMTRACE


#ifndef DISABLE_MEMTRACE

#define MEMTRACE_CPU_ID \
  int _MEMTRACE_cpuId_ = 0;                     \
  asm volatile ( "mrc p15, 0, %0, c0, c0, 5 \n" \
                 "and %0, %0, #0xff         \n" \
                 : "=r" (_MEMTRACE_cpuId_) );   \
  ;

#define MEMTRACE_REGION_BASE 0x20000000

#define MEMTRACE_BUF_SIZE  (0x1000)    // one page per cpu mapped currently in bootstrap/platform.cc
#define MEMTRACE_BASE      ((volatile unsigned int**) ((MEMTRACE_REGION_BASE) + (_MEMTRACE_cpuId_ * MEMTRACE_BUF_SIZE)))

#define MEMTRACE_STR4(str) {                                                                     \
  MEMTRACE_CPU_ID;                                                                               \
  **MEMTRACE_BASE = ((str)[0] + ((str)[1]<<8) + ((str)[2]<<16) + ((str)[3]<<24));                \
  (*MEMTRACE_BASE)++;                                                                            \
  if (((unsigned int) (*MEMTRACE_BASE)) >= ((unsigned int) MEMTRACE_BASE + MEMTRACE_BUF_SIZE)) { \
    *MEMTRACE_BASE = (unsigned int*) MEMTRACE_BASE;                                              \
    (*MEMTRACE_BASE)++;                                                                          \
  }                                                                                              \
  **MEMTRACE_BASE=0x1F1F1F1F;                                                                    \
}

#define MEMTRACE_UINT(uintvalue) { \
  MEMTRACE_CPU_ID;                                                                               \
  **MEMTRACE_BASE = ((unsigned int) uintvalue);                                                  \
  (*MEMTRACE_BASE)++;                                                                            \
  if (((unsigned int) (*MEMTRACE_BASE)) >= ((unsigned int) MEMTRACE_BASE + MEMTRACE_BUF_SIZE)) { \
    *MEMTRACE_BASE = (unsigned int*) MEMTRACE_BASE;                                              \
    (*MEMTRACE_BASE)++;                                                                          \
  }                                                                                              \
  **MEMTRACE_BASE=0x1F1F1F1F;                                                                    \
}

#else


// // not empty to silence compiler errors due to unused variables
// // but in consequence expressions should not be used as they will have side effects
// #define MEMTRACE_STR4(str) { (void)(str); }
// #define MEMTRACE_UINT(uintvalue) { (void)(uintvalue); }

#define MEMTRACE_STR4(str)
#define MEMTRACE_UINT(uintvalue)

#endif /* GENODE_RELEASE */

#endif /* _INCLUDE__BASE__MEMTRACE_H_ */
