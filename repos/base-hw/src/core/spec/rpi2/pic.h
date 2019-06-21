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

#ifndef _CORE__SPEC__RPI2__PIC_H_
#define _CORE__SPEC__RPI2__PIC_H_

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
	class Pic_bcm2836;
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


class Genode::Pic_bcm2836 : Mmio
{
	public:

		enum {
			NR_OF_IRQ = 64,

			/*
			 * dummy IPI value on non-SMP platform,
			 * only used in interrupt reservation within generic code
			 */
			IPI,
		};

	private:

		/* struct Irq_pending_basic : Register<0x0, 32> */
		/* { */
		/* 	struct Timer : Bitfield<0, 1> { }; */
		/* 	struct Gpu   : Bitfield<8, 2> { }; */
		/* }; */

		struct Irq_pending_gpu_1  : Register<0x04, 32> { };
		struct Irq_pending_gpu_2  : Register<0x08, 32> { };
		struct Irq_enable_gpu_1   : Register<0x10, 32> { };
		struct Irq_enable_gpu_2   : Register<0x14, 32> { };
		/* struct Irq_enable_basic   : Register<0x18, 32> { }; */
		struct Irq_disable_gpu_1  : Register<0x1c, 32> { };
		struct Irq_disable_gpu_2  : Register<0x20, 32> { };
		/* struct Irq_disable_basic  : Register<0x24, 32> { }; */

		Usb_dwc_otg _usb { };

		/**
		 * Return true if specified interrupt is pending
		 */
		static bool _is_pending(unsigned i, uint32_t p1, uint32_t p2)
		{
			return i < 32 ? (p1 & (1 << i)) : (p2 & (1 << (i - 32)));
		}

	public:

		Pic_bcm2836();

		/* void init_cpu_local() { } */
		bool take_request(unsigned &irq);
		/* void finish_request() { } */
		void mask();
		void unmask(unsigned const i, unsigned);
		void mask(unsigned const i);

		/* static constexpr bool fast_interrupts() { return false; } */
};


class Genode::Pic : Mmio
{
	public:

		enum {
			NR_OF_IRQ = 64,

			/*
			 * Mailbox 0 used for IPI
			 */
			IPI = 4,
		};

	private:
                // timer related registers

		struct CoreTIrqCtl  : Register_array<0x40, 32, 4, 32>
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

		struct CoreIrqSrc  : Register_array<0x60, 32, 4, 32>
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

		struct CoreMboxCtl  : Register_array<0x50, 32, 4, 32>
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

		const uint8_t MBOX0_OFFSET = 0;

		struct CoreMboxSet  : Register_array<0x80, 32, 4 * 4, 32>
			{
				struct Bit0 : Bitfield<0, 1> { };
			};

		struct CoreMboxClr  : Register_array<0xc0, 32, 4 * 4, 32>
			{
				struct Bit0 : Bitfield<0, 1> { };
			};


		Pic_bcm2836 _bcm2836 { };

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
			write<CoreMboxSet>(1, cpu_id * NR_OF_CPUS + MBOX0_OFFSET);
		}

		/**
		 * Raise inter-processor interrupt on all other cores
		 */
		void send_ipi() { /* NOT USED YET SO NOT IMPLEMENTING NOW */ }

		static constexpr bool fast_interrupts() { return false; }
};

namespace Kernel { using Genode::Pic; }

#endif /* _CORE__SPEC__RPI2__PIC_H_ */
