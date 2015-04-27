/*
 * \brief  Implementation of IRQ session component
 * \author Norman Feske
 * \author Sebastian Sumpf
 * \date   2009-10-02
 */

/*
 * Copyright (C) 2009-2013 Genode Labs GmbH
 *
 * This file is part of the Genode OS framework, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

/* Genode includes */
#include <base/printf.h>
#include <base/sleep.h>

/* core includes */
#include <irq_root.h>
#include <irq_proxy.h>
#include <platform.h>
#include <platform_pd.h>

/* NOVA includes */
#include <nova/syscalls.h>
#include <nova/util.h>
#include <nova_util.h>

using namespace Genode;

namespace Genode {
	class Irq_proxy_component;
}


/**
 * Global worker (i.e. thread with SC)
 */
class Irq_thread : public Thread_base
{
	private:

		enum { STACK_SIZE = 1024 * sizeof(addr_t) };
		enum { WEIGHT = Cpu_session::DEFAULT_WEIGHT };

		static void _thread_start()
		{
			Thread_base::myself()->entry();
			sleep_forever();
		}

	public:

		Irq_thread(char const *name) : Thread_base(WEIGHT, name, STACK_SIZE)
		{ }

		/**
		 * Create global EC, associate it to SC
		 */
		void start()
		{
			using namespace Nova;
			addr_t pd_sel   = Platform_pd::pd_core_sel();
			addr_t utcb = reinterpret_cast<addr_t>(&_context->utcb);

			/*
			 * Put IP on stack, it will be read from core pager in platform.cc
			 */
			addr_t *sp = reinterpret_cast<addr_t *>(_context->stack_top() - sizeof(addr_t));
			*sp = reinterpret_cast<addr_t>(_thread_start);

			/* create global EC */
			enum { GLOBAL = true };
			uint8_t res = create_ec(_tid.ec_sel, pd_sel, boot_cpu(),
			                        utcb, (mword_t)sp, _tid.exc_pt_sel, GLOBAL);
			if (res != NOVA_OK) {
				PERR("%p - create_ec returned %d", this, res);
				throw Cpu_session::Thread_creation_failed();
			}

			/* remap startup portal from main thread */
			if (map_local((Utcb *)Thread_base::myself()->utcb(),
			              Obj_crd(PT_SEL_STARTUP, 0),
			              Obj_crd(_tid.exc_pt_sel + PT_SEL_STARTUP, 0))) {
				PERR("could not create startup portal");
				throw Cpu_session::Thread_creation_failed();
			}

			/* remap debugging page fault portal for core threads */
			if (map_local((Utcb *)Thread_base::myself()->utcb(),
			              Obj_crd(PT_SEL_PAGE_FAULT, 0),
			              Obj_crd(_tid.exc_pt_sel + PT_SEL_PAGE_FAULT, 0))) {
				PERR("could not create page fault portal");
				throw Cpu_session::Thread_creation_failed();
			}

			/* default: we don't accept any mappings or translations */
			Utcb * utcb_obj = reinterpret_cast<Utcb *>(Thread_base::utcb());
			utcb_obj->crd_rcv = Obj_crd();
			utcb_obj->crd_xlt = Obj_crd();

			/* create SC */
			unsigned sc_sel = cap_map()->insert();
			res = create_sc(sc_sel, pd_sel, _tid.ec_sel, Qpd(Qpd::DEFAULT_QUANTUM, Qpd::DEFAULT_PRIORITY + 1));
			if (res != NOVA_OK) {
				PERR("%p - create_sc returned returned %d", this, res);
				throw Cpu_session::Thread_creation_failed();
			}
		}
};


/**
 * Irq_proxy interface implementation
 */
class Genode::Irq_proxy_component : public Irq_proxy<Irq_thread>
{
	private:

		Genode::addr_t _irq_sel; /* IRQ cap selector */
		Genode::addr_t _dev_mem; /* used when MSI or HPET is used */
		bool           _ready;   /* flag to signal that IRQ SM can be used */

		Genode::addr_t _msi_addr;
		Genode::addr_t _msi_data;

	protected:

		bool _associate()
		{
			/* assign IRQ to CPU && request msi data to be used by driver */
			uint8_t res = Nova::assign_gsi(_irq_sel, _dev_mem, boot_cpu(),
			                               _msi_addr, _msi_data);

			_ready = res == Nova::NOVA_OK;

			/*
			 * Return ever success so that the IRQ proxy thread gets started.
			 * For MSIs or HPET a separate associate() call with a valid
			 * dev_mem address is required.
			 */
			if (_dev_mem && !_ready)
				PERR("setting up MSI 0x%lx failed - error %u", _irq_number, res);

			return _dev_mem ? _ready : true;
		}

		void _wait_for_irq()
		{
			if (!_ready)
				PERR("Error: assign_gsi failed for IRQ %ld", _irq_number);

			if (Nova::NOVA_OK != Nova::sm_ctrl(_irq_sel, Nova::SEMAPHORE_DOWN))
				PERR("Error: blocking for irq %ld failed", _irq_number);
		}

		void _ack_irq() { }

	public:

		Irq_proxy_component(long irq_number)
		:
			/* since we run in APIC mode translate IRQ 0 (PIT) to 2 */
			Irq_proxy(irq_number ? irq_number : 2),
			_irq_sel(cap_map()->insert()),
			_dev_mem(0),
			_ready(false),
			_msi_addr(0),
			_msi_data(0)
		{
			/* map IRQ SM cap from kernel to core at _irq_sel selector */
			using Nova::Obj_crd;

			Obj_crd src(platform_specific()->gsi_base_sel() + _irq_number, 0);
			Obj_crd dst(_irq_sel, 0);
			enum { MAP_FROM_KERNEL_TO_CORE = true };

			int ret = map_local((Nova::Utcb *)Thread_base::myself()->utcb(),
			                    src, dst, MAP_FROM_KERNEL_TO_CORE);
			if (ret) {
				PERR("Could not map IRQ %ld", _irq_number);
				throw Root::Unavailable();
			}

			/* let thread run */
			_start();
		}

		void associate(Genode::addr_t phys_mem)
		{
			void * v = 0;
			if (platform()->region_alloc()->alloc_aligned(4096,
			                                              &v, 12).is_error())
				return;

			Genode::addr_t virt_addr = reinterpret_cast<Genode::addr_t>(v);

			if (!virt_addr)
				return;

			using Nova::Mem_crd;
			using Nova::Rights;

			if (map_local_phys_to_virt(reinterpret_cast<Nova::Utcb *>(Thread_base::myself()->utcb()),
			                           Mem_crd(phys_mem >> 12,  0, Rights(true, false, false)),
			                           Mem_crd(virt_addr >> 12, 0, Rights(true, false, false)))) {
				platform()->region_alloc()->free(v, 4096);
				return;
			}

			/* local attached pci config extended io mem of device */
			_dev_mem    = virt_addr;
			/* try to assign MSI to device */
			_associate();

			/* revert local mapping */
			_dev_mem    = 0;
			unmap_local(Mem_crd(virt_addr >> 12, 0, Rights(true, true, true)));
			platform()->region_alloc()->free(v, 4096);
		}

		bool           ready()       const { return _ready; }
		Genode::addr_t msi_address() const { return _msi_addr; }
		Genode::addr_t msi_value()   const { return _msi_data; }
};


static Nova::Hip * kernel_hip()
{
	/**
	 * Initial value of esp register, saved by the crt0 startup code.
	 * This value contains the address of the hypervisor information page.
	 */
	extern addr_t __initial_sp;
	return reinterpret_cast<Nova::Hip *>(__initial_sp);
}


/***************************
 ** IRQ session component **
 ***************************/


void Irq_session_component::ack_irq()
{
	if (!_proxy) {
		PERR("Expected to find IRQ proxy for IRQ %02x", _irq_number);
		return;
	}

	_proxy->ack_irq();
}


Irq_session_component::Irq_session_component(Range_allocator *irq_alloc,
                                             const char      *args)
{
	typedef Irq_proxy<Irq_thread> Proxy;

	long irq_number = Arg_string::find_arg(args, "irq_number").long_value(-1);
	if (irq_number < 0) {
		PERR("invalid IRQ number requested");
		throw Root::Unavailable();
	}

	long device_phys = Arg_string::find_arg(args, "device_config_phys").long_value(0);
	if (device_phys) {
		if (irq_number >= kernel_hip()->sel_gsi)
			throw Root::Unavailable();

		irq_number = kernel_hip()->sel_gsi - 1 - irq_number;
		/* XXX last GSI number unknown - assume 40 GSIs (depends on IO-APIC) */
		if (irq_number < 40)
			throw Root::Unavailable();
	}

	/* check if IRQ thread was started before */
	_proxy = Proxy::get_irq_proxy<Irq_proxy_component>(irq_number, irq_alloc);
	if (!_proxy) {
		PERR("unavailable IRQ %lx requested", irq_number);
		throw Root::Unavailable();
	}

	if (device_phys)
		_proxy->associate(device_phys);

	_irq_number = irq_number;
}


Irq_session_component::~Irq_session_component()
{
	if (!_proxy)
		return;

	if (_irq_sigh.valid())
		_proxy->remove_sharer(&_irq_sigh);
}


void Irq_session_component::sigh(Genode::Signal_context_capability sigh)
{
	if (!_proxy) {
		PERR("signal handler got not registered - irq thread unavailable");
		return;
	}

	Genode::Signal_context_capability old = _irq_sigh;

	if (old.valid() && !sigh.valid())
		_proxy->remove_sharer(&_irq_sigh);

	_irq_sigh = sigh;

	if (!old.valid() && sigh.valid())
		_proxy->add_sharer(&_irq_sigh);
}


Genode::Irq_session::Info Irq_session_component::info()
{
	if (!_proxy || !_proxy->ready() || !_proxy->msi_address() ||
	    !_proxy->msi_value())
		return { .type = Genode::Irq_session::Info::Type::INVALID };

	return {
		.type    = Genode::Irq_session::Info::Type::MSI,
		.address = _proxy->msi_address(),
		.value   = _proxy->msi_value()
	};
}
