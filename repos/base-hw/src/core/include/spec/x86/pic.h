/*
 * \brief  Programmable interrupt controller for core
 * \author Reto Buerki
 * \date   2015-02-17
 */

/*
 * Copyright (C) 2015 Genode Labs GmbH
 *
 * This file is part of the Genode OS framework, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

#ifndef _PIC_H_
#define _PIC_H_

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
}


class Genode::Pic : public Mmio
{
	private:

		enum { REMAP_BASE = Board::VECTOR_REMAP_BASE };

		/* Registers */
		struct EOI : Register<0x0b0, 32, true> { };
		struct Svr : Register<0x0f0, 32>
		{
			struct APIC_enable : Bitfield<8, 1> { };
		};

		/*
		 * ISR register, see Intel SDM Vol. 3A, section 10.8.4. Each of the 8
		 * 32-bit ISR values is followed by 12 bytes of padding.
		 */
		struct Isr : Register_array<0x100, 32, 8 * 4, 32> { };

		class Ioapic : public Mmio
		{
			private:

				uint8_t _irt_count;

				enum {
					/* Number of Redirection Table entries */
					IRTE_COUNT = 0x17,

					IRTE_BIT_POL  = 13,
					IRTE_BIT_TRG  = 15,
					IRTE_BIT_MASK = 16,

					/* Register selectors */
					IOAPICVER = 0x01,
					IOREDTBL  = 0x10,
				};

				/**
				 * Create redirection table entry for given IRQ
				 */
				uint64_t _create_irt_entry(unsigned irq)
				{
					uint32_t entry = REMAP_BASE + irq;

					if (irq > Board::ISA_IRQ_END) {
						/* Use level-triggered, high-active mode for non-legacy
						 * IRQs */
						entry |= 1 << IRTE_BIT_POL | 1 << IRTE_BIT_TRG;
					}
					return entry;
				}

				/**
				 * Return whether 'irq' is an edge-triggered interrupt
				 */
				bool _edge_triggered(unsigned const irq)
				{
					return irq <= REMAP_BASE + Board::ISA_IRQ_END ||
					       irq >  REMAP_BASE + IRTE_COUNT;
				}

			public:

				Ioapic() : Mmio(Board::MMIO_IOAPIC_BASE)
				{
					/* Remap all supported IRQs */
					for (unsigned i = 0; i <= IRTE_COUNT; i++) {
						uint64_t val = _create_irt_entry(i);
						write<Ioregsel>(IOREDTBL + 2 * i + 1);
						write<Iowin>(val >> 32);
						write<Ioregsel>(IOREDTBL + 2 * i);
						write<Iowin>(val & 0xffffffff);
					}
				};

				/* Set/unset mask bit of IRTE for given vector */
				void toggle_mask(unsigned const vector, bool const set)
				{
					/*
					 * Only mask existing RTEs and do *not* mask edge-triggered
					 * interrupts to avoid losing them while masked, see Intel
					 * 82093AA I/O Advanced Programmable Interrupt Controller
					 * (IOAPIC) specification, section 3.4.2, "Interrupt Mask"
					 * flag and edge-triggered interrupts or:
					 * http://yarchive.net/comp/linux/edge_triggered_interrupts.html
					 */
					if (_edge_triggered(vector)) { return; }

					write<Ioregsel>(IOREDTBL + (2 * (vector - REMAP_BASE)));

					uint32_t val = read<Iowin>();
					if (set) {
						val |= 1 << IRTE_BIT_MASK;
					} else {
						val &= ~(1 << IRTE_BIT_MASK);
					}
					write<Iowin>(val);
				}

				/* Registers */
				struct Ioregsel : Register<0x00, 32> { };
				struct Iowin    : Register<0x10, 32> { };
		};

		Ioapic _ioapic;

		/**
		 * Determine lowest pending interrupt in ISR register
		 *
		 * \return index of first ISR bit set starting at index one, zero if no
		 *         bit is set.
		 */
		inline unsigned get_lowest_bit(void)
		{
			unsigned bit, vec_base = 0;

			for (unsigned i = 0; i < 8 * 4; i += 4) {
				bit = __builtin_ffs(read<Isr>(i));
				if (bit) {
					return vec_base + bit;
				}
				vec_base += 32;
			}
			return 0;
		}

	public:

		enum {
			/*
			 * FIXME: dummy ipi value on non-SMP platform, should be removed
			 *        when SMP is an aspect of CPUs only compiled where
			 *        necessary
			 */
			IPI       = 255,
			NR_OF_IRQ = 256,
		};

		/**
		 * Constructor
		 */
		Pic();

		bool take_request(unsigned &irq);

		void finish_request();

		void unmask(unsigned const i, unsigned);

		void mask(unsigned const i);

		/*
		 * Dummies
		 */

		void mask() { }
		void init_cpu_local() { }
		bool is_ip_interrupt(unsigned, unsigned) { return false; }
		void trigger_ip_interrupt(unsigned) { }
};

namespace Kernel { class Pic : public Genode::Pic { }; }

#endif /* _PIC_H_ */
