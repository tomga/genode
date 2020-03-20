/*
 * \brief  Board definitions for Raspberry Pi 3
 * \author Stefan Kalkowski
 * \date   2019-05-10
 */

/*
 * Copyright (C) 2019 Genode Labs GmbH
 *
 * This file is part of the Genode OS framework, which is distributed
 * under the terms of the GNU Affero General Public License version 3.
 */

#ifndef _SRC__INCLUDE__HW__SPEC__ARM__RPI3__BOARD_H_
#define _SRC__INCLUDE__HW__SPEC__ARM__RPI3__BOARD_H_

#include <drivers/uart/bcm2835_mini.h>
#include <hw/spec/arm/boot_info.h>

namespace Hw::Rpi3_board {
	using Serial   = Genode::Bcm2835_mini_uart;

	enum {
		RAM_BASE   = 0,
		RAM_SIZE   = 0x20000000,

		UART_BASE  = 0x3f215000,
		UART_SIZE  = 0x1000,
		UART_CLOCK = 250000000,

		IRQ_CONTROLLER_BASE = 0x3f00b200,
		IRQ_CONTROLLER_SIZE = 0x1000,

		LOCAL_IRQ_CONTROLLER_BASE = 0x40000000,
		LOCAL_IRQ_CONTROLLER_SIZE = 0x1000,

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
};

#endif /* _SRC__INCLUDE__HW__SPEC__ARM__RPI3__BOARD_H_ */
