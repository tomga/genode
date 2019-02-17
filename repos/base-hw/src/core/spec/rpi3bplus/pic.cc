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

  uint32_t p = 0;
  switch (cpu_id) {
  case 0: p = read<Core0IrqSrc>(); break;
  case 1: p = read<Core1IrqSrc>(); break;
  case 2: p = read<Core2IrqSrc>(); break;
  case 3: p = read<Core3IrqSrc>(); break;
  }

  if (Core0IrqSrc::CntPnIrq::get(p)) {
    irq = 1; // Timer
    return true;
  }
  if (Core0IrqSrc::MBox0::get(p)) {

                  switch (cpu_id) { // TODO - write only 1 bit later
                  case 0: write<Core0Mbox0Clr>(1); break;
                  case 1: write<Core1Mbox0Clr>(1); break;
                  case 2: write<Core2Mbox0Clr>(1); break;
                  case 3: write<Core3Mbox0Clr>(1); break;
                  }

    irq = IPI; // IPI
    return true;
  }

	return false;
}


void Genode::Pic::mask()
{
  write<Core0TIrqCtl>(0);
  write<Core1TIrqCtl>(0);
  write<Core2TIrqCtl>(0);
  write<Core3TIrqCtl>(0);

  write<Core0MboxCtl>(0);
  write<Core1MboxCtl>(0);
  write<Core2MboxCtl>(0);
  write<Core3MboxCtl>(0);

  // unmask ipi
  for (unsigned cpu_id = 0; cpu_id < NR_OF_CPUS; cpu_id++) {
    unmask(IPI, cpu_id);
  }
}


void Genode::Pic::unmask(unsigned const i, unsigned cpu_id)
{
  if (i < 4) {
    /* timer interrupts */
    switch (cpu_id) {
    case 0: write<Core0TIrqCtl>(read<Core0TIrqCtl>() | (1 << i)); break;
    case 1: write<Core1TIrqCtl>(read<Core1TIrqCtl>() | (1 << i)); break;
    case 2: write<Core2TIrqCtl>(read<Core2TIrqCtl>() | (1 << i)); break;
    case 3: write<Core3TIrqCtl>(read<Core3TIrqCtl>() | (1 << i)); break;
    }

  } else if (i < 8) {
    /* mbox interrupts */

    switch (cpu_id) {
    case 0: write<Core0MboxCtl>(read<Core0MboxCtl>() | (1 << (i - 4))); break;
    case 1: write<Core1MboxCtl>(read<Core1MboxCtl>() | (1 << (i - 4))); break;
    case 2: write<Core2MboxCtl>(read<Core2MboxCtl>() | (1 << (i - 4))); break;
    case 3: write<Core3MboxCtl>(read<Core3MboxCtl>() | (1 << (i - 4))); break;
    }


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
    write<Core0TIrqCtl>(read<Core0TIrqCtl>() & ~(1 << i));
    write<Core1TIrqCtl>(read<Core1TIrqCtl>() & ~(1 << i));
    write<Core2TIrqCtl>(read<Core2TIrqCtl>() & ~(1 << i));
    write<Core3TIrqCtl>(read<Core3TIrqCtl>() & ~(1 << i));

  } else if (i < 8) {
    unsigned const cpu_id = Cpu::executing_id();
    switch (cpu_id) {
    case 0: write<Core0MboxCtl>(read<Core0MboxCtl>() & ~(1 << (i - 4))); break;
    case 1: write<Core1MboxCtl>(read<Core1MboxCtl>() & ~(1 << (i - 4))); break;
    case 2: write<Core2MboxCtl>(read<Core2MboxCtl>() & ~(1 << (i - 4))); break;
    case 3: write<Core3MboxCtl>(read<Core3MboxCtl>() & ~(1 << (i - 4))); break;
    }

  }    
  /*else
    if (i < 8)
    write<Irq_disable_basic>(1 << i);
    else if (i < 32 + 8)
    write<Irq_disable_gpu_1>(1 << (i - 8));
    else
    write<Irq_disable_gpu_2>(1 << (i - 8 - 32)); */

}
