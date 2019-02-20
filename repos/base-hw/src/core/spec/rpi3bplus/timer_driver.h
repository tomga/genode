/*
 * \brief  Timer driver for core
 * \author Tomasz Gajewski
 * \date   2019-02-15
 */

/*
 * Copyright (C) 2013-2017 Genode Labs GmbH
 *
 * This file is part of the Kernel OS framework, which is distributed
 * under the terms of the GNU Affero General Public License version 3.
 */

#ifndef _TIMER_DRIVER_H_
#define _TIMER_DRIVER_H_

/* Kernel includes */
#include <util/mmio.h>

/* core includes */
#include <board.h>

namespace Kernel { class Timer_driver; }

/**
 * Timer driver for core
 *
 * Timer channels 0 and 2 are used by GPU, so we use channel 1
 */
struct Kernel::Timer_driver : Genode::Mmio
{
	enum { TICS_PER_US = Board::SYSTEM_TIMER_CLOCK / 1000 / 1000 };

	Timer_driver(unsigned cpu_id);

	unsigned const cpu_id;
};

#endif /* _TIMER_DRIVER_H_ */
