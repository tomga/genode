/*
 * \brief  Implementation of linux/sched.h
 * \author Norman Feske
 * \date   2015-09-09
 */

/*
 * Copyright (C) 2015 Genode Labs GmbH
 *
 * This file is part of the Genode OS framework, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

#include <lx_emul/impl/internal/scheduler.h>


static void unblock_task(unsigned long task)
{
	Lx::Task *t = (Lx::Task *)task;

	t->unblock();
}


signed long schedule_timeout(signed long timeout)
{
	struct timer_list timer;

	setup_timer(&timer, unblock_task, (unsigned long)Lx::scheduler().current());
	mod_timer(&timer, timeout);

	Lx::scheduler().current()->block_and_schedule();

	del_timer(&timer);

	timeout = (timeout - jiffies);

	return timeout < 0 ? 0 : timeout;
}
