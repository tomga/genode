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

/* core includes */
#include <platform.h>
#include <kernel/timer.h>

using namespace Genode;
using namespace Kernel;


Timer_driver::Timer_driver(unsigned cpu_id)
:
  Mmio(Platform::mmio_to_virt(Board::SYSTEM_TIMER_MMIO_BASE)),
  cpu_id(cpu_id)
{
}


void Timer::_start_one_shot(time_t const ticks)
{
	/* enable timer - probably should be moved to Pic */
	Cpu::CntpCtl::access_t cntpctl = Cpu::CntpCtl::read();
	Cpu::CntpCtl::Enable::set(cntpctl, 1);
	Cpu::CntpCtl::Imask::set(cntpctl, 0);
	Cpu::CntpCtl::Istatus::set(cntpctl, 0);
	Cpu::CntpCtl::write(cntpctl);

	Cpu::Cntpct_64bit::access_t cntpct = Cpu::Cntpct_64bit::read();

	Cpu::CntpCval_64bit::access_t cntpCval = cntpct + ticks;
	Cpu::CntpCval_64bit::write(cntpCval);
}


time_t Timer::_ticks_to_us(time_t const ticks) const {
	return ticks / Driver::TICS_PER_US; }


time_t Timer::us_to_ticks(time_t const us) const {
	return us * Driver::TICS_PER_US; }


time_t Timer::_max_value() const {
	return (Driver::TimerLS::access_t)~0; }


time_t Timer::_value()
{

	Cpu::Cntpct_64bit::access_t cntpct = Cpu::Cntpct_64bit::read();

	Cpu::CntpCval_64bit::access_t cntpCval = Cpu::CntpCval_64bit::read();
	time_t retval = cntpCval > cntpct ? cntpCval - cntpct : 0;

	return retval;
}


unsigned Timer::interrupt_id() const { return Board::SYSTEM_TIMER_IRQ; }
