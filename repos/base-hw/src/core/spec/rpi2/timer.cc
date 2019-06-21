/*
 * \brief   Timer implementation specific to Rpi
 * \author  Norman Feske
 * \author  Stefan Kalkowski
 * \author  Martin Stein
 * \date    2016-01-07
 */

/*
 * Copyright (C) 2016-2017 Genode Labs GmbH
 *
 * This file is part of the Genode OS framework, which is distributed
 * under the terms of the GNU Affero General Public License version 3.
 */

#include <drivers/timer/util.h>
#include <kernel/timer.h>
#include <platform.h>

using namespace Genode;
using namespace Kernel;


Timer_driver::Timer_driver(unsigned)
{
}


void Timer::_start_one_shot(time_t const ticks)
{
	using Cpu = Hw::Arm_cpu;

	/* enable timer - probably should be moved to Pic */
	Cpu::CntpCtl::access_t cntpctl = Cpu::CntpCtl::read();
	Cpu::CntpCtl::Enable::set(cntpctl, 1);
	Cpu::CntpCtl::Imask::set(cntpctl, 0);
	Cpu::CntpCtl::Istatus::set(cntpctl, 0);
	Cpu::CntpCtl::write(cntpctl);

	Cpu::Cntpct_64bit::access_t cntpct = Cpu::Cntpct_64bit::read();
	_driver.last_time = cntpct;

	Cpu::CntpCval_64bit::access_t cntpCval = cntpct + ticks;
	Cpu::CntpCval_64bit::write(cntpCval);
}


time_t Timer::ticks_to_us(time_t const ticks) const {
	return timer_ticks_to_us(ticks, Driver::TICS_PER_US); }


time_t Timer::us_to_ticks(time_t const us) const {
	return (us / 1000) * Driver::TICS_PER_US; }


time_t Timer::_max_value() const {
	return ~0UL; }


time_t Timer::_duration() const
{
	using Cpu = Hw::Arm_cpu;

	Cpu::Cntpct_64bit::access_t cntpct = Cpu::Cntpct_64bit::read();

	return cntpct - _driver.last_time;
}


unsigned Timer::interrupt_id() const { return Board::SYSTEM_TIMER_IRQ; }
