/*
 * \brief  Implementation of linux/delay.h
 * \author Norman Feske
 * \date   2015-09-09
 */

/*
 * Copyright (C) 2015 Genode Labs GmbH
 *
 * This file is part of the Genode OS framework, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

/* Genode includes */
#include <timer_session/connection.h>

/*
 * XXX  We may consider to use the Lx::Timer instead of opening a dedicated
 *      timer session.
 */
static Timer::Connection _delay_timer;


void udelay(unsigned long usecs) { _delay_timer.usleep(usecs); }


void msleep(unsigned int msecs) { _delay_timer.msleep(msecs); }


void mdelay(unsigned long msecs) { msleep(msecs); }
