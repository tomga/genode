/*
 * \brief  Board driver for core
 * \author Stefan Kalkowski
 * \date   2017-04-27
 */

/*
 * Copyright (C) 2017 Genode Labs GmbH
 *
 * This file is part of the Genode OS framework, which is distributed
 * under the terms of the GNU Affero General Public License version 3.
 */

#ifndef _CORE__SPEC__RPI3BPLUS__BOARD_H_
#define _CORE__SPEC__RPI3BPLUS__BOARD_H_

/* base includes */
#include <drivers/defs/rpi3bplus.h>
#include <drivers/uart/mini.h>

#include <hw/spec/arm/cortex_a15.h>

namespace Board {
	using namespace Rpi3bplus;
	using Cpu_mmio = Hw::Cortex_a15_mmio<IRQ_CONTROLLER_BASE>;
	using Serial   = Genode::Mini_uart;

	enum {
		UART_BASE  = Rpi3bplus::MINIUART_MMIO_BASE,
		UART_CLOCK = Rpi3bplus::MINIUART_CLOCK,
	};

	static constexpr bool SMP = true;
}

#endif /* _CORE__SPEC__RPI3BPLUS__BOARD_H_ */
