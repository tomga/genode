/*
 * \brief  Programmable interrupt controller for core
 * \author Tomasz Gajewski
 * \date   2019-02-15
 */

/*
 * Copyright (C) 2013-2017 Genode Labs GmbH
 *
 * This file is part of the Genode OS framework, which is distributed
 * under the terms of the GNU Affero General Public License version 3.
 */

#ifndef _CORE__SPEC__RPI3BPLUS__PIC_H_
#define _CORE__SPEC__RPI3BPLUS__PIC_H_

/* Genode includes */
#include <util/mmio.h>

/* core includes */
#include <board.h>

namespace Genode
{
	/**
	 * Programmable interrupt controller for core
	 */
	class Pic;

	class Usb_dwc_otg;
}


class Genode::Usb_dwc_otg : Mmio
{
	private:

		struct Core_irq_status : Register<0x14, 32>
		{
			struct Sof : Bitfield<3, 1> { };
		};

		struct Guid : Register<0x3c, 32>
		{
			struct Num : Bitfield<0, 14> { };

			/*
			 * The USB driver set 'Num' to a defined value
			 */
			struct Num_valid : Bitfield<31, 1> { };

			/*
			 * Filter is not used, overridden by the USB driver
			 */
			struct Kick : Bitfield<30, 1> { };
		};

		struct Host_frame_number : Register<0x408, 32>
		{
			struct Num : Bitfield<0, 14> { };
		};

		bool _is_sof() const
		{
			return read<Core_irq_status::Sof>();
		}

		static bool _need_trigger_sof(uint32_t host_frame,
		                              uint32_t scheduled_frame);

	public:

		Usb_dwc_otg();

		bool handle_sof();
};


class Genode::Pic : Mmio
{
	public:

		enum {
			NR_OF_IRQ = 96,

			/*
			 * Mailbox 0 used for IPI
			 */
			IPI = 4,
		};

	private:
                // timer related registers

                struct Core0TIrqCtl  : Register<0x40, 32>
                  {
                    struct NCntPsIrq : Bitfield<0, 1> { };
                    struct NCntPnIrq : Bitfield<1, 1> { };
                    struct NCntHpIrq : Bitfield<2, 1> { };
                    struct NCntVIrq  : Bitfield<3, 1> { };
                    struct NCntPsFiq : Bitfield<4, 1> { };
                    struct NCntPnFiq : Bitfield<5, 1> { };
                    struct NCntHpFiq : Bitfield<6, 1> { };
                    struct NCntVFiq  : Bitfield<7, 1> { };
                  };

                struct Core1TIrqCtl  : Register<0x44, 32>
                  {
                    struct NCntPsIrq : Bitfield<0, 1> { };
                    struct NCntPnIrq : Bitfield<1, 1> { };
                    struct NCntHpIrq : Bitfield<2, 1> { };
                    struct NCntVIrq  : Bitfield<3, 1> { };
                    struct NCntPsFiq : Bitfield<4, 1> { };
                    struct NCntPnFiq : Bitfield<5, 1> { };
                    struct NCntHpFiq : Bitfield<6, 1> { };
                    struct NCntVFiq  : Bitfield<7, 1> { };
                  };

                struct Core2TIrqCtl  : Register<0x48, 32>
                  {
                    struct NCntPsIrq : Bitfield<0, 1> { };
                    struct NCntPnIrq : Bitfield<1, 1> { };
                    struct NCntHpIrq : Bitfield<2, 1> { };
                    struct NCntVIrq  : Bitfield<3, 1> { };
                    struct NCntPsFiq : Bitfield<4, 1> { };
                    struct NCntPnFiq : Bitfield<5, 1> { };
                    struct NCntHpFiq : Bitfield<6, 1> { };
                    struct NCntVFiq  : Bitfield<7, 1> { };
                  };

                struct Core3TIrqCtl  : Register<0x4c, 32>
                  {
                    struct NCntPsIrq : Bitfield<0, 1> { };
                    struct NCntPnIrq : Bitfield<1, 1> { };
                    struct NCntHpIrq : Bitfield<2, 1> { };
                    struct NCntVIrq  : Bitfield<3, 1> { };
                    struct NCntPsFiq : Bitfield<4, 1> { };
                    struct NCntPnFiq : Bitfield<5, 1> { };
                    struct NCntHpFiq : Bitfield<6, 1> { };
                    struct NCntVFiq  : Bitfield<7, 1> { };
                  };


                struct Core0IrqSrc  : Register<0x60, 32>
                  {
                    struct CntPsIrq : Bitfield<0, 1> { };
                    struct CntPnIrq : Bitfield<1, 1> { };
                    struct CntHpIrq : Bitfield<2, 1> { };
                    struct CntVIrq  : Bitfield<3, 1> { };
                    struct MBox0    : Bitfield<4, 1> { };
                    struct MBox1    : Bitfield<5, 1> { };
                    struct MBox2    : Bitfield<6, 1> { };
                    struct MBox3    : Bitfield<7, 1> { };
                    struct Gpu      : Bitfield<8, 1> { };
                    struct Pmu      : Bitfield<9, 1> { };
                    struct Axi      : Bitfield<10, 1> { };
                    struct Timer    : Bitfield<11, 1> { };
                  };

                struct Core1IrqSrc  : Register<0x64, 32>
                  {
                    struct CntPsIrq : Bitfield<0, 1> { };
                    struct CntPnIrq : Bitfield<1, 1> { };
                    struct CntHpIrq : Bitfield<2, 1> { };
                    struct CntVIrq  : Bitfield<3, 1> { };
                    struct MBox0    : Bitfield<4, 1> { };
                    struct MBox1    : Bitfield<5, 1> { };
                    struct MBox2    : Bitfield<6, 1> { };
                    struct MBox3    : Bitfield<7, 1> { };
                    struct Gpu      : Bitfield<8, 1> { };
                    struct Pmu      : Bitfield<9, 1> { };
                    struct Axi      : Bitfield<10, 1> { };
                    struct Timer    : Bitfield<11, 1> { };
                  };

                struct Core2IrqSrc  : Register<0x68, 32>
                  {
                    struct CntPsIrq : Bitfield<0, 1> { };
                    struct CntPnIrq : Bitfield<1, 1> { };
                    struct CntHpIrq : Bitfield<2, 1> { };
                    struct CntVIrq  : Bitfield<3, 1> { };
                    struct MBox0    : Bitfield<4, 1> { };
                    struct MBox1    : Bitfield<5, 1> { };
                    struct MBox2    : Bitfield<6, 1> { };
                    struct MBox3    : Bitfield<7, 1> { };
                    struct Gpu      : Bitfield<8, 1> { };
                    struct Pmu      : Bitfield<9, 1> { };
                    struct Axi      : Bitfield<10, 1> { };
                    struct Timer    : Bitfield<11, 1> { };
                  };

                struct Core3IrqSrc  : Register<0x6c, 32>
                  {
                    struct CntPsIrq : Bitfield<0, 1> { };
                    struct CntPnIrq : Bitfield<1, 1> { };
                    struct CntHpIrq : Bitfield<2, 1> { };
                    struct CntVIrq  : Bitfield<3, 1> { };
                    struct MBox0    : Bitfield<4, 1> { };
                    struct MBox1    : Bitfield<5, 1> { };
                    struct MBox2    : Bitfield<6, 1> { };
                    struct MBox3    : Bitfield<7, 1> { };
                    struct Gpu      : Bitfield<8, 1> { };
                    struct Pmu      : Bitfield<9, 1> { };
                    struct Axi      : Bitfield<10, 1> { };
                    struct Timer    : Bitfield<11, 1> { };
                  };


                // mailboxes related registers

                struct Core0MboxCtl  : Register<0x50, 32>
                  {
                    struct Irq0 : Bitfield<0, 1> { };
                    struct Irq1 : Bitfield<1, 1> { };
                    struct Irq2 : Bitfield<2, 1> { };
                    struct Irq3 : Bitfield<3, 1> { };
                    struct Fiq0 : Bitfield<4, 1> { };
                    struct Fiq1 : Bitfield<5, 1> { };
                    struct Fiq2 : Bitfield<6, 1> { };
                    struct Fiq3 : Bitfield<7, 1> { };
                  };


                struct Core1MboxCtl  : Register<0x54, 32>
                  {
                    struct Irq0 : Bitfield<0, 1> { };
                    struct Irq1 : Bitfield<1, 1> { };
                    struct Irq2 : Bitfield<2, 1> { };
                    struct Irq3 : Bitfield<3, 1> { };
                    struct Fiq0 : Bitfield<4, 1> { };
                    struct Fiq1 : Bitfield<5, 1> { };
                    struct Fiq2 : Bitfield<6, 1> { };
                    struct Fiq3 : Bitfield<7, 1> { };
                  };


                struct Core2MboxCtl  : Register<0x58, 32>
                  {
                    struct Irq0 : Bitfield<0, 1> { };
                    struct Irq1 : Bitfield<1, 1> { };
                    struct Irq2 : Bitfield<2, 1> { };
                    struct Irq3 : Bitfield<3, 1> { };
                    struct Fiq0 : Bitfield<4, 1> { };
                    struct Fiq1 : Bitfield<5, 1> { };
                    struct Fiq2 : Bitfield<6, 1> { };
                    struct Fiq3 : Bitfield<7, 1> { };
                  };


                struct Core3MboxCtl  : Register<0x5c, 32>
                  {
                    struct Irq0 : Bitfield<0, 1> { };
                    struct Irq1 : Bitfield<1, 1> { };
                    struct Irq2 : Bitfield<2, 1> { };
                    struct Irq3 : Bitfield<3, 1> { };
                    struct Fiq0 : Bitfield<4, 1> { };
                    struct Fiq1 : Bitfield<5, 1> { };
                    struct Fiq2 : Bitfield<6, 1> { };
                    struct Fiq3 : Bitfield<7, 1> { };
                  };


                struct Core0Mbox0Set  : Register<0x80, 32>
                  {
                    struct Bit0 : Bitfield<0, 1> { };
                  };
                struct Core1Mbox0Set  : Register<0x90, 32>
                  {
                    struct Bit0 : Bitfield<0, 1> { };
                  };
                struct Core2Mbox0Set  : Register<0xa0, 32>
                  {
                    struct Bit0 : Bitfield<0, 1> { };
                  };
                struct Core3Mbox0Set  : Register<0xb0, 32>
                  {
                    struct Bit0 : Bitfield<0, 1> { };
                  };


                struct Core0Mbox0Clr  : Register<0xc0, 32>
                  {
                    struct Bit0 : Bitfield<0, 1> { };
                  };
                struct Core1Mbox0Clr  : Register<0xd0, 32>
                  {
                    struct Bit0 : Bitfield<0, 1> { };
                  };
                struct Core2Mbox0Clr  : Register<0xe0, 32>
                  {
                    struct Bit0 : Bitfield<0, 1> { };
                  };
                struct Core3Mbox0Clr  : Register<0xf0, 32>
                  {
                    struct Bit0 : Bitfield<0, 1> { };
                  };


		Usb_dwc_otg _usb { };

		/**
		 * Return true if specified interrupt is pending
		 */
		static bool _is_pending(unsigned i, uint32_t p1, uint32_t p2)
		{
			return i < 32 ? (p1 & (1 << i)) : (p2 & (1 << (i - 32)));
		}

	public:

		Pic();

		bool take_request(unsigned &irq);
		void finish_request() { }
		void mask();
		void unmask(unsigned const i, unsigned);
		void mask(unsigned const i);

		/**
		 * Raise inter-processor IRQ of the CPU with kernel name 'cpu_id'
		 */
		void send_ipi(unsigned const cpu_id) {
                  switch (cpu_id) { // TODO - write only 1 bit later
                  case 0: write<Core0Mbox0Set>(1); break;
                  case 1: write<Core1Mbox0Set>(1); break;
                  case 2: write<Core2Mbox0Set>(1); break;
                  case 3: write<Core3Mbox0Set>(1); break;
                  }
                }

		/**
		 * Raise inter-processor interrupt on all other cores
		 */
		void send_ipi() { /* NOT USED YET SO NOT IMPLEMENTING NOW */ }

		static constexpr bool fast_interrupts() { return false; }
};

namespace Kernel { using Genode::Pic; }

#endif /* _CORE__SPEC__RPI3BPLUS__PIC_H_ */
