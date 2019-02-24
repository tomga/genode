/*
 * \brief  MMIO and IRQ definitions for the Raspberry Pi
 * \author Tomasz Gajewski
 * \date   2019-01-30
 */

/*
 * Copyright (C) 2013-2017 Genode Labs GmbH
 *
 * This file is part of the Genode OS framework, which is distributed
 * under the terms of the GNU Affero General Public License version 3.
 */

#ifndef _INCLUDE__DRIVERS__DEFS__RPI_H_
#define _INCLUDE__DRIVERS__DEFS__RPI_H_

/* Genode includes */
#include <util/mmio.h>

namespace Rpi3bplus {
	enum {
		RAM_0_BASE = 0x00000000,
		RAM_0_SIZE = 0x10000000, /* XXX ? */

		MMIO_0_BASE = 0x3f000000,
		MMIO_0_SIZE = 0x01000000,

		/*
		 * IRQ numbers   0..7 refer to the basic IRQs.
		 * IRQ numbers  8..39 refer to GPU IRQs  0..31.
		 * IRQ numbers 40..71 refer to GPU IRQs 32..63.
		 */
		GPU_IRQ_BASE = 8,

		//SYSTEM_TIMER_IRQ       = GPU_IRQ_BASE + 1,
		SYSTEM_TIMER_IRQ       = 1,  /* physical normal */
		SYSTEM_TIMER_MMIO_BASE = 0x40000000,
		SYSTEM_TIMER_MMIO_SIZE = 0x1000,
		SYSTEM_TIMER_CLOCK     = 19200000, // value taken from Cpu::Cntfrq

		//MINIUART_IRQ       = 166,
		MINIUART_MMIO_BASE = 0x3f215000,
		MINIUART_MMIO_SIZE = 0x1000,
		MINIUART_CLOCK     = 250000000,

		/* IRQ_CONTROLLER_BASE = 0x3F00b200, */
		IRQ_CONTROLLER_BASE = 0x40000000,
		IRQ_CONTROLLER_SIZE = 0x100,

                // FROM EXYNOS FOR NOW
		IRQ_CONTROLLER_VT_CTRL_BASE = 0x10484000,
		IRQ_CONTROLLER_VT_CTRL_SIZE = 0x1000,
		IRQ_CONTROLLER_VT_CPU_BASE  = 0x10486000,
		IRQ_CONTROLLER_VT_CPU_SIZE  = 0x1000,

                // FROM EXYNOS FOR NOW
		/* virtual interrupts */
		VT_MAINTAINANCE_IRQ = 25,
		VT_TIMER_IRQ        = 27,

		GPIO_CONTROLLER_BASE = 0x3f200000,
		GPIO_CONTROLLER_SIZE = 0x1000,

		USB_DWC_OTG_BASE = 0x3f980000,
		USB_DWC_OTG_SIZE = 0x20000,

		/* timer */
		TIMER_IRQ = 0,

		/* USB host controller */
		DWC_IRQ = 17,

		/* CPU cache */
		CACHE_LINE_SIZE_LOG2 = 6,

                // FROM EXYNOS FOR NOW
		/* hardware name of the primary processor */
		PRIMARY_MPIDR_AFF_0 = 0,

		/* SD card */
		SDHCI_BASE = MMIO_0_BASE + 0x300000,
		SDHCI_SIZE = 0x100,
		SDHCI_IRQ  = 62,
	};

	enum Videocore_cache_policy { NON_COHERENT = 0,
	                              COHERENT     = 1,
	                              L2_ONLY      = 2,
	                              UNCACHED     = 3 };
};

#endif /* _INCLUDE__DRIVERS__DEFS__RPI_H_ */
