/*
 * \brief   Rpi3bplus specific board definitions
 * \author  Tomasz Gajewski
 * \date    2019-01-30
 */

/*
 * Copyright (C) 2017 Genode Labs GmbH
 *
 * This file is part of the Genode OS framework, which is distributed
 * under the terms of the GNU Affero General Public License version 3.
 */

#ifndef _SRC__BOOTSTRAP__SPEC__RPI3BPLUS__BOARD_H_
#define _SRC__BOOTSTRAP__SPEC__RPI3BPLUS__BOARD_H_

#include <drivers/defs/rpi3bplus.h>
#include <drivers/uart/mini.h>

#include <hw/spec/arm/cortex_a15.h>
#include <hw/spec/arm/lpae.h>
#include <spec/arm/cpu.h>
#include <spec/arm/pic.h>

namespace Board {

	using namespace Rpi3bplus;
	using Cpu_mmio = Hw::Cortex_a15_mmio<IRQ_CONTROLLER_BASE>;
	using Serial   = Genode::Mini_uart;

	enum {
		UART_BASE  = Rpi3bplus::MINIUART_MMIO_BASE,
		UART_CLOCK = Rpi3bplus::MINIUART_CLOCK,
	};
}

#endif /* _SRC__BOOTSTRAP__SPEC__RPI3BPLUS__BOARD_H_ */
