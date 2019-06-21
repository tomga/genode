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
 */
struct Kernel::Timer_driver
{
	enum { TICS_PER_US = Board::SYSTEM_TIMER_CLOCK / 1000 };

	Timer_driver(unsigned cpu_id);

	time_t last_time { 0 };
};

#endif /* _TIMER_DRIVER_H_ */
