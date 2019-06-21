/*
 * \brief   Rpi2 specific board definitions
 * \author  Tomasz Gajewski
 * \date    2019-01-30
 */

/*
 * Copyright (C) 2019 Genode Labs GmbH
 *
 * This file is part of the Genode OS framework, which is distributed
 * under the terms of the GNU Affero General Public License version 3.
 */

#ifndef _BOOTSTRAP__SPEC__RPI2__BOARD_H_
#define _BOOTSTRAP__SPEC__RPI2__BOARD_H_

#include <hw/spec/arm/rpi2_board.h>
#include <hw/spec/arm/lpae.h>
#include <spec/arm/cpu.h>
#include <spec/arm/pic.h>

namespace Board {
	using namespace Hw::Rpi2_board;
}

#endif /* _BOOTSTRAP__SPEC__RPI2__BOARD_H_ */
