/*
 * \brief   Pic implementation specific to Rpi3bplus
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

#include <pic.h>
#include <platform.h>


bool Genode::Usb_dwc_otg::_need_trigger_sof(uint32_t host_frame,
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


Genode::Usb_dwc_otg::Usb_dwc_otg() : Mmio(Platform::mmio_to_virt(Board::USB_DWC_OTG_BASE))
{
	write<Guid::Num>(0);
	write<Guid::Num_valid>(false);
	write<Guid::Kick>(false);
}


bool Genode::Usb_dwc_otg::handle_sof()
{
	if (!_is_sof())
		return false;

	static int cnt = 0;

	if (++cnt == 8*20) {
		cnt = 0;
		return false;
	}

	if (!read<Guid::Num_valid>() || read<Guid::Kick>())
		return false;

	if (_need_trigger_sof(read<Host_frame_number::Num>(),
	                      read<Guid::Num>()))
		return false;

	write<Core_irq_status::Sof>(1);

	return true;
}


Genode::Pic::Pic()
	: Mmio(Platform::mmio_to_virt(Board::IRQ_CONTROLLER_BASE)) { mask(); }


bool Genode::Pic::take_request(unsigned &irq)
{
	/* read basic IRQ status mask */
	unsigned const cpu_id = Cpu::executing_id();

	uint32_t p = read<CoreIrqSrc>(cpu_id);

	if (CoreIrqSrc::CntPnIrq::get(p)) {
		irq = 1; // TODO: fix constant - Timer
		return true;
	}
	if (CoreIrqSrc::MBox0::get(p)) {

		// TODO - write only 1 bit later
		write<CoreMboxClr>(1, cpu_id * NR_OF_CPUS + MBOX0_OFFSET);

		irq = IPI; // IPI
		return true;
	}

	return false;
}


void Genode::Pic::mask()
{
	for (unsigned cpu_id = 0; cpu_id < NR_OF_CPUS; cpu_id++)
		write<CoreTIrqCtl>(0, cpu_id);

	for (unsigned cpu_id = 0; cpu_id < NR_OF_CPUS; cpu_id++)
		write<CoreMboxCtl>(0, cpu_id);

	// unmask ipi
	for (unsigned cpu_id = 0; cpu_id < NR_OF_CPUS; cpu_id++)
		unmask(IPI, cpu_id);
}


void Genode::Pic::unmask(unsigned const i, unsigned cpu_id)
{
	if (i < 4) {
		/* timer interrupts */
		write<CoreTIrqCtl>(read<CoreTIrqCtl>(cpu_id) | (1 << i), cpu_id);

	} else if (i < 8) {
		/* mbox interrupts */
		write<CoreMboxCtl>(read<CoreMboxCtl>(cpu_id) | (1 << (i - 4)), cpu_id);
	}


	// if (i < 8)
	// 	write<Irq_enable_basic>(1 << i);
	// else if (i < 32 + 8)
	// 	write<Irq_enable_gpu_1>(1 << (i - 8));
	// else
	// 	write<Irq_enable_gpu_2>(1 << (i - 8 - 32));
}


void Genode::Pic::mask(unsigned const i)
{
	if (i < 4) {
		for (unsigned cpu_id = 0; cpu_id < NR_OF_CPUS; cpu_id++) {
			write<CoreTIrqCtl>(read<CoreTIrqCtl>(cpu_id) & ~(1 << i), cpu_id);
		}
	} else if (i < 8) {
		unsigned const cpu_id = Cpu::executing_id();
		write<CoreMboxCtl>(read<CoreMboxCtl>(cpu_id) & ~(1 << (i - 4)), cpu_id);
	}
	/*else
		if (i < 8)
		write<Irq_disable_basic>(1 << i);
		else if (i < 32 + 8)
		write<Irq_disable_gpu_1>(1 << (i - 8));
		else
		write<Irq_disable_gpu_2>(1 << (i - 8 - 32)); */

}
