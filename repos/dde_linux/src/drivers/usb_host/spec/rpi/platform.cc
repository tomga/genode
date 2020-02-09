/*
 * \brief  USB initialization for Raspberry Pi
 * \author Norman Feske
 * \date   2013-09-11
 */

/*
 * Copyright (C) 2013-2017 Genode Labs GmbH
 *
 * This file is distributed under the terms of the GNU General Public License
 * version 2.
 */

/* Genode includes */
#include <base/attached_io_mem_dataspace.h>
#include <io_mem_session/connection.h>
#include <util/mmio.h>
#include <platform_session/connection.h>

/* emulation */
#include <platform.h>
#include <lx_emul.h>
#include <lx_kit/timer.h>

/* dwc-otg */
#define new new_
#include <dwc_otg_dbg.h>
#undef new

using namespace Genode;

namespace Genode { class Bcm2835_base; }
namespace Genode { template <unsigned long> class Sp804_base; }

unsigned dwc_irq(Genode::Env&);


class Genode::Bcm2835_base : public Mmio
{
protected:
	enum {
		SYSTEM_TIMER_MMIO_BASE = 0x20003000,
		SYSTEM_TIMER_MMIO_SIZE = 0x1000,
		SYSTEM_TIMER_CLOCK     = 1000000,

		TICS_PER_MS = SYSTEM_TIMER_CLOCK / 1000
	};

	struct Cs  : Register<0x0, 32> { struct M1 : Bitfield<1, 1> { }; };
	struct Clo : Register<0x4,  32> { };
	struct Cmp : Register<0x10, 32> { };

	unsigned long& jiffies_to_use;

	unsigned _last_tick;

public:
	Bcm2835_base(addr_t const mmio_base, unsigned long& jiffies_to_use)
		: Mmio(mmio_base), jiffies_to_use(jiffies_to_use) {
		jiffies_to_use = 0;
		_last_tick = read<Clo>();
	}

	unsigned long update_jiffies()
	{
		unsigned current_tick = read<Clo>();

		unsigned delta_ticks = current_tick - _last_tick; // TODO: handle current tick < last tick
		unsigned delta_ms = ticks_to_ms(delta_ticks);
		unsigned long delta_jiffies = msecs_to_jiffies(delta_ms);

		// static int loggedCount = 0;
		// if (loggedCount < 5 || loggedCount % 100 == 0) {
		// 	Genode::log("bcm_update_jiffies(): ", current_tick, " ", jiffies_to_use, " ", delta_ms, " ", delta_jiffies);
		// }
		// loggedCount++;

		/* delta < 1 jiffie */
		if (!delta_jiffies)
			return jiffies_to_use;

		jiffies_to_use += delta_jiffies;

		unsigned remainder_ms = delta_ms % JIFFIES_TICK_MS;
		unsigned remainder_ticks = ticks_to_ms_remainder(delta_ticks) + ms_to_ticks(remainder_ms);
		_last_tick = current_tick - remainder_ticks;

		return jiffies_to_use;
	}

	/**
	 * Translate native timer value to milliseconds
	 */
	static unsigned long ticks_to_ms(unsigned long const tics) {
		return tics / TICS_PER_MS; }

	static unsigned long ms_to_ticks(unsigned long const ms) {
		return ms * TICS_PER_MS; }

	static unsigned long ticks_to_ms_remainder(unsigned long const tics) {
		return tics % TICS_PER_MS; }

};

struct Platform_timer_bcm : Attached_io_mem_dataspace,
                            Bcm2835_base
{
	Platform_timer_bcm(Genode::Env &env, unsigned long& jiffies_to_use) :
		Attached_io_mem_dataspace(env, SYSTEM_TIMER_MMIO_BASE, SYSTEM_TIMER_MMIO_SIZE),
		Bcm2835_base((Genode::addr_t)local_addr<void>(), jiffies_to_use)
	{ }

	static Platform_timer_bcm *_timerPtr;

	static Platform_timer_bcm &instance(Genode::Env &env, unsigned long& jiffies_to_use)
	{
		Genode::log("Platform_timer_bcm::instance");
		static Platform_timer_bcm _timer(env, jiffies_to_use);
		_timerPtr = &_timer;
		return _timer;
	}

	static Platform_timer_bcm &instance()
	{
		return *_timerPtr;
	}
};

Platform_timer_bcm *Platform_timer_bcm::_timerPtr(nullptr);



/**
 * Basic driver for the ARM SP804 timer
 */
template <unsigned long CLK>
class Genode::Sp804_base : public Mmio
{
	enum {
		TICS_PER_MS = CLK / 1000,
	};

	/**
	 * Holds value that shall be loaded to the timer value register
	 */
	struct Load : Register<0x0, 32> { };

	/**
	 * Timer value register
	 */
	struct Value : Register<0x4, 32> { enum { MAX_VALUE = 0xffffffff }; };

	/**
	 * Timer control register
	 */
	struct Control : Register<0x8, 8>
	{
		struct Oneshot  : Bitfield<0,1> { };
		struct Size     : Bitfield<1,1> { };
		struct Pre      : Bitfield<2,2> { };
		struct Int_en   : Bitfield<5,1> { };
		struct Mode     : Bitfield<6,1> { };
		struct Timer_en : Bitfield<7,1> { };
	};

	/**
	 * Clears the timer interrupt
	 */
	struct Int_clr : Register<0xc, 1> { };

	unsigned long& jiffies_to_use;

	unsigned _last_tick      = Value::MAX_VALUE;

public:

	/**
	 * Constructor, clears interrupt output
	 */
	Sp804_base(addr_t const mmio_base, unsigned long& jiffies_to_use)
		: Mmio(mmio_base), jiffies_to_use(jiffies_to_use)
	{
		jiffies_to_use = 0;

		/* clear irq */
		write<Int_clr>(1);

		/* disabble and configure */
		write<typename Control::Timer_en>(0);
		write<Control>(Control::Mode::bits(0)     | /* free running */
									 Control::Int_en::bits(0)   | /* IRQ disabled */
									 Control::Pre::bits(0)      | /* clk divider 1 */
									 Control::Size::bits(1)     | /* 32 bit */
									 Control::Oneshot::bits(0));  /* one-shot off */

		/* start */
		write<Load>(Value::MAX_VALUE);
		write<typename Control::Timer_en>(1);
	}

	unsigned long update_jiffies()
	{
		unsigned current_tick = read<Value>();

		/* timer runs backwards */
		unsigned delta_ticks = _last_tick - current_tick; // TODO: handle current tick > last tick
		unsigned delta_ms = ticks_to_ms(delta_ticks);
		unsigned long delta_jiffies = msecs_to_jiffies(delta_ms);

		// static int loggedCount = 0;
		// if (loggedCount < 5 || loggedCount % 100 == 0) {
		// 	Genode::log("sp8_update_jiffies(): ", current_tick, " ", jiffies_to_use, " ", delta_ms, " ", delta_jiffies);
		// }
		// loggedCount++;

		/* delta < 1 jiffie */
		if (!delta_jiffies)
			return jiffies_to_use;

		jiffies_to_use += delta_jiffies;

		unsigned remainder_ms = delta_ms % JIFFIES_TICK_MS;
		unsigned remainder_ticks = ticks_to_ms_remainder(delta_ticks) + ms_to_ticks(remainder_ms);
		_last_tick = current_tick + remainder_ticks;

		return jiffies_to_use;
	}

	/**
	 * Translate native timer value to milliseconds
	 */
	static unsigned long ticks_to_ms(unsigned long const tics) {
		return tics / TICS_PER_MS; }

	static unsigned long ms_to_ticks(unsigned long const ms) {
		return ms * TICS_PER_MS; }

	static unsigned long ticks_to_ms_remainder(unsigned long const tics) {
		return tics % TICS_PER_MS; }
};


enum { TIMER_MMIO_BASE   = 0x2000b400,
       TIMER_MMIO_SIZE   = 0x100,
       TIMER_CLOCK       = 992*1000 };

struct Platform_timer : Attached_io_mem_dataspace,
                        Sp804_base<TIMER_CLOCK>
{
	Platform_timer(Genode::Env &env, unsigned long& jiffies_to_use) :
		Attached_io_mem_dataspace(env, TIMER_MMIO_BASE, TIMER_MMIO_SIZE),
		Sp804_base((Genode::addr_t)local_addr<void>(), jiffies_to_use)
	{ }

	static Platform_timer *_timerPtr;

	static Platform_timer &instance(Genode::Env &env, unsigned long& jiffies_to_use)
	{
		Genode::log("Platform_timer::instance");
		static Platform_timer _timer(env, jiffies_to_use);
		_timerPtr = &_timer;
		return _timer;
	}

	static Platform_timer &instance()
	{
		return *_timerPtr;
	}
};

Platform_timer *Platform_timer::_timerPtr(nullptr);

static unsigned long jiffies_update_func()
{
	unsigned long jiffies1 = Platform_timer::instance().update_jiffies();
	unsigned long jiffies2 = Platform_timer_bcm::instance().update_jiffies();
	static unsigned long jiffies1_start = jiffies1;
	static unsigned long jiffies2_start = jiffies2;
	// static unsigned long counter = 0;
	// counter++;
	// if (counter <= 5 || counter % 100 == 0) {
	// 	unsigned long jiffies1_from_start = jiffies1 - jiffies1_start;
	// 	unsigned long jiffies2_from_start = jiffies2 - jiffies2_start;
	// 	Genode::log("J ", jiffies1_from_start,
	// 	            " ", jiffies2_from_start,
	// 	            " ", (unsigned long long) jiffies2_from_start * 1000 / jiffies1_from_start);
	// 	// Genode::log("J ", jiffies2_from_start, " ", jiffies2);
	// }
	return jiffies2;
}


/************************************************
 ** Resource info passed to the dwc_otg driver **
 ************************************************/

enum {
	DWC_BASE = 0x20980000,
	DWC_SIZE = 0x20000,
};


/*******************
 ** Init function **
 *******************/

extern "C" void module_dwc_otg_driver_init();
extern bool fiq_enable, fiq_fsm_enable;

unsigned long reference_jiffies;

void platform_hcd_init(Genode::Env &env, Services *services)
{
	/* enable timer */
	Platform_timer::instance(env, reference_jiffies);
	Platform_timer_bcm::instance(env, jiffies);

	/* use costum jiffies update function and the Sp804 timer */
	Lx::register_jiffies_func(jiffies_update_func);

	unsigned irq = dwc_irq(env);
	static resource _dwc_otg_resource[] =
	{
		{ DWC_BASE, DWC_BASE + DWC_SIZE - 1, "dwc_otg", IORESOURCE_MEM },
		{ irq, irq, "dwc_otg-irq" /* name unused */, IORESOURCE_IRQ }
	};

	/* enable USB power */
	Platform::Connection platform(services->env);
	platform.power_state(Platform::Session::POWER_USB_HCD, true);

	/* disable fiq optimization */
	fiq_enable = false;
	fiq_fsm_enable = false;

	module_dwc_otg_driver_init();

	/* setup host-controller platform device */
	platform_device *pdev = (platform_device *)kzalloc(sizeof(platform_device), 0);
	pdev->name            = (char *)"dwc_otg";
	pdev->id              = 0;
	pdev->num_resources   = sizeof(_dwc_otg_resource)/sizeof(resource);
	pdev->resource        = _dwc_otg_resource;

	/* needed for DMA buffer allocation. See 'hcd_buffer_alloc' in 'buffer.c' */
	static u64 dma_mask         = ~(u64)0;
	pdev->dev.dma_mask          = &dma_mask;
	pdev->dev.coherent_dma_mask = ~0;

	platform_device_register(pdev);
}
