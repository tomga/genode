/*
 * \brief   Pic implementation specific to Rpi 1
 * \author  Norman Feske
 * \author  Stefan Kalkowski
 * \date    2016-01-07
 */

/*
 * Copyright (C) 2016-2017 Genode Labs GmbH
 *
 * This file is part of the Genode OS framework, which is distributed
 * under the terms of the GNU Affero General Public License version 3.
 */

#include <board.h>
#include <platform.h>

using namespace Genode;

bool Hw::Bcm2835_pic::Usb_dwc_otg::_need_trigger_sof(uint32_t host_frame,
                                                     uint32_t scheduled_frame)
{
	uint32_t const max_frame = 0x3fff;

	if (host_frame < scheduled_frame) {
		if (scheduled_frame - host_frame < max_frame / 2)
			return false;  /* scheduled frame not reached yet */
		else
			return true;   /* scheduled frame passed, host frame wrapped */
	} else {
		if (host_frame - scheduled_frame < max_frame / 2)
			return true;   /* scheduled frame passed */
		else
			return false;  /* scheduled frame wrapped, not reached */
	}
}


Hw::Bcm2835_pic::Usb_dwc_otg::Usb_dwc_otg()
: Mmio(Platform::mmio_to_virt(Board::USB_DWC_OTG_BASE))
{
	Genode::raw("Usb_dwc_otg: ", (void*) Board::USB_DWC_OTG_BASE);
	Genode::raw("Usb_dwc_otg virt: ", (void*) base());
	write<Guid::Num>(0);
	write<Guid::Num_valid>(false);
	write<Guid::Kick>(false);
}


bool Hw::Bcm2835_pic::Usb_dwc_otg::handle_sof()
{
	static int handle_sof_count = 0;
	static int is_sof_count = 0;
	static int cnt_count = 0;
	static int num_valid_count = 0;
	static int kick_count = 0;
	static int need_trigger_count = 0;
	static int true_count = 0;

	// if (handle_sof_count >= 100000) {
	// 	Genode::raw(is_sof_count, " ", cnt_count, " ", num_valid_count, " ",
	// 	            kick_count, " ", need_trigger_count, " ", true_count, " ",
	// 	            handle_sof_count);
	//   handle_sof_count = 0;
	//   is_sof_count = 0;
	//   cnt_count = 0;
	//   num_valid_count = 0;
	//   kick_count = 0;
	//   need_trigger_count = 0;
	//   true_count = 0;
	// }

	handle_sof_count++;

	if (!_is_sof()) {
		is_sof_count++;
		return false;
	}

	static int cnt = 0;

	if (++cnt == 8*20) {
		cnt_count++;
		cnt = 0;
		return false;
	}

	if (!read<Guid::Num_valid>()) {
		num_valid_count++;
		return false;
	}

	if (read<Guid::Kick>()) {
		kick_count++;
		return false;
	}

	if (_need_trigger_sof(read<Host_frame_number::Num>(),
	                      read<Guid::Num>())) {
		need_trigger_count++;
		return false;
	}

	true_count++;

	write<Core_irq_status::Sof>(1);

	return true;
}

static void* PHYS_IRQ_CONTROLLER_BASE = 0;
static void* VIRT_IRQ_CONTROLLER_BASE = 0;
static bool already_logged = false;

Hw::Bcm2835_pic::Bcm2835_pic(bool ignore_timer, Genode::addr_t irq_ctrl_base)
: Mmio(Platform::mmio_to_virt(irq_ctrl_base ? irq_ctrl_base : (Genode::addr_t) Board::IRQ_CONTROLLER_BASE)),
  _ignore_timer(ignore_timer)
{ mask();
	PHYS_IRQ_CONTROLLER_BASE = (void*) (irq_ctrl_base ? irq_ctrl_base : (Genode::addr_t) Board::IRQ_CONTROLLER_BASE);
	VIRT_IRQ_CONTROLLER_BASE = (void*) Platform::mmio_to_virt((Genode::addr_t) PHYS_IRQ_CONTROLLER_BASE);
 }


static int dwc_sof_count = 0;
static int dwc_irq_count = 0;

bool Hw::Bcm2835_pic::take_request(unsigned &irq)
{
	if (!already_logged) {
		already_logged = true;
		Genode::raw("Bcm2835_pic::PHYS_IRQ_CONTROLLER_BASE ", (void*) (PHYS_IRQ_CONTROLLER_BASE));
		Genode::raw("Bcm2835_pic::VIRT_IRQ_CONTROLLER_BASE ", (void*) (VIRT_IRQ_CONTROLLER_BASE));
	}

	/* read basic IRQ status mask */
	// uint32_t const p = (!_ignore_timer ? read<Irq_pending_basic>() : 0);
	uint32_t const p = read<Irq_pending_basic>();

	/* read GPU IRQ status mask */
	uint32_t const p1 = read<Irq_pending_gpu_1>(),
	               p2 = read<Irq_pending_gpu_2>();

	if (!_ignore_timer && Irq_pending_basic::Timer::get(p)) {
		irq = Irq_pending_basic::Timer::SHIFT;
		return true;
	}

	// static int logLimitter = 0;
	// if (logLimitter <= 10) {
	// 	Genode::raw("Bcm2835_pic::take_request ", (void*) (p), " ", (void*) (p1), " ", (void*) (p2));
	// 	logLimitter++;
	// }

	/* search for lowest set bit in pending masks */
	for (unsigned i = 0; i < NR_OF_IRQ; i++) {
		if (!_is_pending(i, p1, p2))
			continue;

		irq = Board::GPU_IRQ_BASE + i;
		// Genode::raw("Bcm2835_pic::take_request2 ", irq, " ", (int) Board::GPU_IRQ_BASE, " ", i);



		/* handle SOF interrupts locally, filter from the user land */
		if (irq == Board::DWC_IRQ) {
			if (dwc_irq_count >= 100000) {
				Genode::raw("Sof statistics: ", dwc_sof_count, "/", dwc_irq_count);
				dwc_sof_count = 0;
				dwc_irq_count = 0;
			}
			dwc_irq_count++;
			if (_usb.handle_sof()) {
				dwc_sof_count++;
				return false;
			}
		}

		return true;
	}

	return false;
}


void Hw::Bcm2835_pic::mask()
{
	if (!_ignore_timer) write<Irq_disable_basic>(~0);
	write<Irq_disable_gpu_1>(~0);
	write<Irq_disable_gpu_2>(~0);
	// Genode::raw("Bcm2835_pic::mask()");
}


void Hw::Bcm2835_pic::unmask(unsigned const i, unsigned)
{
	// Genode::raw("Bcm2835_pic::unmask ", i);
	if (i < 8) {
		if (!_ignore_timer) write<Irq_enable_basic>(1 << i);
	} else if (i < 32 + 8) {
		write<Irq_enable_gpu_1>(1 << (i - 8));
		// Genode::raw("Bcm2835_pic::unmask1 ", (void*) (1 << (i - 8)));
	} else {
		write<Irq_enable_gpu_2>(1 << (i - 8 - 32));
		// Genode::raw("Bcm2835_pic::unmask2 ", (void*) (1 << (i - 8 - 32)));
	}
}


void Hw::Bcm2835_pic::mask(unsigned const i)
{
	// Genode::raw("Bcm2835_pic::mask ", i);
	if (i < 8) {
		if (_ignore_timer) write<Irq_disable_basic>(1 << i);
	} else if (i < 32 + 8) {
		write<Irq_disable_gpu_1>(1 << (i - 8));
		// Genode::raw("Bcm2835_pic::mask1 ", (void*) (1 << (i - 8)));
	}	else {
		write<Irq_disable_gpu_2>(1 << (i - 8 - 32));
		// Genode::raw("Bcm2835_pic::mask2 ", (void*) (1 << (i - 8 - 32)));
	}
}


void Hw::Bcm2835_pic::irq_mode(unsigned, unsigned, unsigned) { }
