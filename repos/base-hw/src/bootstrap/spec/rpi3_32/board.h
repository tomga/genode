/*
 * \brief   Rpi3_32 specific board definitions
 * \author  Tomasz Gajewski
 * \date    2019-01-30
 */

/*
 * Copyright (C) 2019 Genode Labs GmbH
 *
 * This file is part of the Genode OS framework, which is distributed
 * under the terms of the GNU Affero General Public License version 3.
 */

#ifndef _BOOTSTRAP__SPEC__RPI3_32__BOARD_H_
#define _BOOTSTRAP__SPEC__RPI3_32__BOARD_H_

#include <hw/spec/arm/rpi3_32_board.h>
#include <hw/spec/arm/lpae.h>
#include <spec/arm/cpu.h>

namespace Board {
	using namespace Hw::Rpi3_32_board;
}

#endif /* _BOOTSTRAP__SPEC__RPI3_32__BOARD_H_ */
