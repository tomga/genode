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

#ifndef _SRC__INCLUDE__HW__SPEC__ARM__RPI3BPLUS_BOARD_H_
#define _SRC__INCLUDE__HW__SPEC__ARM__RPI3BPLUS_BOARD_H_

#include <drivers/defs/rpi3bplus.h>
#include <drivers/uart/bcm2835_mini.h>

#include <hw/spec/arm/boot_info.h>
#include <hw/spec/arm/cortex_a15.h>

namespace Hw::Rpi3bplus_board {
	using namespace Rpi3bplus;
	using Cpu_mmio = Hw::Cortex_a15_mmio<IRQ_CONTROLLER_BASE>;
	using Serial = Genode::Bcm2835_mini_uart;

	enum {
		UART_BASE  = Rpi3bplus::MINIUART_MMIO_BASE,
		UART_CLOCK = Rpi3bplus::MINIUART_CLOCK,
	};
}

#endif /* _SRC__INCLUDE__HW__SPEC__ARM__RPI_BOARD_H_ */
