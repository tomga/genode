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

#ifndef _SRC__INCLUDE__HW__SPEC__ARM__RPI3_32_BOARD_H_
#define _SRC__INCLUDE__HW__SPEC__ARM__RPI3_32_BOARD_H_

#include <hw/spec/arm_64/rpi3_board.h>
#include <hw/spec/arm/boot_info.h>
#include <hw/spec/arm/cortex_a15.h>

namespace Hw::Rpi3_32_board {
	using namespace Rpi3_board;
	using Cpu_mmio = Hw::Cortex_a15_mmio<IRQ_CONTROLLER_BASE>;
}

#endif /* _SRC__INCLUDE__HW__SPEC__ARM__RPI_BOARD_H_ */
