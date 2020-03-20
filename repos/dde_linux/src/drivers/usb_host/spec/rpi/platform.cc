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

namespace Genode { template <unsigned long> class Sp804_base; }

unsigned dwc_irq(Genode::Env&);


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

	unsigned _last_tick = Value::MAX_VALUE;

	public:

		/**
		 * Constructor, clears interrupt output
		 */
		Sp804_base(addr_t const mmio_base) : Mmio(mmio_base)
		{
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
			unsigned tick = read<Value>();

			/* timer runs backwards */
			unsigned delta = ticks_to_ms(_last_tick - tick);

			/* delta < 1 jiffie */
			if (!msecs_to_jiffies(delta))
				return jiffies;

			jiffies   += msecs_to_jiffies(delta);
			_last_tick = tick;
			return jiffies;
		}

		/**
		 * Translate native timer value to milliseconds
		 */
		static unsigned long ticks_to_ms(unsigned long const tics) {
			return tics / TICS_PER_MS; }
};


enum { TIMER_MMIO_BASE   = 0x2000b400,
       TIMER_MMIO_SIZE   = 0x100,
       TIMER_CLOCK       = 992*1000 };

struct Platform_timer : Attached_io_mem_dataspace,
                        Sp804_base<TIMER_CLOCK>
{
	Platform_timer(Genode::Env &env) :
		Attached_io_mem_dataspace(env, TIMER_MMIO_BASE, TIMER_MMIO_SIZE),
		Sp804_base((Genode::addr_t)local_addr<void>())
	{ }

	static Platform_timer *_timerPtr;

	static Platform_timer &instance(Genode::Env &env)
	{
		static Platform_timer _timer(env);
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
	return Platform_timer::instance().update_jiffies();
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

void platform_hcd_init(Genode::Env &env, Services *services)
{
	/* enable timer */
	Platform_timer::instance(env);

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
