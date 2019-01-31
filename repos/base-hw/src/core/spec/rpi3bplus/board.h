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

#ifndef _CORE__SPEC__RPI3BPLUS__BOARD_H_
#define _CORE__SPEC__RPI3BPLUS__BOARD_H_

#include <hw/spec/arm/rpi3bplus_board.h>

namespace Board {
	using namespace Hw::Rpi3bplus_board;

	static constexpr bool SMP = true;
}

#endif /* _CORE__SPEC__RPI3BPLUS__BOARD_H_ */
