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

#ifndef GENODE_RELEASE

#define MEMTRACE_BASE      ((volatile unsigned int**) (0x30000000))
#define MEMTRACE_BUF_SIZE  (0x1000)    // one page mapped currently in bootstrap/platform.cc

#define MEMTRACE_STR4(str) {                                                                     \
  **MEMTRACE_BASE = ((str)[0] + ((str)[1]<<8) + ((str)[2]<<16) + ((str)[3]<<24));                \
  (*MEMTRACE_BASE)++;                                                                            \
  if (((unsigned int) (*MEMTRACE_BASE)) >= ((unsigned int) MEMTRACE_BASE + MEMTRACE_BUF_SIZE)) { \
    *MEMTRACE_BASE = (unsigned int*) MEMTRACE_BASE;                                              \
    (*MEMTRACE_BASE)++;                                                                          \
  }                                                                                              \
  **MEMTRACE_BASE=0x1F1F1F1F;                                                                    \
}

#define MEMTRACE_UINT(uintvalue) { \
  **MEMTRACE_BASE = ((unsigned int) uintvalue);                                                  \
  (*MEMTRACE_BASE)++;                                                                            \
  if (((unsigned int) (*MEMTRACE_BASE)) >= ((unsigned int) MEMTRACE_BASE + MEMTRACE_BUF_SIZE)) { \
    *MEMTRACE_BASE = (unsigned int*) MEMTRACE_BASE;                                              \
    (*MEMTRACE_BASE)++;                                                                          \
  }                                                                                              \
  **MEMTRACE_BASE=0x1F1F1F1F;                                                                    \
}

#else

#define MEMTRACE_STR4(str) {}
#define MEMTRACE_UINT(str) {}

#endif /* GENODE_RELEASE */

#endif /* _INCLUDE__BASE__MEMTRACE_H_ */
