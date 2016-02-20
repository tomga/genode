/*
 * \brief   Singlethreaded minimalistic kernel
 * \author  Martin Stein
 * \author  Stefan Kalkowski
 * \date    2013-09-30
 */

/*
 * Copyright (C) 2013 Genode Labs GmbH
 *
 * This file is part of the Genode OS framework, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

#ifndef _KERNEL__KERNEL_H_
#define _KERNEL__KERNEL_H_

#include <pic.h>
#include <kernel/pd.h>

namespace Kernel {
	Pd                      * core_pd();
	Mode_transition_control * mtc();
	Pic                     * pic();
	Native_utcb             * core_main_thread_utcb_phys_addr();
}

#endif /* _KERNEL__KERNEL_H_ */
