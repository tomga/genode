/*
 * \brief  Board driver for core
 * \author Tomasz Gajewski
 * \date   2019-06-02
 */

/*
 * Copyright (C) 2019 Genode Labs GmbH
 *
 * This file is part of the Genode OS framework, which is distributed
 * under the terms of the GNU Affero General Public License version 3.
 */

#ifndef _CORE__SPEC__RPI3_32__BOARD_H_
#define _CORE__SPEC__RPI3_32__BOARD_H_

#include <hw/spec/arm_64/rpi3_board.h>
#include <hw/spec/arm/cortex_a15.h>

namespace Board {
	using namespace Hw::Rpi3_board;

	using Cpu_mmio = Hw::Cortex_a15_mmio<IRQ_CONTROLLER_BASE>;

	static constexpr bool SMP = true;
}

#endif /* _CORE__SPEC__RPI3_32__BOARD_H_ */
