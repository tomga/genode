/*
 * \brief   Raspberry PI3B+ specific board definitions
 * \author  Tomasz Gajewski
 * \date    2019-06-02
 */

/*
 * Copyright (C) 2019 Genode Labs GmbH
 *
 * This file is part of the Genode OS framework, which is distributed
 * under the terms of the GNU Affero General Public License version 3.
 */

#ifndef _SRC__INCLUDE__HW__SPEC__ARM__RPI2_BOARD_H_
#define _SRC__INCLUDE__HW__SPEC__ARM__RPI2_BOARD_H_

#include <drivers/uart/pl011.h>

#include <hw/spec/arm/boot_info.h>
#include <hw/spec/arm/cortex_a15.h>

namespace Hw::Rpi2_board {

	enum {
		RAM_BASE = 0x00000000,
		RAM_SIZE = 0x3b400000, /* from rpi3b+.dts */

		UART_BASE  = 0x3f201000,
		UART_SIZE  = 0x1000,
		UART_CLOCK = 3000000,

		LOCAL_IRQ_CONTROLLER_BASE = 0x40000000,
		LOCAL_IRQ_CONTROLLER_SIZE = 0x100,
		IRQ_CONTROLLER_BASE = 0x3f00b200,
		IRQ_CONTROLLER_SIZE = 0x100,

		/* CPU cache */
		CACHE_LINE_SIZE_LOG2 = 6,

		/*
		 * IRQ numbers   0..7 refer to the basic IRQs.
		 * IRQ numbers  8..39 refer to GPU IRQs  0..31.
		 * IRQ numbers 40..71 refer to GPU IRQs 32..63.
		 */
		GPU_IRQ_BASE = 8,

		SYSTEM_TIMER_IRQ       = 1,  /* physical normal */
		SYSTEM_TIMER_MMIO_BASE = 0x40000000,
		SYSTEM_TIMER_MMIO_SIZE = 0x1000,
		SYSTEM_TIMER_CLOCK     = 1000000,

		/* virtual interrupts */
		VT_MAINTAINANCE_IRQ = 25,
		VT_TIMER_IRQ        = 27,

		USB_DWC_OTG_BASE = 0x3f980000,
		USB_DWC_OTG_SIZE = 0x20000,

		/* USB host controller */
		DWC_IRQ = 17,
	};

	using Cpu_mmio = Hw::Cortex_a15_mmio<IRQ_CONTROLLER_BASE>;
	using Serial = Genode::Pl011_uart;

}

#endif /* _SRC__INCLUDE__HW__SPEC__ARM__RPI_BOARD_H_ */
